/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include "TH1.h"
#include "TH2.h"

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

   m_hPt1 = new TH1I( "TOBPt1", "Jet TOB Pt 1", 40, 0, 200);
   m_hPt1->SetXTitle("p_{T}");

   m_hPt2 = new TH1I( "TOBPt2", "Jet TOB Pt 2", 40, 0, 200);
   m_hPt2->SetXTitle("p_{T}");

   m_hEtaPhi = new TH2I( "TOBPhiEta", "Jet TOB Location", 25, -50, 50, 64, 0, 64);
   m_hEtaPhi->SetXTitle("#eta");
   m_hEtaPhi->SetYTitle("#phi");

   m_histSvc->regHist( histPath + "TOBPt1", m_hPt1 ).ignore();
   m_histSvc->regHist( histPath + "TOBPt2", m_hPt2 ).ignore();
   m_histSvc->regHist( histPath + "TOBPhiEta", m_hEtaPhi ).ignore();

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
