/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LooperKiller_H
#define LooperKiller_H

#include "G4AtlasTools/UserActionBase.h"

#include <string>

class LooperKiller final: public UserActionBase {

  public:

  LooperKiller(const std::string& type, const std::string& name, const IInterface* parent);
  
  virtual void Step(const G4Step*) override;
  
  virtual StatusCode finalize() override;
  virtual StatusCode queryInterface(const InterfaceID&, void**) override;
  
 private:
  long m_max_steps;
  int m_print_steps;
  int m_count_steps;
  long m_killed_tracks;
  int m_verbose_level;
  bool m_abortEvent, m_setError;
  
};

#endif
