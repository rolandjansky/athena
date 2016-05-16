/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauAnalysisTools/TauEfficiencyEleIDTool.h"
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"

using namespace TauAnalysisTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
TauEfficiencyEleIDTool::TauEfficiencyEleIDTool(std::string sName) :
  CommonEfficiencyTool(sName)
  , m_iIDLevelCache(JETIDNONEUNCONFIGURED)
  , m_iEVLevelCache(ELEIDNONEUNCONFIGURED)
  , m_iOLRLevelCache(ELEIDNONEUNCONFIGURED)
{
  m_sSystematicSet = new CP::SystematicSet();
  m_iSysDirection = 0;

  ATH_MSG_WARNING("implement string caching!!!");
}

//______________________________________________________________________________
TauEfficiencyEleIDTool::~TauEfficiencyEleIDTool()
{
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyEleIDTool::getEfficiencyScaleFactor(const xAOD::TauJet& xTau,
    double& dEfficiencyScaleFactor)
{
  // check which true state is requestet
  if (checkTruthMatch(xTau) != m_eCheckTruth)
  {
    dEfficiencyScaleFactor = 1.;
    return CP::CorrectionCode::Ok;
  }

  // only recreate m_sIDLevel if user changed the IDLevel through the tool
  // properties
  if (m_iIDLevelCache != m_tTECT->m_iIDLevel && m_iEVLevelCache != m_tTECT->m_iEVLevel &&  m_iOLRLevelCache != m_tTECT->m_iOLRLevel)
  {
    m_iIDLevelCache = m_tTECT->m_iIDLevel;
    m_iEVLevelCache = m_tTECT->m_iEVLevel;
    m_iOLRLevelCache = m_tTECT->m_iOLRLevel;
    setupWorkingPointSubstrings();
  }

  // get prong extension for histogram name
  std::string sWorkingPoint = (xTau.nTracks() == 1) ? m_sWorkingPoint_1p : m_sWorkingPoint_3p;

  // get standard scale factor
  CP::CorrectionCode tmpCorrectionCode;
  tmpCorrectionCode = getValue("sf"+sWorkingPoint+"_1p",
                               xTau,
                               dEfficiencyScaleFactor);

  // return correction code if histogram is not available
  if (tmpCorrectionCode != CP::CorrectionCode::Ok)
    return tmpCorrectionCode;

  // skip further process if systematic set is empty
  if (m_sSystematicSet->size() == 0)
    return CP::CorrectionCode::Ok;

  // get uncertainties summed in quadrature
  double dTotalSystematic2 = 0;
  double dDirection = 0;
  for (auto syst : *m_sSystematicSet)
  {
    // check if systematic is available
    auto it = m_mSystematicsHistNames.find(syst.basename());

    // get uncertainty value
    double dUncertaintySyst = 0;
    tmpCorrectionCode = getValue(it->second+sWorkingPoint+"_1p",
                                 xTau,
                                 dUncertaintySyst);

    // return correction code if histogram is not available
    if (tmpCorrectionCode != CP::CorrectionCode::Ok)
      return tmpCorrectionCode;

    // needed for up/down decision
    dDirection = syst.parameter();

    // scale uncertainty with direction, i.e. +/- n*sigma
    dUncertaintySyst *= dDirection;

    // square uncertainty and add to total uncertainty
    dTotalSystematic2 += dUncertaintySyst * dUncertaintySyst;
  }

  // now use dDirection to use up/down uncertainty
  dDirection = (dDirection > 0) ? +1 : -1;

  // finally apply uncertainty (eff * ( 1 +/- \sum  )
  dEfficiencyScaleFactor *= 1 + dDirection * sqrt(dTotalSystematic2);
  return CP::CorrectionCode::Ok;
}

//=================================PRIVATE-PART=================================
//______________________________________________________________________________
void TauEfficiencyEleIDTool::setupWorkingPointSubstrings()
{
  m_sIDLevel = convertIDToString(m_tTECT->m_iIDLevel);
  m_sEVLevel = convertEVetoToString(m_tTECT->m_iEVLevel);
  m_sOLRLevel = convertOLRToString(m_tTECT->m_iOLRLevel);

  if(m_tTECT->m_iIDLevel != JETIDBDTMEDIUM || m_tTECT->m_iEVLevel != ELEIDBDTLOOSE || m_tTECT->m_iOLRLevel != OLRTIGHTPP)
  {
    m_sIDLevel_eveto3p = convertIDToString(JETIDBDTOTHER);
    m_sEVLevel_eveto3p = convertEVetoToString(ELEIDOTHER);
    m_sOLRLevel_eveto3p = convertOLRToString(OLROTHER);
  }
  else
  {
    m_sIDLevel_eveto3p = m_sIDLevel;
    m_sEVLevel_eveto3p = m_sEVLevel;
    m_sOLRLevel_eveto3p = m_sOLRLevel;
  }

  m_sWorkingPoint_1p = "_"+m_sIDLevel+"_"+m_sEVLevel+"_"+m_sOLRLevel;
  m_sWorkingPoint_3p = "_"+m_sIDLevel_eveto3p+"_"+m_sEVLevel_eveto3p+"_"+m_sOLRLevel_eveto3p;
}

//______________________________________________________________________________
std::string TauEfficiencyEleIDTool::convertIDToString(int iLevel) const
{
  switch(iLevel)
  {
  case JETIDNONE:
    return "none";
    break;
  case JETIDBDTLOOSE:
    return "jetbdtsigloose";
    break;
  case JETIDBDTMEDIUM:
    return "jetbdtsigmedium";
    break;
  case JETIDBDTTIGHT:
    return "jetbdtsigtight";
    break;
  case JETIDBDTOTHER:
    return "jetbdtsigother";
    break;
  default:
    assert(false && "No valid ID level passed. Breaking up ...");
    break;
  }
  return "";
}

//______________________________________________________________________________
std::string TauEfficiencyEleIDTool::convertEVetoToString(int iLevel) const
{
  switch(iLevel)
  {
  case ELEIDNONE:
    return "none";
    break;
  case ELEIDBDTLOOSE:
    return "elebdtloose";
    break;
  case ELEIDBDTMEDIUM:
    return "elebdtmedium";
    break;
  case ELEIDBDTTIGHT:
    return "elebdttight";
    break;
  case ELEIDOTHER:
    return "elebdtother";
    break;
  default:
    assert(false && "No valid eveto level passed. Breaking up ...");
    break;
  }
  return "";
}

//______________________________________________________________________________
std::string TauEfficiencyEleIDTool::convertOLRToString(int iLevel) const
{
  switch(iLevel)
  {
  case OLRNONE:
    return "none";
    break;
  case OLRLOOSEPP:
    return "loosepp";
    break;
  case OLRMEDIUMPP:
    return "mediumpp";
    break;
  case OLRTIGHTPP:
    return "tightpp";
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
