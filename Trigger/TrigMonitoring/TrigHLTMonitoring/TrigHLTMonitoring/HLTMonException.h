/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTMONEXCEPTION_H
#define HLTMONEXCEPTION_H

#include <string>

class HLTMonException {
  std::string m_message;

  public:
  HLTMonException();
  HLTMonException(const std::string &message);
  HLTMonException(const char *message);
  virtual ~HLTMonException();

  const std::string &msg() const;
};

#endif

