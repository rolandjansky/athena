/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_RPCHITSTESTTOOL
#define G4AT_RPCHITSTESTTOOL

#include "MuonHitTestToolBase.h"

//#include "MuonReadoutGeometry/MuonDetectorManager.h"
//#include "MuonIdHelpers/RpcIdHelper.h"
class RpcIdHelper;

class RPCHitsTestTool : public MuonHitTestToolBase {


public:

  RPCHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent)
    : MuonHitTestToolBase(type, name, parent),
      m_pRpcIdHelper(0)
  {
    declareProperty("DoRPCTest", m_DoRPCTest=true);
  }

  StatusCode initialize();

  StatusCode processEvent();

 protected:
  Identifier getIdentifier(HitID rpchit);
  StatusCode checkIdentifier(Identifier offid);

private:

  bool m_DoRPCTest;

  const RpcIdHelper* m_pRpcIdHelper;
};

#endif
