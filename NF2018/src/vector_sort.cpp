#include <coruja/container/vector.hpp>

#include <algorithm>

int main()
{
    coruja::vector<int> v{3, 1, 2, 0};
    std::sort(v.begin(), v.end());
}

