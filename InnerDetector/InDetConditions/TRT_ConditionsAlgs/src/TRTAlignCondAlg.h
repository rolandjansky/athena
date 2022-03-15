/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTCONDITIONSALGS_TRTALIGNCONDALG_H
#define TRTCONDITIONSALGS_TRTALIGNCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "TRT_ReadoutGeometry/TRT_DetElementCollection.h"
#include "TRT_ReadoutGeometry/TRT_DetElementContainer.h"

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
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyDynamicGlobal{this, "ReadKeyDynamicGlobal","/TRT/AlignL1/TRT","Read handle for global alignment conditions"};
  SG::ReadCondHandleKey<AlignableTransformContainer> m_readKeyDynamicRegular{this, "ReadKeyDynamicRegular","/TRT/AlignL2","Read handle for Dynamic Alignable transform containers"};
  SG::ReadCondHandleKey<AlignableTransformContainer> m_readKeyRegular{this, "ReadKeyRegular","/TRT/Align","Read handle for Static Alignable transform containers"};
  SG::WriteCondHandleKey<GeoAlignmentStore>  m_writeKeyAlignStore{this,"WriteKeyAlignStore","GeoAlignmentStore","Write handle key to store GeoAlignmentStore constants"};
  SG::WriteCondHandleKey<InDetDD::TRT_DetElementContainer> m_writeKeyDetElCont{this, "WriteKeyDetElCont", "TRT_DetElementContainer", "Key of output to store detector element structures for TRT"};

  const InDetDD::TRT_DetectorManager* m_detManager;

  Gaudi::Property<bool> m_useDynamicFolders{ this, "UseDynamicFolders", false, "Turn on-off use of Dynamic folders" };
};

#endif
