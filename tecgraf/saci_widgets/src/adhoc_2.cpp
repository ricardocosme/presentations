#include <QtGui/QApplication>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QMainWindow>

#include <coruja/object/object.hpp>
#include <coruja/support/signal/signal.hpp>
#include <coruja/support/signal/scoped_connection.hpp>
#include <coruja/support/signal/any_connection.hpp>
#include <iostream>

#include "adhoc_2_view_t.hpp"

struct person_t {
    coruja::object<double> height;
};

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    view_t view;

    person_t person{coruja::object<double>{1.85}};
    coruja::scoped_any_connection conn_0 =
      person.height.for_each([&](double v){ view.height.setValue(v); });
    view.height_conn = conn_0.get();
    
    coruja::scoped_any_connection conn_1 =
        view.on_height_change.connect(
        [&](double v){ person.height = v; });
    coruja::scoped_any_connection conn_2 =
        view.on_height_change.connect(
        [](double v){ std::cout << v << std::endl; });
    view.window.show();
    person.height = 2.05;
    return app.exec();
}
