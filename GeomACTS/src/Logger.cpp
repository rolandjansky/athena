// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "AthenaKernel/getMessageSvc.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "ACTS/Utilities/Logger.hpp"

#include <iostream>

namespace Acts {

class AthenaPrintPolicy : public Logging::OutputPrintPolicy
{
public:

  AthenaPrintPolicy(AthMessaging* athMsg) : m_msg(athMsg->msg())
  {

  }

  void
  flush(const Logging::Level& lvl, const std::ostringstream& input)
  {
    const std::vector<MSG::Level> athLevelVector{
      MSG::VERBOSE, 
      MSG::DEBUG,
      MSG::INFO, 
      MSG::WARNING, 
      MSG::ERROR, 
      MSG::FATAL
    };

    MSG::Level athLevel = athLevelVector[lvl];
    m_msg << athLevel << input.str() << endmsg;
  }

private:
  MsgStream m_msg;
};

class AthenaFilterPolicy : public Logging::OutputFilterPolicy {
public:
  AthenaFilterPolicy(AthMessaging* athMsg) : m_athMsg(athMsg) {}

  bool doPrint(const Acts::Logging::Level& lvl) const {
    const std::vector<MSG::Level> athLevelVector{
      MSG::VERBOSE, 
      MSG::DEBUG,
      MSG::INFO, 
      MSG::WARNING, 
      MSG::ERROR, 
      MSG::FATAL
    };

    MSG::Level l = athLevelVector[lvl];
    return m_athMsg->msgLvl(l);
  }

private:
  AthMessaging *m_athMsg;
  MSG::Level m_currentLevel;
};

std::unique_ptr<const Logger>
getDefaultLogger(const std::string&    name,
                 const Logging::Level& /*lvl*/,
                 std::ostream*         /*log_stream*/)
{
  using namespace Logging;

  AthMessaging *athMsg = new AthMessaging(Athena::getMessageSvc(), name);

  auto filter = std::make_unique<AthenaFilterPolicy>(athMsg);
  auto print = std::make_unique<AthenaPrintPolicy>(athMsg);
  return std::make_unique<const Logger>(std::move(print), std::move(filter));
}
}  // end of namespace Acts
