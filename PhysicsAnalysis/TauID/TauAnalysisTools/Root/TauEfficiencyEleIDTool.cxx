#include "TauAnalysisTools/TauEfficiencyEleIDTool.h"
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"

using namespace TauAnalysisTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
TauEfficiencyEleIDTool::TauEfficiencyEleIDTool(std::string sName)
  : CommonEfficiencyTool(sName)
  , m_sWorkingPoint_1p("")
  , m_sWorkingPoint_3p("")
{
  m_sSystematicSet = new CP::SystematicSet();
}

//______________________________________________________________________________
TauEfficiencyEleIDTool::~TauEfficiencyEleIDTool()
{
}

StatusCode TauEfficiencyEleIDTool::initialize()
{
  setupWorkingPointSubstrings();
  return CommonEfficiencyTool::initialize();
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
  std::string sIDLevel = convertIDToString(m_iIDLevel);
  std::string sEVLevel = convertEVetoToString(m_iEVLevel);
  std::string sOLRLevel = convertOLRToString(m_iOLRLevel);

  std::string sIDLevel_eveto3p;
  std::string sEVLevel_eveto3p;
  std::string sOLRLevel_eveto3p;

  if(m_iIDLevel != JETIDBDTMEDIUM || m_iEVLevel != ELEIDBDTLOOSE || m_iOLRLevel != OLRTIGHTPP)
  {
    sIDLevel_eveto3p = convertIDToString(JETIDBDTOTHER);
    sEVLevel_eveto3p = convertEVetoToString(ELEIDOTHER);
    sOLRLevel_eveto3p = convertOLRToString(OLROTHER);
  }
  else
  {
    sIDLevel_eveto3p = sIDLevel;
    sEVLevel_eveto3p = sEVLevel;
    sOLRLevel_eveto3p = sOLRLevel;
  }

  m_sWorkingPoint_1p = "_"+sIDLevel+"_"+sEVLevel+"_"+sOLRLevel;
  m_sWorkingPoint_3p = "_"+sIDLevel_eveto3p+"_"+sEVLevel_eveto3p+"_"+sOLRLevel_eveto3p;
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
