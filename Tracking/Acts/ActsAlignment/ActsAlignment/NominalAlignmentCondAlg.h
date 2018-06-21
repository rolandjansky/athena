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

#include "StoreGate/StoreGateSvc.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"

#include <string>



class NominalAlignmentCondAlg  :  public AthAlgorithm {
  
public:
    
  NominalAlignmentCondAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~NominalAlignmentCondAlg();
  
  virtual bool isClonable() const override { return true; }

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

private:
  
  SG::ReadHandleKey<EventInfo> m_evt {this,"EvtInfo", "McEventInfo", "EventInfo name"};

  SG::WriteCondHandleKey<GeoAlignmentStore> m_wchk {this, "PixelAlignmentKey", "PixelAlignment", "cond handle key"};

  ServiceHandle<ICondSvc> m_cs;

};

