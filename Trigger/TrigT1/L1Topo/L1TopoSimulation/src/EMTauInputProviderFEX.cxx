// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#include "./EMTauInputProviderFEX.h"

#include <math.h>

#include "GaudiKernel/ITHistSvc.h"

#include "TrigT1CaloEvent/EmTauROI_ClassDEF.h"
#include "TrigT1CaloEvent/CPCMXTopoData.h"

#include "TrigT1Interfaces/CPRoIDecoder.h"

#include "L1TopoEvent/eEmTOB.h"
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

   auto isEDMvalid = m_eEDMKey.initialize();

   //Temporarily check EDM status by hand to avoid the crash!
   if (isEDMvalid != StatusCode::SUCCESS) {
     ATH_MSG_WARNING("No EDM found for eFEX..");
   }

   return StatusCode::SUCCESS;
}


void
EMTauInputProviderFEX::handle(const Incident& incident) {
   if (incident.type()!="BeginRun") return;
   ATH_MSG_DEBUG( "In BeginRun incident");

   string histPath = "/EXPERT/" + name() + "/";
   replace( histPath.begin(), histPath.end(), '.', '/'); 

   auto hEmEt = std::make_unique<TH1I>( "eEmTOBEt", "eEm TOB Et", 400, 0, 400);
   hEmEt->SetXTitle("E_{T}");
   auto hEmEtaPhi = std::make_unique<TH2I>( "eEmTOBPhiEta", "eEm TOB Location", 200, -200, 200, 64, 0, 128);
   hEmEtaPhi->SetXTitle("#eta");
   hEmEtaPhi->SetYTitle("#phi");

   auto hTauEt = std::make_unique<TH1I>( "TauTOBEt", "Tau TOB Et", 400, 0, 400);
   hTauEt->SetXTitle("E_{T}");
   auto hTauEtaPhi = std::make_unique<TH2I>( "TauTOBPhiEta", "Tau TOB Location", 100, -50, 50, 64, 0, 64);
   hTauEtaPhi->SetXTitle("#eta");
   hTauEtaPhi->SetYTitle("#phi");


   if (m_histSvc->regShared( histPath + "eEmTOBEt", std::move(hEmEt), m_hEmEt ).isSuccess()){
     ATH_MSG_DEBUG("eEmTOBEt histogram has been registered successfully for EMTauProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register eEmTOBEt histogram for EmTauProvider");
   }
   if (m_histSvc->regShared( histPath + "eEmTOBPhiEta", std::move(hEmEtaPhi), m_hEmEtaPhi ).isSuccess()){
     ATH_MSG_DEBUG("eEmTOBPhiEta histogram has been registered successfully for EMTauProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register eEmTOBPhiEta histogram for EMTauProvider");
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
  //Temporarily check EDM status by hand to avoid the crash!
  if(!myRoIContainer.isValid()){
    ATH_MSG_WARNING("Could not retrieve EDM Container " << m_eEDMKey.key() << ". No eFEX input for L1Topo");
    
    return StatusCode::SUCCESS;
  }

  for(const auto it : * myRoIContainer){
    const xAOD::eFexEMRoI* eFexRoI = it;
    ATH_MSG_DEBUG( "EDM eFex Number: " 
		   << +eFexRoI->eFexNumber() // returns an 8 bit unsigned integer referring to the eFEX number 
		   << " et: " 
		   << eFexRoI->et() // returns the et value of the EM cluster in MeV
		   << " etTOB: " 
		   << eFexRoI->etTOB() // returns the et value of the EM cluster in units of 100 MeV
		   << " eta: "
		   << eFexRoI->eta() // returns a floating point global eta (will be at full precision 0.025, but currently only at 0.1)
		   << " phi: "
		   << eFexRoI->phi() // returns a floating point global phi
		   << " is TOB? "
		   << +eFexRoI->isTOB() // returns 1 if true, returns 0 if xTOB)
		  );

    if (!eFexRoI->isTOB()) {return StatusCode::SUCCESS;}

    unsigned int EtTopo = eFexRoI->etTOB();
    int etaTopo = eFexRoI->iEtaTopo();
    int phiTopo = eFexRoI->iPhiTopo();

    //Em TOB
    TCS::eEmTOB eem( EtTopo, 0, etaTopo, static_cast<unsigned int>(phiTopo), TCS::EEM , static_cast<long int>(eFexRoI->Word0()) );
    eem.setEtDouble( static_cast<double>(EtTopo/10.) );
    eem.setEtaDouble( static_cast<double>(etaTopo/40.) );
    eem.setPhiDouble( static_cast<double>(phiTopo/20.) );
    
    inputEvent.addeEm( eem );
    
    m_hEmEt->Fill(eem.EtDouble());  // GeV
    m_hEmEtaPhi->Fill(eem.eta(),eem.phi());

  }

  return StatusCode::SUCCESS;

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
