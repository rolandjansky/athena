/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackParticleTrackSummaryAssociationTool.cxx 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/TrackParticleTrackSummaryAssociationTool.cxx
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Associate from a TrackParticle to its default TrackSummary.
 */

#include "TrackParticleTrackSummaryAssociationTool.h"

#include "AthenaKernel/errorcheck.h"
#include "Particle/TrackParticle.h"
#include "TrkTrackSummary/TrackSummary.h"

namespace D3PD {

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrackParticleTrackSummaryAssociationTool::TrackParticleTrackSummaryAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
}


/**
 * @brief Return the target object.
 * @param track The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Trk::TrackSummary *TrackParticleTrackSummaryAssociationTool::get (const Rec::TrackParticle& track)
{
  return track.trackSummary();
}

} // namespace D3PD
