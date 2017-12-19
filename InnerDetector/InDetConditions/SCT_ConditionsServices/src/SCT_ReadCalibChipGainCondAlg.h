/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_ReadCalibChipGainCondAlg_h
#define SCT_ReadCalibChipGainCondAlg_h

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
#include "SCT_ConditionsData/SCT_GainCalibData.h"

// Include boost stuff
#include "boost/array.hpp"

// Forward declarations
class SCT_ID;

class SCT_ReadCalibChipGainCondAlg : public AthAlgorithm 
{  
 public:
  SCT_ReadCalibChipGainCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~SCT_ReadCalibChipGainCondAlg();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  void insertNptGainFolderData(SCT_ModuleGainCalibData& theseCalibData, const coral::AttributeList& folderData);

  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey;
  SG::WriteCondHandleKey<SCT_GainCalibData> m_writeKey;
  ServiceHandle<ICondSvc> m_condSvc;
  const SCT_ID* m_id_sct; //!< Handle to SCT ID helper
};

#endif // SCT_ReadCalibChipGainCondAlg_h
