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

    ui->progressBar->setValue(0);


    m_push = new Push();
    m_push->m_credentials  = settings->value("login").toString()+":"+settings->value("password").toString();
    m_push->m_server = settings->value("server").toString();
    m_push->m_uuid = settings->value("uuid").toString();

    m_push->m_xmpp_client->m_jid = settings->value("pub_uuid").toString() + "@" + settings->value("server").toString();;
    m_push->m_xmpp_client->m_password = settings->value("uuid").toString();
    m_push->m_xmpp_client->connection();

    ui->push_method->setText("push method : " + m_push->m_push_method.toUpper());


    qDebug() << "activated network : " << settings->value("activated_network").toBool();


    m_getdatas_mutex = new QMutex();
    m_getdatas_mutex->lock();

    m_pushdatas_mutex = new QMutex();
    m_pushdatas_mutex->lock();

    p_datas = new QVariantMap;
    m_datas = new Datas(p_datas, m_getdatas_mutex);


    m_thread_getdatas = new QThread;
    m_datas->moveToThread(m_thread_getdatas);
    connect(m_thread_getdatas, SIGNAL(started()), m_datas, SLOT(Populate()));

    refreshStats();


    if (settings->value("refresh_rate").toInt() < 5) settings->setValue("refresh_rate",5);
    ui->spinBox_refresh->setValue(settings->value("refresh_rate").toInt());


    createActions();
    createTrayIcon();
    trayIcon->show();


    this->connect(m_push, SIGNAL(httpResponse(int)), SLOT(on_push_httpResponse(int)));
    this->connect(m_push->m_xmpp_client, SIGNAL(xmppResponse(QString)), SLOT(on_push_xmppResponse(QString)));
    this->connect(m_push, SIGNAL(uuidChanged(QString)), SLOT(on_push_uuidChanged(QString)));
    this->connect(m_push, SIGNAL(pub_uuidChanged(QString)), SLOT(on_push_pub_uuidChanged(QString)));

    this->connect(m_push->m_xmpp_client, SIGNAL(uuidChanged(QString)), SLOT(on_push_uuidChanged(QString)));
    this->connect(m_push->m_xmpp_client, SIGNAL(xmppConnection(bool)), SLOT(on_xmpp_connected(bool)));

    m_timer = new QTimer(this);
    m_timer->setInterval(settings->value("refresh_rate").toInt()*60*1000);
    //connect(m_timer, SIGNAL(timeout()), this, SLOT(on_pushButton_push_clicked()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(push_data()));

    m_timer->stop();
    //m_timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::refreshStats()
{

    m_datas->activated_hardware = settings->value("activated_hardware").toBool();
    ui->checkBox_hardware->setChecked(m_datas->activated_hardware);
    m_datas->activated_uptime = settings->value("activated_uptime").toBool();
    ui->checkBox_uptime->setChecked(m_datas->activated_uptime);
    m_datas->activated_memory = settings->value("activated_memory").toBool();
    ui->checkBox_memory->setChecked(m_datas->activated_memory);
    m_datas->activated_load = settings->value("activated_load").toBool();
    ui->checkBox_load->setChecked(m_datas->activated_load);
    m_datas->activated_process = settings->value("activated_process").toBool();
    ui->checkBox_process->setChecked(m_datas->activated_process);
    m_datas->activated_network = settings->value("activated_network").toBool();
    ui->checkBox_network->setChecked(m_datas->activated_network);
    m_datas->activated_cpu = settings->value("activated_cpu").toBool();
    ui->checkBox_cpu->setChecked(m_datas->activated_cpu);
    m_datas->activated_fs = settings->value("activated_fs").toBool();
    ui->checkBox_fs->setChecked(m_datas->activated_fs);



    m_datas->public_host = settings->value("public_host").toBool();
    ui->checkBox_public_host->setChecked(m_datas->public_host);


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
    ui->progressBar->setValue(100);
    ui->progressBar->setRange(0,0);
    ui->statusbar->showMessage("");


    //if (!m_datas) m_datas = new Datas(p_datas, m_getdatas_mutex);

    refreshStats();
    ui->statusbar->showMessage("extract datas");

    //m_datas->Populate();
    m_thread_getdatas->start();


    while(!m_getdatas_mutex->tryLock()) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 5000);
    }

    ui->progressBar->setValue(100);
    ui->statusbar->showMessage("sending payload");
    ui->progressBar->setRange(0,0);

    m_push->Payload_http(p_datas, m_pushdatas_mutex);

    while(!m_pushdatas_mutex->tryLock()) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 5000);
    }


    if (m_push->m_uuid != ui->lineEdit_uuid->text())
    {
        ui->lineEdit_uuid->setText(m_push->m_uuid);
        settings->setValue("uuid",m_push->m_uuid);
    }

    //delete p_datas;
    //delete m_datas;
    //m_datas = NULL;
    m_thread_getdatas->quit();
}


void MainWindow::on_push_uuidChanged(QString uuid)
{
    ui->lineEdit_uuid->setText(uuid);
    settings->setValue("uuid",uuid);
}


void MainWindow::on_push_pub_uuidChanged(QString pub_uuid)
{
    settings->setValue("pub_uuid",pub_uuid);
}


void MainWindow::on_push_httpResponse(int http_error)
{
    QString tmp;
    //ui->textEdit_output->append("HTTP error : ");
    //ui->textEdit_output->insertPlainText(tmp.setNum(http_error));

    ui->progressBar->setRange(0,100);
    ui->progressBar->setValue(100);
    ui->statusbar->showMessage("HTTP error : " + tmp.setNum(http_error));
}


void MainWindow::on_push_xmppResponse(QString response)
{
    ui->progressBar->setRange(0,100);
    ui->progressBar->setValue(100);
    ui->statusbar->showMessage("datas " + response);
}



void MainWindow::on_lineEdit_server_editingFinished()
{
    settings->setValue("server",ui->lineEdit_server->text());
    m_push->m_server = ui->lineEdit_server->text();
}

void MainWindow::on_lineEdit_login_editingFinished()
{
     settings->setValue("login",ui->lineEdit_login->text());
     //push.m_credentials = ui->lineEdit_login->text()+":"+settings->value("password").toString();
     m_push->m_credentials = ui->lineEdit_login->text()+":"+ui->lineEdit_password->text();
}

void MainWindow::on_lineEdit_password_editingFinished()
{
     settings->setValue("password",ui->lineEdit_password->text());
     // push.m_credentials = settings->value("login").toString()+":"+ui->lineEdit_password->text();
     m_push->m_credentials = ui->lineEdit_login->text()+":"+ui->lineEdit_password->text();
}

void MainWindow::on_lineEdit_uuid_editingFinished()
{
    settings->setValue("uuid",ui->lineEdit_uuid->text());
    m_push->m_uuid = ui->lineEdit_uuid->text();
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
//    ui->progressBar->setRange(0,100);

    ui->progressBar->setValue(100);
    ui->progressBar->setRange(0,0);
    ui->statusbar->showMessage("");

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 5000);

    //if (!p_datas) p_datas  = new QVariantMap;
    //if (!m_datas) m_datas = new Datas(p_datas, m_getdatas_mutex);

    refreshStats();
    ui->statusbar->showMessage("extract datas");

    m_thread_getdatas->start();

    while(!m_getdatas_mutex->tryLock()) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 5000);
    }


//    ui->progressBar->setValue(100);
    ui->statusbar->showMessage("sending payload");

    m_push->Payload_xmpp(p_datas, m_pushdatas_mutex);

    while(!m_pushdatas_mutex->tryLock()) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 5000);
    }


    //ui->progressBar->setValue(70);

    if (m_push->m_uuid != ui->lineEdit_uuid->text())
    {
        ui->lineEdit_uuid->setText(m_push->m_uuid);
        settings->setValue("uuid",m_push->m_uuid);
    }

    //delete p_datas;
    //delete m_datas;
    //m_datas = NULL;
    //p_datas = NULL;

    m_thread_getdatas->quit();
}

void MainWindow::on_xmpp_connected(bool connected)
{
    if (connected)
    {
        m_push->m_push_method = "xmpp";
        ui->pushButton_xmpp->setEnabled(true);
        ui->statusbar->showMessage("XMPP connected");
    }
    else
    {
        m_push->m_push_method = "http";
        ui->pushButton_xmpp->setDisabled(true);
        ui->statusbar->showMessage("XMPP connection failed");
    }
    ui->push_method->setText("push method : " + m_push->m_push_method.toUpper());
    qDebug() << "PUSH METHOD " << m_push->m_push_method;
}


void MainWindow::push_data()
{

    if (m_push->m_push_method=="http")
    {
        qDebug() << "PUSH METHOD = HTTP";
        on_pushButton_push_clicked();
    }
    else if (m_push->m_push_method=="xmpp")
    {
        qDebug() << "PUSH METHOD = XMPP";
        on_pushButton_xmpp_clicked();
    }

}
