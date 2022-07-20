/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "GaudiKernel/ITHistSvc.h"

#include "jFexInputProvider.h"
#include "TrigT1CaloEvent/JetROI_ClassDEF.h"
#include "L1TopoEvent/TopoInputEvent.h"
#include "TrigT1CaloEvent/EnergyRoI_ClassDEF.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "GaudiKernel/PhysicalConstants.h"

using namespace std;
using namespace LVL1;


// jFex to L1Topo conversion factors
const int jFexInputProvider::m_Et_conversion = 2;            // 200 MeV to 100 MeV
const double jFexInputProvider::m_sumEt_conversion = 0.01;   // 1 MeV to 100 MeV
const int jFexInputProvider::m_phi_conversion = 2;           // 10 x phi to 20 x phi
const int jFexInputProvider::m_eta_conversion = 4;           // 10 x eta to 40 x eta

const double jFexInputProvider::m_EtDouble_conversion = 0.1;       // 100 MeV to GeV
const double jFexInputProvider::m_sumEtDouble_conversion = 0.1;    // 100 MeV to GeV
const double jFexInputProvider::m_phiDouble_conversion = 0.05;     // 20 x phi to phi
const double jFexInputProvider::m_etaDouble_conversion = 0.025;    // 40 x eta to eta


jFexInputProvider::jFexInputProvider(const std::string& type, const std::string& name, 
                                   const IInterface* parent) :
   base_class(type, name, parent),
   m_histSvc("THistSvc", name)
{
   declareInterface<LVL1::IInputTOBConverter>( this );
}

jFexInputProvider::~jFexInputProvider()
{}

StatusCode
jFexInputProvider::initialize() {

   CHECK(m_histSvc.retrieve());

   ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", "jFexInputProvider");
   CHECK(incidentSvc.retrieve());
   incidentSvc->addListener(this,"BeginRun", 100);
   incidentSvc.release().ignore();

   CHECK(m_jJet_EDMKey.initialize(SG::AllowEmpty));
   CHECK(m_jLJet_EDMKey.initialize(SG::AllowEmpty));
   CHECK(m_jEM_EDMKey.initialize(SG::AllowEmpty));
   CHECK(m_jTau_EDMKey.initialize(SG::AllowEmpty));
   CHECK(m_jXE_EDMKey.initialize(SG::AllowEmpty));
   CHECK(m_jTE_EDMKey.initialize(SG::AllowEmpty));

   return StatusCode::SUCCESS;
}


void
jFexInputProvider::handle(const Incident& incident) {
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

   auto h_jEmIsolation = std::make_unique<TH1I>( "jEmTOBIsolation", "jEm TOB Isolation", 4, 0, 4);
   h_jEmIsolation->SetXTitle("Isolation");

   auto h_jEmFrac1 = std::make_unique<TH1I>( "jEmTOBFrac1", "jEm TOB Frac1", 4, 0, 4);
   h_jEmFrac1->SetXTitle("Frac1");

   auto h_jEmFrac2 = std::make_unique<TH1I>( "jEmTOBFrac2", "jEm TOB Frac2", 4, 0, 4);
   h_jEmFrac2->SetXTitle("Frac2");

   auto h_jEmPhiEta = std::make_unique<TH2I>( "jEmTOBPhiEta", "jEm TOB Location", 200, -200, 200, 128, 0, 128);
   h_jEmPhiEta->SetXTitle("#eta#times40");
   h_jEmPhiEta->SetYTitle("#phi#times20");

   auto h_jEmIsolationEta = std::make_unique<TH2I>( "jEmTOBIsolationEta", "jEm TOB Isolation vs eta", 200, -200, 200, 4, 0, 4);
   h_jEmIsolationEta->SetXTitle("#eta#times40");
   h_jEmIsolationEta->SetYTitle("Isolation");

   auto h_jEmFrac1Eta = std::make_unique<TH2I>( "jEmTOBFrac1Eta", "jEm TOB Frac1 vs eta", 200, -200, 200, 4, 0, 4);
   h_jEmFrac1Eta->SetXTitle("#eta#times40");
   h_jEmFrac1Eta->SetYTitle("Frac1");

   auto h_jEmFrac2Eta = std::make_unique<TH2I>( "jEmTOBFrac2Eta", "jEm TOB Frac2 vs eta", 200, -200, 200, 4, 0, 4);
   h_jEmFrac2Eta->SetXTitle("#eta#times40");
   h_jEmFrac2Eta->SetYTitle("Frac2");

   if (m_histSvc->regShared( histPath + "jEmTOBPt", std::move(h_jEmPt), m_h_jEmPt ).isSuccess()){
     ATH_MSG_DEBUG("jEmTOB Pt histogram has been registered successfully from jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jEmTOB Pt histogram from jFexInputProvider");
   }
   if (m_histSvc->regShared( histPath + "jEmTOBIsolation", std::move(h_jEmIsolation), m_h_jEmIsolation ).isSuccess()){
     ATH_MSG_DEBUG("jEmTOB Isolation histogram has been registered successfully from jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jEmTOB Isolation histogram from jFexInputProvider");
   }
   if (m_histSvc->regShared( histPath + "jEmTOBFrac1", std::move(h_jEmFrac1), m_h_jEmFrac1 ).isSuccess()){
     ATH_MSG_DEBUG("jEmTOB Frac1 histogram has been registered successfully from jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jEmTOB Frac1 histogram from jFexInputProvider");
   }
   if (m_histSvc->regShared( histPath + "jEmTOBFrac2", std::move(h_jEmFrac2), m_h_jEmFrac2 ).isSuccess()){
     ATH_MSG_DEBUG("jEmTOB Frac2 histogram has been registered successfully from jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jEmTOB Frac2 histogram from jFexInputProvider");
   }
   if (m_histSvc->regShared( histPath + "jEmTOBPhiEta", std::move(h_jEmPhiEta), m_h_jEmPhiEta ).isSuccess()){
     ATH_MSG_DEBUG("jEmTOB PhiEta histogram has been registered successfully from jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jEmTOB PhiEta histogram from jFexInputProvider");
   }
   if (m_histSvc->regShared( histPath + "jEmTOBIsolationEta", std::move(h_jEmIsolationEta), m_h_jEmIsolationEta ).isSuccess()){
     ATH_MSG_DEBUG("jEmTOB IsolationEta histogram has been registered successfully from jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jEmTOB IsolationEta histogram from jFexInputProvider");
   }
   if (m_histSvc->regShared( histPath + "jEmTOBFrac1Eta", std::move(h_jEmFrac1Eta), m_h_jEmFrac1Eta ).isSuccess()){
     ATH_MSG_DEBUG("jEmTOB Frac1Eta histogram has been registered successfully from jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jEmTOB Frac1Eta histogram from jFexInputProvider");
   }
   if (m_histSvc->regShared( histPath + "jEmTOBFrac2Eta", std::move(h_jEmFrac2Eta), m_h_jEmFrac2Eta ).isSuccess()){
     ATH_MSG_DEBUG("jEmTOB Frac2Eta histogram has been registered successfully from jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jEmTOB Frac2Eta histogram from jFexInputProvider");
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

   // jTEC
   auto h_jTEC_sumEt = std::make_unique<TH1I>( "jTECTOBsumEt", "jTEC TOB sumEt", 400, 0, 4000);
   h_jTEC_sumEt->SetXTitle("p_{T} [GeV]");

   if (m_histSvc->regShared( histPath + "jTECTOBsumEt", std::move(h_jTEC_sumEt), m_h_jTEC_sumEt ).isSuccess()){
     ATH_MSG_DEBUG("jTECTOB sumEt histogram has been registered successfully for jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jTECTOB sumEt histogram for jFexInputProvider");
   }

   // jTEFWD
   auto h_jTEFWD_sumEt = std::make_unique<TH1I>( "jTEFWDTOBsumEt", "jTEFWD TOB sumEt", 400, 0, 4000);
   h_jTEFWD_sumEt->SetXTitle("p_{T} [GeV]");

   if (m_histSvc->regShared( histPath + "jTEFWDTOBsumEt", std::move(h_jTEFWD_sumEt), m_h_jTEFWD_sumEt ).isSuccess()){
     ATH_MSG_DEBUG("jTEFWDTOB sumEt histogram has been registered successfully for jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jTEFWDTOB sumEt histogram for jFexInputProvider");
   }

   // jTEFWDA
   auto h_jTEFWDA_sumEt = std::make_unique<TH1I>( "jTEFWDATOBsumEt", "jTEFWDA TOB sumEt", 400, 0, 4000);
   h_jTEFWDA_sumEt->SetXTitle("p_{T} [GeV]");

   if (m_histSvc->regShared( histPath + "jTEFWDATOBsumEt", std::move(h_jTEFWDA_sumEt), m_h_jTEFWDA_sumEt ).isSuccess()){
     ATH_MSG_DEBUG("jTEFWDATOB sumEt histogram has been registered successfully for jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jTEFWDATOB sumEt histogram for jFexInputProvider");
   }

   // jTEFWDC
   auto h_jTEFWDC_sumEt = std::make_unique<TH1I>( "jTEFWDCTOBsumEt", "jTEFWDC TOB sumEt", 400, 0, 4000);
   h_jTEFWDC_sumEt->SetXTitle("p_{T} [GeV]");

   if (m_histSvc->regShared( histPath + "jTEFWDCTOBsumEt", std::move(h_jTEFWDC_sumEt), m_h_jTEFWDC_sumEt ).isSuccess()){
     ATH_MSG_DEBUG("jTEFWDCTOB sumEt histogram has been registered successfully for jFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register jTEFWDCTOB sumEt histogram for jFexInputProvider");
   }

}


StatusCode
jFexInputProvider::fillEM(TCS::TopoInputEvent& inputEvent) const {
  if (m_jEM_EDMKey.empty()) {
    ATH_MSG_DEBUG("jFex EM input disabled, skip filling");
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::jFexFwdElRoIContainer> jEM_EDM(m_jEM_EDMKey);
  ATH_CHECK(jEM_EDM.isValid());

  for(const xAOD::jFexFwdElRoI* jFexRoI : * jEM_EDM) {

    ATH_MSG_DEBUG( "EDM jFex Em Number: " 
           << +jFexRoI->jFexNumber() // returns an 8 bit unsigned integer referring to the jFEX number 
           << " et: " 
           << jFexRoI->et() // returns the et value of the jet in MeV unit
           << " tobEt: " 
           << jFexRoI->tobEt() // returns the et value of the jet in units of 200 MeV
           << " globalEta: "
           << jFexRoI->globalEta() // returns global eta in units of 0.1
           << " globalPhi: "
           << jFexRoI->globalPhi() // returns global phi in units of 0.1
           << " tobEMIso: "
           << +jFexRoI->tobEMIso() // returns isolation bits
           << " tobEMf1: "
           << +jFexRoI->tobEMf1() // returns isolation bits
           << " tobEMf2: "
           << +jFexRoI->tobEMf2() // returns isolation bits
           );
       
    unsigned int EtTopo = jFexRoI->tobEt()*m_Et_conversion;
    unsigned int phiTopo = jFexRoI->globalPhi()*m_phi_conversion;
    int etaTopo = jFexRoI->globalEta()*m_eta_conversion;
    unsigned int isolation = jFexRoI->tobEMIso();
    unsigned int frac1 = jFexRoI->tobEMf2();
    unsigned int frac2 = jFexRoI->tobEMf1();
   
    // Avoid the events with 0 Et (events below threshold)
    if (EtTopo==0) continue;

    TCS::jEmTOB jem( EtTopo, etaTopo, phiTopo );
    jem.setEtDouble( static_cast<double>(EtTopo*m_EtDouble_conversion) );
    jem.setEtaDouble( static_cast<double>(etaTopo*m_etaDouble_conversion) );
    jem.setPhiDouble( static_cast<double>(phiTopo*m_phiDouble_conversion) );
    jem.setIsolation( isolation );
    jem.setFrac1( frac1 );
    jem.setFrac2( frac2 );

    inputEvent.addjEm( jem );

    m_h_jEmPt->Fill(jem.EtDouble());
    m_h_jEmIsolation->Fill(jem.isolation());
    m_h_jEmFrac1->Fill(jem.frac1());
    m_h_jEmFrac2->Fill(jem.frac2());
    m_h_jEmPhiEta->Fill(jem.eta(),jem.phi()); 
    m_h_jEmIsolationEta->Fill(jem.eta(),jem.isolation()); 
    m_h_jEmFrac1Eta->Fill(jem.eta(),jem.frac1()); 
    m_h_jEmFrac2Eta->Fill(jem.eta(),jem.frac2()); 
  }

  return StatusCode::SUCCESS;
}


StatusCode
jFexInputProvider::fillTau(TCS::TopoInputEvent& inputEvent) const {
  if (m_jTau_EDMKey.empty()) {
    ATH_MSG_DEBUG("jFex Tau input disabled, skip filling");
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::jFexTauRoIContainer> jTau_EDM(m_jTau_EDMKey);
  ATH_CHECK(jTau_EDM.isValid());

  for(const xAOD::jFexTauRoI* jFexRoI : * jTau_EDM) {

    ATH_MSG_DEBUG( "EDM jFex Tau Number: " 
		   << +jFexRoI->jFexNumber() // returns an 8 bit unsigned integer referring to the jFEX number 
		   << " et: " 
		   << jFexRoI->et() // returns the et value of the jet in MeV unit
		   << " tobEt: " 
		   << jFexRoI->tobEt() // returns the et value of the jet in units of 200 MeV
		   << " globalEta: "
		   << jFexRoI->globalEta() // returns global eta in units of 0.1
		   << " globalPhi: "
		   << jFexRoI->globalPhi() // returns global phi in units of 0.1
		   << " isolation: "
		   << jFexRoI->tobIso() // returns isolation value in units of 200 MeV
		   );
       
    unsigned int EtTopo = jFexRoI->tobEt()*m_Et_conversion;
    unsigned int phiTopo = jFexRoI->globalPhi()*m_phi_conversion;
    int etaTopo = jFexRoI->globalEta()*m_eta_conversion;
    unsigned int isolation = jFexRoI->tobIso()*m_Et_conversion;
   
    // Avoid the events with 0 Et (events below threshold)
    if (EtTopo==0) continue;

    TCS::jTauTOB jtau( EtTopo, etaTopo, phiTopo );
    jtau.setEtDouble( static_cast<double>(EtTopo*m_EtDouble_conversion) );
    jtau.setEtaDouble( static_cast<double>(etaTopo*m_etaDouble_conversion) );
    jtau.setPhiDouble( static_cast<double>(phiTopo*m_phiDouble_conversion) );
    jtau.setEtIso( isolation );

    inputEvent.addjTau( jtau );
    inputEvent.addcTau( jtau );

    m_h_jTauPt->Fill(jtau.EtDouble());
    m_h_jTauIsolation->Fill(jtau.EtIso()*m_EtDouble_conversion);
    m_h_jTauPhiEta->Fill(jtau.eta(),jtau.phi()); 
    m_h_jTauIsolationEta->Fill(jtau.eta(),jtau.EtIso()*m_EtDouble_conversion); 
  }

  return StatusCode::SUCCESS;
}


StatusCode
jFexInputProvider::fillLRJet(TCS::TopoInputEvent& inputEvent) const {
  if (m_jLJet_EDMKey.empty()) {
    ATH_MSG_DEBUG("jFex LJet input disabled, skip filling");
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::jFexLRJetRoIContainer> jLJet_EDM(m_jLJet_EDMKey);
  ATH_CHECK(jLJet_EDM.isValid());
  
  for(const xAOD::jFexLRJetRoI* jFexRoI : * jLJet_EDM) {

    ATH_MSG_DEBUG( "EDM jFex LJet Number: " 
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

    TCS::jLJetTOB jet( EtTopo, etaTopo, phiTopo );
    jet.setEtDouble( static_cast<double>(EtTopo*m_EtDouble_conversion) );
    jet.setEtaDouble( static_cast<double>(etaTopo*m_etaDouble_conversion) );
    jet.setPhiDouble( static_cast<double>(phiTopo*m_phiDouble_conversion) );

    inputEvent.addjLJet( jet );

    m_h_jLJetPt->Fill(jet.EtDouble());
    m_h_jLJetPhiEta->Fill(jet.eta(),jet.phi());
  }

  return StatusCode::SUCCESS;
}


StatusCode
jFexInputProvider::fillSRJet(TCS::TopoInputEvent& inputEvent) const {
  if (m_jJet_EDMKey.empty()) {
    ATH_MSG_DEBUG("jFex Jet input disabled, skip filling");
    return StatusCode::SUCCESS;
  }
  SG::ReadHandle<xAOD::jFexSRJetRoIContainer> jJet_EDM(m_jJet_EDMKey);
  ATH_CHECK(jJet_EDM.isValid());
  
  for(const xAOD::jFexSRJetRoI* jFexRoI : * jJet_EDM){

    ATH_MSG_DEBUG( "EDM jFex Jet Number: " 
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
    jet.setEtDouble( static_cast<double>(EtTopo*m_EtDouble_conversion) );
    jet.setEtaDouble( static_cast<double>(etaTopo*m_etaDouble_conversion) );
    jet.setPhiDouble( static_cast<double>(phiTopo*m_phiDouble_conversion) );
 
    inputEvent.addjJet( jet );

    m_h_jJetPt->Fill(jet.EtDouble());
    m_h_jJetPhiEta->Fill(jet.eta(),jet.phi());
    
  }

  return StatusCode::SUCCESS;
}


StatusCode
jFexInputProvider::fillXE(TCS::TopoInputEvent& inputEvent) const {
  
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

  TCS::jXETOB jxe( -(global_ExTopo), -(global_EyTopo), EtTopo, TCS::JXE );

  jxe.setExDouble( static_cast<double>(-global_ExTopo*m_EtDouble_conversion) );
  jxe.setEyDouble( static_cast<double>(-global_EyTopo*m_EtDouble_conversion) );
  jxe.setEtDouble( static_cast<double>(EtTopo*m_EtDouble_conversion) );
  jxe.setEt2( Et2Topo );

  inputEvent.setjXE( jxe );
  m_h_jXE_Pt->Fill(jxe.EtDouble());
  m_h_jXE_Phi->Fill( atan2(jxe.Ey(),jxe.Ex()) );

  return StatusCode::SUCCESS;
}


StatusCode
jFexInputProvider::fillTE(TCS::TopoInputEvent& inputEvent) const {
  
  if (m_jTE_EDMKey.empty()) {
    ATH_MSG_DEBUG("jFex TE input disabled, skip filling");
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<xAOD::jFexSumETRoIContainer> jTE_EDM(m_jTE_EDMKey);
  ATH_CHECK(jTE_EDM.isValid());
  // The jFEX SumET container has 12 elements, 2 TOBs per jFEX module, so a total of 24.
  // jTE is the Et sum for the FPGA 0 and FPGA 2.  
  int topoTE = 0;
  // jTE variations include jTEC, jTEFWD, jTEFWDA, jTEFWDC
    // These quantities are defined according to the jFex module number
    // FWDA = 5, FWDC = 0, C = 1,2,3,4
  int topoTEC = 0;
  int topoTEFWD = 0;
  int topoTEFWDA = 0;
  int topoTEFWDC = 0;

  for(const xAOD::jFexSumETRoI* jFexRoI : *jTE_EDM){

    // Get the TE components (upper and lower) and convert to 100 MeV units
    int EtLowerTopo = jFexRoI->Et_lower()*m_sumEt_conversion;
    int EtUpperTopo = jFexRoI->Et_upper()*m_sumEt_conversion;
    int jFexNumber = jFexRoI->jFexNumber();
    int fpgaNumber = jFexRoI->fpgaNumber();  

    ATH_MSG_DEBUG( "EDM jFex TE Number: "
                   << jFexNumber
                   << " FPGA Number: "
                   << fpgaNumber
                   << " Et_lower: "
                   << EtLowerTopo
                   << " Et_upper: "
                   << EtUpperTopo
                   );

    // jTE
    if( fpgaNumber==0 || fpgaNumber==2 )
      {
	topoTE += EtLowerTopo;
	topoTE += EtUpperTopo;
      }

    // jTEC
    if( jFexNumber!=0 && jFexNumber!=5 )
      {
	topoTEC += EtLowerTopo;
	topoTEC += EtUpperTopo;
      }

    // jTEFWD
    if( jFexNumber==0 || jFexNumber==5 )
      {
	topoTEFWD += EtLowerTopo;
	topoTEFWD += EtUpperTopo;
      }

    // jTEFWDA
    if( jFexNumber==5 )
      {
	topoTEFWDA += EtLowerTopo;
	topoTEFWDA += EtUpperTopo;
      }

    // jTEFWDC
    if( jFexNumber==0 )
      {
	topoTEFWDC += EtLowerTopo;
	topoTEFWDC += EtUpperTopo;
      }
  }

  TCS::jTETOB jte( static_cast<unsigned int>(topoTE), TCS::JTE );
  TCS::jTETOB jtec( static_cast<unsigned int>(topoTEC), TCS::JTEC );
  TCS::jTETOB jtefwd( static_cast<unsigned int>(topoTEFWD), TCS::JTEFWD );
  TCS::jTETOB jtefwda( static_cast<unsigned int>(topoTEFWDA), TCS::JTEFWDA );
  TCS::jTETOB jtefwdc( static_cast<unsigned int>(topoTEFWDC), TCS::JTEFWDC );

  jte.setSumEtDouble( static_cast<double>(topoTE*m_sumEtDouble_conversion) );
  jtec.setSumEtDouble( static_cast<double>(topoTEC*m_sumEtDouble_conversion) );
  jtefwd.setSumEtDouble( static_cast<double>(topoTEFWD*m_sumEtDouble_conversion) );
  jtefwda.setSumEtDouble( static_cast<double>(topoTEFWDA*m_sumEtDouble_conversion) );
  jtefwdc.setSumEtDouble( static_cast<double>(topoTEFWDC*m_sumEtDouble_conversion) );

  inputEvent.setjTE( jte );
  inputEvent.setjTEC( jtec );
  inputEvent.setjTEFWD( jtefwd );
  inputEvent.setjTEFWDA( jtefwda );
  inputEvent.setjTEFWDC( jtefwdc );

  m_h_jTE_sumEt->Fill(jte.sumEtDouble());
  m_h_jTEC_sumEt->Fill(jtec.sumEtDouble());
  m_h_jTEFWD_sumEt->Fill(jtefwd.sumEtDouble());
  m_h_jTEFWDA_sumEt->Fill(jtefwda.sumEtDouble());
  m_h_jTEFWDC_sumEt->Fill(jtefwdc.sumEtDouble());

  return StatusCode::SUCCESS;
}


StatusCode
jFexInputProvider::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {
  ATH_CHECK(fillEM(inputEvent));
  ATH_CHECK(fillTau(inputEvent));
  ATH_CHECK(fillSRJet(inputEvent));
  ATH_CHECK(fillLRJet(inputEvent));
  ATH_CHECK(fillXE(inputEvent));
  ATH_CHECK(fillTE(inputEvent));
  return StatusCode::SUCCESS;
}
