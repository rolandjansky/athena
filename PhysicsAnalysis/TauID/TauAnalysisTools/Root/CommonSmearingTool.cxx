/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Framework include(s):
#include "PathResolver/PathResolver.h"

// local include(s)
#include "TauAnalysisTools/CommonSmearingTool.h"
#include "TauAnalysisTools/TauSmearingTool.h"
#include "xAODTruth/TruthParticleContainer.h"

// ROOT include(s)
#include "TF1.h"

// tauRecTools include(s)
#include "tauRecTools/MvaTESVariableDecorator.h"
#include "tauRecTools/MvaTESEvaluator.h"
#include "tauRecTools/CombinedP4FromRecoTaus.h"

using namespace TauAnalysisTools;
/*
  This tool acts as a common tool to apply tau energy smearing and
  uncertainties. By default, only nominal smearings without systematic
  variations are applied. Unavailable systematic variations are ignored, meaning
  that the tool only returns the nominal value. In case the one available
  systematic is requested, the smeared scale factor is computed as:
    - pTsmearing = pTsmearing_nominal +/- n * uncertainty

  where n is in general 1 (representing a 1 sigma smearing), but can be any
  arbitrary value. In case multiple systematic variations are passed they are
  added in quadrature. Note that it's currently only supported if all are up or
  down systematics.

  The tool reads in root files including TH2 histograms which need to fulfill a
  predefined structure:

  nominal smearing:
    - sf_<workingpoint>_<prongness>p
  uncertainties:
    - <NP>_<up/down>_<workingpoint>_<prongness>p (for asymmetric uncertainties)
    - <NP>_<workingpoint>_<prongness>p (for symmetric uncertainties)

  where the <workingpoint> (e.g. loose/medium/tight) fields may be
  optional. <prongness> represents either 1 or 3, whereas 3 is currently used
  for multiprong in general. The <NP> fields are names for the type of nuisance
  parameter (e.g. STAT or SYST), note the tool decides whether the NP is a
  recommended or only an available systematic based on the first character:
    - uppercase -> recommended
    - lowercase -> available
  This magic happens here:
    - CommonSmearingTool::generateSystematicSets()

  In addition the root input file can also contain objects of type TF1 that can
  be used to provide kind of unbinned smearings or systematics. Currently there
  is no usecase for tau energy smearing

  The files may also include TNamed objects which is used to define how x and
  y-axes should be treated. By default the x-axis is given in units of tau-pT in
  GeV and the y-axis is given as tau-eta. If there is for example a TNamed
  object with name "Yaxis" and title "|eta|" the y-axis is treated in units of
  absolute tau eta. All this is done in:
    - void CommonSmearingTool::ReadInputs(TFile* fFile)

  Other tools for scale factors may build up on this tool and overwrite or add
  particular functionality.
*/

//______________________________________________________________________________
CommonSmearingTool::CommonSmearingTool(std::string sName)
  : asg::AsgMetadataTool( sName )
  , m_mSF(0)
  , m_sSystematicSet(0)
  , m_fX(&caloTauPt)
  , m_fY(&caloTauEta)
  , m_bIsData(false)
  , m_bIsConfigured(false)
  , m_tMvaTESVariableDecorator("MvaTESVariableDecorator", this)
  , m_tMvaTESEvaluator("MvaTESEvaluator", this)
  , m_tCombinedP4FromRecoTaus("CombinedP4FromRecoTaus", this)
  , m_eCheckTruth(TauAnalysisTools::Unknown)
  , m_bNoMultiprong(false)
  , m_bPtTauEtaCalibIsAvailable(false)
  , m_bPtTauEtaCalibIsAvailableIsChecked(false)
{
  m_mSystematics = {};

  declareProperty("InputFilePath",       m_sInputFilePath       = "" );
  declareProperty("SkipTruthMatchCheck", m_bSkipTruthMatchCheck = false );
  declareProperty("ApplyFading",         m_bApplyFading         = true );
  declareProperty("ApplyMVATES",         m_bApplyMVATES         = true );
  declareProperty("ApplyCombinedTES",    m_bApplyCombinedTES    = false );
  declareProperty("ApplyMVATESQualityCheck", m_bApplyMVATESQualityCheck = true );
  declareProperty("ApplyInsituCorrection",   m_bApplyInsituCorrection   = true );
}

/*
  need to clear the map of histograms cause we have the ownership, not ROOT
*/
CommonSmearingTool::~CommonSmearingTool()
{
  if (m_mSF)
    for (auto mEntry : *m_mSF)
      delete mEntry.second;
  delete m_mSF;
}

/*
  - Find the root files with smearing inputs on afs/cvmfs using PathResolver
    (more info here:
    https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/PathResolver)
  - Call further functions to process and define NP strings and so on
  - Configure to provide nominal smearings by default
*/
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

  if (m_bApplyCombinedTES || m_bApplyMVATES) // CombinedTES has to be available for MVA fix
  {
    ATH_CHECK(ASG_MAKE_ANA_TOOL(m_tCombinedP4FromRecoTaus, CombinedP4FromRecoTaus));
    ATH_CHECK(m_tCombinedP4FromRecoTaus.setProperty("WeightFileName", "CalibLoopResult_v04-04.root"));
    ATH_CHECK(m_tCombinedP4FromRecoTaus.initialize());
  }


  return StatusCode::SUCCESS;
}

/*
  Retrieve the smearing value and if requested the values for the NP's and add
  this stuff in quadrature. Finally apply the correction to the tau pt of the
  non-const tau.
*/
//______________________________________________________________________________
CP::CorrectionCode CommonSmearingTool::applyCorrection( xAOD::TauJet& xTau )
{
  if (not m_bPtTauEtaCalibIsAvailableIsChecked)
  {
    m_bPtTauEtaCalibIsAvailable = xTau.isAvailable<float>("ptTauEtaCalib");
    m_bPtTauEtaCalibIsAvailableIsChecked = true;
  }

  // save calo based TES before another TES is applied
  if (not m_bPtTauEtaCalibIsAvailable) 
  {
    xTau.auxdecor<float>("ptTauEtaCalib") = xTau.pt();
    xTau.auxdecor<float>("etaTauEtaCalib") = xTau.eta();
    xTau.auxdecor<float>("phiTauEtaCalib") = xTau.phi();
    xTau.auxdecor<float>("mTauEtaCalib") = xTau.m();
  }

  if (m_bApplyMVATES)
  {    
    // veto MVA TES for unreasonably low resolution values
    bool bVeto = dynamic_cast<CombinedP4FromRecoTaus*>(m_tCombinedP4FromRecoTaus.get())->GetUseCaloPtFlag(&xTau);

    if (xTau.nTracks() > 0 and xTau.nTracks() < 6)
    {
      static SG::AuxElement::ConstAccessor<float> accPtFinalCalib("ptFinalCalib");
      static SG::AuxElement::ConstAccessor<float> accEtaFinalCalib("etaFinalCalib");
      static SG::AuxElement::ConstAccessor<float> accPhiFinalCalib("phiFinalCalib");
      static SG::AuxElement::ConstAccessor<float> accMFinalCalib("mFinalCalib");

      xTau.auxdecor<char>("MVATESQuality") = (char)bVeto;
      if (bVeto && m_bApplyMVATESQualityCheck)
      {
        ATH_MSG_DEBUG("veto against MVA TES");
        xTau.auxdata<float>("ptFinalCalib") = xTau.pt();
        xTau.auxdata<float>("etaFinalCalib") = xTau.eta();
        xTau.auxdata<float>("phiFinalCalib") = xTau.phi();
        xTau.auxdata<float>("mFinalCalib") = xTau.m();
      }
      else
      {
        xTau.setP4(accPtFinalCalib(xTau),
                   accEtaFinalCalib(xTau),
                   accPhiFinalCalib(xTau),
                   accMFinalCalib(xTau));
      }
    }
  }

  if (m_bApplyCombinedTES)
  {
    // TODO: only call eventInitialize once per event, probably via migration to
    // AsgMetadataTool
    if (m_tCombinedP4FromRecoTaus->eventInitialize().isFailure())
      return CP::CorrectionCode::Error;
    if (m_tCombinedP4FromRecoTaus->execute(xTau).isFailure())
      return CP::CorrectionCode::Error;

    if (xTau.nTracks() > 0 and xTau.nTracks() < 6)
    {
      xTau.setP4(xTau.auxdata<float>("pt_combined"),
                 xTau.auxdata<float>("eta_combined"),
                 xTau.auxdata<float>("phi_combined"),
                 xTau.auxdata<float>("m_combined"));
    }
  }

  // step out here if we run on data
  if (m_bIsData)
    return CP::CorrectionCode::Ok;

  // check which true state is requestet
  if (!m_bSkipTruthMatchCheck and checkTruthMatch(xTau) != m_eCheckTruth)
  {
    return CP::CorrectionCode::Ok;
  }

  // get prong extension for histogram name
  std::string sProng = ConvertProngToString(xTau.nTracks());

  double dCorrection = 1.;
  CP::CorrectionCode tmpCorrectionCode;
  if (m_bApplyInsituCorrection)
  {
    // get standard scale factor
    tmpCorrectionCode = getValue("sf"+sProng,
                                                    xTau,
                                                    dCorrection);
    // return correction code if histogram is not available
    if (tmpCorrectionCode != CP::CorrectionCode::Ok)
      return tmpCorrectionCode;
  }

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

/*
  Create a non-const copy of the passed const xTau object and apply the
  correction to the non-const copy.
 */
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

/*
  standard check if a systematic is available
*/
//______________________________________________________________________________
bool CommonSmearingTool::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const
{
  CP::SystematicSet sys = affectingSystematics();
  return sys.find (systematic) != sys.end ();
}

/*
  standard way to return systematics that are available (including recommended
  systematics)
*/
//______________________________________________________________________________
CP::SystematicSet CommonSmearingTool::affectingSystematics() const
{
  return m_sAffectingSystematics;
}

/*
  standard way to return systematics that are recommended
*/
//______________________________________________________________________________
CP::SystematicSet CommonSmearingTool::recommendedSystematics() const
{
  return m_sRecommendedSystematics;
}

/*
  Configure the tool to use a systematic variation for further usage, until the
  tool is reconfigured with this function. The passed systematic set is checked
  for sanity:
    - unsupported systematics are skipped
    - only combinations of up or down supported systematics is allowed
    - don't mix recommended systematics with other available systematics, cause
      sometimes recommended are a quadratic sum of the other variations,
      e.g. TOTAL=(SYST^2 + STAT^2)^0.5
*/
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
/*
  Executed at the beginning of each event. Checks if the tool is used on data or
  MC, which is necessary in some cases, wher taus on data are corrected as
  well. (e.g. when using the MVA TES)
*/
//______________________________________________________________________________
StatusCode CommonSmearingTool::beginEvent()
{
  if (m_bIsConfigured)
    return StatusCode::SUCCESS;

  const xAOD::EventInfo* xEventInfo = 0;
  ATH_CHECK(evtStore()->retrieve(xEventInfo,"EventInfo"));
  m_bIsData = !(xEventInfo->eventType( xAOD::EventInfo::IS_SIMULATION));
  m_bIsConfigured=true;

  return StatusCode::SUCCESS;
}

/*
  prongness converter, note that it returns "_3p" for all values, except
  fProngness==1, i.e. for 0, 2, 3, 4, 5...
 */
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
  m_fX = &caloTauPt;
  m_fY = &caloTauEta;

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
      TNamed* tObj = (TNamed*)kKey->ReadObj();
      std::string sTitle = tObj->GetTitle();
      delete tObj;
      if (sTitle == "P")
      {
        m_fX = &caloTauP;
        ATH_MSG_DEBUG("using full momentum for x-axis");
      }
    }
    if (sKeyName == "Yaxis")
    {
      TNamed* tObj = (TNamed*)kKey->ReadObj();
      std::string sTitle = tObj->GetTitle();
      delete tObj;
      if (sTitle == "track-eta")
      {
        m_fY = &tauLeadTrackEta;
        ATH_MSG_DEBUG("using leading track eta for y-axis");
      }
      else if (sTitle == "|eta|")
      {
        m_fY = &caloTauAbsEta;
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
  TH1* hHist = (*m_mSF)[sHistName];
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
        dEfficiencyScaleFactor = (dEfficiencyScaleFactor -1) *f.Eval(m_fX(xTau)) + 1;
      else
        dEfficiencyScaleFactor *= f.Eval(m_fX(xTau));
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
