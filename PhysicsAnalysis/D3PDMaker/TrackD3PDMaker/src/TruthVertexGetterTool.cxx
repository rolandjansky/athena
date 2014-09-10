/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthVertexGetterTool.h"

namespace D3PD {

TruthVertexGetterTool::TruthVertexGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : TruthVertexGetterBase
         (type, name, parent)
{
  declareProperty("KeepOnlyPrimaryVertices", m_keepOnlyPrimaryVertices = true);
  declareProperty("KeepPileUpVertices", m_keepPileUpVertices = true);
}

StatusCode TruthVertexGetterTool::initialize()
{
  CHECK( TruthVertexGetterBase::initialize() );

  return StatusCode::SUCCESS;
}

const std::type_info& TruthVertexGetterTool::typeinfo() const
{
  return typeid(TruthVertexGetterCollectionType);
}


const std::type_info& TruthVertexGetterTool::elementTypeinfo() const
{
  return typeid(TruthVertexGetterElementType);
}


size_t TruthVertexGetterTool::sizeHint(bool allowMissing)
{
  const McEventCollection* mcEvent = get(allowMissing);

  if (!mcEvent) {
    return 0;
  }

  int size_hint = 0;
  McEventCollection::const_iterator evt_itr = mcEvent->begin();
  McEventCollection::const_iterator evt_end = mcEvent->end();
  for(; evt_itr!=evt_end; evt_itr++){

    const HepMC::GenEvent* genEvent = (*evt_itr);
    if(!genEvent) break;

    HepMC::GenEvent::vertex_const_iterator vx_itr = genEvent->vertices_begin();
    HepMC::GenEvent::vertex_const_iterator vx_end = genEvent->vertices_end();
    for(; vx_itr!=vx_end;vx_itr++){
      size_hint++;
    }
  }

  return size_hint;
}


StatusCode TruthVertexGetterTool::reset(bool allowMissing)
{

  const McEventCollection* mcEvent = get(allowMissing);

  if (!mcEvent) {
    m_evtItr = m_evtEnd;
    m_vxItr = m_vxEnd;
    return StatusCode::FAILURE;
  }

  m_evtItr = mcEvent->begin();
  m_evtEnd = mcEvent->end();

  if(m_evtItr == m_evtEnd){
    m_vxItr = m_vxEnd;
    return StatusCode::SUCCESS;
  }

  m_vxItr = (*m_evtItr)->vertices_begin();
  m_vxEnd = (*m_evtItr)->vertices_end();

  if(m_vxItr==m_vxEnd){
    REPORT_MESSAGE (MSG::VERBOSE) << "====> First generated event seems to be empty...";
  }

  return StatusCode::SUCCESS;
}

const void* TruthVertexGetterTool::nextUntyped()
{
  if(m_evtItr == m_evtEnd) return 0;

  // if the event is a separator then we quit (signal and intime pile up will be BEFORE any separator)
  if ((*m_evtItr)->event_number() < 0) return 0;

  const HepMC::GenVertex* vertex = 0;

  if (m_keepOnlyPrimaryVertices && !m_keepPileUpVertices)
  {
    vertex = *m_vxItr;
    m_evtItr = m_evtEnd;
  }
  else if (m_keepOnlyPrimaryVertices && m_keepPileUpVertices)
  {
    while (m_evtItr != m_evtEnd && (*m_evtItr)->vertices_size() <= 0)
    {
      m_evtItr++;
      // we need to check and exit if next event is a separator ...
      if ((*m_evtItr)->event_number() < 0) return 0;
    }

    if(m_evtItr != m_evtEnd){
      m_vxItr = (*m_evtItr)->vertices_begin();
      vertex = (*m_vxItr);
    }

    m_evtItr++;
  }
  else if (!m_keepOnlyPrimaryVertices && !m_keepPileUpVertices)
  {
    if (m_vxItr != m_vxEnd) {
      vertex = *m_vxItr;
      m_vxItr++;
    } // if m_vxItr == m_vxEnd then vertex stays 0 and it will stop by itself
  }
  else if (!m_keepOnlyPrimaryVertices && m_keepPileUpVertices)
  {
    while (m_evtItr != m_evtEnd && (*m_evtItr)->vertices_size() <= 0)
    {
      m_evtItr++;
      // we need to check and exit if next event is a separator ...
      if ((*m_evtItr)->event_number() < 0) return 0;
    }

    if(m_evtItr != m_evtEnd) {
      if (m_vxItr != m_vxEnd)
      {
        vertex = (*m_vxItr);
        m_vxItr++;
      }
    }

    if (m_vxItr == m_vxEnd)
    {
      m_evtItr++;
      if (m_evtItr != m_evtEnd)
      {
        m_vxItr = (*m_evtItr)->vertices_begin();
        m_vxEnd = (*m_evtItr)->vertices_end();
      }
    }
  }
  return vertex;
}

} // namespace D3PD
