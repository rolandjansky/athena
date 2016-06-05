/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_PTFROMRADIUS_H
#define  TRIGL2MUONSA_PTFROMRADIUS_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ServiceHandle.h"

#include "TrigL2MuonSA/TrackData.h"
#include "TrigL2MuonSA/PtBarrelLUTSvc.h"
#include "TrigL2MuonSA/PtBarrelLUT.h"

namespace TrigL2MuonSA {

class PtFromRadius: public AthAlgTool
{
 public:
  
  static const InterfaceID& interfaceID();

  PtFromRadius(const std::string& type, 
	       const std::string& name,
	       const IInterface*  parent);
  
  ~PtFromRadius();
  
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();

  void setMCFlag(BooleanProperty  m_use_mcLUT,
		 const TrigL2MuonSA::PtBarrelLUTSvc* ptBarrelLUTSvc);
  
 public:
  
  // For Barrel
  StatusCode setPt(TrigL2MuonSA::TrackPattern& trackPattern);
  
 private:
  BooleanProperty  m_use_mcLUT;
  
  const ToolHandle<PtBarrelLUT>*    m_ptBarrelLUT;
};
 
} // namespace PtFromRadius

#endif  // PTFROMRADIUS_H
