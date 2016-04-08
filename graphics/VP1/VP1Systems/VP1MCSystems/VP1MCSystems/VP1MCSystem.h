/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1MCSYSTEM_H
#define VP1MCSYSTEM_H

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class VP1MCSystem                                  //
//                                                                     //
//  Author: Joe Boudreau <boudreau@pitt.edu>                           //
//  VP1 implementation: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                                     //
//  Initial VP1 version: May 2007                                      //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1Base/IVP1System.h"
class QTreeWidget;
class QListWidgetItem;
class VP1MCSystem : public IVP1System {

  Q_OBJECT

public:
  VP1MCSystem();
  virtual ~VP1MCSystem();

  void create( StoreGateSvc* detstore );
  void refresh( StoreGateSvc* storegate );
  void erase();

  QByteArray saveState();
  void restoreFromState(QByteArray);

  void setTree(QTreeWidget *tw);

  public slots:

  void addParticle();
  void removeParticle();
  void searchParticles();
  void editItem (QListWidgetItem *);

protected:
  class Imp;
  Imp * d;
};

#endif
