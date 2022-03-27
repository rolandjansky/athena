/*
 *   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */


#include "TrkGeometry/TrackingGeometry.h"
#include "GaudiKernel/EventContext.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IOVInfiniteRange.h"
#include "TrackingGeometryCondAlg/TrackingGeometryCondAlg.h"

Trk::TrackingGeometryCondAlg::TrackingGeometryCondAlg(const std::string& name,
                                                      ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
}

StatusCode Trk::TrackingGeometryCondAlg::initialize()
{
  ATH_MSG_DEBUG("initialize " << name());

  // Write Handle
  ATH_CHECK(m_trackingGeometryWriteKey.initialize());
  // Retrieve tools
  ATH_CHECK(m_trackingGeometryBuilder.retrieve());
  if (m_geometryProcessors.retrieve().isFailure()){
    ATH_MSG_FATAL( "Could not retrieve " << m_geometryProcessors );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode Trk::TrackingGeometryCondAlg::execute(){
  //Set up write handle
  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::WriteCondHandle<Trk::TrackingGeometry> writeHandle{m_trackingGeometryWriteKey, ctx};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("Found valid write handle");
    return StatusCode::SUCCESS;
  }

  //Create dummy IOV range covering 0 - inf
  EventIDRange range = IOVInfiniteRange::infiniteMixed();

  std::pair<EventIDRange, const Trk::TrackingGeometry*> trackingGeometryPair =
    m_trackingGeometryBuilder->trackingGeometry(
      ctx, std::pair<EventIDRange, const Trk::TrackingVolume*>(range, nullptr));
  // cast constness away for StoreGate
  Trk::TrackingGeometry* trackingGeometry = const_cast<Trk::TrackingGeometry*>(trackingGeometryPair.second);

  // loop over the recursive geometry processors
  auto gpIter = m_geometryProcessors.begin();
  auto gpIterE = m_geometryProcessors.end();
  for (; gpIter != gpIterE; ++gpIter) {
    if ((*gpIter)->process(*trackingGeometry).isFailure()) {
      ATH_MSG_FATAL("Processing of TrackingGeometry did not succeed. Abort.");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_VERBOSE("Successfully processed the TrackingGeometry with "
                      << (*gpIter));
    }
  }
  ATH_CHECK(writeHandle.record(trackingGeometryPair.first, trackingGeometry));

  return StatusCode::SUCCESS;
}
