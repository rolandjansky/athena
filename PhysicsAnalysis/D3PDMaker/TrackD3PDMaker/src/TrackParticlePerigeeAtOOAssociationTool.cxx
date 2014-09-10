/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackParticlePerigeeAssociationTool.cxx 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/TrackParticleAtOOPerigeeAtOOAssociationTool.cxx
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Associate from a TrackParticle to its default Perigee.
 */

#include "TrackParticlePerigeeAtOOAssociationTool.h"

#include "Particle/TrackParticle.h"

namespace D3PD {

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrackParticlePerigeeAtOOAssociationTool::TrackParticlePerigeeAtOOAssociationTool
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
const Trk::Perigee*
TrackParticlePerigeeAtOOAssociationTool::get (const Rec::TrackParticle& track)
{
  return track.perigee();
}


/**
 * @brief Return the target object.
 * @param track The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Trk::Perigee*
TrackParticlePerigeeAtOOAssociationTool::get (const xAOD::TrackParticle& track)
{
  return &track.perigeeParameters();
}


} // namespace D3PD
