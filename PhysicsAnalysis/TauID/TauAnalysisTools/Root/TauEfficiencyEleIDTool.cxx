/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef test
#ifndef TAUEFFICIENCYELEIDTOOL_CXX
#define TAUEFFICIENCYELEIDTOOL_CXX
#include "TauAnalysisTools/TauEfficiencyEleIDTool.h"

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
TauEfficiencyEleIDTool::TauEfficiencyEleIDTool(EfficiencyCorrectionType eEfficiencyCorrectionType,
					       const std::string& sSharepath,
					       bool bDebug) :
  CommonEfficiencyTool(eEfficiencyCorrectionType,
		       sSharepath,
		       "eveto/sf",
		       bDebug)
{
}

//______________________________________________________________________________
void TauEfficiencyEleIDTool::configure(int iConfig1,
				       int iConfig2,
				       int iConfig3)
{
  m_eIDLevel = JETID(iConfig1);
  m_eEVLevel = EVETO(iConfig2);
  m_eOLR = OLR(iConfig3);
  m_sIDLevel = ConvertIDToString(m_eIDLevel);
  m_sEVLevel = ConvertEVetoToString(m_eEVLevel);
  m_sOLR = ConvertOLRToString(m_eOLR);
  if(m_eIDLevel != JETIDBDTMEDIUM || m_eEVLevel != EVETOBDTLOOSE || m_eOLR != OLRTIGHTPP){
    m_sIDLevel_eveto3p = ConvertIDToString(JETIDBDTOTHER);
    m_sEVLevel_eveto3p = ConvertEVetoToString(EVETOOTHER);
    m_sOLR_eveto3p = ConvertOLRToString(OLROTHER);
  }
  else{
    m_sIDLevel_eveto3p = m_sIDLevel;
    m_sEVLevel = m_sEVLevel;
    m_sOLR_eveto3p = m_sOLR;
  }
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyEleIDTool::getEfficiencyScaleFactor(const xAOD::TauJet& xTau,
								       double& efficiencyScaleFactor)
{
  // obtain ID SF value
  double dLeadTrackEta = getLeadTrackEta(&xTau);
  return this->GetEVetoSF(efficiencyScaleFactor, dLeadTrackEta, xTau.numTrack());
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyEleIDTool::applyEfficiencyScaleFactor(xAOD::TauJet& xTau)
{
  double sf = 0.;
  // retreive scale factor
  TauAnalysisTools::CorrectionCode tmpCorrectionCode = getEfficiencyScaleFactor(xTau, sf);
  // adding scale factor to auxdata
  xTau.auxdata< double >( "TauScaleFactorEleID" ) = sf;
  return tmpCorrectionCode;
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyEleIDTool::getEfficiencyScaleFactorStatUnc(const xAOD::TauJet& xTau, double& efficiencyScaleFactor, SysDirection eSysDirection, int iSysType){
  ATH_MSG_WARNING("Statistical uncertainty not supported for electron veto ID scale factors");
  efficiencyScaleFactor = 1;
  return TauAnalysisTools::CorrectionCode::Error;
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyEleIDTool::applyEfficiencyScaleFactorStatUnc(xAOD::TauJet& xTau, SysDirection eSysDirection, int iSysType){
  ATH_MSG_WARNING("Statistical uncertainty not supported for electron veto ID scale factors");
  return TauAnalysisTools::CorrectionCode::Error;
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyEleIDTool::getEfficiencyScaleFactorSysUnc(const xAOD::TauJet& xTau, double& efficiencyScaleFactorSysUnc, SysDirection eSysDirection, int iSysType){
  // obtain ID SF value
  double dLeadTrackEta = getLeadTrackEta(&xTau);
  return this->GetEVetoSFUnc(efficiencyScaleFactorSysUnc, dLeadTrackEta, xTau.numTrack(), eSysDirection);
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyEleIDTool::applyEfficiencyScaleFactorSysUnc(xAOD::TauJet& xTau, SysDirection eSysDirection, int iSysType){
  double dSysUnc = 0.;
  // retreive scale factor
  TauAnalysisTools::CorrectionCode tmpCorrectionCode = getEfficiencyScaleFactorSysUnc(xTau, dSysUnc, eSysDirection);
  // adding scale factor to auxdata
  std::string sVarName = "TauScaleFactorEleIDSysUnc";

  eSysDirection == UP ? sVarName += "_UP" : sVarName += "_DOWN";
  
  xTau.auxdata< double >( sVarName ) = dSysUnc;
  return tmpCorrectionCode;
}


//=================================PRIVATE-PART=================================
//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyEleIDTool::GetEVetoSF(double& val,
							 float trackEta,
							 int nTrack)
{
  if(nTrack == 1){
    m_sWorkingPoint = "h_" + m_sIDLevel + "_" + m_sEVLevel + "_" + m_sOLR + "_SF";
  }
  else{
    m_sWorkingPoint = "h_" + m_sIDLevel_eveto3p + "_" + m_sEVLevel_eveto3p + "_" + m_sOLR_eveto3p + "_SF";
  }

  return GetIDValue(val,
		    m_sWorkingPoint,
		    fabs(this->checkTrackEtaValidity(trackEta)));
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyEleIDTool::GetEVetoSFUnc(double& val,
							    float trackEta,
							    int nTrack,
							    int direction)
{
  if(nTrack == 1){
    m_sWorkingPoint = "h_" + m_sIDLevel + "_" + m_sEVLevel + "_" + m_sOLR;
  }
  else{
    m_sWorkingPoint = "h_" + m_sIDLevel_eveto3p + "_" + m_sEVLevel_eveto3p + "_" + m_sOLR_eveto3p;
  }

  direction == 1 ? m_sWorkingPoint += "_EU" : m_sWorkingPoint += "_ED";

  return GetIDValue(val,
		    m_sWorkingPoint,
		    fabs(this->checkTrackEtaValidity(trackEta)));
}

//______________________________________________________________________________
TauAnalysisTools::CorrectionCode TauEfficiencyEleIDTool::GetIDValue(double& val,
							 const std::string& sWorkingPoint,
							 const float& fEta){
  if(m_bDebug)
    std::cout << "Try to access histogram: " << sWorkingPoint << std::endl;
  TH1F* tmp = (*m_mSF)[sWorkingPoint];
  assert(tmp);
  int iBin = tmp->FindBin(fEta);
  //check underflow and overflow bin: 
  if (iBin==0 or iBin == tmp->GetNbinsX() + 1){
    ATH_MSG_WARNING("tau pt out of validity range, set return to 0");
    val = 0;
    return TauAnalysisTools::CorrectionCode::OutOfValidityRange;
  }
  val = tmp->GetBinContent(iBin);
  return TauAnalysisTools::CorrectionCode::OK;
}

//______________________________________________________________________________
float TauEfficiencyEleIDTool::checkTrackEtaValidity(float& trackEta)
{
  if(fabs(trackEta) > 2.47){
    ATH_MSG_WARNING("TauEfficiencyEleIDTool::checkTrackEtaValidity: Track eta out of bounds (SFs only valid up to 2.47). Will use SF for eta = 2.46");
    return 2.46f;
  }
  return trackEta;
}

//______________________________________________________________________________                                                                                                                                                                                       
double TauEfficiencyEleIDTool::getLeadTrackEta(const xAOD::TauJet* xTau){
  double dTrackEta = 0;
  double dTrackMaxPt = 0;
  for( unsigned int iNumTrack = 0; iNumTrack < xTau->numTrack(); iNumTrack++){
    if (xTau->track(iNumTrack)->pt() > dTrackMaxPt){
      dTrackMaxPt = xTau->track(iNumTrack)->pt();
      dTrackEta = xTau->track(iNumTrack)->eta();
    }
  }
  return dTrackEta;
}

//______________________________________________________________________________
std::string TauEfficiencyEleIDTool::ConvertIDToString(const JETID& level)
{
  switch(level)
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
}

//______________________________________________________________________________
std::string TauEfficiencyEleIDTool::ConvertEVetoToString(const EVETO& level)
{
  switch(level)
    {
    case EVETONONE:
      return "None";
      break;
    case EVETOBDTLOOSE:
      return "EleBDTLoose";
      break;
    case EVETOBDTMEDIUM:
      return "EleBDTMedium";
      break;
    case EVETOBDTTIGHT:
      return "EleBDTTight";
      break;
    case EVETOOTHER:
      return "EleBDTOther";
      break;
    default:
      assert(false && "No valid eveto level passed. Breaking up ...");
      break;
    }
}

//______________________________________________________________________________
std::string TauEfficiencyEleIDTool::ConvertOLRToString(const OLR& level)
{
  switch(level)
    {
    case OLRNONE:
      return "None";
      break;
    case OLRLOOSEPP:
      return "loosePP";
      break;
    case OLRMEDIUMPP:
      return "mediumPP";
      break;
    case OLRTIGHTPP:
      return "tightPP";
      break;
    case OLROTHER:
      return "other";
      break;
    default:
      assert(false && "No valid olr level passed. Breaking up ...");
      break;
    }
}

#endif // TAUEFFICIENCYELEIDTOOL_CXX
#endif
