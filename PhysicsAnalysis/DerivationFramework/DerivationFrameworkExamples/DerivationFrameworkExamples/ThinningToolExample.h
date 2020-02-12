/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ThinningToolExample.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_THINNINGTOOLEXAMPLE_H
#define DERIVATIONFRAMEWORK_THINNINGTOOLEXAMPLE_H 1

#include<string>
#include<atomic>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "StoreGate/ThinningHandleKey.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IThinningTool.h"


namespace DerivationFramework {

  /** @class ThinningToolExample
  
      @author James Catmore -at- cern.ch
     */
  class ThinningToolExample : public extends<AthAlgTool, IThinningTool> {
    
  public: 
    /** Constructor with parameters */
    ThinningToolExample( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    virtual ~ThinningToolExample();
    
    // Athena algtool's Hooks
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    
    /** Check that the current event passes this filter */
    virtual StatusCode doThinning() const override;
 
  private:
    StringProperty m_streamName
    { this, "StreamName", "", "Name of the stream being thinned" };
    SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_inDetSGKey
      { this, "InDetTrackParticlesKey", "InDetTrackParticles", "" };

    mutable std::atomic<unsigned int> m_ntot;
    mutable std::atomic<unsigned int> m_npass;
    double m_trackPtCut;
  }; 
  
}


#endif
