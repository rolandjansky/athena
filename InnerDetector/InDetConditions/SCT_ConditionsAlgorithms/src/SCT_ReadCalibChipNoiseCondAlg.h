/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_ReadCalibChipNoiseCondAlg_h
#define SCT_ReadCalibChipNoiseCondAlg_h

// Include parent class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// Include Athena classes
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "SCT_ConditionsData/SCT_NoiseCalibData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

// Include Gaudi classes
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

// Include boost stuff
#include "boost/array.hpp"

// Forward declarations
class SCT_ID;

class SCT_ReadCalibChipNoiseCondAlg : public AthReentrantAlgorithm 
{  
 public:
  SCT_ReadCalibChipNoiseCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_ReadCalibChipNoiseCondAlg() = default;
  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;

 private:
  void insertNoiseOccFolderData(SCT_ModuleNoiseCalibData& theseCalibData, const coral::AttributeList& folderData) const;

  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/SCT/DAQ/Calibration/ChipNoise", "Key of input (raw) noise conditions folder"};
  SG::WriteCondHandleKey<SCT_NoiseCalibData> m_writeKey{this, "WriteKey", "SCT_NoiseCalibData", "Key of output (derived) noise conditions data"};
  ServiceHandle<ICondSvc> m_condSvc;
  const SCT_ID* m_id_sct; //!< Handle to SCT ID helper
};

#endif // SCT_ReadCalibChipNoiseCondAlg_h
