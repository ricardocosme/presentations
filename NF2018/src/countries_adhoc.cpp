#include <coruja/object/object.hpp>
#include <coruja/container/vector.hpp>
#include <coruja/container/transform.hpp>

using namespace coruja;

struct city {
    std::string name;
};

struct country {
    std::string name;
    using cities_t = std::vector<city>;
    
    void push_back_city(const city& c) {
        _cities.push_back(c);
        _city_insert(_cities, std::prev(_cities.end()));
    }

    cities_t::iterator erase(cities_t::iterator it) {
        _city_insert(_cities, it);
        return _cities.erase(it);
    }
    
    const cities_t& cities() const noexcept { return _cities; };

    template<typename F>
    any_connection on_city_insert(F&& f)
    { return _city_insert.connect(std::forward<F>(f)); }

    template<typename F>
    any_connection on_city_erase(F&& f)
    { return _city_erase.connect(std::forward<F>(f)); }
private:
    signal<void(cities_t&,cities_t::iterator)>
    _city_insert, _city_erase;
    cities_t _cities;
};

vector<country> countries;

int main() {
}
