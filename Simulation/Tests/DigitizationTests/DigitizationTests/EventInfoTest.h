/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DIGITIZATIONTESTS_EVENTINFOTEST_H
#define DIGITIZATIONTESTS_EVENTINFOTEST_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODEventInfo/EventInfo.h"

class EventInfoTest : public AthAlgorithm {
 public:
  EventInfoTest(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();

 private:
  // SG::ReadHandleKey<xAOD::EventInfo> m_rhkEventInfo;
  SG::ReadHandleKey<xAOD::EventInfo> m_rhkEventInfo{
      this, "EventInfoName", "EventInfo", "EventInfo name"};
};

#endif
