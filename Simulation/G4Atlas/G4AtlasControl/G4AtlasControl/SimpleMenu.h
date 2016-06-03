/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASCONTROL_SIMPLEG4MENU
#define G4ATLASCONTROL_SIMPLEG4MENU

#include "GaudiKernel/MsgStream.h"
#include <string>

/// NEEDS DOCUMENTATION
class SimpleMenu {
 protected:
  MsgStream * m_log;
  std::string name;
  MsgStream log();
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) { return log().level() <= lvl; }
 public:
  SimpleMenu(const std::string n) : m_log(0),name(n) {}
  ~SimpleMenu() { if (m_log) delete m_log; }
};

#endif
