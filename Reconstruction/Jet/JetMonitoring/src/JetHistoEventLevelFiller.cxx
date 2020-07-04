/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoringKernel/Monitored.h"
#include "JetMonitoring/JetHistoEventLevelFiller.h"
#include "JetMonitoring/JetMonitoringAlg.h"
#include "xAODEventInfo/EventInfo.h"

JetHistoEventLevelFiller::JetHistoEventLevelFiller( const std::string& type,  const std::string & name ,const IInterface* parent):
  AthAlgTool( type, name, parent )
 , m_var(this)

{
  declareInterface<IJetHistoFiller>(this);
  declareProperty("Var", m_var);
}

StatusCode JetHistoEventLevelFiller::processJetContainer(const JetMonitoringAlg& parentAlg, const xAOD::JetContainer & jets, const EventContext& ctx) const {

  auto eventInfo = parentAlg.GetEventInfo(ctx);
  Monitored::Scalar<float> s( m_var->varName() );
  s = m_var->value( *eventInfo, jets );
  parentAlg.fill(m_group, s );

  return StatusCode::SUCCESS;
}

