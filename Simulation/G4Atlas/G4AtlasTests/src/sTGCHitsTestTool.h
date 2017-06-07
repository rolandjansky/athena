/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_sTGCHITSTESTTOOL
#define G4AT_sTGCHITSTESTTOOL

#include "MuonHitTestToolBase.h"


//#include "MuonIdHelpers/MdtIdHelper.h"
//class MdtIdHelper;


class sTGCHitsTestTool : public MuonHitTestToolBase {


public:

  sTGCHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent)
    : MuonHitTestToolBase(type, name, parent)
      //m_pMdtIdHelper(0)
  {
    declareProperty("DosTGCTest", m_DosTGCTest=true);
    m_BarrelEtaCut=10.;
  }

  StatusCode initialize();

  StatusCode processEvent();

 protected:
 
 	TH2 *m_sTgc_TransverseView_TS11_posZ, *m_sTgc_TransverseView_TS21_posZ, *m_sTgc_TransverseView_TS31_posZ;
  	TH2 *m_sTgc_TransverseView_TL11_posZ, *m_sTgc_TransverseView_TL21_posZ, *m_sTgc_TransverseView_TL31_posZ;
  	TH2 *m_sTgc_rZview_positiveZ;
  // Identifier getIdentifier(HitID cschit);
//   StatusCode checkIdentifier(Identifier offid);
private:

  bool m_DosTGCTest;

  //const MdtIdHelper* m_pMdtIdHelper;

};

#endif
