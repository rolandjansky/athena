/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1MCChannel                     //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: June 2007                             //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1MCCHANNEL_H
#define VP1MCCHANNEL_H

#include "VP1Base/IVP1ChannelWidget.h"

class QTreeWidget;
class VP1MCSystem;

class VP1MCChannel : public IVP1ChannelWidget {

  Q_OBJECT

public:
  VP1MCChannel();
  void init();
  virtual ~VP1MCChannel(){}

  Type type() const { return NUMBERS; };//Fixme: Need better type for this kind of system. Maybe an OTHER type?
  void create();


private:
  QTreeWidget  * treeWidget;
  VP1MCSystem  * mcsystem;
};

#endif
