/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthCollectionMaker.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// Removes all truth particles/vertices which do not pass a user-defined cut

#include "DerivationFrameworkMCTruth/TruthCollectionMaker.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
//#include "xAODTruth/TruthVertexContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::TruthCollectionMaker::TruthCollectionMaker(const std::string& t,
                                                                  const std::string& n,
                                                                  const IInterface* p ) :
AthAlgTool(t,n,p),
//m_ntotvtx(0),
m_ntotpart(0),
//m_npassvtx(0),
m_npasspart(0),
m_particlesKey("TruthParticles"),
//m_verticesKey("TruthVertices"),
m_collectionName(""),
m_partString(""),
m_classifier("MCTruthClassifier/MCTruthClassifier"),
m_runClassifier(true)
{
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("ParticlesKey", m_particlesKey);
    //declareProperty("VerticesKey", m_verticesKey);
    declareProperty("NewCollectionName", m_collectionName);
    declareProperty("ParticleSelectionString", m_partString); 
    declareProperty("MCTruthClassifier", m_classifier);
    declareProperty("RunClassifier", m_runClassifier);
}

// Destructor
DerivationFramework::TruthCollectionMaker::~TruthCollectionMaker() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::TruthCollectionMaker::initialize()
{
    ATH_MSG_VERBOSE("initialize() ...");
    if (m_runClassifier) ATH_CHECK(m_classifier.retrieve());

    if (m_particlesKey=="" /*|| m_verticesKey==""*/) {
        ATH_MSG_FATAL("No truth particle collection provided to use as a basis for new collections");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_particlesKey << " as the source collections for new truth collections");}
    
    if (m_collectionName=="") {
        ATH_MSG_FATAL("No key provided for the new truth particle collection");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("New truth particle collection key: " << m_collectionName );}

    if (m_partString=="") {
        ATH_MSG_FATAL("No selection string provided");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Truth particle selection string: " << m_partString );}
    
    // Set up the text-parsing machinery for thinning the truth directly according to user cuts
    if ( m_partString!="") {
	    ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
	    proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
	    proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
	    if (m_partString!="") {
		m_partParser = new ExpressionParsing::ExpressionParser(proxyLoaders);
	    	m_partParser->loadExpression(m_partString);
	    }	
    }
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::TruthCollectionMaker::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    //ATH_MSG_INFO("Processed "<< m_ntotvtx <<" truth vertices, "<< m_npassvtx << " were retained ");
    ATH_MSG_INFO("Processed "<< m_ntotpart <<" truth particles, "<< m_npasspart << " were retained ");
    if (m_partString!="") {
        delete m_partParser;
        m_partParser = 0;
    }
    return StatusCode::SUCCESS;
}

// Selection and collection creation
StatusCode DerivationFramework::TruthCollectionMaker::addBranches() const
{
    
    // Retrieve truth collections
    const xAOD::TruthParticleContainer* importedTruthParticles;
    if (evtStore()->retrieve(importedTruthParticles,m_particlesKey).isFailure()) {
        ATH_MSG_ERROR("No TruthParticle collection with name " << m_particlesKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    /*const xAOD::TruthVertexContainer* importedTruthVertices;
    if (evtStore()->retrieve(importedTruthVertices,m_verticesKey).isFailure()) {
        ATH_MSG_ERROR("No TruthVertex collection with name " << m_verticesKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }*/
    
    // Create the new containers
    xAOD::TruthParticleContainer* newParticleCollection = new xAOD::TruthParticleContainer();
    CHECK( evtStore()->record( newParticleCollection, m_collectionName ) );
    xAOD::TruthParticleAuxContainer* newParticleAuxCollection = new xAOD::TruthParticleAuxContainer();
    CHECK( evtStore()->record( newParticleAuxCollection, m_collectionName + "Aux." ) );
    newParticleCollection->setStore( newParticleAuxCollection );
    ATH_MSG_DEBUG( "Recorded new TruthParticleContainer with key: " <<  m_collectionName);

    // Set up a mask with the same entries as the full collections
    unsigned int nParticles = importedTruthParticles->size();
    //unsigned int nVertices = importedTruthVertices->size();
    m_ntotpart += nParticles;
    
    // Set up decorators
    SG::AuxElement::Decorator< ElementLink<xAOD::TruthParticleContainer> > linkDecorator("originalTruthParticle");
    SG::AuxElement::Decorator< unsigned int > originDecorator("particleOrigin");
    SG::AuxElement::Decorator< unsigned int > typeDecorator("particleType");

    // Execute the text parsers and update the mask
    if (m_partString!="") {
    	std::vector<int> entries =  m_partParser->evaluateAsVector();
    	unsigned int nEntries = entries.size();
    	// check the sizes are compatible
    	if (nParticles != nEntries ) {
    	    ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used TruthParticles?");
            return StatusCode::FAILURE;
    	} else {
            // add relevant particles to new collection
            for (unsigned int i=0; i<nParticles; ++i) {
                ElementLink<xAOD::TruthParticleContainer> eltp(*importedTruthParticles,i); 
                if (entries[i]==1) {
                     xAOD::TruthParticle* xTruthParticle = new xAOD::TruthParticle();
                     newParticleCollection->push_back( xTruthParticle );
                     // Run classification
                     if (m_runClassifier) {
                          std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> classification = 
                          m_classifier->particleTruthClassifier((*importedTruthParticles)[i]);			
                          unsigned int particleType = classification.first;
                          unsigned int particleOrigin = classification.second;
                          typeDecorator(*xTruthParticle) = particleType;
                          originDecorator(*xTruthParticle) = particleOrigin;
                     } 
                     // Fill with numerical content
                     xAOD::TruthParticle* theParticle = const_cast<xAOD::TruthParticle*>( (*importedTruthParticles)[i] );
                     xTruthParticle->setPdgId(theParticle->pdgId());
                     xTruthParticle->setBarcode(theParticle->barcode());
                     xTruthParticle->setStatus(theParticle->status());
                     xTruthParticle->setM(theParticle->m());
                     xTruthParticle->setPx(theParticle->px());
                     xTruthParticle->setPy(theParticle->py());
                     xTruthParticle->setPz(theParticle->pz());
                     xTruthParticle->setE(theParticle->e());
                     linkDecorator(*xTruthParticle) = eltp;
                }     
            }
    	}
        // Count the mask
        for (unsigned int i=0; i<nParticles; ++i) if (entries[i]) ++m_npasspart;
    }

    return StatusCode::SUCCESS;
}

