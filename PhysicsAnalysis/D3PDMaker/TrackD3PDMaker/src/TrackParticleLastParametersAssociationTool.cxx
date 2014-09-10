/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/TrackParticleLastParametersAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2012
 * @brief Associate from a track particle to the last set of parameters.
 */


#include "TrackParticleLastParametersAssociationTool.h"
#include "Particle/TrackParticle.h"
#include "TrkParameters/TrackParameters.h"
#include "AthenaKernel/errorcheck.h"
#include "boost/foreach.hpp"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrackParticleLastParametersAssociationTool::TrackParticleLastParametersAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Trk::TrackParameters*
TrackParticleLastParametersAssociationTool::get (const Rec::TrackParticle& p)
{
  const Trk::TrackParameters* lastValidTrkParametersBase = 0;
  double radius = 0;

  BOOST_FOREACH (const Trk::TrackParameters* base, p.trackParameters()) {
    double tempR =  base->position().perp();
    if( tempR > radius  ) {
      lastValidTrkParametersBase = base;
      radius = tempR;
    }
  }

  return lastValidTrkParametersBase;
}


} // namespace D3PD
