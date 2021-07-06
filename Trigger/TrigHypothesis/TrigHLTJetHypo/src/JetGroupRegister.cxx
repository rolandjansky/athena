/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./JetGroupRegister.h"

std::size_t JetGroupRegister::record(HypoJetVector v){

  std::sort(v.begin(), v.end(), HypoJetLess());
  
  auto it = m_reg.find(v);
  
  if (it == m_reg.end()){
    auto new_ind = m_reg.size();
    m_reg[v] = new_ind;
    return new_ind;
  } else {
    return it->second;
  }
}
