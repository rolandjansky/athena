/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_MMHITSTESTTOOL
#define G4AT_MMHITSTESTTOOL

#include "MuonHitTestToolBase.h"


class MMHitsTestTool : public MuonHitTestToolBase {


public:

  MMHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent)
    : MuonHitTestToolBase(type, name, parent)
  {
    declareProperty("DoMMTest", m_DoMMTest);
    m_BarrelEtaCut=10.;
  }

  virtual StatusCode initialize() override final;

  virtual StatusCode processEvent() override final;

protected:

  TH1 *m_MM_rPlot_S1_posZ = nullptr;
  TH1 *m_MM_rPlot_S2_posZ = nullptr;
  TH1 *m_MM_rPlot_L1_posZ = nullptr;
  TH1 *m_MM_rPlot_L2_posZ = nullptr;

  TH2 *m_MMTransverseEta1SmallWedge1 = nullptr;
  TH2 *m_MMTransverseEta2SmallWedge1 = nullptr;
  TH2 *m_MMTransverseEta1LargeWedge1 = nullptr;
  TH2 *m_MMTransverseEta2LargeWedge1 = nullptr;
  TH2 *m_MMTransverseEta1SmallWedge2 = nullptr;
  TH2 *m_MMTransverseEta2SmallWedge2 = nullptr;
  TH2 *m_MMTransverseEta1LargeWedge2 = nullptr;
  TH2 *m_MMTransverseEta2LargeWedge2 = nullptr;

  TH2 *m_MM_SmallWedge1_TransverseView_positiveZ = nullptr;
  TH2 *m_MM_SmallWedge1_TransverseView_negativeZ = nullptr;
  TH2 *m_MM_LargeWedge1_TransverseView_positiveZ = nullptr;
  TH2 *m_MM_LargeWedge1_TransverseView_negativeZ = nullptr;
  TH2 *m_MM_SmallWedge1_rZview_positiveZ = nullptr;
  TH2 *m_MM_SmallWedge1_rZview_negativeZ = nullptr;
  TH2 *m_MM_LargeWedge1_rZview_positiveZ = nullptr;
  TH2 *m_MM_LargeWedge1_rZview_negativeZ = nullptr;

  TH2 *m_MM_SmallWedge2_TransverseView_positiveZ = nullptr;
  TH2 *m_MM_SmallWedge2_TransverseView_negativeZ = nullptr;
  TH2 *m_MM_LargeWedge2_TransverseView_positiveZ = nullptr;
  TH2 *m_MM_LargeWedge2_TransverseView_negativeZ = nullptr;
  TH2 *m_MM_SmallWedge2_rZview_positiveZ = nullptr;
  TH2 *m_MM_SmallWedge2_rZview_negativeZ = nullptr;
  TH2 *m_MM_LargeWedge2_rZview_positiveZ = nullptr;
  TH2 *m_MM_LargeWedge2_rZview_negativeZ = nullptr;

private:

  bool m_DoMMTest = true;

};

#endif
