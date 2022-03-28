/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "GaudiKernel/ITHistSvc.h"

#include "JetInputProviderFEX.h"
#include "TrigT1CaloEvent/JetROI_ClassDEF.h"
#include "L1TopoEvent/jJetTOB.h"
#include "L1TopoEvent/jXETOB.h"
#include "L1TopoEvent/TopoInputEvent.h"
#include "TrigT1CaloEvent/EnergyRoI_ClassDEF.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
// this should go away
#include <iomanip>
#include <algorithm>

#include "GaudiKernel/PhysicalConstants.h"

using namespace std;
using namespace LVL1;


// jFex to L1Topo conversion factors
const int JetInputProviderFEX::m_Et_conversion = 2;              // 200 MeV to 100 MeV
const int JetInputProviderFEX::m_phi_conversion = 2;             // 10 x phi to 20 x phi
const int JetInputProviderFEX::m_eta_conversion = 4;             // 10 x eta to 40 x eta

const float JetInputProviderFEX::m_EtDouble_conversion = 10.;    // 100 MeV to GeV
const float JetInputProviderFEX::m_phiDouble_conversion = 20.;   // 20 x phi to phi
const float JetInputProviderFEX::m_etaDouble_conversion = 40.;   // 40 x eta to eta


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

   CHECK(m_jEDMKey.initialize(SG::AllowEmpty));
   CHECK(m_JEDMKey.initialize(SG::AllowEmpty));
   CHECK(m_jTauEDMKey.initialize(SG::AllowEmpty));
   CHECK(m_jMet_EDMKey.initialize(SG::AllowEmpty));

   return StatusCode::SUCCESS;
}


void
JetInputProviderFEX::handle(const Incident& incident) {
   if (incident.type()!="BeginRun") return;
   ATH_MSG_DEBUG( "In BeginRun incident");

   string histPath = "/EXPERT/" + name() + "/";
   replace( histPath.begin(), histPath.end(), '.', '/'); 

   // jJet
   auto hjJetPt = std::make_unique<TH1I>( "jJetTOBPt", "jJet TOB Pt", 200, 0, 400);
   hjJetPt->SetXTitle("p_{T} [GeV]");

   auto hjJetPhiEta = std::make_unique<TH2I>( "jJetTOBPhiEta", "jJet TOB Location", 200, -200, 200, 128, 0, 128);
   hjJetPhiEta->SetXTitle("#eta#times40");
   hjJetPhiEta->SetYTitle("#phi#times20");


   if (m_histSvc->regShared( histPath + "jJetTOBPt", std::move(hjJetPt), m_hjJetPt ).isSuccess()){
     ATH_MSG_DEBUG("jJetTOB Pt histogram has been registered successfully from JetProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register jJetTOB Pt histogram from JetProviderFEX");
   }

   if (m_histSvc->regShared( histPath + "jJetTOBPhiEta", std::move(hjJetPhiEta), m_hjJetPhiEta ).isSuccess()){
     ATH_MSG_DEBUG("jJetTOB PhiEta histogram has been registered successfully from JetProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register jJetTOB PhiEta histogram from JetProviderFEX");
   }

   // jLargeRJet
   auto hjLargeRJetPt = std::make_unique<TH1I>( "jLargeRJetTOBPt", "jLargeRJet TOB Pt", 200, 0, 1000);
   hjLargeRJetPt->SetXTitle("p_{T} [GeV]");

   auto hjLargeRJetPhiEta = std::make_unique<TH2I>( "jLargeRJetTOBPhiEta", "jLargeRJet TOB Location", 200, -200, 200, 128, 0, 128);
   hjLargeRJetPhiEta->SetXTitle("#eta#times40");
   hjLargeRJetPhiEta->SetYTitle("#phi#times20");


   if (m_histSvc->regShared( histPath + "jLargeRJetTOBPt", std::move(hjLargeRJetPt), m_hjLargeRJetPt ).isSuccess()){
     ATH_MSG_DEBUG("jLargeRJetTOB Pt histogram has been registered successfully from JetProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register jLargeRJetTOB Pt histogram from JetProviderFEX");
   }

   if (m_histSvc->regShared( histPath + "jLargeRJetTOBPhiEta", std::move(hjLargeRJetPhiEta), m_hjLargeRJetPhiEta ).isSuccess()){
     ATH_MSG_DEBUG("jLargeRJetTOB PhiEta histogram has been registered successfully from JetProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register jLargeRJetTOB PhiEta histogram from JetProviderFEX");
   }

   // jTau
   auto hjTauPt = std::make_unique<TH1I>( "jTauTOBPt", "jTau TOB Pt", 200, 0, 400);
   hjTauPt->SetXTitle("p_{T} [GeV]");

   auto hjTauIsolation = std::make_unique<TH1I>( "jTauTOBIsolation", "jTau TOB Isolation", 200, 0, 400);
   hjTauIsolation->SetXTitle("Isolation [GeV]");

   auto hjTauPhiEta = std::make_unique<TH2I>( "jTauTOBPhiEta", "jTau TOB Location", 200, -200, 200, 128, 0, 128);
   hjTauPhiEta->SetXTitle("#eta#times40");
   hjTauPhiEta->SetYTitle("#phi#times20");

   auto hjTauIsolationEta = std::make_unique<TH2I>( "jTauTOBIsolationEta", "jTau TOB Isolation vs eta", 200, -200, 200, 200, 0, 400);
   hjTauIsolationEta->SetXTitle("#eta#times40");
   hjTauIsolationEta->SetYTitle("Isolation [GeV]");

   if (m_histSvc->regShared( histPath + "jTauTOBPt", std::move(hjTauPt), m_hjTauPt ).isSuccess()){
     ATH_MSG_DEBUG("jTauTOB Pt histogram has been registered successfully from JetProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register jTauTOB Pt histogram from JetProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "jTauTOBIsolation", std::move(hjTauIsolation), m_hjTauIsolation ).isSuccess()){
     ATH_MSG_DEBUG("jTauTOB Isolation histogram has been registered successfully from JetProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register jTauTOB Isolation histogram from JetProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "jTauTOBPhiEta", std::move(hjTauPhiEta), m_hjTauPhiEta ).isSuccess()){
     ATH_MSG_DEBUG("jTauTOB PhiEta histogram has been registered successfully from JetProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register jTauTOB PhiEta histogram from JetProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "jTauTOBIsolationEta", std::move(hjTauIsolationEta), m_hjTauIsolationEta ).isSuccess()){
     ATH_MSG_DEBUG("jTauTOB Eta/Isolation histogram has been registered successfully from JetProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register jTauTOB Eta/Isolation histogram from JetProviderFEX");
   }

   // jEm
   auto hjEmPt = std::make_unique<TH1I>( "jEmTOBPt", "jEm TOB Pt", 200, 0, 400);
   hjEmPt->SetXTitle("p_{T} [GeV]");

   auto hjEmPhiEta = std::make_unique<TH2I>( "jEmTOBPhiEta", "jEm TOB Location", 200, -200, 200, 128, 0, 128);
   hjEmPhiEta->SetXTitle("#eta#times40");
   hjEmPhiEta->SetYTitle("#phi#times20");

   if (m_histSvc->regShared( histPath + "jEmTOBPt", std::move(hjEmPt), m_hjEmPt ).isSuccess()){
     ATH_MSG_DEBUG("jEmTOB Pt histogram has been registered successfully from JetProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register jEmTOB Pt histogram from JetProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "jEmTOBPhiEta", std::move(hjEmPhiEta), m_hjEmPhiEta ).isSuccess()){
     ATH_MSG_DEBUG("jEmTOB PhiEta histogram has been registered successfully from JetProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register jEmTOB PhiEta histogram from JetProviderFEX");
   }

   // jXE
   auto h_jxe_Pt = std::make_unique<TH1I>( "jXEPt", "jXE TOB Pt", 200, 0, 2000);
   h_jxe_Pt->SetXTitle("p_{T} [GeV]");

   auto h_jxe_Phi = std::make_unique<TH1I>( "jXEPhi", "jXE TOB Phi", 64, -3.2, 3.2);
   h_jxe_Phi->SetXTitle("#phi");

   if (m_histSvc->regShared( histPath + "jXEPt", std::move(h_jxe_Pt), m_h_jxe_Pt ).isSuccess()){
     ATH_MSG_DEBUG("jXEPt histogram has been registered successfully for JetProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register jXEPt histogram for JetProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "jXEPhi", std::move(h_jxe_Phi), m_h_jxe_Phi ).isSuccess()){
     ATH_MSG_DEBUG("jXEPhi histogram has been registered successfully for JetProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register jXEPhi histogram for JetProviderFEX");
   }

}


StatusCode
JetInputProviderFEX::fillTau(TCS::TopoInputEvent& inputEvent) const {
  if (m_jTauEDMKey.empty()) {
    ATH_MSG_DEBUG("jFex Tau input disabled, skip filling");
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::jFexTauRoIContainer> jTauContainer(m_jTauEDMKey);
  ATH_CHECK(jTauContainer.isValid());

  for(const auto it : * jTauContainer) {
    const xAOD::jFexTauRoI* jFexRoI = it;
    ATH_MSG_DEBUG( "EDM jFEX jTau Number: " 
		   << +jFexRoI->jFexNumber() // returns an 8 bit unsigned integer referring to the jFEX number 
		   << " et: " 
		   << jFexRoI->et() // returns the et value of the jet in MeV unit
		   << " tobEt: " 
		   << jFexRoI->tobEt() // returns the et value of the jet in units of 200 MeV
		   << " globalEta: "
		   << jFexRoI->globalEta() // returns global eta in units of 0.1
		   << " isolation: "
		   << jFexRoI->tobIso() // returns isolation value in units of 200 MeV
		   << " globalPhi: "
		   << jFexRoI->globalPhi() // returns global phi in units of 0.1
		   );
       
    unsigned int EtTopo = jFexRoI->tobEt()*m_Et_conversion;
    unsigned int phiTopo = jFexRoI->globalPhi()*m_phi_conversion;
    int etaTopo = jFexRoI->globalEta()*m_eta_conversion;
    unsigned int isolation = jFexRoI->tobIso()*m_Et_conversion;
   
    // Avoid the events with 0 Et (events below threshold)
    if (EtTopo==0) continue;

    TCS::jTauTOB jtau( EtTopo, etaTopo, phiTopo );
    jtau.setEtDouble( static_cast<double>(EtTopo/m_EtDouble_conversion) );
    jtau.setEtaDouble( static_cast<double>(etaTopo/m_etaDouble_conversion) );
    jtau.setPhiDouble( static_cast<double>(phiTopo/m_phiDouble_conversion) );
    jtau.setEtIso( isolation );

    inputEvent.addjTau( jtau );
    inputEvent.addcTau( jtau );

    m_hjTauPt->Fill(jtau.EtDouble());
    m_hjTauIsolation->Fill(jtau.EtIso()/m_EtDouble_conversion);
    m_hjTauPhiEta->Fill(jtau.eta(),jtau.phi()); 
    m_hjTauIsolationEta->Fill(jtau.eta(),jtau.EtIso()/m_EtDouble_conversion); 
  }

  return StatusCode::SUCCESS;
}


StatusCode
JetInputProviderFEX::fillLRJet(TCS::TopoInputEvent& inputEvent) const {
  if (m_JEDMKey.empty()) {
    ATH_MSG_DEBUG("jFex LRJet input disabled, skip filling");
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::jFexLRJetRoIContainer> JContainer(m_JEDMKey);
  ATH_CHECK(JContainer.isValid());
  
  for(const auto it : * JContainer) {
    const xAOD::jFexLRJetRoI* jFexRoI = it;
    ATH_MSG_DEBUG( "EDM jFex jJet Number: " 
		   << +jFexRoI->jFexNumber() // returns an 8 bit unsigned integer referring to the jFEX number		   << " et: " 
		   << jFexRoI->et() // returns the et value of the jet in MeV unit
	           << " tobEt: " 
		   << jFexRoI->tobEt() // returns the et value of the jet in units of 200 MeV
		   << " globalEta: "
		   << jFexRoI->globalEta() // returns global eta in units of 0.1
		   << " globalPhi: "
		   << jFexRoI->globalPhi() // returns global phi in units of 0.1
		   );
    
    unsigned int EtTopo = jFexRoI->tobEt()*m_Et_conversion;
    unsigned int phiTopo = jFexRoI->globalPhi()*m_phi_conversion;
    int etaTopo = jFexRoI->globalEta()*m_eta_conversion;

    // Avoid the events with 0 Et (events below threshold)
    if (EtTopo==0) continue;

    TCS::jLargeRJetTOB jet( EtTopo, etaTopo, phiTopo );
    jet.setEtDouble( static_cast<double>(EtTopo/m_EtDouble_conversion) );
    jet.setEtaDouble( static_cast<double>(etaTopo/m_etaDouble_conversion) );
    jet.setPhiDouble( static_cast<double>(phiTopo/m_phiDouble_conversion) );

    inputEvent.addjLargeRJet( jet );

    m_hjLargeRJetPt->Fill(jet.EtDouble());
    m_hjLargeRJetPhiEta->Fill(jet.eta(),jet.phi());
  }

  return StatusCode::SUCCESS;
}


StatusCode
JetInputProviderFEX::fillSRJet(TCS::TopoInputEvent& inputEvent) const {
  if (m_jEDMKey.empty()) {
    ATH_MSG_DEBUG("jFex SRJet input disabled, skip filling");
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::jFexSRJetRoIContainer> jContainer(m_jEDMKey);
  ATH_CHECK(jContainer.isValid());
  
  for(const auto it : * jContainer){
    const xAOD::jFexSRJetRoI* jFexRoI = it;
    ATH_MSG_DEBUG( "EDM jFex jJet Number: " 
		   << +jFexRoI->jFexNumber() // returns an 8 bit unsigned integer referring to the jFEX number 
		   << " et: " 
		   << jFexRoI->et() // returns the et value of the jet in MeV unit
		   << " tobEt: " 
		   << jFexRoI->tobEt() // returns the et value of the jet in units of 200 MeV
		   << " globalEta: "
		   << jFexRoI->globalEta() // returns global eta in units of 0.1
		   << " globalPhi: "
		   << jFexRoI->globalPhi() // returns global phi in units of 0.1
		   );

    unsigned int EtTopo = jFexRoI->tobEt()*m_Et_conversion;
    unsigned int phiTopo = jFexRoI->globalPhi()*m_phi_conversion;
    int etaTopo = jFexRoI->globalEta()*m_eta_conversion;

    // Avoid the events with 0 Et (events below threshold)
    if (EtTopo==0) continue;

    TCS::jJetTOB jet( EtTopo, etaTopo, phiTopo );
    jet.setEtDouble( static_cast<double>(EtTopo/m_EtDouble_conversion) );
    jet.setEtaDouble( static_cast<double>(etaTopo/m_etaDouble_conversion) );
    jet.setPhiDouble( static_cast<double>(phiTopo/m_phiDouble_conversion) );
 
    inputEvent.addjJet( jet );

    m_hjJetPt->Fill(jet.EtDouble());
    m_hjJetPhiEta->Fill(jet.eta(),jet.phi());
    
  }

  return StatusCode::SUCCESS;
}


StatusCode
JetInputProviderFEX::filljXE(TCS::TopoInputEvent& inputEvent) const {
  
  if (m_jMet_EDMKey.empty()) {
    ATH_MSG_DEBUG("jFex XE input disabled, skip filling");
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<xAOD::jFexMETRoIContainer> jMet_EDM(m_jMet_EDMKey);
  ATH_CHECK(jMet_EDM.isValid());
  // The jFEX MET container has 12 elements, 2 TOBs per jFEX module, so a total of 12. 
  // According to the documentation https://gitlab.cern.ch/l1calo-run3-simulation/documentation/Run3L1CaloOfflineSWReqs/-/blob/master/l1caloreqs.pdf
  // we want to do a vector sum of Etx/y for the FPGA 0 and FPGA 3.  
  int global_et_x =0;
  int global_et_y =0;

  for(const auto it : *jMet_EDM){
    const xAOD::jFexMETRoI *jFEXMet = it;
    // Get the MET components and convert to 100 MeV units
    int et_x = jFEXMet->tobEx()*m_Et_conversion;
    int et_y = jFEXMet->tobEy()*m_Et_conversion;
    int jFexNumber = jFEXMet->jFexNumber();
    int fpgaNumber = jFEXMet->fpgaNumber();  

    if( fpgaNumber==0 || fpgaNumber==2)
      {
	global_et_x+=et_x;
	global_et_y+=et_y;
      }

    ATH_MSG_DEBUG("jFEX Met Ex = " << et_x << ", Ey = " << et_y <<", jFexNumber="<<jFexNumber<<", fpgaNumber="<<fpgaNumber);
  }

  ATH_MSG_DEBUG("Global MET candidate Ex = " << global_et_x << ", Ey = " <<global_et_y);
  unsigned int et2 = global_et_x*global_et_x + global_et_y*global_et_y;
  unsigned int et =  std::sqrt( et2 );

  TCS::jXETOB jxe( -(global_et_x), -(global_et_y), et );

  jxe.setExDouble( static_cast<double>(-global_et_x/m_EtDouble_conversion) );
  jxe.setEyDouble( static_cast<double>(-global_et_y/m_EtDouble_conversion) );
  jxe.setEtDouble( static_cast<double>(et/m_EtDouble_conversion) );
  jxe.setEt2( et2 );

  inputEvent.setjXE( jxe );
  m_h_jxe_Pt->Fill(jxe.EtDouble());
  m_h_jxe_Phi->Fill( atan2(jxe.Ey(),jxe.Ex()) );

  return StatusCode::SUCCESS;
}

StatusCode
JetInputProviderFEX::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {
  ATH_CHECK(fillTau(inputEvent));
  ATH_CHECK(fillSRJet(inputEvent));
  ATH_CHECK(fillLRJet(inputEvent));
  ATH_CHECK(filljXE(inputEvent));
  return StatusCode::SUCCESS;
}
