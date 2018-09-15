#include <coruja/support/signal/connection.hpp>
#include <coruja/support/signal/signal.hpp>
#include <array>
#include <iostream>
#include <string>

using namespace coruja;
using namespace std;

class person_t {
    using sig_t = signal<void(const string&)>;
public:
    using connection_t = sig_t::connection_t;
    
    void first_name(string v) {
        _first_name = std::move(v);
        _change_first_name(_first_name);
    }

    const string& first_name() const noexcept
    { return _first_name; }
    
    template<typename F>
    connection_t change_first_name(F&& f)
    { return _change_first_name.connect(std::forward<F>(f)); }

    void surname(string v) {
        _surname = std::move(v);
        _change_surname(_surname);
    }

    const string& surname() const noexcept
    { return _surname; }
    
    template<typename F>
    connection_t change_surname(F&& f)
    { return _change_surname.connect(std::forward<F>(f)); }
    
private:
    sig_t _change_first_name, _change_surname;
    string _first_name, _surname;
};

template<typename F>
struct fullname_impl {
    void operator()(std::string s)
    { f(p.first_name() + p.surname()); }
    
    person_t& p;
    F f;
};

template<typename F>
inline fullname_impl<F> fullname(person_t& p, F&& f)
{ return fullname_impl<F>{p, std::forward<F>(f)}; }

int main()
{
    person_t p;
    // p.change_first_name([](string s){cout << s << endl;});
    // p.change_surname([](string s){cout << s << endl;});
    // p.first_name("Jimmy");
    // p.surname("Page");

    // auto print_fullname =
    //     [&p](std::string){ cout << p.first_name() + p.surname(); };
    
    auto print_fullname = fullname(p, [](std::string s){ cout << s; });

    array<person_t::connection_t, 2> conns {
      p.change_first_name(print_fullname),
      p.change_surname(print_fullname),
    };

    p.first_name("jimmy");
    
    for(auto& c : conns) c.disconnect();
}
