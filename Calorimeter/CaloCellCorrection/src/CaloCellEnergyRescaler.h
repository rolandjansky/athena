/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef CALOCELLENERGYRESCALER_H
#define CALOCELLENERGYRESCALER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "StoreGate/DataHandle.h"  
#include "AthenaKernel/IOVSvcDefs.h"
#include <string>

class CaloCellContainer;
class CaloCondBlobFlt;

/** @class CaloCellEnergyRescaler
    @brief CaloCellMakerTool to re-scale cell energies.
*/

class CaloCellEnergyRescaler : public AthAlgTool, virtual public ICaloCellMakerTool  {
public:
  
  CaloCellEnergyRescaler (const std::string& type, const std::string& name, 
			  const IInterface* parent);

  ~CaloCellEnergyRescaler();
  virtual StatusCode initialize(); 

  virtual StatusCode process( CaloCellContainer * theCellContainer) ;

 private: 
  /// IOV callback method
  StatusCode load(IOVSVC_CALLBACK_ARGS); 

  const DataHandle<AthenaAttributeList> m_attrList;
  const CaloCondBlobFlt* m_corrValues;

  std::string m_key;
};

#endif     
