// -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSALGORITHMS_PIXELDETECTORELEMENTCONDALG_H
#define PIXELCONDITIONSALGORITHMS_PIXELDETECTORELEMENTCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"

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

  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

  StringProperty m_detManagerName{this, "DetManagerName", "Pixel", "Name of the DeterctorManager to retrieve"};
  const InDetDD::PixelDetectorManager* m_detManager{nullptr};
};

#endif // PIXELCONDITIONSALGORITHMS_PIXELDETECTORELEMENTCONDALG_H
