/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> /* atan2 */

#include "TH1.h"
#include "TH2.h"

#include "GaudiKernel/ITHistSvc.h"

#include "EnergyInputProvider.h"
#include "TrigT1CaloEvent/EnergyRoI_ClassDEF.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "L1TopoEvent/ClusterTOB.h"
#include "L1TopoEvent/TopoInputEvent.h"


using namespace std;
using namespace LVL1;

EnergyInputProvider::EnergyInputProvider(const std::string& type, const std::string& name, 
                                         const IInterface* parent) :
   base_class(type, name, parent),
   m_histSvc("THistSvc", name),
   m_energyLocation(TrigT1CaloDefs::EnergyTopoDataLocation)
{
   declareInterface<LVL1::IInputTOBConverter>( this );
   declareProperty( "EnergyROILocation", m_energyLocation, "Storegate key for reading the Topo Energy ROI" );

}

EnergyInputProvider::~EnergyInputProvider()
{}

StatusCode
EnergyInputProvider::initialize() {

   CHECK(m_histSvc.retrieve());

   ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", "EnergyInputProvider");
   CHECK(incidentSvc.retrieve());
   incidentSvc->addListener(this,"BeginRun", 100);
   incidentSvc.release().ignore();

   CHECK(m_energyLocation.initialize());  

   return StatusCode::SUCCESS;
}


void
EnergyInputProvider::handle(const Incident& incident) {
   if (incident.type()!="BeginRun") return;
   ATH_MSG_DEBUG( "In BeginRun incident");

   string histPath = "/EXPERT/" + name() + "/";
   replace( histPath.begin(), histPath.end(), '.', '/'); 


   m_hPt = new TH1I( "MET", "Missing ET TOB", 200, 0, 2000);
   m_hPt->SetXTitle("p_{T}");

   m_hPhi = new TH1I( "METPhi", "MET TOB Phi", 32, -3.2, 3.2);
   m_hPhi->SetXTitle("#phi");

   m_histSvc->regHist( histPath + "MET", m_hPt ).ignore();
   m_histSvc->regHist( histPath + "METPhi", m_hPhi ).ignore();

}


StatusCode
EnergyInputProvider::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {

   /** https://indico.cern.ch/conferenceDisplay.py?confId=284687
       Energy ROI:
       | 0 1 0 0 | T | 0  0 0 | 8b Missing ET Sig hits (or 8*0) | XO | 15b Signed Ex       |
       | 0 1 0 1 | T | 0  0 0 | 8b Missing ET hits              | YO | 15b Signed Ey       |
       | 0 1 1 0 | T | 0  0 0 | 8b Sum ET hits                  | TO | 15b Unsigned Sum ET |
       T = sum type: 0-normal, 1-masked
       O = overflow Ex, Ey, Et
    */

//    EnergyRoI * energyROI = 0;
//    if( ! evtStore()->retrieve(energyROI, m_energyLocation).isSuccess()) {
//       ATH_MSG_ERROR("No EnergyROI with key '" << m_energyLocation << "' found in the event. Configuration issue.");
//       return StatusCode::FAILURE;
//    }

//    ATH_MSG_DEBUG("Not filling the input event from EnergyROI. There is not enough information.");
//    ATH_MSG_DEBUG("EnergyROI: word0 = " << hex << energyROI->roiWord0()
//                  << ", word1 = " << hex << energyROI->roiWord1()
//                  << ", word2 = " << hex << energyROI->roiWord2());


   SG::ReadHandle< EnergyTopoData > topoData (m_energyLocation);

   if( !topoData.isValid()){
      ATH_MSG_WARNING("No EnergyTopoData with SG key '" << m_energyLocation.key() << "' found in the event. No MET input for the L1Topo simulation.");
      return StatusCode::RECOVERABLE;
   }

   ATH_MSG_DEBUG( "EnergyTopoData" << dec
                  << ": Ex = " << topoData->Ex()
                  << ", Ey = " << topoData->Ey()
                  << ", Et = " << topoData->Et()
                  << ", ExTC = " << topoData->ExTC()
                  << ", EyTC = " << topoData->EyTC()
                  );

   
   TCS::MetTOB met( -(topoData->Ex()), -(topoData->Ey()), topoData->Et() );
   inputEvent.setMET( met );
   const bool has_overflow = (topoData->ExOverflow() or
                              topoData->EyOverflow() or
                              topoData->EtOverflow());
   if(has_overflow) {
          inputEvent.setOverflowFromEnergyInput(true);
          ATH_MSG_DEBUG("setOverflowFromEnergyInput : true");
   }
   m_hPt->Fill(met.Et());
   m_hPhi->Fill( atan2(met.Ey(),met.Ex()) );

   return StatusCode::SUCCESS;
}
