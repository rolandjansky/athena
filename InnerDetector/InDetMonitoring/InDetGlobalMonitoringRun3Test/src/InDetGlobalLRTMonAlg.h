/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


/** @file InDetGlobalLRTMonAlg.h
 * Implementation of inner detector global track monitoring tool
 *
 *@author
 * Leonid Serkin <lserkin@cern.ch> @n
 * 
 * based on InDetGlobalLRTMonTool.h 
 * 
 ****************************************************************************/

#ifndef InDetGlobalLRTMonAlg_H
#define InDetGlobalLRTMonAlg_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "GaudiKernel/EventContext.h"

#include "PixelGeoModel/IBLParameterSvc.h"
#include "InDetPrepRawData/PixelClusterContainer.h"


//Detector Managers
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "StoreGate/ReadHandleKey.h"


//------------TrackMon------------
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

#include "TrkTrackSummary/TrackSummary.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"

//Standard c++
#include <string>
#include <memory>



//------------------------------

class PixelID;
class SCT_ID;
class TRT_ID;

namespace InDet {
  class IInDetTrackSelectionTool;
}




class InDetGlobalLRTMonAlg : public AthMonitorAlgorithm {
  
 public:
  
  InDetGlobalLRTMonAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~InDetGlobalLRTMonAlg();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  std::string findComponentString(int bec, int ld) const;
  
  
  // Functions to fill individual sets of histograms
  void fillForwardTracks( const Trk::Track *track, const std::unique_ptr<const Trk::TrackSummary> & summary );
  void fillEtaPhi( const Trk::Track *track, const std::unique_ptr<const Trk::TrackSummary> & summary );
  void fillHits( const Trk::Track *track, const std::unique_ptr<const Trk::TrackSummary> & summary );
  void fillTIDE();
  void fillHoles( const Trk::Track *track, const std::unique_ptr<const Trk::TrackSummary> & summary );
  void fillHitMaps( const Trk::Track *track );
  void fillHoleMaps( const Trk::Track *track );
  
 private:
  
  ToolHandle <Trk::ITrackHoleSearchTool> m_holes_search_tool; // new
  
  ToolHandle <InDet::IInDetTrackSelectionTool> m_trackSelTool; // baseline
  
  ToolHandle <IInDetConditionsTool> m_pixelCondSummaryTool{this, "PixelConditionsSummaryTool", "PixelConditionsSummaryTool", "Tool to retrieve Pixel Conditions summary"};
  
  ToolHandle <Trk::ITrackSummaryTool> m_trkSummaryTool{this,"TrackSummaryTool","Trk::TrackSummaryTool/InDetTrackSummaryTool",""};
  
  
  
  const AtlasDetectorID* m_atlasid;  //tracks only
  
  // the TRT ID helper
  const TRT_ID *m_trtID;
  
  // the SCT ID helper 
  const SCT_ID *m_sctID;  
  
  // the Pixel ID helper 
  const PixelID *m_pixelID;
  
  
  SG::ReadHandleKey<TrackCollection> m_tracksKey         {this,"TrackName", "CombinedInDetTracks", "track data key"};
  SG::ReadHandleKey<TrackCollection> m_CombinedTracksName{this,"TrackName2","CombinedInDetTracks", "track data key"};
  SG::ReadHandleKey<TrackCollection> m_ForwardTracksName {this,"TrackName3","CombinedInDetTracks", "track data key"};
  SG::ReadHandleKey<TrackCollection> m_tracksLarge {this,"TrackName4","ExtendedLargeD0Tracks", "LRT Container"};  

  
  ServiceHandle <IBLParameterSvc> m_IBLParameterSvc;
  
  
  
  //Switch if LB accounting should be done
  bool m_doLumiblock;
  
  // Switch for hole searching
  bool m_doHolePlots;
  bool m_DoHoles_Search;
  
  // Switch for hitmaps
  bool m_doHitMaps;
  
  bool m_doTide;
  bool m_doTideResiduals;
  bool m_doForwardTracks;
  bool m_doIBL;
  
};
#endif
