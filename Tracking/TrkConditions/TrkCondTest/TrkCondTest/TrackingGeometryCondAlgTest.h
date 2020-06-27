/*
 *   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKINGGEOMETRYCONDALG_H
#define TRACKINGGEOMETRYCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "PersistentDataModel/AthenaAttributeList.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "TrkGeometry/TrackingGeometry.h"
#include "TrkDetDescrInterfaces/IGeometryBuilder.h"
#include "TrkDetDescrInterfaces/IGeometryProcessor.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/CLASS_DEF.h"

namespace Trk{
class TrackingGeometryCondAlgTest : public AthReentrantAlgorithm
{
public:
    TrackingGeometryCondAlgTest(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~TrackingGeometryCondAlgTest() override = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx)const override;
    virtual StatusCode finalize() override {return StatusCode::SUCCESS;}

private:

  /// Input conditions object.
  SG::ReadCondHandleKey<TrackingGeometry> m_trackingGeometryReadKey{this, "TrackingGeometryReadKey", "AlignedTrackingGeometry", "Key of input TrackingGeometry"};
  mutable std::vector<Amg::Vector3D> m_worldVolumeCenterCollection;
};
}
#endif //TRACKINGGEOMETRYCONDALG_H
