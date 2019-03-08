// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRACKSELECTIONTOOL_IINDETTRACKSELECTIONTOOL_H
#define INDETTRACKSELECTIONTOOL_IINDETTRACKSELECTIONTOOL_H

// Framework include(s):
#include "PATCore/IAsgSelectionTool.h"

// EDM include(s):
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"

#ifndef XAOD_ANALYSIS
#include "TrkParameters/TrackParameters.h"
#endif

// forward declarations
#ifndef XAOD_ANALYSIS
namespace Trk {
  class Track;
  class Vertex;
  class TrackParticleBase;
  class FitQuality;
}
#endif

namespace InDet {

  // define the flags to select cut level of tool
  // see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/InDetTrackingPerformanceGuidelines
  // if you add levels, make sure to add entries to the map in InDetTrackSelectionTool.cxx
  // so that they can be set from athena as well.
  enum
#if __cplusplus > 201100L
  class
#endif
  CutLevel {
    NoCut, Loose, LoosePrimary, TightPrimary,
    LooseMuon, LooseElectron, LooseTau,
    MinBias, HILoose, HITight, ITkLoose
      };
  
  /// Interface for track selection tool
  ///
  /// @author Michael Clark <michael.ryan.clark@cern.ch>
  /// 
  ///
  class IInDetTrackSelectionTool : public virtual ::IAsgSelectionTool {
    
    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE( InDet::IInDetTrackSelectionTool )

  public:
    /// Function initialising the tool
    virtual StatusCode initialize() = 0;
    /// Function finalizing the tool
    virtual StatusCode finalize() = 0;

    /// Get an object describing the "selection steps" of the tool
    virtual const Root::TAccept& getTAccept() const = 0;

    /// Get the decision using a generic IParticle pointer
    virtual const Root::TAccept& accept( const xAOD::IParticle* p ) const = 0;

    /// Decide whether the track in question is a "good track" or not
    virtual const Root::TAccept& accept( const xAOD::TrackParticle& track,
					 const xAOD::Vertex* vertex = 0 ) const = 0;

#ifndef XAOD_ANALYSIS
    virtual const Root::TAccept& accept( const Trk::Track& track,
					 const Trk::Vertex* vertex = 0 ) const = 0;
#endif

    /// Set the selection to a pre-defined standard set of cuts
    // This function should not be public in Athena, as the cut level should be 
    //   set in the job options. Allow it for now, so as to not break compatibility
    virtual void setCutLevel( InDet::CutLevel level, Bool_t overwrite = true )
      __attribute__ ((deprecated("The cut level should be set in the job options through the \"CutLevel\" property.")))
      = 0;

  }; // class IInDetTrackSelectionTool

} // namespace InDet

#endif // INDETTRACKSELECTIONTOOL_IINDETTRACKSELECTIONTOOL_H
