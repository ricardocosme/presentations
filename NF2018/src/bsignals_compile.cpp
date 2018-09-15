///usr/bin/time -f "%U\n%S" g++ -O3 -c bsignals_compile.cpp -I $HOME/build/boost_1_67_0

#include <boost/signals2/signal.hpp>

boost::signals2::signal<void(int)> sig;

int main(){}
