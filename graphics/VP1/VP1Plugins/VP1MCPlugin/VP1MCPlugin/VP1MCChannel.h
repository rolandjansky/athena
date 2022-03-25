/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1MCChannel                     //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: June 2007
//
//  Major updates:
//  - 2022/03, Riccardo Maria BIANCHI <riccardo.maria.bianchi@cern.ch>
//                                                         
/////////////////////////////////////////////////////////////

#ifndef VP1MCCHANNEL_H
#define VP1MCCHANNEL_H

#include "VP1Base/IVP1ChannelWidget.h"

#include <memory>


// fwd declarations
class QTreeWidget;
class VP1MCSystem;


class VP1MCChannel : public IVP1ChannelWidget {

  Q_OBJECT

public:
  VP1MCChannel();
  void init();
  virtual ~VP1MCChannel();

  Type type() const { return NUMBERS; };//Fixme: Need better type for this kind of system. Maybe an OTHER type?
  void create();


private:
  QTreeWidget  * m_treeWidget;
  std::unique_ptr<VP1MCSystem> m_mcsystem;
};

#endif
