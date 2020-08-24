/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MUFASTTRACKFITTER_H
#define  TRIGL2MUONSA_MUFASTTRACKFITTER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigT1Interfaces/RecMuonRoI.h"

#include "RpcFitResult.h"
#include "TgcFitResult.h"
#include "TrackData.h"

#include "PtEndcapLUTSvc.h"
#include "PtBarrelLUTSvc.h"
#include "AlignmentBarrelLUTSvc.h"

#include "SagittaRadiusEstimate.h"
#include "AlphaBetaEstimate.h"
#include "PtFromRadius.h"
#include "PtFromAlphaBeta.h"

namespace TrigL2MuonSA {

  class MuFastTrackFitter: public AthAlgTool
  {
  public:
    
    MuFastTrackFitter(const std::string& type, 
		      const std::string& name,
		      const IInterface*  parent);
    
    virtual StatusCode initialize() override;
    
  public:
    
    StatusCode findTracks(const LVL1::RecMuonRoI*     p_roi,
			  TrigL2MuonSA::RpcFitResult& rpcFitResult,
			  std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns);
    
    StatusCode findTracks(const LVL1::RecMuonRoI*     p_roi,
			  TrigL2MuonSA::TgcFitResult& tgcFitResult,
			  std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns,
                          const TrigL2MuonSA::MuonRoad& muonRoad);
    
    StatusCode setMCFlag(BooleanProperty  use_mcLUT);

    void setUseEIFromBarrel( BooleanProperty use_endcapInnerFromBarrel ) {m_use_endcapInnerFromBarrel = use_endcapInnerFromBarrel;};

  private:
    BooleanProperty  m_use_mcLUT {true};
    BooleanProperty  m_use_endcapInnerFromBarrel {false};

    ToolHandle<SagittaRadiusEstimate> m_sagittaRadiusEstimate
      {this, "SagittaRadiusEstimate", "TrigL2MuonSA::SagittaRadiusEstimate"};
    ToolHandle<AlphaBetaEstimate> m_alphaBetaEstimate
      {this, "AlphaBetaEstimate", "TrigL2MuonSA::AlphaBetaEstimate"};
    
    ToolHandle<PtFromRadius> m_ptFromRadius
      {this, "PtFromRadius", "TrigL2MuonSA::PtFromRadius"};
    ToolHandle<PtFromAlphaBeta> m_ptFromAlphaBeta
      {this, "PtFromAlphaBeta", "TrigL2MuonSA::PtFromAlphaBeta"};
  };

} // namespace TrigL2MuonSA

#endif  // MUFASTTRACKFITTER_H
