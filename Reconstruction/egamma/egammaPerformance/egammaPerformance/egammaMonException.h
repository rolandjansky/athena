/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef egammaMONEXCEPTION_H
#define egammaMONEXCEPTION_H

#include <string>

class egammaMonException {
  std::string m_message;

  public:
  egammaMonException();
  egammaMonException(const std::string &message);
  egammaMonException(const char *message);
  virtual ~egammaMonException();

  const std::string &msg() const;
};

#endif

