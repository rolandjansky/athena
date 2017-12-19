/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1PREPRAWDATASYSTEM_H
#define VP1PREPRAWDATASYSTEM_H

#include "VP1Base/IVP13DSystemSimple.h"
#include "GeoModelKernel/GeoPVConstLink.h"//It would be good to get rid of this.
#include "VP1Utils/InDetProjFlags.h"
#include <vector>
#include <QList>

class SoMaterial;
namespace Trk
{
  class Track;
  class Segment;
  class PrepRawData;
}

class VP1PrepRawDataSystem : public IVP13DSystemSimple {

  Q_OBJECT

public:
  VP1PrepRawDataSystem();
  virtual ~VP1PrepRawDataSystem();
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);
  void buildPermanentSceneGraph(StoreGateSvc* detstore, SoSeparator *root);//For TRT Projection surfaces
  void systemcreate(StoreGateSvc* detstore);
  void systemerase();
  void systemuncreate();

  void userPickedNode(SoNode* pickedNode, SoPath * pickedPath);
  void userSelectedSingleNode(SoCooperativeSelection*, SoNode* , SoPath*);//SINGLE
  void userDeselectedSingleNode(SoCooperativeSelection*, SoNode* , SoPath*);//SINGLE
  void userChangedSelection(SoCooperativeSelection*, QSet<SoNode*>, QSet<SoPath*>);//TOGGLE/SHIFT
  void userClickedOnBgd();

  virtual void deselectAll(SoCooperativeSelection* exception_sel = 0);

  virtual QWidget * buildController();

  QByteArray saveState();
  void restoreFromState(QByteArray);

public slots:
  void visibleTracksChanged(const std::vector< std::pair<const Trk::Track*, const SoMaterial*> >&);
  void visibleSegmentsChanged(const std::vector< std::pair<const Trk::Segment*, const SoMaterial*> >&);
  void appropriateMDTProjectionsChanged(int);//0: No projections, 1: Project to end of tubes, 2: Project to end of chamber volume.
  void setApplicableIDProjections( InDetProjFlags::InDetProjPartsFlags pixel,
				   InDetProjFlags::InDetProjPartsFlags sct,
				   InDetProjFlags::InDetProjPartsFlags trt );
  void muonChambersWithTracksChanged(const std::set<GeoPVConstLink>&);

signals:
  void touchedMuonChambersChanged(const std::set<GeoPVConstLink>&);//Fixme: Remember to emit in e.g. systemerase (is that automatic?)
  void prdInMuonChamberSelected(const GeoPVConstLink& chamberPV);
  void usedIDProjectionsChanged( InDetProjFlags::DetTypeFlags );
  void selectedPRDsChanged(const QList<const Trk::PrepRawData*>&);

private slots:
  void emitTouchedMuonChambersChanged(const std::set<GeoPVConstLink>&);//for TouchedMuonChamberHelper
  void emitUsedIDProjectionsChanged(InDetProjFlags::DetTypeFlags);//for controller
  void selectionModeChanged();
  void selectionVisualsChanged();
  void clearSelection();
  void updateSelectionVisualsAndPossiblyEmitPRDList();
private:
  class Imp;
  Imp * m_d;
};

#endif

