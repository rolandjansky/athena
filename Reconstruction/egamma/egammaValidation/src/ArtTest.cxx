#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AsgTools/MessageCheck.h"
#include "ArtTest.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"

#include <vector>
#include <cmath>

using namespace std;

ArtTest :: ArtTest (const std::string& name, ISvcLocator *pSvcLocator) : 
  AthAlgorithm (name, pSvcLocator)
{
declareProperty( "particleName", m_particleName = "Unknown", "Descriptive name for the processed type of particle" );
}

// ******

StatusCode ArtTest :: initialize ()
{
  ANA_MSG_INFO ("******************************* Initializing *******************************");

  /// Get Histogram Service ///
  ATH_CHECK(service("THistSvc", rootHistSvc));
  
  ANA_MSG_INFO ("******************* Running over " << m_particleName << "*******************");

  ANA_MSG_INFO ("*******************************  Histo INIT  *******************************");

  m_evtNmb = new TH1D(); m_evtNmb->SetName("evtNmb"); m_evtNmb->SetTitle("Event Number"); 
  CHECK( rootHistSvc->regHist("/MONITORING/evtNmb", m_evtNmb));
    
  if("electron" == m_particleName) {

    m_evtNmb->SetBins(2000, 85000, 87000);

    m_pT_ElTrk_All  = new TH1D(); m_pT_ElTrk_All ->SetName("pT_ElTrk_All") ; m_pT_ElTrk_All ->SetTitle("Electron Pt Track All"); m_pT_ElTrk_All ->SetBins(200,  0, 200);
    CHECK( rootHistSvc->regHist("/MONITORING/pT_ElTrk_All", m_pT_ElTrk_All));
    
    m_pT_ElTrk_LLH  = new TH1D(); m_pT_ElTrk_LLH ->SetName("pT_ElTrk_LLH") ; m_pT_ElTrk_LLH ->SetTitle("Electron Pt Track LLH"); m_pT_ElTrk_LLH ->SetBins(200,  0, 200);
    CHECK( rootHistSvc->regHist("/MONITORING/pT_ElTrk_LLH", m_pT_ElTrk_LLH));
    
    m_pT_ElTrk_MLH  = new TH1D(); m_pT_ElTrk_MLH ->SetName("pT_ElTrk_MLH") ; m_pT_ElTrk_MLH ->SetTitle("Electron Pt Track MLH"); m_pT_ElTrk_MLH ->SetBins(200,  0, 200);
    CHECK( rootHistSvc->regHist("/MONITORING/pT_ElTrk_MLH", m_pT_ElTrk_MLH));
    
    m_pT_ElTrk_TLH  = new TH1D(); m_pT_ElTrk_TLH ->SetName("pT_ElTrk_TLH") ; m_pT_ElTrk_TLH ->SetTitle("Electron Pt Track TLH"); m_pT_ElTrk_TLH ->SetBins(200,  0, 200);
    CHECK( rootHistSvc->regHist("/MONITORING/pT_ElTrk_TLH", m_pT_ElTrk_TLH));
    
    m_eta_ElTrk_All = new TH1D(); m_eta_ElTrk_All->SetName("eta_ElTrk_All"); m_eta_ElTrk_All->SetTitle("Electron Eta All")     ; m_eta_ElTrk_All->SetBins(200, -3,   3);
    CHECK( rootHistSvc->regHist("/MONITORING/eta_ElTrk_All", m_eta_ElTrk_All));
    
    m_phi_ElTrk_All = new TH1D(); m_phi_ElTrk_All->SetName("phi_ElTrk_All"); m_phi_ElTrk_All->SetTitle("Electron Phi All")     ; 
    m_phi_ElTrk_All->SetBins(100, -TMath::Pi(), TMath::Pi());    
    CHECK( rootHistSvc->regHist("/MONITORING/phi_ElTrk_All", m_phi_ElTrk_All));
  
  } // electron Hists

  if("gamma" == m_particleName) {

    m_evtNmb->SetBins(250, 33894000, 33896000);
    
    m_pT_Phot_All  = new TH1D(); m_pT_Phot_All ->SetName("pT_Phot_All") ; m_pT_Phot_All ->SetTitle("Photon Pt All") ; m_pT_Phot_All ->SetBins(200,  0,  200);
    CHECK( rootHistSvc->regHist("/MONITORING/pT_Phot_All", m_pT_Phot_All));
    
    m_eta_Phot_All = new TH1D(); m_eta_Phot_All->SetName("eta_Phot_All"); m_eta_Phot_All->SetTitle("Photon Eta All"); m_eta_Phot_All->SetBins(200, -3,    3);
    CHECK( rootHistSvc->regHist("/MONITORING/eta_Phot_All", m_eta_Phot_All));
    
    m_phi_Phot_All = new TH1D(); m_phi_Phot_All->SetName("phi_Phot_All"); m_phi_Phot_All->SetTitle("Photon Phi All"); m_phi_Phot_All->SetBins(100, -TMath::Pi(), TMath::Pi());
    CHECK( rootHistSvc->regHist("/MONITORING/phi_Phot_All", m_phi_Phot_All));
  
  } // gamma Hists

  //*****************LLH Requirement********************
  m_LooseLH = new AsgElectronLikelihoodTool("LooseLH");
  m_LooseLH->setProperty("WorkingPoint", "LooseLHElectron");
  m_LooseLH->msg().setLevel(MSG::INFO);
  
  if( m_LooseLH->initialize().isFailure()) {
    ATH_MSG_INFO("Initialization Selectors FAILED");
    return StatusCode::FAILURE;
  }
  else  ATH_MSG_INFO("Retrieved Selector tool ");
  //****************************************************
  
  ANA_MSG_INFO ("*******************************  OK LLH Req  *******************************");
  
  //*****************MLH Requirement********************
  m_MediumLH = new AsgElectronLikelihoodTool("MediumLH"); 
  m_MediumLH->setProperty("WorkingPoint", "MediumLHElectron");
  m_MediumLH->msg().setLevel(MSG::INFO);
  
  if( m_MediumLH->initialize().isFailure()) {
    ATH_MSG_INFO("Initialization Selectors FAILED");    
    return StatusCode::FAILURE;
  } 
  else  ATH_MSG_INFO("Retrieved Selector tool ");
  //****************************************************
  
  ANA_MSG_INFO ("*******************************  OK MLH Req  *******************************");
  
  //*****************TLH Requirement********************
  m_TightLH = new AsgElectronLikelihoodTool("TightLH");
  m_TightLH->setProperty("WorkingPoint", "TightLHElectron");
  m_TightLH->msg().setLevel(MSG::INFO);
  
  if( m_TightLH->initialize().isFailure()) {
    ATH_MSG_INFO("Initialization Selectors FAILED");
    return StatusCode::FAILURE;
  }
  else  ATH_MSG_INFO("Retrieved Selector tool ");
  //****************************************************
  
  ANA_MSG_INFO ("*******************************  OK TLH Req  *******************************");
  
  ANA_MSG_INFO ("*******************************   END INIT   *******************************");  
  return StatusCode::SUCCESS;
}

// ******

StatusCode ArtTest :: beginInputFile ()
{
  return StatusCode::SUCCESS;
}

// ******

StatusCode ArtTest :: firstExecute ()
{
  return StatusCode::SUCCESS;
}

// ******

StatusCode ArtTest :: execute ()
{
  
  // Retrieve things from the event store

  const xAOD::EventInfo *eventInfo = nullptr;
  ANA_CHECK (evtStore()->retrieve(eventInfo, "EventInfo"));

  if("electron" == m_particleName) {
    
    const xAOD::ElectronContainer* RecoEl = 0;
    if( !evtStore()->retrieve(RecoEl, "Electrons").isSuccess() ) {
      Error("execute()", "Failed to retrieve electron container. Exiting.");
      return StatusCode::FAILURE;
    }
    
    for(auto elrec : *RecoEl) {
      
      const xAOD::TrackParticle* tp = elrec->trackParticle();

      if((tp->pt())/1000. > 0) {
	
	m_pT_ElTrk_All->Fill((tp->pt())/1000.); 
	
	if(m_LooseLH ->accept(elrec)) m_pT_ElTrk_LLH->Fill((tp->pt())/1000.);
	if(m_MediumLH->accept(elrec)) m_pT_ElTrk_MLH->Fill((tp->pt())/1000.);
	if(m_TightLH ->accept(elrec)) m_pT_ElTrk_TLH->Fill((tp->pt())/1000.);
	
      }

      m_eta_ElTrk_All->Fill(tp->eta());
      m_phi_ElTrk_All->Fill(tp->phi());
      
    } // RecoEl Loop

  } // if electron


  if("gamma" == m_particleName) {
    
    const xAOD::PhotonContainer* RecoPh = 0;
    if( !evtStore()->retrieve(RecoPh, "Photons").isSuccess() ) {
      Error("execute()", "Failed to retrieve photon container. Exiting.");
      return StatusCode::FAILURE;
    }  
    
    for(auto phrec : *RecoPh) {
      
      if((phrec->pt())/1000. > 0) m_pT_Phot_All->Fill((phrec->pt())/1000.);
      m_eta_Phot_All->Fill(phrec->eta());
      m_phi_Phot_All->Fill(phrec->phi());
      
    } // RecoPh Loop

  } // if gamma

  m_evtNmb->Fill(eventInfo->eventNumber());

  return StatusCode::SUCCESS;
}

// ******

StatusCode ArtTest :: finalize ()
{
  ANA_MSG_INFO ("******************************** Finalizing ********************************");
  return StatusCode::SUCCESS;
}
