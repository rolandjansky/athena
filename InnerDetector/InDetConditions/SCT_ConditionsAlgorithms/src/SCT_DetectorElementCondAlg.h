// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CONDITIONSALGORITHMS_SCT_DETECTORELEMENTCONDALG_H
#define SCT_CONDITIONSALGORITHMS_SCT_DETECTORELEMENTCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/CondHandleKeyArray.h"

#include "GaudiKernel/ICondSvc.h"

//dependencies to limit lifetime of SiDetElColl for TrackingGeometry
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TRT_ReadoutGeometry/TRT_DetElementContainer.h"

namespace InDetDD {
  class SCT_DetectorManager;
}

class SCT_DetectorElementCondAlg : public AthReentrantAlgorithm
{
 public:
  SCT_DetectorElementCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_DetectorElementCondAlg() override = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

 private:
  SG::ReadCondHandleKey<GeoAlignmentStore> m_readKey;
  SG::WriteCondHandleKey<InDetDD::SiDetectorElementCollection> m_writeKey{this, "WriteKey", "SCT_DetectorElementCollection", "Key of output SiDetectorElementCollection for SCT"};

  // The DetElement Collection must have a life time <= the Tracking Geometry due to DetElt-> Surface -> Layer connection,
  // which is why we intersect with the IOV Ranges from the TG's dependencies.
  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_muonManagerKey
    {this, "MuonManagerKey", "", "MuonManager ReadKey for IOV Range intersection"};
  SG::ReadCondHandleKey<InDetDD::TRT_DetElementContainer> m_trtDetElContKey
    {this, "TRT_DetEltContKey", "", "TRT ReadKey for IOV Range intersection"};
  SG::ReadCondHandleKey<GeoAlignmentStore> m_pixelReadKey
    {this, "PixelAlignmentStore", "", "PixelAlignmentStore ReadKey for IOV Range intersection"};

  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};
  std::string m_detManagerName;
  const InDetDD::SCT_DetectorManager* m_detManager{nullptr};
};

#endif // SCT_CONDITIONSALGORITHMS_SCT_DETECTORELEMENTCONDALG_H
