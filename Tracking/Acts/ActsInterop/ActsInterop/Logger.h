/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

#include "Acts/Utilities/Logger.hpp"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/CommonMessaging.h"
#include "GaudiKernel/INamedInterface.h"

#include <memory>

#include <boost/optional.hpp>

namespace Acts {

class AthenaPrintPolicy final : public Logging::OutputPrintPolicy
{
public:

  AthenaPrintPolicy(std::shared_ptr<MsgStream> msg) : m_msg(msg) {}

  void
  flush(const Logging::Level& lvl, const std::ostringstream& input);

private:
  std::shared_ptr<MsgStream> m_msg;
};

class AthenaFilterPolicy final : public Logging::OutputFilterPolicy {
public:
  AthenaFilterPolicy(std::shared_ptr<MsgStream> msg) : m_msg(msg) {}

  //~AthenaFilterPolicy() = default;

  bool doPrint(const Acts::Logging::Level& lvl) const;

private:
  std::shared_ptr<MsgStream> m_msg;
  MSG::Level m_currentLevel;
};


std::unique_ptr<const Logger>
makeAthenaLogger(IMessageSvc *svc, const std::string& name, int level, boost::optional<std::string> parent_name);

std::unique_ptr<const Logger>
makeAthenaLogger(CommonMessagingBase* parent, const std::string& name);

std::unique_ptr<const Logger>
makeAthenaLogger(CommonMessagingBase* parent, const std::string& name, boost::optional<std::string> parent_name);

// problem: string literal does not play well with boost::optional
std::unique_ptr<const Logger>
makeAthenaLogger(CommonMessagingBase* parent, const std::string& name, const std::string& parent_name); 

}  // end of namespace Acts
