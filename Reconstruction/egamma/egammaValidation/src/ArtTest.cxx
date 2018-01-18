#include <AsgTools/MessageCheck.h>
#include "ArtTest.h"

#include "AthenaMonitoring/MonitoredScope.h"

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

ArtTest :: ArtTest (const std::string& name, ISvcLocator *pSvcLocator) : AthAlgorithm (name, pSvcLocator)
{
}

// ******

StatusCode ArtTest :: initialize ()
{
  ANA_MSG_INFO ("******************************* Initializing *******************************");

  // Only try to retrieve the tool if one has been set
  if(!m_monTool.empty()) CHECK(m_monTool.retrieve());

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
  using namespace Monitored;

  // General Info
  auto runNumb = MonitoredScalar::declare<int>("runNmb", 0);
  auto evtNumb = MonitoredScalar::declare<int>("evtNmb", 0);
  auto timer   = MonitoredTimer::declare("TIME_execute");

  // Electron
  auto pT_trk_All = MonitoredScalar::declare<float>("pT_ElTrk_All", 1.0);
  auto pT_trk_LLH = MonitoredScalar::declare<float>("pT_ElTrk_LLH", 1.0);  
  auto pT_trk_MLH = MonitoredScalar::declare<float>("pT_ElTrk_MLH", 1.0);
  auto pT_trk_TLH = MonitoredScalar::declare<float>("pT_ElTrk_TLH", 1.0);
  
  auto mon    = MonitoredScope::declare(m_monTool, runNumb, evtNumb, timer, pT_trk_All, pT_trk_LLH, pT_trk_MLH, pT_trk_TLH);

  // Retrieve eventInfo from the event store
  const xAOD::EventInfo *eventInfo = nullptr;
  ANA_CHECK (evtStore()->retrieve(eventInfo, "EventInfo"));

  const xAOD::ElectronContainer* RecoEl = 0;
  if( !evtStore()->retrieve(RecoEl, "Electrons").isSuccess() ) {
    Error("execute()", "Failed to retrieve electron container. Exiting.");
    return StatusCode::FAILURE;
  }

  /*
  // For Checks
  if(RecoEl->size() == 0) {
    cout << " **** No Reco Found. The eventNumber is " << eventInfo->eventNumber() << endl;
  }
  else if((eventInfo->eventNumber())%500 == 0) {
    cout << "CIAO the size is" << RecoEl->size() << endl;
  }
  */
  
  for(auto elrec : *RecoEl) {
    
    const xAOD::TrackParticle* tp = elrec->trackParticle();
    // if(RecoEl->size() > 0 && (eventInfo->eventNumber())%10 == 0) cout << "CIAO the size is " << RecoEl->size() << " and the pt is " << (tp->pt())/1000. << endl;

    pT_trk_All = (tp->pt())/1000.;

    if( m_LooseLH->accept(elrec) ) {
      
      pT_trk_LLH = (tp->pt())/1000.;
      // if(eventInfo->eventNumber()%2 == 0) cout << "Loose,  the pt is \t" << (tp->pt())/1000. << "\t and the evt# is " << eventInfo->eventNumber() << endl;
      
    }

    if( m_MediumLH->accept(elrec) ) {
      
      pT_trk_MLH = (tp->pt())/1000.;
      // if(eventInfo->eventNumber()%2 == 0) cout << "Medium, the pt is \t" << (tp->pt())/1000. << "\t and the evt# is " << eventInfo->eventNumber() << endl;
      
    }

    if( m_TightLH->accept(elrec) ) {

      pT_trk_TLH = (tp->pt())/1000.;
      // if(eventInfo->eventNumber()%2 == 0) cout << "Tight,  the pt is \t" << (tp->pt())/1000. << "\t and the evt# is " << eventInfo->eventNumber() << endl;

    }
  
}
  
  runNumb = eventInfo->runNumber();
  evtNumb = eventInfo->eventNumber();
  
  return StatusCode::SUCCESS;
}

// ******

StatusCode ArtTest :: finalize ()
{
  ANA_MSG_INFO ("******************************** Finalizing ********************************");
  
  return StatusCode::SUCCESS;
}
