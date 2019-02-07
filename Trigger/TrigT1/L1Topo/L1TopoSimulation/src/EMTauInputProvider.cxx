/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./EMTauInputProvider.h"

#include <math.h>
#include "TH1.h"
#include "TH2.h"

#include "GaudiKernel/ITHistSvc.h"

#include "TrigT1CaloEvent/EmTauROI_ClassDEF.h"
#include "TrigT1CaloEvent/CPCMXTopoData.h"

#include "TrigT1Interfaces/CPRoIDecoder.h"

#include "L1TopoEvent/ClusterTOB.h"
#include "L1TopoEvent/TopoInputEvent.h"


using namespace std;
using namespace LVL1;

EMTauInputProvider::EMTauInputProvider(const std::string& type, const std::string& name, 
                                       const IInterface* parent) :
   base_class(type, name, parent),
   m_histSvc("THistSvc", name),
   m_emTauLocation( TrigT1CaloDefs::EmTauTopoTobLocation )
{
   declareInterface<LVL1::IInputTOBConverter>( this );
   declareProperty( "EmTauROILocation", m_emTauLocation, "Storegate key for the EMTAU info from CMX" );
}

EMTauInputProvider::~EMTauInputProvider()
{}

StatusCode
EMTauInputProvider::initialize() {

   CHECK(m_histSvc.retrieve());

   ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", "EnergyInputProvider");
   CHECK(incidentSvc.retrieve());
   incidentSvc->addListener(this,"BeginRun", 100);
   incidentSvc.release().ignore();

   return StatusCode::SUCCESS;
}


void
EMTauInputProvider::handle(const Incident& incident) {
   if (incident.type()!="BeginRun") return;
   ATH_MSG_DEBUG( "In BeginRun incident");

   string histPath = "/EXPERT/" + name() + "/";
   replace( histPath.begin(), histPath.end(), '.', '/'); 

   m_hEMEt = new TH1I( "EMTOBEt", "EM TOB Et", 40, 0, 200);
   m_hEMEt->SetXTitle("E_{T}");
   m_hEMEtaPhi = new TH2I( "EMTOBPhiEta", "EM TOB Location", 25, -50, 50, 64, 0, 64);
   m_hEMEtaPhi->SetXTitle("#eta");
   m_hEMEtaPhi->SetYTitle("#phi");

   m_hTauEt = new TH1I( "TauTOBEt", "Tau TOB Et", 40, 0, 200);
   m_hTauEt->SetXTitle("E_{T}");
   m_hTauEtaPhi = new TH2I( "TauTOBPhiEta", "Tau TOB Location", 25, -50, 50, 64, 0, 64);
   m_hTauEtaPhi->SetXTitle("#eta");
   m_hTauEtaPhi->SetYTitle("#phi");

   m_histSvc->regHist( histPath + "EMTOBEt", m_hEMEt ).ignore();
   m_histSvc->regHist( histPath + "EMTOBPhiEta", m_hEMEtaPhi ).ignore();
   m_histSvc->regHist( histPath + "TauTOBEt", m_hTauEt ).ignore();
   m_histSvc->regHist( histPath + "TauTOBPhiEta", m_hTauEtaPhi ).ignore();
}



StatusCode
EMTauInputProvider::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {

   /** this will be the new format
       https://indico.cern.ch/conferenceDisplay.py?confId=284687
       Electron ROI:
       | 0 0 1 0 | 2b Crate | 4b CPM Num | 3b CPChip | 3b Local coords | 0 0 0 | 5b electron isolation/veto | 8b electron energy |
       Tau ROI:
       | 0 0 1 1 | 2b Crate | 4b CPM Num | 3b CPChip | 3b Local coords | 0 0 0 | 5b tau isolation/veto      | 8b tau energy      |
   */

   // Retrieve EMTAU RoIs (they are built by EMTAUTrigger)

   DataVector<CPCMXTopoData> * emtau = 0;
   if( evtStore()->contains<DataVector<CPCMXTopoData>>(m_emTauLocation) ) {
      CHECK( evtStore()->retrieve(emtau, m_emTauLocation) );
   } else {
      ATH_MSG_WARNING("No CPCMXTopoDataCollection with SG key '" << m_emTauLocation.toString() << "' found in the event. No EM or TAU input for the L1Topo simulation.");
      return StatusCode::RECOVERABLE;
   }

   ATH_MSG_DEBUG("Filling the input event. Number of emtau topo data objects: " << emtau->size());
   // topoData is read in in reverse in order to obtain the same crate order as in the hardware (3->0, not 0->3)
   for(auto iTopoData = emtau->rbegin(); iTopoData != emtau->rend(); ++iTopoData) {
      const CPCMXTopoData *topoData = *iTopoData;

      // fill the vector of TOBs
      std::vector< CPTopoTOB > tobs;
      topoData->tobs(tobs);
      ATH_MSG_DEBUG("Emtau topo data object has # TOBs: " << tobs.size());
      for(const CPTopoTOB & tob : tobs ) {
         ATH_MSG_DEBUG( "EMTAU TOB with cmx = " << tob.cmx() << "[" << (tob.cmx()==0?"EM":"TAU") << "]"
                        << " : e = " << setw(3) << tob.et() << ", isolation " << tob.isolation()
                        << ", eta = " << setw(2) << tob.eta() << ", phi = " << tob.phi()
                        << ", ieta = " << setw(2) << tob.ieta() << ", iphi = " << tob.iphi()
                        << ", word = " << hex << tob.roiWord() << dec
                        );

         TCS::ClusterTOB cl(tob.et(), tob.isolation(), tob.ieta(), tob.iphi(), tob.cmx()==0 ? TCS::CLUSTER : TCS::TAU, tob.roiWord() );
         cl.setEtaDouble( tob.eta() );
         cl.setPhiDouble( tob.phi() );
         
         if(tob.cmx()==0) {
            inputEvent.addCluster( cl );
            m_hEMEt->Fill(cl.Et());
            m_hEMEtaPhi->Fill(cl.eta(),cl.phi());
         } else {
            inputEvent.addTau( cl );            
            m_hTauEt->Fill(cl.Et());
            m_hTauEtaPhi->Fill(cl.eta(),cl.phi());
         }
      }
      if(topoData->overflow()){
          inputEvent.setOverflowFromEmtauInput(true);
          ATH_MSG_DEBUG("setOverflowFromEmtauInput : true");
      }
   }
   return StatusCode::SUCCESS;
}

void 
EMTauInputProvider::CalculateCoordinates(int32_t roiWord, double & eta, double & phi) const {
   static CPRoIDecoder get;
   static double TwoPI = 2 * M_PI;
   CoordinateRange coordRange = get.coordinate( roiWord );
   
   eta = coordRange.eta();
   phi = coordRange.phi();
   if( phi > M_PI ) phi -= TwoPI;
}
