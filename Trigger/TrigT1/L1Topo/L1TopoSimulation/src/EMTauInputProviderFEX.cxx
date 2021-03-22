/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./EMTauInputProviderFEX.h"

#include <math.h>

#include "GaudiKernel/ITHistSvc.h"

#include "TrigT1CaloEvent/EmTauROI_ClassDEF.h"
#include "TrigT1CaloEvent/CPCMXTopoData.h"

#include "TrigT1Interfaces/CPRoIDecoder.h"

#include "L1TopoEvent/ClusterTOB.h"
#include "L1TopoEvent/TopoInputEvent.h"

#include "GaudiKernel/PhysicalConstants.h"

using namespace std;
using namespace LVL1;

EMTauInputProviderFEX::EMTauInputProviderFEX(const std::string& type, const std::string& name, 
                                       const IInterface* parent) :
   base_class(type, name, parent),
   m_histSvc("THistSvc", name)
{
   declareInterface<LVL1::IInputTOBConverter>( this );
}

EMTauInputProviderFEX::~EMTauInputProviderFEX()
{}

StatusCode
EMTauInputProviderFEX::initialize() {

   CHECK(m_histSvc.retrieve());

   ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", "EnergyInputProviderFEX");
   CHECK(incidentSvc.retrieve());
   incidentSvc->addListener(this,"BeginRun", 100);
   incidentSvc.release().ignore();

   CHECK(m_eEDMKey.initialize());

   return StatusCode::SUCCESS;
}


void
EMTauInputProviderFEX::handle(const Incident& incident) {
   if (incident.type()!="BeginRun") return;
   ATH_MSG_DEBUG( "In BeginRun incident");

   string histPath = "/EXPERT/" + name() + "/";
   replace( histPath.begin(), histPath.end(), '.', '/'); 

   auto hEMEt = std::make_unique<TH1I>( "EMTOBEt", "EM TOB Et", 400, 0, 400);
   hEMEt->SetXTitle("E_{T}");
   auto hEMEtaPhi = std::make_unique<TH2I>( "EMTOBPhiEta", "EM TOB Location", 100, -50, 50, 64, 0, 64);
   hEMEtaPhi->SetXTitle("#eta");
   hEMEtaPhi->SetYTitle("#phi");

   auto hTauEt = std::make_unique<TH1I>( "TauTOBEt", "Tau TOB Et", 400, 0, 400);
   hTauEt->SetXTitle("E_{T}");
   auto hTauEtaPhi = std::make_unique<TH2I>( "TauTOBPhiEta", "Tau TOB Location", 100, -50, 50, 64, 0, 64);
   hTauEtaPhi->SetXTitle("#eta");
   hTauEtaPhi->SetYTitle("#phi");


   if (m_histSvc->regShared( histPath + "EMTOBEt", std::move(hEMEt), m_hEMEt ).isSuccess()){
     ATH_MSG_DEBUG("EMTOBEt histogram has been registered successfully for EMTauProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register EMTOBEt histogram for EMTauProvider");
   }
   if (m_histSvc->regShared( histPath + "EMTOBPhiEta", std::move(hEMEtaPhi), m_hEMEtaPhi ).isSuccess()){
     ATH_MSG_DEBUG("EMTOBPhiEta histogram has been registered successfully for EMTauProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register EMTOBPhiEta histogram for EMTauProvider");
   }

   if (m_histSvc->regShared( histPath + "TauTOBEt", std::move(hTauEt), m_hTauEt ).isSuccess()){
     ATH_MSG_DEBUG("TauTOBEt histogram has been registered successfully for EMTauProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register TauTOBEt histogram for EMTauProvider");
   }
   if (m_histSvc->regShared( histPath + "TauTOBPhiEta", std::move(hTauEtaPhi), m_hTauEtaPhi ).isSuccess()){
     ATH_MSG_DEBUG("TauTOBPhiEta histogram has been registered successfully for EMTauProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register TauTOBPhiEta histogram for EMTauProvider");
   }
   
}



StatusCode
EMTauInputProviderFEX::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {
  
  SG::ReadHandle<xAOD::eFexEMRoIContainer> myRoIContainer(m_eEDMKey);
  if(!myRoIContainer.isValid()){
    ATH_MSG_FATAL("Could not retrieve EDM Container " << m_eEDMKey.key());
    return StatusCode::FAILURE;
  }

  for(const auto it : * myRoIContainer){
    const xAOD::eFexEMRoI* eFexRoI = it;
    ATH_MSG_DEBUG( "EDM eFex Number: " 
		   << +eFexRoI->eFexNumber() // returns an 8 bit unsigned integer referring to the eFEX number 
		   << " et: " 
		   << eFexRoI->et() // returns the et value of the EM cluster in MeV
		   << " eta: "
		   << eFexRoI->eta() // returns a floating point global eta (will be at full precision 0.025, but currently only at 0.1)
		   << " phi: "
		   << eFexRoI->phi() // returns a floating point global phi
		   << " is TOB? "
		   << +eFexRoI->isTOB() // returns 1 if true, returns 0 if xTOB)
		  );

    if (!eFexRoI->isTOB()) continue;

    int ieta = ConvertEta((int)eFexRoI->iEta());
    int iphi = eFexRoI->iPhi();
    unsigned int iet = static_cast<unsigned int>(eFexRoI->et()/Gaudi::Units::GeV);

    //EM TOB
    TCS::ClusterTOB cluster( iet, static_cast<unsigned int>(0), ieta, iphi, TCS::CLUSTER , static_cast<long int>(eFexRoI->Word0()) );
    cluster.setEtaDouble( eFexRoI->eta() );
    cluster.setPhiDouble( eFexRoI->phi() );
    
    inputEvent.addCluster( cluster );
    
    m_hEMEt->Fill(cluster.Et());
    m_hEMEtaPhi->Fill(cluster.eta(),cluster.phi());

  }

  return StatusCode::SUCCESS;

}

//This will be implemented later as firmware does.
int
EMTauInputProviderFEX::ConvertEta(const int val) const {
  return val - 25 + 1;
}

void 
EMTauInputProviderFEX::CalculateCoordinates(int32_t roiWord, double & eta, double & phi) const {
   CPRoIDecoder get;
   double TwoPI = 2 * M_PI;
   CoordinateRange coordRange = get.coordinate( roiWord );
   
   eta = coordRange.eta();
   phi = coordRange.phi();
   if( phi > M_PI ) phi -= TwoPI;
}
