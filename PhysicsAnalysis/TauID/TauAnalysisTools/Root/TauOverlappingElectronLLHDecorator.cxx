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
  , m_xElectronContainer(0)
  , m_sElectronContainerName("Electrons")
  , m_bElectonsAvailable(true)
  , m_hCutValues(0)
  , m_bEleOLRMatchAvailable(false)
  , m_bEleOLRMatchAvailableChecked(false)
  , m_bNewEvent(false)
  , m_sElectronPhotonSelectorToolsConfigFile("ElectronPhotonSelectorTools/offline/mc15_20150224_taufix/ElectronLikelihoodLooseOfflineConfig2015.conf")
{
  m_sEleOLRFilePath = "TauAnalysisTools/"+std::string(sSharedFilesVersion)+"/Selection/eveto_cutvals.root";

  declareProperty( "ElectronContainerName", m_sElectronContainerName);
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
  // create a EM LH tool
  m_tEMLHTool = new AsgElectronLikelihoodTool (name()+"_ELHTool");
  m_tEMLHTool->msg().setLevel( msg().level() );
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

  TFile tmpFile(PathResolverFindCalibFile(m_sEleOLRFilePath).c_str());
  m_hCutValues = (TH2D*) tmpFile.Get("eveto_cutvals");
  m_hCutValues->SetDirectory(0);
  tmpFile.Close();

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauOverlappingElectronLLHDecorator::decorate(const xAOD::TauJet& xTau)
{
  if (m_bNewEvent)
  {
    if (retrieveElectrons().isFailure())
      return StatusCode::FAILURE;
    m_bNewEvent = false;
  }

  if (!m_bEleOLRMatchAvailableChecked)
  {
    m_bEleOLRMatchAvailable = (xTau.isAvailable<char>("ele_olr_pass") || xTau.isAvailable<float>("EleMatchLikelihoodScore"));
    m_bEleOLRMatchAvailableChecked = true;
    if (m_bEleOLRMatchAvailable)
    {
      ATH_MSG_DEBUG("ele_olr_pass decoration is available on first tau processed, switched of processing for further taus.");
      ATH_MSG_DEBUG("If a reprocessing of the electron overlap removal is needed, please pass a shallow copy of the original tau.");
    }
  }
  if (m_bEleOLRMatchAvailable)
    return StatusCode::SUCCESS;


  const xAOD::Electron * xEleMatch = 0;
  float fLHScore = -4.; // default if no match was found

  float fEleMatchPt = -1.;
  // find electron with pt>5GeV within 0.4 cone with largest pt
  for( auto xElectron : *(m_xElectronContainer) )
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
    fLHScore = (m_tEMLHTool->calculate(xEleMatch)).getMVAResponse ();

  static SG::AuxElement::Decorator< ElementLink< xAOD::ElectronContainer > > decElectronLink("electronLink");
  // create link to the matched electron
  if (xEleMatch)
  {
    ElementLink < xAOD::ElectronContainer > lElectronMatchLink(xEleMatch, *(m_xElectronContainer));
    decElectronLink(xTau) = lElectronMatchLink;
  }
  else
  {
    ElementLink < xAOD::ElectronContainer > lElectronMatchLink;
    decElectronLink(xTau) = lElectronMatchLink;
  }

  // decorate tau with score
  static SG::AuxElement::Decorator< float > decEleMatchLhscore("ele_match_lhscore");
  decEleMatchLhscore(xTau) = fLHScore;

  bool bPass = false;
  if (xTau.nTracks() == 1)
    bPass = (fLHScore <= getCutVal(xTau.track(0)->eta(),
                                   xTau.pt()/1000.));
  else
    bPass = true;
  static SG::AuxElement::Decorator< char > decEleOlrPass("ele_olr_pass");
  decEleOlrPass(xTau) = (char)bPass;

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauOverlappingElectronLLHDecorator::initializeEvent()
{
  return beginEvent();
}

//=================================PRIVATE-PART=================================
//______________________________________________________________________________
float TauOverlappingElectronLLHDecorator::getCutVal(float fEta, float fPt)
{
  if(fPt>250) fPt=250;
  if(fabs(fEta)>2.465) fEta=2.465;

  int iBin= m_hCutValues->FindBin(fPt, fabs(fEta));
  return m_hCutValues->GetBinContent(iBin);
}

//______________________________________________________________________________
StatusCode TauOverlappingElectronLLHDecorator::beginEvent()
{
  m_bNewEvent = true;
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauOverlappingElectronLLHDecorator::retrieveElectrons()
{
  if (evtStore()->contains<xAOD::ElectronContainer>(m_sElectronContainerName))
    if ( evtStore()->retrieve(m_xElectronContainer,m_sElectronContainerName).isSuccess() )
      return StatusCode::SUCCESS;
  ATH_MSG_FATAL("Electron container with name " << m_sElectronContainerName << " was not found in event store, but is needed for electron OLR. Ensure that it is there with the correct name");
  return StatusCode::FAILURE;
}
