/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework include(s):
#include "PathResolver/PathResolver.h"

// local include(s)
#include "TauAnalysisTools/CommonDiTauEfficiencyTool.h"
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"
#include "xAODTruth/TruthParticleContainer.h"

// ROOT include(s)
#include "TH2F.h"

using namespace TauAnalysisTools;

//______________________________________________________________________________
CommonDiTauEfficiencyTool::CommonDiTauEfficiencyTool(std::string sName)
  : CommonEfficiencyTool( sName )
  , m_fX(&DiTauPt)
  , m_fY(&DiTauEta)
  , m_bSFIsAvailable(false)
  , m_bSFIsAvailableChecked(false)
{
}

CommonDiTauEfficiencyTool::~CommonDiTauEfficiencyTool()
{
}


/*
  Retrieve the scale factors and if requested the values for the NP's and add
  this stuff in quadrature. Finally return sf_nom +/- n*uncertainty
*/
//______________________________________________________________________________
CP::CorrectionCode CommonDiTauEfficiencyTool::getEfficiencyScaleFactor(const xAOD::DiTauJet& xDiTau,
    double& dEfficiencyScaleFactor)
{
  // check which true state is requestet
  if (!m_bSkipTruthMatchCheck and checkTruthMatch(xDiTau) != m_eCheckTruth)
  {
    dEfficiencyScaleFactor = 1.;
    return CP::CorrectionCode::Ok;
  }

  CP::CorrectionCode tmpCorrectionCode = getValue(m_sSFHistName,
                                                  xDiTau,
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

    // needed for up/down decision
    dDirection = syst.parameter();

    // build up histogram name
    std::string sHistName = it->second;
    if (dDirection>0)   sHistName+="_up";
    else                sHistName+="_down";
    if (!m_sWP.empty()) sHistName+="_"+m_sWP;
    // sHistName += sProng;

    // get the uncertainty from the histogram
    tmpCorrectionCode = getValue(sHistName,
                                 xDiTau,
                                 dUncertaintySyst);

    // return correction code if histogram is not available
    if (tmpCorrectionCode != CP::CorrectionCode::Ok)
      return tmpCorrectionCode;

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

/*
  Get scale factor from getEfficiencyScaleFactor and decorate it to the
  tau. Note that this can only be done if the variable name is not already used,
  e.g. if the variable was already decorated on a previous step (enured by the
  m_bSFIsAvailableChecked check).

  Technical note: cannot use `static SG::AuxElement::Decorator` as we will have
  multiple instances of this tool with different decoration names.
*/
//______________________________________________________________________________
CP::CorrectionCode CommonDiTauEfficiencyTool::applyEfficiencyScaleFactor(const xAOD::DiTauJet& xDiTau)
{
  double dSf = 0.;

  if (!m_bSFIsAvailableChecked)
  {
    m_bSFIsAvailable = xDiTau.isAvailable< double >(m_sVarName);
    // m_bSFIsAvailable = xDiTau.isAvailable< double >("bliblablubb");
    m_bSFIsAvailableChecked = true;
    if (m_bSFIsAvailable)
    {
      ATH_MSG_DEBUG(m_sVarName << " decoration is available on first ditau processed, switched of applyEfficiencyScaleFactor for further ditaus.");
      ATH_MSG_DEBUG("If an application of efficiency scale factors needs to be redone, please pass a shallow copy of the original ditau.");
    }
  }
  if (m_bSFIsAvailable)
    return CP::CorrectionCode::Ok;

  // retreive scale factor
  CP::CorrectionCode tmpCorrectionCode = getEfficiencyScaleFactor(xDiTau, dSf);
  // adding scale factor to tau as decoration
  xDiTau.auxdecor<double>(m_sVarName) = dSf;

  return tmpCorrectionCode;
}


// //______________________________________________________________________________
void CommonDiTauEfficiencyTool::ReadInputs(TFile* fFile)
{
  m_mSF->clear();

  // initialize function pointer
  m_fX = &DiTauPt;
  m_fY = &DiTauEta;

  TKey *kKey;
  TIter itNext(fFile->GetListOfKeys());
  while ((kKey = (TKey*)itNext()))
  {
    // parse file content for objects of type TNamed, check their title for
    // known strings and reset funtion pointer
    std::string sKeyName = kKey->GetName();

    std::vector<std::string> vSplitName = {};
    split(sKeyName,'_',vSplitName);
    if (vSplitName[0] == "sf")
    {
      addHistogramToSFMap(kKey, sKeyName);
    }
    else
    {
      if (sKeyName.find("_up_") != std::string::npos or sKeyName.find("_down_") != std::string::npos)
        addHistogramToSFMap(kKey, sKeyName);
      else
      {
        size_t iPos = sKeyName.find('_');
        addHistogramToSFMap(kKey, sKeyName.substr(0,iPos)+"_up"+sKeyName.substr(iPos));
        addHistogramToSFMap(kKey, sKeyName.substr(0,iPos)+"_down"+sKeyName.substr(iPos));
      }
    }
  }
  ATH_MSG_INFO("data loaded from " << fFile->GetName());
}


/*
  return value from the tuple map object based on the pt/eta values (or the
  corresponding value in case of configuration)
*/
//______________________________________________________________________________
CP::CorrectionCode CommonDiTauEfficiencyTool::getValue(const std::string& sHistName,
    const xAOD::DiTauJet& xDiTau,
    double& dEfficiencyScaleFactor) const
{
  if (m_mSF->find(sHistName) == m_mSF->end())
  {
    ATH_MSG_ERROR("Object with name "<<sHistName<<" was not found in input file.");
    ATH_MSG_DEBUG("Content of input file");
    for (auto eEntry : *m_mSF)
      ATH_MSG_DEBUG("  Entry: "<<eEntry.first);
    return CP::CorrectionCode::Error;
  }

  // get a tuple (TObject*,functionPointer) from the scale factor map
  tTupleObjectFunc tTuple = (*m_mSF)[sHistName];

  // get pt and eta (for x and y axis respectively)
  double dPt = m_fX(xDiTau);
  double dEta = m_fY(xDiTau);

  // finally obtain efficiency scale factor from TH1F/TH1D/TF1, by calling the
  // function pointer stored in the tuple from the scale factor map
  return  (std::get<1>(tTuple))(std::get<0>(tTuple), dEfficiencyScaleFactor, dPt, dEta);
}


/*
  Check the type of truth particle, previously matched with the
  DiTauTruthMatchingTool. The type to match was parsed from the input file in
  CommonEfficiencyTool::generateSystematicSets()
*/
//______________________________________________________________________________
e_TruthMatchedParticleType CommonDiTauEfficiencyTool::checkTruthMatch(const xAOD::DiTauJet& xDiTau) const
{

  if (!xDiTau.isAvailable<char>("IsTruthHadronic"))
    ATH_MSG_ERROR("No truth match information available. Please run DiTauTruthMatchingTool first");
  static SG::AuxElement::Accessor<char> accIsTruthHadronic("IsTruthHadronic");

  e_TruthMatchedParticleType eTruthMatchedParticleType = Unknown;

  if (accIsTruthHadronic(xDiTau))
    eTruthMatchedParticleType = TruthHadronicDiTau;

  return eTruthMatchedParticleType;
}


//______________________________________________________________________________
double TauAnalysisTools::DiTauPt(const xAOD::DiTauJet& xDiTau)
{
  // return ditau pt in GeV
  return xDiTau.auxdata<double>("ditau_pt")/1000.;
}


//______________________________________________________________________________
double TauAnalysisTools::DiTauEta(const xAOD::DiTauJet& xDiTau)
{
  // return ditau eta
  return xDiTau.eta();
}
