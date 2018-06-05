/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "ActsAlignment/ShiftCondObj.h"
#include "AthExHive/IASCIICondDbSvc.h"

#include "EventInfo/EventInfo.h"
#include "GaudiKernel/ICondSvc.h"

#include <string>

class GeomShiftCondAlg  :  public AthAlgorithm {
  
public:
    
  GeomShiftCondAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~GeomShiftCondAlg();
  
  virtual bool isClonable() const override { return true; }

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

private:
  
  SG::ReadHandleKey<EventInfo> m_evt {this,"EvtInfo", "McEventInfo", "EventInfo name"};

  SG::WriteCondHandleKey<ShiftCondObj> m_wchk {this, "Key_CH", "X2", "cond handle key"};

  Gaudi::Property<std::string> m_dbKey {this, "Key_DB", "X2", "explicit dbKey for cond handle"};

  ServiceHandle<ICondSvc> m_cs;
  //ServiceHandle<IASCIICondDbSvc> m_cds;


};

