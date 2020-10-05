/*
 *   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Trk includes
#include "AthenaKernel/IOVSvcDefs.h"

#include "TrkCondTest/TrackingGeometryCondAlgTest.h"


Trk::TrackingGeometryCondAlgTest::TrackingGeometryCondAlgTest(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode Trk::TrackingGeometryCondAlgTest::initialize()
{
  ATH_MSG_DEBUG("initialize " << name());

  // Read Handle Key
  ATH_CHECK(m_trackingGeometryReadKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode Trk::TrackingGeometryCondAlgTest::execute(const EventContext& ctx) const {
  
  //Set up read handle
  SG::ReadCondHandle<Trk::TrackingGeometry> readHandle{m_trackingGeometryReadKey, ctx};
  if (!readHandle.isValid() || *readHandle == nullptr) {
    ATH_MSG_WARNING(m_trackingGeometryReadKey.fullKey() << " is not available.");
    return StatusCode::FAILURE;
  }
  const Trk::TrackingGeometry* trkGeom = *readHandle;
  ATH_MSG_INFO( "eventID: "  << ctx.eventID());

  Amg::Vector3D center = trkGeom->highestTrackingVolume()->center();
  ATH_MSG_INFO("Center coordinates of highest tracking volume");
  ATH_MSG_INFO(center);
  if(m_worldVolumeCenterCollection.size()>=1 && m_worldVolumeCenterCollection.back() != center){
    ATH_MSG_INFO("hooray, we have moved the world");
  }
  m_worldVolumeCenterCollection.push_back(center);

  ATH_MSG_INFO("TrackingGeometry retrieved");
  return StatusCode::SUCCESS;
}
