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

#ifndef VP1AODSYSTEMS_VP1AODSYSTEM_H
#define VP1AODSYSTEMS_VP1AODSYSTEM_H

#include "VP1Base/IVP13DSystemSimple.h"

#include "GaudiKernel/SystemOfUnits.h"

#include <QList>

#include <vector>
#include <map>

class SoMaterial;
class SoCooperativeSelection;

#ifndef Q_MOC_RUN
#include "xAODTracking/TrackParticleFwd.h"
// #include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODMuon/MuonSegment.h"
#endif 

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

  SoCooperativeSelection * selObjects() const;

  /// Reimplementing methods declared in IVP13DSystem
  void userPickedNode(SoNode* pickedNode, SoPath *pickedPath);
  void userSelectedSingleNode(SoCooperativeSelection*, SoNode* , SoPath*);//SINGLE
  void userDeselectedSingleNode(SoCooperativeSelection*, SoNode* , SoPath*);//SINGLE
  void userChangedSelection(SoCooperativeSelection*, QSet<SoNode*>, QSet<SoPath*>);//TOGGLE/SHIFT
  void userClickedOnBgd();  

public slots:
// FIXME - might be best to make a helper tool to do this, so we don't need to expose the xAOD objects in the interface here.
// (Of course, that means we would then need to think of an alternate design to fill the collections in the system...)
  void updateAssociatedObjects(const QList<const xAOD::TrackParticle*>&);
  // void updateAssociatedObjects(QList<xAOD::CaloCluster*>&);
  void updateAssociatedObjects(const QList<const xAOD::MuonSegment*>&);

  void dumpToJSON();

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
