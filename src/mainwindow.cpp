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


#include <QtGui>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    sendData = false;
    settings = new QSettings("Geekast", "geekast");

    ui->setupUi(this);

    ui->lineEdit_server->setText(settings->value("server").toString());
    ui->lineEdit_login->setText(settings->value("login").toString());
    ui->lineEdit_password->setText(settings->value("password").toString());
    ui->lineEdit_uuid->setText(settings->value("uuid").toString());


    push.m_credentials  = settings->value("login").toString()+":"+settings->value("password").toString();
    push.m_server = settings->value("server").toString();
    push.m_uuid = settings->value("uuid").toString();



    qDebug() << "activated network : " << settings->value("activated_network").toBool();


    datas = new Datas();
    refreshStats();


    if (settings->value("refresh_rate").toInt() < 5) settings->setValue("refresh_rate",5);
    ui->spinBox_refresh->setValue(settings->value("refresh_rate").toInt());


    createActions();
    createTrayIcon();
    trayIcon->show();


    this->connect(&push, SIGNAL(httpResponse(int)), SLOT(on_push_httpResponse(int)));
    this->connect(&push, SIGNAL(uuidChanged(QString)), SLOT(on_push_uuidChanged(QString)));
    this->connect(&push.m_xmpp_client, SIGNAL(uuidChanged(QString)), SLOT(on_push_uuidChanged(QString)));

    m_timer = new QTimer(this);
    m_timer->setInterval(settings->value("refresh_rate").toInt()*60*1000);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(on_pushButton_push_clicked()));
    m_timer->stop();
    //m_timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::refreshStats()
{

    datas->activated_hardware = settings->value("activated_hardware").toBool();
    ui->checkBox_hardware->setChecked(datas->activated_hardware);
    datas->activated_uptime = settings->value("activated_uptime").toBool();
    ui->checkBox_uptime->setChecked(datas->activated_uptime);
    datas->activated_memory = settings->value("activated_memory").toBool();
    ui->checkBox_memory->setChecked(datas->activated_memory);
    datas->activated_load = settings->value("activated_load").toBool();
    ui->checkBox_load->setChecked(datas->activated_load);
    datas->activated_process = settings->value("activated_process").toBool();
    ui->checkBox_process->setChecked(datas->activated_process);
    datas->activated_network = settings->value("activated_network").toBool();
    ui->checkBox_network->setChecked(datas->activated_network);
    datas->activated_cpu = settings->value("activated_cpu").toBool();
    ui->checkBox_cpu->setChecked(datas->activated_cpu);
    datas->activated_fs = settings->value("activated_fs").toBool();
    ui->checkBox_fs->setChecked(datas->activated_fs);



    datas->public_host = settings->value("public_host").toBool();
    ui->checkBox_public_host->setChecked(datas->public_host);


}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}



void MainWindow::createActions()
{
    hideAction = new QAction(tr("H&ide"), this);
    connect(hideAction, SIGNAL(triggered()), this, SLOT(hide()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}


void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);

    trayIconMenu->addAction(hideAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);

    QIcon icon (":/images/stats.jpeg");

    trayIcon->setIcon(icon);
    setWindowIcon(icon);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(on_tray_show_hide(QSystemTrayIcon::ActivationReason)));
}



void MainWindow::on_tray_show_hide( QSystemTrayIcon::ActivationReason reason )
{
    if( reason )
    {
        if( reason != QSystemTrayIcon::Trigger )
        return;
    }

    if( isVisible() )
    {
        hide();
    }
    else
    {
        show();
        raise();
        setFocus();
    }
}


void MainWindow::on_pushButton_infos_clicked()
{
  //ui->textEdit_output->append(system.infos());

   //ui->listWidget_output->addItems(system.infos());
   //system.infos();

}

void MainWindow::on_pushButton_push_clicked()
{
    p_datas = new QVariantMap;

    if (!datas) datas = new Datas();


    refreshStats();

    datas->Populate(p_datas);
    push.Payload_http(p_datas);

    if (push.m_uuid != ui->lineEdit_uuid->text())
    {
        ui->lineEdit_uuid->setText(push.m_uuid);
        settings->setValue("uuid",push.m_uuid);
    }

    delete p_datas;
    delete datas;
    datas = NULL;
}


void MainWindow::on_push_uuidChanged(QString uuid)
{
    ui->lineEdit_uuid->setText(uuid);
    settings->setValue("uuid",uuid);
}


void MainWindow::on_push_httpResponse(int http_error)
{
    QString tmp;
    //ui->textEdit_output->append("HTTP error : ");
    //ui->textEdit_output->insertPlainText(tmp.setNum(http_error));

    ui->statusbar->showMessage("HTTP error : " + tmp.setNum(http_error));

}


void MainWindow::on_lineEdit_server_editingFinished()
{
    settings->setValue("server",ui->lineEdit_server->text());
    push.m_server = ui->lineEdit_server->text();
}

void MainWindow::on_lineEdit_login_editingFinished()
{
     settings->setValue("login",ui->lineEdit_login->text());
     //push.m_credentials = ui->lineEdit_login->text()+":"+settings->value("password").toString();
     push.m_credentials = ui->lineEdit_login->text()+":"+ui->lineEdit_password->text();
}

void MainWindow::on_lineEdit_password_editingFinished()
{
     settings->setValue("password",ui->lineEdit_password->text());
     // push.m_credentials = settings->value("login").toString()+":"+ui->lineEdit_password->text();
     push.m_credentials = ui->lineEdit_login->text()+":"+ui->lineEdit_password->text();
}

void MainWindow::on_lineEdit_uuid_editingFinished()
{
    settings->setValue("uuid",ui->lineEdit_uuid->text());
    push.m_uuid = ui->lineEdit_uuid->text();
}

void MainWindow::on_pushButton_send_clicked()
{
    // ui->pushButton_send->isChecked() ? sendData = false : sendData = true;
    if (!ui->pushButton_send->isChecked())
    {
         m_timer->stop();
         qDebug() << "timer stop";
         ui->pushButton_send->setText("Auto push is OFF");
    }
    else
    {
         m_timer->start();
         qDebug() << "timer start";
         ui->pushButton_send->setText("Auto push is ON");
    }

}

void MainWindow::on_checkBox_hardware_clicked(bool checked)
{
      settings->setValue("activated_hardware",checked);
      //datas->activated_hardware = settings->value("activated_hardware").toBool();
}

void MainWindow::on_checkBox_uptime_clicked(bool checked)
{
      settings->setValue("activated_uptime",checked);
      //datas->activated_uptime = settings->value("activated_uptime").toBool();
}

void MainWindow::on_checkBox_memory_clicked(bool checked)
{
    settings->setValue("activated_memory",checked);
    //datas->activated_memory = settings->value("activated_memory").toBool();
}

void MainWindow::on_checkBox_load_clicked(bool checked)
{
    settings->setValue("activated_load",checked);
    //datas->activated_load = settings->value("activated_load").toBool();
}

void MainWindow::on_checkBox_process_clicked(bool checked)
{
    settings->setValue("activated_process",checked);
    //datas->activated_process = settings->value("activated_process").toBool();
}

void MainWindow::on_checkBox_network_clicked(bool checked)
{
    settings->setValue("activated_network",checked);
    //datas->activated_network = settings->value("activated_network").toBool();
    qDebug() << "activated network : " << settings->value("activated_network").toBool();

}


void MainWindow::on_spinBox_refresh_valueChanged(int rate)
{
    settings->setValue("refresh_rate",rate);
    m_timer->setInterval(settings->value("refresh_rate").toInt()*60*1000);
}


void MainWindow::on_checkBox_public_host_clicked(bool checked)
{
     settings->setValue("public_host",checked);
     //datas->public_host = settings->value("public_host").toBool();
}

void MainWindow::on_checkBox_cpu_clicked(bool checked)
{
    settings->setValue("activated_cpu",checked);
    //datas->activated_cpu = settings->value("activated_cpu").toBool();
}

void MainWindow::on_checkBox_fs_clicked(bool checked)
{
    settings->setValue("activated_fs",checked);
    //datas->activated_fs = settings->value("activated_fs").toBool();
}


void MainWindow::on_pushButton_xmpp_clicked()
{
    p_datas = new QVariantMap;

    if (!datas) datas = new Datas();

    refreshStats();

    datas->Populate(p_datas);
    push.Payload_xmpp(p_datas);

    if (push.m_uuid != ui->lineEdit_uuid->text())
    {
        ui->lineEdit_uuid->setText(push.m_uuid);
        settings->setValue("uuid",push.m_uuid);
    }

    delete p_datas;
    delete datas;
    datas = NULL;
}
