/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#include "TrigCombinedEvent/TrigComposite.h"
#undef private

#include "TrigCombinedEventTPCnv/TrigComposite_p1.h"
#include "TrigCombinedEventTPCnv/TrigCompositeCnv_p1.h"
//#include "AthenaKernel/errorcheck.h"
#include "boost/any.hpp"

void TrigCompositeCnv_p1::transToPers(const TrigComposite* trans, 
				      TrigComposite_p1* pers, MsgStream &log )
{
  
  log << MSG::DEBUG << "TrigCompositeCnv_p1::transToPers" << endreq;
  
  if ( (!trans) || (!pers) ) {
    log << MSG::ERROR << "TrigCompositeCnv_p1::transToPers with trans or pers == 0" << endreq;
  }else{

  // everywhere blow the vector::reserve() can be used since we know the sizes in advance, 
  // in fact, however, we expect this arrays to be of small size so it should not matter  
  std::map< std::string, TrigFeatureLink >::const_iterator it_link;
  for(it_link=trans->m_links.begin(); it_link!=trans->m_links.end(); it_link++){
  
  	pers->m_labels.push_back( it_link->first );
  	pers->m_clids.push_back( (uint32_t)it_link->second.clid() );
  	pers->m_stypes.push_back( it_link->second.subTypeIndex() );
  	pers->m_indexes.push_back( it_link->second.index() );
  }
  
  //  std::map<std::string, bool>::const_iterator it_set;

  std::map<std::string, float>::const_iterator i;
  for ( i=trans->m_floats.begin() ; i != trans->m_floats.end(); i++ ){
    pers->m_floats.push_back(i->second);
    pers->m_fnames.push_back(i->first);
    //    it_set=(trans->m_set).find(i->first);
    //std::cout<<"Camille: bool value: "<<i->first<<"  "<< (*it_set).second<<std::endl;
    //pers->m_v_set.push_back(it_set->second);
  }

  std::map<std::string, int>::const_iterator j;
  for ( j=trans->m_ints.begin() ; j != trans->m_ints.end(); j++ ){
    pers->m_ints.push_back(j->second);
    pers->m_inames.push_back(j->first);
    //    it_set=(trans->m_set).find(j->first);
    //pers->m_v_set.push_back(it_set->second);
    //std::cout<<": bool value: "<<j->first<<"  "<< (*it_set).second<<std::endl;
  }

  std::map<std::string, std::string>::const_iterator k;
  for ( k=trans->m_strings.begin() ; k != trans->m_strings.end(); k++ ){
    pers->m_strings.push_back(k->second);
    pers->m_snames.push_back(k->first);
    //    it_set=(trans->m_set).find(k->first);
    //pers->m_v_set.push_back((*it_set).second);
  }
  std::map<std::string, std::vector<float> >::const_iterator l;
  for ( l=trans->m_v_floats.begin() ; l != trans->m_v_floats.end(); l++ ){
    pers->m_v_floats.push_back(l->second);
    pers->m_v_fnames.push_back(l->first);
    //    it_set=(trans->m_set).find(l->first);
    //    pers->m_v_set.push_back((*it_set).second);
  }

  std::map<std::string, std::vector<int> >::const_iterator m;
  for ( m=trans->m_v_ints.begin() ; m != trans->m_v_ints.end(); m++ ){
    pers->m_v_ints.push_back(m->second);
    pers->m_v_inames.push_back(m->first);
    //    it_set=(trans->m_set).find(m->first);
    //    pers->m_v_set.push_back((*it_set).second);
  }

  std::map<std::string, std::vector<std::string> >::const_iterator n;
  for ( n=trans->m_v_strings.begin() ; n != trans->m_v_strings.end(); n++ ){
    pers->m_v_strings.push_back(n->second);
    pers->m_v_snames.push_back(n->first);
    //    it_set=(trans->m_set).find(n->first);
    //    pers->m_v_set.push_back((*it_set).second);
  }

  pers->m_instance=trans->m_name;
  //  pers->m_f_locked=trans->m_locked;
  //  pers->m_mustset=trans->m_mustset;
  }
}
template<class T>
void copyDetails2Trans(const std::vector<std::string>& keys, const std::vector<T>& values, TrigComposite* trans, MsgStream &log) {
  if (values.size() != keys.size() ) {
    log << MSG::ERROR << "TrigCompositeCnv_p1::persToTrans: The number of values and names for do not match, problem with the stored data, while" << __PRETTY_FUNCTION__ << endreq;    
    // the __PREETY_FUNCTION__ should give actual function name with the template so we know if this is about floats, ints ....
  }
  typename std::vector<T>::const_iterator vi = values.begin();
  std::vector<std::string>::const_iterator ki =  keys.begin();
  
  for ( ;  vi != values.end() and ki != keys.end(); ki++, vi++ )
    trans->addDetail(*ki, *vi);  
}

void TrigCompositeCnv_p1::persToTrans(const TrigComposite_p1* pers, 
				      TrigComposite* trans, MsgStream &log )
{
  
  log << MSG::DEBUG << "TrigCompositeCnv_p1::persToTrans" << endreq;
  
  if ( (!trans) || (!pers) ) {
    log << MSG::ERROR << "TrigCompositeCnv_p1::persToTrans with trans or pers == 0" << endreq;
  }else{
    //  std::vector<bool>::const_iterator it_set;
    //  it_set=(pers->m_v_set).begin();
    
    copyDetails2Trans(pers->m_fnames, pers->m_floats, trans, log);
    copyDetails2Trans(pers->m_inames, pers->m_ints, trans, log);
    copyDetails2Trans(pers->m_snames, pers->m_strings, trans, log);
    
    copyDetails2Trans(pers->m_v_fnames, pers->m_v_floats, trans, log);
    copyDetails2Trans(pers->m_v_inames, pers->m_v_ints, trans, log);
    copyDetails2Trans(pers->m_v_snames, pers->m_v_strings, trans, log);
    
    if((pers->m_labels.size())==(pers->m_clids.size())  && (pers->m_labels.size())==(pers->m_stypes.size()) && (pers->m_labels.size())==(pers->m_indexes.size())){
      std::vector<std::string>::const_iterator labelIt = pers->m_labels.begin();
      std::vector<uint32_t>::const_iterator     clidIt = pers->m_clids.begin();	
      std::vector<uint16_t>::const_iterator      colIt = pers->m_stypes.begin();
      std::vector<uint32_t>::const_iterator    indexIt = pers->m_indexes.begin();
      
      
      for (  ; labelIt != pers->m_labels.end(); clidIt++, indexIt++, colIt++, labelIt++ ){                  
	trans->addObject(*labelIt, TrigFeatureLink(*clidIt, *colIt, *indexIt));      
      }  	
    }else{
      log << MSG::ERROR << "TrigCompositeCnv_p1::persToTrans: The number of TrigFeatureLink labels, CLIDs, types and indexes does not match, problem with the stored data" << endreq;  
    }	
    
    trans->m_name=pers->m_instance;
    //  trans->m_locked=pers->m_f_locked;
    //  traans->m_mustset=pers->m_mustset;
    //trans->m_log.m_label=pers->m_instance;
  }
}
