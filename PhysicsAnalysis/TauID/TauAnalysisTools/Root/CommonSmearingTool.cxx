/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework include(s):
#include "PathResolver/PathResolver.h"

// local include(s)
#include "TauAnalysisTools/CommonSmearingTool.h"
#include "TauAnalysisTools/TauSmearingTool.h"
#include "xAODTruth/TruthParticleContainer.h"

// ROOT include(s)
#include "TF1.h"

using namespace TauAnalysisTools;

//______________________________________________________________________________
CommonSmearingTool::CommonSmearingTool(std::string sName)
  : asg::AsgTool( sName )
  , m_mSF(0)
  , m_tTST(0)
  , m_sSystematicSet(0)
  , m_fX(&tauPt)
  , m_fY(&tauEta)
  , m_sInputFilePath("")
  , m_bSkipTruthMatchCheck(false)
  , m_bApplyFading(true)
  , m_eCheckTruth(TauAnalysisTools::Unknown)
  , m_bNoMultiprong(false)
{
  m_mSystematics = {};

  declareProperty("InputFilePath", m_sInputFilePath);
  declareProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck);
  declareProperty("ApplyFading", m_bApplyFading);
}

CommonSmearingTool::~CommonSmearingTool()
{
  if (m_mSF)
    for (auto mEntry : *m_mSF)
      delete mEntry.second;
  delete m_mSF;
}

StatusCode CommonSmearingTool::initialize()
{
  ATH_MSG_INFO( "Initializing CommonSmearingTool" );
  // only read in histograms once
  if (m_mSF==nullptr)
  {
    std::string sInputFilePath = PathResolverFindCalibFile(m_sInputFilePath);

    m_mSF = new SFMAP();
    TFile* fSF = TFile::Open(sInputFilePath.c_str(), "READ");
    if(!fSF)
    {
      ATH_MSG_FATAL("Could not open file " << sInputFilePath.c_str());
      return StatusCode::FAILURE;
    }
    ReadInputs(fSF, m_mSF);
    fSF->Close();
    delete fSF;
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

//______________________________________________________________________________
CP::CorrectionCode CommonSmearingTool::applyCorrection( xAOD::TauJet& xTau )
{
  // check which true state is requestet
  if (!m_bSkipTruthMatchCheck and checkTruthMatch(xTau) != m_eCheckTruth)
  {
    return CP::CorrectionCode::Ok;
  }

  // get prong extension for histogram name
  std::string sProng = ConvertProngToString(xTau.nTracks());

  double dCorrection = 1.;
  // get standard scale factor
  CP::CorrectionCode tmpCorrectionCode;
  tmpCorrectionCode = getValue("sf"+sProng,
                               xTau,
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
      tmpCorrectionCode = getValue(it->second+sProng,
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
    dCorrection *= 1 + dDirection * sqrt(dTotalSystematic2);
  }

  // finally apply correction
  xTau.setP4( xTau.pt() * dCorrection,
              xTau.eta(), xTau.phi(), xTau.m());
  return CP::CorrectionCode::Ok;
}

//______________________________________________________________________________
CP::CorrectionCode CommonSmearingTool::correctedCopy( const xAOD::TauJet& xTau,
    xAOD::TauJet*& xTauCopy )
{

  // A sanity check:
  if( xTauCopy )
  {
    ATH_MSG_WARNING( "Non-null pointer received. "
                     "There's a possible memory leak!" );
  }

  // Create a new object:
  xTauCopy = new xAOD::TauJet();
  xTauCopy->makePrivateStore( xTau );

  // Use the other function to modify this object:
  return applyCorrection( *xTauCopy );
}

//______________________________________________________________________________
bool CommonSmearingTool::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const
{
  CP::SystematicSet sys = affectingSystematics();
  return sys.find (systematic) != sys.end ();
}

//______________________________________________________________________________
CP::SystematicSet CommonSmearingTool::affectingSystematics() const
{
  return m_sAffectingSystematics;
}

//______________________________________________________________________________
CP::SystematicSet CommonSmearingTool::recommendedSystematics() const
{
  return m_sRecommendedSystematics;
}

//______________________________________________________________________________
void CommonSmearingTool::setParent(TauSmearingTool* tTST)
{
  m_tTST = tTST;
}

//______________________________________________________________________________
CP::SystematicCode CommonSmearingTool::applySystematicVariation ( const CP::SystematicSet& sSystematicSet)
{
  // first check if we already know this systematic configuration
  auto itSystematicSet = m_mSystematicSets.find(sSystematicSet);
  if (itSystematicSet != m_mSystematicSets.end())
  {
    m_sSystematicSet = &itSystematicSet->first;
    return CP::SystematicCode::Ok;
  }

  // sanity checks if systematic set is supported
  double dDirection = 0;
  CP::SystematicSet sSystematicSetAvailable;
  for (auto sSyst : sSystematicSet)
  {
    // check if systematic is available
    auto it = m_mSystematicsHistNames.find(sSyst.basename());
    if (it == m_mSystematicsHistNames.end())
    {
      ATH_MSG_VERBOSE("unsupported systematic variation: "<< sSyst.basename()<<"; skipping this one");
      continue;
    }


    if (sSyst.parameter() * dDirection < 0)
    {
      ATH_MSG_ERROR("unsupported set of systematic variations, you should either use only \"UP\" or only \"DOWN\" systematics in one set!");
      ATH_MSG_ERROR("systematic set will not be applied");
      return CP::SystematicCode::Unsupported;
    }
    dDirection = sSyst.parameter();

    if ((m_sRecommendedSystematics.find(sSyst.basename()) != m_sRecommendedSystematics.end()) and sSystematicSet.size() > 1)
    {
      ATH_MSG_ERROR("unsupported set of systematic variations, you should not combine \"TAUS_{TRUE|FAKE}_SME_TOTAL\" with other systematic variations!");
      ATH_MSG_ERROR("systematic set will not be applied");
      return CP::SystematicCode::Unsupported;
    }

    // finally add the systematic to the set of systematics to process
    sSystematicSetAvailable.insert(sSyst);
  }

  // store this calibration for future use, and make it current
  m_sSystematicSet = &m_mSystematicSets.insert(std::pair<CP::SystematicSet,std::string>(sSystematicSetAvailable, sSystematicSet.name())).first->first;

  return CP::SystematicCode::Ok;
}

//=================================PRIVATE-PART=================================
//______________________________________________________________________________
std::string CommonSmearingTool::ConvertProngToString(const int& fProngness)
{
  std::string prong = "";
  if (fProngness == 0)
    ATH_MSG_DEBUG("passed tau with 0 tracks, which is not supported, taking multiprong SF for now");
  fProngness == 1 ? prong = "_1p" : prong = "_3p";
  return prong;
}

//______________________________________________________________________________
template<class T>
void CommonSmearingTool::ReadInputs(TFile* fFile, std::map<std::string, T>* mMap)
{
  // initialize function pointer
  m_fX = &tauPt;
  m_fY = &tauEta;

  TKey *kKey;
  TIter itNext(fFile->GetListOfKeys());
  while ((kKey = (TKey*)itNext()))
  {
    TClass *cClass = gROOT->GetClass(kKey->GetClassName());

    // parse file content for objects of type TNamed, check their title for
    // known strings and reset funtion pointer
    std::string sKeyName = kKey->GetName();
    if (sKeyName == "Xaxis")
    {
      TNamed* tObj = (T)kKey->ReadObj();
      std::string sTitle = tObj->GetTitle();
      delete tObj;
      if (sTitle == "P")
      {
        m_fX = &tauP;
        ATH_MSG_DEBUG("using full momentum for x-axis");
      }
    }
    if (sKeyName == "Yaxis")
    {
      TNamed* tObj = (T)kKey->ReadObj();
      std::string sTitle = tObj->GetTitle();
      delete tObj;
      if (sTitle == "track-eta")
      {
        m_fY = &tauLeadTrackEta;
        ATH_MSG_DEBUG("using leading track eta for y-axis");
      }
      else if (sTitle == "|eta|")
      {
        m_fY = &tauAbsEta;
        ATH_MSG_DEBUG("using absolute tau eta for y-axis");
      }
    }
    if (!cClass->InheritsFrom("TH1"))
      continue;
    T tObj = (T)kKey->ReadObj();
    tObj->SetDirectory(0);
    (*mMap)[sKeyName] = tObj;
  }
  ATH_MSG_INFO("data loaded from " << fFile->GetName());
}

//______________________________________________________________________________
void CommonSmearingTool::generateSystematicSets()
{
  // creation of basic string for all NPs, e.g. "TAUS_TRUEHADTAU_SME_TES_"
  std::vector<std::string> m_vSplitInputFilePath = {};
  split(m_sInputFileName,'_',m_vSplitInputFilePath);
  std::string sEfficiencyType = m_vSplitInputFilePath.at(0);
  std::string sTruthType = m_vSplitInputFilePath.at(1);
  std::transform(sEfficiencyType.begin(), sEfficiencyType.end(), sEfficiencyType.begin(), toupper);
  std::transform(sTruthType.begin(), sTruthType.end(), sTruthType.begin(), toupper);
  std::string sSystematicBaseString = "TAUS_"+sTruthType+"_SME_"+sEfficiencyType+"_";

  // set truth type to check for in truth matching
  if (sTruthType=="TRUEHADTAU") m_eCheckTruth = TauAnalysisTools::TruthHadronicTau;
  if (sTruthType=="TRUEELECTRON") m_eCheckTruth = TauAnalysisTools::TruthElectron;
  if (sTruthType=="TRUEMUON") m_eCheckTruth = TauAnalysisTools::TruthMuon;
  if (sTruthType=="TRUEJET") m_eCheckTruth = TauAnalysisTools::TruthJet;
  if (sEfficiencyType=="ELEOLR") m_bNoMultiprong = true;

  for (auto mSF : *m_mSF)
  {
    // parse for nuisance parameter in histogram name
    std::vector<std::string> m_vSplitNP = {};
    split(mSF.first,'_',m_vSplitNP);
    std::string sNP = m_vSplitNP.at(0);
    std::string sNPUppercase = m_vSplitNP.at(0);

    // skip nominal scale factors
    if (sNP == "sf") continue;
    // skip if non 1p histogram to avoid duplications (TODO: come up with a better solution)
    if (mSF.first.find("_1p") == std::string::npos) continue;

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
CP::CorrectionCode CommonSmearingTool::getValue(const std::string& sHistName,
    const xAOD::TauJet& xTau,
    double& dEfficiencyScaleFactor) const
{
  TH1F* hHist = (*m_mSF)[sHistName];
  if (!hHist)
  {
    ATH_MSG_ERROR("Histogram with name "<<sHistName<<" was not found in input file.");
    return CP::CorrectionCode::Error;
  }

  double dPt = m_fX(xTau);
  double dEta = m_fY(xTau);

  // protect values from underflow bins
  dPt = std::max(dPt,hHist->GetXaxis()->GetXmin());
  dEta = std::max(dEta,hHist->GetYaxis()->GetXmin());
  // protect values from overflow bins (times .999 to keep it inside last bin)
  dPt = std::min(dPt,hHist->GetXaxis()->GetXmax() * .999);
  dEta = std::min(dEta,hHist->GetYaxis()->GetXmax() * .999);

  int iBin = hHist->FindFixBin(dPt,dEta);
  dEfficiencyScaleFactor = hHist->GetBinContent(iBin);

  if (m_bApplyFading)
  {
    std::string sTitle = hHist->GetTitle();
    if (sTitle.size()>0)
    {
      TF1 f("",sTitle.c_str(), 0, 1000);
      if (sHistName.find("sf_") != std::string::npos)
        dEfficiencyScaleFactor = (dEfficiencyScaleFactor -1) *f.Eval(dPt) + 1;
      else
        dEfficiencyScaleFactor *= f.Eval(dPt);
    }
  }
  return CP::CorrectionCode::Ok;
}

//______________________________________________________________________________
e_TruthMatchedParticleType CommonSmearingTool::checkTruthMatch(const xAOD::TauJet& xTau) const
{
  // check if reco tau is a truth hadronic tau
  typedef ElementLink< xAOD::TruthParticleContainer > Link_t;
  if (!xTau.isAvailable< Link_t >("truthParticleLink"))
    ATH_MSG_ERROR("No truth match information available. Please run TauTruthMatchingTool first.");

  static SG::AuxElement::Accessor<Link_t> accTruthParticleLink("truthParticleLink");
  const Link_t xTruthParticleLink = accTruthParticleLink(xTau);

  // if there is no link, then it is a truth jet
  e_TruthMatchedParticleType eTruthMatchedParticleType = TauAnalysisTools::TruthJet;

  if (xTruthParticleLink.isValid())
  {
    const xAOD::TruthParticle* xTruthParticle = *xTruthParticleLink;
    if (xTruthParticle->isTau())
    {
      static SG::AuxElement::ConstAccessor<char> accIsHadronicTau("IsHadronicTau");
      if ((bool)accIsHadronicTau(*xTruthParticle))
      {
        eTruthMatchedParticleType = TruthHadronicTau;
      }
    }
    else if (xTruthParticle->isElectron())
      eTruthMatchedParticleType = TruthElectron;
    else if (xTruthParticle->isMuon())
      eTruthMatchedParticleType = TruthMuon;
  }
  else
    ATH_MSG_VERBOSE("Truth particle link is not valid");

  return eTruthMatchedParticleType;
}
