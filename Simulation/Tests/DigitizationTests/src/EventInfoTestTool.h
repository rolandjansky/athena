/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_EVENTINFOTESTTOOL
#define G4AT_EVENTINFOTESTTOOL

#include "DigiTestToolBase.h"
#include "xAODEventInfo/EventInfo.h"

class EventInfoTestTool : public DigiTestToolBase {
 public:
  EventInfoTestTool(const std::string& name, const std::string& type,
                    const IInterface* parent);

  StatusCode initialize();

  StatusCode processEvent();

  StatusCode finalize();

 private:
  SG::ReadHandleKey<xAOD::EventInfo> m_rhkEventInfo{
      this, "EventInfoName", "EventInfo", "EventInfo name"};
};

#endif
