/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONDALGS_ALGA_H
#define CONDALGS_ALGA_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthExHive/HiveDataObj.h"

#include "xAODEventInfo/EventInfo.h"

#include <string>

class AlgA  :  public AthAlgorithm {
  
public:
  
  AlgA (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~AlgA ();

  virtual bool isClonable() const override { return true; }
  
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
private:

  SG::WriteHandleKey<HiveDataObj> m_wrh1 {this, "Key_W1", "a1", "write key 1"};
  SG::WriteHandleKey<HiveDataObj> m_wrh2 {this, "Key_W2", "a2", "write key 2"};
  SG::ReadHandleKey<xAOD::EventInfo>    m_evt  {this, "EvtInfo", "EventInfo", "EventInfo name"};
};
#endif
