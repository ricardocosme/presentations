
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <boost/fusion/include/front.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/fusion/include/pop_front.hpp>
#include <boost/fusion/include/vector.hpp>
#include <range/v3/range_for.hpp>
#include <gtk/gtk.h>

#include <functional>
#include <iostream>
#include <string>

const std::string& _name(const coruja::object<std::string>& s)
{ return s.get(); }

const std::string& _name(const std::string& s)
{ return s; }

template<typename It>
inline typename std::enable_if<
    std::is_same<
        ranges::iterator_value_t<It>,
        std::string>::value
    ||
    std::is_same<
      typename std::decay<ranges::iterator_value_t<It>>::type,
        coruja::object<std::string>>::value
    , const std::string&>::type
name(It child)
{ return _name(*child); }

template<typename It>
inline typename std::enable_if<
    !(std::is_same<
        ranges::iterator_value_t<It>,
        std::string>::value
    ||
    std::is_same<
      typename std::decay<ranges::iterator_value_t<It>>::type,
        coruja::object<std::string>>::value)
    , const std::string&>::type
name(It child)
{ return _name((*child).first); }

template<typename Rng, typename Seq>
inline typename std::enable_if<boost::fusion::result_of::empty<Seq>::value, std::string>::type
parent_path(Rng&& rng, Seq seq)
{ return ""; }

template<typename Rng, typename Seq>
typename std::enable_if<!boost::fusion::result_of::empty<Seq>::value, std::string>::type
parent_path(Rng&& rng, Seq seq)
{
    using namespace boost::fusion;
    auto it(ranges::begin(rng));
    auto e = front(seq);
    for(; it != ranges::end(rng); ++it)
        if((*it).first == e) break;
    return std::to_string(ranges::distance(ranges::begin(rng), it))
        + ":" + parent_path((*it).second, pop_front(seq));
}

template<typename It>
typename std::enable_if<
    !(std::is_same<
        ranges::iterator_value_t<It>,
        std::string>::value
    ||
    std::is_same<
        ranges::iterator_value_t<It>,
        coruja::object<std::string>>::value)
    >::type
react_changes(GtkTreeStore* store, GtkTreeIter iter, It it)
{
    (*it).first.after_change
        ([store, iter](const std::string& s) mutable
         { gtk_tree_store_set(store, &iter, 0, s.c_str(), -1); });
}

template<typename It>
typename std::enable_if<
    std::is_same<
        ranges::iterator_value_t<It>,
        coruja::object<std::string>>::value
    >::type
react_changes(GtkTreeStore* store, GtkTreeIter iter, It it)
{
    (*it).after_change
        ([store, iter](const std::string& s) mutable
         { gtk_tree_store_set(store, &iter, 0, s.c_str(), -1); });
}

template<typename It>
typename std::enable_if<
    std::is_same<
        ranges::iterator_value_t<It>,
        std::string>::value
    >::type
react_changes(GtkTreeStore* store, GtkTreeIter iter, It it)
{
}

template<typename Outer, typename It>
inline typename std::enable_if<
    std::is_same<
        typename std::decay<ranges::iterator_value_t<It>>::type,
        std::string>::value
    ||
    std::is_same<
        ranges::iterator_value_t<It>,
        coruja::object<std::string>>::value
    >::type
react_children(Outer& outer, It it, GtkTreeStore*, GtkTreeIter)
{}

template<typename Outer, typename It>
typename std::enable_if<
    !(std::is_same<
      typename std::decay<ranges::iterator_value_t<It>>::type,
        std::string>::value
    ||
    std::is_same<
        ranges::iterator_value_t<It>,
        coruja::object<std::string>>::value)
    >::type
inline react_children(Outer& outer, It it, GtkTreeStore* store, GtkTreeIter parent)
{
    using namespace ranges;
    using Child = typename iterator_value_t<It>::second_type;
    
    (*it).second.for_each
        ([&outer, store, parent](Child& child_rng,
            range_iterator_t<Child> child_it)
         {
             
             auto position = ranges::distance(begin(child_rng), child_it);
             GtkTreeIter iter;
             gtk_tree_store_insert (store, &iter, const_cast<GtkTreeIter*>(&parent), position);
             gtk_tree_store_set (store, &iter, 0, name(child_it).c_str(), -1);
             react_changes(store, iter, child_it);
             react_children(outer, child_it, store, iter);
         });
}

template<typename Rows>
class tree_t {
    Rows _rows;
    coruja::scoped_any_connection c;
public:
    GtkTreeStore *store;
    GtkWidget *tree;
    using rows_t = Rows;
    tree_t() = default;
    
    tree_t(Rows rows, GtkWidget* window)
        : _rows(rows)
    {
        using namespace ranges;
                
        store = gtk_tree_store_new (1, G_TYPE_STRING);
        c = _rows.for_each
            ([this](const Rows& outer, range_iterator_t<Rows> it)
             {
                 auto position = distance(begin(outer), it);
                 GtkTreeIter iter;
                 gtk_tree_store_insert (store, &iter, NULL, position);
                 gtk_tree_store_set (store, &iter, 0, name(it).c_str(), -1);
                 react_children(outer, it, store, iter);
             });
        
        tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
    
        GtkCellRenderer *renderer;
        GtkTreeViewColumn *column;

        renderer = gtk_cell_renderer_text_new ();
        column = gtk_tree_view_column_new_with_attributes ("Name",
                                                           renderer,
                                                           "text", 0,
                                                           NULL);
        gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);
        gtk_container_add (GTK_CONTAINER (window), tree);
        gtk_widget_show(tree);
    }
};

template<typename Rows>
inline tree_t<Rows> make_tree(Rows&& rows, GtkWidget* window)
{ return tree_t<Rows>(std::forward<Rows>(rows), window); }
