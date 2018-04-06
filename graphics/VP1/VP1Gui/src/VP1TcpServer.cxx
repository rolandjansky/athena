/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1TcpServer                   //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Gui/VP1TcpServer.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QTimer>
#include <QMap>
#include <cassert>

//____________________________________________________________________
class VP1TcpServer::Imp {
public:
  VP1TcpServer * tcpserv;
  quint16 port;
  QTcpServer tcpserver;
  QMap<QTcpSocket *,quint16> sockets2blocksize;
  QTcpSocket * recognisedSocket(QObject*) const;
  int lastemit_listen;
};

//____________________________________________________________________
VP1TcpServer::VP1TcpServer()
  : QObject(), m_d(new Imp)
{
  m_d->lastemit_listen=-1;
  m_d->tcpserv=this;
  m_d->port=0;
  connect(&m_d->tcpserver, SIGNAL(newConnection()),this, SLOT(acceptConnection()));

  //
  QTimer * timer = new QTimer(this);
  timer->start(2000);
  connect(timer, SIGNAL(timeout()),this, SLOT(listenStateMightHaveChanged()));
}

//____________________________________________________________________
VP1TcpServer::~VP1TcpServer()
{
  close();
  delete m_d; m_d=0;
}

//____________________________________________________________________
bool VP1TcpServer::listen(QString& err, const quint16& port) {
  if (isListening())
    close();
  bool success = m_d->tcpserver.listen(QHostAddress::LocalHost,port);
  if (!success) {
    err = m_d->tcpserver.errorString();
    m_d->port=0;
    listenStateMightHaveChanged();
    return false;
  }
  m_d->port=port;
  err.clear();

  listenStateMightHaveChanged();
  return true;
}

//____________________________________________________________________
quint16 VP1TcpServer::port() const
{
  return m_d->port;
}

//____________________________________________________________________
bool VP1TcpServer::isListening()
{
  return m_d->tcpserver.isListening();
}

//____________________________________________________________________
void VP1TcpServer::close()
{
  if (!isListening())
    return;
  disconnect(&m_d->tcpserver, SIGNAL(newConnection()),this, SLOT(acceptConnection()));

  QMapIterator<QTcpSocket *,quint16> it(m_d->sockets2blocksize);
  while (it.hasNext()) {
    it.next();
    QTcpSocket * socket = it.key();
    delete socket;
  }
  m_d->sockets2blocksize.clear();

  m_d->tcpserver.close();
  m_d->port=0;
  listenStateMightHaveChanged();
}

//____________________________________________________________________
void VP1TcpServer::acceptConnection()
{
  if (!m_d->tcpserver.hasPendingConnections())
    return;
  QTcpSocket * socket = m_d->tcpserver.nextPendingConnection();
  if (m_d->sockets2blocksize.contains(socket)) {
    //we are already dealing with this one, but look back here in a
    //short while to se if anything else is queued.
    QTimer::singleShot(100, this, SLOT(acceptConnection()));
    return;
  }
  assert(!m_d->sockets2blocksize.contains(socket));
  m_d->sockets2blocksize.insert(socket,0);
  connect(socket, SIGNAL(readyRead()),this, SLOT(readData()));
  connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleError(QAbstractSocket::SocketError)));
  connect(socket, SIGNAL(disconnected()),socket, SLOT(deleteLater()));
  connect(socket, SIGNAL(destroyed(QObject*)),this,SLOT(socketDestroyed (QObject*)));
  listenStateMightHaveChanged();
}

//____________________________________________________________________
void VP1TcpServer::handleError(QAbstractSocket::SocketError se)
{
  if (se == QTcpSocket::RemoteHostClosedError)
    return;
  QTcpSocket * socket = m_d->recognisedSocket(sender());
  if (!socket)
    return;
  if (socket->state()!=QAbstractSocket::UnconnectedState) {
    socket->disconnectFromHost();
  }
  //   std::cout << "VP1TcpServer::Error during connection:"<<m_d->tcpserver.errorString().toStdString()<<std::endl;
  listenStateMightHaveChanged();
}

//____________________________________________________________________
void VP1TcpServer::readData()
{
  QTcpSocket * socket = m_d->recognisedSocket(sender());
  if (!socket)
    return;
  if (socket->state()!=QAbstractSocket::ConnectedState) {
    //     assert(0&&"This should never happen...");
    return;
  }

  QDataStream in(socket);
  in.setVersion(QDataStream::Qt_4_2);

  //Make sure that we can read enough to determine the data length:
  if (m_d->sockets2blocksize[socket] == 0) {
    if (socket->bytesAvailable() < (int)sizeof(quint16))
      return;
    in >> m_d->sockets2blocksize[socket];
  }

  //Make sure that we can read all of the data:
  if (socket->bytesAvailable() < m_d->sockets2blocksize[socket])
    return;

  QString data;
  in >> data;
  //   qDebug(QString("Got: "+data).toStdString().c_str());

  if (m_d->sockets2blocksize.contains(socket)) {
    socket->disconnectFromHost();
  }

  //Notice that we dont fire any signals unless we positively see something that we know how to handle!
  if (data.startsWith("VP1ExternalRequest")) {
    VP1ExternalRequest request(data);
    if (request.isValid()) {
      //qDebug("Got valid VP1ExternalRequest!");
      receivedExternalRequest(request);
    }
  }
  listenStateMightHaveChanged();
}

//____________________________________________________________________
QTcpSocket * VP1TcpServer::Imp::recognisedSocket(QObject*o) const
{
  QTcpSocket * socket = static_cast<QTcpSocket *>(o);
  if (!socket)
    return 0;
  return sockets2blocksize.contains(socket) ? socket : 0;
}

//____________________________________________________________________
void VP1TcpServer::socketDestroyed ( QObject * obj )
{
  QTcpSocket * socket = static_cast<QTcpSocket *>(obj);
  if (!socket)
    return;
  if (m_d->sockets2blocksize.contains(socket))
    m_d->sockets2blocksize.remove(socket);
  listenStateMightHaveChanged();
}
//____________________________________________________________________
void VP1TcpServer::listenStateMightHaveChanged()
{
  bool l = isListening();
  bool last = m_d->lastemit_listen == 1 ? true : false;
  if (m_d->lastemit_listen==-1||(l!=last)) {
    listenStateChanged(l);
  }
  m_d->lastemit_listen = l ? 1 : 0;
}
