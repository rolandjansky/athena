/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1AODSystem                        //
//                                                            //
//  Description: System displaying all AOD objects            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1AODSYSTEM_H
#define VP1AODSYSTEM_H

#include "VP1Base/IVP13DSystemSimple.h"

#include "GaudiKernel/SystemOfUnits.h"

#include <QtCore/QList>

#include <vector>
#include <map>

class SoMaterial;
class SoCooperativeSelection;
class AODHandleBase;

class VP1AODSystem : public IVP13DSystemSimple {

  Q_OBJECT

public:

  VP1AODSystem(QString name="Analysis");
  virtual ~VP1AODSystem();

  void systemcreate(StoreGateSvc* detstore);
  void systemuncreate();
  QWidget * buildController();
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);
  void systemerase();
  QByteArray saveState();
  void restoreFromState(QByteArray);

	/// Reimplementing methods declared in IVP13DSystem
  void userPickedNode(SoNode* pickedNode, SoPath *pickedPath);
  void userSelectedSingleNode(SoCooperativeSelection*, SoNode* , SoPath*);//SINGLE
  void userDeselectedSingleNode(SoCooperativeSelection*, SoNode* , SoPath*);//SINGLE
  void userChangedSelection(SoCooperativeSelection*, QSet<SoNode*>, QSet<SoPath*>);//TOGGLE/SHIFT
  void userClickedOnBgd();

private slots:
  void visibleObjectsChanged();
  void updateSelectionMode();
  void updateShownTotMomentum();
private:
    
  class Imp;
  Imp * d;
};

#endif
//
