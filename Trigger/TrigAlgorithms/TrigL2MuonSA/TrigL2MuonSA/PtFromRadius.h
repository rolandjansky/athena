/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_PTFROMRADIUS_H
#define  TRIGL2MUONSA_PTFROMRADIUS_H

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"

#include "TrigL2MuonSA/TrackData.h"
#include "TrigL2MuonSA/PtBarrelLUTSvc.h"
#include "TrigL2MuonSA/PtBarrelLUT.h"

namespace TrigL2MuonSA {

class PtFromRadius
{
 public:
  
  PtFromRadius(MsgStream* msg,
	       BooleanProperty  m_use_mcLUT,
	       const TrigL2MuonSA::PtBarrelLUTSvc* ptBarrelLUTSvc);
  
  ~PtFromRadius();
  
 public:
  
  // For Barrel
  StatusCode setPt(TrigL2MuonSA::TrackPattern& trackPattern);
  
 private:
  /** @brief Pointer to MsgStream.*/
  MsgStream* m_msg;
  
  /**
   * @brief Accessor method for the MsgStream.
   * @return handle to the MsgStream.
   */
  inline MsgStream& msg() const { return *m_msg; }

  BooleanProperty  m_use_mcLUT;
  
  const TrigL2MuonSA::PtBarrelLUT*    m_ptBarrelLUT;
};
 
} // namespace PtFromRadius

#endif  // PTFROMRADIUS_H
