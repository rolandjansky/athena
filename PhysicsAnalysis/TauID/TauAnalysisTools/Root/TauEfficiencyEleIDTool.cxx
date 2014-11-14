/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUEFFICIENCYELEIDTOOL_CXX
#define TAUEFFICIENCYELEIDTOOL_CXX

#include "TauAnalysisTools/TauEfficiencyEleIDTool.h"
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"

using namespace TauAnalysisTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
TauEfficiencyEleIDTool::TauEfficiencyEleIDTool(std::string sName) :
  CommonEfficiencyTool(sName)
{
  m_sSystematicSet = new CP::SystematicSet();
  m_iSysDirection = 0;
}

//______________________________________________________________________________
TauEfficiencyEleIDTool::~TauEfficiencyEleIDTool()
{
}

//______________________________________________________________________________
StatusCode TauEfficiencyEleIDTool::initialize()
{
  ATH_MSG_INFO("Initializing TauEfficiencyEleIDTool");

  m_mSystematics =
  {
    {"TAUS_EFF_ELEID_SYST", SYST}
  };

  return CommonEfficiencyTool::initialize();
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyEleIDTool::getEfficiencyScaleFactor(const xAOD::TauJet& xTau,
    double& dEfficiencyScaleFactor)
{
  setupWorkingPointSubstrings();
  // obtain ID SF value
  double dLeadTrackEta = getLeadTrackEta(&xTau);
  if (m_sSystematicSet->size() != 1)  // return nominal scale factor
  {
    if (m_sSystematicSet->size() > 1)
      ATH_MSG_ERROR("It is currently not supported to combine several systematic variations!");
    return GetEVetoSF(dEfficiencyScaleFactor, dLeadTrackEta, xTau.nTracks());
  }
  else
  {
    for (auto syst : *m_sSystematicSet)
    {
      auto it = m_mSystematics.find(syst.basename());
      if (it == m_mSystematics.end())
      {
        ATH_MSG_VERBOSE("unsupported systematic variation: skipping this one, returning nominal SF");
        return GetEVetoSF(dEfficiencyScaleFactor, dLeadTrackEta, xTau.nTracks());
        continue;
      }

      if (fabs(syst.parameter()) != 1)
      {
        ATH_MSG_WARNING("systematic variation other than 1 sigma is not supported, skipping this one");
        continue;
      }
      m_iSysDirection = syst.parameter();
      if (it->second == SYST)
        return GetEVetoSFUnc(dEfficiencyScaleFactor, dLeadTrackEta, xTau.nTracks());
      else
      {
        ATH_MSG_WARNING("unsupported systematic variation: skipping this one");
        continue;
      }
    }
  }
  return CP::CorrectionCode::Error;
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyEleIDTool::applyEfficiencyScaleFactor(const xAOD::TauJet& xTau)
{
  double dSf = 0.;
  // retreive scale factor
  CP::CorrectionCode tmpCorrectionCode = getEfficiencyScaleFactor(xTau, dSf);
  // adding scale factor to auxdecor
  xTau.auxdecor< double >( m_tTECT->m_sVarNameBase ) = dSf;
  ATH_MSG_VERBOSE("Stored value " << dSf << " as " << m_tTECT->m_sVarNameBase << " in auxdecor");
  return tmpCorrectionCode;
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyEleIDTool::getEfficiencyScaleFactorStatUnc(const xAOD::TauJet& xTau, double& dEfficiencyScaleFactorStatUnc)
{
  ATH_MSG_WARNING("Statistical uncertainty not supported for electron veto ID scale factors");
  // useless function to avoid compiler warnings
  dEfficiencyScaleFactorStatUnc = xTau.pt() * 0. + 1.;
  return CP::CorrectionCode::Error;
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyEleIDTool::applyEfficiencyScaleFactorStatUnc(const xAOD::TauJet& xTau)
{
  double dStatUnc = 0.;
  return getEfficiencyScaleFactorStatUnc(xTau, dStatUnc);
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyEleIDTool::getEfficiencyScaleFactorSysUnc(const xAOD::TauJet& xTau, double& dEfficiencyScaleFactorSysUnc)
{
  setupWorkingPointSubstrings();
  // obtain ID SF value
  double dLeadTrackEta = getLeadTrackEta(&xTau);
  return this->GetEVetoSFUnc(dEfficiencyScaleFactorSysUnc, dLeadTrackEta, xTau.nTracks());
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyEleIDTool::applyEfficiencyScaleFactorSysUnc(const xAOD::TauJet& xTau)
{
  double dSysUnc = 0.;
  // retreive scale factor
  CP::CorrectionCode tmpCorrectionCode = getEfficiencyScaleFactorSysUnc(xTau, dSysUnc);
  // adding scale factor to auxdecor
  std::string sVarName = m_tTECT->m_sVarNameBase + "SysUnc";

  if (m_tTECT->m_iSysDirection == 1)
    sVarName += "_Up";
  else if (m_tTECT->m_iSysDirection == -1)
    sVarName += "_Down";
  else
  {
    ATH_MSG_WARNING("SysDirection "<<m_tTECT->m_iSysDirection<<" is not supported, need to be 1 or -1");
    return CP::CorrectionCode::Error;
  }

  xTau.auxdecor< double >( sVarName ) = dSysUnc;
  ATH_MSG_VERBOSE("Stored value " << dSysUnc << " as " << sVarName << " in auxdecor");
  return tmpCorrectionCode;
}

bool TauEfficiencyEleIDTool::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const
{
  CP::SystematicSet sys = affectingSystematics();
  return sys.find (systematic) != sys.end ();
}

CP::SystematicSet TauEfficiencyEleIDTool::affectingSystematics() const
{
  CP::SystematicSet result = recommendedSystematics();
  return result;
}

CP::SystematicSet TauEfficiencyEleIDTool::recommendedSystematics() const
{
  CP::SystematicSet result;
  result.insert (CP::SystematicVariation ("TAUS_EFF_ELEID_SYST", 1));
  result.insert (CP::SystematicVariation ("TAUS_EFF_ELEID_SYST", -1));
  return result;
}

CP::SystematicCode TauEfficiencyEleIDTool::applySystematicVariation ( const CP::SystematicSet& sSystematicSet)
{
  m_sSystematicSet = &sSystematicSet;
  return CP::SystematicCode::Ok;
}

//=================================PRIVATE-PART=================================
//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyEleIDTool::GetEVetoSF(double& dVal,
    float fTrackEta,
    int iNTrack)
{
  if(iNTrack == 1)
  {
    m_sWorkingPoint = "h_" + m_sIDLevel + "_" + m_sEVLevel + "_" + m_sOLRLevel + "_SF";
  }
  else
  {
    m_sWorkingPoint = "h_" + m_sIDLevel_eveto3p + "_" + m_sEVLevel_eveto3p + "_" + m_sOLRLevel_eveto3p + "_SF";
  }

  return GetIDValue(dVal,
                    m_sWorkingPoint,
                    fabs(this->checkTrackEtaValidity(fTrackEta)));
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyEleIDTool::GetEVetoSFUnc(double& dVal,
    float fTrackEta,
    int iNTrack)
{
  if(iNTrack == 1)
  {
    m_sWorkingPoint = "h_" + m_sIDLevel + "_" + m_sEVLevel + "_" + m_sOLRLevel;
  }
  else
  {
    m_sWorkingPoint = "h_" + m_sIDLevel_eveto3p + "_" + m_sEVLevel_eveto3p + "_" + m_sOLRLevel_eveto3p;
  }

  if (m_iSysDirection == 1)
    m_sWorkingPoint += "_EU";
  else if (m_iSysDirection == -1)
    m_sWorkingPoint += "_ED";
  else
  {
    ATH_MSG_WARNING("SysDirection "<<m_iSysDirection<<" is not supported, need to be 1 or -1");
    return CP::CorrectionCode::Error;
  }

  return GetIDValue(dVal,
                    m_sWorkingPoint,
                    fabs(this->checkTrackEtaValidity(fTrackEta)));
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyEleIDTool::GetIDValue(double& dVal,
    const std::string& sWorkingPoint,
    const float& fEta)
{
  ATH_MSG_DEBUG("Try to access histogram: " << sWorkingPoint);
  TH1F* tmp = (*m_mSF)[sWorkingPoint];
  if (!tmp)
  {
    ATH_MSG_FATAL("could not find histogram " << sWorkingPoint << ", breaking up");
    return CP::CorrectionCode::Error;
  }
  int iBin = tmp->FindBin(fEta);
  //check underflow and overflow bin:
  if (iBin==0 or iBin == tmp->GetNbinsX() + 1)
  {
    ATH_MSG_WARNING("tau pt out of validity range, set return to 0");
    dVal = 0;
    return CP::CorrectionCode::OutOfValidityRange;
  }
  dVal = tmp->GetBinContent(iBin);
  ATH_MSG_VERBOSE("Got value " << dVal << " in bin " << iBin << "(eta=" << fEta <<")");
  return CP::CorrectionCode::Ok;
}

//______________________________________________________________________________
float TauEfficiencyEleIDTool::checkTrackEtaValidity(float& fTrackEta)
{
  if(fabs(fTrackEta) > 2.47)
  {
    ATH_MSG_WARNING("TauEfficiencyEleIDTool::checkTrackEtaValidity: Track eta out of bounds (SFs only valid up to 2.47). Will use SF for eta = 2.46");
    return 2.46f;
  }
  return fTrackEta;
}

//______________________________________________________________________________
double TauEfficiencyEleIDTool::getLeadTrackEta(const xAOD::TauJet* xTau)
{
  double dTrackEta = 0;
  double dTrackMaxPt = 0;
  for( unsigned int iNumTrack = 0; iNumTrack < xTau->nTracks(); iNumTrack++)
  {
    if (xTau->track(iNumTrack)->pt() > dTrackMaxPt)
    {
      dTrackMaxPt = xTau->track(iNumTrack)->pt();
      dTrackEta = xTau->track(iNumTrack)->eta();
    }
  }
  return dTrackEta;
}

//______________________________________________________________________________
void TauEfficiencyEleIDTool::setupWorkingPointSubstrings()
{
  m_sIDLevel = ConvertIDToString(m_tTECT->m_iIDLevel);
  m_sEVLevel = ConvertEVetoToString(m_tTECT->m_iEVLevel);
  m_sOLRLevel = ConvertOLRToString(m_tTECT->m_iOLRLevel);
  if(m_tTECT->m_iIDLevel != JETIDBDTMEDIUM || m_tTECT->m_iEVLevel != ELEIDBDTLOOSE || m_tTECT->m_iOLRLevel != OLRTIGHTPP)
  {
    m_sIDLevel_eveto3p = ConvertIDToString(JETIDBDTOTHER);
    m_sEVLevel_eveto3p = ConvertEVetoToString(ELEIDOTHER);
    m_sOLRLevel_eveto3p = ConvertOLRToString(OLROTHER);
  }
  else
  {
    m_sIDLevel_eveto3p = m_sIDLevel;
    m_sEVLevel_eveto3p = m_sEVLevel;
    m_sOLRLevel_eveto3p = m_sOLRLevel;
  }
}

//______________________________________________________________________________
std::string TauEfficiencyEleIDTool::ConvertIDToString(int iLevel)
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
  default:
    assert(false && "No valid ID level passed. Breaking up ...");
    break;
  }
  return "";
}

//______________________________________________________________________________
std::string TauEfficiencyEleIDTool::ConvertEVetoToString(int iLevel)
{
  switch(iLevel)
  {
  case ELEIDNONE:
    return "None";
    break;
  case ELEIDBDTLOOSE:
    return "EleBDTLoose";
    break;
  case ELEIDBDTMEDIUM:
    return "EleBDTMedium";
    break;
  case ELEIDBDTTIGHT:
    return "EleBDTTight";
    break;
  case ELEIDOTHER:
    return "EleBDTOther";
    break;
  default:
    assert(false && "No valid eveto level passed. Breaking up ...");
    break;
  }
  return "";
}

//______________________________________________________________________________
std::string TauEfficiencyEleIDTool::ConvertOLRToString(int iLevel)
{
  switch(iLevel)
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
  return "";
}

#endif // TAUEFFICIENCYELEIDTOOL_CXX
