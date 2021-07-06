/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

#include "Acts/Utilities/Logger.hpp"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/CommonMessaging.h"
#include "GaudiKernel/INamedInterface.h"

#include <memory>

#include <boost/optional.hpp>

class ActsAthenaPrintPolicy final : public Acts::Logging::OutputPrintPolicy
{
public:

  ActsAthenaPrintPolicy(std::shared_ptr<MsgStream> msg) : m_msg(msg) {}

  void
  flush(const Acts::Logging::Level& lvl, const std::string& input);

private:
  std::shared_ptr<MsgStream> m_msg;
};

class ActsAthenaFilterPolicy final : public Acts::Logging::OutputFilterPolicy {
public:
  ActsAthenaFilterPolicy(std::shared_ptr<MsgStream> msg) : m_msg(msg) {}

  //~AthenaFilterPolicy() = default;

  bool doPrint(const Acts::Logging::Level& lvl) const;

private:
  std::shared_ptr<MsgStream> m_msg;
};


std::unique_ptr<const Acts::Logger>
makeActsAthenaLogger(IMessageSvc *svc, const std::string& name, 
    int level, boost::optional<std::string> parent_name);

std::unique_ptr<const Acts::Logger>
makeActsAthenaLogger(const CommonMessagingBase* parent, const std::string& name);

std::unique_ptr<const Acts::Logger>
makeActsAthenaLogger(const CommonMessagingBase* parent, const std::string& name, 
    boost::optional<std::string> parent_name);

// problem: string literal does not play well with boost::optional
std::unique_ptr<const Acts::Logger>
makeActsAthenaLogger(const CommonMessagingBase* parent, const std::string& name, 
    const std::string& parent_name); 

