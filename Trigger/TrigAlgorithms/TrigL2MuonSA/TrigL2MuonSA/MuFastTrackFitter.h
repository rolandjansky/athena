/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MUFASTTRACKFITTER_H
#define  TRIGL2MUONSA_MUFASTTRACKFITTER_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"

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

  class MuFastTrackFitter: public AlgTool
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
			  std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns);
    
    StatusCode setMCFlag(BooleanProperty  use_mcLUT);

  private:
    
    /** @brief Pointer to MsgStream.*/
    MsgStream* m_msg;
    
    /**
     * @brief Accessor method for the MsgStream.
     * @return handle to the MsgStream.
     */
    inline MsgStream& msg() const { return *m_msg; }

    /**
     * @brief Accessor method for the message level variable.
     * @return value of the message level for this algorithm.
     */
    inline MSG::Level msgLvl() const { return  (m_msg != 0) ? m_msg->level() : MSG::NIL; }
    
    // Reference to StoreGateSvc;
    ServiceHandle<StoreGateSvc>    m_storeGateSvc;
    
    BooleanProperty  m_use_mcLUT;

    const TrigL2MuonSA::PtEndcapLUTSvc*    m_ptEndcapLUTSvc;
    const TrigL2MuonSA::PtBarrelLUTSvc*    m_ptBarrelLUTSvc;
    
    const TrigL2MuonSA::AlignmentBarrelLUTSvc* m_alignmentBarrelLUTSvc;
    
    TrigL2MuonSA::SagittaRadiusEstimate*   m_sagittaRadiusEstimate;
    TrigL2MuonSA::AlphaBetaEstimate*       m_alphaBetaEstimate;
    
    TrigL2MuonSA::PtFromRadius*            m_ptFromRadius;
    TrigL2MuonSA::PtFromAlphaBeta*         m_ptFromAlphaBeta;
  };

} // namespace TrigL2MuonSA

#endif  // MUFASTTRACKFITTER_H
