/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef CALOCELLTIMECORRTOOL
#define CALOCELLTIMECORRTOOL

#include "CaloUtils/CaloCellCorrection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "StoreGate/DataHandle.h"  
#include "AthenaKernel/IOVSvcDefs.h"
#include <string>

class CaloCell;
class CaloCondBlobFlt;

/** @class CaloCellTimeCorrTool
    @brief Tool to shift cell time values
*/

class CaloCellTimeCorrTool : public CaloCellCorrection {
public:
  
  CaloCellTimeCorrTool (const std::string& type, const std::string& name, 
			const IInterface* parent);

  ~CaloCellTimeCorrTool();
  virtual StatusCode initialize(); 

  void MakeCorrection(CaloCell* theCell);    

 private: 
  /// IOV callback method
  StatusCode load(IOVSVC_CALLBACK_ARGS); 

  const DataHandle<AthenaAttributeList> m_attrList;
  const CaloCondBlobFlt* m_corrValues;

  std::string m_key;
};

#endif     
