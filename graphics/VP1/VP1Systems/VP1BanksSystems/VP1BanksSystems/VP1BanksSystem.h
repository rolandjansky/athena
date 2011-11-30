/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1BANKSSYSTEM_H
#define VP1BANKSSYSTEM_H

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class VP1BanksSystem                               //
//                                                                     //
//  Author: Joe Boudreau <boudreau@pitt.edu>                           //
//  VP1 implementation: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                                     //
//  Initial VP1 version: May 2007                                      //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1Base/IVP1System.h"

class VP1BanksSystem : public IVP1System {

  Q_OBJECT

public:
  VP1BanksSystem();
  virtual ~VP1BanksSystem();

  void create( StoreGateSvc* detstore );
  void refresh( StoreGateSvc* storegate );
  void erase();

  QByteArray saveState();
  void restoreFromState(QByteArray);

signals:
  void entriesChanged(const QStringList& entry_key,const QStringList& entry_type,const QStringList& entry_id);

public slots:
  void refreshList();

protected:
  class Imp;
  Imp * d;
};

#endif
