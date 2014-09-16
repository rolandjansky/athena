/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCHASHID_H
#define CSCHASHID_H

#include <vector>

#include "GaudiKernel/Algorithm.h"
#include "StoreGate/DataHandle.h"
#include "Identifier/Identifier.h"

class StoreGateSvc;
class MsgStream;
class CscIdHelper;
namespace MuonGM {
class MuonDetectorManager;
}

/////////////////////////////////////////////////////////////////////////////

class CscHashId: public Algorithm {

public:

  CscHashId (const std::string& name, ISvcLocator* pSvcLocator);
  ~CscHashId();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  StatusCode cscHash();

private:

  StoreGateSvc               * m_EvtStore;
  const CscIdHelper          * m_cscId;
  const MuonGM::MuonDetectorManager  * m_muon_mgr;

};

#endif



