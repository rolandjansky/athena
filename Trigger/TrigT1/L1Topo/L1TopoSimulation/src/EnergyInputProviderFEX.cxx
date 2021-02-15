/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> /* atan2 */

#include "GaudiKernel/ITHistSvc.h"

#include "EnergyInputProviderFEX.h"
#include "TrigT1CaloEvent/EnergyRoI_ClassDEF.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "TrigT1CaloEvent/EnergyTopoData.h"

#include "L1TopoEvent/ClusterTOB.h"
#include "L1TopoEvent/TopoInputEvent.h"

#include "GaudiKernel/PhysicalConstants.h"

using namespace std;
using namespace LVL1;

EnergyInputProviderFEX::EnergyInputProviderFEX(const std::string& type, const std::string& name, 
                                         const IInterface* parent) :
   base_class(type, name, parent),
   m_histSvc("THistSvc", name),
   m_gFEXMETLoc ("gXERHO_MET")
{
   declareInterface<LVL1::IInputTOBConverter>( this );
   declareProperty( "gFEXMETInput", m_gFEXMETLoc, "StoreGate location of default gFEX MET input" );
}

EnergyInputProviderFEX::~EnergyInputProviderFEX()
{}

StatusCode
EnergyInputProviderFEX::initialize() {

   CHECK(m_histSvc.retrieve());

   ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", "EnergyInputProviderFEX");
   CHECK(incidentSvc.retrieve());
   incidentSvc->addListener(this,"BeginRun", 100);
   incidentSvc.release().ignore();
  
   return StatusCode::SUCCESS;
}


void
EnergyInputProviderFEX::handle(const Incident& incident) {
   if (incident.type()!="BeginRun") return;
   ATH_MSG_DEBUG( "In BeginRun incident");

   string histPath = "/EXPERT/" + name() + "/";
   replace( histPath.begin(), histPath.end(), '.', '/'); 

   auto hPt = std::make_unique<TH1I>( "MET", "Missing ET TOB", 200, 0, 2000);
   hPt->SetXTitle("p_{T}");

   auto hPhi = std::make_unique<TH1I>( "METPhi", "MET TOB Phi", 32, -3.2, 3.2);
   hPhi->SetXTitle("#phi");

   if (m_histSvc->regShared( histPath + "MET", std::move(hPt), m_hPt ).isSuccess()){
     ATH_MSG_DEBUG("MET histogram has been registered successfully for EnergyProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register MET histogram for EnergyProvider");
   }
   if (m_histSvc->regShared( histPath + "METPhi", std::move(hPhi), m_hPhi ).isSuccess()){
     ATH_MSG_DEBUG("METPhi histogram has been registered successfully for EnergyProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register METPhi histogram for EnergyProvider");
   }

}


StatusCode
EnergyInputProviderFEX::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {

  const xAOD::EnergySumRoI* gFEXMET = nullptr; // MET from gFEX (will make it an array later)
  if(  evtStore()->contains< xAOD::EnergySumRoI >(m_gFEXMETLoc)  ) {
    CHECK ( evtStore()->retrieve( gFEXMET, m_gFEXMETLoc ) );
    ATH_MSG_DEBUG( "Retrieved gFEX MET '" << m_gFEXMETLoc << "'");
  }
  else {
    ATH_MSG_WARNING("No xAOD::EnergySumRoI with SG key '" << m_gFEXMETLoc.toString() << "' found in the event. No JET input for the L1Topo simulation.");
    return StatusCode::RECOVERABLE;
  }

  ATH_MSG_DEBUG( "EnergyTopoData" << dec
		 << ": Ex = "   << gFEXMET->energyX()
		 << ", Ey = "   << gFEXMET->energyY()
		 << ", Et = "   << gFEXMET->energyT()
		 );

  //doing this differently compared to what mentioned in the twiki https://twiki.cern.ch/twiki/bin/viewauth/Atlas/L1CaloUpgradeSimulation
  TCS::MetTOB met( -(gFEXMET->energyX()/Gaudi::Units::GeV), -(gFEXMET->energyY()/Gaudi::Units::GeV), gFEXMET->energyT()/Gaudi::Units::GeV );
  inputEvent.setMET( met );
  m_hPt->Fill(met.Et());
  m_hPhi->Fill( atan2(met.Ey(),met.Ex()) );

  /* not checking overflow currently to be enabled in release 22. 
     const bool has_overflow = (topoData->ExOverflow() or
     topoData->EyOverflow() or
     topoData->EtOverflow());
     if(has_overflow) {
     inputEvent.setOverflowFromEnergyInput(true);
     ATH_MSG_DEBUG("setOverflowFromEnergyInput : true");
     }
  */

  return StatusCode::SUCCESS;
}
