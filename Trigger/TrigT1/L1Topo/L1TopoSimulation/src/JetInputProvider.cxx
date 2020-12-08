/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "GaudiKernel/ITHistSvc.h"

#include "JetInputProvider.h"
#include "TrigT1CaloEvent/JetROI_ClassDEF.h"
#include "L1TopoEvent/ClusterTOB.h"
#include "L1TopoEvent/TopoInputEvent.h"


using namespace std;
using namespace LVL1;

JetInputProvider::JetInputProvider(const std::string& type, const std::string& name, 
                                   const IInterface* parent) :
   base_class(type, name, parent),
   m_histSvc("THistSvc", name),
   m_jetLocation(TrigT1CaloDefs::JetTopoTobLocation)
{
   declareInterface<LVL1::IInputTOBConverter>( this );
   declareProperty( "JetROILocation", m_jetLocation, "Storegate key for the Jet ROIs" );
}

JetInputProvider::~JetInputProvider()
{}

StatusCode
JetInputProvider::initialize() {

   CHECK(m_histSvc.retrieve());

   ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", "EnergyInputProvider");
   CHECK(incidentSvc.retrieve());
   incidentSvc->addListener(this,"BeginRun", 100);
   incidentSvc.release().ignore();

   CHECK(m_jetLocation.initialize()); 

   return StatusCode::SUCCESS;
}


void
JetInputProvider::handle(const Incident& incident) {
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
JetInputProvider::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {
   // https://indico.cern.ch/conferenceDisplay.py?confId=284687
   


   SG::ReadHandle< DataVector<JetCMXTopoData> > jettobdata (m_jetLocation);

   if(!jettobdata.isValid()){
      ATH_MSG_WARNING("No DataVector<JetCMXTopoData> with SG key '" << m_jetLocation.key() << "' found in the event. No JET input for the L1Topo simulation.");
      return StatusCode::RECOVERABLE;
   }


   ATH_MSG_DEBUG("Filling the input event. Number of jet topo data objects: " << jettobdata->size());
   for(const JetCMXTopoData * topoData : * jettobdata) {

      // fill the vector of TOBs
      std::vector< JetTopoTOB > tobs;
      topoData->tobs(tobs);

      ATH_MSG_DEBUG("Jet topo data object has # TOBs: " << tobs.size());
      for(const JetTopoTOB & tob: tobs) {

         ATH_MSG_DEBUG( "JET TOB with : et large = " << setw(4) << tob.etLarge() << ", et small " << tob.etSmall()
                        << ", eta = " << setw(2) << tob.eta() << ", phi = " << tob.phi()
                        << ", ieta = " << setw(2) << tob.ieta() << ", iphi = " << tob.iphi()
                        << ", word = " << hex << tob.roiWord() << dec
                        );

         TCS::JetTOB jet( tob.etLarge(), tob.etSmall(), tob.ieta(), tob.iphi(), tob.roiWord() );
         jet.setEtaDouble( tob.eta() );
         jet.setPhiDouble( tob.phi() );
         inputEvent.addJet( jet );
         m_hPt1->Fill(jet.Et1());
         m_hPt2->Fill(jet.Et2());
         m_hEtaPhi->Fill(jet.eta(),jet.phi());
      }
      if(topoData->overflow()){
          inputEvent.setOverflowFromJetInput(true);
          ATH_MSG_DEBUG("setOverflowFromJetInput : true");
      }
   }
   return StatusCode::SUCCESS;
}
