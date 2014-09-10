/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TSOSPixelClusterAssociationTool.cxx 569325 2013-11-08 11:27:17Z nstyles $
/**
 * @file TrackD3PDMaker/src/TSOSPixelClusterAssociationTool.cxx
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Associate from a pixel TrackStateOnSurface to a PixelCluster.
 */

#include "TSOSPixelClusterAssociationTool.h"

#include "AthenaKernel/errorcheck.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

namespace D3PD {

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TSOSPixelClusterAssociationTool::TSOSPixelClusterAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_idHelper(0)
{
}

StatusCode TSOSPixelClusterAssociationTool::initialize(){

  CHECK( Base::initialize() );

  // need Atlas id-helpers to identify sub-detectors, take them from detStore
  CHECK (detStore()->retrieve(m_idHelper, "AtlasID"));

  return StatusCode::SUCCESS;
}

/**
 * @brief Return the target object.
 * @param track The source object for the association.
 * Return the target of the association, or 0.
 */
  const InDet::PixelCluster *TSOSPixelClusterAssociationTool::get (const Trk::TrackStateOnSurface& tsos)
{

  /// check if the TSOS is a pixel hit
  bool isPixelHit = false;
  if( tsos.types()[Trk::TrackStateOnSurface::Measurement] ||
      tsos.types()[Trk::TrackStateOnSurface::Outlier]){

    const Trk::TrackParameters *tp = tsos.trackParameters();
    if(tp){
      const Trk::Surface& surface = tp->associatedSurface();
      const Trk::TrkDetElementBase* detElement = surface.associatedDetectorElement();
      if(detElement){
        Identifier Id = detElement->identify();
        if (m_idHelper->is_pixel(Id) ) {
          isPixelHit = true;
        }
      }
    }
  }
  if(!isPixelHit){
    return 0;
  }

  const Trk::MeasurementBase *measurement = tsos.measurementOnTrack();
  const Trk::RIO_OnTrack *rio = dynamic_cast<const Trk::RIO_OnTrack*>(measurement);
  if(!rio){
    const Trk::CompetingRIOsOnTrack *crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(measurement);
    if(crot){
      rio = &crot->rioOnTrack( crot->indexOfMaxAssignProb() );
    }
  }
  const InDet::PixelCluster *clus = 0;
  if(rio){
    clus = dynamic_cast<const InDet::PixelCluster*>(rio->prepRawData());
  }
  
  return clus;
}

} // namespace D3PD
