/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTMonitoring/HLTMonException.h"

HLTMonException::HLTMonException() {
  m_message = "";
}

HLTMonException::HLTMonException(const std::string &message) {
  m_message = message;
}

HLTMonException::HLTMonException(const char *message) {
  m_message = message;
}

HLTMonException::~HLTMonException() {
}

const std::string &HLTMonException::msg() const {
  return m_message;
}

