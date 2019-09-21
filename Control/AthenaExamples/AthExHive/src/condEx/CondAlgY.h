/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONDALGS_CONDALGY_H
#define CONDALGS_CONDALGY_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "AthExHive/CondDataObjY.h"
#include "AthExHive/IASCIICondDbSvc.h"

#include "GaudiKernel/ICondSvc.h"

#include <string>

class CondAlgY  :  public AthAlgorithm {
  
public:
    
  CondAlgY (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CondAlgY();
  
  virtual bool isClonable() const override { return true; }

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
private:
  
  SG::WriteCondHandleKey<CondDataObjY> m_wch1 {this, "Key_CH1", "Y1", "cond handle key 1"};
  SG::WriteCondHandleKey<CondDataObjY> m_wch2 {this, "Key_CH2", "Y2", "cond handle key 2"};

  Gaudi::Property<std::string> m_dbk1 {this, "Key_DB1", "Y1", "explicit dbKey for cond handle 1"};
  Gaudi::Property<std::string> m_dbk2 {this, "Key_DB2", "Y2", "explicit dbKey for cond handle 2"};

  ServiceHandle<ICondSvc> m_cs;
  ServiceHandle<IASCIICondDbSvc> m_cds;

};

#endif
