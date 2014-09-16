/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_SAGITTARADIUSESTIMATE_H
#define  TRIGL2MUONSA_SAGITTARADIUSESTIMATE_H

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"

#include "TrigT1Interfaces/RecMuonRoI.h"

#include "TrigL2MuonSA/RpcFitResult.h"
#include "TrigL2MuonSA/TrackData.h"

#include "TrigL2MuonSA/AlignmentBarrelLUTSvc.h"
#include "TrigL2MuonSA/AlignmentBarrelLUT.h"

namespace TrigL2MuonSA {

class SagittaRadiusEstimate
{
 public:
  
  SagittaRadiusEstimate(MsgStream* msg,
			BooleanProperty use_mcLUT,
			const TrigL2MuonSA::AlignmentBarrelLUTSvc* alignmentBarrelLUTSvc);
  
  ~SagittaRadiusEstimate();
  
 public:
  
  StatusCode setSagittaRadius(const LVL1::RecMuonRoI*     p_roi,
			      TrigL2MuonSA::RpcFitResult& rpcFitResult,
			      TrigL2MuonSA::TrackPattern& trackPattern);
  
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
  
  BooleanProperty  m_use_mcLUT;

  const TrigL2MuonSA::AlignmentBarrelLUTSvc* m_alignmentBarrelLUTSvc;
  const TrigL2MuonSA::AlignmentBarrelLUT*    m_alignmentBarrelLUT;

  float f(float x, float c0, float c1, float c2, float c3) const;
  float fp(float x, float c33, float c22, float c1) const;
};
 
 
 inline float TrigL2MuonSA::SagittaRadiusEstimate::f(float x, float c0, float c1, float c2, float c3) const
   {
     return c0 + x * (c1 + x * (c2 + x * c3));
   }
 
 inline float TrigL2MuonSA::SagittaRadiusEstimate::fp(float x, float c33, float c22, float c1) const
   {
     return c1 + x * (c22 + x * c33);
   }
 
} // namespace TrigL2MuonSA

#endif  // SAGITTARADIUSESTIMATE_H
