/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework include(s):
#include "PathResolver/PathResolver.h"

// local include(s)
#include "TauAnalysisTools/CommonEfficiencyTool.h"
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"
#include "xAODTruth/TruthParticleContainer.h"

// ROOT include(s)
#include "TH2F.h"

using namespace TauAnalysisTools;

//______________________________________________________________________________
CommonEfficiencyTool::CommonEfficiencyTool(std::string sName)
  : asg::AsgTool( sName )
  , m_mSF(0)
  , m_tTECT(0)
  , m_sSystematicSet(0)
  , m_fX(&tauPt)
  , m_fY(&tauEta)
  , m_sInputFilePath("")
  , m_sWP("")
  , m_sVarName("")
  , m_bSkipTruthMatchCheck(false)
  , m_sSFHistName("sf")
  , m_eCheckTruth(TauAnalysisTools::Unknown)
  , m_bNoMultiprong(false)
  , m_bSFIsAvailable(false)
  , m_bSFIsAvailableChecked(false)
{
  m_mSystematics = {};

  declareProperty( "InputFilePath", m_sInputFilePath );
  declareProperty( "VarName", m_sVarName );
  declareProperty( "SkipTruthMatchCheck", m_bSkipTruthMatchCheck );
  declareProperty( "WP", m_sWP );
}

CommonEfficiencyTool::~CommonEfficiencyTool()
{
  if (m_mSF)
    for (auto mEntry : *m_mSF)
      delete std::get<0>(mEntry.second);
  delete m_mSF;
}

StatusCode CommonEfficiencyTool::initialize()
{
  ATH_MSG_INFO( "Initializing CommonEfficiencyTool" );
  // only read in histograms once
  if (m_mSF==nullptr)
  {
    std::string sInputFilePath = PathResolverFindCalibFile(m_sInputFilePath);

    m_mSF = new tSFMAP();
    TFile* fSF = TFile::Open(sInputFilePath.c_str(), "READ");
    if(!fSF)
    {
      ATH_MSG_FATAL("Could not open file " << sInputFilePath.c_str());
      return StatusCode::FAILURE;
    }
    ReadInputs(fSF);
    fSF->Close();
    delete fSF;
  }

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

//______________________________________________________________________________
CP::CorrectionCode CommonEfficiencyTool::getEfficiencyScaleFactor(const xAOD::TauJet& xTau,
    double& dEfficiencyScaleFactor)
{
  // check which true state is requestet
  if (!m_bSkipTruthMatchCheck and checkTruthMatch(xTau) != m_eCheckTruth)
  {
    dEfficiencyScaleFactor = 1.;
    return CP::CorrectionCode::Ok;
  }

  // check if 1 prong
  if (m_bNoMultiprong && xTau.nTracks() != 1)
  {
    dEfficiencyScaleFactor = 1.;
    return CP::CorrectionCode::Ok;
  }

  // get prong extension for histogram name
  std::string sProng = ConvertProngToString(xTau.nTracks());

  // get standard scale factor
  CP::CorrectionCode tmpCorrectionCode;
  tmpCorrectionCode = getValue(m_sSFHistName+sProng,
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

    // needed for up/down decision
    dDirection = syst.parameter();

    // build up histogram name
    std::string sHistName = it->second;
    if (dDirection>0)   sHistName+="_up";
    else                sHistName+="_down";
    if (!m_sWP.empty()) sHistName+="_"+m_sWP;
    sHistName += sProng;

    // get the uncertainty from the histogram
    tmpCorrectionCode = getValue(sHistName,
                                 xTau,
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

//______________________________________________________________________________
CP::CorrectionCode CommonEfficiencyTool::applyEfficiencyScaleFactor(const xAOD::TauJet& xTau)
{
  double dSf = 0.;

  if (!m_bSFIsAvailableChecked)
  {
    m_bSFIsAvailable = xTau.isAvailable< double >(m_sVarName);
    m_bSFIsAvailableChecked = true;
    if (m_bSFIsAvailable)
    {
      ATH_MSG_DEBUG(m_sVarName << " decoration is available on first tau processed, switched of applyEfficiencyScaleFactor for further taus.");
      ATH_MSG_DEBUG("If an application of efficiency scale factors needs to be redone, please pass a shallow copy of the original tau.");
    }
  }
  if (m_bSFIsAvailable)
    return CP::CorrectionCode::Ok;

  // retreive scale factor
  CP::CorrectionCode tmpCorrectionCode = getEfficiencyScaleFactor(xTau, dSf);
  // adding scale factor to tau as decoration
  xTau.auxdecor<double>(m_sVarName) = dSf;

  return tmpCorrectionCode;
}

//______________________________________________________________________________
bool CommonEfficiencyTool::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const
{
  CP::SystematicSet sys = affectingSystematics();
  return sys.find (systematic) != sys.end ();
}

//______________________________________________________________________________
CP::SystematicSet CommonEfficiencyTool::affectingSystematics() const
{
  return m_sAffectingSystematics;
}

//______________________________________________________________________________
CP::SystematicSet CommonEfficiencyTool::recommendedSystematics() const
{
  return m_sRecommendedSystematics;
}

//______________________________________________________________________________
void CommonEfficiencyTool::setParent(TauEfficiencyCorrectionsTool* tTECT)
{
  m_tTECT = tTECT;
}

//______________________________________________________________________________
CP::SystematicCode CommonEfficiencyTool::applySystematicVariation ( const CP::SystematicSet& sSystematicSet)
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
      ATH_MSG_ERROR("unsupported set of systematic variations, you should not combine \"TAUS_{TRUE|FAKE}_EFF_*_TOTAL\" with other systematic variations!");
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
std::string CommonEfficiencyTool::ConvertProngToString(const int& fProngness)
{
  std::string prong = "";
  if (fProngness == 0)
    ATH_MSG_DEBUG("passed tau with 0 tracks, which is not supported, taking multiprong SF for now");
  fProngness == 1 ? prong = "_1p" : prong = "_3p";
  return prong;
}

//______________________________________________________________________________
void CommonEfficiencyTool::ReadInputs(TFile* fFile)
{
  m_mSF->clear();

  // initialize function pointer
  m_fX = &tauPt;
  m_fY = &tauEta;

  TKey *kKey;
  TIter itNext(fFile->GetListOfKeys());
  while ((kKey = (TKey*)itNext()))
  {
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
        m_fX = &tauP;
        ATH_MSG_DEBUG("using full momentum for x-axis");
      }
      continue;
    }
    else if (sKeyName == "Yaxis")
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
        m_fY = &tauAbsEta;
        ATH_MSG_DEBUG("using absolute tau eta for y-axis");
      }
      continue;
    }

    std::vector<std::string> m_vSplitName = {};
    split(sKeyName,'_',m_vSplitName);
    if (m_vSplitName[0] == "sf")
    {
      addHistogramToSFMap(kKey, sKeyName);
    }
    else
    {
      // std::string sDirection = m_vSplitName[1];
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

void CommonEfficiencyTool::addHistogramToSFMap(TKey* kKey, const std::string& sKeyName)
{
  // handling for the 3 different input types TH1F/TH1D/TF1, function pointer
  // handle the access methods for the final scale factor retrieval
  TClass *cClass = gROOT->GetClass(kKey->GetClassName());
  if (cClass->InheritsFrom("TH2"))
  {
    TH1F* oObject = (TH1F*)kKey->ReadObj();
    oObject->SetDirectory(0);
    if (cClass->InheritsFrom("TH2D"))
      (*m_mSF)[sKeyName] = tTupleObjectFunc(oObject,&getValueTH2D);
    else
      (*m_mSF)[sKeyName] = tTupleObjectFunc(oObject,&getValueTH2F);
    ATH_MSG_DEBUG("added histogram with name "<<sKeyName);
  }
  else if (cClass->InheritsFrom("TF1"))
  {
    TObject* oObject = kKey->ReadObj();
    (*m_mSF)[sKeyName] = tTupleObjectFunc(oObject,&getValueTF1);
    ATH_MSG_DEBUG("added function with name "<<sKeyName);
  }
  else
  {
    ATH_MSG_DEBUG("ignored object with name "<<sKeyName);
  }
}

//______________________________________________________________________________
void CommonEfficiencyTool::generateSystematicSets()
{
  // creation of basic string for all NPs, e.g. "TAUS_TRUEHADTAU_EFF_RECO_"
  std::vector<std::string> m_vSplitInputFilePath = {};
  split(m_sInputFileName,'_',m_vSplitInputFilePath);
  std::string sEfficiencyType = m_vSplitInputFilePath.at(0);
  std::string sTruthType = m_vSplitInputFilePath.at(1);
  std::transform(sEfficiencyType.begin(), sEfficiencyType.end(), sEfficiencyType.begin(), toupper);
  std::transform(sTruthType.begin(), sTruthType.end(), sTruthType.begin(), toupper);
  std::string sSystematicBaseString = "TAUS_"+sTruthType+"_EFF_"+sEfficiencyType+"_";

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

    ATH_MSG_DEBUG("connected base name " << sNP << " with systematic " <<sSystematicString);
    m_mSystematicsHistNames.insert({sSystematicString,sNP});
  }
}

//______________________________________________________________________________
CP::CorrectionCode CommonEfficiencyTool::getValue(const std::string& sHistName,
    const xAOD::TauJet& xTau,
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
  double dPt = m_fX(xTau);
  double dEta = m_fY(xTau);

  // finally obtain efficiency scale factor from TH1F/TH1D/TF1, by calling the
  // function pointer stored in the tuple from the scale factor map
  return  (std::get<1>(tTuple))(std::get<0>(tTuple), dEfficiencyScaleFactor, dPt, dEta);
}

//______________________________________________________________________________
CP::CorrectionCode CommonEfficiencyTool::getValueTH2F(const TObject* oObject,
    double& dEfficiencyScaleFactor,
    double dPt,
    double dEta)
{
  const TH2F* hHist = dynamic_cast<const TH2F*>(oObject);

  if (!hHist)
  {
    // ATH_MSG_ERROR("Problem with casting TObject of type "<<oObject->ClassName()<<" to TH2F");
    return CP::CorrectionCode::Error;
  }

  // protect values from underflow bins
  dPt = std::max(dPt,hHist->GetXaxis()->GetXmin());
  dEta = std::max(dEta,hHist->GetYaxis()->GetXmin());
  // protect values from overflow bins (times .999 to keep it inside last bin)
  dPt = std::min(dPt,hHist->GetXaxis()->GetXmax() * .999);
  dEta = std::min(dEta,hHist->GetYaxis()->GetXmax() * .999);

  // get bin from TH2 depending in x and y values; finally set the scale factor
  int iBin = hHist->FindFixBin(dPt,dEta);
  dEfficiencyScaleFactor = hHist->GetBinContent(iBin);
  return CP::CorrectionCode::Ok;
}

//______________________________________________________________________________
CP::CorrectionCode CommonEfficiencyTool::getValueTH2D(const TObject* oObject,
    double& dEfficiencyScaleFactor,
    double dPt,
    double dEta)
{
  const TH2D* hHist = dynamic_cast<const TH2D*>(oObject);

  if (!hHist)
  {
    // ATH_MSG_ERROR("Problem with casting TObject of type "<<oObject->ClassName()<<" to TH2D");
    return CP::CorrectionCode::Error;
  }

  // protect values from underflow bins
  dPt = std::max(dPt,hHist->GetXaxis()->GetXmin());
  dEta = std::max(dEta,hHist->GetYaxis()->GetXmin());
  // protect values from overflow bins (times .999 to keep it inside last bin)
  dPt = std::min(dPt,hHist->GetXaxis()->GetXmax() * .999);
  dEta = std::min(dEta,hHist->GetYaxis()->GetXmax() * .999);

  // get bin from TH2 depending in x and y values; finally set the scale factor
  int iBin = hHist->FindFixBin(dPt,dEta);
  dEfficiencyScaleFactor = hHist->GetBinContent(iBin);
  return CP::CorrectionCode::Ok;
}

//______________________________________________________________________________
CP::CorrectionCode CommonEfficiencyTool::getValueTF1(const TObject* oObject,
    double& dEfficiencyScaleFactor,
    double dPt,
    double dEta)
{
  const TF1* fFunc = static_cast<const TF1*>(oObject);

  if (!fFunc)
  {
    // ATH_MSG_ERROR("Problem with casting TObject of type "<<oObject->ClassName()<<" to TF1");
    return CP::CorrectionCode::Error;
  }

  // evaluate TFunction and set scale factor
  dEfficiencyScaleFactor = fFunc->Eval(dPt, dEta);
  return CP::CorrectionCode::Ok;
}

//______________________________________________________________________________
e_TruthMatchedParticleType CommonEfficiencyTool::checkTruthMatch(const xAOD::TauJet& xTau) const
{
  // check if reco tau is a truth hadronic tau
  typedef ElementLink< xAOD::TruthParticleContainer > Link_t;
  if (!xTau.isAvailable< Link_t >("truthParticleLink"))
    ATH_MSG_ERROR("No truth match information available. Please run TauTruthMatchingTool first");

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
