/****************************************************************************
** Copyright (C) 2010-2011 Frédéric Logier
** Contact: Frédéric Logier <frederic@logier.org>
**
** https://github.com/nodecast/geekast
**
** This file is part of Geekast.
**
** Geekast is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Geekast is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Geekast.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QSystemTrayIcon>
#include <QMainWindow>
#include "core.h"
#include "push.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QVariantMap *p_datas;

protected:
    void changeEvent(QEvent *e);
    Datas *datas;
    Push push;


private:
    Ui::MainWindow *ui;
    void createActions();
    void createTrayIcon();
    void refreshStats();

    QAction *hideAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QSettings *settings;
    bool sendData;
    QTimer *m_timer;



private slots:
    void on_checkBox_fs_clicked(bool checked);
    void on_checkBox_cpu_clicked(bool checked);
    void on_checkBox_public_host_clicked(bool checked);
    void on_spinBox_refresh_valueChanged(int );
    void on_checkBox_network_clicked(bool checked);
    void on_checkBox_process_clicked(bool checked);
    void on_checkBox_load_clicked(bool checked);
    void on_checkBox_memory_clicked(bool checked);
    void on_checkBox_uptime_clicked(bool checked);
    void on_checkBox_hardware_clicked(bool checked);
    void on_pushButton_send_clicked();
    void on_lineEdit_uuid_editingFinished();
    void on_lineEdit_password_editingFinished();
    void on_lineEdit_login_editingFinished();
    void on_lineEdit_server_editingFinished();
    void on_pushButton_push_clicked();
    void on_pushButton_infos_clicked();
    void on_push_uuidChanged(QString uuid);
    void on_push_httpResponse(int http_error);
    void on_tray_show_hide(QSystemTrayIcon::ActivationReason reason);
    void on_pushButton_xmpp_clicked();
};

#endif // MAINWINDOW_H
