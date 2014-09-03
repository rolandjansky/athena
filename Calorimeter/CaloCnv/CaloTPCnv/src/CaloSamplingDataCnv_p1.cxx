/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloSamplingDataCnv_p1.h"
#include "CaloTPCnv/CaloSamplingData_p1.h"

#define private public
#include "CaloEvent/CaloSamplingData.h"
#undef private

#include <algorithm>
#include <cassert>


CaloSamplingDataCnv_p1::CaloSamplingDataCnv_p1()
{
}


void CaloSamplingDataCnv_p1::persToTrans(const CaloSamplingData_p1* pers, CaloSamplingData* trans /*, MsgStream &*/) {
  trans->m_varTypePattern=pers->m_varTypePattern;
  if (pers->m_dataStore.empty()) {
    trans->m_dataStore.clear();
    return;
  }

  size_t nSamplings=trans->fastNsamp();
  trans->m_dataStore.resize (trans->index (pers->m_dataStore.size(), 0));
  
  std::vector<std::vector<float> >::const_iterator it=pers->m_dataStore.begin();
  std::vector<std::vector<float> >::const_iterator it_e=pers->m_dataStore.end();
  CaloSamplingData::variable_store_type::iterator pos = trans->m_dataStore.begin();
  for (;it!=it_e;++it) {
    size_t nPersSamplings = it->size();
    size_t nCopySamplings = std::min (nSamplings, nPersSamplings);
    std::copy(it->begin(),it->begin()+nCopySamplings,pos);
    if (nCopySamplings < nSamplings)
      std::fill(pos+nCopySamplings, pos+nSamplings, 0);
    pos += nSamplings;
  }
  assert (pos == trans->m_dataStore.end());
}


void CaloSamplingDataCnv_p1::transToPers(const CaloSamplingData* trans, CaloSamplingData_p1* pers /*, MsgStream &*/)  {
  pers->m_varTypePattern=trans->m_varTypePattern;
  if (trans->m_dataStore.empty()) {
    pers->m_dataStore.clear();
    return;
  }
  size_t nsamp = trans->fastNsamp();
  pers->m_dataStore.resize(trans->m_dataStore.size() / nsamp);
  CaloSamplingData::variable_store_type::const_iterator it=trans->m_dataStore.begin();
  CaloSamplingData::variable_store_type::const_iterator it_e=trans->m_dataStore.end();
  unsigned i=0;
  for (; it<it_e; it += nsamp) {
    pers->m_dataStore[i].assign (it, it+nsamp);
    ++i;
  }
}
