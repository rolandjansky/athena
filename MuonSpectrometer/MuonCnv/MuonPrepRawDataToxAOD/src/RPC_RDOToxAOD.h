/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPC_RDOTOXAOD_H
#define RPC_RDOTOXAOD_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcFiredChannel.h"
#include "MuonRDO/RpcCoinMatrix.h"
#include "MuonRDO/RpcPadContainer.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"

// STL include:
#include <vector>

struct RpcTree {
  const RpcPad* Padid;
  const RpcCoinMatrix* Cmaid;
  const RpcFiredChannel* Chid;
};

class RPC_RDOToxAOD : public ::AthAlgorithm {

public:
  // Constructor with parameters:
  RPC_RDOToxAOD(const std::string &name,ISvcLocator *pSvcLocator);

  // Destructor
  ~RPC_RDOToxAOD() {};

  // algorithm execute:
  virtual StatusCode execute();
  virtual StatusCode initialize();
  virtual StatusCode finalize()  { return StatusCode::SUCCESS; }

private:

  SG::WriteHandleKey<xAOD::TrackMeasurementValidationContainer> m_trackMeasVal;
  SG::ReadHandleKey<RpcPadContainer> m_rpcPad;

};


#endif 
