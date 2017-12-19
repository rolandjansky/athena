/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1TrackSystem                      //
//                                                            //
//  Description: System displaying all track-like objects     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1TRACKSYSTEM_H
#define VP1TRACKSYSTEM_H

#include "VP1Base/IVP13DSystemSimple.h"
#include "VP1Utils/InDetProjFlags.h"
#include "GeoModelKernel/GeoPVConstLink.h"
#include <QList>
#include <vector>
#include <map>
class SoMaterial;
class SoCooperativeSelection;
class TrackHandleBase;

namespace Trk {
  class Track;
  class Segment;
  class PrepRawData;
  class ITrackFitter;
}

class VP1TrackSystem : public IVP13DSystemSimple {

  Q_OBJECT

public:

  VP1TrackSystem(QString name="Tracks");
  virtual ~VP1TrackSystem();

  void systemcreate(StoreGateSvc* detstore);
  void systemuncreate();
  QWidget * buildController();
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);
  void systemerase();
  QByteArray saveState();
  void restoreFromState(QByteArray);

  void userPickedNode(SoNode* pickedNode, SoPath *pickedPath);
  void userSelectedSingleNode(SoCooperativeSelection*, SoNode* , SoPath*);//SINGLE
  void userDeselectedSingleNode(SoCooperativeSelection*, SoNode* , SoPath*);//SINGLE
  void userChangedSelection(SoCooperativeSelection*, QSet<SoNode*>, QSet<SoPath*>);//TOGGLE/SHIFT
  void userClickedOnBgd();
  SoCooperativeSelection * selTracks(); //!< Needed in TrackSystemController::objectBrowserClicked(QTreeWidgetItem * item, int)
  SoCooperativeSelection * selAscs(); //!< Needed in TrackSystemController::objectBrowserClicked(QTreeWidgetItem * item, int)
  SoMaterial* materialFromVertex(const TrackHandleBase* trk) const; //!< Returns the material for the vertex which knows this trackhandle (if known)

  public slots:

  //Fixme: Use this or use the prd system directly? The thing to
  //remember is that we have CSC's/TGC's and RPC's also, so it might
  //get complicated, and thus better to keep the logic in a single place.
//  void appropriateMDTProjectionsChanged(int);//0: No projections, 1: Project to end of tubes, 2: Project to end of chamber volume.

  //Fixme: It would be better (and more like the tracksys/segmentsys->prd sys interaction) just to receive a list of visible vertices/jets and their colours!!
  //  void highlightTracks( const std::vector<std::pair<SoMaterial*,std::set<const Trk::Track*> > >&);//Orders remembered on a per-sender basis, so a given sender
                                                                                                  //(JetSystem/VertexSystem/...) can override its own orders again.
  void setApplicableIDProjections( InDetProjFlags::InDetProjPartsFlags pixel,
				   InDetProjFlags::InDetProjPartsFlags sct,
				   InDetProjFlags::InDetProjPartsFlags trt );

  void setSelectedPRDs(const QList<const Trk::PrepRawData*>&);
  void updateAlignment();  //!< Special mode to allow in VP1 alignment tests
  void tracksFromVertexChanged(QList< std::pair<const SoMaterial*, QList< const Trk::Track*> > >& );

signals:
  void touchedMuonChambersChanged(const std::set<GeoPVConstLink>&);//Fixme: Remember to emit in e.g. systemerase (is that automatic?)
  void objectInMuonChamberSelected(const GeoPVConstLink& chamberPV);//For automatic chamber reorientations
  void visibleTracksChanged(const std::vector< std::pair<const Trk::Track*, const SoMaterial*> >&);
  void visibleSegmentsChanged(const std::vector< std::pair<const Trk::Segment*, const SoMaterial*> >&);
  void usedIDProjectionsChanged( InDetProjFlags::DetTypeFlags );
  void newTrackSelected( const TrackHandleBase&);
  void muonChamberT0sChanged(const std::map<GeoPVConstLink, float>& , int );//!< The map is the dt0 per chamber, plus a label identifier, which by convention (!) is 0=Moore, 1=Muonboy

private slots:
  void emitTouchedMuonChambersChanged(const std::set<GeoPVConstLink>&);
  void visibleObjectsChanged();
  void updateSelectionMode();
  void updateShownTotMomentum();
  void possiblyEmitUsedIDProjectionsChanged();
  void refit();//Temporary
private:
  
  void refitFromPRDs(const Trk::ITrackFitter* currentFitter, QList<const Trk::Track*>& fittedtracks);
  void refitSingleTrack(const Trk::ITrackFitter* currentFitter, QList<const Trk::Track*>& fittedtracks);
  
  class Imp;
  Imp * m_d;

};

#endif
//
