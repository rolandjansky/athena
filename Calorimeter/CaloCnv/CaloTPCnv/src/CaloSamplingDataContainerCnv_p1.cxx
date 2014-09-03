/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloSamplingDataContainerCnv_p1.h"
#define private public
#define protected public
#include "CaloEvent/CaloSamplingData.h"
#undef private
#undef protected
#include "AthenaKernel/errorcheck.h"

void CaloSamplingDataContainerCnv_p1::transToPers(const CaloSamplingData* trans, 
						  CaloSamplingDataContainer_p1* pers, MsgStream& /*log*/) {

  pers->m_varTypePatterns.push_back(trans->m_varTypePattern);
  typedef CaloSamplingData::variable_store_type::size_type var_size_type;
  //The sampling data store gets resized to nVariables x nSamplings by its constructor
  size_t nVar=trans->getNumberOfVariableTypes();
  size_t nSamplings=trans->fastNsamp();
  for (var_size_type i=0;i<nVar;++i) {
    //Copy only if filled
    if (trans->m_varTypePattern & (0x1U<<i)) {
      CaloSamplingData::variable_store_type::const_iterator pos =
        trans->m_dataStore.begin() + trans->index (i, 0);
      pers->m_dataStore.insert (pers->m_dataStore.end(),
                                pos, pos + nSamplings);
    }
  }//loop over variable types
}
					  
void CaloSamplingDataContainerCnv_p1::persToTrans(const CaloSamplingDataContainer_p1* /*pers*/,
						  CaloSamplingData* trans) {

  //Convert m_varTypePattern
  trans->m_varTypePattern=*(m_varTypePatternsIterator++);
  size_t nVar=trans->getNumberOfVariableTypes();
  size_t nSamplings=trans->fastNsamp();
  typedef CaloSamplingData::variable_key_type vartype;

  size_t nCopySamplings = std::min (nSamplings, m_nPersSamplings);

  for (size_t i=0;i<nVar;++i) {
    //Copy only if filled
    if (trans->m_varTypePattern & (0x1U<<i)) {
      CaloSamplingData::variable_store_type::iterator pos =
        trans->m_dataStore.begin() + trans->index (i, 0);
      std::copy (m_samplingStoreIterator,
                 m_samplingStoreIterator + nCopySamplings,
                 pos);
      if (m_nPersSamplings < nSamplings)
        std::fill (pos + m_nPersSamplings, pos + nSamplings, 0);
      m_samplingStoreIterator += m_nPersSamplings;
    }
    else //variable not present, set to zero
      trans->removeVariable (static_cast<vartype>(i));
  }//end for loop over varTypes
}


bool CaloSamplingDataContainerCnv_p1::setIterator(const CaloSamplingDataContainer_p1* pers,
                                                  unsigned int ncluster)
{
  m_samplingStoreIterator=pers->m_dataStore.begin();
  m_varTypePatternsIterator=pers->m_varTypePatterns.begin();

  // Previous versions of the code could have written one more sampling
  // than actually exists.  Count the number of variables that were written,
  // then divide the persistent vector by that to get the number of samplings
  // that were actually written.
  std::vector<unsigned int>::const_iterator it =
    pers->m_varTypePatterns.begin();
  std::vector<unsigned int>::const_iterator end =
    pers->m_varTypePatterns.end();
  size_t nPersVars = 0;
  for (; it != end; ++it) {
    unsigned int mask = (*it & CaloVariableType::getAllVariableBits());
    while (mask != 0) {
      if ((mask & 1) != 0)
        ++nPersVars;
      mask >>= 1;
    }
  }
  if (nPersVars == 0)
    m_nPersSamplings = 0;
  else
    m_nPersSamplings = pers->m_dataStore.size() / nPersVars;

  if (ncluster != pers->m_varTypePatterns.size()) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, "CaloSamplingDataContainerCnv_p1")
      << "Corrupted persistent data seen: "
      << ncluster << " clusters != "
      << pers->m_varTypePatterns.size() << " stored vartype patterns.";
    return false;
  }

  if (nPersVars == 0 && pers->m_dataStore.empty())
    {} // ok
  else if (nPersVars == 0 || (pers->m_dataStore.size() % nPersVars) != 0) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, "CaloSamplingDataContainerCnv_p1")
      << "Corrupted persistent data seen: "
      << pers->m_dataStore.size() << " stored values is not a multiple of "
      << nPersVars << " stored variables.";
    return false;
  }

  return true;
}
