/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1RawDataSystem                    //
//                                                            //
//  Description: System displaying raw data (RDO's, BCM hits, //
//               LUCID hits, ...                              //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: June 2008 (rewritten January 2009)       //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1RAWDATASYSTEM_H
#define VP1RAWDATASYSTEM_H

#include "VP1Base/IVP13DSystemSimple.h"

class VP1RawDataSystem : public IVP13DSystemSimple {

  Q_OBJECT

 public:
  VP1RawDataSystem();
  virtual ~VP1RawDataSystem();
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);
  void systemerase();
  void systemuncreate();
  void userPickedNode(SoNode* pickedNode, SoPath * pickedPath);
  virtual QWidget * buildController();
  QByteArray saveState();
  void restoreFromState(QByteArray);

private:
  class Imp;
  Imp * d;
};

#endif
