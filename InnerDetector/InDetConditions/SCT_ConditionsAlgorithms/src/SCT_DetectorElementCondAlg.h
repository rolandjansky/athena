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

#include "GaudiKernel/ICondSvc.h"

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

  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};
  const InDetDD::SCT_DetectorManager* m_detManager{nullptr};
};

#endif // SCT_CONDITIONSALGORITHMS_SCT_DETECTORELEMENTCONDALG_H
