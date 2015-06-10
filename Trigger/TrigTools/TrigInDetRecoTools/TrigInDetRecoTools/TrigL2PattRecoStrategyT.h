/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_PATT_RECO_STRATEGY_T_H__
#define __TRIG_L2_PATT_RECO_STRATEGY_T_H__

#include<vector>

//#include "GaudiKernel/IAlgTool.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigInDetRecoTools/ITrigL2SeededPattRecoStrategy.h"
#include "InDetRecToolInterfaces/ITRT_TrackSegmentsMaker.h"
#include "TrigInDetToolInterfaces/ITrigTRT_DriftCircleProvider.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

class IRegSelSvc;
class IROBDataProviderSvc;


class IRoiDescriptor;


class SCT_ID;
class PixelID;
class TRT_ID;

#include "TrigInDetEvent/TrigSiSpacePointCollection.h"

class TrigTimer;

//#include "EventInfo/EventID.h"

namespace InDet { 
  class ITRT_SeededTrackFinder;
}
  
//#include "InDetPrepRawData/SiClusterContainer.h" 
//#include "InDetPrepRawData/PixelClusterContainer.h" 
//#include "InDetPrepRawData/SCT_ClusterContainer.h" 
//#include "InDetPrepRawData/PixelClusterCollection.h" 
//#include "InDetPrepRawData/SCT_ClusterCollection.h" 
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
//#include "InDetPrepRawData/TRT_DriftCircleCollection.h"

class TrigL2PattRecoStrategyT: public AthAlgTool, virtual public ITrigL2SeededPattRecoStrategy { 
  
 public: 
  TrigL2PattRecoStrategyT(const std::string&, const std::string&, const IInterface* );
  virtual ~TrigL2PattRecoStrategyT(); 
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  HLT::ErrorCode findTracks(const TrigInDetTrackCollection& inputTrackColl,
			    const std::vector<const TrigSiSpacePoint*>& vsp, 
			    const IRoiDescriptor& roi,
			    TrigInDetTrackCollection* trackColl);

  HLT::ErrorCode findTracks(const TrigInDetTrackCollection& inputTrackColl,
			    const std::vector<const TrigSiSpacePoint*>& vsp,
			    TrigInDetTrackCollection* trackColl);

  inline double getTimingA() const {
    return m_pattRecoTiming;
  }

 private:

  
  StatusCode getFilteredTRT_Hits(const TrigInDetTrackCollection& inputTrackColl, const IRoiDescriptor& roi, 
				 std::vector<IdentifierHash>& );
  StatusCode getFilteredTRT_Hits(const TrigInDetTrackCollection& inputTrackColl, 
				 std::vector<IdentifierHash>& );

  void createTriggerTrack(const Trk::Track&,const Trk::TrackSegment&, TrigInDetTrackCollection*);
  StatusCode getIDCs();

  TrigTimer* m_totalTimer;
  double m_pattRecoTiming;

  int                     m_nseeds;       //!< Number seeds 
  int                     m_ntracks;      //!< Number found tracks 
  int                     m_nfreeCut;     // Min number free clusters 

  int m_minTrtHits;

  ToolHandle<InDet::ITRT_SeededTrackFinder> m_trackmaker;   // Track                maker ; 
  ServiceHandle<IRegSelSvc>     m_regionSelector;      //!< region selector service
  ToolHandle<ITrigTRT_DriftCircleProviderTool> m_trtDataProvider;
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;

  ToolHandle<InDet::ITRT_TrackSegmentsMaker>    m_segmentsmaker   ;


  InDet::TRT_DriftCircleContainer* m_filteredDriftCircleContainer;
  std::string m_trtFilteredContName;
  std::string m_trtContName;


  std::string m_sctOnlineSpacePointsName; 
  std::string m_pixelOnlineSpacePointsName;
  const TrigSiSpacePointContainer* m_sctOnlineSpacePointsContainer;
  const TrigSiSpacePointContainer* m_pixelOnlineSpacePointsContainer;

  const SCT_ID* m_sctId;
  const PixelID* m_pixelId;
  const TRT_ID* m_trtId; 

  std::vector<const ROBF*> m_vROBF;

  bool m_timers;
  TrigTimer* m_timer[10];
};



#endif


 
