/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_ReadCalibChipNoiseCondAlg_h
#define SCT_ReadCalibChipNoiseCondAlg_h

// Include parent class
#include "AthenaBaseComps/AthAlgorithm.h"

// Include Gaudi classes
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

// Include Athena classes
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "SCT_ConditionsData/SCT_NoiseCalibData.h"

// Include boost stuff
#include "boost/array.hpp"

// Forward declarations
class SCT_ID;

class SCT_ReadCalibChipNoiseCondAlg : public AthAlgorithm 
{  
 public:
  SCT_ReadCalibChipNoiseCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_ReadCalibChipNoiseCondAlg() = default;
  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;

 private:
  void insertNoiseOccFolderData(SCT_ModuleNoiseCalibData& theseCalibData, const coral::AttributeList& folderData);

  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey;
  SG::WriteCondHandleKey<SCT_NoiseCalibData> m_writeKey;
  ServiceHandle<ICondSvc> m_condSvc;
  const SCT_ID* m_id_sct; //!< Handle to SCT ID helper
};

#endif // SCT_ReadCalibChipNoiseCondAlg_h
