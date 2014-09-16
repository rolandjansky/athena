/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_ALPHABETAESTIMATE_H
#define  TRIGL2MUONSA_ALPHABETAESTIMATE_H

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"

#include "TrigT1Interfaces/RecMuonRoI.h"

#include "TrigL2MuonSA/TgcFitResult.h"
#include "TrigL2MuonSA/TrackData.h"

#include "TrigL2MuonSA/PtEndcapLUTSvc.h"

namespace TrigL2MuonSA {
  
  class AlphaBetaEstimate
  {
  public:
    
    AlphaBetaEstimate(MsgStream* msg,
		      const TrigL2MuonSA::PtEndcapLUTSvc* ptEndcapLUTSvc);
    
    ~AlphaBetaEstimate();
    
  public:
    
    StatusCode setAlphaBeta(const LVL1::RecMuonRoI*     p_roi,
			    TrigL2MuonSA::TgcFitResult& tgcFitResult,
			    TrigL2MuonSA::TrackPattern& trackPattern);
    
  private:
    
    double     computeRadius(double InnerSlope, double InnerR, double InnerZ,
			      double MiddleSlope, double MiddleR, double MiddleZ,
			      double sign);
    
    double     computeRadius3Points(double InnerZ, double InnerR, double EEZ, double EER,double MiddleZ, double MiddleR );
    
    int        ClassifySL(double tgcPhi);
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
    inline MSG::Level msgLvl() const { return (m_msg != 0) ? m_msg->level() : MSG::NIL; }
    
    const TrigL2MuonSA::PtEndcapLUT*    m_ptEndcapLUT;
  };
  
} // namespace TrigL2MuonSA

#endif  // ALPHABETAESTIMATE_H
