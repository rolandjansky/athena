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
   
   auto isEDMvalid = m_jEDMKey.initialize();

   //Temporarily check EDM status by hand to avoid the crash!
   if (isEDMvalid != StatusCode::SUCCESS) {
     ATH_MSG_WARNING("No EDM found for eFEX..");
   }

   return StatusCode::SUCCESS;
}


void
JetInputProviderFEX::handle(const Incident& incident) {
   if (incident.type()!="BeginRun") return;
   ATH_MSG_DEBUG( "In BeginRun incident");

   string histPath = "/EXPERT/" + name() + "/";
   replace( histPath.begin(), histPath.end(), '.', '/'); 

   auto hPt = std::make_unique<TH1I>( "TOBPt", "Jet TOB Pt 1", 40, 0, 200);
   hPt->SetXTitle("p_{T}");

   auto hEtaPhi = std::make_unique<TH2I>( "TOBPhiEta", "Jet TOB Location", 220, -110, 110, 128, 0, 128);
   hEtaPhi->SetXTitle("#eta");
   hEtaPhi->SetYTitle("#phi");


   if (m_histSvc->regShared( histPath + "TOBPt", std::move(hPt), m_hPt ).isSuccess()){
     ATH_MSG_DEBUG("TOBPt1 histogram has been registered successfully for JetProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register TOBPt histogram for jJetProvider");
   }

   if (m_histSvc->regShared( histPath + "TOBPhiEta", std::move(hEtaPhi), m_hEtaPhi ).isSuccess()){
     ATH_MSG_DEBUG("TOBPhiEta histogram has been registered successfully for jJetProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register TOBPhiEta histogram for jJetProvider");
   }


}


StatusCode
JetInputProviderFEX::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {
   
  SG::ReadHandle<xAOD::jFexSRJetRoIContainer> myRoIContainer(m_jEDMKey);
  //Temporarily check EDM status by hand to avoid the crash!
  if(!myRoIContainer.isValid()){
     ATH_MSG_WARNING("Could not retrieve EDM Container " << m_jEDMKey.key() << ". No jFEX input for L1Topo");
     
    return StatusCode::SUCCESS;
  }

  for(const auto it : * myRoIContainer){
    const xAOD::jFexSRJetRoI* jFexRoI = it;
    ATH_MSG_DEBUG( "EDM jFex Number: " 
		   << +jFexRoI->jFexNumber() // returns an 8 bit unsigned integer referring to the jFEX number 
		   << " et: " 
		   << jFexRoI->et() // returns the et value of the jet in MeV ??
		   << " eta: "
		   << jFexRoI->eta() // returns a floating point global eta (will be at full precision 0.025, but currently only at 0.1)
		   << " phi: "
		   << jFexRoI->phi() // returns a floating point global phi
		   );

    unsigned int Et = jFexRoI->et();
    // The 1/0.025 conversion necessary for take correct value from LUT. (0.025 is the finest granularity in phase1 l1muon input)
    int eta = round(jFexRoI->eta()*40.0);
    // The 1/0.05 conversion necessary for take correct value from LUT. (0.05 is the finest granularity in phase1 l1muon input)
    int phi = round(jFexRoI->eta()*20.0);
    //Change range from [0,4,9] to [-2.5,2.5]
    eta-=2.5*40;

    TCS::jJetTOB jet( Et, eta, phi );
    inputEvent.addjJet( jet );
    m_hPt->Fill(jet.Et());
    m_hEtaPhi->Fill(jet.eta(),jet.phi());
    
  }
  return StatusCode::SUCCESS;
}
