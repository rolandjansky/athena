/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_IEXTENDTRACKTOLAYERTOOL_H
#define TRIGEFMISSINGET_IEXTENDTRACKTOLAYERTOOL_H

#include "AsgTools/IAsgTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <limits>
#include <cmath>

class IExtendTrackToLayerTool : public virtual asg::IAsgTool
{
  ASG_TOOL_INTERFACE(IExtendTrackToLayerTool)
public:
  /// Virtual destructor
  virtual ~IExtendTrackToLayerTool() {}

  /**
   * @brief Helper struct to hold track extrapolation information
   *
   * Note that the extrapolation could decide that the given track never
   * reaches the requested layer, in which case phi and eta will be -inf
   */
  struct TrackExtension
  {
    /// The extrapolated eta
    double eta{-std::numeric_limits<double>::infinity()};
    /// The extrapolated phi
    double phi{-std::numeric_limits<double>::infinity()};
    /// Whether or not the track crossed the layer
    inline bool isValid() const
    {
      return std::isfinite(phi) && std::isfinite(eta);
    }
  };

  /**
   * @brief Extend the track to a given layer
   * @param ctx The event context to use
   * @param track The track to extend
   * @return A struct containing the new eta/phi coordinates for the track
   */
  virtual TrackExtension extendTrack(
    const EventContext& ctx,
     const xAOD::TrackParticle &track) const = 0;
}; //> end class IExtendTrackToLayerTool

#endif //> !TRIGEFMISSINGET_IEXTENDTRACKTOLAYERTOOL_H
