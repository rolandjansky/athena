/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Tau Electron Overlap Removal Decorator Tool
 *
 * Author: Dirk Duschinger
 * Modified: Lorenz Hauswald
 */

#include "TauDiscriminant/TauEleOLRDecorator.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"
#include "CxxUtils/make_unique.h"
#include "TFile.h"

TauEleOLRDecorator::TauEleOLRDecorator(const std::string& name):
  TauDiscriToolBase(name),
  m_tEMLHTool(nullptr),
  m_xElectronContainer(nullptr),
  m_sElectronContainerName("Electrons"),
  m_bElectonsAvailable(true),
  m_sEleOLRFilePath("eveto_cutvals.root"),
  m_hCutValues(nullptr),
  m_bEleOLRMatchAvailable(false),
  m_bEleOLRMatchAvailableChecked(false)
{
  declareProperty("ElectronContainerName", m_sElectronContainerName);
  declareProperty("EleOLRFile", m_sEleOLRFilePath);
}

TauEleOLRDecorator::~TauEleOLRDecorator(){

}

StatusCode TauEleOLRDecorator::eventInitialize()
{
  return retrieveElectrons();
}

StatusCode TauEleOLRDecorator::initialize()
{
  ATH_MSG_INFO( "Initializing TauEleOLRDecorator" );

  std::string confDir = "ElectronPhotonSelectorTools/offline/mc15_20150712/";
  m_tEMLHTool = CxxUtils::make_unique<AsgElectronLikelihoodTool>(name()+"_ELHTool");
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

  m_sEleOLRFilePath = find_calibFile(m_sEleOLRFilePath);
  TFile tmpFile(m_sEleOLRFilePath.c_str());
  m_hCutValues = std::unique_ptr<TH2D>(static_cast<TH2D*>(tmpFile.Get("eveto_cutvals")));
  m_hCutValues->SetDirectory(0);
  tmpFile.Close();
  
  return StatusCode::SUCCESS;
}

StatusCode TauEleOLRDecorator::execute(xAOD::TauJet& tau)
{
  if (!m_bEleOLRMatchAvailableChecked)
    {
      m_bEleOLRMatchAvailable = tau.isAvailable<char>("ele_olr_pass");
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

  if (m_bElectonsAvailable)
    if (!m_xElectronContainer)
      m_bElectonsAvailable = false;
  if (!m_bElectonsAvailable)
    if (retrieveElectrons().isFailure())
      return StatusCode::FAILURE;
  float fEleMatchPt = -1.;
  // find electron with pt>5GeV within 0.4 cone with largest pt
  for( auto xElectron : *(m_xElectronContainer) )
    {
      if(xElectron->pt() < 5000.) continue;
      if(xElectron->p4().DeltaR( tau.p4() ) > 0.4 ) continue;
      if(xElectron->pt() > fEleMatchPt )
	{
	  fEleMatchPt=xElectron->pt();
	  xEleMatch=xElectron;
	}
    }

  // compute the LH score if there is a match
  if(xEleMatch!=0)
    fLHScore = (m_tEMLHTool->calculate(xEleMatch)).getMVAResponse ();

  // create link to the matched electron
  if (xEleMatch)
    {
      ElementLink < xAOD::ElectronContainer > lElectronMatchLink(xEleMatch, *(m_xElectronContainer));
      tau.auxdecor< ElementLink< xAOD::ElectronContainer > >("electronLink" ) = lElectronMatchLink;
    }
  else
    {
      ElementLink < xAOD::ElectronContainer > lElectronMatchLink;
      tau.auxdecor< ElementLink< xAOD::ElectronContainer > >("electronLink" ) = lElectronMatchLink;
    }

  // decorate tau with score
  tau.auxdecor< float >( "ele_match_lhscore" ) = fLHScore;

  bool bPass = false;
  if (tau.nTracks() == 1)
    bPass = (fLHScore <= getCutVal(tau.track(0)->eta(),
				   tau.pt()/1000.));
  else
    bPass = true;
  tau.auxdecor< char >( "ele_olr_pass" ) = static_cast<char>(bPass);
  
  return StatusCode::SUCCESS;
}

StatusCode TauEleOLRDecorator::finalize()
{
  
  return StatusCode::SUCCESS;
}

StatusCode TauEleOLRDecorator::retrieveElectrons()
{
  if (evtStore()->contains<xAOD::ElectronContainer>(m_sElectronContainerName))
    if ( evtStore()->retrieve(m_xElectronContainer,m_sElectronContainerName).isSuccess() )
      return StatusCode::SUCCESS;
  ATH_MSG_FATAL("Electron container with name " << m_sElectronContainerName << " was not found in event store, but is needed for electron OLR. Ensure that it is there with the\
 correct name");
  return StatusCode::FAILURE;
}

float TauEleOLRDecorator::getCutVal(float fEta, float fPt)
{
  if(fPt>250) fPt=250;
  if(fabs(fEta)>2.465) fEta=2.465;

  int iBin= m_hCutValues->FindBin(fPt, fabs(fEta));
  return m_hCutValues->GetBinContent(iBin);
}

