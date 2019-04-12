/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./ConditionDebugVisitor.h"

#include "./EtaEtConditionMT.h"
#include "./EtaEtAsymmetricConditionMT.h"
#include "./DijetConditionMT.h"
#include "./HTConditionMT.h"
#include "./TLAConditionMT.h"

void ConditionDebugVisitor::visit(const EtaEtConditionMT*,
                                  const std::string& s){
  std::string key{"EtaEt"};
  m_debugStrings[key]  += s;
}

void ConditionDebugVisitor::visit(const EtaEtAsymmetricConditionMT*,
                                  const std::string& s){
  std::string key{"EtaEtAsymmetric"};
  m_debugStrings[key]  = m_debugStrings[key] + s;
}

void ConditionDebugVisitor::visit(const DijetConditionMT*,
                                  const std::string& s){
std::string key{"Dijet"};
m_debugStrings[key]  = m_debugStrings[key] + s;
}

void ConditionDebugVisitor::visit(const HTConditionMT*,
                                  const std::string& s){
  std::string key{"HT"};
  m_debugStrings[key]  = m_debugStrings[key] + s;
}


void ConditionDebugVisitor::visit(const TLAConditionMT*, 
                                  const std::string& s){
std::string key{"TLA"};
  m_debugStrings[key]  = m_debugStrings[key] + s;
}

std::string ConditionDebugVisitor::toString() const {
  std::string s;
  for(const auto& kv : m_debugStrings){
    if (!(kv.first).empty()){
      s += kv.first + '\n' + kv.second;
    }
  }
  return s; 
}
