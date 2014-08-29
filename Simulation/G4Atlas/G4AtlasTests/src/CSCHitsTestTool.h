/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_CSCHITSTESTTOOL
#define G4AT_CSCHITSTESTTOOL

#include "MuonHitTestToolBase.h"

class CscIdHelper;

class CSCHitsTestTool : public MuonHitTestToolBase {


public:
  
 CSCHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent) :
    MuonHitTestToolBase(type, name, parent),
    m_pCscIdHelper(0)
  {
    declareProperty("DoCSCTest",  m_DoCSCTest=true);
  }

    StatusCode initialize();

    //  StatusCode bookHistograms();

  StatusCode processEvent();
 protected:
  Identifier getIdentifier(HitID);
  StatusCode checkIdentifier(Identifier);
private:

  bool m_DoCSCTest;

  const CscIdHelper* m_pCscIdHelper;

};

#endif
