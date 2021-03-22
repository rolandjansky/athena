/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigVertexCountsCnvTool.cxx 603232 2014-06-23 17:24:35Z azemla $

// EDM include(s):
#include "xAODTrigMinBias/TrigVertexCounts.h"
#include "TrigInDetEvent/TrigVertexCountsCollection.h"

// Local include(s):
#include "TrigVertexCountsCnvTool.h"

namespace xAODMaker {
  
  TrigVertexCountsCnvTool::TrigVertexCountsCnvTool( const std::string& type,
						    const std::string& name,
						    const IInterface* parent )
  : AthAlgTool( type, name, parent ) {
    
    // Declare the interface(s) provided by the tool:
    declareInterface< ITrigVertexCountsCnvTool >( this );
  }
  
  StatusCode TrigVertexCountsCnvTool::initialize() {
    
    // Greet the user:
    ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
    
    // Return gracefully:
    return StatusCode::SUCCESS;
  }
  
  /**
   * This is the important function of the tool. It takes the old EDM objects
   * from a TrigVertexCountsContainer, and fills an xAOD::TrigVertexCountsContainer with them.
   *
   * @param aod The AOD object to take the TrigVertexCounts from
   * @param xaod The xAOD container to fill the TrigVertexCounts into
   * @returns <code>StatusCode::SUCCESS</code> if all went fine,
   *          something else if not
   */
  StatusCode TrigVertexCountsCnvTool::convert( const TrigVertexCountsCollection* aod,
					       xAOD::TrigVertexCountsContainer* xaod ) const {
    
    // A small sanity check. The output container should really be empty...
    if( !xaod->empty() ) {
      ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
      << xaod->size() << ")" );
    }
    ATH_MSG_VERBOSE("AOD TrigVertexCountsCollection size:: "<< aod->size());
    
    TrigVertexCountsCollection::const_iterator tvc_iter = aod->begin();
    TrigVertexCountsCollection::const_iterator tvc_iter_end = aod->end();
    
    for(;tvc_iter != tvc_iter_end;++tvc_iter){
      // Create a (single) output xAOD object.
      xAOD::TrigVertexCounts* xtvc = new xAOD::TrigVertexCounts();
      xaod->push_back(xtvc);
    
      xtvc->setVtxNtrks((*tvc_iter)->vtxNtrks());
      xtvc->setVtxTrkPtSqSum((*tvc_iter)->vtxTrkPtSqSum());
      
      ATH_MSG_VERBOSE("AOD vs xAOD::vtxNtrks size:: "<< (*tvc_iter)->vtxNtrks().size()<<" "<<xtvc->vtxNtrks().size());
      ATH_MSG_VERBOSE("AOD vs xAOD:vtxTrkPtSqSum size:: "<< (*tvc_iter)->vtxTrkPtSqSum().size()<<" "<<xtvc->vtxTrkPtSqSum().size());
      // 
    }
    // Return gracefully:
    return StatusCode::SUCCESS;
					       }
					       
} // namespace xAODMaker
