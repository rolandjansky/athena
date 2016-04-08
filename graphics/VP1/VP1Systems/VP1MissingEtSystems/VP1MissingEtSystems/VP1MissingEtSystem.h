/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1MissingEtSystem                  //
//                                                            //
//  Description: VP1 system displaying missing Et.            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2007, rewritten July 2008            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1MISSINGETSYSTEM_H
#define VP1MISSINGETSYSTEM_H

#include "VP1Base/IVP13DSystemSimple.h"

class VP1MissingEtSystem : public IVP13DSystemSimple {

  Q_OBJECT

public:
  VP1MissingEtSystem();
  virtual ~VP1MissingEtSystem();
  void systemerase();
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);
  void userPickedNode(SoNode* pickedNode, SoPath * pickedPath);
  QWidget * buildController();
  QByteArray saveState();
  void restoreFromState(QByteArray ba);

private:
  class Imp;
  Imp * d;

};

#endif
