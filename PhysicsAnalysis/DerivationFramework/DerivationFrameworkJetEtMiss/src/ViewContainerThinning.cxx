///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ViewContainerThinning.cxx 
// Implementation file for class ViewContainerThinning
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// DerivationFrameworkJetEtMiss includes
#include "ViewContainerThinning.h"

// STL includes

// FrameWork includes
#include "StoreGate/ThinningHandle.h"
#include "StoreGate/ReadHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticleContainer.h"



namespace DerivationFramework {

  template <class T>
  StatusCode
  ViewContainerThinning::doThinningT (const SG::ThinningHandleKey<T>& key,
                                      const SG::ReadHandleKey<T>& viewKey) const
  {
    const EventContext& ctx = Gaudi::Hive::currentContext();

    SG::ThinningHandle<T> sourceCont (key, ctx);
    SG::ReadHandle<T>     viewCont   (viewKey, ctx);
    
    std::vector<bool> masks;
    masks.assign( sourceCont->size(), false);
    
    for( const auto* part: *viewCont){ 
      masks[ part->index() ] = true;
    }

    sourceCont.keep (masks);
    return StatusCode::SUCCESS;
  }


  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode ViewContainerThinning::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    ATH_CHECK( m_trackParticleKey.initialize (m_streamName, SG::AllowEmpty) );
    ATH_CHECK( m_caloClusterKey.initialize   (m_streamName, SG::AllowEmpty) );
    ATH_CHECK( m_truthParticleKey.initialize (m_streamName, SG::AllowEmpty) );

    ATH_CHECK( m_trackParticleViewKey.initialize (SG::AllowEmpty) );
    ATH_CHECK( m_caloClusterViewKey.initialize   (SG::AllowEmpty) );
    ATH_CHECK( m_truthParticleViewKey.initialize (SG::AllowEmpty) );

    return StatusCode::SUCCESS;
  }

  StatusCode ViewContainerThinning::finalize()
  {
    return StatusCode::SUCCESS;
  }



  StatusCode ViewContainerThinning::doThinning() const
  {
    if (!m_trackParticleKey.empty()) {
      ATH_CHECK( doThinningT (m_trackParticleKey, m_trackParticleViewKey) );
    }
    if (!m_caloClusterKey.empty()) {
      ATH_CHECK( doThinningT (m_caloClusterKey, m_caloClusterViewKey) );
    }
    if (!m_truthParticleKey.empty()) {
      ATH_CHECK( doThinningT (m_truthParticleKey, m_truthParticleViewKey) );
    }
    return StatusCode::SUCCESS;
  }

}
