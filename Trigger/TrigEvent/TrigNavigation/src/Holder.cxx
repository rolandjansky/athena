/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include <boost/regex.hpp>

#include "TrigNavigation/Holder.h"
#include "TrigSerializeResult/StringSerializer.h"
#include "TrigSerializeResult/ITrigSerializerToolBase.h"

using namespace HLTNavDetails;

/*****************************************************************************
 *
 * INNER CLASS (fatures handling)
 *
 *****************************************************************************/
IHolder::IHolder()
  : m_serialized(0),
    m_log(0),
    m_storeGate(0),
    m_serializer(0),
    m_label(""),
    m_subTypeIndex(0),
    m_aux(0),
    m_uniqueCounter(0)
{}

IHolder::~IHolder() {
  if ( m_aux ) { delete m_aux; }
}

void IHolder::prepare(MsgStream* log, HLT::AccessProxy* sg) {
  m_storeGate = sg;
  m_log = log;
}

bool IHolder::serialize(std::vector<uint32_t>& output)  const {
  if ( m_log->level()<=MSG::DEBUG )
    *m_log << MSG::DEBUG << "Holder<T> serialize, " << *this  << endreq;
  // if feature to be forgotten then indication of it is simply 0 size of serialized vector

  output.push_back( typeClid() );
  output.push_back( m_subTypeIndex );

  // label
  std::vector<uint32_t> serializedLabel;
  StringSerializer ss;
  ss.serialize( m_label, serializedLabel );
  output.push_back( serializedLabel.size() );
  output.insert(output.end(), serializedLabel.begin(), serializedLabel.end());
  return true;
}

bool IHolder::enquireSerialized(const std::vector<uint32_t>& blob,
                                std::vector<uint32_t>::const_iterator& it,
                                CLID& c, std::string& label, uint16_t& subtypeIndex ) {

  if ( it == blob.end() )  return false;

  c = *it++;
  if ( it == blob.end() )  return false;

  subtypeIndex = *it++;
  if ( it == blob.end() ) return false;

  unsigned labelSize = *it++;
  if ( it == blob.end() ) return false;
  if ( it+labelSize > blob.end() ) return false;

  std::vector<uint32_t> tmp(it, it+labelSize);
  it += labelSize;

  StringSerializer s;
  s.deserialize(tmp, label);
  return true;
}



std::string IHolder::generateAliasKey(CLID c, uint16_t sti, const std::string& label, unsigned size) {
  std::ostringstream ss;
  ss <<  "HLTAutoKey_" << label << "_" << c << "_" << sti << "_" << (size == 0xffffffff ? "back" : "to")  << "_" << m_uniqueCounter;
  std::string st = ss.str();
  if ( m_log->level()<=MSG::DEBUG )
    *m_log << MSG::DEBUG << "IHolder::generateAliasKey generated key: " << st << endreq;
  m_uniqueCounter++;
  return st;
}

void IHolder::print(MsgStream& m) const {
  m << "CLID: " << typeClid() << " label: \"" << label() << "\" subTypeIndex: " << subTypeIndex();
}

MsgStream& HLTNavDetails::operator<< ( MsgStream& m, const HLTNavDetails::IHolder& h ) {
  h.print (m);
  return m;
}

std::string HLTNavDetails::formatSGkey(const std::string& prefix, const std::string& containername, const std::string& label){
  std::string cleaned = containername;
  boost::regex rx1("_v[0-9]+$");
  cleaned = boost::regex_replace(cleaned,rx1,std::string(""));
  boost::regex rx2("::");
  cleaned = boost::regex_replace(cleaned,rx2,std::string("__"));
  return prefix+"_"+cleaned+(label.size() ? "_"+label : "");
}


