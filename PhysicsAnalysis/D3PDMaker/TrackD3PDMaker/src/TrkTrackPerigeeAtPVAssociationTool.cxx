/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrkTrackPerigeeAtPVAssociationTool.cxx 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/TrkTrackPerigeeAtPVAssociationTool.cxx
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Associate from a TrkTrack to its perigee at PV.
 */

#include "TrkTrackPerigeeAtPVAssociationTool.h"

#include "TrkTrack/Track.h"
#include "AthenaKernel/errorcheck.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxContainer.h"

namespace D3PD {

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrkTrackPerigeeAtPVAssociationTool::TrkTrackPerigeeAtPVAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_trackToVertexTool("Reco::TrackToVertex")
{
  declareProperty ("SGKey", m_vxCandidate = "VxPrimaryCandidate");
  declareProperty ("TrackToVertexTool", m_trackToVertexTool);
}

StatusCode TrkTrackPerigeeAtPVAssociationTool::initialize()
{
  CHECK( Base::initialize() );
  CHECK( m_trackToVertexTool.retrieve() );
  return StatusCode::SUCCESS;
}

/**
 * @brief Return the target object.
 * @param track The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Trk::TrackParameters *TrkTrackPerigeeAtPVAssociationTool::get (const Trk::Track& track)
{

  const VxContainer* vxContainer = 0;
  StatusCode sc = evtStore()->retrieve(vxContainer, m_vxCandidate);
  if (sc.isFailure() || !vxContainer) {
    REPORT_MESSAGE (MSG::WARNING) << "Could not retrieve primary vertex container: " << m_vxCandidate;
    return 0;
  }

  if(vxContainer->size()<1){
    REPORT_MESSAGE (MSG::WARNING) << "No primary vertices reconstructed";
    return 0;
  }

  VxContainer::const_iterator vxI = vxContainer->begin();
  const Trk::TrackParameters* perigee = m_trackToVertexTool->perigeeAtVertex(track, (*vxI)->recVertex().position());

  return perigee;
}

/**
 * @brief Release an object retrieved from the association.
 * @param p The object to release.
 *
 * Call this when you are done with the object returned by
 * @c get().  The default implementation is a no-op,
 * but since the association dynamically allocates the object which
 * it returned, this gives it a chance to free it.
 */
void TrkTrackPerigeeAtPVAssociationTool::releaseObject (const Trk::TrackParameters* p)
{
  if(p) delete p;
}

} // namespace D3PD
