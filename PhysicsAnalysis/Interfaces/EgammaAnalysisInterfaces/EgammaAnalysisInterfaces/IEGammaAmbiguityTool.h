/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAANALYSISINTERFACES__IEGammaAmbiguityTool__
#define EGAMMAANALYSISINTERFACES__IEGammaAmbiguityTool__

/**
   @class EGammaAmbiguityTool
   @brief Electron / photon ambiguity resolution. Dual-use version, interface

   @author Bruno Lenzi
   @date   May 2015
*/

// Include the interfaces
#include "AsgTools/IAsgTool.h"

// Forward declarations
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/EgammaContainerFwd.h"

//This will need to be copied over to EgammaEnums.h
//under xAODEgamma
//Plus Set/Get true Accessor methods when we have 
//finalise the design.
namespace xAOD {
  namespace AmbiguityTool {
    /// @name Ambiguity types
    enum AmbiguityType {
      electron = 0,
      ambiguousTrackEoverPBetterThanVertexEoverP=1,
      ambiguousTrackLowPt=2,
      ambiguousTrackHighEoverP=3,
      ambiguousTrackNoPixel =4,
      ambiguousVertexEoverPBetterThanTrackEoverP=5,
      photon=6,
      unknown=7
    };
  }
}



class IEGammaAmbiguityTool : virtual public asg::IAsgTool
{
  ASG_TOOL_INTERFACE(IEGammaAmbiguityTool)

public:

  /**Virtual Destructor*/
  virtual ~IEGammaAmbiguityTool() {};

  /** Return value: AuthorElectron, AuthorPhoton, AuthorAmbiguous, AuthorUnknown */
  virtual unsigned int ambiguityResolve(const xAOD::CaloCluster* cluster,
                                        const xAOD::Vertex* vx,
                                        const xAOD::TrackParticle* tp, xAOD::AmbiguityTool::AmbiguityType& type) const = 0;

  /** Return value: AuthorElectron, AuthorPhoton, AuthorAmbiguous, AuthorUnknown 
      Needed because of cliets
      implementation calls method above
   */
  virtual unsigned int ambiguityResolve(const xAOD::CaloCluster* cluster,
                                        const xAOD::Vertex* vx,
                                        const xAOD::TrackParticle* tp) const = 0;

  /** Redo the ambiguity resolution of central electrons and photons and return
    * AuthorElectron, AuthorPhoton, AuthorAmbiguous, AuthorUnknown 
    * or the author of the object if no overlapping object is found **/
  virtual unsigned int ambiguityResolve(const xAOD::Egamma& egamma) const = 0;

  /** Accept or reject egamma object based on ambiguity resolution */
  virtual bool accept( const xAOD::Egamma& egamma) const = 0;
  
}; // End: class definition


#endif

