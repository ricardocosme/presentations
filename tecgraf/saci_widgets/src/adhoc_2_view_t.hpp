#pragma once

#include <coruja/support/signal/any_connection.hpp>
#include <coruja/support/signal/signal.hpp>
#include <coruja/support/signal/scoped_blocked_connection.hpp>
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
    coruja::any_connection height_conn;
public Q_SLOTS:
    void height_change_slot(double v) {
        if(height_conn == coruja::any_connection{}) return;
        auto bc = coruja::make_scoped_blocked_connection(height_conn);
        on_height_change(v);
    }
};

