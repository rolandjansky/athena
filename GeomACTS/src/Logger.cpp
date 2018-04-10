// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "GeomACTS/Logger.h"

#include "AthenaKernel/getMessageSvc.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/SmartIF.h"

#include "ACTS/Utilities/Logger.hpp"

#include <iostream>
#include <string>

void
Acts::AthenaPrintPolicy::flush(const Logging::Level& lvl, const std::ostringstream& input)
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
  (*p_msg) << athLevel << input.str() << endmsg;
}
  

bool
Acts::AthenaFilterPolicy::doPrint(const Acts::Logging::Level& lvl) const 
{
  const std::array<MSG::Level, 6> athLevelVector{
    MSG::VERBOSE, 
    MSG::DEBUG,
    MSG::INFO, 
    MSG::WARNING, 
    MSG::ERROR, 
    MSG::FATAL
  };

  MSG::Level athLevel = athLevelVector[lvl];
  return p_msg->level() <= athLevel;
}

std::unique_ptr<const Acts::Logger>
Acts::getDefaultLogger(const std::string&    name,
                 const Logging::Level& /*lvl*/,
                 std::ostream*         /*log_stream*/)
{
  //throw std::runtime_error(std::string(__FUNCTION__) + " for " + name + " is not supported");;
  AthMessaging *athMsg = new AthMessaging(Athena::getMessageSvc(), name);
  
  std::shared_ptr<MsgStream> msg(&athMsg->msg());

  auto filter = std::make_unique<AthenaFilterPolicy>(msg);
  auto print = std::make_unique<AthenaPrintPolicy>(msg);
  return std::make_unique<const Logger>(std::move(print), std::move(filter));
}


std::unique_ptr<const Acts::Logger>
Acts::makeAthenaLogger(MsgStream& msg_) {

  using namespace Logging;
  auto msg = std::make_shared<MsgStream>(msg_);
  auto filter = std::make_unique<AthenaFilterPolicy>(msg);
  auto print = std::make_unique<AthenaPrintPolicy>(msg);
  return std::make_unique<const Logger>(std::move(print), std::move(filter));
}

std::unique_ptr<const Acts::Logger>
Acts::makeAthenaLogger(IMessageSvc *svc, const std::string&parent_name, const std::string& name = "") {
  using namespace Logging;
  auto msg = std::make_shared<MsgStream>(svc, parent_name + "." + name);
  msg->setLevel(svc->outputLevel(parent_name));
  auto filter = std::make_unique<AthenaFilterPolicy>(msg);
  auto print = std::make_unique<AthenaPrintPolicy>(msg);
  return std::make_unique<const Logger>(std::move(print), std::move(filter));
}
