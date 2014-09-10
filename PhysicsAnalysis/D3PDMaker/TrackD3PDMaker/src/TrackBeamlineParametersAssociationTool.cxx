/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/TrackParametersBeamlineParametersAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2012
 * @brief Associate from a track to the corresponding parameters
 *        evaluated at the beamline.
 */


#include "TrackBeamlineParametersAssociationTool.h"
#include "Particle/TrackParticle.h"
#include "TrkTrack/Track.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrackBeamlineParametersAssociationTool::TrackBeamlineParametersAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_trackToVertexTool( "Reco::TrackToVertex")
{
  declareProperty ("TrackToVertexTool", m_trackToVertexTool,
                   "TrackToVertexTool instance.");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode TrackBeamlineParametersAssociationTool::initialize()
{
  CHECK( m_trackToVertexTool.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Trk::TrackParameters*
TrackBeamlineParametersAssociationTool::get (const Trk::Track& p)
{
  return m_trackToVertexTool->trackAtBeamline (p);
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Trk::TrackParameters*
TrackBeamlineParametersAssociationTool::get (const Rec::TrackParticle& p)
{
  return m_trackToVertexTool->trackAtBeamline (p);
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Trk::TrackParameters*
TrackBeamlineParametersAssociationTool::get (const Trk::TrackParameters& p)
{
  return m_trackToVertexTool->trackAtBeamline (p);
}


/**
 * @brief Release an object retrieved from the association.
 * @param p The object to release.
 *
 * Call this when you are done with the object returned by
 * @c get().  The default implementation is a no-op,
 * but if the association dynamically allocated the object which
 * it returned, this gives it a chance to free it.
 */
void
TrackBeamlineParametersAssociationTool::releaseObject
  (const Trk::TrackParameters* p)
{
  delete p;
}


} // namespace D3PD
