#pragma once

#include <coruja/support/signal/signal.hpp>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QMainWindow>
#include <QObject>

class view_t : public QObject {
    Q_OBJECT
public:
    view_t() : height(&window) {
        QObject::connect(&height, SIGNAL(valueChanged(double)),
                         this, SLOT(height_change_slot(double)));
    }
    virtual ~view_t() = default;
    QMainWindow window;
    QDoubleSpinBox height;
    coruja::signal<void(double)> on_height_change;
public Q_SLOTS:
    void height_change_slot(double v)
    { on_height_change(v); }
};

