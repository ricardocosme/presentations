#include <QtGui/QApplication>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QMainWindow>

#include <coruja/object/object.hpp>
#include <coruja/support/signal/signal.hpp>
#include <coruja/support/signal/scoped_connection.hpp>
#include <coruja/support/signal/any_connection.hpp>
#include <iostream>

#include "adhoc_view_t.hpp"

struct person_t {
    double height;
};

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    view_t view;

    person_t person{1.85};
    view.height.setValue(person.height);
    coruja::scoped_any_connection conn =
        view.on_height_change.connect(
        [&](double v){ person.height = v; });
    coruja::scoped_any_connection conn2 =
        view.on_height_change.connect(
        [](double v){ std::cout << v << std::endl; });
    view.window.show();
    return app.exec();
}
