/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_TGCHITSTESTTOOL
#define G4AT_TGCHITSTESTTOOL

#include "MuonHitTestToolBase.h"

//#include "MuonReadoutGeometry/MuonDetectorManager.h"
//#include "MuonIdHelpers/TgcIdHelper.h"
class TgcIdHelper;


class TGCHitsTestTool : public MuonHitTestToolBase {


public:

  TGCHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent)
    : MuonHitTestToolBase(type, name, parent),
    m_pTgcIdHelper(0)
  {
    declareProperty("DoTGCTest", m_DoTGCTest=true);
  }

  StatusCode initialize();

  StatusCode processEvent();

 protected:
  Identifier getIdentifier(HitID);
  StatusCode checkIdentifier(Identifier);


private:

  bool m_DoTGCTest;

  const TgcIdHelper* m_pTgcIdHelper;

};

#endif
