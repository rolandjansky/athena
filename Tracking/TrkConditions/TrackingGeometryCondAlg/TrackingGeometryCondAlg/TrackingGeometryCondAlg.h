/*
 *   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRACKINGGEOMETRYCONDALG_H
#define TRACKINGGEOMETRYCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "PersistentDataModel/AthenaAttributeList.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "TrkGeometry/TrackingGeometry.h"
#include "TrkDetDescrInterfaces/IGeometryBuilderCond.h"
#include "TrkDetDescrInterfaces/IGeometryProcessor.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/CLASS_DEF.h"

namespace Trk{
class TrackingGeometryCondAlg : public AthReentrantAlgorithm
{
public:
  TrackingGeometryCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrackingGeometryCondAlg() override = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override {return StatusCode::SUCCESS;}
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

private:

  /// Output conditions object.
  SG::WriteCondHandleKey<TrackingGeometry> m_trackingGeometryWriteKey{this, "TrackingGeometryWriteKey", "AlignedTrackingGeometry", "Key of output of TrackingGeometry for ID"};
  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};
  ToolHandle<Trk::IGeometryBuilderCond>           m_trackingGeometryBuilder {this, "GeometryBuilder", ""};
  ToolHandleArray<Trk::IGeometryProcessor>    m_geometryProcessors ;
};
}
#endif //TRACKINGGEOMETRYCONDALG_H
