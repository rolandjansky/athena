/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_STANDALONE

#include "AthContainers/tools/UDSLabelHashTable.h"
#include "AthenaKernel/getMessageSvc.h"

void UDSLabelHashTable::addLabel(const std::string& label, hash_t hash)
{
  hash_t h = addLabel (label);
  if (h != hash) {
    MsgStream logstr(Athena::getMessageSvc(), "UDSLabelHashTable");
    logstr << MSG::ERROR << "Hash mismatch for " << label
           << ": " << hash << "/" << h << endmsg;
  }
}


UDSLabelHashTable::hash_t UDSLabelHashTable::addLabel(const std::string& label) {
  if (label.size()==0) {
    MsgStream logstr(Athena::getMessageSvc(), "UDSLabelHashTable");
    logstr << MSG::ERROR << "Zero-length string given as Label!" << endmsg;
    return INVALID;
  }
  hash_t h=this->hash(label);
  if (h==INVALID) {
    MsgStream logstr(Athena::getMessageSvc(), "UDSLabelHashTable");
    logstr << MSG::ERROR << "Label " << label << " yields an invalid hash. Please choose different name!" << endmsg;
    return INVALID;
  }

  std::map<hash_t, std::string>::const_iterator it=m_hashLabels.find(h);
  if (it==m_hashLabels.end()) {
    //new label, new hash
    m_hashLabels[h]=label;
  }
  else {
    //Hash exists already
    if (it->second!=label) { //Same hash but different label -> hash-collision! 
      MsgStream logstr(Athena::getMessageSvc(), "UDSLabelHashTable");
      logstr << MSG::ERROR << "Hash collision between label " << label << " and label " << it->second << endmsg;
      return INVALID;
    }
  }
  return h;
}
   

const std::string& UDSLabelHashTable::getLabel(const hash_t number) const {
  std::map<hash_t, std::string>::const_iterator it=m_hashLabels.find(number);
  if (it==m_hashLabels.end()) 
    return m_empty;
  else 
    return it->second;
}


std::vector<const std::string*> UDSLabelHashTable::getAllLabels() const {

  std::vector<const std::string*> allLabels;
  allLabels.reserve(m_hashLabels.size());
  std::map<hash_t, std::string>::const_iterator it=m_hashLabels.begin();
  std::map<hash_t, std::string>::const_iterator it_e=m_hashLabels.end();
  for(;it!=it_e;++it) 
    allLabels.push_back(&(it->second));

  return allLabels;
}

#endif // not XAOD_STANDALONE
