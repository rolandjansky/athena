/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MUFASTTRACKFITTER_H
#define  TRIGL2MUONSA_MUFASTTRACKFITTER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigT1Interfaces/RecMuonRoI.h"

#include "TrigL2MuonSA/RpcFitResult.h"
#include "TrigL2MuonSA/TgcFitResult.h"
#include "TrigL2MuonSA/TrackData.h"

#include "TrigL2MuonSA/PtEndcapLUTSvc.h"
#include "TrigL2MuonSA/PtBarrelLUTSvc.h"
#include "TrigL2MuonSA/AlignmentBarrelLUTSvc.h"

#include "TrigL2MuonSA/SagittaRadiusEstimate.h"
#include "TrigL2MuonSA/AlphaBetaEstimate.h"
#include "TrigL2MuonSA/PtFromRadius.h"
#include "TrigL2MuonSA/PtFromAlphaBeta.h"

class StoreGateSvc;

namespace TrigL2MuonSA {

  class MuFastTrackFitter: public AthAlgTool
  {
  public:
    
    static const InterfaceID& interfaceID();
    
  public:
    
    MuFastTrackFitter(const std::string& type, 
		      const std::string& name,
		      const IInterface*  parent);
    
    ~MuFastTrackFitter();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize  ();
    
  public:
    
    StatusCode findTracks(const LVL1::RecMuonRoI*     p_roi,
			  TrigL2MuonSA::RpcFitResult& rpcFitResult,
			  std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns);
    
    StatusCode findTracks(const LVL1::RecMuonRoI*     p_roi,
			  TrigL2MuonSA::TgcFitResult& tgcFitResult,
			  std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns,
                          const TrigL2MuonSA::MuonRoad& muonRoad);
    
    StatusCode setMCFlag(BooleanProperty  use_mcLUT);

    void setUseEIFromBarrel( BooleanProperty use_endcapInnerFromBarrel );

  private:
    // Reference to StoreGateSvc;
    ServiceHandle<StoreGateSvc>    m_storeGateSvc;
    
    BooleanProperty  m_use_mcLUT;

    const PtEndcapLUTSvc*                  m_ptEndcapLUTSvc;
    const PtBarrelLUTSvc*                  m_ptBarrelLUTSvc;
    
    const AlignmentBarrelLUTSvc*           m_alignmentBarrelLUTSvc;
    
    BooleanProperty  m_use_endcapInnerFromBarrel;

    ToolHandle<SagittaRadiusEstimate>      m_sagittaRadiusEstimate;
    ToolHandle<AlphaBetaEstimate>          m_alphaBetaEstimate;
    
    ToolHandle<PtFromRadius>               m_ptFromRadius;
    ToolHandle<PtFromAlphaBeta>            m_ptFromAlphaBeta;
  };

} // namespace TrigL2MuonSA

#endif  // MUFASTTRACKFITTER_H
