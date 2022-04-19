/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   IMultiStateExtrapolator.h
 * @date   Tuesday 25th January 2005
 * @author Anthony Morley, Christos Anastopoulos
 * Abstract base class for extrapolation of a MultiComponentState
 */

#ifndef TrkIMultiStateExtrapolator_H
#define TrkIMultiStateExtrapolator_H

#include "TrkGaussianSumFilterUtils/GsfMaterial.h"
#include "TrkGaussianSumFilterUtils/MultiComponentState.h"
//
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/BoundaryCheck.h"
//
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IAlgTool.h"
//
#include <memory>
#include <vector>

namespace Trk {

class Surface;
class Track;
class TrackingVolume;
class TrackStateOnSurface;

/** @struct StateAtBoundarySurface
  - Structure to contain information about a state at the interface between
  tracking volumes
  */

struct StateAtBoundarySurface
{
  /** Data members */
  const MultiComponentState* stateAtBoundary = nullptr;
  const TrackParameters* navigationParameters = nullptr;
  const TrackingVolume* trackingVolume = nullptr;
  /** Update State at Boundary Surface Information */
  void updateBoundaryInformation(const MultiComponentState* boundaryState,
                                 const TrackParameters* navParameters,
                                 const TrackingVolume* nextVolume)
  {
    stateAtBoundary = boundaryState;
    navigationParameters = navParameters;
    trackingVolume = nextVolume;
  }
};

static const InterfaceID IID_IMultiStateExtrapolator("IMultiStateExtrapolator",
                                                     1,
                                                     0);

class IMultiStateExtrapolator : virtual public IAlgTool
{
public:
  /** @brief MultiStateExtrapolator cache class.
   *
   * This object holds information regarding the internal state of the
   * extrpolation process as well as a large store for  material effects
   * properties.
   *
   * It to be passed as argument to extrapolation calls
   */
  struct Cache
  {
    //!< Flag the recall solution
    bool m_recall = false;
    //!< Surface for recall   (not owning)
    const Surface* m_recallSurface = nullptr;
    //!< Layer for recall   (not owning)
    const Layer* m_recallLayer = nullptr;
    //!< Tracking volume for recall (not owning)
    const TrackingVolume* m_recallTrackingVolume = nullptr;
    //!< Instance of structure describing the state
    //!< at a boundary of tracking volumes
    StateAtBoundarySurface m_stateAtBoundarySurface;
    // Vector of combined material effects
    std::vector<GsfMaterial::Combined> m_materialEffectsCaches;
    //!< Recycle bin for MultiComponentState objects,keep track of them
    //!< and delete at the end
    std::vector<std::unique_ptr<const MultiComponentState>> m_mcsRecycleBin;
    //!< Recycle bin for TrackParameter objects, keep track of them
    //!< and delete at the end
    std::vector<std::unique_ptr<const TrackParameters>> m_tpRecycleBin;

    Cache() { m_materialEffectsCaches.reserve(12); }
  };

  /** Virtual destructor */
  virtual ~IMultiStateExtrapolator() = default;

  /** AlgTool interface method */
  static const InterfaceID& interfaceID()
  {
    return IID_IMultiStateExtrapolator;
  };

  /** Configured AlgTool extrapolation method (1) */
  virtual MultiComponentState extrapolate(
    const EventContext& ctx,
    Cache&,
    const MultiComponentState&,
    const Surface&,
    PropDirection direction,
    const BoundaryCheck& boundaryCheck,
    ParticleHypothesis particleHypothesis) const = 0;

  /** Configured AlgTool extrapolation without material effects method (2) */
  virtual MultiComponentState extrapolateDirectly(
    const EventContext& ctx,
    const MultiComponentState&,
    const Surface&,
    PropDirection direction,
    const BoundaryCheck& boundaryCheck,
    ParticleHypothesis particleHypothesis) const = 0;
};

} // end trk namespace

#endif
