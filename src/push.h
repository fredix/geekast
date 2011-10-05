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

#ifndef PUSH_H
#define PUSH_H

#include <QString>
//#include <QHttp>
#include <QtNetwork>
#include <QAuthenticator>
#include <QTest>
//#include <qtf_http.h>
#include <QDomDocument>


//using namespace QteiFul;

class Push : public QObject
{
    Q_OBJECT

public:
    Push(QObject *parent = 0);
    ~Push();
    void Payload(QByteArray *ldatas);
    QString m_credentials;
    QString m_server;
    QString m_uuid;

public slots:
    void slotRequestFinished(QNetworkReply *);
    //void slotAuthenticationRequired(const QString &, quint16, QAuthenticator *);

signals:
    void uuidChanged(QString uuid);
    void httpResponse(int http_error);


private:
    QNetworkAccessManager *m_network;
    QByteArray *m_content;
    QNetworkReply *m_reply;
    QUrl url;
    QNetworkRequest m_request;
    QHttpRequestHeader m_header;
    //QHttp m_http;
    //QFile m_file;
    //QtfHttp http;    
    QAuthenticator *m_auth;
    QString m_post_response;
    int m_http_error;
    QDomDocument m_xml_response;
    QDomElement m_root;
    QDomNode m_node;

};
//! [0]


#endif // PUSH_H
