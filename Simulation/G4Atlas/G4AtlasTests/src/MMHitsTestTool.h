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
    m_BarrelEtaCut=10.;
  }

  StatusCode initialize();

  StatusCode processEvent();

protected:

  TH1 *m_MM_rPlot_S1_posZ, *m_MM_rPlot_S2_posZ, *m_MM_rPlot_L1_posZ, *m_MM_rPlot_L2_posZ;

  TH2 *m_MMTransverseEta1SmallWedge1, *m_MMTransverseEta2SmallWedge1, *m_MMTransverseEta1LargeWedge1;
  TH2 *m_MMTransverseEta2LargeWedge1, *m_MMTransverseEta1SmallWedge2, *m_MMTransverseEta2SmallWedge2;
  TH2 *m_MMTransverseEta1LargeWedge2, *m_MMTransverseEta2LargeWedge2;

  TH2 *m_MM_SmallWedge1_TransverseView_positiveZ, *m_MM_SmallWedge1_TransverseView_negativeZ;
  TH2 *m_MM_LargeWedge1_TransverseView_positiveZ, *m_MM_LargeWedge1_TransverseView_negativeZ;
  TH2 *m_MM_SmallWedge1_rZview_positiveZ, *m_MM_SmallWedge1_rZview_negativeZ;
  TH2 *m_MM_LargeWedge1_rZview_positiveZ, *m_MM_LargeWedge1_rZview_negativeZ;

  TH2 *m_MM_SmallWedge2_TransverseView_positiveZ, *m_MM_SmallWedge2_TransverseView_negativeZ;
  TH2 *m_MM_LargeWedge2_TransverseView_positiveZ, *m_MM_LargeWedge2_TransverseView_negativeZ;
  TH2 *m_MM_SmallWedge2_rZview_positiveZ, *m_MM_SmallWedge2_rZview_negativeZ;
  TH2 *m_MM_LargeWedge2_rZview_positiveZ, *m_MM_LargeWedge2_rZview_negativeZ;

  // Identifier getIdentifier(HitID cschit);
  //   StatusCode checkIdentifier(Identifier offid);

private:

  bool m_DoMMTest;

  //const MdtIdHelper* m_pMdtIdHelper;

};

#endif
