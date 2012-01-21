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
#include "push.h"



Push::Push(QObject* parent) : QObject(parent)
{
    //m_auth->setUser("aaa");
    m_network = new QNetworkAccessManager(this);       
    m_post_response = "";
    m_http_error = 0;
    m_push_method = "http";

    m_request.setRawHeader("content-type", "application/xml");

    this->connect( m_network, SIGNAL(finished(QNetworkReply *)),SLOT(slotRequestFinished(QNetworkReply *)));

/*
    QObject::connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
                            SLOT(slotSetProgress(qint64,qint64)));
*/


    m_xmpp_client = Xmpp_client::getInstance();

    qRegisterMetaType<QXmppLogger::MessageType>("QXmppLogger::MessageType");
}


void Push::Payload_http(QVariantMap *ldatas, QMutex *m_pushdatas_mutex) {

    qDebug() << "payload and pass : " << m_credentials;

    QString json = QxtJSON::stringify(*ldatas);

    //qDebug() << "JSON : " << json;


    m_request.setRawHeader("Authorization", "Basic " + QByteArray((m_credentials).toAscii()).toBase64());


    // qDebug() << QString::fromAscii(QByteArray(m_request.rawHeader( "Authorization" )).fromBase64());

    qDebug() << m_request.rawHeader( "Authorization" ) << m_credentials.toAscii().toBase64();


    if (m_uuid != "")
    {
        // UPDATE
        url.setUrl("http://" + m_server + ":" + m_http_port + "/host/update/" + m_uuid);
        //url.setUrl(m_server + "/host/update/");
        qDebug() << "PAYLOAD UPDATE" << m_uuid << " SERVER : " << m_server << "pass : " << m_credentials;
        m_request.setUrl(url);

        m_reply = m_network->put(m_request, json.toUtf8().toBase64());
    }
    else
    {
        // CREATE
        qDebug() << "PAYLOAD CREATE" << m_uuid << " SERVER : " << m_server;
        //url.setUrl("http://127.0.0.1:3000/hosts.xml");
        url.setUrl("http://" + m_server + ":" + m_http_port + "/host/create");
        m_request.setUrl(url);

        m_reply = m_network->post(m_request, json.toUtf8().toBase64());
    }

    this->connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(replyError(QNetworkReply::NetworkError)));



    m_pushdatas_mutex->unlock();
}




void Push::Payload_xmpp(QVariantMap *ldatas, QMutex *m_pushdatas_mutex) {

    qDebug() << "payload and pass : " << m_credentials;


    if (m_xmpp_client->m_connected)
    {
        QVariantMap payload;
        payload.insert("credentials", m_credentials);
        payload.insert("datas", *ldatas);

        // qDebug() << QString::fromAscii(QByteArray(m_request.rawHeader( "Authorization" )).fromBase64());

        qDebug() << m_request.rawHeader( "Authorization" ) << m_credentials.toAscii().toBase64();


        if (m_uuid != "")
        {
            payload.insert("action", "update");
            payload.insert("uuid", m_uuid);

            QString json = QxtJSON::stringify(payload);


            // UPDATE
            qDebug() << "PAYLOAD UPDATE" << m_uuid << " SERVER : " << m_server << "pass : " << m_credentials;

            m_xmpp_client->sendMessage("ncs@" + m_server + "/cli", json.toUtf8().toBase64());
            //m_xmpp_client.sendPacket(QXmppMessage("", "ncs@localhost/cli", *ldatas));
        }
        else
        {
            payload.insert("action", "create");
            QString json = QxtJSON::stringify(payload);

            // CREATE
            qDebug() << "PAYLOAD CREATE" << m_uuid << " SERVER : " << m_server;
            m_xmpp_client->sendMessage("ncs@" + m_server + "/cli", json.toUtf8().toBase64());
            //m_xmpp_client.sendPacket(QXmppMessage("", "ncs@localhost/cli", *ldatas));
        }
    }
    m_pushdatas_mutex->unlock();
}



Push::~Push() {            
    free(m_network);    
    m_xmpp_client->kill();
}



void Push::slotRequestFinished(QNetworkReply *) {

    m_http_error = m_reply->error();
    qDebug() << "HTTP ERROR : " << m_http_error;

    emit httpResponse(m_http_error);

    m_post_response = m_reply->readAll();


    qDebug() << "HTTP RESPONSE : " << m_post_response;

    QVariant json = QxtJSON::parse(m_post_response);



    if (json.toMap().contains("uuid") && json.toMap().contains("pub_uuid"))
    {
        m_uuid = json.toMap()["uuid"].toString();
        m_pub_uuid = json.toMap()["pub_uuid"].toString();

        m_xmpp_client->kill();
        m_xmpp_client = Xmpp_client::getInstance();
        m_xmpp_client->m_jid=m_pub_uuid + "@" + m_server;
        m_xmpp_client->m_password=m_uuid;
        m_xmpp_client->connection();

        emit uuidChanged(m_uuid);
        emit pub_uuidChanged(m_pub_uuid);
    }
    m_post_response = "";
}




void Push::replyError(QNetworkReply::NetworkError errorCode)
{

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    switch(errorCode)
    {
    case QNetworkReply::TimeoutError:
    {
        qDebug() << "Push::replyError HTTP ERROR TIMEOUT !";
        //m_requestList.remove(QString::fromStdString(m_request));
        reply->deleteLater();
        //finished();
    }
    case QNetworkReply::HostNotFoundError:
    {
        qDebug() << "Push::replyError HOST NOT FOUND";
        //m_requestList.remove(QString::fromStdString(m_request));
        reply->deleteLater();
        //finished();
    }
    case QNetworkReply::ConnectionRefusedError:
    {
        qDebug() << "Push::replyError Connection refused";
        //m_requestList.remove(QString::fromStdString(m_request));
        reply->deleteLater();
        //finished();
    }

    default:
    {
        qDebug() << "Push::replyError HTTP ERROR DEFAULT ! errorcode : " << errorCode;
        break;
    }
    }
}
