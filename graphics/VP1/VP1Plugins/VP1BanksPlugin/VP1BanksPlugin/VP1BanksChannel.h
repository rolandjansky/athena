/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1BanksChannel                  //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: June 2007                             //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1BANKSCHANNEL_H
#define VP1BANKSCHANNEL_H

#include "VP1Base/IVP1ChannelWidget.h"

class QTableWidget;
class VP1BanksSystem;

class VP1BanksChannel : public IVP1ChannelWidget {

  Q_OBJECT

public:
  VP1BanksChannel();
  void init();
  virtual ~VP1BanksChannel(){}

  Type type() const { return NUMBERS; };//Fixme: Need better type for this kind of system. Maybe an OTHER type?
  void create();

protected slots:
  void entriesChanged(const QStringList& entry_key,const QStringList& entry_type,const QStringList& entry_id);

private:
  QTableWidget * tableWidget;
  VP1BanksSystem * bankssystem;
};

#endif
