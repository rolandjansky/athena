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
{}

// ******

StatusCode ArtTest :: initialize ()
{
  ANA_MSG_INFO ("******************************* Initializing *******************************");

  /// Get Histogram Service ///
  ATH_CHECK(service("THistSvc", rootHistSvc));
  
  ANA_MSG_INFO ("*******************************  Histo INIT  *******************************");

  m_evtNmb       = new TH1D(); m_evtNmb      ->SetName("evtNmb")      ; m_evtNmb      ->SetTitle("Event Number");         m_evtNmb       ->SetBins(250, 33894000, 33896000);
  //  CHECK( rootHistSvc()->regHist("/MONITORING/evtNmb", m_evtNmb) );
  CHECK( rootHistSvc->regHist("/MONITORING/evtNmb", m_evtNmb));
  
  m_pT_ElTrk_All = new TH1D(); m_pT_ElTrk_All->SetName("pT_ElTrk_All"); m_pT_ElTrk_All->SetTitle("Electron Pt Track All"); m_pT_ElTrk_All->SetBins(200,        0,      200);
  //  CHECK( rootHistSvc()->regHist("/MONITORING/pT_ElTrk_All", m_pT_ElTrk_All) );
  CHECK( rootHistSvc->regHist("/MONITORING/pT_ElTrk_All", m_pT_ElTrk_All));
  
  m_pT_ElTrk_LLH = new TH1D(); m_pT_ElTrk_LLH->SetName("pT_ElTrk_LLH"); m_pT_ElTrk_LLH->SetTitle("Electron Pt Track LLH"); m_pT_ElTrk_LLH->SetBins(200,        0,      200);
  //  CHECK( rootHistSvc()->regHist("/MONITORING/pT_ElTrk_LLH", m_pT_ElTrk_LLH) );
  CHECK( rootHistSvc->regHist("/MONITORING/pT_ElTrk_LLH", m_pT_ElTrk_LLH));
  
  m_pT_ElTrk_MLH = new TH1D(); m_pT_ElTrk_MLH->SetName("pT_ElTrk_MLH"); m_pT_ElTrk_MLH->SetTitle("Electron Pt Track MLH"); m_pT_ElTrk_MLH->SetBins(200,        0,      200);
  //  CHECK( rootHistSvc()->regHist("/MONITORING/pT_ElTrk_MLH", m_pT_ElTrk_MLH) );
  CHECK( rootHistSvc->regHist("/MONITORING/pT_ElTrk_MLH", m_pT_ElTrk_MLH));
  
  m_pT_ElTrk_TLH = new TH1D(); m_pT_ElTrk_TLH->SetName("pT_ElTrk_TLH"); m_pT_ElTrk_TLH->SetTitle("Electron Pt Track TLH"); m_pT_ElTrk_TLH->SetBins(200,        0,      200);
  //  CHECK( rootHistSvc()->regHist("/MONITORING/pT_ElTrk_TLH", m_pT_ElTrk_TLH) );
  CHECK( rootHistSvc->regHist("/MONITORING/pT_ElTrk_TLH", m_pT_ElTrk_TLH));
  
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
  
  // Retrieve eventInfo from the event store
  const xAOD::EventInfo *eventInfo = nullptr;
  ANA_CHECK (evtStore()->retrieve(eventInfo, "EventInfo"));

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

    // SOME INFO PRINTED OUT
    if(eventInfo->eventNumber()%10 == 0) {
      if( m_LooseLH ->accept(elrec) ) cout << "Loose,  the pt is \t" << (tp->pt())/1000. << "\t and the evt# is " << eventInfo->eventNumber() << endl;
      if( m_MediumLH->accept(elrec) ) cout << "Medium, the pt is \t" << (tp->pt())/1000. << "\t and the evt# is " << eventInfo->eventNumber() << endl;
      if( m_TightLH ->accept(elrec) ) cout << "Tight,  the pt is \t" << (tp->pt())/1000. << "\t and the evt# is " << eventInfo->eventNumber() << endl;
    }
    // END OF SOME INFO PRINTED OUT
    
  } // RecoEl Loop
  
  m_evtNmb->Fill(eventInfo->eventNumber());

  return StatusCode::SUCCESS;
}

// ******

StatusCode ArtTest :: finalize ()
{
  ANA_MSG_INFO ("******************************** Finalizing ********************************");
  return StatusCode::SUCCESS;
}
