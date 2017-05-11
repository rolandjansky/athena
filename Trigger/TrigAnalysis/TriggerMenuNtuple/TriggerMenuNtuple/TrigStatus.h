/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUNTUPLE_TRIGSTATUS_H
#define TRIGGERMENUNTUPLE_TRIGSTATUS_H
/*
  TrigStatus.h
*/
#include <string>

class TrigStatus {
public:
  TrigStatus(const std::string& name="", int status=0);
  ~TrigStatus();

  std::string name() const { return m_name; }
  int status() const { return m_status; }
  bool isPassed() const { return (m_status != 0); }

  void setName(const std::string& name) { m_name = name; }
  void setStatus(int n) { m_status = n; }

protected:
  std::string m_name;
  int m_status;
  
};

#endif // TRIGGERMENUNTUPLE_TRIGSTATUS_H
