// -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSALGORITHMS_PIXELALIGNCONDALG_H
#define PIXELCONDITIONSALGORITHMS_PIXELALIGNCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"

#include "DetDescrConditions/AlignableTransformContainer.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"

namespace InDetDD {
  class PixelDetectorManager;
}

class PixelAlignCondAlg : public AthAlgorithm
{
 public:
  PixelAlignCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~PixelAlignCondAlg() override = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

 private:
  BooleanProperty m_useDynamicAlignFolders{
    this, "UseDynamicAlignFolders", false, "Flag of dynamic or static ID alignment folders"};
  SG::ReadCondHandleKey<AlignableTransformContainer> m_readKeyStatic{
    this, "ReadKeyStatic", "/Indet/Align", "Key for the static alignment folder"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyDynamicL1{
    this, "ReadKeyDynamicL1", "/Indet/AlignL1/ID", "Key for the dynamic L1 alignment folder"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyDynamicL2{
    this, "ReadKeyDynamicL2", "/Indet/AlignL2/PIX", "Key for the dynamic L2 alignment folder"};
  SG::ReadCondHandleKey<AlignableTransformContainer> m_readKeyDynamicL3{
    this, "ReadKeyDynamicL3", "/Indet/AlignL3", "Key for the dynamic L3 alignment folder"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyIBLDist{
    this, "ReadKeyIBLDist", "/Indet/IBLDist", "Key for the IBL alignment folder"};

  SG::WriteCondHandleKey<GeoAlignmentStore> m_writeKey{
    this, "WriteKey", "PixelAlignmentStore", "Output pixel alignment data"};

  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

  StringProperty m_detManagerName{this, "DetManagerName", "Pixel", "Name of the DeterctorManager to retrieve"};
  const InDetDD::PixelDetectorManager* m_detManager{nullptr};
};

#endif
