// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

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

   auto is_eEM_EDMvalid = m_eEM_EDMKey.initialize();

   //Temporarily check EDM status by hand to avoid the crash!
   if (is_eEM_EDMvalid != StatusCode::SUCCESS) {
     ATH_MSG_WARNING("No EDM found for eFEX..");
   }

   auto is_eTau_EDMvalid = m_eTau_EDMKey.initialize();

   //Temporarily check EDM status by hand to avoid the crash!
   if (is_eTau_EDMvalid != StatusCode::SUCCESS) {
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

   auto hEmEt = std::make_unique<TH1I>( "eEmTOBEt", "eEm TOB Et", 200, 0, 200);
   hEmEt->SetXTitle("E_{T} [GeV]");
   auto hEmREta = std::make_unique<TH1I>( "eEmTOBREta", "eEm TOB rEta isolation", 3, 0, 3);
   hEmREta->SetXTitle("rEta isolation");
   auto hEmRHad = std::make_unique<TH1I>( "eEmTOBRHad", "eEm TOB rHad isolation", 3, 0, 3);
   hEmRHad->SetXTitle("rHad isolation");
   auto hEmWsTot = std::make_unique<TH1I>( "eEmTOBWsTot", "eEm TOB WsTot isolation", 3, 0, 3);
   hEmWsTot->SetXTitle("WsTot isolation");
   auto hEmPhiEta = std::make_unique<TH2I>( "eEmTOBPhiEta", "eEm TOB Location", 200, -200, 200, 64, 0, 128);
   hEmPhiEta->SetXTitle("#eta#times40");
   hEmPhiEta->SetYTitle("#phi#times20");
   auto hEmPhiEta_local = std::make_unique<TH2I>( "eEmTOBPhiEta_local", "eEm TOB local phi vs eta", 200, -200, 200, 64, 0, 128);
   hEmPhiEta_local->SetXTitle("#eta#times10");
   hEmPhiEta_local->SetYTitle("#phi#times10");
   auto hEmEtEta = std::make_unique<TH2I>( "eEmTOBEtEta", "eEm TOB Et vs eta", 200, -200, 200, 100, 0, 200);
   hEmEtEta->SetXTitle("#eta#times40");
   hEmEtEta->SetYTitle("E_{t} [GeV]");
   auto hEmEtPhi = std::make_unique<TH2I>( "eEmTOBEtPhi", "eEm TOB Et vs phi", 64, 0, 128, 100, 0, 200);
   hEmEtPhi->SetXTitle("#phi#times20");
   hEmEtPhi->SetYTitle("E_{t} [GeV]");
   auto hEmEtREta = std::make_unique<TH2I>( "eEmTOBEtREta", "eEm TOB Et vs rEta isolation", 3, 0, 3, 100, 0, 200);
   hEmEtREta->SetXTitle("rEta isolation");
   hEmEtREta->SetYTitle("E_{t} [GeV]");
   auto hEmEtRHad = std::make_unique<TH2I>( "eEmTOBEtRHad", "eEm TOB Et vs rHad isolation", 3, 0, 3, 100, 0, 200);
   hEmEtRHad->SetXTitle("rHad isolation");
   hEmEtRHad->SetYTitle("E_{t} [GeV]");
   auto hEmEtWsTot = std::make_unique<TH2I>( "eEmTOBEtWsTot", "eEm TOB Et vs WsTot isolation", 3, 0, 3, 100, 0, 200);
   hEmEtWsTot->SetXTitle("WsTot isolation");
   hEmEtWsTot->SetYTitle("E_{t} [GeV]");

   auto hTauEt = std::make_unique<TH1I>( "eTauTOBEt", "eTau TOB Et", 200, 0, 200);
   hTauEt->SetXTitle("E_{T} [GeV]");
   auto hTauRCore = std::make_unique<TH1I>( "eTauRCore", "eTau TOB rCore isolation", 3, 0, 3);
   hTauRCore->SetXTitle("rCore isolation");
   auto hTauRHad = std::make_unique<TH1I>( "eTauRHad", "eTau TOB rHad isolation", 3, 0, 3);
   hTauRHad->SetXTitle("rHad isolation");
   auto hTauPhiEta = std::make_unique<TH2I>( "eTauTOBPhiEta", "eTau TOB Location", 400, -200, 200, 128, 0, 128);
   hTauPhiEta->SetXTitle("#eta#times40");
   hTauPhiEta->SetYTitle("#phi#times20");
   auto hTauEtEta = std::make_unique<TH2I>( "eTauTOBEtEta", "eTau TOB Et vs eta", 200, -200, 200, 100, 0, 200);
   hTauEtEta->SetXTitle("#eta#times40");
   hTauEtEta->SetYTitle("E_{t} [GeV]");
   auto hTauEtPhi = std::make_unique<TH2I>( "eTauTOBEtPhi", "eTau TOB Et vs phi", 64, 0, 128, 100, 0, 200);
   hTauEtPhi->SetXTitle("#phi#times20");
   hTauEtPhi->SetYTitle("E_{t} [GeV]");
   auto hTauEtRCore = std::make_unique<TH2I>( "eTauTOBEtRCore", "eTau TOB Et vs rCore isolation", 3, 0, 3, 100, 0, 200);
   hTauEtRCore->SetXTitle("rCore isolation");
   hTauEtRCore->SetYTitle("E_{t} [GeV]");
   auto hTauEtRHad = std::make_unique<TH2I>( "eTauTOBEtRHad", "eTau TOB Et vs rHad isolation", 3, 0, 3, 100, 0, 200);
   hTauEtRHad->SetXTitle("rHad isolation");
   hTauEtRHad->SetYTitle("E_{t} [GeV]");

   if (m_histSvc->regShared( histPath + "eEmTOBEt", std::move(hEmEt), m_hEmEt ).isSuccess()){
     ATH_MSG_DEBUG("eEmTOBEt histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eEmTOBEt histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eEmTOBREta", std::move(hEmREta), m_hEmREta ).isSuccess()){
     ATH_MSG_DEBUG("eEmTOBREta histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eEmTOBREta histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eEmTOBRHad", std::move(hEmRHad), m_hEmRHad ).isSuccess()){
     ATH_MSG_DEBUG("eEmTOBRHad histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eEmTOBRHad histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eEmTOBWsTot", std::move(hEmWsTot), m_hEmWsTot ).isSuccess()){
     ATH_MSG_DEBUG("eEmTOBWsTot histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eEmTOBWsTot histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eEmTOBPhiEta", std::move(hEmPhiEta), m_hEmPhiEta ).isSuccess()){
     ATH_MSG_DEBUG("eEmTOBPhiEta histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eEmTOBPhiEta histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eEmTOBPhiEta_local", std::move(hEmPhiEta_local), m_hEmPhiEta_local ).isSuccess()){
     ATH_MSG_DEBUG("eEmTOBPhiEta (local coordinates) histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eEmTOBPhiEta (local coordinates) histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eEmTOBEtEta", std::move(hEmEtEta), m_hEmEtEta ).isSuccess()){
     ATH_MSG_DEBUG("eEmTOBEtEta histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eEmTOBEtEta histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eEmTOBEtPhi", std::move(hEmEtPhi), m_hEmEtPhi ).isSuccess()){
     ATH_MSG_DEBUG("eEmTOBEtPhi histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eEmTOBEtPhi histogram for EmTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eEmTOBEtREta", std::move(hEmEtREta), m_hEmEtREta ).isSuccess()){
     ATH_MSG_DEBUG("eEmTOBEtREta histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eEmTOBEtREta histogram for EmTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eEmTOBEtRHad", std::move(hEmEtRHad), m_hEmEtRHad ).isSuccess()){
     ATH_MSG_DEBUG("eEmTOBEtRHad histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eEmTOBEtRHad histogram for EmTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eEmTOBEtWsTot", std::move(hEmEtWsTot), m_hEmEtWsTot ).isSuccess()){
     ATH_MSG_DEBUG("eEmTOBEtWsTot histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eEmTOBEtWsTot histogram for EmTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eTauTOBEt", std::move(hTauEt), m_hTauEt ).isSuccess()){
     ATH_MSG_DEBUG("eTauTOBEt histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eTauTOBEt histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eTauRCore", std::move(hTauRCore), m_hTauRCore ).isSuccess()){
     ATH_MSG_DEBUG("eTauRCore histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eTauRCore histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eTauRHad", std::move(hTauRHad), m_hTauRHad ).isSuccess()){
     ATH_MSG_DEBUG("eTauRHad histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eTauRHad histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eTauTOBPhiEta", std::move(hTauPhiEta), m_hTauPhiEta ).isSuccess()){
     ATH_MSG_DEBUG("eTauTOBPhiEta histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eTauTOBPhiEta histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eTauTOBEtEta", std::move(hTauEtEta), m_hTauEtEta ).isSuccess()){
     ATH_MSG_DEBUG("eTauTOBEtEta histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eTauTOBEtEta histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eTauTOBEtPhi", std::move(hTauEtPhi), m_hTauEtPhi ).isSuccess()){
     ATH_MSG_DEBUG("eTauTOBEtPhi histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eTauTOBEtPhi histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eTauTOBEtRCore", std::move(hTauEtRCore), m_hTauEtRCore ).isSuccess()){
     ATH_MSG_DEBUG("eTauTOBEtRCore histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eTauTOBEtRCore histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eTauTOBEtRHad", std::move(hTauEtRHad), m_hTauEtRHad ).isSuccess()){
     ATH_MSG_DEBUG("eTauTOBEtRHad histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eTauTOBEtRHad histogram for EMTauProviderFEX");
   }
}



StatusCode
EMTauInputProviderFEX::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {

  //eEM
  SG::ReadHandle<xAOD::eFexEMRoIContainer> eEM_EDM(m_eEM_EDMKey);
  //Temporarily check EDM status by hand to avoid the crash!
  if(!eEM_EDM.isValid()){
    ATH_MSG_WARNING("Could not retrieve EDM Container " << m_eEM_EDMKey.key() << ". No eFEX input for L1Topo");
    
    return StatusCode::SUCCESS;
  }

  for(const auto it : * eEM_EDM){
    const xAOD::eFexEMRoI* eFexRoI = it;
    ATH_MSG_DEBUG( "EDM eFex Number: " 
		   << +eFexRoI->eFexNumber() // returns an 8 bit unsigned integer referring to the eFEX number 
		   << " et: " 
		   << eFexRoI->et() // returns the et value of the EM cluster in MeV
		   << " etTOB: " 
		   << eFexRoI->etTOB() // returns the et value of the EM cluster in units of 100 MeV
		   << " eta: "
		   << eFexRoI->eta() // returns a floating point global eta
		   << " phi: "
		   << eFexRoI->phi() // returns a floating point global phi
		   << " reta: "
		   << eFexRoI->RetaThresholds() // jet disc 1
		   << " rhad: "
		   << eFexRoI->RhadThresholds() // jet disc 2
		   << " wstot: "
		   << eFexRoI->WstotThresholds() // jet disc 3
		  );


    unsigned int EtTopo = eFexRoI->etTOB();
    int etaTopo = eFexRoI->iEtaTopo();
    int phiTopo = eFexRoI->iPhiTopo();
    unsigned int reta = eFexRoI->RetaThresholds();
    unsigned int rhad = eFexRoI->RhadThresholds();
    unsigned int wstot = eFexRoI->WstotThresholds();

    //Em TOB
    TCS::eEmTOB eem( EtTopo, 0, etaTopo, static_cast<unsigned int>(phiTopo), TCS::EEM , static_cast<long int>(eFexRoI->word0()) );
    eem.setEtDouble( static_cast<double>(EtTopo/10.) );
    eem.setEtaDouble( static_cast<double>(etaTopo/40.) );
    eem.setPhiDouble( static_cast<double>(phiTopo/20.) );
    eem.setReta( reta );
    eem.setRhad( rhad );
    eem.setWstot( wstot );
    
    inputEvent.addeEm( eem );
    
    m_hEmEt->Fill(eem.EtDouble());
    m_hEmREta->Fill(eem.Reta());
    m_hEmRHad->Fill(eem.Rhad());
    m_hEmWsTot->Fill(eem.Wstot());
    m_hEmPhiEta->Fill(eem.eta(),eem.phi());
    m_hEmPhiEta_local->Fill(eFexRoI->iEta(),eFexRoI->iPhi());
    m_hEmEtEta->Fill(eem.eta(),eem.EtDouble());
    m_hEmEtPhi->Fill(eem.phi(),eem.EtDouble());
    m_hEmEtREta->Fill(eem.Reta(),eem.EtDouble());
    m_hEmEtRHad->Fill(eem.Rhad(),eem.EtDouble());
    m_hEmEtWsTot->Fill(eem.Wstot(),eem.EtDouble());

  }

  //eTau
  SG::ReadHandle<xAOD::eFexTauRoIContainer> eTau_EDM(m_eTau_EDMKey);
  //Temporarily check EDM status by hand to avoid the crash!
  if(!eTau_EDM.isValid()){
    ATH_MSG_WARNING("Could not retrieve EDM Container " << m_eTau_EDMKey.key() << ". No eFEX input for L1Topo");
    
    return StatusCode::SUCCESS;
  }
  
  // TODO: read eTau isolation variables from eTau RoI
  for(const auto it : * eTau_EDM){
    const xAOD::eFexTauRoI* eFexTauRoI = it;
    ATH_MSG_DEBUG( "EDM eFex Number: " 
		   << +eFexTauRoI->eFexNumber() // returns an 8 bit unsigned integer referring to the eFEX number 
		   << " et: " 
		   << eFexTauRoI->et() // returns the et value of the Tau cluster in MeV
		   << " etTOB: " 
		   << eFexTauRoI->etTOB() // returns the et value of the Tau cluster in units of 100 MeV
		   << " eta: "
		   << eFexTauRoI->eta() // returns a floating point global eta 
		   << " phi: "
		   << eFexTauRoI->phi() // returns a floating point global phi
		   << " rCore: "
		   << eFexTauRoI->rCoreThresholds() 
		   << " rHad: "
		   << eFexTauRoI->rHadThresholds()
		  );


    unsigned int EtTopo = eFexTauRoI->etTOB();
    int etaTopo = eFexTauRoI->iEtaTopo();
    int phiTopo = eFexTauRoI->iPhiTopo();
    unsigned int rCore = eFexTauRoI->rCoreThresholds();
    unsigned int rHad = eFexTauRoI->rHadThresholds();

    //Tau TOB
    TCS::eTauTOB etau( EtTopo, 0, etaTopo, static_cast<unsigned int>(phiTopo), TCS::ETAU );
    etau.setEtDouble(  static_cast<double>(EtTopo/10.) );
    etau.setEtaDouble( static_cast<double>(etaTopo/40.) );
    etau.setPhiDouble( static_cast<double>(phiTopo/20.) );

    etau.setRCore( rCore );
    etau.setRHad( rHad );

    inputEvent.addeTau( etau );
    inputEvent.addcTau( etau );

    m_hTauEt->Fill(etau.EtDouble());
    m_hTauRCore->Fill(etau.rCore());  
    m_hTauRHad->Fill(etau.rHad());  
    m_hTauPhiEta->Fill(etau.eta(),etau.phi());
    m_hTauEtEta->Fill(etau.eta(),etau.EtDouble());
    m_hTauEtPhi->Fill(etau.phi(),etau.EtDouble());
    m_hTauEtRCore->Fill(etau.rCore(),etau.EtDouble());
    m_hTauEtRHad->Fill(etau.rHad(),etau.EtDouble());
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
