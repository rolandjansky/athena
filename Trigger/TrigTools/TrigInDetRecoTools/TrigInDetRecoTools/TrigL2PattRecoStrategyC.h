/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_PATT_RECO_STRATEGY_C_H__
#define __TRIG_L2_PATT_RECO_STRATEGY_C_H__

#include<vector>

#include "GaudiKernel/IAlgTool.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigInDetRecoTools/ITrigL2PattRecoStrategy.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"



class TrigSiSpacePoint;
class IRoiDescriptor;
class TrigInDetTrackCollection;

class SpacePointCollection;  
class SpacePointContainer;  

class IRegSelSvc;


class SCT_ID;
class PixelID;

#include "TrigInDetEvent/TrigSiSpacePointCollection.h"
#include "TrkTrack/TrackCollection.h" 


class TrigTimer; 
//class IRegSelSvc;
 
namespace InDet { 
  class ISiSpacePointsSeedMaker; 
  class ISiZvertexMaker; 
  class ISiTrackMaker; 
}  
  
class TrigL2PattRecoStrategyC: public AthAlgTool, virtual public ITrigL2PattRecoStrategy { 
  
 public: 
  TrigL2PattRecoStrategyC(const std::string&, const std::string&, const IInterface* );
  virtual ~TrigL2PattRecoStrategyC(); 
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  using ITrigL2PattRecoStrategy::findTracks;

  HLT::ErrorCode findTracks(const std::vector<const TrigSiSpacePoint*>&, 
				    const IRoiDescriptor&,
				    TrigInDetTrackCollection*);

  HLT::ErrorCode findTracks(const std::vector<const TrigSiSpacePoint*>&,
				    TrigInDetTrackCollection*);

  inline double getTimingA() const {
    return m_pattRecoTiming;
  }

 private:
  bool                    m_useZvertexTool;  
  int                     m_nseeds;       //!< Number seeds 
  int                     m_ntracks;      //!< Number found tracks 
  int                     m_nfreeCut;     // Min number free clusters 

  ToolHandle< InDet::ISiSpacePointsSeedMaker > m_seedsmaker;   // Space poins seed     maker 
  ToolHandle< InDet::ISiZvertexMaker         > m_zvertexmaker; // Space poins z-vertex maker 
  ToolHandle< InDet::ISiTrackMaker           > m_trackmaker;   // Track                maker 

  ServiceHandle<IRegSelSvc>     m_regionSelector;      //!< region selector service

  std::string m_sctSpacePointsName; 
  std::string m_pixelSpacePointsName;  

  std::string m_sctOnlineSpacePointsName; 
  std::string m_pixelOnlineSpacePointsName; 


  SpacePointContainer* m_sctSpacePointsContainer;  
  SpacePointContainer* m_pixelSpacePointsContainer;  
  const TrigSiSpacePointContainer* m_sctOnlineSpacePointsContainer;
  const TrigSiSpacePointContainer* m_pixelOnlineSpacePointsContainer;

  double trackQuality(const Trk::Track*);
  void filterSharedTracks(std::multimap<double,Trk::Track*>&);

  StatusCode convertInputData( const std::vector<IdentifierHash>&, const std::vector<IdentifierHash>&);
  StatusCode convertInputData();
  StatusCode getIDCs();

  TrigInDetTrack* convertOutputTracks(Trk::Track*);

  const SCT_ID* m_sctId;
  const PixelID* m_pixelId;

  bool m_timers;
  TrigTimer* m_timer[10];

  double m_pattRecoTiming;

};



#endif


 
