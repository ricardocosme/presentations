#include <coruja/object/object.hpp>
#include <coruja/object/lift.hpp>
#include <iostream>
#include <string>

// using namespace coruja;
using namespace std;

struct person_t {
    coruja::object<string> first_name, surname;
};

int main()
{
    person_t p;
    // p.first_name.after_change([](string s){cout << s << endl;});
    // p.surname.after_change([](string s){cout << s << endl;});
    // p.first_name = "Jimmy";
    // p.surname = "Page";

    auto print = [](string s){cout << s;};  
    auto fullname = p.first_name + p.surname;
    auto c = fullname.after_change(print);
    
    p.first_name = "jimmy";

    c.disconnect();
}
