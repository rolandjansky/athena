/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "GaudiKernel/ITHistSvc.h"

#include "JetInputProviderFEX.h"
#include "TrigT1CaloEvent/JetROI_ClassDEF.h"
#include "L1TopoEvent/jJetTOB.h"
#include "L1TopoEvent/TopoInputEvent.h"

// this should go away
#include <iomanip>
#include <algorithm>

#include "GaudiKernel/PhysicalConstants.h"

using namespace std;
using namespace LVL1;

JetInputProviderFEX::JetInputProviderFEX(const std::string& type, const std::string& name, 
                                   const IInterface* parent) :
   base_class(type, name, parent),
   m_histSvc("THistSvc", name)
{
   declareInterface<LVL1::IInputTOBConverter>( this );
}

JetInputProviderFEX::~JetInputProviderFEX()
{}

StatusCode
JetInputProviderFEX::initialize() {

   CHECK(m_histSvc.retrieve());

   ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", "JetInputProviderFEX");
   CHECK(incidentSvc.retrieve());
   incidentSvc->addListener(this,"BeginRun", 100);
   incidentSvc.release().ignore();
   
   auto isjEDMvalid = m_jEDMKey.initialize();
   auto isJEDMvalid = m_JEDMKey.initialize();
   auto isjTauEDMvalid = m_jTauEDMKey.initialize();

   //Temporarily check EDM status by hand to avoid the crash!
   if (isjEDMvalid != StatusCode::SUCCESS) {
     ATH_MSG_WARNING("No EDM found for jFEX small R jets..");
   }
   if (isJEDMvalid != StatusCode::SUCCESS) {
     ATH_MSG_WARNING("No EDM found for jFEX large R jets..");
   }
   if (isjTauEDMvalid != StatusCode::SUCCESS) {
     ATH_MSG_WARNING("No EDM found for jFEX taus..");
   }

   return StatusCode::SUCCESS;
}


void
JetInputProviderFEX::handle(const Incident& incident) {
   if (incident.type()!="BeginRun") return;
   ATH_MSG_DEBUG( "In BeginRun incident");

   string histPath = "/EXPERT/" + name() + "/";
   replace( histPath.begin(), histPath.end(), '.', '/'); 

   //jJet
   auto hjJetPt = std::make_unique<TH1I>( "jJetTOBPt", "jJet TOB Pt", 200, 0, 400);
   hjJetPt->SetXTitle("p_{T} [GeV]");

   auto hjJetPhiEta = std::make_unique<TH2I>( "jJetTOBPhiEta", "jJet TOB Location", 240, -240, 240, 64, 0, 128);
   hjJetPhiEta->SetXTitle("#eta#times40");
   hjJetPhiEta->SetYTitle("#phi#times20");


   if (m_histSvc->regShared( histPath + "jJetTOBPt", std::move(hjJetPt), m_hjJetPt ).isSuccess()){
     ATH_MSG_DEBUG("jJetTOB Pt histogram has been registered successfully from JetProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register jJetTOB Pt histogram from JetProviderFEX");
   }

   if (m_histSvc->regShared( histPath + "jJetTOBPhiEta", std::move(hjJetPhiEta), m_hjJetPhiEta ).isSuccess()){
     ATH_MSG_DEBUG("jJetTOB PhiEta histogram has been registered successfully from JetProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register jJetTOB PhiEta histogram from JetProviderFEX");
   }

   //jLargeRJet
   auto hjLargeRJetPt = std::make_unique<TH1I>( "jLargeRJetTOBPt", "jLargeRJet TOB Pt", 200, 0, 2000);
   hjLargeRJetPt->SetXTitle("p_{T} [GeV]");

   auto hjLargeRJetPhiEta = std::make_unique<TH2I>( "jLargeRJetTOBPhiEta", "jLargeRJet TOB Location", 240, -240, 240, 64, 0, 128);
   hjLargeRJetPhiEta->SetXTitle("#eta#times40");
   hjLargeRJetPhiEta->SetYTitle("#phi#times20");


   if (m_histSvc->regShared( histPath + "jLargeRJetTOBPt", std::move(hjLargeRJetPt), m_hjLargeRJetPt ).isSuccess()){
     ATH_MSG_DEBUG("jLargeRJetTOB Pt histogram has been registered successfully from JetProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register jLargeRJetTOB Pt histogram from JetProviderFEX");
   }

   if (m_histSvc->regShared( histPath + "jLargeRJetTOBPhiEta", std::move(hjLargeRJetPhiEta), m_hjLargeRJetPhiEta ).isSuccess()){
     ATH_MSG_DEBUG("jLargeRJetTOB PhiEta histogram has been registered successfully from JetProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register jLargeRJetTOB PhiEta histogram from JetProviderFEX");
   }

   //jTau
   auto hjTauPt = std::make_unique<TH1I>( "jTauTOBPt", "jTau TOB Pt", 200, 0, 200);
   hjTauPt->SetXTitle("p_{T} [GeV]");

   auto hjTauIsolation = std::make_unique<TH1I>( "jTauTOBIsolation", "jTau TOB Isolation", 200, 0, 2000);
   hjTauIsolation->SetXTitle("Isolation [100 MeV]");

   auto hjTauPhiEta = std::make_unique<TH2I>( "jTauTOBPhiEta", "jTau TOB Location", 240, -240, 240, 64, 0, 128);
   hjTauPhiEta->SetXTitle("#eta#times40");
   hjTauPhiEta->SetYTitle("#phi#times20");

   auto hjTauIsolationEta = std::make_unique<TH2I>( "jTauTOBIsolationEta", "jTau TOB Isolation vs eta", 240, -240, 240, 200, 0, 2000);
   hjTauIsolationEta->SetXTitle("#eta#times40");
   hjTauIsolationEta->SetYTitle("Isolation [100 MeV]");

   if (m_histSvc->regShared( histPath + "jTauTOBPt", std::move(hjTauPt), m_hjTauPt ).isSuccess()){
     ATH_MSG_DEBUG("jTauTOB Pt histogram has been registered successfully from JetProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register jTauTOB Pt histogram from JetProviderFEX");
   }

   if (m_histSvc->regShared( histPath + "jTauTOBIsolation", std::move(hjTauIsolation), m_hjTauIsolation ).isSuccess()){
     ATH_MSG_DEBUG("jTauTOB Isolation histogram has been registered successfully from JetProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register jTauTOB Isolation histogram from JetProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "jTauTOBPhiEta", std::move(hjTauPhiEta), m_hjTauPhiEta ).isSuccess()){
     ATH_MSG_DEBUG("jTauTOB PhiEta histogram has been registered successfully from JetProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register jTauTOB PhiEta histogram from JetProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "jTauTOBIsolationEta", std::move(hjTauIsolationEta), m_hjTauIsolationEta ).isSuccess()){
     ATH_MSG_DEBUG("jTauTOB Eta/Isolation histogram has been registered successfully from JetProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register jTauTOB Eta/Isolation histogram from JetProviderFEX");
   }

}


StatusCode
JetInputProviderFEX::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {
   
  SG::ReadHandle<xAOD::jFexSRJetRoIContainer> jContainer(m_jEDMKey);
  //Temporarily check EDM status by hand to avoid the crash!
  if(!jContainer.isValid()){
     ATH_MSG_WARNING("Could not retrieve EDM Container for small R jets " << m_jEDMKey.key() << ". No jFEX input for L1Topo");
     
    return StatusCode::SUCCESS;
  }
   
  SG::ReadHandle<xAOD::jFexLRJetRoIContainer> JContainer(m_JEDMKey);
  //Temporarily check EDM status by hand to avoid the crash!
  if(!JContainer.isValid()){
     ATH_MSG_WARNING("Could not retrieve EDM Container for large R jets" << m_JEDMKey.key() << ". No jFEX input for L1Topo");
     
    return StatusCode::SUCCESS;
  }
   
  SG::ReadHandle<xAOD::jFexTauRoIContainer> jTauContainer(m_jTauEDMKey);
  //Temporarily check EDM status by hand to avoid the crash!
  if(!jTauContainer.isValid()){
     ATH_MSG_WARNING("Could not retrieve EDM Container for jTaus " << m_jTauEDMKey.key() << ". No jFEX input for L1Topo");
     
    return StatusCode::SUCCESS;
  }

  for(const auto it : * jTauContainer) {
    const xAOD::jFexTauRoI* jFexRoI = it;
    ATH_MSG_DEBUG( "EDM jFEX jTau Number: " 
		   << +jFexRoI->jFexNumber() // returns an 8 bit unsigned integer referring to the jFEX number 
		   << " et: " 
		   << jFexRoI->et() // returns the et value of the jet in MeV unit
		   << " tobEt: " 
		   << jFexRoI->tobEt() // returns the et value of the jet in units of 200 MeV
		   << " globalEta: "
		   << jFexRoI->globalEta() // returns global eta in units of 0.1
		   << " isolation: "
		   << jFexRoI->tobIso() // returns isolation value in units of 200 MeV
		   << " globalPhi: "
		   << jFexRoI->globalPhi() // returns global phi in units of 0.1
		   );
       
    unsigned int EtTopo = jFexRoI->tobEt()*2; // Convert Et to 100 MeV unit
    unsigned int phiTopo = jFexRoI->globalPhi()*2; // Convert to 0.05 granularity
    int etaTopo = jFexRoI->globalEta()*4; // Convert to 0.025 granularity
    unsigned int isolation = jFexRoI->tobIso()*2;  // Convert isolation to 100 MeV unit
   
    // Avoid the events with 0 Et (events below threshold)
    if (EtTopo==0) continue;

    TCS::jTauTOB jtau( EtTopo, isolation, etaTopo, phiTopo );
    jtau.setEtDouble( static_cast<double>(EtTopo/10.) );
    jtau.setEtaDouble( static_cast<double>(etaTopo/40.) );
    jtau.setPhiDouble( static_cast<double>(phiTopo/20.) );

    inputEvent.addjTau( jtau );
    inputEvent.addcTau( jtau );

    m_hjTauPt->Fill(jtau.EtDouble());
    m_hjTauIsolation->Fill(jtau.isolation());
    m_hjTauPhiEta->Fill(jtau.eta(),jtau.phi()); 
    m_hjTauIsolationEta->Fill(jtau.eta(),jtau.isolation()); 
  }
  
  for(const auto it : * JContainer) {
    const xAOD::jFexLRJetRoI* jFexRoI = it;
    ATH_MSG_DEBUG( "EDM jFex jJet Number: " 
		   << +jFexRoI->jFexNumber() // returns an 8 bit unsigned integer referring to the jFEX number		   << " et: " 
		   << jFexRoI->et() // returns the et value of the jet in MeV unit
	           << " tobEt: " 
		   << jFexRoI->tobEt() // returns the et value of the jet in units of 200 MeV
		   << " globalEta: "
		   << jFexRoI->globalEta() // returns global eta in units of 0.1
		   << " globalPhi: "
		   << jFexRoI->globalPhi() // returns global phi in units of 0.1
		   );
    
    unsigned int EtTopo = jFexRoI->tobEt()*2; // Convert Et to 100 MeV unit
    unsigned int phiTopo = jFexRoI->globalPhi()*2; // Convert to 0.05 granularity
    int etaTopo = jFexRoI->globalEta()*4; // Convert to 0.025 granularity

    // Avoid the events with 0 Et (events below threshold)
    if (EtTopo==0) continue;

    TCS::jLargeRJetTOB jet( EtTopo, etaTopo, phiTopo );
    jet.setEtDouble( static_cast<double>(EtTopo/10.) );
    jet.setEtaDouble( static_cast<double>(etaTopo/40.) );
    jet.setPhiDouble( static_cast<double>(phiTopo/20.) );

    inputEvent.addjLargeRJet( jet );

    m_hjLargeRJetPt->Fill(jet.EtDouble());
    m_hjLargeRJetPhiEta->Fill(jet.eta(),jet.phi());
  }
  
  for(const auto it : * jContainer){
    const xAOD::jFexSRJetRoI* jFexRoI = it;
    ATH_MSG_DEBUG( "EDM jFex jJet Number: " 
		   << +jFexRoI->jFexNumber() // returns an 8 bit unsigned integer referring to the jFEX number 
		   << " et: " 
		   << jFexRoI->et() // returns the et value of the jet in MeV unit
		   << " tobEt: " 
		   << jFexRoI->tobEt() // returns the et value of the jet in units of 200 MeV
		   << " globalEta: "
		   << jFexRoI->globalEta() // returns global eta in units of 0.1
		   << " globalPhi: "
		   << jFexRoI->globalPhi() // returns global phi in units of 0.1
		   );

    unsigned int EtTopo = jFexRoI->tobEt()*2; // Convert Et to 100 MeV unit
    unsigned int phiTopo = jFexRoI->globalPhi()*2; // Convert to 0.05 granularity
    int etaTopo = jFexRoI->globalEta()*4; // Convert to 0.025 granularity

    // Avoid the events with 0 Et (events below threshold)
    if (EtTopo==0) continue;

    TCS::jJetTOB jet( EtTopo, etaTopo, phiTopo );
    jet.setEtDouble( static_cast<double>(EtTopo/10.) );
    jet.setEtaDouble( static_cast<double>(etaTopo/40.) );
    jet.setPhiDouble( static_cast<double>(phiTopo/20.) );
 
    inputEvent.addjJet( jet );

    m_hjJetPt->Fill(jet.EtDouble());
    m_hjJetPhiEta->Fill(jet.eta(),jet.phi());
    
  }
  return StatusCode::SUCCESS;
}
