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
// $Id: G4HadronPhysicsFTFP_BERT_ATL_rescattering.cc 83699 2014-09-10 07:18:25Z gcosmo $
//
//---------------------------------------------------------------------------
// Author: Alberto Ribon
// Date:   October 2017
//
// Hadron physics for the new physics list FTFP_BERT_ATL_rescattering.
// This is a modified version of the FTFP_BERT_ATL hadron physics for ATLAS,
// in which the rescattering of the final-state produced by FTF is simulated
// with Binary Cascade (similar to FTF_BIC).
//----------------------------------------------------------------------------
//
#include <iomanip>   

#include "G4HadronPhysicsFTFP_BERT_ATL_rescattering.hh"

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
G4_DECLARE_PHYSCONSTR_FACTORY(G4HadronPhysicsFTFP_BERT_ATL_rescattering);

G4ThreadLocal G4HadronPhysicsFTFP_BERT_ATL_rescattering::ThreadPrivate* G4HadronPhysicsFTFP_BERT_ATL_rescattering::tpdata=0;

G4HadronPhysicsFTFP_BERT_ATL_rescattering::G4HadronPhysicsFTFP_BERT_ATL_rescattering(G4int)
    :  G4VPhysicsConstructor("hInelastic FTFP_BERT_ATL_rescattering")
/*    , theNeutrons(0)
    , theBertiniNeutron(0)
    , theFTFBinaryNeutron(0)
    , thePion(0)
    , theBertiniPion(0)
    , theFTFBinaryPion(0)
    , theKaon(0)
    , theBertiniKaon(0)
    , theFTFBinaryKaon(0)
    , thePro(0)
    , theBertiniPro(0)
    , theFTFBinaryPro(0)
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

G4HadronPhysicsFTFP_BERT_ATL_rescattering::G4HadronPhysicsFTFP_BERT_ATL_rescattering(const G4String& name, G4bool quasiElastic)
    :  G4VPhysicsConstructor(name) 
/*    , theNeutrons(0)
    , theBertiniNeutron(0)
    , theFTFBinaryNeutron(0)
    , thePion(0)
    , theBertiniPion(0)
    , theFTFBinaryPion(0)
    , theKaon(0)
    , theBertiniKaon(0)
    , theFTFBinaryKaon(0)
    , thePro(0)
    , theBertiniPro(0)
    , theFTFBinaryPro(0)
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

void G4HadronPhysicsFTFP_BERT_ATL_rescattering::CreateModels()
{

  G4double minFTFP =  9.0 * GeV;
  G4double maxBERT = 12.0 * GeV;
  G4cout << " FTFP_BERT_ATL_rescattering : same transitions as FTFP_BERT_ATL," << G4endl
         << " but using Binary Cascade (BIC) for rescattering of final-state produced by FTF"
         << G4endl;

  tpdata->theNeutrons=new G4NeutronBuilder;
  tpdata->theFTFBinaryNeutron=new G4FTFBinaryNeutronBuilder(QuasiElastic);
  tpdata->theNeutrons->RegisterMe(tpdata->theFTFBinaryNeutron);
  tpdata->theFTFBinaryNeutron->SetMinEnergy(minFTFP);
  tpdata->theNeutrons->RegisterMe(tpdata->theBertiniNeutron=new G4BertiniNeutronBuilder);
  tpdata->theBertiniNeutron->SetMinEnergy(0.0*GeV);
  tpdata->theBertiniNeutron->SetMaxEnergy(maxBERT);

  tpdata->thePro=new G4ProtonBuilder;
  tpdata->theFTFBinaryPro=new G4FTFBinaryProtonBuilder(QuasiElastic);
  tpdata->thePro->RegisterMe(tpdata->theFTFBinaryPro);
  tpdata->theFTFBinaryPro->SetMinEnergy(minFTFP);
  tpdata->thePro->RegisterMe(tpdata->theBertiniPro=new G4BertiniProtonBuilder);
  tpdata->theBertiniPro->SetMaxEnergy(maxBERT);

  tpdata->thePion=new G4PionBuilder;
  tpdata->thePion->RegisterMe(tpdata->theFTFBinaryPion=new G4FTFBinaryPionBuilder(QuasiElastic));
  tpdata->theFTFBinaryPion->SetMinEnergy(minFTFP);
  tpdata->thePion->RegisterMe(tpdata->theBertiniPion=new G4BertiniPionBuilder);
  tpdata->theBertiniPion->SetMaxEnergy(maxBERT);

  tpdata->theKaon=new G4KaonBuilder;
  tpdata->theKaon->RegisterMe(tpdata->theFTFBinaryKaon=new G4FTFBinaryKaonBuilder(QuasiElastic));
  tpdata->theFTFBinaryKaon->SetMinEnergy(minFTFP);
  tpdata->theKaon->RegisterMe(tpdata->theBertiniKaon=new G4BertiniKaonBuilder);
  tpdata->theBertiniKaon->SetMaxEnergy(maxBERT);

  tpdata->theHyperon=new G4HyperonFTFPBuilder;
    
  tpdata->theAntiBaryon=new G4AntiBarionBuilder;
  tpdata->theAntiBaryon->RegisterMe(tpdata->theFTFPAntiBaryon=new  G4FTFPAntiBarionBuilder(QuasiElastic));
}

G4HadronPhysicsFTFP_BERT_ATL_rescattering::~G4HadronPhysicsFTFP_BERT_ATL_rescattering()
{
  if (!tpdata) return;

  delete tpdata->theNeutrons;
  delete tpdata->theBertiniNeutron;
  delete tpdata->theFTFBinaryNeutron;

  delete tpdata->thePion;
  delete tpdata->theBertiniPion;
  delete tpdata->theFTFBinaryPion;
  delete tpdata->theKaon;
  delete tpdata->theBertiniKaon;
  delete tpdata->theFTFBinaryKaon;
    
  delete tpdata->thePro;
  delete tpdata->theBertiniPro;
  delete tpdata->theFTFBinaryPro;    
    
  delete tpdata->theHyperon;
  delete tpdata->theAntiBaryon;
  delete tpdata->theFTFPAntiBaryon;
 
  //Note that here we need to set to 0 the pointer
  //since tpdata is static and if thread are "reused"
  //it can be problematic
  delete tpdata; tpdata = 0;
}

void G4HadronPhysicsFTFP_BERT_ATL_rescattering::ConstructParticle()
{
  G4MesonConstructor pMesonConstructor;
  pMesonConstructor.ConstructParticle();

  G4BaryonConstructor pBaryonConstructor;
  pBaryonConstructor.ConstructParticle();

  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();  
}

#include "G4ProcessManager.hh"
void G4HadronPhysicsFTFP_BERT_ATL_rescattering::ConstructProcess()
{
  if ( tpdata == 0 ) tpdata = new ThreadPrivate;
  CreateModels();
  tpdata->theNeutrons->Build();
  tpdata->thePro->Build();
  tpdata->thePion->Build();
  tpdata->theKaon->Build();

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
