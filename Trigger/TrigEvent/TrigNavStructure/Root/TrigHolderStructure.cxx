/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavStructure/TrigHolderStructure.h"

HLT::TrigHolderStructure::TrigHolderStructure() : asg::AsgMessaging("TrigHolderStructure"){
} 

void HLT::TrigHolderStructure::reset(){
  m_holderByClidAndIndex.clear();
  m_lookupLabels.clear();
  m_lookupSubIndex.clear();
} 

HLT::BaseHolder* HLT::TrigHolderStructure::getBaseHolder(class_id_type clid, sub_index_type sti) const {
  if( sti == HLT::invalid_sub_index){
    ATH_MSG_DEBUG("requested holder with invalid sub_type_index");
    return 0;
  }
  auto allsti_it = m_holderByClidAndIndex.find(clid);
  if(allsti_it == m_holderByClidAndIndex.end()){
    return 0;
  }
  auto sti_holder_map =  allsti_it->second;
  auto holder_it = sti_holder_map.find(sti);
  if(holder_it == sti_holder_map.end()){
    return 0;
  }
  return holder_it->second.get();
}


bool HLT::TrigHolderStructure::registerHolder(const std::shared_ptr<BaseHolder>& holder){ 
  auto clid = holder->typeClid();
  auto subtypeindex = holder->subTypeIndex();
  auto label = holder->label();

  if(getHolder(clid,subtypeindex)){
    ATH_MSG_WARNING("we already have a holder like this! clid:" << clid << " subtypeIndex: " << subtypeindex << " label: " << label);
    return false;
  }
  


  m_holderByClidAndIndex[clid][subtypeindex] = holder;
  m_lookupLabels[clid][subtypeindex] = label;
  m_lookupSubIndex[clid][label] = subtypeindex;

  return true;
}
