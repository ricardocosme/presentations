#include <coruja/object/object_view.hpp>
#include <coruja/object/object.hpp>
#include <coruja/object/transform.hpp>
#include <iostream>

class widget_t {
    using obool = coruja::object<bool>;
    obool::after_change_connection_t _enabled_conn;
    obool _enabled;
    coruja::scoped_any_connection conn;

    void set_enable(bool b)
    {
        std::cout << "**set widget property**" << std::endl;
        _enabled = b;
    }
    
public:
    
    widget_t() = default;
    
    void enable(bool b)
    {
        conn = coruja::scoped_any_connection{};
        set_enable(b);
    }
    
    //ObservableObject::observed_t is bool
    template<typename ObservableObject>
    void enable(ObservableObject&& o)
    {
        conn = o.after_change([this](bool b){ set_enable(b); });
        set_enable(o.observed());
    }
    
    coruja::object_view<obool> enable() noexcept
    { return _enabled; }
};

int main()
{
    using namespace coruja;
    object<std::string> s;
    widget_t w;
    w.enable(transform
             (s, [](const std::string& s){return !s.empty();}));
    w.enable().after_change([](bool b){std::cout << b << std::endl;});
    s = "abc";
    s = "";
    w.enable(true);
    s = "def";
}
