/****************************************************************************
**   ncs is the backend's server of nodecast
**   Copyright (C) 2010-2011  Frédéric Logier <frederic@logier.org>
**
**   https://github.com/nodecast/geekast
**
**   This program is free software: you can redistribute it and/or modify
**   it under the terms of the GNU Affero General Public License as
**   published by the Free Software Foundation, either version 3 of the
**   License, or (at your option) any later version.
**
**   This program is distributed in the hope that it will be useful,
**   but WITHOUT ANY WARRANTY; without even the implied warranty of
**   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**   GNU Affero General Public License for more details.
**
**   You should have received a copy of the GNU Affero General Public License
**   along with this program.  If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/


#include "xmpp_client.h"

Xmpp_client *Xmpp_client::_singleton = NULL;

Xmpp_client::Xmpp_client(QObject *parent) : QXmppClient(parent)
{        
    qDebug() << "Xmpp_client::Xmpp_client !!!";

    bool check = connect(this, SIGNAL(messageReceived(const QXmppMessage&)),
           SLOT(messageReceived(const QXmppMessage&)));
       Q_ASSERT(check);
       Q_UNUSED(check);


    check = connect(this, SIGNAL(presenceReceived(const QXmppPresence&)),
                    this, SLOT(presenceReceived(const QXmppPresence&)));



    check = connect(this, SIGNAL(connected()),
                    SLOT(connectedToServer()));


    check = connect(this, SIGNAL(error(QXmppClient::Error)),
                    SLOT(connectedError()));



    //this->logger()->setLoggingType(QXmppLogger::StdoutLogging);

    /*
    this->configuration().setJid("fredix@gmail.com@localhost");
    this->configuration().setPassword("f5Csoe6BNO760iLxpHAz");
    this->configuration().setResource("e218d6f8-1a4e-44fe-8566-f7915aff93bd");

    this->connectToServer(this->configuration());
    */

    //this->connectToServer("ncs@localhost/geekast", "scn");



   /* subscribe.setTo("ncs@localhost");
    subscribe.setType(QXmppPresence::Subscribe);
    this->sendPacket(subscribe);*/
}


Xmpp_client::~Xmpp_client()
{
    this->disconnectFromServer();
}


void Xmpp_client::connection()
{
    this->configuration().setJid(m_jid);
    this->configuration().setPassword(m_password);
    this->configuration().setResource("geekast");

    this->connectToServer(this->configuration());
}


Xmpp_client* Xmpp_client::getInstance() {
    if (NULL == _singleton)
        {
          qDebug() << "creating singleton.";
          _singleton =  new Xmpp_client;
        }
      else
        {
          qDebug() << "singleton already created!";
        }
      return _singleton;
}

void Xmpp_client::kill ()
  {
    if (NULL != _singleton)
      {
        delete _singleton;
        _singleton = NULL;
      }
  }



void Xmpp_client::connectedToServer()
{
    qDebug() << "Xmpp_client::connectedToServer";
    qDebug() << "Connection successfull !";
    m_connected=true;
    emit xmppConnection(true);
}


void Xmpp_client::connectedError()
{
    qDebug() << "Xmpp_client::connectedError";
    qDebug() << "Connection failed !";
    m_connected=false;
    emit xmppConnection(false);
}


void Xmpp_client::messageReceived(const QXmppMessage& message)
{
    qDebug() << "Xmpp_client::messageReceived !!!";

    QString from = message.from();
    QString msg = message.body();

    //sendPacket(QXmppMessage("", from, "Your message: " + msg));

    //qDebug() << "Xmpp_client::messageReceived : OOOOOOOOOKKKKKKKKK : ";
    //qDebug() << "Xmpp_client::messageReceived : FROM : " << from << " MSG : " << msg;


    QVariant json = QxtJSON::parse(msg);


    qDebug() << "json : " << json;

/*
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
*/

    if (json.toMap().contains("status"))
    {
        emit xmppResponse(json.toMap()["status"].toString());
    }
    else if (json.toMap().contains("error"))
    {
        emit xmppResponse("error : " + json.toMap()["error"].toString());
    }


    m_post_response = "";


    /*
    m_xml_response.setContent(msg);
    m_root = m_xml_response.documentElement();


    m_node = m_root.firstChild();
    m_post_response = m_node.toElement().text();

    if (m_node.toElement().tagName() == "uuid")
    {
        qDebug() << "node : " << m_node.toElement().tagName() << " : " << m_post_response;
        m_uuid = m_post_response;
        emit uuidChanged(m_uuid);
        m_post_response = "";
    }

    if (m_node.toElement().tagName() == "status")
    {
        qDebug() << "node : " << m_node.toElement().tagName() << " : " << m_post_response;
        emit xmppResponse(m_post_response);
    }*/



}





void Xmpp_client::presenceReceived(const QXmppPresence& presence)
{
    qDebug() << "Xmpp_client::presenceReceived !!!";

    QString from = presence.from();

    QString message;
    switch(presence.type())
    {
    case QXmppPresence::Subscribe:
        {
            QXmppPresence subscribed;
            subscribed.setTo(from);

            subscribed.setType(QXmppPresence::Subscribed);
            this->sendPacket(subscribed);

            // reciprocal subscription
            QXmppPresence subscribe;
            subscribe.setTo(from);
            subscribe.setType(QXmppPresence::Subscribe);
            this->sendPacket(subscribe);

            return;
        }
        break;
    case QXmppPresence::Subscribed:
        message = "<B>%1</B> accepted your request";
        break;
    case QXmppPresence::Unsubscribe:
        message = "<B>%1</B> unsubscribe";
        break;
    case QXmppPresence::Unsubscribed:
        message = "<B>%1</B> unsubscribed";
        break;
    default:
        return;
        break;
    }

    if(message.isEmpty())
        return;
}
