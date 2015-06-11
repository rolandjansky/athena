/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkEGamma/BkgElectronClassification.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"
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

    std::pair<MCTruthPartClassifier::ParticleType,
      MCTruthPartClassifier::ParticleOrigin> res;

    for (auto el: *electrons){
      static SG::AuxElement::Accessor<int> tT("truthType") ;
      static SG::AuxElement::Accessor<int> tO("truthOrigin") ;
      static SG::AuxElement::Accessor<ElementLink<xAOD::TruthParticleContainer> >  tPL ("truthParticleLink");

      static SG::AuxElement::Decorator<int> bkgtT("bkgTruthType") ;
      static SG::AuxElement::Decorator<int> bkgtO("bkgTruthOrigin") ;
      static SG::AuxElement::Decorator<ElementLink<xAOD::TruthParticleContainer> >  bkgtPL ("bkgTruthParticleLink");
      static SG::AuxElement::Decorator<int> bkgMotherPdgID("bkgMotherPdgId") ;
      bkgtT(*el)= 0;
      bkgtO(*el)= 0;

      ElementLink<xAOD::TruthParticleContainer> link;
      bkgtPL(*el)=link;
      bkgMotherPdgID(*el)=0;

      if(tT.isAvailable(*el)   && tT(*el)==MCTruthPartClassifier::BkgElectron 
	 && tO.isAvailable(*el) && tO(*el)==MCTruthPartClassifier::PhotonConv
	 && tPL.isAvailable(*el)  && tPL(*el).isValid()){      

	res = m_mcTruthClassifier->checkOrigOfBkgElec(*tPL(*el));
	bkgtT(*el)= res.first;
	bkgtO(*el)= res.second;
	const xAOD::TruthParticle* truthParticle= m_mcTruthClassifier->getBkgElecMother();
	if(truthParticle){
	  ElementLink<xAOD::TruthParticleContainer> link(truthParticle, *truthContainer);
	  bkgtPL(*el)=link;
	  bkgMotherPdgID(*el)=truthParticle->pdgId();  
	}
      }
    }
    return StatusCode::SUCCESS;
  }
}
