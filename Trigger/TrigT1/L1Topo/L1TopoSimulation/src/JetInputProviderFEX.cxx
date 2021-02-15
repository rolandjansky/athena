/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "GaudiKernel/ITHistSvc.h"

#include "JetInputProviderFEX.h"
#include "TrigT1CaloEvent/JetROI_ClassDEF.h"
#include "L1TopoEvent/ClusterTOB.h"
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
   m_histSvc("THistSvc", name),
   m_gFEXJetLoc ("jRoundJets")
{
   declareInterface<LVL1::IInputTOBConverter>( this );
   declareProperty( "gFEXJetInput", m_gFEXJetLoc, "StoreGate location of gFEX Jet inputs" );
}

JetInputProviderFEX::~JetInputProviderFEX()
{}

StatusCode
JetInputProviderFEX::initialize() {

   CHECK(m_histSvc.retrieve());

   ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", "MissingEnergyInputProviderFEX");
   CHECK(incidentSvc.retrieve());
   incidentSvc->addListener(this,"BeginRun", 100);
   incidentSvc.release().ignore();

   return StatusCode::SUCCESS;
}


void
JetInputProviderFEX::handle(const Incident& incident) {
   if (incident.type()!="BeginRun") return;
   ATH_MSG_DEBUG( "In BeginRun incident");

   string histPath = "/EXPERT/" + name() + "/";
   replace( histPath.begin(), histPath.end(), '.', '/'); 

   auto hPt1 = std::make_unique<TH1I>( "TOBPt1", "Jet TOB Pt 1", 40, 0, 200);
   hPt1->SetXTitle("p_{T}");

   auto hPt2 = std::make_unique<TH1I>( "TOBPt2", "Jet TOB Pt 2", 40, 0, 200);
   hPt2->SetXTitle("p_{T}");

   auto hEtaPhi = std::make_unique<TH2I>( "TOBPhiEta", "Jet TOB Location", 25, -50, 50, 64, 0, 64);
   hEtaPhi->SetXTitle("#eta");
   hEtaPhi->SetYTitle("#phi");


   if (m_histSvc->regShared( histPath + "TOBPt1", std::move(hPt1), m_hPt1 ).isSuccess()){
     ATH_MSG_DEBUG("TOBPt1 histogram has been registered successfully for JetProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register TOBPt1 histogram for JetProvider");
   }

   if (m_histSvc->regShared( histPath + "TOBPt2", std::move(hPt2), m_hPt2 ).isSuccess()){
     ATH_MSG_DEBUG("TOBPt2 histogram has been registered successfully for JetProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register TOBPt2 histogram for JetProvider");
   }
   if (m_histSvc->regShared( histPath + "TOBPhiEta", std::move(hEtaPhi), m_hEtaPhi ).isSuccess()){
     ATH_MSG_DEBUG("TOBPhiEta histogram has been registered successfully for JetProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register TOBPhiEta histogram for JetProvider");
   }


}


StatusCode
JetInputProviderFEX::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {
   
  const xAOD::JetRoIContainer* gFEXJet = nullptr;   // jets from gFEX (will make it an array later)
  if( evtStore()->contains< xAOD::JetRoIContainer >(m_gFEXJetLoc) ) {
    CHECK ( evtStore()->retrieve( gFEXJet, m_gFEXJetLoc ) );
    ATH_MSG_DEBUG( "Retrieved gFEX Jet container '" << m_gFEXJetLoc << "' with size " << gFEXJet->size());
  }
  else {
    ATH_MSG_WARNING("No xAOD::JetRoIContainer with SG key '" << m_gFEXJetLoc.toString() << "' found in the event. No JET input for the L1Topo simulation.");
    return StatusCode::RECOVERABLE;
  }


  for(const xAOD::JetRoI * topoData : * gFEXJet) {

    ATH_MSG_DEBUG( "JET TOB with : et large = " << setw(4) << topoData->et8x8() << ", et small " << topoData->et4x4()
		   << ", eta = " << setw(2) <<  topoData->eta() << ", phi = " << topoData->phi()
		   << ", word = " << hex << topoData->roiWord() << dec
		   );     
    
    TCS::JetTOB jet( topoData->et8x8()/Gaudi::Units::GeV, topoData->et4x4()/Gaudi::Units::GeV, topoData->eta(), topoData->phi(), topoData->roiWord() );
    jet.setEtaDouble( topoData->eta() );
    jet.setPhiDouble( topoData->phi() );
    inputEvent.addJet( jet );
    m_hPt1->Fill(jet.Et1());
    m_hPt2->Fill(jet.Et2());
    m_hEtaPhi->Fill(jet.eta(),jet.phi());
     
    /*
      Pseudo code for overflow case which we won't consider for the time being 
      if(has_overflow){
      inputEvent.setOverflowFromJetInput(true);
      ATH_MSG_DEBUG("setOverflowFromJetInput : true");
      }
    */
  }
  return StatusCode::SUCCESS;
}
