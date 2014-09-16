/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_PTFROMALPHABETA_H
#define  TRIGL2MUONSA_PTFROMALPHABETA_H

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"

#include "TrigL2MuonSA/TrackData.h"
#include "TrigL2MuonSA/PtEndcapLUTSvc.h"
#include "TrigL2MuonSA/PtEndcapLUT.h"

namespace TrigL2MuonSA {
  
  class PtFromAlphaBeta
  {
  public:
    
    PtFromAlphaBeta(MsgStream* msg,
		    const TrigL2MuonSA::PtEndcapLUTSvc* ptEndcapLUTSvc);
    
    ~PtFromAlphaBeta();
    
  public:
    
    StatusCode setPt(TrigL2MuonSA::TrackPattern& trackPattern);
    
    double     compute_radius(double InnerSlope, double InnerR, double InnerZ,
			      double MiddleSlope, double MiddleR, double MiddleZ,
			      double sign);
    
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
    
    float f(float x, float c0, float c1, float c2, float c3) const;
    float fp(float x, float c33, float c22, float c1) const;
    
    const TrigL2MuonSA::PtEndcapLUT*    m_ptEndcapLUT;
      
};

} // namespace TrigL2MuonSA

#endif  // PTFROMALPHABETA_H
