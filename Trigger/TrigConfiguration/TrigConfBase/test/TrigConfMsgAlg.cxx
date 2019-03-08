/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrigConfBase/TrigConfMessaging.h"

/**
 * TrigConfMessaging athena-based testing
 *
 * Simple testing algorithm to make sure that messages printed via
 * TrigConfMessaging are indeed output via the Gaudi MessageSvc.
 */

class MyClass : public TrigConf::TrigConfMessaging {
public:
  MyClass(const std::string& name) : TrigConf::TrigConfMessaging(name) {}

  void print() const {
    TRG_MSG_INFO("An INFO message");
    TRG_MSG_DEBUG("A DEBUG message (should not be visible by default)");
  }
};

class TrigConfMsgAlg : public AthAlgorithm {
public:
  TrigConfMsgAlg(const std::string& name, ISvcLocator* svcLoc) :
      AthAlgorithm(name, svcLoc),
      m_class(name)
  {}

  virtual StatusCode execute() override
  {
    ATH_MSG_INFO("Next message is printed via TrigConfMessaging:");
    m_class.print();
    return StatusCode::SUCCESS;
  }

private:
  MyClass m_class;
};

DECLARE_COMPONENT(TrigConfMsgAlg)
