/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1CaloClusterSystem                //
//                                                            //
//  Description: System displaying clusters                   //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: June 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1CALOCLUSTERSYSTEM_H
#define VP1CALOCLUSTERSYSTEM_H

#include "VP1Base/IVP13DSystemSimple.h"

class VP1CaloClusterSystem : public IVP13DSystemSimple {

  Q_OBJECT

 public:
  VP1CaloClusterSystem();
  virtual ~VP1CaloClusterSystem();
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);
  void systemerase();
  void userPickedNode(SoNode* pickedNode, SoPath * pickedPath);
  virtual QWidget * buildController();
  QByteArray saveState();
  void restoreFromState(QByteArray);

private:
  class Imp;
  Imp * d;
};

#endif
