/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////
// TrackParametersAtPV.h  (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TRACKPARAMETERSATPV_H
#define DERIVATIONFRAMEWORK_TRACKPARAMETERSATPV_H 

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include <vector>

#include <StoreGate/ReadHandleKey.h>
#include <StoreGate/WriteHandleKey.h>

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {

  /** @class TrackParametersAtPV
 
      the code used in this implementation is kindly stolen from:
      atlasoff:: ISF/ISF_Core/ISF_Tools

      @author James Catmore -at- cern.ch
     */
  class TrackParametersAtPV : public AthAlgTool, public IAugmentationTool {
   
  public: 
    /** Constructor with parameters */
    TrackParametersAtPV( const std::string& t, const std::string& n, const IInterface* p );
   
    /** Destructor */
    ~TrackParametersAtPV();
   
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
 
    /** Check that the current event passes this filter */
    virtual StatusCode addBranches() const;

  private:
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_collTrackKey
       { this, "TrackParticleContainerName", "InDetTrackParticles", ""};
    SG::ReadHandleKey<xAOD::VertexContainer>        m_collVertexKey
       { this, "VertexContainerName", "PrimaryVertices", ""};

    SG::WriteHandleKey< std::vector<float> > m_trackZ0PVKey
       { this, "Z0SGEntryName", "", "" };

  };
 
}

#endif
