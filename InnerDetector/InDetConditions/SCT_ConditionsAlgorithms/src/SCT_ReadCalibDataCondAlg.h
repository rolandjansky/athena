/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_ReadCalibDataCondAlg_h
#define SCT_ReadCalibDataCondAlg_h

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "SCT_ConditionsData/SCT_CalibDefectData.h"
#include "SCT_ConditionsData/SCT_AllGoodStripInfo.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

#include <map>
#include <vector>

// Forward declarations
class SCT_ID;

class SCT_ReadCalibDataCondAlg : public AthReentrantAlgorithm
{  
 public:
  SCT_ReadCalibDataCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_ReadCalibDataCondAlg() = default;
  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;

 private:
  enum Feature {GAIN=0, NOISE=1, NFEATURES=2};

  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyGain{this, "ReadKeyGain", "/SCT/DAQ/Calibration/NPtGainDefects", "Key of input (raw) gain defect conditions folder"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyNoise{this, "ReadKeyNoise", "/SCT/DAQ/Calibration/NoiseOccupancyDefects", "Key of input (raw) noise defect conditions folder"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
  SG::WriteCondHandleKey<SCT_CalibDefectData> m_writeKeyGain{this, "WriteKeyGain", "SCT_CalibDefectNPtGain", "Key of output (derived) gain defect conditions data"};
  SG::WriteCondHandleKey<SCT_CalibDefectData> m_writeKeyNoise{this, "WriteKeyNoise", "SCT_CalibDefectNPtNoise", "Key of output (derived) noise defect conditions data"};
  SG::WriteCondHandleKey<SCT_AllGoodStripInfo> m_writeKeyInfo{this, "WriteKeyInfo", "SCT_AllGoodStripInfo", "Key of output (derived) good strip information conditions data"};

  // Defect type map, contains Fit, NPtGain and No defects for now
  std::map<int, std::string> m_defectMapIntToString;
  // Arrays to hold defects to ignore/limits
  StringArrayProperty m_ignoreDefects{this, "IgnoreDefects", {}, "Defects to ignore"};
  FloatArrayProperty m_ignoreDefectParameters{this, "IgnoreDefectsParameters", {}, "Limit on defect to ignore parameters"};

  ServiceHandle<ICondSvc> m_condSvc;
  const SCT_ID* m_id_sct; //!< Handle to SCT ID helper
};

#endif // SCT_ReadCalibDataCondAlg_h
