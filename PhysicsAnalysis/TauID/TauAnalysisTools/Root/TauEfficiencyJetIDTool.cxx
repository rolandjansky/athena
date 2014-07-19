/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUEFFICIENCYTOOL_CXX
#define TAUEFFICIENCYTOOL_CXX

// #include "TauAnalysisTools/Enums.h"
#include "TauAnalysisTools/TauEfficiencyJetIDTool.h"

#include <iostream>
#include <algorithm>
#include <iterator>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "TDirectory.h"
#include "TParameter.h"
#include "TKey.h"

using namespace TauAnalysisTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
TauEfficiencyJetIDTool::TauEfficiencyJetIDTool(std::string sName) :
  CommonEfficiencyTool(sName)
{
}

//______________________________________________________________________________
StatusCode TauEfficiencyJetIDTool::configure(){
  ATH_MSG_WARNING("Nothing to configure, use StatusCode TauEfficiencyJetIDTool::configure(PropertyMgr* pPropertyMgr) instead");
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauEfficiencyJetIDTool::configure(PropertyMgr* pPropertyMgr)
{
#ifdef ASGTOOL_ATHENA
  assert(false && pPropertyMgr && "PropertyMgr->getProperty(std::string s, T& val); not working within athena");
#else
  pPropertyMgr->getProperty("UseIDExclusiveSF", m_bUseIDExclusiveSF);
  pPropertyMgr->getProperty("UseInclusiveEta", m_bUseInclusiveEta);
  pPropertyMgr->getProperty("UsePtBinnedSF", m_bUsePtBinnedSF);
  pPropertyMgr->getProperty("UseHighPtUncert", m_bUseHighPtUncert);
  pPropertyMgr->getProperty("IDLevel", (int&)m_iIDLevel);
#endif
  m_sIDLevel = ConvertIDToString(m_iIDLevel);
  SwitchOnPtBinnedIDSF(m_bUseHighPtUncert);
  return CommonEfficiencyTool::configure(pPropertyMgr);
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyJetIDTool::getEfficiencyScaleFactor(const xAOD::TauJet& xTau,
										  double& dEfficiencyScaleFactor)
{
  // retreive ID SF value
  if (m_bUseIDExclusiveSF)
    return GetExclIDSF(dEfficiencyScaleFactor, xTau.eta(), xTau.nTracks(), xTau.pt());
  // else
  return GetIDSF(dEfficiencyScaleFactor, xTau.eta(), xTau.nTracks(), xTau.pt());
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyJetIDTool::applyEfficiencyScaleFactor(xAOD::TauJet& xTau)
{
  double dSf = 0.;
  // retreive scale factor
  TauAnalysisTools::CorrectionCode tmpCorrectionCode = getEfficiencyScaleFactor(xTau, dSf);
  // adding scale factor to auxdata
  xTau.auxdata< double >( "TauScaleFactorJetID" ) = dSf;
  
  return tmpCorrectionCode;
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyJetIDTool::getEfficiencyScaleFactorStatUnc(const xAOD::TauJet& xTau, double& dStatUnc)
{
  if (m_bUseIDExclusiveSF)
    return GetExclIDStatUnc(dStatUnc, xTau.eta(), xTau.nTracks(), xTau.pt());
  else
    return GetIDStatUnc(dStatUnc, xTau.eta(), xTau.nTracks(), xTau.pt());
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyJetIDTool::applyEfficiencyScaleFactorStatUnc(xAOD::TauJet& xTau)
{
  double dStatUnc = 0;
  // retreive ID SF value
  TauAnalysisTools::CorrectionCode tmpCorrectionCode = getEfficiencyScaleFactorStatUnc(xTau, dStatUnc);

  std::string sVarName = "TauScaleFactorJetIDStatUnc";
  m_iSysDirection == 1 ? sVarName += "_Up" : sVarName += "_Down";

  // adding stat unc to auxdata
  xTau.auxdata< double >( sVarName ) = dStatUnc;

  return tmpCorrectionCode;
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyJetIDTool::getEfficiencyScaleFactorSysUnc(const xAOD::TauJet& xTau, double& dSysUnc)
{
  TauAnalysisTools::CorrectionCode tmpCorrectionCode;
  if (m_bUseIDExclusiveSF)
    return GetExclIDSysUnc(dSysUnc, xTau.eta(), xTau.nTracks(), xTau.pt());
  else
    return GetIDSysUnc(dSysUnc, xTau.eta(), xTau.nTracks(), xTau.pt());
  return tmpCorrectionCode;
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyJetIDTool::applyEfficiencyScaleFactorSysUnc(xAOD::TauJet& xTau)
{
  double dSysUnc = 0.;
  // retreive ID SF value
  TauAnalysisTools::CorrectionCode tmpCorrectionCode = getEfficiencyScaleFactorSysUnc(xTau, dSysUnc);
  // adding sys unc to auxdata
  std::string sVarName = "TauScaleFactorJetIDSysUnc";
  m_iSysDirection == 1 ? sVarName += "_Up" : sVarName += "_Down";

  xTau.auxdata< double >( sVarName ) = dSysUnc;

  return tmpCorrectionCode;
}


//=================================PRIVATE-PART=================================
//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyJetIDTool::GetIDSF(double& val,
								 double eta,
								 int prongness,
								 double pT)
{
  m_sWorkingPoint = "ID_SF_" + m_sIDLevel + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  ATH_MSG_DEBUG("using working point: "+m_sWorkingPoint);

  return this->GetIDValue(val, m_sWorkingPoint, pT);
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyJetIDTool::GetIDSysUnc(double& val,
								     double eta,
								     int prongness,
								     double pT)
{
  m_sWorkingPoint = "ID_SF_SysUnc_" + m_sIDLevel + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  double lowptunc = 0;
  TauAnalysisTools::CorrectionCode tmp_correctioncode_unc;
  TauAnalysisTools::CorrectionCode tmp_correctioncode_sf;
  tmp_correctioncode_unc = this->GetIDValue(lowptunc, m_sWorkingPoint, pT);
  if(m_bUseHighPtUncert && pT > 100.){
    double highpt_rel = 0.;
    double sf = 0;
    tmp_correctioncode_sf = this->GetIDSF(sf,
					  eta,
					  prongness,
					  pT);
    if(prongness == 1)
      highpt_rel = 0.000137 * (pT - 100.);
    else
      highpt_rel = 0.0000783 * (pT - 100.);
    double highptunc = highpt_rel * sf;
    val = sqrt(lowptunc * lowptunc + highptunc * highptunc);
    return tmp_correctioncode_unc; // one correction code return should be enough
  }
  val = lowptunc;
  return tmp_correctioncode_unc;
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyJetIDTool::GetIDStatUnc(double& val,
								      double eta,
								      int prongness,
								      double pT)
{
  m_sWorkingPoint = "ID_SF_StatUnc_" + m_sIDLevel + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  return this->GetIDValue(val, m_sWorkingPoint, pT);
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyJetIDTool::GetExclIDSF(double& val,
								     double eta,
								     int prongness,
								     double pT)
{
  if(m_iIDLevel == JETIDBDTTIGHT or m_iIDLevel == JETIDLLHTIGHT)
    return GetIDSF(eta,
		   prongness,
		   pT);

  m_sWorkingPoint = "ID_SF_" + m_sIDLevel + "_excl" + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  return this->GetIDValue(val, m_sWorkingPoint, pT);
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyJetIDTool::GetExclIDSysUnc(double& val,
									 double eta,
									 int prongness,
									 double pT){
  if(m_iIDLevel == JETIDBDTTIGHT or m_iIDLevel == JETIDLLHTIGHT)
    return GetIDSysUnc(eta,
		       prongness,
		       pT);
  m_sWorkingPoint = "ID_SF_SysUnc_" + m_sIDLevel + "_excl" + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  double lowptunc = 0;
  TauAnalysisTools::CorrectionCode tmp_correctioncode_unc;
  TauAnalysisTools::CorrectionCode tmp_correctioncode_sf;
  tmp_correctioncode_unc = this->GetIDValue(lowptunc, m_sWorkingPoint, pT);
  if(pT > 100.){
    double highpt_rel = 0.;
    double sf = 0;
    tmp_correctioncode_sf = this->GetExclIDSF(sf,
					      eta,
					      prongness,
					      pT);
    if(prongness == 1)
      highpt_rel = 0.000137 * (pT - 100.);
    else
      highpt_rel = 0.0000783 * (pT - 100.);
    double highptunc = highpt_rel / sf;
    val = sqrt(lowptunc * lowptunc + highptunc * highptunc);
    return min(tmp_correctioncode_unc,tmp_correctioncode_sf);
  }
  val = lowptunc;
  return tmp_correctioncode_unc;
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyJetIDTool::GetExclIDStatUnc(double& val,
									  double eta,
									  int prongness,
									  double pT)
{
  if(m_iIDLevel == JETIDBDTTIGHT or m_iIDLevel == JETIDLLHTIGHT)
    return GetIDStatUnc(val,
			eta,
			prongness,
			pT);

  m_sWorkingPoint = "ID_SF_StatUnc_" + m_sIDLevel + "_excl" + ConvertProngToString(prongness) + ConvertEtaToString(eta) + m_sIDpt;
  return this->GetIDValue(val, m_sWorkingPoint, pT);
}

//______________________________________________________________________________
std::string TauEfficiencyJetIDTool::ConvertIDToString(const int& iLevel)
{
  switch(iLevel)
    {
    case JETIDNONE:
      return "None";
      break;
    case JETIDBDTLOOSE:
      return "JetBDTSigLoose";
      break;
    case JETIDBDTMEDIUM:
      return "JetBDTSigMedium";
      break;
    case JETIDBDTTIGHT:
      return "JetBDTSigTight";
      break;
    case JETIDBDTOTHER:
      return "JetBDTSigOther";
      break;
    case JETIDLLHLOOSE:
      return "tauJLLHLoose";
      break;
    case JETIDLLHMEDIUM:
      return "tauJLLHMedium";
      break;
    case JETIDLLHTIGHT:
      return "tauJLLHTight";
      break;
    case JETIDLLHFAIL:
      return "tauJLLH";
      break;
    case JETIDBDTFAIL:
      return "JetBDTSig";
      break;
    default:
      assert(false && "No valid ID level passed. Breaking up ...");
      break;
    }
  return "";
}

//______________________________________________________________________________
std::string TauEfficiencyJetIDTool::ConvertEtaToString(const float& fEta){
  if(m_bUseInclusiveEta)
    return "_etaAll";
  if(fabs(fEta) <= 1.5)
    return "_barrel";
  else if(fabs(fEta) > 1.5)
    return "_endcap";
  else
    assert(false && "Invalid eta provided. Breaking up ...");
  return "";
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyJetIDTool::GetIDValue(double& val,
								    const std::string& sWorkingPoint,
								    const float& fPt){
  ATH_MSG_VERBOSE("Try to access histogram: " << sWorkingPoint);
  ATH_MSG_INFO("Try to access histogram: " << sWorkingPoint);
  TH1F* tmp = (*m_mSF)[sWorkingPoint];
  assert(tmp);
  int iBin = tmp->FindBin(fPt / 1000.); // histograms are binned in GeV
  //check underflow:
  if (iBin==0){
    ATH_MSG_WARNING("tau pt out of validity range, set scale factor to 0");
    val = 0;
    return TauAnalysisTools::CorrectionCode::OutOfValidityRange;
  }
  //check overflow:
  if(iBin == tmp->GetNbinsX() + 1)
    iBin -= 1;
  val = tmp->GetBinContent(iBin);
  return TauAnalysisTools::CorrectionCode::OK;
}


//______________________________________________________________________________
void TauEfficiencyJetIDTool::SwitchOnPtBinnedIDSF(bool bSwitch){
  ATH_MSG_DEBUG("switching PtBinnedIDSF on/off");
  if(bSwitch)
    m_sIDpt = "_ptbin";
  else
    m_sIDpt = "_ptinc";
}

#endif // TAUEFFICIENCYTOOL_CXX


//  LocalWords:  athena
