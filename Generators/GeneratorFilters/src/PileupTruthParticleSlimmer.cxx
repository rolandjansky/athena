/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/errorcheck.h"

#include "GeneratorObjects/McEventCollection.h"
#include "GeneratorObjects/xAODTruthParticleLink.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

#include "GeneratorFilters/PileupTruthParticleSlimmer.h"


using namespace std;

    
PileupTruthParticleSlimmer::PileupTruthParticleSlimmer( const string& name, ISvcLocator* svcLoc )
: AthAlgorithm( name, svcLoc ), m_classifier("MCTruthClassifier/MCTruthClassifier")
{
    /// @todo TruthParticle -> TruthParticles
    declareProperty("xAODTruthParticleContainerName", m_xaodTruthParticleContainerName="TruthParticles" );
    declareProperty("xAODTruthPileupParticleContainerName", m_xaodTruthPileupParticleContainerName="TruthPileupParticles" );
    declareProperty( "ForceRerun", m_forceRerun = false);
    declareProperty("photon_pt_selection", m_photon_pt_selection=8.*Gaudi::Units::GeV ); //User provides units in MeV!
    declareProperty("lepton_pt_selection", m_lepton_pt_selection=1.*Gaudi::Units::GeV ); //User provides units in MeV!
    declareProperty("abseta_selection", m_abseta_selection=4.5 );
}


StatusCode PileupTruthParticleSlimmer::initialize() {
    ATH_MSG_DEBUG("Initializing");
    ATH_MSG_DEBUG("xAOD input TruthParticleContainer name = " << m_xaodTruthParticleContainerName );
    ATH_MSG_DEBUG("xAOD output TruthPileupParticleContainer name = " << m_xaodTruthPileupParticleContainerName );
    ATH_CHECK(m_classifier.retrieve());
    return StatusCode::SUCCESS;
}


StatusCode PileupTruthParticleSlimmer::execute() {
    
    
    // If the containers already exist then assume that nothing needs to be done
    if ( evtStore()->contains< xAOD::TruthParticleContainer >(m_xaodTruthPileupParticleContainerName) &&
        !m_forceRerun) {
        ATH_MSG_WARNING("xAOD Pileup Truth Particles are already available in the event");
        return StatusCode::SUCCESS;
    }
    

    // Create new output container
    xAOD::TruthParticleContainer* xTruthPileupParticleContainer = new xAOD::TruthParticleContainer();
    CHECK( evtStore()->record( xTruthPileupParticleContainer, m_xaodTruthPileupParticleContainerName ) );
    xAOD::TruthParticleAuxContainer* xTruthPileupParticleAuxContainer = new xAOD::TruthParticleAuxContainer();
    CHECK( evtStore()->record( xTruthPileupParticleAuxContainer, m_xaodTruthPileupParticleContainerName + "Aux." ) );
    xTruthPileupParticleContainer->setStore( xTruthPileupParticleAuxContainer );
    ATH_MSG_DEBUG( "Recorded TruthPileupParticleContainer with key: " << m_xaodTruthPileupParticleContainerName );

    // Retrieve full TruthParticle container
    const xAOD::TruthParticleContainer* xTruthParticleContainer;
    if (evtStore()->retrieve(xTruthParticleContainer, m_xaodTruthParticleContainerName).isFailure()) {
        ATH_MSG_ERROR("No TruthParticle collection with name " << m_xaodTruthParticleContainerName << " found in StoreGate!");
        return StatusCode::FAILURE;
    }

    // Set up decorators
    const static SG::AuxElement::Decorator<unsigned int> originDecorator("classifierParticleOrigin");
    const static SG::AuxElement::Decorator<unsigned int> typeDecorator("classifierParticleType");
    const static SG::AuxElement::Decorator<unsigned int> outcomeDecorator("classifierParticleOutCome");
    const static SG::AuxElement::Decorator<unsigned int> classificationDecorator("Classification");
    const static SG::AuxElement::Decorator<int> parenthadronPIDDecorator("parentHadronID");

    // Loop over full TruthParticle container
    //unsigned int nParticles = xTruthParticleContainer->size();
    //for (unsigned int iPart=0; iPart<nParticles; ++iPart) {
    for (const xAOD::TruthParticle *theParticle : *xTruthParticleContainer) {
     
      //We only want to save stable particles (lifetime > 10mm)
      if (theParticle->status() != 1){
          continue; //Skip this particle
      }

      int this_absPdgID = theParticle->absPdgId();
      float this_abseta = theParticle->abseta();
      float this_pt =  theParticle->pt();

      //Save photons above 8 GeV or electrons/muons above 1 GeV, & within detector acceptance (4.5)
      if( ( (this_absPdgID == 22 && this_pt >= m_photon_pt_selection) || ((this_absPdgID == 11 || this_absPdgID == 13) && this_pt >= m_lepton_pt_selection) )
          && (this_abseta < m_abseta_selection) ){

        xAOD::TruthParticle* xTruthParticle = new xAOD::TruthParticle();
        xTruthPileupParticleContainer->push_back( xTruthParticle );

        // Fill with numerical content
        xTruthParticle->setPdgId(theParticle->pdgId());
        xTruthParticle->setBarcode(theParticle->barcode());
        xTruthParticle->setStatus(theParticle->status());
        xTruthParticle->setM(theParticle->m());
        xTruthParticle->setPx(theParticle->px());
        xTruthParticle->setPy(theParticle->py());
        xTruthParticle->setPz(theParticle->pz());
        xTruthParticle->setE(theParticle->e());

#ifdef MCTRUTHCLASSIFIER_CONST
        IMCTruthClassifier::Info info;
        std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> classification = 
          m_classifier->particleTruthClassifier(theParticle, &info);
        unsigned int particleOutCome = info.particleOutCome;
#else
        std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> classification = 
          m_classifier->particleTruthClassifier(theParticle);
        unsigned int particleOutCome = m_classifier->getParticleOutCome();
#endif

        unsigned int result = (unsigned int)m_classifier->classify(theParticle);

        int hadron_pdg = (int)m_classifier->getParentHadronID(theParticle);

        unsigned int particleType = classification.first;
        unsigned int particleOrigin = classification.second;
        typeDecorator(*xTruthParticle) = particleType;
        originDecorator(*xTruthParticle) = particleOrigin;
        outcomeDecorator(*xTruthParticle) = particleOutCome;  

        classificationDecorator(*xTruthParticle) = result;
        parenthadronPIDDecorator(*xTruthParticle) = hadron_pdg;
      }

    } //end of loop over particles

    return StatusCode::SUCCESS;
}


