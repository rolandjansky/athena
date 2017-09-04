/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// Truth3CollectionMaker.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch) with modifications by Benjamin Nachman (bnachman@cern.ch)
// Removes all truth particles/vertices which do not pass a user-defined cut

#include "DerivationFrameworkSUSY/Truth3CollectionMaker.h"
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
DerivationFramework::Truth3CollectionMaker::Truth3CollectionMaker(const std::string& t,
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
DerivationFramework::Truth3CollectionMaker::~Truth3CollectionMaker() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::Truth3CollectionMaker::initialize()
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

StatusCode DerivationFramework::Truth3CollectionMaker::finalize()
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
StatusCode DerivationFramework::Truth3CollectionMaker::addBranches() const
{
    
    // Retrieve truth collections
    const xAOD::TruthParticleContainer* importedTruthParticles = 0;
    if (evtStore()->retrieve(importedTruthParticles,m_particlesKey).isFailure()) {
        ATH_MSG_ERROR("No TruthParticle collection with name " << m_particlesKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }

    // Create the new containers
    xAOD::TruthParticleContainer* newParticleCollection = new xAOD::TruthParticleContainer();
    CHECK( evtStore()->record( newParticleCollection, m_collectionName ) );
    xAOD::TruthParticleAuxContainer* newParticleAuxCollection = new xAOD::TruthParticleAuxContainer();
    CHECK( evtStore()->record( newParticleAuxCollection, m_collectionName + "Aux." ) );
    newParticleCollection->setStore( newParticleAuxCollection );
    ATH_MSG_DEBUG( "Recorded new TruthParticleContainer with key: " <<  m_collectionName);

    // Set up a mask with the same entries as the full collections
    unsigned int nParticles = importedTruthParticles->size();
    m_ntotpart += nParticles;
    
    // Set up decorators
    SG::AuxElement::Decorator< ElementLink<xAOD::TruthParticleContainer> > linkDecorator("originalTruthParticle");
    SG::AuxElement::Decorator< int > originDecorator("particleMotherPdgId");
    SG::AuxElement::Decorator< int > typeDecorator("particleOriginBarcode");
    SG::AuxElement::Decorator< float > typeDecoratorMass("particleOriginMass");
    SG::AuxElement::Decorator< int > tauprongDecorator("nprong");
    SG::AuxElement::Decorator< int > tautypeDecorator("islep");

    std::vector<int> entries;

    // Execute the text parsers and update the mask
    if (m_partString!="") {
    	entries =  m_partParser->evaluateAsVector();
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
	      
	      xAOD::TruthParticle* theParticle = const_cast<xAOD::TruthParticle*>( (*importedTruthParticles)[i] );
	      
	      //SUSYTRUTH definitions are based on Appendix A in http://arxiv.org/pdf/1403.4853v1.pdf.

	      //SUSYTRUTH Leptons
	      if (theParticle->isLepton()){
		bool drop = false;
		if (!theParticle->hasProdVtx()) drop = true;
		else{
		  int parentid = abs(theParticle->prodVtx()->incomingParticle(0)->pdgId());
		  double parentmass = theParticle->prodVtx()->incomingParticle(0)->p4().M()/1000.;
		  if (parentid==24 && parentmass < 20){ //semi-leptonic b-decays in Herwig++ where the off-shell W is saved
		    drop = true;
		  } 
		  else if (parentid!=24 && parentid!=15){ //what about W decays in Sherpa?
		    drop = true;
		  }
		  else if (parentid==15){//check to make sure the tau came from a W
		    if (theParticle->prodVtx()->incomingParticle(0)->hasProdVtx()){
		      const xAOD::TruthParticle * mother_hold = theParticle->prodVtx()->incomingParticle(0)->prodVtx()->incomingParticle(0);
		      int mcount = 0;
		      while (mother_hold->hasProdVtx() && abs(mother_hold->pdgId())==15){
			mcount++;
			if (mcount > 10){ //should not ever come in here, but just in case there is a closed loop somewhere.
			  break; 
			}
			mother_hold = mother_hold->prodVtx()->incomingParticle(0);
		      }
		      if (mcount > 10 || abs(mother_hold->pdgId())!=24){
			drop = true;
		      }
		      else{
			//keep
		      }
		    }
		    else{
		      drop = true;
		    }
		  }
		}
		if (drop){
		  entries[i]=0;
		  continue;
		}
	      }

	      xAOD::TruthParticle* xTruthParticle = new xAOD::TruthParticle();
	      newParticleCollection->push_back( xTruthParticle );
	      
	      int motherbarcode = -1;
	      int motherPDGid = 0;
	      float mothermass = 0.;
	      if (theParticle->hasProdVtx()){
		int parentid = abs(theParticle->prodVtx()->incomingParticle(0)->pdgId());
		xTruthParticle->setBarcode(parentid);
		motherbarcode = theParticle->prodVtx()->incomingParticle(0)->barcode();
		const xAOD::TruthParticle * mother_hold = theParticle->prodVtx()->incomingParticle(0);
		motherPDGid = mother_hold->pdgId();
		mothermass = mother_hold->p4().M()/1000.;
		int mcount = 0;
		//Let's find the first mother of mothers that has a different PDGid
		while (mother_hold->hasProdVtx() && mother_hold->pdgId()==theParticle->pdgId()){
		  mcount++;
		  if (mcount > 10){
		    break; //should not come in here, but just in case we have a closed loop from a bug
		  }
		  mother_hold = mother_hold->prodVtx()->incomingParticle(0);
		  motherbarcode = mother_hold->barcode();
		  motherPDGid = mother_hold->pdgId();
		  mothermass = mother_hold->p4().M()/1000.;
		}
	      }

	      xTruthParticle->setBarcode(motherbarcode);
              xTruthParticle->setPdgId(theParticle->pdgId());
              xTruthParticle->setStatus(theParticle->status());
              xTruthParticle->setM(theParticle->m());
              xTruthParticle->setPx(theParticle->px());
              xTruthParticle->setPy(theParticle->py());
              xTruthParticle->setPz(theParticle->pz());
              xTruthParticle->setE(theParticle->e());
              originDecorator(*xTruthParticle) = motherPDGid;
	      typeDecorator(*xTruthParticle) = motherbarcode;
	      typeDecoratorMass(*xTruthParticle) = mothermass;
	      
	      //Check for tau decays
	      if (abs(theParticle->pdgId()) == 15 && theParticle->hasDecayVtx()){
		int nprong = 0;
		int islep = 0;

		//Algorithm:
		//Make a vector of all children of the tau.  While one of the children has a child:
		//Go down one level and store all of the children of the children.  If no children have a child, stop.
		
		/*
		std::vector< std::vector<const xAOD::TruthParticle *> > tau_family;
		bool has_a_child = 0;
		std::vector<const xAOD::TruthParticle *> tau_daughts_first;
		for (unsigned int j=0; j<theParticle->nChildren(); j++){
		  const xAOD::TruthParticle * daught_hold = theParticle->decayVtx()->outgoingParticle(j);
		  tau_daughts_first.push_back(daught_hold);
		  if (daught_hold->hasDecayVtx()){
		    has_a_child = 1;
		  }
		} 
		tau_family.push_back(tau_daughts_first);
		int level = -1;
		while (has_a_child){
		  level++;
		  has_a_child = 0;
		  std::vector<const xAOD::TruthParticle *> tau_daughts;
		  for (unsigned int j=0; j<tau_family[level].size(); j++){
		    for (unsigned int k=0; k<tau_family[level][j]->nChildren(); k++){
		      const xAOD::TruthParticle * daught_hold = tau_family[level][j]->decayVtx()->outgoingParticle(k);
		      tau_daughts.push_back(daught_hold);
		      if (daught_hold->hasDecayVtx()){
			has_a_child = 1;
		      }
		    }
		  }
		  tau_family.push_back(tau_daughts);
		}

		for (unsigned int i=0; i<tau_family.size(); i++){
		  for (unsigned int j=0; j<tau_family[i].size(); j++){
		    if (tau_family[i][j]->isChLepton()){
		      islep=1;
		    }
		    if (tau_family[i][j]->hasDecayVtx()){
		      continue;
		    }
		    if (tau_family[i][j]->isCharged()){
		      nprong++;
		    }
		  }
		}
		*/
		
		//std::cout << "nprong " << nprong << " " << islep << std::endl;
		tauprongDecorator(*xTruthParticle) = nprong;
		tautypeDecorator(*xTruthParticle) = islep;
	      }
	    }
	  }
	}
    }
    // Count the mask
    for (unsigned int i=0; i<nParticles; ++i){
      if (entries[i]) ++m_npasspart;
    }
    
    return StatusCode::SUCCESS;
}

