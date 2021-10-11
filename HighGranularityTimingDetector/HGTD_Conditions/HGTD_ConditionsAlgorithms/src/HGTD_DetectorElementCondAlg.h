// -*- C++ -*-
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HGTD_CONDITIONSALGORITHMS_HGTD_DETECTORELEMENTCONDALG_H
#define HGTD_CONDITIONSALGORITHMS_HGTD_DETECTORELEMENTCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/CondHandleKeyArray.h"

#include "GaudiKernel/ICondSvc.h"

//dependencies to limit lifetime of SiDetElColl for TrackingGeometry
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TRT_ReadoutGeometry/TRT_DetElementContainer.h"

class HGTD_DetectorManager;

class HGTD_DetectorElementCondAlg : public AthReentrantAlgorithm
{
 public:
  HGTD_DetectorElementCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HGTD_DetectorElementCondAlg() override = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

 private:
  SG::WriteCondHandleKey<InDetDD::HGTD_DetectorElementCollection> m_writeKey
  {this, "WriteKey", "HGTD_DetectorElementCollection", "Key of output HGTD_DetectorElementCollection for HGTD"};

  // The DetElement Collection must have a life time <= the Tracking Geometry due to DetElt-> Surface -> Layer connection,
  // which is why we intersect with the IOV Ranges from the TG's dependencies.
  SG::ReadCondHandleKeyArray<MuonGM::MuonDetectorManager> m_muonManagerKey
     {this, "MuonManagerKey", {}, "MuonManager ReadKey for IOV Range intersection"};
  SG::ReadCondHandleKeyArray<InDetDD::TRT_DetElementContainer> m_trtDetElContKey
     {this, "TRT_DetEltContKey", {}, "TRT ReadKey for IOV Range intersection"};
  SG::ReadCondHandleKeyArray<GeoAlignmentStore> m_pixelReadKey
     {this, "PixelAlignmentStore", {}, "PixelAlignmentStore ReadKey for IOV Range intersection"};
  SG::ReadCondHandleKeyArray<GeoAlignmentStore> m_SCT_readKey
     {this, "SCTAlignmentStore", {}, "SCTAlignmentStore ReadKey for IOV Range intersection "};

  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

  StringProperty m_detManagerName{this, "DetManagerName", "HGTD", "Name of the DeterctorManager to retrieve"};
  const HGTD_DetectorManager* m_detManager{nullptr};
};

#endif // HGTD_CONDITIONSALGORITHMS_HGTD_DETECTORELEMENTCONDALG_H
