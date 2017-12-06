/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class TrackSysCommonData                  //
//                                                            //
//  Description: Common pointers, data, node-to-object maps   //
//               etc. for the track system                    //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TRACKSYSCOMMONDATA_H
#define TRACKSYSCOMMONDATA_H

#include "VP1Base/VP1HelperClassBase.h"
#include "VP1Base/VisibleObjectToMaterialHelper.h"
#include "VP1Utils/HitsSoNodeManager.h"

class VP1TrackSystem;
class IVP13DSystem;
class AscObjSelectionManager;
class TrackSystemController;
class MuonChamberProjectionHelper;
class InDetProjHelper;
class TouchedMuonChamberHelper;
class TrackPropagationHelper;
class TrackLODManager;
class VP1TrackSanity;
class SoNode;
class TrackHandleBase;
class SoPointSet;
class QTreeWidgetItem;
class SoSeparator;//Fixme!

namespace Trk {
  class Track;
  class Segment;
}
class VP1TrackSystem;

class TrackSysCommonData : public VP1HelperClassBase {
public:

  TrackSysCommonData(VP1TrackSystem * sys,TrackSystemController *);
  virtual ~TrackSysCommonData();

  VP1TrackSystem * system() const;
  AscObjSelectionManager* ascObjSelectionManager() const;//returns null if not in refreshed state
  TrackSystemController * controller() const;
  MuonChamberProjectionHelper * muonChamberProjectionHelper() const;
  InDetProjHelper * indetProjHelper_Pixel() const;
  InDetProjHelper * indetProjHelper_SCT() const;
  InDetProjHelper * indetProjHelper_TRT() const;
  TouchedMuonChamberHelper * touchedMuonChamberHelper() const;
  TrackPropagationHelper * trackPropagationHelper() const;
  TrackLODManager* trackLODManager() const;
  VisibleObjectToMaterialHelper<Trk::Track> * visTrkTracksToMaterialHelper() const;
  VisibleObjectToMaterialHelper<Trk::Segment> * visTrkSegmentsToMaterialHelper() const;
  VP1TrackSanity * trackSanityHelper() const;
  SoPointSet * singlePoint() const;//A single point at (0,0,0)
  HitsSoNodeManager& nodeManager();

  void setEventData(AscObjSelectionManager*);
  void clearEventData(); //Deletes AscObjSelectionManager

  //register node->track connection (erased at next clearEventData)
  void registerTrack(SoNode*,TrackHandleBase*);
  void unregisterTrack(SoNode*);
  
  // void registerBrowserItem(QTreeWidgetItem* item, SoNode* node);
  // void unregisterBrowserItem(QTreeWidgetItem* item);
  
  //get the connection:
  TrackHandleBase* trackHandle(SoNode*n);
  SoNode* node(TrackHandleBase* h); //!< Returns the node associated with this handle. Returns zero if no match found.
  SoNode* node(QTreeWidgetItem* item); //!< Returns the node associated with this item. Returns zero if no match found.
  
  void setLastSelectedTrack(TrackHandleBase* sel);
  TrackHandleBase* lastSelectedTrackHandle();//!< pointer to base of last selected track (zero if nothing selected)
  SoSeparator* textSep(); // FIXME! FIXME!

  SoSeparator* m_textSep; // FIXME! FIXME! Make private, but then need set method for VP1TrackSystem.cxx:247

  const TrackHandleBase* getHandle(const Trk::Track* trk);
  
private:
  class Imp;
  Imp * m_d;

  VP1TrackSystem * m_3dsystem;
  AscObjSelectionManager* m_ascObjSelectionManager;
  TrackSystemController * m_controller;
  MuonChamberProjectionHelper * m_muonChamberProjectionHelper;
  InDetProjHelper * m_indetProjHelper_Pixel;
  InDetProjHelper * m_indetProjHelper_SCT;
  InDetProjHelper * m_indetProjHelper_TRT;
  TouchedMuonChamberHelper * m_touchedMuonChamberHelper;
  TrackPropagationHelper * m_trackPropagationHelper;
  TrackLODManager* m_trackLODManager;
  VisibleObjectToMaterialHelper<Trk::Track> * m_visTrkTracksToMaterialHelper;
  VisibleObjectToMaterialHelper<Trk::Segment> * m_visTrkSegmentsToMaterialHelper;
  VP1TrackSanity * m_trackSanity;
  SoPointSet * m_singlePoint;
  TrackHandleBase* m_lastSelectedTrack;
  HitsSoNodeManager m_nodeManager;
};

inline VP1TrackSystem* TrackSysCommonData::system() const { return m_3dsystem; }
inline AscObjSelectionManager* TrackSysCommonData::ascObjSelectionManager() const { return m_ascObjSelectionManager; }
inline TrackSystemController * TrackSysCommonData::controller() const { return m_controller; }
inline MuonChamberProjectionHelper * TrackSysCommonData::muonChamberProjectionHelper() const { return m_muonChamberProjectionHelper; }
inline InDetProjHelper * TrackSysCommonData::indetProjHelper_Pixel() const { return m_indetProjHelper_Pixel; }
inline InDetProjHelper * TrackSysCommonData::indetProjHelper_SCT() const { return m_indetProjHelper_SCT; }
inline InDetProjHelper * TrackSysCommonData::indetProjHelper_TRT() const { return m_indetProjHelper_TRT; }
inline TouchedMuonChamberHelper * TrackSysCommonData::touchedMuonChamberHelper() const { return m_touchedMuonChamberHelper; }
inline TrackPropagationHelper * TrackSysCommonData::trackPropagationHelper() const { return m_trackPropagationHelper; }
inline TrackLODManager* TrackSysCommonData::trackLODManager() const { return m_trackLODManager; }
inline VisibleObjectToMaterialHelper<Trk::Track> * TrackSysCommonData::visTrkTracksToMaterialHelper() const { return m_visTrkTracksToMaterialHelper; }
inline VisibleObjectToMaterialHelper<Trk::Segment> * TrackSysCommonData::visTrkSegmentsToMaterialHelper() const { return m_visTrkSegmentsToMaterialHelper; }
inline VP1TrackSanity * TrackSysCommonData::trackSanityHelper() const { return m_trackSanity; }
inline SoPointSet * TrackSysCommonData::singlePoint() const { return m_singlePoint; }
inline void TrackSysCommonData::setLastSelectedTrack( TrackHandleBase* sel) {m_lastSelectedTrack=sel;}
inline TrackHandleBase* TrackSysCommonData::lastSelectedTrackHandle() {return m_lastSelectedTrack;}
inline SoSeparator* TrackSysCommonData::textSep() {return m_textSep;} // FIXME! FIXME!
inline HitsSoNodeManager& TrackSysCommonData::nodeManager()  {return m_nodeManager;}

#endif

