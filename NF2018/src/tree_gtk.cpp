
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <coruja/container/transform.hpp>
#include <coruja/container/vector.hpp>
#include <coruja/object/object.hpp>

#include <boost/fusion/include/front.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/fusion/include/pop_front.hpp>
#include <boost/fusion/include/vector.hpp>
#include <range/v3/range_for.hpp>
#include <gtk/gtk.h>

#include <functional>
#include <iostream>
#include <string>

void print_row(coruja::object<std::string>& s, std::size_t depth)
{
    for(std::size_t i(0); i < depth; ++i)
        std::cout << "| ";
    std::cout << s.get() << std::endl;
}

void print_row(std::string s, std::size_t depth)
{
    for(std::size_t i(0); i < depth; ++i)
        std::cout << "| ";
    std::cout << s << std::endl;
}
    
template<typename Row>
void print_row(Row&& row, std::size_t depth = 0)
{
    print_row(row.first, depth++);
    if(ranges::size(row.second) == 0) return;
    RANGES_FOR(auto&& child, row.second)
    { print_row(child, depth); }
}

std::string _name(const coruja::object<std::string>& s)
{ return s.get(); }

std::string _name(const std::string& s)
{ return s; }

template<typename It>
inline typename std::enable_if<
    std::is_same<
        ranges::iterator_value_t<It>,
        std::string>::value
    ||
    std::is_same<
        ranges::iterator_value_t<It>,
        coruja::object<std::string>&>::value
    , std::string>::type
name(It child)
{ return _name(*child); }

template<typename It>
inline typename std::enable_if<
    !(std::is_same<
        ranges::iterator_value_t<It>,
        std::string>::value
    ||
    std::is_same<
        ranges::iterator_value_t<It>,
        coruja::object<std::string>&>::value)
    , std::string>::type
name(It child)
{ return _name((*child).first); }

template<typename It>
typename std::enable_if<
    !std::is_same<
        ranges::iterator_value_t<It>,
        std::string>::value
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
        std::string>::value
    >::type
react_changes(GtkTreeStore* store, GtkTreeIter iter, It it)
{
}

template<typename Outer, typename It>
inline typename std::enable_if<
    std::is_same<
        ranges::iterator_value_t<It>,
        std::string>::value
    >::type
react_children(Outer&& outer, It it, GtkTreeStore*, GtkTreeIter)
{}

template<typename Outer, typename It>
typename std::enable_if<
    !std::is_same<
        ranges::iterator_value_t<It>,
        std::string>::value
    >::type
react_children(Outer&& outer, It it, GtkTreeStore* store, GtkTreeIter parent)
{
    using namespace ranges;
    using Child = typename iterator_value_t<It>::second_type;
    
    (*it).second.for_each
        ([outer, store, parent](Child& child_rng,
                       range_iterator_t<Child> child_it)
         {
             auto path = gtk_tree_model_get_string_from_iter(GTK_TREE_MODEL(store), const_cast<GtkTreeIter*>(&parent));
             auto position = distance(begin(child_rng), child_it);
             std::cout << "inserted "
                       << name(child_it) << " "
                       << path
                       << position
                       << std::endl;
             GtkTreeIter iter;
             gtk_tree_store_insert (store, &iter, const_cast<GtkTreeIter*>(&parent), position);
             gtk_tree_store_set (store, &iter,
                                 0, name(child_it).c_str(),
                                 -1);
             react_changes(store, iter, child_it);
             react_children(outer, child_it, store, iter);
         });
}

template<typename Rows>
class tree_t {
    Rows _rows;
public:
    tree_t() = default;
    
    tree_t(Rows rows, GtkWidget* window)
        : _rows(std::move(rows))
    {
        using namespace ranges;
                
        GtkTreeStore *store = gtk_tree_store_new (1, G_TYPE_STRING);
        _rows.for_each
            ([store](const Rows& outer, range_iterator_t<Rows> it)
             {
                 auto position = distance(begin(outer), it);
                 std::cout << "inserted "
                           << name(it) << " " 
                           << position
                           << std::endl;
                 GtkTreeIter iter;
                 gtk_tree_store_insert (store, &iter, NULL, position);
                 gtk_tree_store_set (store, &iter,
                                     0, name(it).c_str(),
                                     -1);
                 (*it).first.after_change
                     ([store, iter](const std::string& s) mutable
                      { gtk_tree_store_set(store, &iter, 0, s.c_str(), -1); });
                 react_children(outer, it, store, iter);
             });
        
        GtkWidget *tree;
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

    void render() const
    { RANGES_FOR(auto&& e, _rows){ print_row(e); } }    
};

template<typename Rows>
inline tree_t<typename std::decay<Rows>::type> make_tree(Rows&& rows, GtkWidget* window)
{ return tree_t<typename std::decay<Rows>::type>(std::forward<Rows>(rows), window); }

struct species_t {
    std::string name;
};

struct genus_t {
    coruja::object<std::string> name;
    coruja::vector<species_t> species;
};

struct family_t {
    coruja::object<std::string> name;
    coruja::vector<genus_t> genera;
};

coruja::vector<family_t> families;

int main(int argc, char** argv)
{
    using namespace coruja;
    
    auto rng = 
        transform(families, 
         [](family_t& family)
         { return std::make_pair
                 (std::ref(family.name),
                  transform(family.genera,
                            [](genus_t& genus)
                            { return std::make_pair
                                    (std::ref(genus.name),
                                     transform(genus.species,
                                               [](species_t &species)
                                               { return species.name; })); })); });

    families.push_back(family_t{coruja::object<std::string>{"Strigidae"}});
    families.back().genera.push_back(genus_t{coruja::object<std::string>{"Pseudoscops"}});
    families.back().genera.back().species.push_back(species_t{"Coruja-orelhuda"});
    families.back().genera.push_back(genus_t{coruja::object<std::string>{"Athene"}});
    families.back().genera.back().species.push_back(species_t{"Coruja-buraqueira"});
    
    GtkWidget* window;
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    
    auto tree = make_tree(rng, window);
    
    family_t picidae{coruja::object<std::string>{"Picidae"}};
    picidae.genera.push_back(genus_t{coruja::object<std::string>{"Campephilus"}});
    picidae.genera.back().species.push_back(species_t{"Pica-pau-de-topete-vermelho"});
    families.insert(families.begin(), std::move(picidae));

    families.front().genera.back().species.push_back(species_t{"Pica-pau-imperial"});
    
    families.back().genera.back().name = "something";
    
    tree.render();

    gtk_widget_show(window);
    gtk_main();
}
