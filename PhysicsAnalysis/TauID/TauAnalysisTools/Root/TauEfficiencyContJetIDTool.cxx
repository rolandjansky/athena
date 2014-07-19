/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef DEVEL
#ifndef TAUEFFICIENCYCONTJETBDTTOOL_CXX
#define TAUEFFICIENCYCONTJETBDTTOOL_CXX
#include "TauAnalysisTools/TauEfficiencyContJetIDTool.h"

#include <iostream>
#include <algorithm>
#include <iterator>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <string>

#include "TDirectory.h"
#include "TParameter.h"
#include "TKey.h"

using namespace TauAnalysisTools;

//==================================PUBLIC-PART=================================
//______________________________________________________________________________
TauEfficiencyContJetIDTool::TauEfficiencyContJetIDTool(std::string sName, PropertyMgr* pPropertyMgr) :
  CommonEfficiencyTool(sName, pPropertyMgr)
{
  configure(pPropertyMgr);
}

void TauEfficiencyContJetIDTool::configure(PropertyMgr* pPropertyMgr)
{
  
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyContJetIDTool::getEfficiencyScaleFactor(const xAOD::TauJet& xTau,
										      double& dEfficiencyScaleFactor)
{
  // obtain ID SF value
  if (m_bUseInclusiveEta)
    return getBDTbinnedSF(dEfficiencyScaleFactor, xTau.numTrack(), xTau.eta(), xTau.discriminant(xAOD::TauJetParameters::TauID(15)));
  // else
  return getBDTbinnedSF(dEfficiencyScaleFactor, xTau.numTrack(), xTau.discriminant(xAOD::TauJetParameters::TauID(15)));
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyContJetIDTool::applyEfficiencyScaleFactor(xAOD::TauJet& xTau)
{
  double sf = 0.;
  // retreive scale factor
  TauAnalysisTools::CorrectionCode tmpCorrectionCode = getEfficiencyScaleFactor(xTau, sf);
  // adding scale factor to auxdata
  xTau.auxdata< double >( "TauScaleFactorContJetID" ) = sf;
  
  return tmpCorrectionCode;
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyContJetIDTool::getEfficiencyScaleFactorStatUnc(const xAOD::TauJet& xTau, double& dEfficiencyScaleFactorStatUnc)
{
  if (m_bUseInclusiveEta)
    return getBDTbinnedSFStatUnc(dEfficiencyScaleFactorStatUnc, xTau.numTrack(), xTau.eta(), xTau.discriminant(xAOD::TauJetParameters::TauID(15)));
  else
    return getBDTbinnedSFStatUnc(dEfficiencyScaleFactorStatUnc, xTau.numTrack(), xTau.discriminant(xAOD::TauJetParameters::TauID(15)));
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyContJetIDTool::applyEfficiencyScaleFactorStatUnc(xAOD::TauJet& xTau)
{
  double dStatUnc = 0.;
  TauAnalysisTools::CorrectionCode tmpCorrectionCodeStat = getEfficiencyScaleFactorStatUnc(xTau, dStatUnc, eSysDirection, iSysType);

  // adding scale factor to auxdata
  std::string sVarName = "TauScaleFactorContJetIDStatUnc";
  eSysDirection == UP ? sVarName += "_UP" : sVarName += "_DOWN";

  xTau.auxdata< double >( sVarName ) = dStatUnc;
  
  return tmpCorrectionCodeStat;
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyContJetIDTool::getEfficiencyScaleFactorSysUnc(const xAOD::TauJet& xTau, double& dEfficiencyScaleFactorSysUnc)
{
  if (m_bUseInclusiveEta)
    return getBDTbinnedSFSysUnc(dEfficiencyScaleFactorSysUnc, xTau.numTrack(), xTau.eta(), xTau.discriminant(xAOD::TauJetParameters::TauID(15)));
  else
    return getBDTbinnedSFSysUnc(dEfficiencyScaleFactorSysUnc, xTau.numTrack(), xTau.discriminant(xAOD::TauJetParameters::TauID(15)), eSysDirection, CONTIDSYS(iSysType));
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyContJetIDTool::applyEfficiencyScaleFactorSysUnc(xAOD::TauJet& xTau)
{
  double dSysUnc = 0.;
  TauAnalysisTools::CorrectionCode tmpCorrectionCodeSys = getEfficiencyScaleFactorSysUnc(xTau, dSysUnc, eSysDirection, iSysType);

  // adding scale factor to auxdata
  std::string sVarName = "TauScaleFactorContJetIDSysUnc";
  m_iSysDirection == 1 ? sVarName += "_UP" : sVarName += "_DOWN";

  xTau.auxdata< double >( sVarName ) = dSysUnc;
  
  return tmpCorrectionCodeSys;
}

//=================================PRIVATE-PART=================================
// Central Value
//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyContJetIDTool::getBDTbinnedSF(double& val,
									    int prongness,
									    const double& BDT)
{
  std::string workingPoint = "ID_SF" + ConvertProngToString(prongness);
  return this->GetIDValue(val, workingPoint, BDT);
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyContJetIDTool::getBDTbinnedSF(double& val,
									    int prongness,
									    const double& eta,
									    const double& BDT)
{
  std::string workingPoint = "ID_SF" + ConvertProngToString(prongness) + ConvertEtaToString(eta);
  return this->GetIDValue(val, workingPoint, BDT);
}

// Systematic Uncertainty
//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyContJetIDTool::getBDTbinnedSFSysUnc(double& val,
										  int prongness,
										  const double& BDT)
{
  std::string workingPoint = "ID_SF_sys" + ConvertSystematicToString(m_eContSysType) + ConvertDirectionToString(m_iSysDirection) + ConvertProngToString(prongness);
  return this->GetIDValue(val, workingPoint, BDT);
}


//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyContJetIDTool::getBDTbinnedSFSysUnc(double& val,
										  int prongness,
										  const double& eta,
										  const double& BDT)
{
  std::string workingPoint = "ID_SF_sys" + ConvertSystematicToString(m_eContSysType) + ConvertDirectionToString(m_iSysDirection) + ConvertProngToString(prongness) + ConvertEtaToString(eta);
  return this->GetIDValue(val, workingPoint, BDT);
}

// Statistical Uncertainty
//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyContJetIDTool::getBDTbinnedSFStatUnc(double& val,
										   int prongness,
										   const double& BDT)
{
  std::string workingPoint = "ID_SF_stat" + ConvertDirectionToString(m_iSysDirection) + ConvertProngToString(prongness);
  return this->GetIDValue(val, workingPoint, BDT);
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyContJetIDTool::getBDTbinnedSFStatUnc(double& val,
										   int prongness,
										   const double& eta,
										   const double& BDT)
{

  std::string workingPoint = "ID_SF_stat" + ConvertDirectionToString(m_iSysDirection) + ConvertProngToString(prongness) + ConvertEtaToString(eta);
  return this->GetIDValue(val, workingPoint, BDT);
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyContJetIDTool::GetIDValue(double& val,
									const std::string& sWorkingPoint,
									const float& fBDT){
  ATH_MSG_VERBOSE("Try to access histogram: " << sWorkingPoint);
  if (fBDT < 0.4 || fBDT > 1.0){
    ATH_MSG_WARNING("BDT Value out of bounds: "<<fBDT<<" (SFs only valid between 0.4 and 1.0). Returning a SF of 1.0");
    val = 1.0;
    return TauAnalysisTools::CorrectionCode::OutOfValidityRange;
  }
  TH1F* tmp = (*m_mSF)[sWorkingPoint];
  assert(tmp);
  int iBin = tmp->FindBin(fBDT);
  //check overflow:
  if(iBin == tmp->GetNbinsX() + 1)
    iBin -= 1;
  val = tmp->GetBinContent(iBin);
  return TauAnalysisTools::CorrectionCode::OK;
}

// Return Histogram Names
//______________________________________________________________________________
std::string TauEfficiencyContJetIDTool::ConvertDirectionToString(const int direction){
  if(direction == 1)
    return "_UP";
  else if (direction == -1)
    return "_DOWN";
  else
    assert(false && "Statistical/Systematic direction must be UP or DOWN. Breaking up ...");
}

//______________________________________________________________________________
std::string TauEfficiencyContJetIDTool::ConvertEtaToString(const float& fEta){
  if(m_bUseInclusiveEta)
    return "_etaAll";
  if(fabs(fEta) <= 1.5)
    return "_barrel";
  else if(fabs(fEta) > 1.5)
    return "_endcap";
  else
    assert(false && "Invalid eta provided. Breaking up ...");
}


//______________________________________________________________________________
std::string TauEfficiencyContJetIDTool::ConvertSystematicToString(CONTIDSYS sys)
{
  switch(sys)
    {
    case TOTAL:
      return "";
      break;
    case QCD:
      return "_QCD";
      break;
    case WJETS:
      return "_Wjets";
      break;
    case TES:
      return "_TES";
      break;
    case REMAIN:
      return "_Other";
      break;
    default:
      std::cerr << "No valid systematic uncertainty component passed. Breaking up";
      assert(false);
      break;
    }
}

#endif // TAUEFFICIENCYCONTJETBDTTOOL_CXX
#endif
