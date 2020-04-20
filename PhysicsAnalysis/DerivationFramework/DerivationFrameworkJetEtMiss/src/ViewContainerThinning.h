///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ViewContainerThinning.h 
// Header file for class ViewContainerThinning
// Author: P-A Delsart
/////////////////////////////////////////////////////////////////// 
#ifndef DERIVATIONFRAMEWORKJETETMISS_VIEWCONTAINERTHINNING_H
#define DERIVATIONFRAMEWORKJETETMISS_VIEWCONTAINERTHINNING_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// DerivationFrameworkInterfaces includes
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "StoreGate/ThinningHandleKey.h"
#include "StoreGate/ReadHandleKey.h"


namespace DerivationFramework {

  class ViewContainerThinning
    :             public extends<AthAlgTool, IThinningTool>
  { 

    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
  public: 
    /// Delegate ctor to base.
    using base_class::base_class;

    // Athena algtool's Hooks
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    virtual StatusCode doThinning() const override;

  protected: 

    template <class T>
    StatusCode doThinningT (const SG::ThinningHandleKey<T>& key,
                            const SG::ReadHandleKey<T>& viewKey) const;

    StringProperty m_streamName
      { this, "StreamName", "", "Name of the stream being thinned" };

    SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_trackParticleKey
      { this, "TrackParticleKey", "", "" };
    SG::ThinningHandleKey<xAOD::CaloClusterContainer> m_caloClusterKey
      { this, "CaloClusterKey", "", "" };
    SG::ThinningHandleKey<xAOD::TruthParticleContainer> m_truthParticleKey
      { this, "TruthParticleKey", "", "" };

    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticleViewKey
      { this, "TrackParticleViewKey", "", "" };
    SG::ReadHandleKey<xAOD::CaloClusterContainer> m_caloClusterViewKey
      { this, "CaloClusterViewKey", "", "" };
    SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthParticleViewKey
      { this, "TruthParticleViewKey", "", "" };
  }; 

}

#endif //> !DERIVATIONFRAMEWORKJETETMISS_VIEWCONTAINERTHINNING_H
