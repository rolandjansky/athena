/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// CoraCoolObject.cxx
// implementation forCoraCoolObject
// Richard Hawkings, started 10/06

#include "CoraCool/CoraCoolObject.h"

CoraCoolObject::CoraCoolObject(const cool::ValidityKey& since, 
	const cool::ValidityKey& until, const cool::ChannelId& chan):
  m_since(since), m_until(until), m_chan(chan), m_spec(0) {}

CoraCoolObject::CoraCoolObject(const cool::ValidityKey& since, 
	const cool::ValidityKey& until, const cool::ChannelId& chan,
	       const coral::AttributeListSpecification* spec):
  m_since(since), m_until(until), m_chan(chan), m_spec(spec) {}

CoraCoolObject::~CoraCoolObject() {}

void CoraCoolObject::add(const coral::AttributeList& data) {
  if (m_spec==0) {
    m_payload.push_back(data);
  } else {
    m_payload.emplace_back(*m_spec,true);
    m_payload.back().fastCopyData(data);
  }
}
