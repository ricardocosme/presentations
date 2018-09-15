// /usr/bin/time -f "%U\n%S" g++ -O3 -c csignals_compile.cpp -I $HOME/build/coruja/include -I $HOME/build/boost_1_67_0

#include <coruja/support/signal.hpp>

coruja::signal<void(int)> sig;

int main(){}
