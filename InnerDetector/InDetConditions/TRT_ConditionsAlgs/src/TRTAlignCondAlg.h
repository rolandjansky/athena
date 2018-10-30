/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTCONDITIONSALGS_TRTALIGNCONDALG_H
#define TRTCONDITIONSALGS_TRTALIGNCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "DetDescrConditions/AlignableTransformContainer.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"

namespace InDetDD {
  class TRT_DetectorManager;
}

class TRTAlignCondAlg : public AthAlgorithm
{
 public:
  TRTAlignCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TRTAlignCondAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyDynamicGlobal;
  SG::ReadCondHandleKey<AlignableTransformContainer> m_readKeyDynamicRegular;
  SG::ReadCondHandleKey<AlignableTransformContainer> m_readKeyRegular;
  SG::WriteCondHandleKey<GeoAlignmentStore>  m_writeKey;

  ServiceHandle<ICondSvc> m_condSvc;
  const InDetDD::TRT_DetectorManager* m_detManager;

  bool m_useDynamicFolders;
};

#endif
