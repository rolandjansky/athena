/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JetCalibTools/CalibrationMethods/BcidOffsetCorrection.h"
#include "PathResolver/PathResolver.h"

BcidOffsetCorrection::BcidOffsetCorrection()
  : JetCalibrationToolBase( "BcidOffsetCorrection::BcidOffsetCorrection" ),
    m_config(nullptr), m_jetAlgo(""), m_calibAreaTag(""), m_isData(false)//,
{ }

BcidOffsetCorrection::BcidOffsetCorrection(const std::string& name)
  : JetCalibrationToolBase( name ),
    m_config(nullptr), m_jetAlgo(""), m_calibAreaTag(""), m_isData(false)//,
{ }

BcidOffsetCorrection::BcidOffsetCorrection(const std::string& name, TEnv * config, TString jetAlgo, TString calibAreaTag, bool isData, bool /*dev*/)
  : JetCalibrationToolBase( name ),
    m_config(config), m_jetAlgo(jetAlgo), m_calibAreaTag(calibAreaTag), m_isData(isData)//,
{ }

BcidOffsetCorrection::~BcidOffsetCorrection() {

}

StatusCode BcidOffsetCorrection::initializeTool(const std::string&) {

  m_doEMECIW2bcid = m_config->GetValue("ApplyEMECIW2bcid",true);
  m_doEMECIW3bcid = m_config->GetValue("ApplyEMECIW3bcid",false);
  m_doHEC0bcid = m_config->GetValue("ApplyHEC0bcid",true);
  m_doHEC1bcid = m_config->GetValue("ApplyHEC1bcid",true);
  m_doHEC2bcid = m_config->GetValue("ApplyHEC2bcid",false);
  m_doHEC3bcid = m_config->GetValue("ApplyHEC3bcid",false);
  m_doFCal0OuterBcid = m_config->GetValue("ApplyFCal0OuterBcid",false);
  m_doFCal1OuterBcid = m_config->GetValue("ApplyFCal1OuterBcid",false);
  m_doFCal2OuterBcid = m_config->GetValue("ApplyFCal2OuterBcid",false);
  m_doFCal0bcid = m_config->GetValue("ApplyFCal0bcid",true);
  m_doFCal1bcid = m_config->GetValue("ApplyFCal1bcid",false);
  m_doFCal2bcid = m_config->GetValue("ApplyFCal2bcid",false);
  m_doFCal0InnerBcid = m_config->GetValue("ApplyFCal0InnerBcid",false);
  m_doFCal1InnerBcid = m_config->GetValue("ApplyFCal1InnerBcid",false);
  m_doFCal2InnerBcid = m_config->GetValue("ApplyFCal2InnerBcid",false);

  if(m_doEMECIW3bcid || m_doHEC2bcid || m_doHEC3bcid){
    ATH_MSG_FATAL("You are attempting to apply the BCID offset correction to the EMEC3 or HEC back wheel cells which is not implemented.");
    return StatusCode::FAILURE;
  }
  if(m_doFCal0OuterBcid || m_doFCal1OuterBcid || m_doFCal2OuterBcid){
    ATH_MSG_FATAL("You are attempting to apply the BCID offset correction to the outer small FCal cells which is not implemented.");
    return StatusCode::FAILURE;
  }
  if(m_doFCal1bcid || m_doFCal2bcid){
    ATH_MSG_FATAL("You are attempting to apply the BCID offset correction to the large FCal1/2 cells which is not implemented.");
    return StatusCode::FAILURE;
  }
  if(m_doFCal0InnerBcid || m_doFCal1InnerBcid || m_doFCal2InnerBcid){
    ATH_MSG_FATAL("You are attempting to apply the BCID offset correction to the inner small FCal cells which is not implemented.");
    return StatusCode::FAILURE;
  }

  m_jetStartScale = m_config->GetValue("BcidStartingScale","JetConstitScaleMomentum");

  //find the ROOT file containing the BCID slopes and numbers of cells.
  TString BCIDFile = m_config->GetValue("BCIDOffsetFile","");
  if ( BCIDFile.EqualTo("empty") ) { 
    ATH_MSG_FATAL("No BCIDOffsetFile specified. Aborting.");
    return StatusCode::FAILURE;
  }
  BCIDFile.Insert(14,m_calibAreaTag);
  TString fileName = PathResolverFindCalibFile(BCIDFile.Data());
  std::unique_ptr<TFile> inputFile(TFile::Open(fileName));
  if (!inputFile){
    ATH_MSG_FATAL("Cannot open BCID offset calibration file" << fileName);
    return StatusCode::FAILURE;
  }

  m_bcid_nCells_EMECIW2 = JetCalibUtils::GetHisto2(*inputFile,"bcid_nCells_EMECIW2");
  m_bcid_nCells_HEC0 = JetCalibUtils::GetHisto2(*inputFile,"bcid_nCells_HEC0");
  m_bcid_nCells_HEC1 = JetCalibUtils::GetHisto2(*inputFile,"bcid_nCells_HEC1");
  m_bcid_nCells_FCal0 = JetCalibUtils::GetHisto2(*inputFile,"bcid_nCells_FCal0big");

  m_bcid_slope_EMECIW2_pos = JetCalibUtils::GetHisto2(*inputFile,"bcid_slope_EMECIW2_pos");
  m_bcid_slope_EMECIW2_neg = JetCalibUtils::GetHisto2(*inputFile,"bcid_slope_EMECIW2_neg");
  m_bcid_slope_HEC0_pos = JetCalibUtils::GetHisto2(*inputFile,"bcid_slope_HEC0_pos");
  m_bcid_slope_HEC0_neg = JetCalibUtils::GetHisto2(*inputFile,"bcid_slope_HEC0_neg");
  m_bcid_slope_HEC1_pos = JetCalibUtils::GetHisto2(*inputFile,"bcid_slope_HEC1_pos");
  m_bcid_slope_HEC1_neg = JetCalibUtils::GetHisto2(*inputFile,"bcid_slope_HEC1_neg");
  m_bcid_slope_FCal0 = JetCalibUtils::GetHisto2(*inputFile,"bcid_slope_FCal0big");

  m_bcid_slope_EMECIW2_8b4e_pos = JetCalibUtils::GetHisto2(*inputFile,"bcid_slope_EMECIW2_8b4e_pos");
  m_bcid_slope_EMECIW2_8b4e_neg = JetCalibUtils::GetHisto2(*inputFile,"bcid_slope_EMECIW2_8b4e_neg");
  m_bcid_slope_HEC0_8b4e_pos = JetCalibUtils::GetHisto2(*inputFile,"bcid_slope_HEC0_8b4e_pos");
  m_bcid_slope_HEC0_8b4e_neg = JetCalibUtils::GetHisto2(*inputFile,"bcid_slope_HEC0_8b4e_neg");
  m_bcid_slope_HEC1_8b4e_pos = JetCalibUtils::GetHisto2(*inputFile,"bcid_slope_HEC1_8b4e_pos");
  m_bcid_slope_HEC1_8b4e_neg = JetCalibUtils::GetHisto2(*inputFile,"bcid_slope_HEC1_8b4e_neg");
  m_bcid_slope_FCal0_8b4e = JetCalibUtils::GetHisto2(*inputFile,"bcid_slope_FCal0big_8b4e");

  return StatusCode::SUCCESS;
}

StatusCode BcidOffsetCorrection::calibrateImpl(xAOD::Jet& jet, JetEventInfo& jetEventInfo) const {

  // correction should only be applied to data
  if (!m_isData){
    jet.setAttribute<xAOD::JetFourMom_t>("JetBcidScaleMomentum",jet.jetP4());
    return StatusCode::SUCCESS;
  }

  xAOD::JetFourMom_t jetStartP4;
  ATH_CHECK( setStartP4(jet) );
  jetStartP4 = jet.jetP4();

  float startPt=jet.pt();
  float calibPt=jet.pt();

  // if we are not looking at 2017 or 2018 data then apply no correction and pass on the new jet name
  if (jetEventInfo.runNumber()>358656 || jetEventInfo.runNumber()<325713){
    jet.setAttribute<xAOD::JetFourMom_t>("JetBcidScaleMomentum",jetStartP4);
    return StatusCode::SUCCESS;
  }

  bool is8b4e=false;
  if (jetEventInfo.runNumber()>=334842 && jetEventInfo.runNumber()<=340453) is8b4e=true;

  int bcidDistanceFromFront = jetEventInfo.bcidDistanceFromFront();
  int bcidGapBeforeTrain = jetEventInfo.bcidGapBeforeTrain();
  int bcidGapBeforeTrainMinus12 = jetEventInfo.bcidGapBeforeTrainMinus12();
  if (!is8b4e){// this separates the sub-trains
    if (bcidGapBeforeTrain<=15) bcidDistanceFromFront+=55;
  }
  if (is8b4e){// this separates sub-trains and the 1st and 2nd sets of 8b4e
    if (bcidGapBeforeTrain==5 && bcidGapBeforeTrainMinus12==5) bcidDistanceFromFront+=24;
    if (bcidGapBeforeTrain>5 && bcidGapBeforeTrain<20) bcidDistanceFromFront+=60;
    if (bcidGapBeforeTrain==5 && bcidGapBeforeTrainMinus12>5 && bcidGapBeforeTrainMinus12<20) bcidDistanceFromFront+=72;
    if (bcidGapBeforeTrain==5 && bcidGapBeforeTrainMinus12>20) bcidDistanceFromFront+=12;
  }

  xAOD::JetFourMom_t calibP4;
  calibP4 = jet.jetP4();
  float detEta=jet.auxdata<float>("DetectorEta");
  float mu=jetEventInfo.mu();

  if (m_doEMECIW2bcid && !is8b4e){
    if ((bcidDistanceFromFront>=0 && bcidDistanceFromFront<12) || (bcidDistanceFromFront>=55 && bcidDistanceFromFront<67)){// only apply correction to 1st 12 BCIDs
      for (unsigned int i=1; i<=7; i++){
        float nCells = m_bcid_nCells_EMECIW2->GetBinContent(i,static_cast<int>(abs(detEta)*10.0+1.0));
        float slope = 0;
        if (detEta>0) slope = m_bcid_slope_EMECIW2_pos->GetBinContent(i,bcidDistanceFromFront+1);
        if (detEta<0) slope = m_bcid_slope_EMECIW2_neg->GetBinContent(i,bcidDistanceFromFront+1);
        calibPt -= mu*nCells*slope;
      }
    }
  }
  if (m_doEMECIW2bcid && is8b4e){
    if (bcidDistanceFromFront>=0 && bcidDistanceFromFront<115){// apply to all BCIDs
      for (unsigned int i=1; i<=7; i++){
        float nCells = m_bcid_nCells_EMECIW2->GetBinContent(i,static_cast<int>(abs(detEta)*10.0+1.0));
        float slope = 0;
        if (detEta>0) slope = m_bcid_slope_EMECIW2_8b4e_pos->GetBinContent(i,bcidDistanceFromFront+1);
        if (detEta<0) slope = m_bcid_slope_EMECIW2_8b4e_neg->GetBinContent(i,bcidDistanceFromFront+1);
        calibPt -= mu*nCells*slope;
      }
    }
  }
  if (m_doHEC0bcid && !is8b4e){
    if ((bcidDistanceFromFront>=0 && bcidDistanceFromFront<10) || (bcidDistanceFromFront>=55 && bcidDistanceFromFront<65) || bcidDistanceFromFront==47 || bcidDistanceFromFront==102){// only apply correction to 1st 10 and last BCIDs
      for (unsigned int i=1; i<=4; i++){
        float nCells = m_bcid_nCells_HEC0->GetBinContent(i,static_cast<int>(abs(detEta)*10.0+1.0));
        float slope = 0;
        if (detEta>0) slope = m_bcid_slope_HEC0_pos->GetBinContent(i,bcidDistanceFromFront+1);
        if (detEta<0) slope = m_bcid_slope_HEC0_neg->GetBinContent(i,bcidDistanceFromFront+1);
        calibPt -= mu*nCells*slope;
      }
    }
  }
  if (m_doHEC0bcid && is8b4e){
    if (bcidDistanceFromFront>=0 && bcidDistanceFromFront<115){// apply to all BCIDs
      for (unsigned int i=1; i<=4; i++){
        float nCells = m_bcid_nCells_HEC0->GetBinContent(i,static_cast<int>(abs(detEta)*10.0+1.0));
        float slope = 0;
        if (detEta>0) slope = m_bcid_slope_HEC0_8b4e_pos->GetBinContent(i,bcidDistanceFromFront+1);
        if (detEta<0) slope = m_bcid_slope_HEC0_8b4e_neg->GetBinContent(i,bcidDistanceFromFront+1);
        calibPt -= mu*nCells*slope;
      }
    }
  }
  if (m_doHEC1bcid && !is8b4e){
    if ((bcidDistanceFromFront>=0 && bcidDistanceFromFront<10) || (bcidDistanceFromFront>=55 && bcidDistanceFromFront<65) || bcidDistanceFromFront==47 || bcidDistanceFromFront==102){// only apply correction to 1st 10 and last BCIDs
      for (unsigned int i=1; i<=3; i++){
        float nCells = m_bcid_nCells_HEC1->GetBinContent(i,static_cast<int>(abs(detEta)*10.0+1.0));
        float slope = 0;
        if (detEta>0) slope = m_bcid_slope_HEC1_pos->GetBinContent(i,bcidDistanceFromFront+1);
        if (detEta<0) slope = m_bcid_slope_HEC1_neg->GetBinContent(i,bcidDistanceFromFront+1);
        calibPt -= mu*nCells*slope;
      }
    }
  }
  if (m_doHEC1bcid && is8b4e){
    if (bcidDistanceFromFront>=0 && bcidDistanceFromFront<115){// apply to all BCIDs
      for (unsigned int i=1; i<=3; i++){
        float nCells = m_bcid_nCells_HEC1->GetBinContent(i,static_cast<int>(abs(detEta)*10.0+1.0));
        float slope = 0;
        if (detEta>0) slope = m_bcid_slope_HEC1_8b4e_pos->GetBinContent(i,bcidDistanceFromFront+1);
        if (detEta<0) slope = m_bcid_slope_HEC1_8b4e_neg->GetBinContent(i,bcidDistanceFromFront+1);
        calibPt -= mu*nCells*slope;
      }
    }
  }
  if (m_doFCal0bcid && !is8b4e){
    if ((bcidDistanceFromFront>=0 && bcidDistanceFromFront<12) || (bcidDistanceFromFront>=55 && bcidDistanceFromFront<67)){// only apply correction to 1st 12 BCIDs
      for (unsigned int i=1; i<=6; i++){
        float nCells = m_bcid_nCells_FCal0->GetBinContent(i,static_cast<int>(abs(detEta)*10.0+1.0));
        float slope = 0;
        slope = m_bcid_slope_FCal0->GetBinContent(i,bcidDistanceFromFront+1);
        calibPt -= mu*nCells*slope;
      }
    }
  }
  if (m_doFCal0bcid && is8b4e){
    if (bcidDistanceFromFront>=0 && bcidDistanceFromFront<115){// apply to all BCIDs
      for (unsigned int i=1; i<=6; i++){
        float nCells = m_bcid_nCells_FCal0->GetBinContent(i,static_cast<int>(abs(detEta)*10.0+1.0));
        float slope = 0;
        slope = m_bcid_slope_FCal0_8b4e->GetBinContent(i,bcidDistanceFromFront+1);
        calibPt -= mu*nCells*slope;
      }
    }
  }

  calibP4*=calibPt/startPt;

  //Transfer calibrated jet properties to the Jet object
  jet.setAttribute<xAOD::JetFourMom_t>("JetBcidScaleMomentum",calibP4);
  jet.setJetP4( calibP4 );

  return StatusCode::SUCCESS;
}

