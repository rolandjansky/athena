// -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSALGORITHMS_PIXELDETECTORELEMENTCONDALG_H
#define PIXELCONDITIONSALGORITHMS_PIXELDETECTORELEMENTCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/CondHandleKeyArray.h"

//dependencies to limit lifetime of SiDetElColl for TrackingGeometry
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TRT_ReadoutGeometry/TRT_DetElementContainer.h"

namespace InDetDD {
  class PixelDetectorManager;
}

class PixelDetectorElementCondAlg : public AthReentrantAlgorithm
{
 public:
  PixelDetectorElementCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~PixelDetectorElementCondAlg() override = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

 private:
  SG::ReadCondHandleKey<GeoAlignmentStore> m_readKey
  {this, "PixelAlignmentStore", "PixelAlignmentStore", "PixelAlignmentStore data"};
 
  SG::WriteCondHandleKey<InDetDD::SiDetectorElementCollection> m_writeKey
  {this, "WriteKey", "PixelDetectorElementCollection", "Key of output SiDetectorElementCollection for Pixel"};

  // The DetElement Collection must have a life time <= the Tracking Geometry due to DetElt-> Surface -> Layer connection,
  // which is why we intersect with the IOV Ranges from the TG's dependencies.
  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_muonManagerKey
    {this, "MuonManagerKey", "", "MuonManager ReadKey for IOV Range intersection"};
  SG::ReadCondHandleKey<InDetDD::TRT_DetElementContainer> m_trtDetElContKey
    {this, "TRT_DetEltContKey", "", "TRT ReadKey for IOV Range intersection"};
  SG::ReadCondHandleKey<GeoAlignmentStore> m_SCT_readKey
    {this, "SCTAlignmentStore", "", "SCTAlignmentStore ReadKey for IOV Range intersection "};

  StringProperty m_detManagerName{this, "DetManagerName", "Pixel", "Name of the DeterctorManager to retrieve"};
  const InDetDD::PixelDetectorManager* m_detManager{nullptr};
};

#endif // PIXELCONDITIONSALGORITHMS_PIXELDETECTORELEMENTCONDALG_H
