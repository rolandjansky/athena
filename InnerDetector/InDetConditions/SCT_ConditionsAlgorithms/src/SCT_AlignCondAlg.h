/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CONDITIONSALGORITHMS_SCT_ALIGNCONDALG_H
#define SCT_CONDITIONSALGORITHMS_SCT_ALIGNCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"

#include "DetDescrConditions/AlignableTransformContainer.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"

namespace InDetDD {
  class SCT_DetectorManager;
}

// SCT_AlignCondAlg cannot inherit AthReentrantAlgorithm.
// SCT_AlignCondAlg::execute uses the following methods.
//  InDetDD::InDetDetectorManager::align
//   InDetDD::InDetDetectorManager::processAlignmentContainer
//    InDetDD::InDetDetectorManager::processKey
//     InDetDD::SCT_DetectorManager::setAlignableTransformDelta
//      InDetDD::SiDetectorElement::defModuleTransform
//       InDetDD::SiDetectorElement::defTransform
//        InDetDD::SiDetectorElement::defTransformCLHEP
//         GeoVFullPhysVol::getDefAbsoluteTransform
// GeoVFullPhysVol::getDefAbsoluteTransform is used without argument.
// To be thread-safe, we need to pass non-const GeoVAlignmentStore pointer.
// However, we cannot give non-const pointer for SiDetectorElement
// in SCT_DetectorManager in the above chain.

class SCT_AlignCondAlg : public AthAlgorithm
{
 public:
  SCT_AlignCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_AlignCondAlg() override = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

 private:
  BooleanProperty m_useDynamicAlignFolders{this, "UseDynamicAlignFolders", false, "Flag of dynamic or static ID alignment folders"};
  SG::ReadCondHandleKey<AlignableTransformContainer> m_readKeyStatic{this, "ReadKeyStatic", "/Indet/Align", "Key for the static alignment folder"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyDynamicL1{this, "ReadKeyDynamicL1", "/Indet/AlignL1/ID", "Key for the dynamic L1 alignment folder"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyDynamicL2{this, "ReadKeyDynamicL2", "/Indet/AlignL2/SCT", "Key for the dynamic L2 alignment folder"};
  SG::ReadCondHandleKey<AlignableTransformContainer> m_readKeyDynamicL3{this, "ReadKeyDynamicL3", "/Indet/AlignL3", "Key for the dynamic L3 alignment folder"};
  SG::WriteCondHandleKey<GeoAlignmentStore> m_writeKey;

  ServiceHandle<ICondSvc> m_condSvc;
  const InDetDD::SCT_DetectorManager* m_detManager;
};

#endif // SCT_CONDITIONSALGORITHMS_SCT_ALIGNCONDALG_H
