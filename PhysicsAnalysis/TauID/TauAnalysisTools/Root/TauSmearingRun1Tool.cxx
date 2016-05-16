/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework include(s):
#include "PathResolver/PathResolver.h"

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "PATInterfaces/SystematicRegistry.h"

// Local include(s):
#include "TauAnalysisTools/TauSmearingRun1Tool.h"
#include "TauAnalysisTools/TauSmearingTool.h"
#include "TauAnalysisTools/SharedFilesVersion.h"

using namespace TauAnalysisTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
TauSmearingRun1Tool::TauSmearingRun1Tool( const std::string& sName )
  : CommonSmearingTool(sName)
  , GeV(1000.)
  , m_iEtaBins(5)
  , m_dInSituStat1P(0.013)
  , m_dInSituStat3P(0.014)
  , m_dInSituSyst1P(0.006)
  , m_dInSituSyst3P(0.007)
{
  m_sSystematicSet = new CP::SystematicSet();
  m_sInputFilePath = "TauAnalysisTools/"+std::string(sSharedFilesVersion)+"/Smearing/mc12_p1344_medium.root";
  m_vEtaBins = {0.,0.3,0.8,1.3,1.6,2.5};
  m_vPF_EtaBins = {"0_03","03_08","08_13","13_16","16_25"};
}

//______________________________________________________________________________
TauSmearingRun1Tool::~TauSmearingRun1Tool()
{
  std::map<std::string, TH1D*>::iterator it = m_hHisto.begin();
  for (; it != m_hHisto.end(); ++it)
    if((*it).second)
      delete (*it).second;
}

//______________________________________________________________________________
StatusCode TauSmearingRun1Tool::initialize()
{
  ATH_MSG_INFO( "Initializing TauSmearingRun1Tool" );
  if (loadInputHistograms() == StatusCode::FAILURE)
    return StatusCode::FAILURE;

  // Add the affecting systematics to the global registry
  CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
  if (!registry.registerSystematics(*this))
  {
    ATH_MSG_ERROR ("Unable to register the systematics");
    return StatusCode::FAILURE;
  }

  m_mSystematics =
  {
    {"TAUS_SME_INSITUSTAT", INSITUSTAT}
    , {"TAUS_SME_INSITUSYST", INSITUSYST}
    , {"TAUS_SME_INSITUSTATINTERPOL", INSITUSTATINTERPOL}
    , {"TAUS_SME_SINGLEPARTICLEINTERPOL", SINGLEPARTICLEINTERPOL}
    , {"TAUS_SME_INSITUSYSTINTERPOL", INSITUSYSTINTERPOL}
    , {"TAUS_SME_SYSREST", SYSREST}
    , {"TAUS_SME_MODELING", MODELING}
    , {"TAUS_SME_CLOSURE", CLOSURE}
    , {"TAUS_SME_SINGLEPARTICLE", SINGLEPARTICLE}
    , {"TAUS_SME_INSITU", INSITU}
    , {"TAUS_SME_INSITUINTERPOL", INSITUINTERPOL}
    , {"TAUS_SME_TOTAL", TOTAL}
  };

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
CP::CorrectionCode TauSmearingRun1Tool::applyCorrection( xAOD::TauJet& xTau )
{
  if (m_tTST->m_bIsData)
  {
    // apply TES shift only in data
    xTau.setP4( ( 1 + getTESShift(xTau.pt(), xTau.nTracks()) ) * xTau.pt(),
                xTau.eta(), xTau.phi(), xTau.m());
  }
  else
  {
    double dUncertainty = 1.;
    for (auto syst : *m_sSystematicSet)
    {
      // up/down systematic depend on sign of systematic type (see
      // affectingSystematics() and recommendedSystematics())
      auto it = m_mSystematics.find(syst.basename());
      if (it == m_mSystematics.end())
      {
        ATH_MSG_VERBOSE("unsupported systematic variation: skipping this one");
        continue;
      }
      dUncertainty *= (1 + syst.parameter() * getTESUncertainty(xTau.pt(),
                       xTau.eta(),
                       xTau.nTracks(),
                       it->second));
    }
    ATH_MSG_VERBOSE( "TES uncertainty: " << dUncertainty );
    // finally apply uncertainty to tau pt
    xTau.setP4( xTau.pt() * dUncertainty,
                xTau.eta(), xTau.phi(), xTau.m());
  }

  return CP::CorrectionCode::Ok;
}

CP::SystematicSet TauSmearingRun1Tool::affectingSystematics() const
{
  CP::SystematicSet result = recommendedSystematics();
  result.insert (CP::SystematicVariation ("TAUS_SME_INSITUSTAT", 1));
  result.insert (CP::SystematicVariation ("TAUS_SME_INSITUSTAT", -1));
  result.insert (CP::SystematicVariation ("TAUS_SME_INSITUSYST", 1));
  result.insert (CP::SystematicVariation ("TAUS_SME_INSITUSYST", -1));
  result.insert (CP::SystematicVariation ("TAUS_SME_INSITUSTATINTERPOL", 1));
  result.insert (CP::SystematicVariation ("TAUS_SME_INSITUSTATINTERPOL", -1));
  result.insert (CP::SystematicVariation ("TAUS_SME_SINGLEPARTICLEINTERPOL", 1));
  result.insert (CP::SystematicVariation ("TAUS_SME_SINGLEPARTICLEINTERPOL", -1));
  result.insert (CP::SystematicVariation ("TAUS_SME_INSITUSYSTINTERPOL", 1));
  result.insert (CP::SystematicVariation ("TAUS_SME_INSITUSYSTINTERPOL", -1));
  result.insert (CP::SystematicVariation ("TAUS_SME_SYSREST", 1));
  result.insert (CP::SystematicVariation ("TAUS_SME_SYSREST", -1));
  result.insert (CP::SystematicVariation ("TAUS_SME_MODELING", 1));
  result.insert (CP::SystematicVariation ("TAUS_SME_MODELING", -1));
  result.insert (CP::SystematicVariation ("TAUS_SME_CLOSURE", 1));
  result.insert (CP::SystematicVariation ("TAUS_SME_CLOSURE", -1));
  result.insert (CP::SystematicVariation ("TAUS_SME_SINGLEPARTICLE", 1));
  result.insert (CP::SystematicVariation ("TAUS_SME_SINGLEPARTICLE", -1));
  result.insert (CP::SystematicVariation ("TAUS_SME_INSITU", 1));
  result.insert (CP::SystematicVariation ("TAUS_SME_INSITU", -1));
  result.insert (CP::SystematicVariation ("TAUS_SME_INSITUINTERPOL", 1));
  result.insert (CP::SystematicVariation ("TAUS_SME_INSITUINTERPOL", -1));

  return result;
}

CP::SystematicSet TauSmearingRun1Tool::recommendedSystematics() const
{
  CP::SystematicSet result;
  result.insert (CP::SystematicVariation ("TAUS_SME_TOTAL", 1));
  result.insert (CP::SystematicVariation ("TAUS_SME_TOTAL", -1));
  return result;
}

CP::SystematicCode TauSmearingRun1Tool::applySystematicVariation ( const CP::SystematicSet& sSystematicSet)
{
  // first check if we already know this systematic configuration
  auto itSystematicSet = m_mSystematicSets.find(sSystematicSet);
  if (itSystematicSet != m_mSystematicSets.end())
  {
    m_sSystematicSet = &itSystematicSet->first;
    return CP::SystematicCode::Ok;
  }
  // store this calibration for future use, and make it current
  m_sSystematicSet = &m_mSystematicSets.insert(std::pair<CP::SystematicSet,std::string>(sSystematicSet, sSystematicSet.name())).first->first;
  return CP::SystematicCode::Ok;
}

//=================================PRIVATE-PART=================================
//______________________________________________________________________________
StatusCode TauSmearingRun1Tool::loadInputHistograms()
{
  std::string sInputFilePath = PathResolverFindCalibFile(m_sInputFilePath);
  if(sInputFilePath.empty())
  {
    ATH_MSG_FATAL("PathResolver could not find file " << sInputFilePath);
    return StatusCode::FAILURE;
  }
  TFile* fFile = TFile::Open(sInputFilePath.c_str());
  if(!fFile || !fFile->IsOpen())
  {
    ATH_MSG_FATAL("Could not open file " << sInputFilePath);
    return StatusCode::FAILURE;
  }

  m_mComponents[FINAL] = "TESUFinal_";
  m_mComponents[SINGLEPARTICLE] = "TESUsinglep_";

  m_mComponents[EOP_RSP] = "TESUEoP_Rsp_";
  m_mComponents[EOP_SYST] = "TESUEoP_syst_";
  m_mComponents[CTB_RSP] = "TESUCTB_Rsp_";
  m_mComponents[CTB_SYST] = "TESUCTB_syst_";
  m_mComponents[EM] = "TESUEM_";
  m_mComponents[LCW] = "TESUShowerShape_";
  m_mComponents[Bias] = "TESUBias_";

  m_mComponents[UE] = "TESUue_";
  m_mComponents[DM] = "TESUdm_";
  m_mComponents[CLOSURE] = "TESUclosure_";
  m_mComponents[PU] = "TESUpu_";
  m_mComponents[SHOWERMODEL] = "TESUshowerm_";
  m_mComponents[REMAININGSYS] = "TESRemainingSys_";
  for(unsigned int ieta = 0; ieta < m_iEtaBins; ++ieta)
  {
    for(int itrack = 0; itrack < 2; ++itrack)
    {
      getHist(fFile, convertConfigToString(FINAL,itrack, ieta));
      getHist(fFile, convertConfigToString(SINGLEPARTICLE,itrack, ieta));
      getHist(fFile, convertConfigToString(UE,itrack, ieta));
      getHist(fFile, convertConfigToString(DM,itrack, ieta));
      getHist(fFile, convertConfigToString(CLOSURE,itrack, ieta));
      getHist(fFile, convertConfigToString(PU,itrack, ieta));

      if(ieta > 1)
        getHist(fFile, convertConfigToString(SHOWERMODEL,itrack, ieta));
      else
      {
        getHist(fFile,
                convertConfigToString(FINAL,itrack, ieta),
                convertConfigToString(SHOWERMODEL,itrack, ieta));
        m_hHisto[convertConfigToString(SHOWERMODEL,itrack, ieta)]->Reset("ICESM");
      }


      getHist(fFile,
              convertConfigToString(UE,itrack, ieta),
              convertConfigToString(OTHERS,itrack, ieta));
      for(int ibin = 1; ibin < m_hHisto[convertConfigToString(UE,itrack, ieta)]->GetNbinsX() + 1; ++ibin)
      {
        double bincontentue = m_hHisto[convertConfigToString(UE,itrack, ieta)]->GetBinContent(ibin);
        double bincontentdm = m_hHisto[convertConfigToString(DM,itrack, ieta)]->GetBinContent(ibin);
        double bincontentclosure = m_hHisto[convertConfigToString(CLOSURE,itrack, ieta)]->GetBinContent(ibin);
        double bincontentshower = m_hHisto[convertConfigToString(SHOWERMODEL,itrack, ieta)]->GetBinContent(ibin);
        m_hHisto[convertConfigToString(OTHERS,itrack, ieta)]->SetBinContent(ibin,
            sqrt(bincontentue * bincontentue +
                 bincontentdm * bincontentdm +
                 bincontentclosure * bincontentclosure +
                 bincontentshower * bincontentshower));
      }
      // total - closure - pu
      getHist(fFile,
              convertConfigToString(FINAL,itrack, ieta),
              convertConfigToString(REMAININGSYS,itrack, ieta));

      for(int ibin = 1; ibin < m_hHisto[convertConfigToString(OTHERS,itrack, ieta)]->GetNbinsX() + 1; ++ibin)
      {
        double bincontentfinal = m_hHisto[convertConfigToString(FINAL,itrack, ieta)]->GetBinContent(ibin);
        double bincontentpu = m_hHisto[convertConfigToString(PU,itrack, ieta)]->GetBinContent(ibin);
        double bincontentclosure = m_hHisto[convertConfigToString(CLOSURE,itrack, ieta)]->GetBinContent(ibin);
        m_hHisto[convertConfigToString(REMAININGSYS,itrack, ieta)]->SetBinContent(ibin,
            sqrt(bincontentfinal * bincontentfinal -
                 bincontentclosure * bincontentclosure -
                 bincontentpu * bincontentpu));

      }
    }
  }

  ATH_MSG_INFO("Input data successfully loaded from " << sInputFilePath);
  fFile->Close();
  delete fFile;
  return  StatusCode::SUCCESS;
}

//______________________________________________________________________________
void TauSmearingRun1Tool::getHist(TFile* fFile, std::string sName, std::string sMapName)
{
  TH1D* hHist = (TH1D*)fFile->Get(sName.c_str());
  if (!hHist)
  {
    ATH_MSG_FATAL("Could not get required histogram " << sName);
    return;
  }
  hHist = (TH1D*)hHist->Clone();
  hHist->SetDirectory(0);
  if (sMapName.length() == 0)
    sMapName = sName;
  m_hHisto[sMapName] = hHist;
  ATH_MSG_VERBOSE("Loaded histogram " << sName);
}


//______________________________________________________________________________
double TauSmearingRun1Tool::getTESShift(double pt, // MeV
                                        int ntracks)
{
  pt = pt / GeV;		// pt in GeV
  if(pt > 70.)
    return 0.;
  double alpha = ntracks == 1 ? 0.008 : 0.011;
  alpha *= interpolateLin(pt);
  return alpha / (1. - alpha);
}

//______________________________________________________________________________
double TauSmearingRun1Tool::interpolateLin(const double& pt)
{
  if(pt < 50.)
    return 1.;
  else if(pt > 70.)
    return 0.;
  return (1  - (pt - 50.) / 20.);
}

//______________________________________________________________________________
double TauSmearingRun1Tool::getTESUncertainty(double pt, // MeV
    double eta,
    int ntracks,
    int iComponent)
{
  /* switched on for HSG4
     assert(component != REMAININGSYS && "User are not supposed to access REMAININGSYS component. Giving up");
  */
  pt = pt / GeV;
  if(pt < 15)
    pt = 16.; // Take the lowest bin

  if(pt > 199)
    pt = 199.; // Take the highest bin

  if(std::fabs(eta)>2.5)
  {
    ATH_MSG_DEBUG("GetTESUncertainty: There is no TES uncertainty defined for |eta|>=2.5 (you gave " << eta << "). Returning 1.");
    return 1.;
  }


  // since it's an array - should be refactored
  int itrack = 0;
  if(ntracks > 1)
    itrack = 1;

  unsigned int iE = 0;
  for(iE = 0; iE < m_iEtaBins; ++iE)
    if(fabs(eta) >= m_vEtaBins.at(iE) && fabs(eta) <= m_vEtaBins.at(iE + 1))
      break;
  if(iComponent == INSITUSTAT)
    return getInSituStat(ntracks);
  if(iComponent == INSITUSYST)
    return getInSituSys(itrack);
  if(iComponent == INSITUSTATINTERPOL)
    return sqrt(interpolateLin(pt)) * getInSituStat(ntracks);
  if(iComponent == SINGLEPARTICLEINTERPOL)
    return getSingleParticleResponseInterpolated(pt, itrack, iE);
  if(iComponent == INSITUSYSTINTERPOL)
    return sqrt(interpolateLin(pt)) * getInSituSys(itrack);
  if(iComponent == SYSREST)
    return getSystRest(pt, ntracks, itrack, iE);
  if(iComponent == MODELING)
    return getModeling(pt, itrack, iE);
  if(iComponent == CLOSURE)
    return getClosure(pt, itrack, iE);
  if(iComponent == SINGLEPARTICLE)
    return getSingleParticleResponse(pt, itrack, iE);
  if(iComponent == INSITU)
    return getInSitu(itrack);
  if(iComponent == INSITUINTERPOL)
    return getInSitu(pt,itrack);

  if(iComponent == TOTAL)
    return getTotal(pt, itrack, iE);

  return m_hHisto[convertConfigToString(iComponent,itrack,iE)]->GetBinContent(m_hHisto[convertConfigToString(iComponent,itrack,iE)]->FindBin(pt));
}


//______________________________________________________________________________
double TauSmearingRun1Tool::getTotal(const double& pt,
                                     const unsigned int& itrack,
                                     const unsigned int& iE)
{
  return sqrt(pow(getModeling(pt, itrack, iE), 2) +
              pow(getSingleParticleResponseInterpolated(pt, itrack, iE), 2) +
              pow(getClosure(pt, itrack, iE), 2) +
              pow(getInSitu(pt, itrack), 2));
}

//______________________________________________________________________________
double TauSmearingRun1Tool::getModeling(const double& pt,
                                        const unsigned int& itrack,
                                        const unsigned int& iE)
{
  double pu = m_hHisto[convertConfigToString(PU,itrack,iE)]->GetBinContent(m_hHisto[convertConfigToString(PU,itrack,iE)]->FindBin(pt));
  double showermodel = m_hHisto[convertConfigToString(SHOWERMODEL,itrack,iE)]->GetBinContent(m_hHisto[convertConfigToString(SHOWERMODEL,itrack,iE)]->FindBin(pt));
  double dm = m_hHisto[convertConfigToString(DM,itrack,iE)]->GetBinContent(m_hHisto[convertConfigToString(DM,itrack,iE)]->FindBin(pt));
  double ue = m_hHisto[convertConfigToString(UE,itrack,iE)]->GetBinContent(m_hHisto[convertConfigToString(UE,itrack,iE)]->FindBin(pt));
  return sqrt(pow(showermodel, 2) +
              pow(pu, 2) +
              pow(ue, 2) +
              pow(dm, 2));
}

//______________________________________________________________________________
double TauSmearingRun1Tool::getClosure(const double& pt,
                                       const unsigned int& itrack,
                                       const unsigned int& iE)
{
  return  m_hHisto[convertConfigToString(CLOSURE,itrack,iE)]->GetBinContent(m_hHisto[convertConfigToString(CLOSURE,itrack,iE)]->FindBin(pt));
}

//______________________________________________________________________________
double TauSmearingRun1Tool::getInSitu(const unsigned int& itrack)
{
  return sqrt(pow(getInSituStat(itrack), 2) +
              pow(getInSituSys(itrack), 2));
}

//______________________________________________________________________________
double TauSmearingRun1Tool::getInSitu(const double& pt, const unsigned int& itrack)
{
  return sqrt(interpolateLin(pt) * (pow(getInSituStat(itrack), 2) +
                                    pow(getInSituSys(itrack), 2)));
}

//______________________________________________________________________________
double TauSmearingRun1Tool::getSingleParticleResponse(const double& pt,
    const unsigned int& itrack,
    const unsigned int& iE)
{
  return m_hHisto[convertConfigToString(SINGLEPARTICLE,itrack,iE)]->GetBinContent(m_hHisto[convertConfigToString(SINGLEPARTICLE,itrack,iE)]->FindBin(pt));
}

//______________________________________________________________________________
double TauSmearingRun1Tool::getSingleParticleResponseInterpolated(const double& pt,
    const unsigned int& itrack,
    const unsigned int& iE)
{
  return sqrt(1 - interpolateLin(pt)) * m_hHisto[convertConfigToString(SINGLEPARTICLE,itrack,iE)]->GetBinContent(m_hHisto[convertConfigToString(SINGLEPARTICLE,itrack,iE)]->FindBin(pt));
}

//______________________________________________________________________________
double TauSmearingRun1Tool::getInSituStat(const int& ntracks)
{
  return ntracks == 1 ? m_dInSituStat1P : m_dInSituStat3P;
}

//______________________________________________________________________________
double TauSmearingRun1Tool::getInSituSys(const int& ntracks)
{
  return ntracks == 1 ? m_dInSituStat1P : m_dInSituStat3P;
}

//______________________________________________________________________________
double TauSmearingRun1Tool::getSystRest(const double& pt, const int& ntracks, const int& itrack, const unsigned int& iE)
{
  if(pt >= 50.) // this should be a member - refactoring
    return m_hHisto[convertConfigToString(REMAININGSYS,itrack,iE)]->GetBinContent(m_hHisto[convertConfigToString(REMAININGSYS,itrack,iE)]->FindBin(pt));
  double insitusys = ntracks == 1 ? m_dInSituSyst1P : m_dInSituSyst3P;
  double remainingsyst = m_hHisto[convertConfigToString(REMAININGSYS,itrack,iE)]->GetBinContent(m_hHisto[convertConfigToString(REMAININGSYS,itrack,iE)]->FindBin(pt));
  return sqrt(pow(insitusys, 2) +
              pow(remainingsyst, 2));
}

//______________________________________________________________________________
std::string TauSmearingRun1Tool::convertConfigToString(int iTESComponent, int iTrack, int iEta)
{
  std::string sProng = (iTrack == 1) ? "_1p" : "_3p";
  return m_mComponents[iTESComponent] + m_vPF_EtaBins[iEta] + sProng;
}
