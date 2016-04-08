/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCombinedEvent/TrigComposite.h"
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
  }
  else{

    // everywhere below the vector::reserve() can be used since we know the sizes in advance, 
    // in fact, however, we expect this arrays to be of small size so it should not matter  
    for (const std::pair<const std::string, TrigFeatureLink>& p :
           trans->allDetails<TrigFeatureLink>())
    {
  	pers->m_labels.push_back( p.first );
  	pers->m_clids.push_back( (uint32_t)p.second.clid() );
  	pers->m_stypes.push_back( p.second.subTypeIndex() );
  	pers->m_indexes.push_back( p.second.index() );
    }
  
    for (const std::pair<const std::string, float>& p :
           trans->allDetails<float>())
    {
      pers->m_floats.push_back(p.second);
      pers->m_fnames.push_back(p.first);
    }

    for (const std::pair<const std::string, int>& p :
           trans->allDetails<int>())
    {
      pers->m_ints.push_back(p.second);
      pers->m_inames.push_back(p.first);
    }

    for (const std::pair<const std::string, std::string>& p :
           trans->allDetails<std::string>())
    {
      pers->m_strings.push_back(p.second);
      pers->m_snames.push_back(p.first);
    }

    for (const std::pair<const std::string, std::vector<float> >& p :
           trans->allDetails<std::vector<float> >())
    {
      pers->m_v_floats.push_back(p.second);
      pers->m_v_fnames.push_back(p.first);
    }

    for (const std::pair<const std::string, std::vector<int> >& p :
           trans->allDetails<std::vector<int> >())
    {
      pers->m_v_ints.push_back(p.second);
      pers->m_v_inames.push_back(p.first);
    }

    for (const std::pair<const std::string, std::vector<std::string> >& p :
           trans->allDetails<std::vector<std::string> >())
    {
      pers->m_v_strings.push_back(p.second);
      pers->m_v_snames.push_back(p.first);
    }

    pers->m_instance=trans->name();
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
    *trans = TrigComposite (pers->m_instance);

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
    
    //  trans->m_locked=pers->m_f_locked;
    //  traans->m_mustset=pers->m_mustset;
    //trans->m_log.m_label=pers->m_instance;
  }
}
