/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "StaticPEBInfoWriterTool.h"

// =============================================================================

StaticPEBInfoWriterTool::StaticPEBInfoWriterTool(const std::string& type, const std::string& name, const IInterface* parent)
: PEBInfoWriterToolBase(type,name,parent) {}

// =============================================================================

StaticPEBInfoWriterTool::~StaticPEBInfoWriterTool() {}

// =============================================================================

StatusCode StaticPEBInfoWriterTool::initialize() {
  m_pebi.robs.insert(m_robList.begin(), m_robList.end());
  m_pebi.subdets.insert(m_subDetList.begin(), m_subDetList.end());
  ATH_MSG_DEBUG("Configured static PEBInfo: " << m_pebi);
  return StatusCode::SUCCESS;
}

// =============================================================================

PEBInfoWriterToolBase::PEBInfo StaticPEBInfoWriterTool::createPEBInfo(const PEBInfoWriterToolBase::Input& /*input*/) const {
  ATH_MSG_DEBUG("Returning static PEBInfo configured in initialize()");
  return m_pebi;
}
