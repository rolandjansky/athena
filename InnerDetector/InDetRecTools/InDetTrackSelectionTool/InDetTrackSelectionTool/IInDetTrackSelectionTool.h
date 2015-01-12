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

//#if ( ! defined(XAOD_STANDALONE) ) && (! defined(XAOD_ANALYSIS) )
// anything defining XAOD_STANDALONe should also define XAOD_ANALYSIS
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
#if __cplusplus > 201100
  class
#endif
  CutLevel {
    NoCut=0, Loose=1, LoosePrimary=2, TightPrimary=3
      };
  
  /// Interface for track selection tools
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
    virtual void setCutLevel( InDet::CutLevel level, Bool_t overwrite = true ) = 0;

  }; // class IInDetTrackSelectionTool

} // namespace InDet

#endif // INDETTRACKSELECTIONTOOL_IINDETTRACKSELECTIONTOOL_H
