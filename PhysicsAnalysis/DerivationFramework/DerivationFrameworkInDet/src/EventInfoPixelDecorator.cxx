/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// EventInfoPixelDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: David Salek (David.Salek@cern.ch)
// Decorator tool to store per-module summary information for pixel clusters

#include "DerivationFrameworkInDet/EventInfoPixelDecorator.h"
#include "DerivationFrameworkInDet/DecoratorUtils.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "xAODEventInfo/EventAuxInfo.h"
#include <vector>
#include <string>

namespace DerivationFramework {

  EventInfoPixelDecorator::EventInfoPixelDecorator(const std::string& type,
      const std::string& name,
      const IInterface* parent) : 
    ExpressionParserUser<AthAlgTool>(type,name,parent)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
  }

  StatusCode EventInfoPixelDecorator::initialize()
  {
    ATH_MSG_VERBOSE("initialize() ...");

    if (m_selectionString.empty()) {
        ATH_MSG_FATAL("No inner detector track selection string provided!");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Selection string for the per-module pixel cluster decoration: " << m_selectionString);}

    if (m_decorationPrefix.empty()) {
      ATH_MSG_WARNING("No decoration prefix name provided for the output of EventInfoPixelDecorator!");
    }
    
    if (m_eventInfoKey.key().empty()) {
      ATH_MSG_ERROR("No collection provided for EventInfoPixelDecorator!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK(m_eventInfoKey.initialize());

    //check xAOD::TrackMeasurementValidation collection
    ATH_CHECK(m_pixelKey.initialize());
    ATH_MSG_INFO("Using " << m_pixelKey.key() << " as the source collection for EventInfoPixelDecorator.");

    // Set up the text-parsing machinery
    if (!m_selectionString.empty()) {
       ATH_CHECK( initializeParser(m_selectionString));
    }

    {
       std::vector<std::string> names;
       names.resize(kNIntDecor);
       names[kperModuleMultiplicity] ="perModuleMultiplicity";
       names[klayer]                 ="layer";
       names[keta_module]            ="eta_module";
       names[kphi_module]            ="phi_module";
       createDecoratorKeys(*this,m_eventInfoKey,m_decorationPrefix.value()+"_",names, m_intDecorKeys);
    }

    return StatusCode::SUCCESS;
  }

  StatusCode EventInfoPixelDecorator::finalize()
  {
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" measurements, "<< m_npass<< " were used for the per-module summary");
    ATH_CHECK( finalizeParser() );
    return StatusCode::SUCCESS;
  }

  StatusCode EventInfoPixelDecorator::addBranches() const
  {
    ATH_MSG_DEBUG("Adding per-module information to EventInfo");
    const EventContext& ctx = Gaudi::Hive::currentContext();

    SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey, ctx);
    CHECK( eventInfo.isValid() ? StatusCode::SUCCESS : StatusCode::FAILURE);

    // Get the cluster container
    SG::ReadHandle<xAOD::TrackMeasurementValidationContainer> clusters(m_pixelKey, ctx);
    if (!clusters.isValid()) {
       ATH_MSG_ERROR ("Couldn't retrieve TrackMeasurementValidationContainer with key" << m_pixelKey.key());
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
    unsigned int n_pass=0;
    for (unsigned int i=0; i<nClusters; ++i) {
        if (mask[i]) ++n_pass;
    }
    m_npass += n_pass;

    // fill the per-module information
    std::array<std::vector<int>,kNIntDecor> vec;

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
        vec[klayer].push_back( clus_layer );
        vec[keta_module].push_back( clus_eta_module );
        vec[kphi_module].push_back( clus_phi_module );
        vec[kperModuleMultiplicity].push_back( 1 );
      } else {
        assert(index < vec[kperModuleMultiplicity].size());
        vec[kperModuleMultiplicity][index]++;
      }
    }

    std::vector<SG::WriteDecorHandle<xAOD::EventInfo,std::vector<int> > >   int_decor_handles(createDecorators<xAOD::EventInfo,std::vector<int> >(m_intDecorKeys,ctx));
    assert(int_decor_handles.size() == kNIntDecor);
    for(unsigned int decorate_i=0; decorate_i<int_decor_handles.size(); ++decorate_i) {
       int_decor_handles[decorate_i](*eventInfo) = std::move(vec[decorate_i]);
    }

    return StatusCode::SUCCESS;
  }  
  
}
