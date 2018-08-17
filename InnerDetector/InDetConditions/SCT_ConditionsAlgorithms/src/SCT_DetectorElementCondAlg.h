/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CONDITIONSALGORITHMS_SCT_DETECTORELEMENTCONDALG_H
#define SCT_CONDITIONSALGORITHMS_SCT_DETECTORELEMENTCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"

#include "GeoModelUtilities/GeoAlignmentStore.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

namespace InDetDD {
  class SCT_DetectorManager;
  class SiCommonItems;
}

class SCT_DetectorElementCondAlg : public AthAlgorithm
{
 public:
  SCT_DetectorElementCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_DetectorElementCondAlg() override = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

 private:
  SG::ReadCondHandleKey<GeoAlignmentStore> m_readKey;
  SG::WriteCondHandleKey<InDetDD::SiDetectorElementCollection> m_writeKey{this, "WriteKey", "SCT_DetectorElementCollection", "Key of output SiDetectorElementCollection for SCT"};

  ServiceHandle<ICondSvc> m_condSvc;
  const InDetDD::SCT_DetectorManager* m_detManager;
  InDetDD::SiCommonItems* m_commonItems;
};

#endif // SCT_CONDITIONSALGORITHMS_SCT_DETECTORELEMENTCONDALG_H
