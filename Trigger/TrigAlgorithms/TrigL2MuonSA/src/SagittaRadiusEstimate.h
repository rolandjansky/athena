/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_SAGITTARADIUSESTIMATE_H
#define  TRIGL2MUONSA_SAGITTARADIUSESTIMATE_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ServiceHandle.h"

#include "TrigT1Interfaces/RecMuonRoI.h"

#include "RpcFitResult.h"
#include "TrackData.h"

#include "AlignmentBarrelLUTSvc.h"
#include "AlignmentBarrelLUT.h"

namespace TrigL2MuonSA {

  class SagittaRadiusEstimate: public AthAlgTool
{
 public:
  
  SagittaRadiusEstimate(const std::string& type, 
			const std::string& name,
			const IInterface*  parent);
  
  void setMCFlag(BooleanProperty use_mcLUT,
		 const AlignmentBarrelLUTSvc* alignmentBarrelLUTSvc);

  void setUseEndcapInner( BooleanProperty use_endcapInner ){ m_use_endcapInner = use_endcapInner; };
  
 public:
  
  StatusCode setSagittaRadius(const LVL1::RecMuonRoI*     p_roi,
			      TrigL2MuonSA::RpcFitResult& rpcFitResult,
			      TrigL2MuonSA::TrackPattern& trackPattern);
  
 private:
  
  BooleanProperty  m_use_mcLUT{0};

  BooleanProperty  m_use_endcapInner{0};

  const ToolHandle<AlignmentBarrelLUT>*    m_alignmentBarrelLUT;

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
