/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Framework include(s):
#include "PathResolver/PathResolver.h"

// local include(s)
#include "TauAnalysisTools/CommonDiTauSmearingTool.h"
// #include "TauAnalysisTools/DiTauSmearingTool.h"
#include "xAODTruth/TruthParticleContainer.h"

// ROOT include(s)
#include "TF1.h"

using namespace TauAnalysisTools;

//______________________________________________________________________________
CommonDiTauSmearingTool::CommonDiTauSmearingTool(std::string sName)
  : CommonSmearingTool( sName )
  , m_fX(&TruthLeadPt)
  , m_fY(&TruthSubleadPt)
  , m_fZ(&TruthDeltaR)
{
}

CommonDiTauSmearingTool::~CommonDiTauSmearingTool()
{
}

/*
  - Find the root files with smearing inputs on eos/cvmfs using PathResolver
  - Call further functions to process and define NP strings and so on
  - Configure to provide nominal smearings by default
*/
StatusCode CommonDiTauSmearingTool::initialize()
{
  ATH_MSG_INFO( "Initializing CommonDiTauSmearingTool" );

  // only read in histograms once
  if (m_mSF==nullptr)
  {
    std::string sInputFilePath = PathResolverFindCalibFile(m_sInputFilePath);

    m_mSF = new SFMAP();
    std::unique_ptr< TFile > fSF( TFile::Open(sInputFilePath.c_str(), "READ") );
    if(!fSF)
    {
      ATH_MSG_FATAL("Could not open file " << sInputFilePath.c_str());
      return StatusCode::FAILURE;
    }
    ReadInputs(*fSF, m_mSF);
    fSF->Close();
  }

  std::vector<std::string> vInputFilePath;
  split(m_sInputFilePath,'/',vInputFilePath);
  m_sInputFileName = vInputFilePath.back();
  generateSystematicSets();

  // load empty systematic variation by default
  if (applySystematicVariation(CP::SystematicSet()) != CP::SystematicCode::Ok )
    return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

/*
  Retrieve the smearing value and if requested the values for the NP's and add
  this stuff in quadrature. Finally apply the correction to the tau pt of the
  non-const tau.
*/
//______________________________________________________________________________
CP::CorrectionCode CommonDiTauSmearingTool::applyCorrection( xAOD::DiTauJet& xDiTau )
{
  // step out here if we run on data
  if (m_bIsData)
    return CP::CorrectionCode::Ok;

  // check which true state is requestet
  if (!m_bSkipTruthMatchCheck and checkTruthMatch(xDiTau) != m_eCheckTruth)
  {
    return CP::CorrectionCode::Ok;
  }

  double dCorrection = 1.;
  // get standard scale factor
  CP::CorrectionCode tmpCorrectionCode = getValue("sf",
                                                  xDiTau,
                                                  dCorrection);
  // return correction code if histogram is not available
  if (tmpCorrectionCode != CP::CorrectionCode::Ok)
    return tmpCorrectionCode;

  // skip further process if systematic set is empty
  if (m_sSystematicSet->size() > 0)
  {
    // get uncertainties summed in quadrature
    double dTotalSystematic2 = 0;
    double dDirection = 0;
    for (auto syst : *m_sSystematicSet)
    {
      // check if systematic is available
      auto it = m_mSystematicsHistNames.find(syst.basename());

      // get uncertainty value
      double dUncertaintySyst = 0;
      tmpCorrectionCode = getValue(it->second,
                                   xDiTau,
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
    dCorrection *= 1 + dDirection * sqrt(dTotalSystematic2);
  }

  // finally apply correction
  xDiTau.setP4( xDiTau.pt() * dCorrection,
              xDiTau.eta(), xDiTau.phi(), xDiTau.m());
  return CP::CorrectionCode::Ok;
}

/*
  Create a non-const copy of the passed const xDiTau object and apply the
  correction to the non-const copy.
 */
//______________________________________________________________________________
CP::CorrectionCode CommonDiTauSmearingTool::correctedCopy( const xAOD::DiTauJet& xDiTau,
    xAOD::DiTauJet*& xDiTauCopy )
{

  // A sanity check:
  if( xDiTauCopy )
  {
    ATH_MSG_WARNING( "Non-null pointer received. "
                     "There's a possible memory leak!" );
  }

  // Create a new object:
  xDiTauCopy = new xAOD::DiTauJet();
  xDiTauCopy->makePrivateStore( xDiTau );

  // Use the other function to modify this object:
  return applyCorrection( *xDiTauCopy );
}

//=================================PRIVATE-PART=================================

template<class T>
void CommonDiTauSmearingTool::ReadInputs(TFile& fFile, std::map<std::string, T>* mMap)
{
  // initialize function pointer
  m_fX = &TruthLeadPt;
  m_fY = &TruthSubleadPt;
  m_fZ = &TruthDeltaR;

  TKey *kKey;
  TIter itNext(fFile.GetListOfKeys());
  while ((kKey = (TKey*)itNext()))
  {
    TClass *cClass = gROOT->GetClass(kKey->GetClassName());
    std::string sKeyName = kKey->GetName();

    if (!cClass->InheritsFrom("TH3"))
      continue;

    T tObj = (T)kKey->ReadObj();
    tObj->SetDirectory(0);
    (*mMap)[sKeyName] = tObj;
  }
  ATH_MSG_INFO("data loaded from " << fFile.GetName());
}

//______________________________________________________________________________
void CommonDiTauSmearingTool::generateSystematicSets()
{
  // creation of basic string for all NPs, e.g. "TAUS_TRUEHADTAU_SME_TES_"
  std::vector<std::string> vSplitInputFilePath = {};
  split(m_sInputFileName,'_',vSplitInputFilePath);
  std::string sEfficiencyType = vSplitInputFilePath.at(0);
  std::string sTruthType = vSplitInputFilePath.at(1);
  std::transform(sEfficiencyType.begin(), sEfficiencyType.end(), sEfficiencyType.begin(), toupper);
  std::transform(sTruthType.begin(), sTruthType.end(), sTruthType.begin(), toupper);
  std::string sSystematicBaseString = "TAUS_"+sTruthType+"_SME_"+sEfficiencyType+"_";

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

    ATH_MSG_DEBUG("connected histogram base name " << sNP << " with systematic " <<sSystematicString);
    m_mSystematicsHistNames.insert({sSystematicString,sNP});
  }
}

//______________________________________________________________________________
CP::CorrectionCode CommonDiTauSmearingTool::getValue(const std::string& sHistName,
    const xAOD::DiTauJet& xDiTau,
    double& dEfficiencyScaleFactor) const
{
  TH1* hHist = (*m_mSF)[sHistName];
  if (!hHist)
  {
    ATH_MSG_ERROR("Histogram with name "<<sHistName<<" was not found in input file.");
    return CP::CorrectionCode::Error;
  }

  double dX = m_fX(xDiTau);
  double dY = m_fY(xDiTau);
  double dZ = m_fZ(xDiTau);

  // protect values from underflow bins
  dX = std::max(dX,hHist->GetXaxis()->GetXmin());
  dY = std::max(dY,hHist->GetYaxis()->GetXmin());
  dZ = std::max(dZ,hHist->GetZaxis()->GetXmin());
  // protect values from overflow bins (times .999 to keep it inside last bin)
  dX = std::min(dX,hHist->GetXaxis()->GetXmax() * .999);
  dY = std::min(dY,hHist->GetYaxis()->GetXmax() * .999);
  dZ = std::min(dZ,hHist->GetZaxis()->GetXmax() * .999);

  int iBin = hHist->FindFixBin(dX,dY,dZ);
  dEfficiencyScaleFactor = hHist->GetBinContent(iBin);

  return CP::CorrectionCode::Ok;
}

//______________________________________________________________________________
e_TruthMatchedParticleType CommonDiTauSmearingTool::checkTruthMatch(const xAOD::DiTauJet& xDiTau) const
{
  if (!xDiTau.isAvailable<char>("IsTruthHadronic"))
    ATH_MSG_ERROR("No truth match information available. Please run DiTauTruthMatchingTool first");
  static SG::AuxElement::Accessor<char> accIsTruthHadronic("IsTruthHadronic");

  e_TruthMatchedParticleType eTruthMatchedParticleType = Unknown;

  if (accIsTruthHadronic(xDiTau))
    eTruthMatchedParticleType = TruthHadronicDiTau;

  return eTruthMatchedParticleType;
}
