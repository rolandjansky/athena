/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigDecisionTool_Logger_h
#define TrigDecisionTool_Logger_h

#include "AsgTools/AsgTool.h"
#include "CxxUtils/checker_macros.h"
#include <stdexcept>

namespace Trig{
  /**
   * Logging adaptor to provide interfaces required for ATH_MSG macros.
   *
   * TrigDecisionTool helper classes derive from this class to be able to use the usual
   * ATH_MSG macros in their code. One has to ensure that one class in the hierarchy
   * initializes the (static) Logger via the non-default constructor. All messaging
   * then occurs via the actual messaging implementation of that class.
   *
   * Note that while we are using asg::AsgTool as the logger type, in a full athena
   * release, this is a direct descendent of AthAlgTool.
   */
  class Logger {
  public:
    Logger() = default;
    Logger(asg::AsgTool* logger) { s_logger = logger; }

    MsgStream& msg() const {
      if (s_logger) return s_logger->msg();
      else throw std::runtime_error("TrigDecisionTool Logger not initialized.");
    }
    MsgStream& msg(const MSG::Level lvl) const { return msg() << lvl; }
    bool msgLvl(const MSG::Level lvl) const { return s_logger && s_logger->msgLvl(lvl); }

  private:
    // we rely on the AsgTool logging being thread-safe
    inline static asg::AsgTool* s_logger ATLAS_THREAD_SAFE {nullptr};
  };
}

#endif
