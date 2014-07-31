/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaPerformance/egammaMonException.h"

egammaMonException::egammaMonException() {
  m_message = "";
}

egammaMonException::egammaMonException(const std::string &message) {
  m_message = message;
}

egammaMonException::egammaMonException(const char *message) {
  m_message = message;
}

egammaMonException::~egammaMonException() {
}

const std::string &egammaMonException::msg() const {
  return m_message;
}

