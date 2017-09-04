/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////
// TrackParametersKVU.h  (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TRACKPARAMETERSATPV_H
#define DERIVATIONFRAMEWORK_TRACKPARAMETERSATPV_H 

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"


#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace Trk
{
  class IVertexTrackUpdator;
  class IExtrapolator;
  class IVertexLinearizedTrackFactory;
  class ITrackToVertexIPEstimator;
  class LinearizedTrack;
}

namespace DerivationFramework {


  /** @class TrackParametersKVU
 
      the code used in this implementation is kindly stolen from:
      atlasoff:: ISF/ISF_Core/ISF_Tools

      @author James Catmore -at- cern.ch
     */
  class TrackParametersKVU : public AthAlgTool, public IAugmentationTool {
   
  public: 
    /** Constructor with parameters */
    TrackParametersKVU( const std::string& t, const std::string& n, const IInterface* p );
   
    /** Destructor */
    ~TrackParametersKVU();
   
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
 
    /** Check that the current event passes this filter */
    virtual StatusCode addBranches() const;

  private:
    std::string m_collTrackName;
    std::string m_collVertexName;
 

    ToolHandle< Trk::IVertexTrackUpdator > m_vertexTrackUpdator; 
    ToolHandle<Trk::IExtrapolator>  m_extrapolator;
    ToolHandle< Trk::IVertexLinearizedTrackFactory > m_LinearizedTrackFactory;
    ToolHandle< Trk::ITrackToVertexIPEstimator>   m_IPEstimator;

    std::string m_sgName;
    std::string m_sgKey1;

  }; 
 
}

#endif
