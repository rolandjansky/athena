/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCTSIPROPERTIESTESTALG
#define SCTSIPROPERTIESTESTALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "SiPropertiesTool/ISiPropertiesTool.h"

#include "GaudiKernel/ToolHandle.h"

class SCT_ID;

class SCTSiPropertiesTestAlg : public AthReentrantAlgorithm 
{  
 public:
  SCTSiPropertiesTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCTSiPropertiesTestAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

 private:
  ToolHandle<ISiPropertiesTool> m_tool{this, "SCTPropertiesTool", "SCTPropertiesTool", "Tool to retrieve SCT Properties"};
  const SCT_ID* m_id;
};

#endif // SCTSIPROPERTIESTESTALG
