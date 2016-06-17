/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// EventInfoPixelDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: David Salek (David.Salek@cern.ch)
// Decorator tool to store per-module summary information for pixel clusters

#include "DerivationFrameworkInDet/EventInfoPixelDecorator.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"
#include <vector>
#include <string>

namespace DerivationFramework {

  EventInfoPixelDecorator::EventInfoPixelDecorator(const std::string& type,
      const std::string& name,
      const IInterface* parent) : 
    AthAlgTool(type,name,parent),  
    m_parser(0),
    m_selectionString(""),
    m_ntot(0),
    m_npass(0),
    m_eventInfoKey(""),
    m_decorationPrefix(""),
    m_pixelKey("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("DecorationPrefix",       m_decorationPrefix);
    declareProperty("EventInfoKey",           m_eventInfoKey="EventInfo");
    declareProperty("SelectionString", m_selectionString);
    declareProperty("TrackMeasurementValidationKey", m_pixelKey="PixelClusters");
  }

  StatusCode EventInfoPixelDecorator::initialize()
  {
    ATH_MSG_VERBOSE("initialize() ...");

    if (m_selectionString=="") {
        ATH_MSG_FATAL("No inner detector track selection string provided!");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Selection string for the per-module pixel cluster decoration: " << m_selectionString);}

    if (m_decorationPrefix=="") {
      ATH_MSG_WARNING("No decoration prefix name provided for the output of EventInfoPixelDecorator!");
    }
    
    if (m_eventInfoKey=="") {
      ATH_MSG_ERROR("No collection provided for EventInfoPixelDecorator!");
      return StatusCode::FAILURE;
    }

    //check xAOD::TrackMeasurementValidation collection
    if (m_pixelKey=="") {
        ATH_MSG_FATAL("No track measurement validation collection provided for EventInfoPixelDecorator.");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_pixelKey << " as the source collection for EventInfoPixelDecorator.");}

    // Set up the text-parsing machinery
    if (m_selectionString!="") {
	    ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
	    proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
	    proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
	    m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
	    m_parser->loadExpression(m_selectionString);
    }

    return StatusCode::SUCCESS;
  }

  StatusCode EventInfoPixelDecorator::finalize()
  {
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" measurements, "<< m_npass<< " were used for the per-module summary");
    if (m_parser) {
        delete m_parser;
        m_parser = 0;
    }
    return StatusCode::SUCCESS;
  }

  StatusCode EventInfoPixelDecorator::addBranches() const
  {
    ATH_MSG_DEBUG("Adding per-module information to EventInfo");

    const xAOD::EventInfo* eventInfo;
    CHECK( evtStore()->retrieve( eventInfo, m_eventInfoKey ) );

    // Get the cluster container
    const xAOD::TrackMeasurementValidationContainer* clusters;
    if (evtStore()->retrieve(clusters, m_pixelKey).isFailure()) {
      ATH_MSG_ERROR ("Couldn't retrieve TrackMeasurementValidationContainer with key" << m_pixelKey);
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
 
    // fill the per-module information
    std::vector<int> perModuleMultiplicity;
    std::vector<int> layer;
    std::vector<int> eta_module;
    std::vector<int> phi_module;

    unsigned int i=0;
    std::vector<int> keys;
    for (const xAOD::TrackMeasurementValidation* clusIt : *clusters) {

      if( !mask[++i] ) continue;

      static SG::AuxElement::Accessor<int> acc_layer("layer");
      int clus_layer = acc_layer(*clusIt);
      static SG::AuxElement::Accessor<int> acc_eta_module("eta_module");
      int clus_eta_module = acc_eta_module(*clusIt);
      static SG::AuxElement::Accessor<int> acc_phi_module("phi_module");
      int clus_phi_module = acc_phi_module(*clusIt);

      int key = clus_layer * 10000 + clus_eta_module * 100 + clus_phi_module;
      unsigned int index = 9999;
      for( unsigned int j = 0; j < keys.size() ; j++ )
        if( key == keys[j] ){
          index = j;
          break;
        }
      if( index == 9999 ){
        keys.push_back( key );
        layer.push_back( clus_layer );
        eta_module.push_back( clus_eta_module );
        phi_module.push_back( clus_phi_module );
        perModuleMultiplicity.push_back( 1 );
      } else {
        perModuleMultiplicity[index]++;
      }
    }

    // decorate per-layer multiplicity
    eventInfo->auxdecor< std::vector<int> >(m_decorationPrefix+"_perModuleMultiplicity") = perModuleMultiplicity;
    eventInfo->auxdecor< std::vector<int> >(m_decorationPrefix+"_layer")                 = layer;
    eventInfo->auxdecor< std::vector<int> >(m_decorationPrefix+"_eta_module")            = eta_module;
    eventInfo->auxdecor< std::vector<int> >(m_decorationPrefix+"_phi_module")            = phi_module;
    
    return StatusCode::SUCCESS;
  }  
  
}
