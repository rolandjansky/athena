/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> /* atan2 */

#include "GaudiKernel/ITHistSvc.h"

#include "gFexInputProvider.h"
#include "TrigT1CaloEvent/EnergyRoI_ClassDEF.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "L1TopoEvent/TopoInputEvent.h"

#include <unistd.h>

using namespace std;
using namespace LVL1;


// gFex to L1Topo conversion factors
const int gFexInputProvider::m_EtJet_conversion = 2;            // 200 MeV to 100 MeV for gJet, gLJet
const double gFexInputProvider::m_EtGlobal_conversion = 0.01;   // 1 MeV to 100 MeV for gXE, gTE
const int gFexInputProvider::m_phi_conversion = 1;              // 20 x phi to 20 x phi
const int gFexInputProvider::m_eta_conversion = 40;             // eta to 40 x eta

const double gFexInputProvider::m_EtDoubleJet_conversion = 0.1;       // 100 MeV to GeV for gJet, gLJet
const double gFexInputProvider::m_EtDoubleGlobal_conversion = 0.1;    // 100 MeV to GeV for gXE, gTE
const double gFexInputProvider::m_phiDouble_conversion = 0.05;        // 20 x phi to phi
const double gFexInputProvider::m_etaDouble_conversion = 0.025;       // 40 x eta to eta


gFexInputProvider::gFexInputProvider(const std::string& type, const std::string& name, 
                                         const IInterface* parent) :
   base_class(type, name, parent),
   m_histSvc("THistSvc", name)
{
   declareInterface<LVL1::IInputTOBConverter>( this );
}


StatusCode
gFexInputProvider::initialize() {

  CHECK(m_histSvc.retrieve());
  
  ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", "gFexInputProvider");
  CHECK(incidentSvc.retrieve());
  incidentSvc->addListener(this,"BeginRun", 100);
  incidentSvc.release().ignore();
  
  ATH_CHECK(m_gJet_EDMKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_gLJet_EDMKey.initialize(SG::AllowEmpty));

  ATH_CHECK(m_gXEJWOJ_EDMKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_gMHT_EDMKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_gXENC_EDMKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_gXERHO_EDMKey.initialize(SG::AllowEmpty));

  ATH_CHECK(m_gTE_EDMKey.initialize(SG::AllowEmpty));

  return StatusCode::SUCCESS;
}


void
gFexInputProvider::handle(const Incident& incident) {
   if (incident.type()!="BeginRun") return;
   ATH_MSG_DEBUG( "In BeginRun incident");

   string histPath = "/EXPERT/" + name() + "/";
   replace( histPath.begin(), histPath.end(), '.', '/'); 

   // gJet
   auto h_gJet_Pt = std::make_unique<TH1I>("gJetTOBPt", "gJet TOB Pt", 200, 0, 400);
   h_gJet_Pt->SetXTitle("p_{T} [GeV]");
   auto h_gJet_PhiEta = std::make_unique<TH2I>("gJetTOBPhiEta", "gJet TOB Location", 200, -200, 200, 128, 0, 128);
   h_gJet_PhiEta->SetXTitle("#eta#times40");
   h_gJet_PhiEta->SetYTitle("#phi#times20");

   if (m_histSvc->regShared( histPath + "gJetTOBPt", std::move(h_gJet_Pt), m_h_gJet_Pt ).isSuccess()){
     ATH_MSG_DEBUG("gJetTOB Pt histogram has been registered successfully for gFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register gJetTOB Pt histogram for gFexInputProvider");
   }
   if (m_histSvc->regShared( histPath + "gJetTOBPhiEta", std::move(h_gJet_PhiEta), m_h_gJet_PhiEta ).isSuccess()){
     ATH_MSG_DEBUG("gJetTOB PhiEta histogram has been registered successfully for gFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register gJetTOB PhiEta histogram for gFexInputProvider");
   }

   // gLJet
   auto h_gLJet_Pt = std::make_unique<TH1I>("gLJetTOBPt", "gLJet TOB Pt", 200, 0, 1000);
   h_gLJet_Pt->SetXTitle("p_{T} [GeV]");
   auto h_gLJet_PhiEta = std::make_unique<TH2I>("gLJetTOBPhiEta", "gLJet TOB Location", 200, -200, 200, 128, 0, 128);
   h_gLJet_PhiEta->SetXTitle("#eta#times40");
   h_gLJet_PhiEta->SetYTitle("#phi#times20");

   if (m_histSvc->regShared( histPath + "gLJetTOBPt", std::move(h_gLJet_Pt), m_h_gLJet_Pt ).isSuccess()){
     ATH_MSG_DEBUG("gLJetTOB Pt histogram has been registered successfully for gFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register gLJetTOB Pt histogram for gFexInputProvider");
   }
   if (m_histSvc->regShared( histPath + "gLJetTOBPhiEta", std::move(h_gLJet_PhiEta), m_h_gLJet_PhiEta ).isSuccess()){
     ATH_MSG_DEBUG("gLJetTOB PhiEta histogram has been registered successfully for gFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register gLJetTOB PhiEta histogram for gFexInputProvider");
   }

   // gXEJWOJ
   auto h_gXEJWOJ_Pt = std::make_unique<TH1I>("gXEJWOJTOBPt", "gXEJWOJ TOB Pt", 200, 0, 2000);
   h_gXEJWOJ_Pt->SetXTitle("p_{T} [GeV]");
   auto h_gXEJWOJ_Phi = std::make_unique<TH1I>("gXEJWOJTOBPhi", "gXEJWOJ TOB Phi", 64, -3.2, 3.2);
   h_gXEJWOJ_Phi->SetXTitle("#phi");

   if (m_histSvc->regShared( histPath + "gXEJWOJTOBPt", std::move(h_gXEJWOJ_Pt), m_h_gXEJWOJ_Pt ).isSuccess()){
     ATH_MSG_DEBUG("gXEJWOJTOB Pt histogram has been registered successfully for gFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register gXEJWOJTOB Pt histogram for gFexInputProvider");
   }
   if (m_histSvc->regShared( histPath + "gXEJWOJTOBPhi", std::move(h_gXEJWOJ_Phi), m_h_gXEJWOJ_Phi ).isSuccess()){
     ATH_MSG_DEBUG("gXEJWOJTOB Phi histogram has been registered successfully for gFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register gXEJWOJTOB Phi histogram for gFexInputProvider");
   }

   // gMHT
   auto h_gMHT_Pt = std::make_unique<TH1I>("gMHTTOBPt", "gMHT TOB Pt", 200, 0, 2000);
   h_gMHT_Pt->SetXTitle("p_{T} [GeV]");
   auto h_gMHT_Phi = std::make_unique<TH1I>("gMHTTOBPhi", "gMHT TOB Phi", 64, -3.2, 3.2);
   h_gMHT_Phi->SetXTitle("#phi");

   if (m_histSvc->regShared( histPath + "gMHTTOBPt", std::move(h_gMHT_Pt), m_h_gMHT_Pt ).isSuccess()){
     ATH_MSG_DEBUG("gMHTTOB Pt histogram has been registered successfully for gFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register gMHTTOB Pt histogram for gFexInputProvider");
   }
   if (m_histSvc->regShared( histPath + "gMHTTOBPhi", std::move(h_gMHT_Phi), m_h_gMHT_Phi ).isSuccess()){
     ATH_MSG_DEBUG("gMHTTOB Phi histogram has been registered successfully for gFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register gMHTTOB Phi histogram for gFexInputProvider");
   }

   // gXENC
   auto h_gXENC_Pt = std::make_unique<TH1I>("gXENCTOBPt", "gXENC TOB Pt", 200, 0, 2000);
   h_gXENC_Pt->SetXTitle("p_{T} [GeV]");

   if (m_histSvc->regShared( histPath + "gXENCTOBPt", std::move(h_gXENC_Pt), m_h_gXENC_Pt ).isSuccess()){
     ATH_MSG_DEBUG("gXENCTOB Pt histogram has been registered successfully for gFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register gXENCTOB Pt histogram for gFexInputProvider");
   }

   // gXERHO
   auto h_gXERHO_Pt = std::make_unique<TH1I>("gXERHOTOBPt", "gXERHO TOB Pt", 200, 0, 2000);
   h_gXERHO_Pt->SetXTitle("p_{T} [GeV]");

   if (m_histSvc->regShared( histPath + "gXERHOTOBPt", std::move(h_gXERHO_Pt), m_h_gXERHO_Pt ).isSuccess()){
     ATH_MSG_DEBUG("gXERHOTOB Pt histogram has been registered successfully for gFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register gXERHOTOB Pt histogram for gFexInputProvider");
   }

   // gTE
   auto h_gTE_sumEt = std::make_unique<TH1I>("gTETOBsumEt", "gTE TOB sumEt", 400, 0, 4000);
   h_gTE_sumEt->SetXTitle("p_{T} [GeV]");

   if (m_histSvc->regShared( histPath + "gTETOBsumEt", std::move(h_gTE_sumEt), m_h_gTE_sumEt ).isSuccess()){
     ATH_MSG_DEBUG("gTETOB Et histogram has been registered successfully for gFexInputProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register gTETOB sumEt histogram for gFexInputProvider");
   }

}


StatusCode
gFexInputProvider::fillSRJet(TCS::TopoInputEvent& inputEvent) const {

  if (m_gJet_EDMKey.empty()) {
    ATH_MSG_DEBUG("gFex Jet input disabled, skip filling");
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<xAOD::gFexJetRoIContainer> gJet_EDM(m_gJet_EDMKey);
  ATH_CHECK(gJet_EDM.isValid());

  for(const xAOD::gFexJetRoI* gFexRoI : * gJet_EDM) {

    auto jetType = gFexRoI->gFexType();
    if ( jetType != 1 and jetType != 2 ) { continue; } // 1 = gFEX gBlockLead, 2 = gFEX gBlockSub (L1Topo gJet)

    ATH_MSG_DEBUG( "EDM gFex Jet type: "
                   << gFexRoI->gFexType() 
                   << " Et: " 
		   << gFexRoI->et() // returns the et value of the jet in units of MeV
                   << " gFexTobEt: " 
		   << gFexRoI->gFexTobEt() // returns the et value of the jet in units of 200 MeV
		   << " eta: "
		   << gFexRoI->eta() // returns a floating point eta corresponding to the center of the tower (fixed values, defined in gFexJetRoI_v1.cxx)
		   << " phi: "
		   << gFexRoI->phi() // returns a floating point phi corresponding to the center of the tower
		   << " iPhiTopo: "
		   << gFexRoI->iPhiTopo() // returns an integer phi from 0 to 127
		   );
    
   unsigned int EtTopo = gFexRoI->gFexTobEt()*m_EtJet_conversion;
   unsigned int phiTopo = gFexRoI->iPhiTopo()*m_phi_conversion;
   int etaTopo = static_cast<int>(gFexRoI->eta()*m_eta_conversion);

   TCS::gJetTOB gJet( EtTopo, etaTopo, phiTopo );
 
   gJet.setEtDouble( static_cast<double>(EtTopo*m_EtDoubleJet_conversion) );
   gJet.setEtaDouble( static_cast<double>(etaTopo*m_etaDouble_conversion) );
   gJet.setPhiDouble( static_cast<double>(phiTopo*m_phiDouble_conversion) );
 
   inputEvent.addgJet( gJet );
   m_h_gJet_Pt->Fill( gJet.EtDouble() );
   m_h_gJet_PhiEta->Fill( gJet.eta(), gJet.phi() );

   }

   return StatusCode::SUCCESS;

}


StatusCode
gFexInputProvider::fillLRJet(TCS::TopoInputEvent& inputEvent) const {

  if (m_gLJet_EDMKey.empty()) {
    ATH_MSG_DEBUG("gFex LJet input disabled, skip filling");
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<xAOD::gFexJetRoIContainer> gLJet_EDM(m_gLJet_EDMKey);
  ATH_CHECK(gLJet_EDM.isValid());

  for(const xAOD::gFexJetRoI* gFexRoI : * gLJet_EDM) {

    auto jetType = gFexRoI->gFexType();
    if ( jetType != 3 ) { continue; } // 3 = gFEX gJet (L1Topo gLJet)

    ATH_MSG_DEBUG( "EDM gFex LJet type: "
                   << gFexRoI->gFexType() 
                   << " Et: " 
		   << gFexRoI->et() // returns the et value of the jet in units of MeV
                   << " gFexTobEt: " 
		   << gFexRoI->gFexTobEt() // returns the et value of the jet in units of 200 MeV
		   << " eta: "
		   << gFexRoI->eta() // returns a floating point eta corresponding to the center of the tower (fixed values, defined in gFexJetRoI_v1.cxx)
		   << " phi: "
		   << gFexRoI->phi() // returns a floating point phi corresponding to the center of the tower
		   << " iPhiTopo: "
		   << gFexRoI->iPhiTopo() // returns an integer phi from 0 to 127
		   );
    
   unsigned int EtTopo = gFexRoI->gFexTobEt()*m_EtJet_conversion;
   unsigned int phiTopo = gFexRoI->iPhiTopo()*m_phi_conversion;
   int etaTopo = static_cast<int>(gFexRoI->eta()*m_eta_conversion);

   TCS::gLJetTOB gJet( EtTopo, etaTopo, phiTopo );
 
   gJet.setEtDouble( static_cast<double>(EtTopo*m_EtDoubleJet_conversion) );
   gJet.setEtaDouble( static_cast<double>(etaTopo*m_etaDouble_conversion) );
   gJet.setPhiDouble( static_cast<double>(phiTopo*m_phiDouble_conversion) );
 
   inputEvent.addgLJet( gJet );
   m_h_gLJet_Pt->Fill( gJet.EtDouble() );
   m_h_gLJet_PhiEta->Fill( gJet.eta(), gJet.phi() );

   }

   return StatusCode::SUCCESS;

}


StatusCode
gFexInputProvider::fillXEJWOJ(TCS::TopoInputEvent& inputEvent) const {

  if (m_gXEJWOJ_EDMKey.empty()) {
    ATH_MSG_DEBUG("gFex XEJWOJ input disabled, skip filling");
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gXEJWOJ_EDM(m_gXEJWOJ_EDMKey);
  ATH_CHECK(gXEJWOJ_EDM.isValid());

  for(const xAOD::gFexGlobalRoI* gFexRoI : * gXEJWOJ_EDM) {

    auto globalType = gFexRoI->globalType();
    if ( globalType != 2 ) { continue; } // 2 = MET components (METx, METy)

    ATH_MSG_DEBUG( "EDM gFex XEJWOJ type: "
                   << gFexRoI->globalType()
                   << " Ex: " 
                   << gFexRoI->METquantityOne() // returns the Ex component in MeV
                   << " Ey: " 
		   << gFexRoI->METquantityTwo() // returns the Ey component in MeV
		   );
    
   int ExTopo = gFexRoI->METquantityOne()*m_EtGlobal_conversion;
   int EyTopo = gFexRoI->METquantityTwo()*m_EtGlobal_conversion;
   unsigned int Et2Topo = ExTopo*ExTopo + EyTopo*EyTopo;
   unsigned int EtTopo  = std::sqrt(Et2Topo);
 
   TCS::gXETOB gxe( -ExTopo, -EyTopo, EtTopo, TCS::GXEJWOJ );
 
   gxe.setExDouble( static_cast<double>(ExTopo*m_EtDoubleGlobal_conversion) );
   gxe.setEyDouble( static_cast<double>(EyTopo*m_EtDoubleGlobal_conversion) );
   gxe.setEtDouble( static_cast<double>(EtTopo*m_EtDoubleGlobal_conversion) );
   gxe.setEt2( Et2Topo );
 
   inputEvent.setgXEJWOJ( gxe );
   m_h_gXEJWOJ_Pt->Fill( gxe.EtDouble() );
   m_h_gXEJWOJ_Phi->Fill( atan2(gxe.Ey(), gxe.Ex()) );

   }

   return StatusCode::SUCCESS;

}


StatusCode
gFexInputProvider::fillMHT(TCS::TopoInputEvent& inputEvent) const {

  if (m_gMHT_EDMKey.empty()) {
    ATH_MSG_DEBUG("gFex MHT input disabled, skip filling");
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gMHT_EDM(m_gMHT_EDMKey);
  ATH_CHECK(gMHT_EDM.isValid());

  for(const xAOD::gFexGlobalRoI* gFexRoI : * gMHT_EDM) {

    auto globalType = gFexRoI->globalType();
    if ( globalType != 3 ) { continue; } // 3 = MET hard term components (MHTx, MHTy)

    ATH_MSG_DEBUG( "EDM gFex MHT type: "
                   << gFexRoI->globalType()
                   << " Ex: " 
                   << gFexRoI->METquantityOne() // returns the Ex component in MeV
                   << " Ey: " 
		   << gFexRoI->METquantityTwo() // returns the Ey component in MeV
		   );
    
   int ExTopo = gFexRoI->METquantityOne()*m_EtGlobal_conversion;
   int EyTopo = gFexRoI->METquantityTwo()*m_EtGlobal_conversion;
   unsigned int Et2Topo = ExTopo*ExTopo + EyTopo*EyTopo;
   unsigned int EtTopo  = std::sqrt(Et2Topo);
 
   TCS::gXETOB gxe( -ExTopo, -EyTopo, EtTopo, TCS::GMHT );
 
   gxe.setExDouble( static_cast<double>(ExTopo*m_EtDoubleGlobal_conversion) );
   gxe.setEyDouble( static_cast<double>(EyTopo*m_EtDoubleGlobal_conversion) );
   gxe.setEtDouble( static_cast<double>(EtTopo*m_EtDoubleGlobal_conversion) );
   gxe.setEt2( Et2Topo );
 
   inputEvent.setgMHT( gxe );
   m_h_gMHT_Pt->Fill( gxe.EtDouble() );
   m_h_gMHT_Phi->Fill( atan2(gxe.Ey(), gxe.Ex()) );

   }

   return StatusCode::SUCCESS;

}


StatusCode
gFexInputProvider::fillXENC(TCS::TopoInputEvent& inputEvent) const {

  if (m_gXENC_EDMKey.empty()) {
    ATH_MSG_DEBUG("gFex XENC input disabled, skip filling");
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gXENC_EDM(m_gXENC_EDMKey);
  ATH_CHECK(gXENC_EDM.isValid());

  for(const xAOD::gFexGlobalRoI* gFexRoI : * gXENC_EDM) {

    ATH_MSG_DEBUG( "EDM gFex XENC type: "
                   << gFexRoI->globalType()
                   << " Ex: " 
                   << gFexRoI->METquantityOne() // returns the Ex component in MeV
                   << " Ey: " 
		   << gFexRoI->METquantityTwo() // returns the Ey component in MeV
		   );
    
   int ExTopo = gFexRoI->METquantityOne()*m_EtGlobal_conversion;
   int EyTopo = gFexRoI->METquantityTwo()*m_EtGlobal_conversion;
   unsigned int Et2Topo = ExTopo*ExTopo + EyTopo*EyTopo;
   unsigned int EtTopo  = std::sqrt(Et2Topo);
 
   TCS::gXETOB gxe( -ExTopo, -EyTopo, EtTopo, TCS::GXENC );
 
   gxe.setExDouble( static_cast<double>(ExTopo*m_EtDoubleGlobal_conversion) );
   gxe.setEyDouble( static_cast<double>(EyTopo*m_EtDoubleGlobal_conversion) );
   gxe.setEtDouble( static_cast<double>(EtTopo*m_EtDoubleGlobal_conversion) );
   gxe.setEt2( Et2Topo );
 
   inputEvent.setgXENC( gxe );
   m_h_gXENC_Pt->Fill( gxe.EtDouble() );

   }

   return StatusCode::SUCCESS;

}


StatusCode
gFexInputProvider::fillXERHO(TCS::TopoInputEvent& inputEvent) const {

  if (m_gXERHO_EDMKey.empty()) {
    ATH_MSG_DEBUG("gFex XENC input disabled, skip filling");
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gXERHO_EDM(m_gXERHO_EDMKey);
  ATH_CHECK(gXERHO_EDM.isValid());

  for(const xAOD::gFexGlobalRoI* gFexRoI : * gXERHO_EDM) {

    ATH_MSG_DEBUG( "EDM gFex XERHO type: "
                   << gFexRoI->globalType()
                   << " Ex: " 
                   << gFexRoI->METquantityOne() // returns the Ex component in MeV
                   << " Ey: " 
		   << gFexRoI->METquantityTwo() // returns the Ey component in MeV
		   );
    
   int ExTopo = gFexRoI->METquantityOne()*m_EtGlobal_conversion;
   int EyTopo = gFexRoI->METquantityTwo()*m_EtGlobal_conversion;
   unsigned int Et2Topo = ExTopo*ExTopo + EyTopo*EyTopo;
   unsigned int EtTopo  = std::sqrt(Et2Topo);
 
   TCS::gXETOB gxe( -ExTopo, -EyTopo, EtTopo, TCS::GXERHO );
 
   gxe.setExDouble( static_cast<double>(ExTopo*m_EtDoubleGlobal_conversion) );
   gxe.setEyDouble( static_cast<double>(EyTopo*m_EtDoubleGlobal_conversion) );
   gxe.setEtDouble( static_cast<double>(EtTopo*m_EtDoubleGlobal_conversion) );
   gxe.setEt2( Et2Topo );
 
   inputEvent.setgXERHO( gxe );
   m_h_gXERHO_Pt->Fill( gxe.EtDouble() );

   }

   return StatusCode::SUCCESS;

}


StatusCode
gFexInputProvider::fillTE(TCS::TopoInputEvent& inputEvent) const {

  if (m_gTE_EDMKey.empty()) {
    ATH_MSG_DEBUG("gFex TE input disabled, skip filling");
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gTE_EDM(m_gTE_EDMKey);
  ATH_CHECK(gTE_EDM.isValid());

  for(const xAOD::gFexGlobalRoI* gFexRoI : * gTE_EDM) {

    auto globalType = gFexRoI->globalType();
    if ( globalType != 1 ) { continue; } // 1 = scalar values (MET, SumET)

    ATH_MSG_DEBUG( "EDM gFex TE type: "
                   << gFexRoI->globalType()
                   << " sumEt: " 
		   << gFexRoI->METquantityTwo() // returns sumEt in MeV
		   );
    
   unsigned int sumEtTopo = gFexRoI->METquantityTwo()*m_EtGlobal_conversion;

   TCS::gTETOB gte( sumEtTopo, TCS::GTE );
 
   gte.setSumEtDouble( static_cast<double>(sumEtTopo*m_EtDoubleGlobal_conversion) );
 
   inputEvent.setgTE( gte );
   m_h_gTE_sumEt->Fill( gte.sumEtDouble() );

   }

   return StatusCode::SUCCESS;

}


StatusCode
gFexInputProvider::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {
  ATH_CHECK(fillSRJet(inputEvent));
  ATH_CHECK(fillLRJet(inputEvent));

  ATH_CHECK(fillXEJWOJ(inputEvent));
  ATH_CHECK(fillMHT(inputEvent));
  ATH_CHECK(fillXENC(inputEvent));
  ATH_CHECK(fillXERHO(inputEvent));

  ATH_CHECK(fillTE(inputEvent));
  return StatusCode::SUCCESS;
}

