/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigTrackCountsCnvTool.cxx 603232 2014-06-23 17:24:35Z azemla $

// EDM include(s):
#include "xAODTrigMinBias/TrigTrackCountsContainer.h"
#include "xAODTrigMinBias/TrigTrackCounts.h"

#include "TrigInDetEvent/TrigHisto2D.h"
#include "TrigInDetEvent/TrigTrackCountsCollection.h"

// Local include(s):
#include "TrigTrackCountsCnvTool.h"

namespace xAODMaker {
  
  TrigTrackCountsCnvTool::TrigTrackCountsCnvTool( const std::string& type,
						  const std::string& name,
						  const IInterface* parent )
  : AthAlgTool( type, name, parent ) {
    
    // Declare the interface(s) provided by the tool:
    declareInterface< ITrigTrackCountsCnvTool >( this );
  }
  
  StatusCode TrigTrackCountsCnvTool::initialize() {
    
    // Greet the user:
    ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
    
    // Return gracefully:
    return StatusCode::SUCCESS;
  }
  
  /**
   * This is the important function of the tool. It takes the old EDM objects
   * from a TrigTrackCountsContainer, and fills an xAOD::TrigTrackCountsContainer with them.
   *
   * @param aod The AOD object to take the TrigTrackCounts from
   * @param xaod The xAOD container to fill the TrigTrackCounts into
   * @returns <code>StatusCode::SUCCESS</code> if all went fine,
   *          something else if not
   */
  StatusCode TrigTrackCountsCnvTool::convert( const TrigTrackCountsCollection* aod,
					      xAOD::TrigTrackCountsContainer* xaod ) const {
    
    // A small sanity check. The output container should really be empty...
    if( !xaod->empty() ) {
      ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
      << xaod->size() << ")" );
    }
    
    ATH_MSG_VERBOSE("execute: TrigTrackCountsCnvTool::convert");
    ATH_MSG_VERBOSE("AOD TrigTrackCountsCollection size::" <<aod->size());
    TrigTrackCountsCollection::const_iterator ttc_iter		= aod->begin();
    TrigTrackCountsCollection::const_iterator ttc_iter_end 	= aod->end();
    
    for(;ttc_iter !=  ttc_iter_end; ++ttc_iter)
    {
      // Create a (single) output xAOD object.
      xAOD::TrigTrackCounts* xttcounts = new xAOD::TrigTrackCounts();
      xaod->push_back(xttcounts);
      // 
      xttcounts->setZ0Bins((*ttc_iter)->z0_pt().nbins_x());
      xttcounts->setZ0Min((*ttc_iter)->z0_pt().min_x());
      xttcounts->setZ0Max((*ttc_iter)->z0_pt().max_x());
      xttcounts->setPtBins((*ttc_iter)->z0_pt().nbins_y());
      xttcounts->setPtMin((*ttc_iter)->z0_pt().min_y());
      xttcounts->setPtMax((*ttc_iter)->z0_pt().max_y());
      
      xttcounts->setZ0_pt((*ttc_iter)->z0_pt().contents());
      
      xttcounts->setEtaBins((*ttc_iter)->eta_phi().nbins_x());
      xttcounts->setEtaMin((*ttc_iter)->eta_phi().min_x());
      xttcounts->setEtaMax((*ttc_iter)->eta_phi().max_x());
      xttcounts->setPhiBins((*ttc_iter)->eta_phi().nbins_y());
      xttcounts->setPhiMin((*ttc_iter)->eta_phi().min_y());
      xttcounts->setPhiMax((*ttc_iter)->eta_phi().max_y());
      
      xttcounts->setEta_phi((*ttc_iter)->eta_phi().contents());
      
      //now some verbose stuff
      ATH_MSG_VERBOSE("Orig vs xAOD: Z0Bins:   " <<  (*ttc_iter)->z0_pt().nbins_x()<< " " << xttcounts->z0Bins());
      
    }
    // Return gracefully:
    return StatusCode::SUCCESS;
					      }
					      
} // namespace xAODMaker
