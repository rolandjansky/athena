/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCHASHID_H
#define CSCHASHID_H

#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ActiveStoreSvc.h"
#include "Identifier/Identifier.h"

class StoreGateSvc;
class MsgStream;
class CscIdHelper;
namespace MuonGM {
class MuonDetectorManager;
}

/////////////////////////////////////////////////////////////////////////////

class CscHashId: public AthAlgorithm {

public:

  CscHashId (const std::string& name, ISvcLocator* pSvcLocator);
  ~CscHashId();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  StatusCode cscHash();

private:

  const CscIdHelper          * m_cscId;
  const MuonGM::MuonDetectorManager  * m_muon_mgr;

};

#endif



