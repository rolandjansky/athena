/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "GaudiKernel/ITHistSvc.h"

#include "JetInputProviderFEX.h"
#include "TrigT1CaloEvent/JetROI_ClassDEF.h"
#include "L1TopoEvent/TopoInputEvent.h"
#include "TrigT1CaloEvent/EnergyRoI_ClassDEF.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "GaudiKernel/PhysicalConstants.h"

using namespace std;
using namespace LVL1;


// jFex to L1Topo conversion factors
const int JetInputProviderFEX::m_Et_conversion = 2;            // 200 MeV to 100 MeV
const float JetInputProviderFEX::m_sumEt_conversion = 0.01;    // 1 MeV to 100 MeV
const int JetInputProviderFEX::m_phi_conversion = 2;           // 10 x phi to 20 x phi
const int JetInputProviderFEX::m_eta_conversion = 4;           // 10 x eta to 40 x eta

const float JetInputProviderFEX::m_EtDouble_conversion = 10.;      // 100 MeV to GeV
const float JetInputProviderFEX::m_sumEtDouble_conversion = 10.;   // 100 MeV to GeV
const float JetInputProviderFEX::m_phiDouble_conversion = 20.;     // 20 x phi to phi
const float JetInputProviderFEX::m_etaDouble_conversion = 40.;     // 40 x eta to eta


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

   CHECK(m_jJet_EDMKey.initialize(SG::AllowEmpty));
   CHECK(m_jLJet_EDMKey.initialize(SG::AllowEmpty));
   CHECK(m_jTau_EDMKey.initialize(SG::AllowEmpty));
   CHECK(m_jXE_EDMKey.initialize(SG::AllowEmpty));
   CHECK(m_jTE_EDMKey.initialize(SG::AllowEmpty));

   return StatusCode::SUCCESS;
}


void
JetInputProviderFEX::handle(const Incident& incident) {
   if (incident.type()!="BeginRun") return;
   ATH_MSG_DEBUG( "In BeginRun incident");

   string histPath = "/EXPERT/" + name() + "/";
   replace( histPath.begin(), histPath.end(), '.', '/'); 

   // jJet
   auto h_jJetPt = std::make_unique<TH1I>( "jJetTOBPt", "jJet TOB Pt", 200, 0, 400);
   h_jJetPt->SetXTitle("p_{T} [GeV]");

   auto h_jJetPhiEta = std::make_unique<TH2I>( "jJetTOBPhiEta", "jJet TOB Location", 200, -200, 200, 128, 0, 128);
   h_jJetPhiEta->SetXTitle("#eta#times40");
   h_jJetPhiEta->SetYTitle("#phi#times20");


   if (m_histSvc->regShared( histPath + "jJetTOBPt", std::move(h_jJetPt), m_h_jJetPt ).isSuccess()){
     ATH_MSG_DEBUG("jJetTOB Pt histogram has been registered successfully from jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jJetTOB Pt histogram from jFexInputProvider");
   }

   if (m_histSvc->regShared( histPath + "jJetTOBPhiEta", std::move(h_jJetPhiEta), m_h_jJetPhiEta ).isSuccess()){
     ATH_MSG_DEBUG("jJetTOB PhiEta histogram has been registered successfully from jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jJetTOB PhiEta histogram from jFexInputProvider");
   }

   // jLJet
   auto h_jLJetPt = std::make_unique<TH1I>( "jLJetTOBPt", "jLJet TOB Pt", 200, 0, 1000);
   h_jLJetPt->SetXTitle("p_{T} [GeV]");

   auto h_jLJetPhiEta = std::make_unique<TH2I>( "jLJetTOBPhiEta", "jLJet TOB Location", 200, -200, 200, 128, 0, 128);
   h_jLJetPhiEta->SetXTitle("#eta#times40");
   h_jLJetPhiEta->SetYTitle("#phi#times20");


   if (m_histSvc->regShared( histPath + "jLJetTOBPt", std::move(h_jLJetPt), m_h_jLJetPt ).isSuccess()){
     ATH_MSG_DEBUG("jLJetTOB Pt histogram has been registered successfully from jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jLJetTOB Pt histogram from jFexInputProvider");
   }

   if (m_histSvc->regShared( histPath + "jLJetTOBPhiEta", std::move(h_jLJetPhiEta), m_h_jLJetPhiEta ).isSuccess()){
     ATH_MSG_DEBUG("jLJetTOB PhiEta histogram has been registered successfully from jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jLJetTOB PhiEta histogram from jFexInputProvider");
   }

   // jTau
   auto h_jTauPt = std::make_unique<TH1I>( "jTauTOBPt", "jTau TOB Pt", 200, 0, 400);
   h_jTauPt->SetXTitle("p_{T} [GeV]");

   auto h_jTauIsolation = std::make_unique<TH1I>( "jTauTOBIsolation", "jTau TOB Isolation", 200, 0, 400);
   h_jTauIsolation->SetXTitle("Isolation [GeV]");

   auto h_jTauPhiEta = std::make_unique<TH2I>( "jTauTOBPhiEta", "jTau TOB Location", 200, -200, 200, 128, 0, 128);
   h_jTauPhiEta->SetXTitle("#eta#times40");
   h_jTauPhiEta->SetYTitle("#phi#times20");

   auto h_jTauIsolationEta = std::make_unique<TH2I>( "jTauTOBIsolationEta", "jTau TOB Isolation vs eta", 200, -200, 200, 200, 0, 400);
   h_jTauIsolationEta->SetXTitle("#eta#times40");
   h_jTauIsolationEta->SetYTitle("Isolation [GeV]");

   if (m_histSvc->regShared( histPath + "jTauTOBPt", std::move(h_jTauPt), m_h_jTauPt ).isSuccess()){
     ATH_MSG_DEBUG("jTauTOB Pt histogram has been registered successfully from jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jTauTOB Pt histogram from jFexInputProvider");
   }
   if (m_histSvc->regShared( histPath + "jTauTOBIsolation", std::move(h_jTauIsolation), m_h_jTauIsolation ).isSuccess()){
     ATH_MSG_DEBUG("jTauTOB Isolation histogram has been registered successfully from jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jTauTOB Isolation histogram from jFexInputProvider");
   }
   if (m_histSvc->regShared( histPath + "jTauTOBPhiEta", std::move(h_jTauPhiEta), m_h_jTauPhiEta ).isSuccess()){
     ATH_MSG_DEBUG("jTauTOB PhiEta histogram has been registered successfully from jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jTauTOB PhiEta histogram from jFexInputProvider");
   }
   if (m_histSvc->regShared( histPath + "jTauTOBIsolationEta", std::move(h_jTauIsolationEta), m_h_jTauIsolationEta ).isSuccess()){
     ATH_MSG_DEBUG("jTauTOB Eta/Isolation histogram has been registered successfully from jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jTauTOB Eta/Isolation histogram from jFexInputProvider");
   }

   // jEm
   auto h_jEmPt = std::make_unique<TH1I>( "jEmTOBPt", "jEm TOB Pt", 200, 0, 400);
   h_jEmPt->SetXTitle("p_{T} [GeV]");

   auto h_jEmPhiEta = std::make_unique<TH2I>( "jEmTOBPhiEta", "jEm TOB Location", 200, -200, 200, 128, 0, 128);
   h_jEmPhiEta->SetXTitle("#eta#times40");
   h_jEmPhiEta->SetYTitle("#phi#times20");

   if (m_histSvc->regShared( histPath + "jEmTOBPt", std::move(h_jEmPt), m_h_jEmPt ).isSuccess()){
     ATH_MSG_DEBUG("jEmTOB Pt histogram has been registered successfully from jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jEmTOB Pt histogram from jFexInputProvider");
   }
   if (m_histSvc->regShared( histPath + "jEmTOBPhiEta", std::move(h_jEmPhiEta), m_h_jEmPhiEta ).isSuccess()){
     ATH_MSG_DEBUG("jEmTOB PhiEta histogram has been registered successfully from jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jEmTOB PhiEta histogram from jFexInputProvider");
   }

   // jXE
   auto h_jXE_Pt = std::make_unique<TH1I>( "jXETOBPt", "jXE TOB Pt", 200, 0, 2000);
   h_jXE_Pt->SetXTitle("p_{T} [GeV]");

   auto h_jXE_Phi = std::make_unique<TH1I>( "jXETOBPhi", "jXE TOB Phi", 64, -3.2, 3.2);
   h_jXE_Phi->SetXTitle("#phi");

   if (m_histSvc->regShared( histPath + "jXETOBPt", std::move(h_jXE_Pt), m_h_jXE_Pt ).isSuccess()){
     ATH_MSG_DEBUG("jXETOB Pt histogram has been registered successfully for jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jXETOB Pt histogram for jFexInputProvider");
   }
   if (m_histSvc->regShared( histPath + "jXETOBPhi", std::move(h_jXE_Phi), m_h_jXE_Phi ).isSuccess()){
     ATH_MSG_DEBUG("jXETOB Phi histogram has been registered successfully for jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jXETOB Phi histogram for jFexInputProvider");
   }

   // jTE
   auto h_jTE_sumEt = std::make_unique<TH1I>( "jTETOBsumEt", "jTE TOB sumEt", 400, 0, 4000);
   h_jTE_sumEt->SetXTitle("p_{T} [GeV]");

   if (m_histSvc->regShared( histPath + "jTETOBsumEt", std::move(h_jTE_sumEt), m_h_jTE_sumEt ).isSuccess()){
     ATH_MSG_DEBUG("jTETOB sumEt histogram has been registered successfully for jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jTETOB sumEt histogram for jFexInputProvider");
   }

}


StatusCode
JetInputProviderFEX::fillTau(TCS::TopoInputEvent& inputEvent) const {
  if (m_jTau_EDMKey.empty()) {
    ATH_MSG_DEBUG("jFex Tau input disabled, skip filling");
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::jFexTauRoIContainer> jTau_EDM(m_jTau_EDMKey);
  ATH_CHECK(jTau_EDM.isValid());

  for(const xAOD::jFexTauRoI* jFexRoI : * jTau_EDM) {

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

    m_h_jTauPt->Fill(jtau.EtDouble());
    m_h_jTauIsolation->Fill(jtau.EtIso()/m_EtDouble_conversion);
    m_h_jTauPhiEta->Fill(jtau.eta(),jtau.phi()); 
    m_h_jTauIsolationEta->Fill(jtau.eta(),jtau.EtIso()/m_EtDouble_conversion); 
  }

  return StatusCode::SUCCESS;
}


StatusCode
JetInputProviderFEX::fillLRJet(TCS::TopoInputEvent& inputEvent) const {
  if (m_jLJet_EDMKey.empty()) {
    ATH_MSG_DEBUG("jFex LJet input disabled, skip filling");
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::jFexLRJetRoIContainer> jLJet_EDM(m_jLJet_EDMKey);
  ATH_CHECK(jLJet_EDM.isValid());
  
  for(const xAOD::jFexLRJetRoI* jFexRoI : * jLJet_EDM) {

    ATH_MSG_DEBUG( "EDM jFex jJet Number: " 
		   << jFexRoI->jFexNumber() // returns an 8 bit unsigned integer referring to the jFEX number
	           << " et: " 
		   << jFexRoI->et() // returns the et value of the jet in MeV
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

    m_h_jLJetPt->Fill(jet.EtDouble());
    m_h_jLJetPhiEta->Fill(jet.eta(),jet.phi());
  }

  return StatusCode::SUCCESS;
}


StatusCode
JetInputProviderFEX::fillSRJet(TCS::TopoInputEvent& inputEvent) const {
  if (m_jJet_EDMKey.empty()) {
    ATH_MSG_DEBUG("jFex Jet input disabled, skip filling");
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::jFexSRJetRoIContainer> jJet_EDM(m_jJet_EDMKey);
  ATH_CHECK(jJet_EDM.isValid());
  
  for(const xAOD::jFexSRJetRoI* jFexRoI : * jJet_EDM){

    ATH_MSG_DEBUG( "EDM jFex jJet Number: " 
		   << +jFexRoI->jFexNumber() // returns an 8 bit unsigned integer referring to the jFEX number 
		   << " et: " 
		   << jFexRoI->et() // returns the et value of the jet in MeV
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

    m_h_jJetPt->Fill(jet.EtDouble());
    m_h_jJetPhiEta->Fill(jet.eta(),jet.phi());
    
  }

  return StatusCode::SUCCESS;
}


StatusCode
JetInputProviderFEX::fillXE(TCS::TopoInputEvent& inputEvent) const {
  
  if (m_jXE_EDMKey.empty()) {
    ATH_MSG_DEBUG("jFex XE input disabled, skip filling");
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<xAOD::jFexMETRoIContainer> jXE_EDM(m_jXE_EDMKey);
  ATH_CHECK(jXE_EDM.isValid());
  // The jFEX MET container has 12 elements, 2 TOBs per jFEX module, so a total of 24.
  // We want to do a vector sum of Ex/Ey for the FPGA 0 and FPGA 2.  
  int global_ExTopo = 0;
  int global_EyTopo = 0;

  for(const xAOD::jFexMETRoI* jFexRoI : *jXE_EDM){

    // Get the XE components and convert to 100 MeV units
    int ExTopo = jFexRoI->tobEx()*m_Et_conversion;
    int EyTopo = jFexRoI->tobEy()*m_Et_conversion;
    int jFexNumber = jFexRoI->jFexNumber();
    int fpgaNumber = jFexRoI->fpgaNumber();  

    if( fpgaNumber==0 || fpgaNumber==2)
      {
	global_ExTopo += ExTopo;
	global_EyTopo += EyTopo;
      }

    ATH_MSG_DEBUG( "EDM jFex XE Number: "
                   << jFexNumber
                   << " FPGA Number: "
                   << fpgaNumber
                   << " Ex: "
                   << ExTopo
                   << " Ey: "
                   << EyTopo
                   );
  }

  unsigned int Et2Topo = global_ExTopo*global_ExTopo + global_EyTopo*global_EyTopo;
  unsigned int EtTopo =  std::sqrt( Et2Topo );

  TCS::jXETOB jxe( -(global_ExTopo), -(global_EyTopo), EtTopo );

  jxe.setExDouble( static_cast<double>(-global_ExTopo/m_EtDouble_conversion) );
  jxe.setEyDouble( static_cast<double>(-global_EyTopo/m_EtDouble_conversion) );
  jxe.setEtDouble( static_cast<double>(EtTopo/m_EtDouble_conversion) );
  jxe.setEt2( Et2Topo );

  inputEvent.setjXE( jxe );
  m_h_jXE_Pt->Fill(jxe.EtDouble());
  m_h_jXE_Phi->Fill( atan2(jxe.Ey(),jxe.Ex()) );

  return StatusCode::SUCCESS;
}


StatusCode
JetInputProviderFEX::fillTE(TCS::TopoInputEvent& inputEvent) const {
  
  if (m_jTE_EDMKey.empty()) {
    ATH_MSG_DEBUG("jFex TE input disabled, skip filling");
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<xAOD::jFexSumETRoIContainer> jTE_EDM(m_jTE_EDMKey);
  ATH_CHECK(jTE_EDM.isValid());
  // The jFEX SumET container has 12 elements, 2 TOBs per jFEX module, so a total of 24.
  // We want to do a sum of all Et for the FPGA 0 and FPGA 2.  
  int global_EtTopo = 0;

  for(const xAOD::jFexSumETRoI* jFexRoI : *jTE_EDM){

    // Get the TE components (upper and lower) and convert to 100 MeV units
    int EtLowerTopo = jFexRoI->Et_lower()*m_sumEt_conversion;
    int EtUpperTopo = jFexRoI->Et_upper()*m_sumEt_conversion;
    int jFexNumber = jFexRoI->jFexNumber();
    int fpgaNumber = jFexRoI->fpgaNumber();  

    if( fpgaNumber==0 || fpgaNumber==2)
      {
	global_EtTopo += EtLowerTopo;
	global_EtTopo += EtUpperTopo;
      }

    ATH_MSG_DEBUG( "EDM jFex TE Number: "
                   << jFexNumber
                   << " FPGA Number: "
                   << fpgaNumber
                   << " Et_lower: "
                   << EtLowerTopo
                   << " Et_upper: "
                   << EtUpperTopo
                   );
  }

  TCS::jTETOB jte( global_EtTopo );

  jte.setSumEtDouble( static_cast<double>(global_EtTopo/m_sumEtDouble_conversion) );

  inputEvent.setjTE( jte );
  m_h_jTE_sumEt->Fill(jte.sumEtDouble());

  return StatusCode::SUCCESS;
}


StatusCode
JetInputProviderFEX::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {
  ATH_CHECK(fillTau(inputEvent));
  ATH_CHECK(fillSRJet(inputEvent));
  ATH_CHECK(fillLRJet(inputEvent));
  ATH_CHECK(fillXE(inputEvent));
  ATH_CHECK(fillTE(inputEvent));
  return StatusCode::SUCCESS;
}
