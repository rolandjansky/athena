/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VALIDATIONEXCEPTION_H
#define VALIDATIONEXCEPTION_H

#include <string>

class ValidationException {
    std::string m_msg;
  public:
    ValidationException() { m_msg = ""; }
    ValidationException(const std::string &msg) { m_msg = msg; }
    const std::string &msg() const { return m_msg; }
};

#endif

