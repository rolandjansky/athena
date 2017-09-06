/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1TcpServer                     //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1TCPSERVER_H
#define VP1TCPSERVER_H

#include <QObject>
#include <QAbstractSocket>
#include "VP1Base/VP1ExternalRequest.h"

class VP1TcpServer : public QObject {

  Q_OBJECT

public:

  /////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////

  VP1TcpServer();
  virtual ~VP1TcpServer();

  bool listen(QString& err, const quint16& port = 4747);//Returns false (and fills the err string) if not successful.
  quint16 port() const;
  bool isListening();
public slots:
  void close();

signals:
  void receivedExternalRequest(VP1ExternalRequest);
  void listenStateChanged(bool);//Might be emitted up to 2 seconds after the state really changed

  /////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////

private:
  class Imp;
  Imp * d;
private slots:
  void acceptConnection();
  void readData();
  void handleError(QAbstractSocket::SocketError);
  void socketDestroyed(QObject*);
  void listenStateMightHaveChanged();
};

#endif
