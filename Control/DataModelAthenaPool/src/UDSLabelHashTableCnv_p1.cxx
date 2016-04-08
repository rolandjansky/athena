/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthContainers/tools/UDSLabelHashTable.h"
#include "DataModelAthenaPool/UDSLabelHashTable_p1.h"
#include "DataModelAthenaPool/UDSLabelHashTableCnv_p1.h"

void UDSLabelHashTableCnv_p1::transToPers(const UDSLabelHashTable& trans, UDSLabelHashTable_p1& pers) {
  const size_t s=trans.size();
  pers.m_label.resize(s);
  pers.m_labelHash.resize(s);
  for (const auto& p : trans) {
    pers.m_labelHash.push_back(p.first);
    pers.m_label.push_back(p.second);
  }
  return;
}


void UDSLabelHashTableCnv_p1::persToTrans(const UDSLabelHashTable_p1& pers, UDSLabelHashTable& trans){
  const size_t s=pers.m_labelHash.size();
  for (size_t i=0;i<s;++i) {
    trans.addLabel (pers.m_label[i], pers.m_labelHash[i]);
  }
}
