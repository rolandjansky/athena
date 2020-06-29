/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_PTFROMRADIUS_H
#define  TRIGL2MUONSA_PTFROMRADIUS_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ServiceHandle.h"

#include "TrackData.h"
#include "PtBarrelLUTSvc.h"
#include "PtBarrelLUT.h"

namespace TrigL2MuonSA {

class PtFromRadius: public AthAlgTool
{
 public:
  
  PtFromRadius(const std::string& type, 
	       const std::string& name,
	       const IInterface*  parent);
  
  void setMCFlag(BooleanProperty  m_use_mcLUT,
		 const TrigL2MuonSA::PtBarrelLUTSvc* ptBarrelLUTSvc);
  
 public:
  
  // For Barrel
  StatusCode setPt(TrigL2MuonSA::TrackPattern& trackPattern);
  
 private:
  BooleanProperty  m_use_mcLUT{0};
  
  const ToolHandle<PtBarrelLUT>*   m_ptBarrelLUT{nullptr};
};
 
} // namespace PtFromRadius

#endif  // PTFROMRADIUS_H
