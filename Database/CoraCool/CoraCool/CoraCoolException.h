/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CORACOOL_CORACOOLEXCEPTION_H
#define CORACOOL_CORACOOLEXCEPTION_H
// CoraCool exception base class - simply derive from cool::Exception
// so clients can catch cool::Exception and get both cool & CoraCool exceptions
// Richard Hawkings, started 10/2006

#include "CoolKernel/Exception.h"

class CoraCoolException : public cool::Exception {
 public:
  CoraCoolException(const std::string& message, const std::string& domain) :
    cool::Exception(message,domain) {};

  virtual ~CoraCoolException() throw() {};
};

#endif // CORACOOL_CORACOOLEXCEPTION_H
