/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkEGamma/BkgElectronClassification.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/EgammaTruthxAODHelpers.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"

namespace DerivationFramework {

  BkgElectronClassification::BkgElectronClassification(const std::string& t,const std::string& n, const IInterface* p) : 
    AthAlgTool(t,n,p){

    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("ElectronContainerName", m_electronContainerName="Electrons",
		    "Name of the input electron container");

    declareProperty("TruthParticleContainerName", m_truthParticleContainerName ="TruthParticles",
		    "Name of the truth particle container");

    declareProperty("MCTruthClassifierTool", m_mcTruthClassifier,
		    "Handle of MCTruthClassifier");

    declareProperty("barcodeCut", m_barcodecut=200e3,
		    "Cut on the barcode for the xAODEgammaTruthHelpers");

  }

  StatusCode BkgElectronClassification::initialize(){

    ATH_MSG_DEBUG("Initializing " << name() << "...");
    
    CHECK( m_mcTruthClassifier.retrieve() );
    ATH_MSG_DEBUG("Retrieved tool " << m_mcTruthClassifier);
    ATH_MSG_DEBUG("Initialization successful");
    
    return StatusCode::SUCCESS;
  }

  StatusCode BkgElectronClassification::finalize(){
    return StatusCode::SUCCESS;
  }

  StatusCode BkgElectronClassification::addBranches() const {

    const xAOD::ElectronContainer* electrons(0);
    CHECK ( evtStore()->retrieve(electrons,m_electronContainerName) );    
    
    const xAOD::TruthParticleContainer* truthContainer(0);
    CHECK ( evtStore()->retrieve(truthContainer, m_truthParticleContainerName) );


    for (auto el: *electrons){

      //Access for the already existing info
      static SG::AuxElement::Accessor<int> tT("truthType") ;
      static SG::AuxElement::Accessor<int> tO("truthOrigin") ;
      static SG::AuxElement::Accessor<ElementLink<xAOD::TruthParticleContainer> >  tPL ("truthParticleLink");
      //Add this in case people have completely broken all links
      static SG::AuxElement::Decorator<int> tPdgID("truthPdgId") ;
      tPdgID(*el)=0;
      if(tPL.isAvailable(*el) && tPL(*el).isValid() ){
	tPdgID(*el)=(*tPL(*el))->pdgId();  
      }

      //Use the Helpers for electron from electron or photon
      //Add Extra Decoration from Egamma helpers in case of BkgElectron (Electron coming for a photon)
      //Go back to the first/last electron/photon Generator mother and classify this one
      
      //First the one entering the Geant, the first we meet on the way back
      static SG::AuxElement::Decorator<int> firstEgMotherTT("firstEgMotherTruthType") ;
      static SG::AuxElement::Decorator<int> firstEgMotherTO("firstEgMotherTruthOrigin") ;
      static SG::AuxElement::Decorator<ElementLink<xAOD::TruthParticleContainer> >  firstEgMotherTPL ("firstEgMotherTruthParticleLink");
      static SG::AuxElement::Decorator<int> firstEgMotherPdgID("firstEgMotherPdgId") ;
      firstEgMotherTT(*el)= 0;
      firstEgMotherTO(*el)= 0;
      firstEgMotherTPL(*el)=ElementLink<xAOD::TruthParticleContainer>();
      firstEgMotherPdgID(*el)=0;
      const xAOD::TruthParticle* firstElTruth = xAOD::EgammaHelpers::getBkgElectronMother(el,m_barcodecut);
      if( firstElTruth ){
	auto res = m_mcTruthClassifier->particleTruthClassifier(firstElTruth);
	firstEgMotherTT(*el)= res.first;
	firstEgMotherTO(*el)= res.second;
	firstEgMotherPdgID(*el)=firstElTruth->pdgId();
	ElementLink<xAOD::TruthParticleContainer> link( firstElTruth , *truthContainer);
	firstEgMotherTPL(*el)=link;
      }

      //The last electron / photon  we meet on the way back towards the Generator vertex 
      static SG::AuxElement::Decorator<int> lastEgMotherTT("lastEgMotherTruthType") ;
      static SG::AuxElement::Decorator<int> lastEgMotherTO("lastEgMotherTruthOrigin") ;
      static SG::AuxElement::Decorator<ElementLink<xAOD::TruthParticleContainer> >  lastEgMotherTPL ("lastEgMotherTruthParticleLink");
      static SG::AuxElement::Decorator<int> lastEgMotherPdgID("lastEgMotherPdgId") ;
      lastEgMotherTT(*el)= 0;
      lastEgMotherTO(*el)= 0;
      lastEgMotherTPL(*el)=ElementLink<xAOD::TruthParticleContainer>();;
      lastEgMotherPdgID(*el)=0;
      const xAOD::TruthParticle* lastElTruth = xAOD::EgammaHelpers::getBkgElectronMother(el);
      if( lastElTruth ){
	auto res = m_mcTruthClassifier->particleTruthClassifier(lastElTruth);
	lastEgMotherTT(*el)= res.first;
	lastEgMotherTO(*el)= res.second;
	lastEgMotherPdgID(*el)=lastElTruth->pdgId();
	ElementLink<xAOD::TruthParticleContainer> link( lastElTruth , *truthContainer);
	lastEgMotherTPL(*el)=link;
      }

    }
    return StatusCode::SUCCESS;
  }
}
