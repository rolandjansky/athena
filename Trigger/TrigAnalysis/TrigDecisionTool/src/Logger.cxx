/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/getMessageSvc.h"

#include "TrigDecisionTool/Logger.h"

MsgStream* Trig::Logger::m_stream = 0;

Trig::Logger::Logger() 
  : m_name("TrigDecisionTool::?") {}

Trig::Logger::Logger(const std::string& name) 
  : m_name(name) {}


MsgStream& Trig::Logger::log() const {
  if (m_stream == 0 ) {
    m_stream = new MsgStream(Athena::getMessageSvc(), m_name);
    m_stream->setLevel(MSG::ERROR);
  }
  return *m_stream;
}

void Trig::Logger::updateMsgStream(MsgStream* log) {
  if (m_stream)
    delete m_stream;
  m_stream = log;
}

void Trig::Logger::setOutputLevel(int level) {
  log().setLevel(level);
}

