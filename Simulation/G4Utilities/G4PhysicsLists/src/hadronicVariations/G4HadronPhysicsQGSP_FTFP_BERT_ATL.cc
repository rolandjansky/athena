//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: G4HadronPhysicsQGSP_FTFP_BERT_ATL.cc 83699 2014-09-10 07:18:25Z gcosmo $
//
//---------------------------------------------------------------------------
// Author: Alberto Ribon
// Date:   October 2017
//
// Hadron physics for the new physics list QGSP_FTFP_BERT_ATL.
// This is a modified version of FTFP_BERT_ATL for ATLAS.
// QGSP_FTFP_BERT_ATL uses QGSP above 25 GeV, with transition between
// FTFP and QGSP between 12 - 25 GeV.
//----------------------------------------------------------------------------
//
#include <iomanip>   

#include "G4HadronPhysicsQGSP_FTFP_BERT_ATL.hh"

#include "globals.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

#include "G4ChipsKaonMinusInelasticXS.hh"
#include "G4ChipsKaonPlusInelasticXS.hh"
#include "G4ChipsKaonZeroInelasticXS.hh"
#include "G4CrossSectionDataSetRegistry.hh"

#include "G4HadronCaptureProcess.hh"
#include "G4NeutronRadCapture.hh"
#include "G4NeutronInelasticXS.hh"
#include "G4NeutronCaptureXS.hh"

#include "G4PhysListUtil.hh"

// factory
#include "G4PhysicsConstructorFactory.hh"
//
G4_DECLARE_PHYSCONSTR_FACTORY(G4HadronPhysicsQGSP_FTFP_BERT_ATL);

G4ThreadLocal G4HadronPhysicsQGSP_FTFP_BERT_ATL::ThreadPrivate* G4HadronPhysicsQGSP_FTFP_BERT_ATL::tpdata=0;

G4HadronPhysicsQGSP_FTFP_BERT_ATL::G4HadronPhysicsQGSP_FTFP_BERT_ATL(G4int)
    :  G4VPhysicsConstructor("hInelastic QGSP_FTFP_BERT_ATL")
/*    , theNeutrons(0)
    , theBertiniNeutron(0)
    , theFTFPNeutron(0)
    , theQGSPNeutron(0)
    , thePiK(0)
    , theBertiniPiK(0)
    , theFTFPPiK(0)
    , theQGSPPiK(0)
    , thePro(0)
    , theBertiniPro(0)
    , theFTFPPro(0)
    , theQGSPPro(0)
    , theHyperon(0)
    , theAntiBaryon(0)
    , theFTFPAntiBaryon(0) */
    , QuasiElastic(false)
  /*    , ChipsKaonMinus(0)
    , ChipsKaonPlus(0)
    , ChipsKaonZero(0)
    , xsNeutronInelasticXS(0)
    , xsNeutronCaptureXS(0)*/
{}

G4HadronPhysicsQGSP_FTFP_BERT_ATL::G4HadronPhysicsQGSP_FTFP_BERT_ATL(const G4String& name, G4bool quasiElastic)
    :  G4VPhysicsConstructor(name) 
/*    , theNeutrons(0)
    , theBertiniNeutron(0)
    , theFTFPNeutron(0)
    , theQGSPNeutron(0)
    , thePiK(0)
    , theBertiniPiK(0)
    , theFTFPPiK(0)
    , theQGSPPiK(0)
    , thePro(0)
    , theBertiniPro(0)
    , theFTFPPro(0)
    , theQGSPPro(0)
    , theHyperon(0)
    , theAntiBaryon(0)
    , theFTFPAntiBaryon(0)*/
    , QuasiElastic(quasiElastic)
  /*    , ChipsKaonMinus(0)
    , ChipsKaonPlus(0)
    , ChipsKaonZero(0)
    , xsNeutronInelasticXS(0)
    , xsNeutronCaptureXS(0)*/
{}

void G4HadronPhysicsQGSP_FTFP_BERT_ATL::CreateModels()
{

  // First transition, between BERT and FTFP
  G4double minFTFP =  9.0 * GeV;
  G4double maxBERT = 12.0 * GeV;
  // Second transition, between FTFP and QGSP
  G4double minQGSP = 12.0 * GeV;
  G4double maxFTFP = 25.0 * GeV;

  G4bool quasiElasFTF= false;   // Use built-in quasi-elastic (not add-on)
  G4bool quasiElasQGS= true;    // For QGS, it must use it

  G4cout << " QGSP_FTFP_BERT_ATL : physics list";
  G4cout << "   Thresholds: " << G4endl;
  G4cout << "     1) between BERT and FTFP over the interval " 
         << minFTFP/GeV << " to " << maxBERT/GeV << " GeV. " << G4endl;
  G4cout << "     2) between FTFP and QGSP over the interval " 
         << minQGSP/GeV << " to " << maxFTFP/GeV << " GeV. " << G4endl;

  tpdata->theNeutrons=new G4NeutronBuilder;
  tpdata->theQGSPNeutron=new G4QGSPNeutronBuilder(quasiElasQGS);
  tpdata->theNeutrons->RegisterMe(tpdata->theQGSPNeutron);
  tpdata->theQGSPNeutron->SetMinEnergy(minQGSP);   
  tpdata->theFTFPNeutron=new G4FTFPNeutronBuilder(quasiElasFTF);
  tpdata->theNeutrons->RegisterMe(tpdata->theFTFPNeutron);
  tpdata->theFTFPNeutron->SetMinEnergy(minFTFP);
  tpdata->theFTFPNeutron->SetMaxEnergy(maxFTFP);
  tpdata->theBertiniNeutron=new G4BertiniNeutronBuilder;
  tpdata->theNeutrons->RegisterMe(tpdata->theBertiniNeutron);
  tpdata->theBertiniNeutron->SetMinEnergy(0.0*GeV);
  tpdata->theBertiniNeutron->SetMaxEnergy(maxBERT);

  tpdata->thePro=new G4ProtonBuilder;
  tpdata->theQGSPPro=new G4QGSPProtonBuilder(quasiElasQGS);
  tpdata->thePro->RegisterMe(tpdata->theQGSPPro);
  tpdata->theQGSPPro->SetMinEnergy(minQGSP);   
  tpdata->theFTFPPro=new G4FTFPProtonBuilder(quasiElasFTF);
  tpdata->thePro->RegisterMe(tpdata->theFTFPPro);
  tpdata->theFTFPPro->SetMinEnergy(minFTFP);
  tpdata->theFTFPPro->SetMaxEnergy(maxFTFP);
  tpdata->theBertiniPro=new G4BertiniProtonBuilder;
  tpdata->thePro->RegisterMe(tpdata->theBertiniPro);
  tpdata->theBertiniPro->SetMaxEnergy(maxBERT);

  tpdata->thePiK=new G4PiKBuilder;
  tpdata->theQGSPPiK=new G4QGSPPiKBuilder(quasiElasQGS);
  tpdata->thePiK->RegisterMe(tpdata->theQGSPPiK);
  tpdata->theQGSPPiK->SetMinEnergy(minQGSP);   
  tpdata->theFTFPPiK=new G4FTFPPiKBuilder(quasiElasFTF);
  tpdata->thePiK->RegisterMe(tpdata->theFTFPPiK);
  tpdata->theFTFPPiK->SetMinEnergy(minFTFP);
  tpdata->theFTFPPiK->SetMaxEnergy(maxFTFP);
  tpdata->theBertiniPiK=new G4BertiniPiKBuilder;
  tpdata->thePiK->RegisterMe(tpdata->theBertiniPiK);
  tpdata->theBertiniPiK->SetMaxEnergy(maxBERT);
  
  tpdata->theHyperon=new G4HyperonFTFPBuilder;
    
  tpdata->theAntiBaryon=new G4AntiBarionBuilder;
  tpdata->theAntiBaryon->RegisterMe(tpdata->theFTFPAntiBaryon=new G4FTFPAntiBarionBuilder(quasiElasFTF));
}

G4HadronPhysicsQGSP_FTFP_BERT_ATL::~G4HadronPhysicsQGSP_FTFP_BERT_ATL()
{
  if (!tpdata) return;

  delete tpdata->theNeutrons;
  delete tpdata->theBertiniNeutron;
  delete tpdata->theFTFPNeutron;
  delete tpdata->theQGSPNeutron;

  delete tpdata->thePiK;
  delete tpdata->theBertiniPiK;
  delete tpdata->theFTFPPiK;
  delete tpdata->theQGSPPiK;

  delete tpdata->thePro;
  delete tpdata->theBertiniPro;
  delete tpdata->theFTFPPro;    
  delete tpdata->theQGSPPro;
  
  delete tpdata->theHyperon;
  delete tpdata->theAntiBaryon;
  delete tpdata->theFTFPAntiBaryon;
 
  //Note that here we need to set to 0 the pointer
  //since tpdata is static and if thread are "reused"
  //it can be problematic
  delete tpdata; tpdata = 0;
}

void G4HadronPhysicsQGSP_FTFP_BERT_ATL::ConstructParticle()
{
  G4MesonConstructor pMesonConstructor;
  pMesonConstructor.ConstructParticle();

  G4BaryonConstructor pBaryonConstructor;
  pBaryonConstructor.ConstructParticle();

  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();  
}

#include "G4ProcessManager.hh"
void G4HadronPhysicsQGSP_FTFP_BERT_ATL::ConstructProcess()
{
  if ( tpdata == 0 ) tpdata = new ThreadPrivate;
  CreateModels();
  tpdata->theNeutrons->Build();
  tpdata->thePro->Build();
  tpdata->thePiK->Build();

  // --- Kaons ---
  tpdata->ChipsKaonMinus = G4CrossSectionDataSetRegistry::Instance()->GetCrossSectionDataSet(G4ChipsKaonMinusInelasticXS::Default_Name());
  tpdata->ChipsKaonPlus = G4CrossSectionDataSetRegistry::Instance()->GetCrossSectionDataSet(G4ChipsKaonPlusInelasticXS::Default_Name());
  tpdata->ChipsKaonZero = G4CrossSectionDataSetRegistry::Instance()->GetCrossSectionDataSet(G4ChipsKaonZeroInelasticXS::Default_Name());
    
  G4PhysListUtil::FindInelasticProcess(G4KaonMinus::KaonMinus())->AddDataSet(tpdata->ChipsKaonMinus);
  G4PhysListUtil::FindInelasticProcess(G4KaonPlus::KaonPlus())->AddDataSet(tpdata->ChipsKaonPlus);
  G4PhysListUtil::FindInelasticProcess(G4KaonZeroShort::KaonZeroShort())->AddDataSet(tpdata->ChipsKaonZero );
  G4PhysListUtil::FindInelasticProcess(G4KaonZeroLong::KaonZeroLong())->AddDataSet(tpdata->ChipsKaonZero );
    
  tpdata->theHyperon->Build();
  tpdata->theAntiBaryon->Build();

  // --- Neutrons ---
  tpdata->xsNeutronInelasticXS = (G4NeutronInelasticXS*)G4CrossSectionDataSetRegistry::Instance()->GetCrossSectionDataSet(G4NeutronInelasticXS::Default_Name());
  G4PhysListUtil::FindInelasticProcess(G4Neutron::Neutron())->AddDataSet(tpdata->xsNeutronInelasticXS);

  G4HadronicProcess* capture = 0;
  G4ProcessManager* pmanager = G4Neutron::Neutron()->GetProcessManager();
  G4ProcessVector*  pv = pmanager->GetProcessList();
  for ( size_t i=0; i < static_cast<size_t>(pv->size()); ++i ) {
    if ( fCapture == ((*pv)[i])->GetProcessSubType() ) {
      capture = static_cast<G4HadronicProcess*>((*pv)[i]);
    }
  }
  if ( ! capture ) {
    capture = new G4HadronCaptureProcess("nCapture");
    pmanager->AddDiscreteProcess(capture);
  }
  tpdata->xsNeutronCaptureXS = (G4NeutronCaptureXS*)G4CrossSectionDataSetRegistry::Instance()->GetCrossSectionDataSet(G4NeutronCaptureXS::Default_Name());
  capture->AddDataSet(tpdata->xsNeutronCaptureXS);
  capture->RegisterMe(new G4NeutronRadCapture());
}
