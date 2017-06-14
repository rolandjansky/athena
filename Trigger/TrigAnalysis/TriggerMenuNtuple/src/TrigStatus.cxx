/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TrigStatus.cxx
*/
#include "TriggerMenuNtuple/TrigStatus.h"

TrigStatus::TrigStatus(const std::string& name, int status) : 
  m_name(name), m_status(status) {
}

TrigStatus::~TrigStatus() {
}
