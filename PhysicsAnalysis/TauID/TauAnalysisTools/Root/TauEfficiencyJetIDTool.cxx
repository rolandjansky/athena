/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauAnalysisTools/TauEfficiencyJetIDTool.h"
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"

using namespace TauAnalysisTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
TauEfficiencyJetIDTool::TauEfficiencyJetIDTool(std::string sName)
  : CommonEfficiencyTool(sName)
  , m_sIDLevel("")
  , m_iIDLevelCache(-1)
{
}

TauEfficiencyJetIDTool::~TauEfficiencyJetIDTool()
{
}

//______________________________________________________________________________
CP::CorrectionCode TauEfficiencyJetIDTool::getEfficiencyScaleFactor(const xAOD::TauJet& xTau,
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
  if (m_iIDLevelCache != m_tTECT->m_iIDLevel)
  {
    m_iIDLevelCache = m_tTECT->m_iIDLevel;
    m_sIDLevel = "_"+m_tTECT->ConvertJetIDToString(m_iIDLevelCache);
    if (m_tTECT->m_bUseIDExclusiveSF)
      m_sIDLevel+="_excl";
  }

  // get prong extension for histogram name
  std::string sProng = (xTau.nTracks() == 1) ? "_1p" : "_3p";

  // get standard scale factor
  CP::CorrectionCode tmpCorrectionCode;
  tmpCorrectionCode = getValue("sf"+m_sIDLevel+sProng,
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
    tmpCorrectionCode = getValue(it->second+m_sIDLevel+sProng,
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

    // add high pt inflation in quadrature to TAUS_EFF_JETID_SYST
    if(m_tTECT->m_bUseHighPtUncert && xTau.pt() > 100000. && syst.basename() == "TAUS_EFF_JETID_SYST" )
    {
      double dHighPtInflation = 0.;
      if(xTau.nTracks() == 1)
        dHighPtInflation = 13.7e-8 * (xTau.pt() - 100000.);
      else
        dHighPtInflation = 7.83e-8 * (xTau.pt() - 100000.);
      dHighPtInflation *= dDirection;
      dTotalSystematic2 += dHighPtInflation*dHighPtInflation;
    }
  }

  // now use dDirection to use up/down uncertainty
  dDirection = (dDirection > 0) ? +1 : -1;

  // finally apply uncertainty (eff * ( 1 +/- \sum  )
  dEfficiencyScaleFactor *= 1 + dDirection * sqrt(dTotalSystematic2);

  return CP::CorrectionCode::Ok;
}

//=================================PRIVATE-PART=================================
