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

Xmpp_client::Xmpp_client(QObject *parent) : QXmppClient(parent)
{        
    qDebug() << "Xmpp_client::Xmpp_client !!!";


    bool check = connect(this, SIGNAL(messageReceived(const QXmppMessage&)),
           SLOT(messageReceived(const QXmppMessage&)));
       Q_ASSERT(check);
       Q_UNUSED(check);


    check = connect(this, SIGNAL(presenceReceived(const QXmppPresence&)),
                    this, SLOT(presenceReceived(const QXmppPresence&)));

//    this->logger()->setLoggingType(QXmppLogger::StdoutLogging);

    this->configuration().setJid("ncs@localhost");
    this->configuration().setPassword("scn");
    this->configuration().setResource("geekast");

    this->connectToServer(this->configuration());

    //this->connectToServer("ncs@localhost/geekast", "scn");



   /* subscribe.setTo("ncs@localhost");
    subscribe.setType(QXmppPresence::Subscribe);
    this->sendPacket(subscribe);*/
}


Xmpp_client::~Xmpp_client()
{}



void Xmpp_client::messageReceived(const QXmppMessage& message)
{
    qDebug() << "Xmpp_client::messageReceived !!!";

    QString from = message.from();
    QString msg = message.body();

    //sendPacket(QXmppMessage("", from, "Your message: " + msg));

    //qDebug() << "Xmpp_client::messageReceived : OOOOOOOOOKKKKKKKKK : ";
    qDebug() << "Xmpp_client::messageReceived : FROM : " << from << " MSG : " << msg;
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
