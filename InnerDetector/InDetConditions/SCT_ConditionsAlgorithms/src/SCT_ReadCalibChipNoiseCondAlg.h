// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "Gaudi/Property.h"

// Forward declarations
class SCT_ID;

class SCT_ReadCalibChipNoiseCondAlg : public AthReentrantAlgorithm 
{  
 public:
  SCT_ReadCalibChipNoiseCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_ReadCalibChipNoiseCondAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

 private:
  void insertNoiseOccFolderData(SCT_ModuleNoiseCalibData& theseCalibData, const coral::AttributeList& folderData) const;

  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/SCT/DAQ/Calibration/ChipNoise", "Key of input (raw) noise conditions folder"};
  SG::WriteCondHandleKey<SCT_NoiseCalibData> m_writeKey{this, "WriteKey", "SCT_NoiseCalibData", "Key of output (derived) noise conditions data"};
  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};
  const SCT_ID* m_id_sct{nullptr}; //!< Handle to SCT ID helper
};

#endif // SCT_ReadCalibChipNoiseCondAlg_h
