/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// BTrackVertexMapLogger.cxx
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
// - w.w., 2017-01-22: Added use of BPhysMetaDataTool.
//
// Store JO metadata in the output file.
//
// It uses the BPhysMetaDataTool (default) or the IOVDbMetaDataTool to
// store job option information as metadata in a specific branch whose
// name needs to prefixed by the deriviation format name.
// Note: Metadata stored by the IOVDbMetaDataTool is not readable on
// 'RootCore' level.
//
// This is a base class.  Inherit from it to add the job options you want
// to store.  For a usage example, see
//   Bmumu_metadata.h / Bmumu_metadata.cxx
// and
//   BPHY8.py .
//
// Job options provided by the base class:
// - DerivationName       -- assign the name of the derivation format
// - MetadataFolderName   -- assign the name of the metadata folder,
//                           should start with the derivation format name,
//                           defaults to DerivationName if not set.
// - UseIOVDbMetaDataTool -- use the IOVDbMetaDataTool to store
//                           the additional metadata
// - UseBPhysMetaDataTool -- use the BPhysMetaDataTool to store
//                           the additional metadata
//                           
//============================================================================
//

#include "DerivationFrameworkBPhys/BTrackVertexMapLogger.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

namespace DerivationFramework {

  //--------------------------------------------------------------------------
  BTrackVertexMapLogger::BTrackVertexMapLogger(const std::string& t,
				       const std::string& n,
				       const IInterface*  p)
    : AthAlgTool(t,n,p) {
    
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    
    // Declare BPhysTrackVertexMapTool handles
    declareProperty("TrackVertexMapTools", m_ttvmTools);

    // Enable log output?
    declareProperty("Enable", m_enable = true);
  }
  //--------------------------------------------------------------------------
  StatusCode BTrackVertexMapLogger::initialize() {
  
    ATH_MSG_DEBUG("BTrackVertexMapLogger::initialize() -- begin");

    // get the BPhysTrackVertexMapTools
    if ( m_enable ) {
      for (auto ttvmTool : m_ttvmTools) {
	ATH_CHECK( ttvmTool.retrieve() );
	ATH_MSG_INFO("initialize: Successfully retrieved "
		     << ttvmTool.name() << " ....");
      }
    } // if m_enable

    ATH_MSG_DEBUG("BTrackVertexMapLogger::initialize() -- end");

    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
  StatusCode BTrackVertexMapLogger::finalize() {

    ATH_MSG_DEBUG("BTrackVertexMapLogger::finalize()");

    // everything all right
    return StatusCode::SUCCESS;
  }
  //--------------------------------------------------------------------------
  StatusCode BTrackVertexMapLogger::addBranches() const {

    ATH_MSG_DEBUG("BTrackVertexMapLogger::addBranches()");

    // call the BPhysTrackVertexMapTools
    if ( m_enable ) {
      for (auto ttvmTool : m_ttvmTools) {
	if ( ttvmTool->doLog() ) {
	  ATH_MSG_INFO("addBranches: dump for " << ttvmTool.name() << ":");
	  ATH_CHECK( ttvmTool->logEvent() );
	} // if doLog()
      } // for
    } // if m_enable

    // still everything is ok
    return StatusCode::SUCCESS;
  }  
  //--------------------------------------------------------------------------
}
