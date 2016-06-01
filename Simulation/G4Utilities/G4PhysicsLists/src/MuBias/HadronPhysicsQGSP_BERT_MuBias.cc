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
// $Id: G4HadronPhysicsQGSP_BERT_MuBias.cc 76703 2013-11-14 10:29:11Z gcosmo $
//
//---------------------------------------------------------------------------
//
// ClassName:   G4HadronPhysicsQGSP_BERT
//
// Author: 2002 J.P. Wellisch
//
// Modified:
// 23.11.2005 G.Folger: migration to non static particles
// 08.06.2006 V.Ivanchenko: remove stopping
// 20.06.2006 G.Folger: Bertini applies to Kaons, i.e. use SetMinEnergy instead of SetMinPionEnergy
// 25.04.2007 G.Folger: Add code for quasielastic
// 10.12.2007 G.Folger: Add projectilediffrative option for proton/neutron, off by default
// 31.10.2012 A.Ribon: Use G4MiscBuilder
// 19.03.2013 A.Ribon: Replace LEP with FTFP
//
//----------------------------------------------------------------------------
//
#include <iomanip>   

#include "HadronPhysicsQGSP_BERT_MuBias.hh"

#include "globals.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4ShortLivedConstructor.hh"
#include "G4IonConstructor.hh"

#include "G4HadronCaptureProcess.hh"
#include "G4NeutronRadCapture.hh"
#include "G4NeutronInelasticXS.hh"
#include "G4NeutronCaptureXS.hh"

#include "G4PhysListUtil.hh"

// factory
#include "G4PhysicsConstructorFactory.hh"

#include "G4Version.hh"
#if G4VERSION_NUMBER>=1000
#  include "G4UrbanMscModel.hh"
#else
#  include "G4UrbanMscModel96.hh"
    using G4UrbanMscModel=G4UrbanMscModel96;
#endif

//
G4_DECLARE_PHYSCONSTR_FACTORY(G4HadronPhysicsQGSP_BERT_MuBias);

G4ThreadLocal G4HadronPhysicsQGSP_BERT_MuBias::ThreadPrivate* 
G4HadronPhysicsQGSP_BERT_MuBias::s_tpdata = 0;


G4HadronPhysicsQGSP_BERT_MuBias::G4HadronPhysicsQGSP_BERT_MuBias(G4int)
    :  G4VPhysicsConstructor("hInelastic QGSP_BERT_MuBias")
/*    , theNeutrons(0)
    , theFTFPNeutron(0)
    , theQGSPNeutron(0)
    , theBertiniNeutron(0)
    , thePiK(0)
    , theFTFPPiK(0)
    , theQGSPPiK(0)
    , theBertiniPiK(0)
    , thePro(0)
    , theFTFPPro(0)
    , theQGSPPro(0) 
    , theBertiniPro(0)
    , theHyperon(0)
    , theAntiBaryon(0)
    , theFTFPAntiBaryon(0)
    , xsNeutronInelasticXS(0)
    , xsNeutronCaptureXS(0)*/
    , m_QuasiElastic(true)
{
}

G4HadronPhysicsQGSP_BERT_MuBias::G4HadronPhysicsQGSP_BERT_MuBias(const G4String& name, G4bool quasiElastic)
    :  G4VPhysicsConstructor(name)
/*    , theNeutrons(0)
    , theFTFPNeutron(0)
    , theQGSPNeutron(0)
    , theBertiniNeutron(0)
    , thePiK(0)
    , theFTFPPiK(0)
    , theQGSPPiK(0)
    , theBertiniPiK(0)
    , thePro(0)
    , theFTFPPro(0)
    , theQGSPPro(0) 
    , theBertiniPro(0)
    , theHyperon(0)
    , theAntiBaryon(0)
    , theFTFPAntiBaryon(0)
    , xsNeutronInelasticXS(0)
    , xsNeutronCaptureXS(0)*/
    , m_QuasiElastic(quasiElastic)
{
}

void G4HadronPhysicsQGSP_BERT_MuBias::CreateModels()
{
  G4bool quasiElasticFTF= false;   // Use built-in quasi-elastic (not add-on)
  G4bool quasiElasticQGS= true;    // For QGS, it must use it.

  const G4double minQGSP = 12.0*GeV;
  const G4double maxFTFP = 25.0*GeV;
  const G4double minFTFP =  9.5*GeV;
  const G4double maxBERT =  9.9*GeV;
  const G4double minBERT =  0.0*GeV;

  s_tpdata->theNeutrons=new G4NeutronBuilder;
  s_tpdata->theNeutrons->RegisterMe(s_tpdata->theQGSPNeutron=new G4QGSPNeutronBuilder(quasiElasticQGS));
  s_tpdata->theQGSPNeutron->SetMinEnergy(minQGSP);   
  s_tpdata->theNeutrons->RegisterMe(s_tpdata->theFTFPNeutron=new G4FTFPNeutronBuilder(quasiElasticFTF));
  s_tpdata->theFTFPNeutron->SetMinEnergy(minFTFP);
  s_tpdata->theFTFPNeutron->SetMaxEnergy(maxFTFP);  

  s_tpdata->theNeutrons->RegisterMe(s_tpdata->theBertiniNeutron=new G4BertiniNeutronBuilder);
  s_tpdata->theBertiniNeutron->SetMinEnergy(minBERT);
  s_tpdata->theBertiniNeutron->SetMaxEnergy(maxBERT);

  s_tpdata->thePro=new G4ProtonBuilder;
  s_tpdata->thePro->RegisterMe(s_tpdata->theQGSPPro=new G4QGSPProtonBuilder(quasiElasticQGS));
  s_tpdata->theQGSPPro->SetMinEnergy(minQGSP);   
  s_tpdata->thePro->RegisterMe(s_tpdata->theFTFPPro=new G4FTFPProtonBuilder(quasiElasticFTF));
  s_tpdata->theFTFPPro->SetMinEnergy(minFTFP);
  s_tpdata->theFTFPPro->SetMaxEnergy(maxFTFP);
  s_tpdata->thePro->RegisterMe(s_tpdata->theBertiniPro=new G4BertiniProtonBuilder);
  s_tpdata->theBertiniPro->SetMaxEnergy(maxBERT);

  s_tpdata->thePiK=new G4PiKBuilder;
  s_tpdata->thePiK->RegisterMe(s_tpdata->theQGSPPiK=new G4QGSPPiKBuilder(quasiElasticQGS));
  s_tpdata->theQGSPPiK->SetMinEnergy(minQGSP);
  s_tpdata->thePiK->RegisterMe(s_tpdata->theFTFPPiK=new G4FTFPPiKBuilder(quasiElasticFTF));
  s_tpdata->theFTFPPiK->SetMinEnergy(minFTFP);
  s_tpdata->theFTFPPiK->SetMaxEnergy(maxFTFP);
  s_tpdata->thePiK->RegisterMe(s_tpdata->theBertiniPiK=new G4BertiniPiKBuilder);
  s_tpdata->theBertiniPiK->SetMaxEnergy(maxBERT);

  s_tpdata->theHyperon=new G4HyperonFTFPBuilder;

  s_tpdata->theAntiBaryon=new G4AntiBarionBuilder;
  s_tpdata->theAntiBaryon->RegisterMe(s_tpdata->theFTFPAntiBaryon=new G4FTFPAntiBarionBuilder(quasiElasticFTF));
}

G4HadronPhysicsQGSP_BERT_MuBias::~G4HadronPhysicsQGSP_BERT_MuBias()
{
   delete s_tpdata->theBertiniNeutron;
   delete s_tpdata->theQGSPNeutron;
   delete s_tpdata->theFTFPNeutron;
   delete s_tpdata->theNeutrons;
   delete s_tpdata->theBertiniPiK;
   delete s_tpdata->theQGSPPiK;
   delete s_tpdata->theFTFPPiK;
   delete s_tpdata->thePiK;
   delete s_tpdata->theBertiniPro;
   delete s_tpdata->theQGSPPro;
   delete s_tpdata->theFTFPPro;
   delete s_tpdata->thePro;
   delete s_tpdata->theFTFPAntiBaryon;
   delete s_tpdata->theAntiBaryon;
   delete s_tpdata->theHyperon;
   delete s_tpdata->xsNeutronInelasticXS;
   delete s_tpdata->xsNeutronCaptureXS;
  
   delete s_tpdata; s_tpdata = 0;
}

void G4HadronPhysicsQGSP_BERT_MuBias::ConstructParticle()
{
  G4MesonConstructor pMesonConstructor;
  pMesonConstructor.ConstructParticle();

  G4BaryonConstructor pBaryonConstructor;
  pBaryonConstructor.ConstructParticle();

  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();  

  G4IonConstructor pIonConstructor;
  pIonConstructor.ConstructParticle();
}

#include "G4ProcessManager.hh"
void G4HadronPhysicsQGSP_BERT_MuBias::ConstructProcess()
{
  if ( s_tpdata == 0 ) s_tpdata = new ThreadPrivate;
  CreateModels();
  s_tpdata->theNeutrons->Build();
  s_tpdata->thePro->Build();
  s_tpdata->thePiK->Build();
  s_tpdata->theHyperon->Build(); 
  s_tpdata->theAntiBaryon->Build();

  // --- Neutrons ---
  s_tpdata->xsNeutronInelasticXS = new G4NeutronInelasticXS();  
  G4PhysListUtil::FindInelasticProcess(G4Neutron::Neutron())->AddDataSet(s_tpdata->xsNeutronInelasticXS);

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
  s_tpdata->xsNeutronCaptureXS = new G4NeutronCaptureXS();
  capture->AddDataSet(s_tpdata->xsNeutronCaptureXS);
  capture->RegisterMe(new G4NeutronRadCapture());
}

