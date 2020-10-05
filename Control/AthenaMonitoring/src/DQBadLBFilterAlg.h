/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQBadLBFILTERALG_H
#define DQBadLBFILTERALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/AttributeListSpecification.h"
#include "GaudiKernel/ICondSvc.h"

#include <string>
#include <vector>

// This filter tool rejects events where the a bad LB block is flagged from a given defect
// @author Michele Petteni <mpetteni@sfu.ca>

class DQBadLBFilterAlg :  public AthAlgorithm  {
 public:
  DQBadLBFilterAlg(const std::string&, ISvcLocator*);
        
  virtual ~DQBadLBFilterAlg () override;
  
  virtual StatusCode initialize() override;

  virtual StatusCode execute() override;

 private:
  bool m_ignoreRecoverable;
  std::vector<int> m_listofdefects;
  std::vector<std::string> m_listofdefects_str;
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey;
  SG::WriteCondHandleKey<AthenaAttributeList> m_writeKey;
  ServiceHandle<ICondSvc> m_condSvc;
  coral::AttributeListSpecification* m_attribListSpec;
};

#endif //DQATLASREADYFILTERALG_H
