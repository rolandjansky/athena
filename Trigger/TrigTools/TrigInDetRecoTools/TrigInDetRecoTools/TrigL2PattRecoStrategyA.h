/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_PATT_RECO_STRATEGY_A_H__
#define __TRIG_L2_PATT_RECO_STRATEGY_A_H__

#include<vector>

#include "GaudiKernel/IAlgTool.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigInDetRecoTools/ITrigL2PattRecoStrategy.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

class TrigTimer;
class IRegSelSvc;
class ITrigRun1ZFinder;
class IBeamCondSvc;
class ITrigHitFilter;
class ITrigL2DupTrackRemovalTool;
class EventID;

//#include "EventInfo/EventID.h"

  
class TrigL2PattRecoStrategyA: public AthAlgTool, virtual public ITrigL2PattRecoStrategy { 
  
 public: 
  TrigL2PattRecoStrategyA(const std::string&, const std::string&, const IInterface* );
  virtual ~TrigL2PattRecoStrategyA(); 
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  using ITrigL2PattRecoStrategy::findTracks;

  HLT::ErrorCode findTracks(const std::vector<const TrigSiSpacePoint*>& vsp, 
				    const IRoiDescriptor& roi,
				    TrigInDetTrackCollection* trackColl);

  HLT::ErrorCode findTracks(const std::vector<const TrigSiSpacePoint*>& vsp,
				    TrigInDetTrackCollection* trackColl);

  inline double getTimingA() const {
    return m_pattRecoTiming;
  }

 private:
  // fill the detector mask
  BooleanProperty m_check_detector_mask;
  BooleanProperty m_detector_mask_on_event;
  bool m_detector_mask_not_checked;
  void setup_detector_mask(const EventID* eventId);

  // check missing layers
  bool m_hasPixelBarrel, m_hasPixelDisk, m_hasPixelBLayer;
  bool m_hasSCTBarrelA, m_hasSCTEndcapA;
  bool m_hasSCTBarrelC, m_hasSCTEndcapC;
  // adjust threshold accordingly
  bool   m_adjustLayerThreshold;

  ServiceHandle<IRegSelSvc>     m_regionSelector;      //!< region selector service

  // z-finder stuff
  ToolHandle<ITrigRun1ZFinder>   m_zFinder;
  long m_zFinderMode;         // 0: run zfinder, 1:use z0 from MC truth, 2:use z0=m_zVertexValue (not implemented?)
  bool m_findMultipleZ;       // false: only take tracks from one vertex, true: use tracks from all verticies
  std::string m_TrueVertexLocation;
  int m_nZvertices;

  // beamspot stuff
  bool m_useBeamSpot;

  bool m_doZF_Only;

  IBeamCondSvc* m_iBeamCondSvc;

  // patt rec. stuff
  ToolHandle<ITrigHitFilter>  m_hitFilter;
  ToolHandle<ITrigL2DupTrackRemovalTool>  m_dupTrkRemoval;
 
  bool m_timers;
  TrigTimer* m_totalTimer;
  TrigTimer* m_ZFinderTimer;
  TrigTimer* m_HitFilterTimer;
  double m_pattRecoTiming;

};



#endif


 
