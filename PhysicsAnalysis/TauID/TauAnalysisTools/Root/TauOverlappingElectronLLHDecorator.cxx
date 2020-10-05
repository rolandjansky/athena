#ifndef XAOD_STANDALONE

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "PATInterfaces/SystematicRegistry.h"

// Local include(s):
#include "TauAnalysisTools/TauOverlappingElectronLLHDecorator.h"
#include "TauAnalysisTools/SharedFilesVersion.h"

// Framework include(s):
#include "PathResolver/PathResolver.h"

// Root include(s)
#include "TFile.h"

using namespace TauAnalysisTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
TauOverlappingElectronLLHDecorator::TauOverlappingElectronLLHDecorator( const std::string& name )
  : asg::AsgMetadataTool( name )
  , m_tEMLHTool(0)
  , m_bElectonsAvailable(true)
  , m_hCutValues(0)
  , m_bEleOLRMatchAvailable(false)
  , m_bNewEvent(false)
  // , m_sElectronPhotonSelectorToolsConfigFile("ElectronPhotonSelectorTools/offline/mc15_20150224/ElectronLikelihoodLooseOfflineConfig2015.conf")
  , m_sElectronPhotonSelectorToolsConfigFile("ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronLikelihoodLooseOfflineConfig2015.conf")
  , m_sEleOlrPassDecorationName("ele_olr_pass_fix")
#ifndef XAODTAU_VERSIONS_TAUJET_V3_H
  , m_sEleOlrLhScoreDecorationName("ele_match_lhscore_fix")
#else
  , m_sEleOlrLhScoreDecorationName("EleMatchLikelihoodScore")
#endif
{
  m_sEleOLRFilePath = "TauAnalysisTools/"+std::string(sSharedFilesVersion)+"/Selection/eveto_cutvals.root";

  declareProperty( "EleOLRFilePath", m_sEleOLRFilePath);
}

//______________________________________________________________________________
TauOverlappingElectronLLHDecorator::~TauOverlappingElectronLLHDecorator()
{
  delete m_hCutValues;
  delete m_tEMLHTool;
}

//______________________________________________________________________________
StatusCode TauOverlappingElectronLLHDecorator::initialize()
{
  ATH_MSG_INFO( "Initializing TauOverlappingElectronLLHDecorator" );

  // This will check that the properties were initialized properly
  ATH_CHECK( m_sElectronContainerName.initialize() );

  // create a EM LH tool
  m_tEMLHTool = new AsgElectronLikelihoodTool (name()+"_ELHTool");
  ATH_CHECK(m_tEMLHTool->setProperty("OutputLevel", msg().level() ));
  if (m_tEMLHTool->setProperty("primaryVertexContainer","PrimaryVertices").isFailure())
  {
    ATH_MSG_FATAL("SelectionCutEleOLR constructor failed setting property primaryVertexContainer");
    return StatusCode::FAILURE;
  }
  if (m_tEMLHTool->setProperty("ConfigFile", m_sElectronPhotonSelectorToolsConfigFile).isFailure())
  {
    ATH_MSG_FATAL("SelectionCutEleOLR constructor failed setting property ConfigFile");
    return StatusCode::FAILURE;
  }
  if (m_tEMLHTool->initialize().isFailure())
  {
    ATH_MSG_FATAL("SelectionCutEleOLR constructor failed initializing AsgElectronLikelihoodTool");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Loading ele OLR cut file "<< m_sEleOLRFilePath);
  TFile tmpFile(PathResolverFindCalibFile(m_sEleOLRFilePath).c_str());
  m_hCutValues = (TH2D*) tmpFile.Get("eveto_cutvals");
  m_hCutValues->SetDirectory(0);
  tmpFile.Close();

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauOverlappingElectronLLHDecorator::setEleOlrPassDecorationName(const std::string& name)
{
  m_sEleOlrPassDecorationName = name;
  return StatusCode::SUCCESS;
}


//______________________________________________________________________________
StatusCode TauOverlappingElectronLLHDecorator::setEleOlrLhScoreDecorationName(const std::string& name)
{
  m_sEleOlrLhScoreDecorationName = name;
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauOverlappingElectronLLHDecorator::decorate(const xAOD::TauJet& xTau) const
{
  if (m_bNewEvent)
  {
    m_bNewEvent = false;

    m_bEleOLRMatchAvailable = (xTau.isAvailable<char>(m_sEleOlrPassDecorationName) || xTau.isAvailable<float>(m_sEleOlrLhScoreDecorationName));
    if (m_bEleOLRMatchAvailable)
      ATH_MSG_DEBUG("ele_olr_pass decoration is available in this event.");
    else
      ATH_MSG_DEBUG("ele_olr_pass decoration is not available in this event. Will decorated it.");
  }
  if (m_bEleOLRMatchAvailable)
    return StatusCode::SUCCESS;

  SG::ReadHandle<xAOD::ElectronContainer> h_ElectronContainer(m_sElectronContainerName);
  if (!h_ElectronContainer.isValid()) {
    ATH_MSG_FATAL("Electron container with name " << m_sElectronContainerName << " was not found in event store, but is needed for electron OLR. Ensure that it is there with the correct name");
    return StatusCode::FAILURE;
  }

  const xAOD::Electron * xEleMatch = 0;
  float fLHScore = -4.; // default if no match was found

  float fEleMatchPt = -1.;
  // find electron with pt>5GeV within 0.4 cone with largest pt
  for( const auto& xElectron : *h_ElectronContainer )
  {
    if(xElectron->pt() < 5000.) continue;
    if(xElectron->p4().DeltaR( xTau.p4() ) > 0.4 ) continue;
    if(xElectron->pt() > fEleMatchPt )
    {
      fEleMatchPt=xElectron->pt();
      xEleMatch=xElectron;
    }
  }

  // compute the LH score if there is a match
  if(xEleMatch!=0)
      fLHScore = m_tEMLHTool->calculate(xEleMatch);

  // static SG::AuxElement::Decorator< ElementLink< xAOD::ElectronContainer > > decElectronLink("electronLink");
  // // create link to the matched electron
  // if (xEleMatch)
  // {
  //   ElementLink < xAOD::ElectronContainer > lElectronMatchLink(xEleMatch, *(m_xElectronContainer));
  //   decElectronLink(xTau) = lElectronMatchLink;
  // }
  // else
  // {
  //   ElementLink < xAOD::ElectronContainer > lElectronMatchLink;
  //   decElectronLink(xTau) = lElectronMatchLink;
  // }

  // decorate tau with score
  SG::AuxElement::Decorator< float > decEleMatchLhscore(m_sEleOlrLhScoreDecorationName);
  decEleMatchLhscore(xTau) = fLHScore;

  bool bPass = false;
  if (xTau.nTracks() == 1)
  {
#ifdef XAODTAU_VERSIONS_TAUJET_V3_H
    const static SG::AuxElement::Accessor< xAOD::TauJet::TauTrackLinks_t > trackAcc( "tauTrackLinks" );
#else
    const static SG::AuxElement::Accessor< xAOD::TauJet::TrackParticleLinks_t > trackAcc( "trackLinks" );
#endif // XAODTAU_VERSIONS_TAUJET_V3_H
    if (trackAcc(xTau)[0].isValid())
      bPass = (fLHScore <= getCutVal(xTau.track(0)->eta(),
                                     xTau.pt()/1000.));
    else
    {
      ATH_MSG_DEBUG("Invalid track link, TauSelectionTool will discard this tau");
      bPass = false;
    }
  }
  else
    bPass = true;
  SG::AuxElement::Decorator< char > decEleOlrPass(m_sEleOlrPassDecorationName);
  decEleOlrPass(xTau) = (char)bPass;

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
StatusCode TauOverlappingElectronLLHDecorator::initializeEvent()
{
  return beginEvent();
}
#pragma GCC diagnostic pop

//=================================PRIVATE-PART=================================
//______________________________________________________________________________
float TauOverlappingElectronLLHDecorator::getCutVal(float fEta, float fPt) const
{
  if(fPt>250) fPt=250;
  if(std::abs(fEta)>2.465) fEta=2.465;

  int iBin= m_hCutValues->FindBin(fPt, std::abs(fEta));
  return m_hCutValues->GetBinContent(iBin);
}

//______________________________________________________________________________
StatusCode TauOverlappingElectronLLHDecorator::beginEvent()
{
  m_bNewEvent = true;
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________

#endif
