// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackParticlePerigeeAtOOAssociationTool.h 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/TrackParticlePerigeeAtOOAssociationTool.h
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Associate from a TrackParticle to its default Perigee.
 */
#ifndef TRACKD3PDMAKER_TRACKPARTICLEPERIGEEATOOASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_TRACKPARTICLEPERIGEEATOOASSOCIATIONTOOL_H

#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "xAODTracking/TrackParticle.h"
#include "TrkParameters/TrackParameters.h"

namespace Rec {
  class TrackParticle;
}

namespace D3PD {

/**
 * @brief Associate from a TrackParticle to its default Perigee.
 */
class TrackParticlePerigeeAtOOAssociationTool
  : public SingleAssociationTool<Types<Rec::TrackParticle, xAOD::TrackParticle>, Trk::Perigee>
{
public:
  typedef SingleAssociationTool<Types<Rec::TrackParticle, xAOD::TrackParticle>, Trk::Perigee> Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrackParticlePerigeeAtOOAssociationTool (const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const Trk::Perigee* get (const Rec::TrackParticle& p) override;


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const Trk::Perigee* get (const xAOD::TrackParticle& p) override;
};


} // namespace D3PD



#endif // not TRACKD3PDMAKER_TRACKPARTICLEPERIGEEATOOASSOCIATIONTOOL_H
