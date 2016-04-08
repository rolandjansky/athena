/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// GenericObjectThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// Removes all ID tracks which do not pass a user-defined cut

#include "DerivationFrameworkTools/GenericObjectThinning.h"
#include "AthenaKernel/IThinningSvc.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::GenericObjectThinning::GenericObjectThinning(const std::string& t,
                                                                  const std::string& n,
                                                                  const IInterface* p ) :
AthAlgTool(t,n,p),
m_thinningSvc("ThinningSvc",n),
m_selectionString(""),
m_ntot(0),
m_npass(0),
m_SGKey(""),
m_and(false)
{
    declareInterface<DerivationFramework::IThinningTool>(this);
    //thinning service
    declareProperty("ThinningService", m_thinningSvc);
    // logic and selection settings
    declareProperty("SelectionString", m_selectionString);
    declareProperty("ApplyAnd", m_and);
    declareProperty("ContainerName", m_SGKey);
}

// Destructor
DerivationFramework::GenericObjectThinning::~GenericObjectThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::GenericObjectThinning::initialize()
{
    ATH_MSG_VERBOSE("initialize() ...");
    if (m_selectionString=="") {
        ATH_MSG_FATAL("No selection string provided!");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Selection string: " << m_selectionString);}
    
    // Set up the text-parsing machinery for thinning the tracks directly according to user cuts
    if (m_selectionString!="") {
	    ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
	    proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
	    proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
	    m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
	    m_parser->loadExpression(m_selectionString);
    }

    //check xAOD::InDetTrackParticle collection
    if (m_SGKey=="") {
        ATH_MSG_FATAL("No object collection provided for thinning.");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_SGKey << "as the source collection");}
        
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::GenericObjectThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" objects, "<< m_npass<< " were retained ");
    if (m_parser) {
        delete m_parser;
        m_parser = 0;
    }
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::GenericObjectThinning::doThinning() const
{
    
    // Retrieve main object collection
    const xAOD::IParticleContainer* particles = evtStore()->retrieve< const xAOD::IParticleContainer >( m_SGKey );
    if( ! particles ) {
        ATH_MSG_ERROR ("Couldn't retrieve IParticles with key: " << m_SGKey );
        return StatusCode::FAILURE;
    }   
 
    // Check the event contains objects
    unsigned int nObjects = particles->size();
    if (nObjects==0) return StatusCode::SUCCESS;
    
    // Set up a mask with the same entries as the full collection
    std::vector<bool> mask;
    mask.assign(nObjects,false); // default: don't keep any tracks
    m_ntot += nObjects;
    
    // Execute the text parser and update the mask
    if (m_parser) {
    	std::vector<int> entries =  m_parser->evaluateAsVector();
    	unsigned int nEntries = entries.size();
    	// check the sizes are compatible
    	if (nObjects != nEntries ) {
    		ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used the same collection as the input collection??");
            return StatusCode::FAILURE;
    	} else {
            // set mask
            for (unsigned int i=0; i<nObjects; ++i) if (entries[i]==1) mask[i]=true;
    	}
    }
    // Count the mask
    for (unsigned int i=0; i<nObjects; ++i) {
        if (mask[i]) ++m_npass;
    }

    // Execute the thinning service based on the mask. Finish.
    IThinningSvc::Operator::Type thinningOperator;
    if (m_and) thinningOperator = IThinningSvc::Operator::And;
    else thinningOperator = IThinningSvc::Operator::Or;
    if (m_thinningSvc->filter(*particles, mask, thinningOperator).isFailure()) {
      ATH_MSG_ERROR("Application of thinning service failed! ");
      return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
}

