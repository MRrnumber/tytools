/*
 * ty, a collection of GUI and command-line tools to manage Teensy devices
 *
 * Distributed under the MIT license (see LICENSE.txt or http://opensource.org/licenses/MIT)
 * Copyright (c) 2015 Niels Martignène <niels.martignene@gmail.com>
 */

#ifndef LOG_WINDOW_HH
#define LOG_WINDOW_HH

#include "ui_log_window.h"

class LogWindow: public QWidget, private Ui::LogWindow {
    Q_OBJECT

public:
    LogWindow(QWidget *parent = nullptr, Qt::WindowFlags f = 0);

public slots:
    void appendError(const QString &msg);
    void appendDebug(const QString &msg);
    void clearAll();

private:
    void keyPressEvent(QKeyEvent *e);

private slots:
    void showLogContextMenu(const QPoint &pos);
};

#endif
