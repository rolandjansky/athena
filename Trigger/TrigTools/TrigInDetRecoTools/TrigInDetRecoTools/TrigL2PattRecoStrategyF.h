/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_PATT_RECO_STRATEGY_F_H__
#define __TRIG_L2_PATT_RECO_STRATEGY_F_H__

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

//class SpacePointCollection;  
//class SpacePointContainer;  

class IRegSelSvc;

class SCT_ID;
class PixelID;

#include "TrigInDetEvent/TrigSiSpacePointCollection.h"


class TrigTimer; 
//class IRegSelSvc;
  
class TrigL2PattRecoStrategyF: public AthAlgTool, virtual public ITrigL2PattRecoStrategy { 
  
 public: 
  TrigL2PattRecoStrategyF(const std::string&, const std::string&, const IInterface* );
  virtual ~TrigL2PattRecoStrategyF(); 
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

  ServiceHandle<IRegSelSvc>     m_regionSelector;      //!< region selector service
  std::string m_ftkTracksName;

  double m_ptCut;
  double m_chi2Cut;

  const TrigInDetTrackCollection* m_ftkTracksContainer;

  std::vector<const TrigInDetTrackCollection*> m_selectedFtkTracks;

  StatusCode getFTK_Tracks();
  StatusCode getFTK_Tracks(const std::vector<IdentifierHash>&);

  StatusCode retrieveIDC();
  TrigInDetTrack* createOutputTrack(const TrigInDetTrack*);

  const SCT_ID* m_sctId;
  const PixelID* m_pixelId;

  bool m_timers;
  TrigTimer* m_timer[10];

  double m_pattRecoTiming;

};



#endif


 
