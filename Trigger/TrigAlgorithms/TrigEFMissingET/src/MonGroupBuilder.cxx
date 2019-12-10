/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @file MonGroupBuilder.cxx
 *
 * Implementation of monitoring group builder
 * @author Jon Burr
 *****************************************************************************/

#include "MonGroupBuilder.h"

namespace HLT { namespace MET {
  void MonGroupBuilder::add(Monitored::IMonitoredVariable& variable)
  {
    m_references.push_back(variable);
  }

  Monitored::Group MonGroupBuilder::build(
      const ToolHandle<GenericMonitoringTool>& tool) const
  {
    return Monitored::Group(tool, m_references);
  }

  void MonGroupBuilder::increaseCapacity(std::size_t value, bool owned)
  {
    m_references.reserve(value+m_references.capacity() );
    if (owned)
      m_ptrs.reserve(value+m_ptrs.capacity() );
  }
} } //> end namespace HLT::MET
