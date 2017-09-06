/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1IncomingMessageDialog         //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1INCOMINGMESSAGEDIALOG_H
#define VP1INCOMINGMESSAGEDIALOG_H

#include "VP1Base/VP1ExternalRequest.h"

#include <QDialog>
#include <QQueue>
#include <QList>

class VP1TabManager;
class VP1ChannelManager;

class VP1IncomingMessageDialog : public QDialog {

  Q_OBJECT

public:

  VP1IncomingMessageDialog( const VP1ExternalRequest&,
			    QQueue<VP1ExternalRequest>*,
			    bool *blockallmessages,
			    QList<QString>* messages_blockedsenders,
			    QList<VP1ExternalRequest>* _messages_blockedexactmessages,
			    VP1TabManager*,
			    VP1ChannelManager* ,
			    QWidget *parent = 0 );
  virtual ~VP1IncomingMessageDialog();

  void updatependinginfo();
protected slots:
  void request_gothere();
  void request_allclear();
  void request_allblock();
  void request_senderclear();
  void request_senderblock();
  void request_messageblock();
  void request_messageclear();

private:
  class Imp;
  Imp * d;
};

#endif
