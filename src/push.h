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
#include <QtNetwork>
#include <QAuthenticator>
#include <QTest>
#include <QxtJSON>
#include "xmpp_client.h"


class Push : public QObject
{
    Q_OBJECT

public:
    Push(QObject *parent = 0);
    ~Push();
    void Payload_http(QVariantMap *ldatas, QMutex *a_mutex);
    void Payload_xmpp(QVariantMap *ldatas, QMutex *a_mutex);
    QString m_credentials;
    QString m_server;
    QString m_port;
    QString m_uuid;
    QString m_pub_uuid;
    QString m_jid;
    QString m_push_method;
    Xmpp_client *m_xmpp_client;
    QNetworkAccessManager *m_network;


public slots:
    void slotRequestFinished(QNetworkReply *);
    //void slotAuthenticationRequired(const QString &, quint16, QAuthenticator *);

signals:
    void uuidChanged(QString uuid);
    void pub_uuidChanged(QString pub_uuid);
    void httpResponse(int http_error);


private:
    QByteArray *m_content;
    QNetworkReply *m_reply;
    QUrl url;
    QNetworkRequest m_request;
    QHttpRequestHeader m_header;
    QAuthenticator *m_auth;
    QString m_post_response;
    int m_http_error;
};
//! [0]


#endif // PUSH_H
