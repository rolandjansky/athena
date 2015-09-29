/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ROOT include(s):
#include <TRandom.h>

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "PATInterfaces/SystematicRegistry.h"

// Local include(s):
#include "TauAnalysisTools/TauOverlappingElectronLLHDecorator.h"
#include "TauAnalysisTools/SharedFilesVersion.h"

using namespace TauAnalysisTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
TauOverlappingElectronLLHDecorator::TauOverlappingElectronLLHDecorator( const std::string& name )
  : asg::AsgTool( name )
  , m_tEMLHTool(0)
  , m_xElectronContainer(0)
  , m_sElectronContainerName("Electrons")
  , m_bElectonsAvailable(true)
{
  declareProperty( "ElectronContainerName", m_sElectronContainerName);
}

//______________________________________________________________________________
TauOverlappingElectronLLHDecorator::~TauOverlappingElectronLLHDecorator()
{
  delete m_tEMLHTool;
}

//______________________________________________________________________________
StatusCode TauOverlappingElectronLLHDecorator::initialize()
{
  ATH_MSG_INFO( "Initialising TauOverlappingElectronLLHDecorator" );
  // create a EM LH tool
  std::string confDir = "ElectronPhotonSelectorTools/offline/mc15_20150224/";
  m_tEMLHTool = new AsgElectronLikelihoodTool (name()+"_ELHTool");
  m_tEMLHTool->msg().setLevel( msg().level() );
  if (m_tEMLHTool->setProperty("primaryVertexContainer","PrimaryVertices").isFailure())
  {
    ATH_MSG_FATAL("SelectionCutEleOLR constructor failed setting property primaryVertexContainer");
    return StatusCode::FAILURE;
  }
  if (m_tEMLHTool->setProperty("ConfigFile",confDir+"ElectronLikelihoodLooseOfflineConfig2015.conf").isFailure())
  {
    ATH_MSG_FATAL("SelectionCutEleOLR constructor failed setting property ConfigFile");
    return StatusCode::FAILURE;
  }
  if (m_tEMLHTool->initialize().isFailure())
  {
    ATH_MSG_FATAL("SelectionCutEleOLR constructor failed initializing AsgElectronLikelihoodTool");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauOverlappingElectronLLHDecorator::decorate(const xAOD::TauJet& xTau)
{
  const xAOD::Electron * xEleMatch = 0;
  float fLHScore = -4.; // default if no match was found

  if (m_bElectonsAvailable)
    if (!m_xElectronContainer)
      m_bElectonsAvailable = false;
  if (!m_bElectonsAvailable)
    if ( evtStore()->retrieve( m_xElectronContainer , m_sElectronContainerName ).isFailure() )
    {
      ATH_MSG_FATAL("Electron container with name " << m_sElectronContainerName << "was not found in event store, but is needed for electron OLR. Ensure that it is there with the correct name");
      return StatusCode::FAILURE;
    }

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

  // create link to the original TruthParticle
  if (xEleMatch)
  {
    ElementLink < xAOD::ElectronContainer > lTruthParticleLink(xEleMatch, *(m_xElectronContainer));
    xTau.auxdecor< ElementLink< xAOD::ElectronContainer > >("electronLink" ) = lTruthParticleLink;
  }
  else
  {
    ElementLink < xAOD::ElectronContainer > lTruthParticleLink;
    xTau.auxdecor< ElementLink< xAOD::ElectronContainer > >("electronLink" ) = lTruthParticleLink;
  }

  // decorate tau with score
  xTau.auxdecor< float >( "ele_match_lhscore" ) = fLHScore;
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauOverlappingElectronLLHDecorator::initializeEvent()
{
  if ( evtStore()->retrieve(m_xElectronContainer,m_sElectronContainerName).isSuccess() )
  {
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}


//=================================PRIVATE-PART=================================

