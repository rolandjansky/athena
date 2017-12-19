/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_ReadCalibDataCondAlg_h
#define SCT_ReadCalibDataCondAlg_h

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "SCT_ConditionsData/SCT_CalibDefectData.h"
#include "SCT_ConditionsData/SCT_AllGoodStripInfo.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

#include <map>
#include <vector>

// Forward declarations
class SCT_ID;
namespace InDetDD{ class SCT_DetectorManager; }

class SCT_ReadCalibDataCondAlg : public AthAlgorithm 
{  
 public:
  SCT_ReadCalibDataCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~SCT_ReadCalibDataCondAlg();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  enum Feature {GAIN=0, NOISE=1, NFEATURES=2};

  // Flag to set true to be able to use all methods not just isGood
  bool m_recoOnly;

  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyGain;
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyNoise;
  SG::WriteCondHandleKey<SCT_CalibDefectData> m_writeKeyGain;
  SG::WriteCondHandleKey<SCT_CalibDefectData> m_writeKeyNoise;
  SG::WriteCondHandleKey<SCT_AllGoodStripInfo> m_writeKeyInfo;

  // Defect type map, contains Fit, NPtGain and No defects for now
  std::map<int, std::string> m_defectMapIntToString;
  // Arrays to hold defects to ignore/limits
  std::vector<std::string> m_ignoreDefects;
  std::vector<float> m_ignoreDefectParameters;

  ServiceHandle<ICondSvc> m_condSvc;
  const SCT_ID* m_id_sct; //!< Handle to SCT ID helper
  const InDetDD::SCT_DetectorManager* m_SCTdetMgr; //!< Handle to SCT detector manager
};

#endif // SCT_ReadCalibDataCondAlg_h
