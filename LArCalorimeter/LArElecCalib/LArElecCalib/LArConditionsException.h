//Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARELECCALIB_LARCONDITONSECEPTION_H
#define LARELECCALIB_LARCONDITONSECEPTION_H

#include <string>

class LArConditionsException {

public:
  LArConditionsException() : m_message("No message") {};
  LArConditionsException(const std::string& message) : m_message(message) {};

  virtual ~LArConditionsException() {};

  virtual const std::string& what() const { return m_message;}
  virtual const std::string& message() const { return m_message;}
  virtual operator std::string() const { return m_message;}

 private:
  std::string m_message;

};


#endif
