/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TrackMeasurementThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: David Salek (David.Salek@cern.ch)
// Thinning tool for TrackMeasurementValidationContainer

#include "DerivationFrameworkInDet/TrackMeasurementThinning.h"
#include "AthenaKernel/IThinningSvc.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::TrackMeasurementThinning::TrackMeasurementThinning( 	const std::string& t,
                                                 		const std::string& n,
                                                 		const IInterface* p ) : 
  AthAlgTool(t,n,p),
  m_thinningSvc("ThinningSvc",n),
  m_parser(0),
  m_selectionString(""),
  m_ntot(0),
  m_npass(0),
  m_SGKey("PixelClusters"),
  m_and(false)
  {
    declareInterface<DerivationFramework::IThinningTool>(this);
    declareProperty("ThinningService", m_thinningSvc);
    declareProperty("SelectionString", m_selectionString);
    declareProperty("ApplyAnd", m_and);
    declareProperty("TrackMeasurementValidationKey", m_SGKey);
  }
  
// Destructor
DerivationFramework::TrackMeasurementThinning::~TrackMeasurementThinning() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::TrackMeasurementThinning::initialize()
{
    ATH_MSG_VERBOSE("initialize() ...");
    if (m_selectionString=="") {
        ATH_MSG_FATAL("No inner detector track selection string provided!");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Track thinning selection string: " << m_selectionString);}
    
    //check xAOD::InDetTrackParticle collection
    if (m_SGKey=="") {
        ATH_MSG_FATAL("No track measurement validation collection provided for thinning.");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_SGKey << "as the source collection for thinning.");}

    // Set up the text-parsing machinery for thinning the tracks directly according to user cuts
    if (m_selectionString!="") {
	    ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
	    proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
	    proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
	    m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
	    m_parser->loadExpression(m_selectionString);
    }
    return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::TrackMeasurementThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" measurements, "<< m_npass<< " were retained ");
    if (m_parser) {
        delete m_parser;
        m_parser = 0;
    }
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::TrackMeasurementThinning::doThinning() const
{

    // Get the cluster container
    const xAOD::TrackMeasurementValidationContainer* clusters;
    if (evtStore()->retrieve(clusters, m_SGKey).isFailure()) {
      ATH_MSG_ERROR ("Couldn't retrieve TrackMeasurementValidationContainer with key" << m_SGKey);
      return StatusCode::FAILURE;
    }

    // Check the event contains clusters
    unsigned int nClusters = clusters->size();
    if (nClusters==0) return StatusCode::SUCCESS;

    // Set up a mask with the same entries as the full collection
    std::vector<bool> mask;
    mask.assign(nClusters,false); // default: don't keep anything
    m_ntot += nClusters;

    // Execute the text parser and update the mask
    if (m_parser) {
        std::vector<int> entries =  m_parser->evaluateAsVector();
        unsigned int nEntries = entries.size();
        // check the sizes are compatible
        if (nClusters != nEntries ) {
            ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used the same collection as the input collection??");
            return StatusCode::FAILURE;
        } else {
            // set mask
            for (unsigned int i=0; i<nClusters; ++i) if (entries[i]==1) mask[i]=true;
        }
    }
    // Count the mask
    for (unsigned int i=0; i<nClusters; ++i) {
        if (mask[i]) ++m_npass;
    }
 
    // Execute the thinning service based on the mask. Finish.
    IThinningSvc::Operator::Type thinningOperator;
    if (m_and) thinningOperator = IThinningSvc::Operator::And;
    else thinningOperator = IThinningSvc::Operator::Or;
    if (m_thinningSvc->filter(*clusters, mask, thinningOperator).isFailure()) {
      ATH_MSG_ERROR("Application of thinning service failed! ");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}  
  
