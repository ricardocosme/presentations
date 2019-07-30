#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QtGui/QDoubleSpinBox>

#include <coruja/object/object.hpp>
#include <saci/qt/spinbox/spinbox.hpp>

#include <iostream>
#include <string>

struct person_t {
    coruja::object<double> height;
};

struct view_t {
    QMainWindow window;
private:    
    QDoubleSpinBox _height;
public:    
    view_t(coruja::object<double>& mheight)
        : _height(&window)
        , height(mheight, _height)
        // , height(mheight, child<QDoubleSpinBox>("height"))
    {}
    saci::qt::spinbox height;
};

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    
    person_t person{coruja::object<double>{1.85}};
    
    person.height.for_each([](double v)
    { std::cout << "model is " << v << std::endl; });

    view_t view(person.height);
        
    view.window.show();

    person.height = 15.7f;
    
    return app.exec();
}
