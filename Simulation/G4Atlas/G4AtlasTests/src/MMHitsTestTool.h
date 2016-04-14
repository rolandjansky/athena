/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_MMHITSTESTTOOL
#define G4AT_MMHITSTESTTOOL

#include "MuonHitTestToolBase.h"


//#include "MuonIdHelpers/MdtIdHelper.h"
//class MdtIdHelper;


class MMHitsTestTool : public MuonHitTestToolBase {


public:

  MMHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent)
    : MuonHitTestToolBase(type, name, parent)
      //m_pMdtIdHelper(0)
  {
    declareProperty("DoMMTest", m_DoMMTest=true);
    m_BarrelEtaCut=1.;
  }

  StatusCode initialize();

  StatusCode processEvent();

 protected:
  // Identifier getIdentifier(HitID cschit);
//   StatusCode checkIdentifier(Identifier offid);
private:

  bool m_DoMMTest;

  //const MdtIdHelper* m_pMdtIdHelper;

};

#endif
