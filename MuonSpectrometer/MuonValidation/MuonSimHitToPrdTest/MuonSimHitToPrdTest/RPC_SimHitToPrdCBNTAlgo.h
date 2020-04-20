/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMHITTOPRD_H
#define MUONSIMHITTOPRD_H

#include <string.h>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "EventPrimitives/EventPrimitives.h"

class RpcHitIdHelper; 

namespace MuonGM
{
    class MuonDetectorManager;
}


class RPC_SimHitToPrdCBNTAlgo : public AthAlgorithm 
{
 public:
  // Agorithm constructor
  RPC_SimHitToPrdCBNTAlgo (const std::string &name, ISvcLocator *pSvcLocator); 
  
  ~RPC_SimHitToPrdCBNTAlgo()=default;

  // Gaudi hooks
  virtual StatusCode initialize();
  virtual StatusCode execute();

  StatusCode clearNTuple();
 
 protected:  
  std::string m_HitCollectionName   ; // name of the input hits
  std::string m_DigitCollectionName ; // name of the digits
  std::string m_RDOCollectionName   ; // name of the RDO
  std::string m_PrdCollectionName   ; // name of the Prd
 
 private: 
  // Private function to add the clusters to the ntuple
  StatusCode doMCtruth  ();
  StatusCode doRPCSimHit();
  StatusCode doRPCDigit ();
  StatusCode doRPCRDO   ();
  StatusCode doRPCPrep  ();

  const MuonGM::MuonDetectorManager*           m_muonMgr     ;
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  RpcHitIdHelper*                              m_muonHelper; // simulation id helper
 
  bool m_doMCtruth;
  bool m_doRPCSimHit;
  bool m_doRPCDigit ;
  bool m_doRPCRDO   ;
  bool m_doRPCPrep  ; 

  class Clockwork;
  Clockwork *m_c;

};
#endif     // MUONSIMHITTOPRD_H
