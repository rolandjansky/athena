/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Test SCT_DistortionsTool.cxx

//Package
#include "SCT_TestDistortionsTool.h"

//Gaudi
#include "GaudiKernel/ITHistSvc.h"

SCT_TestDistortionsTool::SCT_TestDistortionsTool(const std::string& name, ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator)
{
  //nop
}

StatusCode
SCT_TestDistortionsTool::initialize() {
  ITHistSvc* tHistSvc;
  ATH_CHECK(Gaudi::svcLocator()->service("THistSvc", tHistSvc));

  ATH_CHECK(m_SCTDistoTool.retrieve());
  ATH_MSG_INFO("Test algorithm for SCT_DistortionsTool");

  m_ZvsX = new TH2F("delZvsX", "delZvsX", 128, -64, 64, 100, -0.05, 0.15);
  ATH_CHECK(tHistSvc->regHist("/SCT_TestDistortionsTool/delZvsX", m_ZvsX));
  m_ZvsY = new TH2F("delZvsY", "delZvsY", 66, -33, 33, 100, -0.05, 0.15);
  ATH_CHECK(tHistSvc->regHist("/SCT_TestDistortionsTool/delZvsY", m_ZvsY));
  m_XYZ = new TH3F("3Dplot", "3Dplot", 128, -64, 64, 66, -33, 33, 100, -0.05, 0.15);
  ATH_CHECK(tHistSvc->regHist("/SCT_TestDistortionsTool/3Dplot", m_XYZ));
  m_outerXedge = new TH2F("outerXedge", "outerXedge", 128, -64, 64, 100, -0.05, 0.15);
  ATH_CHECK(tHistSvc->regHist("/SCT_TestDistortionsTool/outerXedge", m_outerXedge));
  m_outerYedge = new TH2F("outerYedge", "outerYedge", 66, -33, 33, 100, -0.05, 0.15);
  ATH_CHECK(tHistSvc->regHist("/SCT_TestDistortionsTool/outerYedge", m_outerYedge));
  m_outerX = new TH2F("outerXedge2D", "outerXedge2D", 128, -64, 64, 100, -0.05, 0.15);
  ATH_CHECK(tHistSvc->regHist("/SCT_TestDistortionsTool/outerXedge2D", m_outerX));
  m_outerY = new TH2F("outerYedge2D", "outerYedge2D", 66, -33, 33, 100, -0.05, 0.15);
  ATH_CHECK(tHistSvc->regHist("/SCT_TestDistortionsTool/outerYedge2D", m_outerY));

  return StatusCode::SUCCESS;
}

StatusCode
SCT_TestDistortionsTool::execute() {
  IdentifierHash hash{4744};
  ATH_MSG_INFO(" Hash " << hash << " from region: " << m_SCTDistoTool->identifyRegion(hash));
  ATH_MSG_INFO(" ******************************************* ");
  ATH_MSG_INFO(" **           Working so far              ** ");
  ATH_MSG_INFO(" ******************************************* ");
  int side{0};
  int region{m_SCTDistoTool->identifyRegion(hash)};
  const std::vector<float>* zVec{m_SCTDistoTool->readDistortions(region, side)};
  ATH_MSG_INFO(" zVec.size() = " << zVec->size());
  ATH_MSG_INFO(" **           Working so far              ** ");

  int k{0};
  float zData[50];
  for (const float z: *zVec) {
    zData[k] = z;
    k++;
  }

  ATH_MSG_INFO(" **           Working so far              ** ");
  ATH_MSG_INFO(" zData[0] = " << zData[0]);
  ATH_MSG_INFO(" zData[24] = " << zData[24]);
  ATH_MSG_INFO(" zData[25] = " << zData[25]);
  ATH_MSG_INFO(" zData[49] = " << zData[49]);
  ATH_MSG_INFO(" **           Working so far              ** ");

  float xGrid[10];
  float yGrid[5];
  xGrid[0] = -62.8; xGrid[1] = -47.5; xGrid[2] = -32.2; xGrid[3] = -16.8; xGrid[4] = -1.84;
  xGrid[5] = 1.41;  xGrid[6] = 16.7;  xGrid[7] = 32.1;  xGrid[8] = 47.5;  xGrid[9] = 62.6;
  yGrid[0] = -31.33835;  yGrid[1] = -15.67632;  yGrid[2] = -0.01504;  yGrid[3] = 15.64652;  yGrid[4] = 31.30763;

  for (int i{0}; i<10; i++) {
    double y1{-30.4};
    double x1{xGrid[i]};
    double z1{m_SCTDistoTool->zShift(x1, y1, zVec)};
    ATH_MSG_INFO(" x = " << x1 << " y = " << y1 << " z = "<< z1);
    m_outerXedge->Fill(x1, z1);
  }
  for (int i{0}; i<5; i++) {
    double y1{yGrid[i]};
    double x1{-61.9};
    double z1{m_SCTDistoTool->zShift(x1, y1, zVec)};
    ATH_MSG_INFO(" x = " << x1 << " y = " << y1 << " z = " << z1);
    m_outerYedge->Fill(y1, z1);
  }

  for (double x{-630.}; x<630; x++) {
    for (double y{-320.}; y<320; y++) {
      double z{m_SCTDistoTool->zShift(x/10., y/10., zVec)};
      m_ZvsX->Fill(x/10., z);
      m_ZvsY->Fill(y/10., z);
      m_XYZ->Fill(x/10., y/10., z);

      if (x/10. < -62.8 or x/10. > 62.6) m_outerY->Fill(y/10., z);
      if (y/10. < -31.4 or y/10. > 31.4) m_outerX->Fill(x/10., z);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode
SCT_TestDistortionsTool::finalize() {
  ATH_MSG_INFO("finalize()");
  return StatusCode::SUCCESS;
}
