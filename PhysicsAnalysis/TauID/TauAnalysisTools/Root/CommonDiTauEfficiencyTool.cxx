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
  , m_fX(&TruthLeadPt)
  , m_fY(&TruthSubleadPt)
  , m_fZ(&TruthDeltaR)
  , m_bSFIsAvailable(false)
  , m_bSFIsAvailableChecked(false)
{
}

CommonDiTauEfficiencyTool::~CommonDiTauEfficiencyTool()
{
}


/*
  - Find the root files with scale factor inputs on afs/cvmfs using PathResolver
    (more info here:
    https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/PathResolver)
  - Call further functions to process and define NP strings and so on
  - Configure to provide nominal scale factors by default
*/
StatusCode CommonDiTauEfficiencyTool::initialize()
{
  ATH_MSG_INFO( "Initializing CommonDiTauEfficiencyTool" );
  // only read in histograms once
  if (m_mSF==nullptr)
  {
    std::string sInputFilePath = PathResolverFindCalibFile(m_sInputFilePath);

    m_mSF = std::make_unique< tSFMAP >();
    std::unique_ptr< TFile > fSF( TFile::Open( sInputFilePath.c_str(), "READ" ) );
    if(!fSF)
    {
      ATH_MSG_FATAL("Could not open file " << sInputFilePath.c_str());
      return StatusCode::FAILURE;
    }
    ReadInputs(fSF);
    fSF->Close();
  }

  // needed later on in generateSystematicSets(), maybe move it there
  std::vector<std::string> vInputFilePath;
  split(m_sInputFilePath,'/',vInputFilePath);
  m_sInputFileName = vInputFilePath.back();

  generateSystematicSets();

  if (m_sWP.length()>0)
    m_sSFHistName = "sf_"+m_sWP;

  // load empty systematic variation by default
  if (applySystematicVariation(CP::SystematicSet()) != CP::SystematicCode::Ok )
    return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
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
  if (checkTruthMatch(xDiTau) != m_eCheckTruth)
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
void CommonDiTauEfficiencyTool::ReadInputs(std::unique_ptr<TFile> &fFile)
{
  m_mSF->clear();

  // initialize function pointer
  m_fX = &TruthLeadPt;
  m_fY = &TruthSubleadPt;
  m_fZ = &TruthDeltaR;

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
  This function parses the names of the objects from the input file and
  generates the systematic sets and defines which ones are recommended or only
  available. It also checks, based on the root file name, on which tau it needs
  to be applied, e.g. only on reco taus coming from true taus or on those faked
  by true electrons...
  Examples:
  filename: JetID_TrueHadDiTau_2017-fall.root -> apply only to true ditaus
  histname: sf_* -> nominal scale factor
  histname: TOTAL_* -> "total" NP, recommended
  histname: afii_* -> "total" NP, not recommended, but available
*/
//______________________________________________________________________________
void CommonDiTauEfficiencyTool::generateSystematicSets()
{
  // creation of basic string for all NPs, e.g. "TAUS_TRUEHADTAU_EFF_RECO_"
  std::vector<std::string> vSplitInputFilePath = {};
  split(m_sInputFileName,'_',vSplitInputFilePath);
  std::string sEfficiencyType = vSplitInputFilePath.at(0);
  std::string sTruthType = vSplitInputFilePath.at(1);
  std::transform(sEfficiencyType.begin(), sEfficiencyType.end(), sEfficiencyType.begin(), toupper);
  std::transform(sTruthType.begin(), sTruthType.end(), sTruthType.begin(), toupper);
  std::string sSystematicBaseString = "TAUS_"+sTruthType+"_EFF_"+sEfficiencyType+"_";
  // set truth type to check for in truth matching
  if (sTruthType=="TRUEHADTAU") m_eCheckTruth = TauAnalysisTools::TruthHadronicTau;
  if (sTruthType=="TRUEELECTRON") m_eCheckTruth = TauAnalysisTools::TruthElectron;
  if (sTruthType=="TRUEMUON") m_eCheckTruth = TauAnalysisTools::TruthMuon;
  if (sTruthType=="TRUEJET") m_eCheckTruth = TauAnalysisTools::TruthJet;
  if (sTruthType=="TRUEHADDITAU") m_eCheckTruth = TauAnalysisTools::TruthHadronicDiTau;
  if (sEfficiencyType=="ELEOLR") m_bNoMultiprong = true;
  for (auto mSF : *m_mSF)
  {
    // parse for nuisance parameter in histogram name
    std::vector<std::string> vSplitNP = {};
    split(mSF.first,'_',vSplitNP);
    std::string sNP = vSplitNP.at(0);
    std::string sNPUppercase = vSplitNP.at(0);
    // skip nominal scale factors
    if (sNP == "sf") continue;
    // test if NP starts with a capital letter indicating that this should be recommended
    bool bIsRecommended = false;
    if (isupper(sNP.at(0)))
      bIsRecommended = true;
    // make sNP uppercase and build final NP entry name
    std::transform(sNPUppercase.begin(), sNPUppercase.end(), sNPUppercase.begin(), toupper);
    std::string sSystematicString = sSystematicBaseString+sNPUppercase;
    // add all found systematics to the AffectingSystematics
    m_sAffectingSystematics.insert(CP::SystematicVariation (sSystematicString, 1));
    m_sAffectingSystematics.insert(CP::SystematicVariation (sSystematicString, -1));
    // only add found uppercase systematics to the RecommendedSystematics
    if (bIsRecommended)
    {
      m_sRecommendedSystematics.insert(CP::SystematicVariation (sSystematicString, 1));
      m_sRecommendedSystematics.insert(CP::SystematicVariation (sSystematicString, -1));
    }
    ATH_MSG_DEBUG("connected base name " << sNP << " with systematic " <<sSystematicString);
    m_mSystematicsHistNames.insert({sSystematicString,sNP});
  }
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
  double dX = m_fX(xDiTau);
  double dY = m_fY(xDiTau);
  double dZ = m_fZ(xDiTau);

  double dVars[3] = {dX, dY, dZ};
  // finally obtain efficiency scale factor from TH1F/TH1D/TF1, by calling the
  // function pointer stored in the tuple from the scale factor map
  return  (std::get<1>(tTuple))(std::get<0>(tTuple), dEfficiencyScaleFactor, dVars);
}

/*
  Check the type of truth particle, previously matched with the
  DiTauTruthMatchingTool. The type to match was parsed from the input file in
  CommonDiTauEfficiencyTool::generateSystematicSets()
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
double TauAnalysisTools::TruthLeadPt(const xAOD::DiTauJet& xDiTau)
{
  // return leading truth tau pt in GeV
  static const SG::AuxElement::ConstAccessor< double > acc( "TruthVisLeadPt" );
  return acc( xDiTau ) * 0.001;
}

//______________________________________________________________________________
double TauAnalysisTools::TruthSubleadPt(const xAOD::DiTauJet& xDiTau)
{
  // return subleading truth tau pt in GeV
  static const SG::AuxElement::ConstAccessor< double > acc( "TruthVisSubleadPt" );
  return acc( xDiTau ) * 0.001;
}

//______________________________________________________________________________
double TauAnalysisTools::TruthDeltaR(const xAOD::DiTauJet& xDiTau)
{
  // return truth taus distance delta R
  static const SG::AuxElement::ConstAccessor< double > acc( "TruthVisDeltaR" );
  return acc( xDiTau );
}

