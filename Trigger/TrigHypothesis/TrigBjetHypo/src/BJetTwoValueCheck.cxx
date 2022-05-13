/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "BJetTwoValueCheck.h"
#include "AthenaMonitoringKernel/Monitored.h"

BJetTwoValueCheck::BJetTwoValueCheck(
  const std::string& type,
  const std::string& name,
  const IInterface* parent ) :
  AthAlgTool( type, name, parent ),
  m_acc(nullptr)
{
}

StatusCode BJetTwoValueCheck::initialize() {
  m_acc.reset(new Accessors {
      std::string(m_numName),
      std::string(m_denomName)});
  if ( not m_monTool.name().empty() ) {
    ATH_CHECK( m_monTool.retrieve() );
    ATH_MSG_DEBUG("MonTool name: " << m_monTool);
  }
  return StatusCode::SUCCESS;
}

bool BJetTwoValueCheck::passThreshold(const SG::AuxElement& btag) const
{
  float n = m_acc->n(btag);
  float d = m_acc->d(btag);
  float llr = std::log( n / d);
  Monitored::Group(m_monTool, Monitored::Scalar(m_llrName, llr));
  return llr > m_threshold;
}
