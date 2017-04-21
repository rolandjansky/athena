/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_SAGITTARADIUSESTIMATE_H
#define  TRIGL2MUONSA_SAGITTARADIUSESTIMATE_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ServiceHandle.h"

#include "TrigT1Interfaces/RecMuonRoI.h"

#include "TrigL2MuonSA/RpcFitResult.h"
#include "TrigL2MuonSA/TrackData.h"

#include "TrigL2MuonSA/AlignmentBarrelLUTSvc.h"
#include "TrigL2MuonSA/AlignmentBarrelLUT.h"

namespace TrigL2MuonSA {

  class SagittaRadiusEstimate: public AthAlgTool
{
 public:
  
  static const InterfaceID& interfaceID();

  SagittaRadiusEstimate(const std::string& type, 
			const std::string& name,
			const IInterface*  parent);
  
  ~SagittaRadiusEstimate();
  
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();

  void setMCFlag(BooleanProperty use_mcLUT,
		 const AlignmentBarrelLUTSvc* alignmentBarrelLUTSvc);

  void setUseEndcapInner( BooleanProperty use_endcapInner );
  
 public:
  
  StatusCode setSagittaRadius(const LVL1::RecMuonRoI*     p_roi,
			      TrigL2MuonSA::RpcFitResult& rpcFitResult,
			      TrigL2MuonSA::TrackPattern& trackPattern);
  
 private:
  
  BooleanProperty  m_use_mcLUT;

  BooleanProperty  m_use_endcapInner;

  const ToolHandle<AlignmentBarrelLUT>*    m_alignmentBarrelLUT;
  const AlignmentBarrelLUTSvc*             m_alignmentBarrelLUTSvc;

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
