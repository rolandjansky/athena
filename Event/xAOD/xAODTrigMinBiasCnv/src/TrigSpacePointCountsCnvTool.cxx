/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$

// EDM include(s):
#include "xAODTrigMinBias/TrigSpacePointCountsContainer.h"
#include "xAODTrigMinBias/TrigSpacePointCounts.h"
#include "TrigInDetEvent/TrigSpacePointCountsCollection.h"
// Local include(s):
#include "TrigSpacePointCountsCnvTool.h"

namespace xAODMaker {
  
  TrigSpacePointCountsCnvTool::TrigSpacePointCountsCnvTool( const std::string& type,
							    const std::string& name,
							    const IInterface* parent )
  : AthAlgTool( type, name, parent ) {
    
    // Declare the interface(s) provided by the tool:
    declareInterface< ITrigSpacePointCountsCnvTool >( this );
  }
  
  StatusCode TrigSpacePointCountsCnvTool::initialize() {
    
    // Greet the user:
    ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
    
    // Return gracefully:
    return StatusCode::SUCCESS;
  }
  
  /**
   * This is the important function of the tool. It takes the muon RoI objects
   * from a LVL1_ROI container, and fills an xAOD::TrigSpacePointCountsContainer with them.
   *
   * @param aod The AOD object to take the muon RoIs from
   * @param xaod The xAOD container to fill the RoIs into
   * @returns <code>StatusCode::SUCCESS</code> if all went fine,
   *          something else if not
   */
  StatusCode TrigSpacePointCountsCnvTool::convert( const TrigSpacePointCountsCollection* aod,
						   xAOD::TrigSpacePointCountsContainer* xaod ) const {
    
    // A small sanity check. The output container should really be empty...
    if( !xaod->empty() ) {
      ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
      << xaod->size() << ")" );
    }
    
    
    TrigSpacePointCountsCollection::const_iterator tspc_iter 		= aod->begin();
    TrigSpacePointCountsCollection::const_iterator tspc_iter_end 	= aod->end();
    
    ATH_MSG_VERBOSE("AOD TrigSpacePointCountsCollection size:: "<< aod->size());
    
    for(; tspc_iter != tspc_iter_end; ++tspc_iter){
      // Add a new muon RoI object to the output:
      xAOD::TrigSpacePointCounts* xtspc = new xAOD::TrigSpacePointCounts();
      xaod->push_back( xtspc );
      
      // Now fill it with data:
      xtspc->setContentsPixelClusEndcapC((*tspc_iter)->pixelClusEndcapC().contents());
      xtspc->setContentsPixelClusBarrel((*tspc_iter)->pixelClusBarrel().contents());
      xtspc->setContentsPixelClusEndcapA((*tspc_iter)->pixelClusEndcapA().contents());
      xtspc->setPixelClusTotBins((*tspc_iter)->pixelClusEndcapC().nbins_x());
      xtspc->setPixelClusTotMin((*tspc_iter)->pixelClusEndcapC().min_x());
      xtspc->setPixelClusTotMax((*tspc_iter)->pixelClusEndcapC().max_x());
      xtspc->setPixelClusSizeBins((*tspc_iter)->pixelClusEndcapC().nbins_y());
      xtspc->setPixelClusSizeMin((*tspc_iter)->pixelClusEndcapC().min_y());
      xtspc->setPixelClusSizeMax((*tspc_iter)->pixelClusEndcapC().max_y());
      xtspc->setSctSpEndcapC((*tspc_iter)->sctSpEndcapC());
      xtspc->setSctSpBarrel((*tspc_iter)->sctSpBarrel());
      xtspc->setSctSpEndcapA((*tspc_iter)->sctSpEndcapA());
      
      ATH_MSG_VERBOSE("AOD vs xAOD pixelClusEndcapC::contents::size:: "<<(*tspc_iter)->pixelClusEndcapC().contents().size()<< " "<<xtspc->contentsPixelClusEndcapC().size() );
      ATH_MSG_VERBOSE("AOD vs xAOD pixelClusBarrel::contents::size:: "<<(*tspc_iter)->pixelClusBarrel().contents().size()<< " "<<xtspc->contentsPixelClusBarrel().size() );
      ATH_MSG_VERBOSE("AOD vs xAOD pixelClusEndcapA::contents::size:: "<<(*tspc_iter)->pixelClusEndcapA().contents().size()<< " "<<xtspc->contentsPixelClusEndcapC().size()  );
      ATH_MSG_VERBOSE("AOD vs xAOD pixelClusTotBins:: "<< (*tspc_iter)->pixelClusEndcapC().nbins_x()<< " " <<xtspc->pixelClusTotBins() );
      ATH_MSG_VERBOSE("AOD vs xAOD pixelClusTotMin:: "<< (*tspc_iter)->pixelClusEndcapC().min_x()<< " " <<xtspc->pixelClusTotMin() );
      ATH_MSG_VERBOSE("AOD vs xAOD pixelClusTotMax:: "<< (*tspc_iter)->pixelClusEndcapC().max_x()<< " " <<xtspc->pixelClusTotMax() );
      ATH_MSG_VERBOSE("AOD vs xAOD pixelClusSizeBins:: "<< (*tspc_iter)->pixelClusEndcapC().nbins_y()<< " " <<xtspc->pixelClusSizeBins() );
      ATH_MSG_VERBOSE("AOD vs xAOD pixelClusSizeMin:: "<< (*tspc_iter)->pixelClusEndcapC().min_y()<< " " <<xtspc->pixelClusSizeMin() );
      ATH_MSG_VERBOSE("AOD vs xAOD pixelClusSizeMax:: "<< (*tspc_iter)->pixelClusEndcapC().max_y()<< " " <<xtspc->pixelClusSizeMax() );
      ATH_MSG_VERBOSE("AOD vs xAOD sctSpEndcapC:: "<<(*tspc_iter)->sctSpEndcapC() << " " << xtspc->sctSpEndcapC() );
      ATH_MSG_VERBOSE("AOD vs xAOD sctSpBarrel:: "<<(*tspc_iter)->sctSpBarrel() << " " << xtspc->sctSpBarrel() );
      ATH_MSG_VERBOSE("AOD vs xAOD sctSpEndcapA:: "<<(*tspc_iter)->sctSpEndcapA() << " " << xtspc->sctSpEndcapA() );
    }
    // Return gracefully:
    return StatusCode::SUCCESS;
						   }
						   
} // namespace xAODMaker
