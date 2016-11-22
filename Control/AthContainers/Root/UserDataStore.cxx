/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthContainers/UserDataStore.h"

#ifndef XAOD_STANDALONE

#include "AthenaKernel/getMessageSvc.h"

UserDataStore::UserDataStore() : 
  m_data(), 
  m_lastIt(m_data.end()), 
  m_lastIt_nc(m_data.end()),
  m_eventABC(IAthenaBarCode::UNDEFINEDBARCODE),
  m_msg(Athena::getMessageSvc(),"UserDataStore"),
  m_whyNotFound(ALLGOOD)

 {}



UserDataStore::const_iterator
UserDataStore::find(const AthenaBarCode_t& barcode, const std::string& label) const {
  m_whyNotFound=ALLGOOD;
  if (m_lastIt==m_data.end() || m_lastIt->first!=barcode) 
    m_lastIt=m_data.find(barcode);
  
  if (m_lastIt==m_data.end()) { //No user data for this bar-code
    m_whyNotFound=BARCODE;
    return m_notFoundIt;
  }

  const index_t idx=m_labelTable.hash(label);
  const_iterator label_it=m_lastIt->second.find(idx);
  if (label_it==m_lastIt->second.end()) {
    //std::cout << "Did not find label " << label << "(hash=" << idx << ")" << std::endl;
    m_whyNotFound=LABEL;
    return m_notFoundIt;
  }
  else 
    return label_it;
}

void UserDataStore::setEventABC(const AthenaBarCode_t& eventABC) {
  if (m_eventABC==IAthenaBarCode::UNDEFINEDBARCODE)  m_eventABC=eventABC;
}
    


StatusCode UserDataStore::recordAny(const IAthenaBarCode& obj, const std::string& label, const boost::any& value) {
  const AthenaBarCode_t& barcode=obj.getAthenaBarCode();
  
  if (barcode==IAthenaBarCode::UNDEFINEDBARCODE) {
    m_msg << MSG::ERROR << "Attempt to record UserData for an object with undefined AthenaBarCode. Label=" << label << endmsg;
    return StatusCode::FAILURE;
  }

  const index_t idx=m_labelTable.addLabel(label);
  if (idx==UDSLabelHashTable::INVALID) {
    m_msg << MSG::ERROR << "Failed to convert label '" << label << "' into a hash value" << endmsg;
    return StatusCode::FAILURE;
  }
  else {
    if (m_lastIt_nc==m_data.end() || m_lastIt_nc->first!=barcode) {
      m_lastIt_nc=m_data.find(barcode);
    }
    if (m_lastIt_nc==m_data.end()) {
      m_lastIt_nc=m_data.insert(std::make_pair(barcode,m_defaultEntry)).first;
    }
    (m_lastIt_nc->second)[idx]=value;
    return StatusCode::SUCCESS;
  }
}


StatusCode UserDataStore::retrieveAny(const IAthenaBarCode& obj, const std::string& label, const boost::any*& value) const {
  const AthenaBarCode_t& barcode=obj.getAthenaBarCode();
  
  const_iterator it=this->find(barcode,label);
  if (it==m_notFoundIt) {
    if (m_whyNotFound==BARCODE) 
      m_msg << MSG::ERROR << "No user data found for bar code " << barcode << endmsg;
    if (m_whyNotFound==LABEL) 
      m_msg << MSG::ERROR << "No user data with label '" << label << "' found for this barcode" << endmsg;
    return StatusCode::FAILURE;
  }
  else {
    value=&(it->second);
    return StatusCode::SUCCESS;
  }
}


bool UserDataStore::contains(const IAthenaBarCode& obj, const std::string& label) const {
 const AthenaBarCode_t& barcode=obj.getAthenaBarCode();
 const_iterator it=this->find(barcode,label);
 return (it!=m_notFoundIt);
}


std::vector<const std::string*> UserDataStore::getLabels(const AthenaBarCode_t& barcode) const {
  
  std::vector<const std::string*> out;
  mapmap_t::const_iterator abc_it=m_data.find(barcode);
  if (abc_it!=m_data.end()) {
    const std::map<index_t, boost::any>& labelMap=abc_it->second;
    const_iterator it=labelMap.begin();
    const_iterator it_e=labelMap.end();
    for (;it!=it_e;++it) {
      const std::string& sLabel=m_labelTable.getLabel(it->first);
      out.push_back(&sLabel);
    }
  }// end if abc_it!=m_data.end()
  return out;
}

#endif // not XAOD_STANDALONE
