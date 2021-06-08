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
   auto hjJetPt = std::make_unique<TH1I>( "jJetTOBPt", "jJet TOB Pt", 40, 0, 200);
   hjJetPt->SetXTitle("p_{T}");

   auto hjJetEtaPhi = std::make_unique<TH2I>( "jJetTOBPhiEta", "jJet TOB Location", 220, -110, 110, 128, 0, 128);
   hjJetEtaPhi->SetXTitle("#eta");
   hjJetEtaPhi->SetYTitle("#phi");


   if (m_histSvc->regShared( histPath + "jJetTOBPt", std::move(hjJetPt), m_hjJetPt ).isSuccess()){
     ATH_MSG_DEBUG("jJetTOB Pt histogram has been registered successfully from JetProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jJetTOB Pt histogram from JetProvider");
   }

   if (m_histSvc->regShared( histPath + "jJetTOBPhiEta", std::move(hjJetEtaPhi), m_hjJetEtaPhi ).isSuccess()){
     ATH_MSG_DEBUG("jJetTOB PhiEta histogram has been registered successfully from JetProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jJetTOB PhiEta histogram from JetProvider");
   }

   //jLargeRJet
   auto hjLargeRJetPt = std::make_unique<TH1I>( "jLargeRJetTOBPt", "jLargeRJet TOB Pt", 80, 0, 400);
   hjLargeRJetPt->SetXTitle("p_{T}");

   auto hjLargeRJetEtaPhi = std::make_unique<TH2I>( "jLargeRJetTOBPhiEta", "jLargeRJet TOB Location", 220, -110, 110, 128, 0, 128);
   hjLargeRJetEtaPhi->SetXTitle("#eta");
   hjLargeRJetEtaPhi->SetYTitle("#phi");


   if (m_histSvc->regShared( histPath + "jLargeRJetTOBPt", std::move(hjLargeRJetPt), m_hjLargeRJetPt ).isSuccess()){
     ATH_MSG_DEBUG("jLargeRJetTOB Pt histogram has been registered successfully from JetProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jLargeRJetTOB Pt histogram from JetProvider");
   }

   if (m_histSvc->regShared( histPath + "jLargeRJetTOBPhiEta", std::move(hjLargeRJetEtaPhi), m_hjLargeRJetEtaPhi ).isSuccess()){
     ATH_MSG_DEBUG("jLargeRJetTOB PhiEta histogram has been registered successfully from JetProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jLargeRJetTOB PhiEta histogram from JetProvider");
   }

   //jTau
   auto hjTauPt = std::make_unique<TH1I>( "jTauTOBPt", "jTau TOB Pt", 40, 0, 200);
   hjTauPt->SetXTitle("p_{T}");

   auto hjTauEtaPhi = std::make_unique<TH2I>( "jTauTOBPhiEta", "jTau TOB Location", 220, -110, 110, 128, 0, 128);
   hjTauEtaPhi->SetXTitle("#eta");
   hjTauEtaPhi->SetYTitle("#phi");


   if (m_histSvc->regShared( histPath + "jTauTOBPt", std::move(hjTauPt), m_hjTauPt ).isSuccess()){
     ATH_MSG_DEBUG("jTauTOB Pt histogram has been registered successfully from JetProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jTauTOB Pt histogram from JetProvider");
   }

   if (m_histSvc->regShared( histPath + "jTauTOBPhiEta", std::move(hjTauEtaPhi), m_hjTauEtaPhi ).isSuccess()){
     ATH_MSG_DEBUG("jTauTOB PhiEta histogram has been registered successfully from JetProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jTauTOB PhiEta histogram from JetProvider");
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
		   << jFexRoI->et() // returns the et value of the jet in 200 MeV unit
		   << " eta: "
		   << jFexRoI->eta() // returns a floating point global eta (will be at full precision 0.025, but currently only at 0.1)
		   << " phi: "
		   << jFexRoI->phi() // returns a floating point global phi
		   );
       
    unsigned int Et = jFexRoI->et()*2; //Convert Et to 100 MeV unit
    // Eta and phi is local coordinates, need to switch with global coordinates.
    double phi = jFexRoI->phi()/10.;
    double eta = jFexRoI->eta()/10.;
    //Need to understand inputs
    //Change range from [0,4.9] to [-2.5,2.5]
    eta -= 2.5;
    
    // The 1/0.025 conversion necessary for take correct value from LUT. (0.025 is the finest granularity in phase1 l1muon input)
    int ieta = round(eta*40.0);
    // The 1/0.05 conversion necessary for take correct value from LUT. (0.05 is the finest granularity in phase1 l1muon input)
    unsigned uphi = round(phi*20.0);

    // Avoid the events with 0 Et (Leaving this comment to be aware there are events with 0 Et)
    //if (Et==0) continue;

    TCS::jTauTOB tau( Et, ieta, uphi );
    tau.setEtaDouble(eta);
    tau.setPhiDouble(phi);
    inputEvent.addjTau( tau );
    m_hjTauPt->Fill(tau.EtDouble());
    m_hjTauEtaPhi->Fill(tau.eta(),tau.phi()); 
  }
  
  for(const auto it : * JContainer) {
    const xAOD::jFexLRJetRoI* jFexRoI = it;
    ATH_MSG_DEBUG( "EDM jFex JJet Number: " 
		   << +jFexRoI->jFexNumber() // returns an 8 bit unsigned integer referring to the jFEX number 
		   << " et: " 
		   << jFexRoI->et() // returns the et value of the jet in 200 MeV unit
		   << " eta: "
		   << jFexRoI->eta() // returns a floating point global eta (will be at full precision 0.025, but currently only at 0.1)
		   << " phi: "
		   << jFexRoI->phi() // returns a floating point global phi
		   );
    
    unsigned int Et = jFexRoI->et()*2; //Convert Et to 100 MeV unit
    // Eta and phi is local coordinates, need to switch with global coordinates.
    double phi = jFexRoI->phi()/10.;
    double eta = jFexRoI->eta()/10.;
    //Need to understand inputs
    //Change range from [0,4.9] to [-2.5,2.5]
    eta -= 2.5;
    
    // The 1/0.025 conversion necessary for take correct value from LUT. (0.025 is the finest granularity in phase1 l1muon input)
    int ieta = round(eta*40.0);
    // The 1/0.05 conversion necessary for take correct value from LUT. (0.05 is the finest granularity in phase1 l1muon input)
    unsigned uphi = round(phi*20.0);

    // Avoid the events with 0 Et (Leaving this comment to be aware there are events with 0 Et)
    //if (Et==0) continue;

    TCS::jLargeRJetTOB jet( Et, ieta, uphi );
    jet.setEtaDouble(eta);
    jet.setPhiDouble(phi);
    inputEvent.addjLargeRJet( jet );
    m_hjLargeRJetPt->Fill(jet.EtDouble());
    m_hjLargeRJetEtaPhi->Fill(jet.eta(),jet.phi());
  }
  
  for(const auto it : * jContainer){
    const xAOD::jFexSRJetRoI* jFexRoI = it;
    ATH_MSG_DEBUG( "EDM jFex jJet Number: " 
		   << +jFexRoI->jFexNumber() // returns an 8 bit unsigned integer referring to the jFEX number 
		   << " et: " 
		   << jFexRoI->et() // returns the et value of the jet in 200 MeV unit
		   << " eta: "
		   << jFexRoI->eta() // returns a floating point global eta (will be at full precision 0.025, but currently only at 0.1)
		   << " phi: "
		   << jFexRoI->phi() // returns a floating point global phi
		   );

    unsigned int Et = jFexRoI->et()*2; //Convert Et to 100 MeV unit
    // Eta and phi is local coordinates, need to switch with global coordinates.
    double phi = jFexRoI->phi()/10.;
    double eta = jFexRoI->eta()/10.;
    //Need to understand inputs
    //Change range from [0,4.9] to [-2.5,2.5]
    eta -= 2.5;
    
    // The 1/0.025 conversion necessary for take correct value from LUT. (0.025 is the finest granularity in phase1 l1muon input)
    int ieta = round(eta*40.0);
    // The 1/0.05 conversion necessary for take correct value from LUT. (0.05 is the finest granularity in phase1 l1muon input)
    unsigned uphi = round(phi*20.0);

    // Avoid the events with 0 Et (Leaving this comment to be aware there are events with 0 Et)
    //if (Et==0) continue;

    TCS::jJetTOB jet( Et, ieta, uphi );
    jet.setEtaDouble(eta);
    jet.setPhiDouble(phi);
    inputEvent.addjJet( jet );
    m_hjJetPt->Fill(jet.EtDouble());
    m_hjJetEtaPhi->Fill(jet.eta(),jet.phi());
    
  }
  return StatusCode::SUCCESS;
}
