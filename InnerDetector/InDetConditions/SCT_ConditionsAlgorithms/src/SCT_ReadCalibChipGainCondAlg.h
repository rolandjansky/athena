/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_ReadCalibChipGainCondAlg_h
#define SCT_ReadCalibChipGainCondAlg_h

// Include parent class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// Include Gaudi classes
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

// Include Athena classes
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "SCT_ConditionsData/SCT_GainCalibData.h"

// Include boost stuff
#include "boost/array.hpp"

// Forward declarations
class SCT_ID;

class SCT_ReadCalibChipGainCondAlg : public AthReentrantAlgorithm 
{  
 public:
  SCT_ReadCalibChipGainCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_ReadCalibChipGainCondAlg() = default;
  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;

 private:
  void insertNptGainFolderData(SCT_ModuleGainCalibData& theseCalibData, const coral::AttributeList& folderData) const;

  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/SCT/DAQ/Calibration/ChipGain", "Key of input (raw) gain conditions folder"};
  SG::WriteCondHandleKey<SCT_GainCalibData> m_writeKey{this, "WriteKey", "SCT_GainCalibData", "Key of output (derived) gain conditions data"};
  ServiceHandle<ICondSvc> m_condSvc;
  const SCT_ID* m_id_sct; //!< Handle to SCT ID helper
};

#endif // SCT_ReadCalibChipGainCondAlg_h
