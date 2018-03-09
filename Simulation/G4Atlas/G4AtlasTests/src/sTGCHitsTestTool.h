/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_sTGCHITSTESTTOOL
#define G4AT_sTGCHITSTESTTOOL

#include "MuonHitTestToolBase.h"


class sTGCHitsTestTool : public MuonHitTestToolBase {


public:

  sTGCHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent)
    : MuonHitTestToolBase(type, name, parent)
  {
    declareProperty("DosTGCTest", m_DosTGCTest);
    m_BarrelEtaCut=10.;
  }

  virtual StatusCode initialize() override final;

  virtual StatusCode processEvent() override final;

 protected:

  TH2 *m_sTgc_TransverseView_TS11_posZ = nullptr;
  TH2 *m_sTgc_TransverseView_TS21_posZ = nullptr;
  TH2 *m_sTgc_TransverseView_TS31_posZ = nullptr;
  TH2 *m_sTgc_TransverseView_TL11_posZ = nullptr;
  TH2 *m_sTgc_TransverseView_TL21_posZ = nullptr;
  TH2 *m_sTgc_TransverseView_TL31_posZ = nullptr;
  TH2 *m_sTgc_rZview_positiveZ = nullptr;

private:

  bool m_DosTGCTest = true;

  //const MdtIdHelper* m_pMdtIdHelper;

};

#endif
