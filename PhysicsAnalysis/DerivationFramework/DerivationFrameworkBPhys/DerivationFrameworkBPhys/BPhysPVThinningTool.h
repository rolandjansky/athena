/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BPhysPVThinningTool.h
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_BPhysPVThinningTool_H
#define DERIVATIONFRAMEWORK_BPhysPVThinningTool_H 1

#include "xAODTracking/VertexContainer.h"
// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "StoreGate/ThinningHandleKey.h"
// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IThinningTool.h"

namespace DerivationFramework {

  
  class BPhysPVThinningTool : public AthAlgTool, public IThinningTool {
    
  public: 
    /** Constructor with parameters */
    BPhysPVThinningTool( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~BPhysPVThinningTool();
    
    // Athena algtool's Hooks
    virtual StatusCode  initialize() override;
    virtual StatusCode  finalize() override;
    
    /** Check that the current event passes this filter */
    virtual StatusCode doThinning() const override;
 
  private:
    StringProperty m_streamName{ this, "StreamName", "", "Name of the stream being thinned" };
    SG::ReadHandleKeyArray<xAOD::VertexContainer> m_BPhyCandList;
    SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_TrackContainerName;
    SG::ThinningHandleKey<xAOD::VertexContainer> m_PVContainerName;
    mutable std::atomic<unsigned int> m_ntot;
    mutable std::atomic<unsigned int> m_npass;
    mutable std::atomic<unsigned int> m_tracks_kept;
    bool m_keepTracks;
  }; 
  
}


#endif
