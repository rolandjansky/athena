/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#include "AthContainers/tools/UDSLabelHashTable.h"
#undef private

#include "DataModelAthenaPool/UDSLabelHashTable_p1.h"
#include "DataModelAthenaPool/UDSLabelHashTableCnv_p1.h"

void UDSLabelHashTableCnv_p1::transToPers(const UDSLabelHashTable& trans, UDSLabelHashTable_p1& pers) {
  const size_t s=trans.size();
  pers.m_label.resize(s);
  pers.m_labelHash.resize(s);
  std::map<UDSLabelHashTable::hash_t, std::string>::const_iterator it=trans.m_hashLabels.begin();
  std::map<UDSLabelHashTable::hash_t, std::string>::const_iterator it_e=trans.m_hashLabels.end();
  for(;it!=it_e;++it) {
    pers.m_labelHash.push_back(it->first);
    pers.m_label.push_back(it->second);
  }
  return;
}


void UDSLabelHashTableCnv_p1::persToTrans(const UDSLabelHashTable_p1& pers, UDSLabelHashTable& trans){
  const size_t s=pers.m_labelHash.size();
  for (size_t i=0;i<s;++i) {
    trans.m_hashLabels.insert(trans.m_hashLabels.end(),std::make_pair(pers.m_labelHash[i],pers.m_label[i]));
  }
}
