// Emacs -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigNavigation/FullHolderFactory.h"
#include "TrigNavStructure/BaseHolder.h"
#include "TrigNavigation/Holder.h"
#include "GaudiKernel/IConversionSvc.h"
#include "TrigNavigation/TypeMaps.h"


HLT::FullHolderFactory::FullHolderFactory(const std::string& prefix)
  : asg::AsgMessaging("HolderFactory"), 
    m_serializerSvc(0), 
    m_storeGate(0), 
    m_prefix(prefix),
    m_readonly(false)
{
}

HLT::BaseHolder* HLT::FullHolderFactory::fromSerialized(int version, const std::vector<uint32_t>::const_iterator& start, const std::vector<uint32_t>::const_iterator& end){
  class_id_type clid;
  sub_index_type idx;
  std::string label;

  std::vector<uint32_t>::const_iterator blobIt = start;
  
  auto blobsize = std::distance(blobIt,end);

  if(!blobsize){
    ATH_MSG_WARNING("trying to deserialize empty blob, returning nullptr");
    return nullptr;
  }

  BaseHolder::enquireSerialized(blobIt,end, clid, label, idx);

  auto itr = m_ignore.find(clid);
  if ( itr!=m_ignore.end() && (itr->second.empty() || itr->second.find(label)!=itr->second.end()) ) {
    ATH_MSG_DEBUG( "deserialize: skipping " << clid << "#" << label );
    return nullptr;
  }

  ATH_MSG_VERBOSE("deserialize: extracted blob CLID: " << clid << " of size: " << blobsize << " SubTypeIndex: " << idx << " Label: " << label);
  
  BaseHolder* baseholder = createHolder(clid, label, idx);
  if (! baseholder) {
    ATH_MSG_ERROR("deserialize: Could not create holder for CLID " << clid
		  << " this probably means that the package holding the class for this CLID was not compiled against the TrigNavigation package in use!");
    return nullptr;
  }

  // now we have the blobIt pointing to the place where objects may start
  // if so we ought to do deserialization
  if ( blobIt != end ) {
    //copy the blob
    std::vector<uint32_t>::const_iterator constEnd = end;
    std::vector<uint32_t> blobpart2(blobIt, constEnd);

    auto iholder = static_cast<HLTNavDetails::IHolder*>(baseholder);

    iholder->deserializePayload(blobpart2,version);
    
    if ( !iholder->syncWithSG() ) {
      ATH_MSG_WARNING("deserialize: after deserialization obejct not accessible though SG");
    }
  }
  return baseholder;
}

HLT::BaseHolder* HLT::FullHolderFactory::createHolder(CLID clid, const std::string& label, sub_index_type index) {
  ATH_MSG_DEBUG("createHolder: creating holder for CLID: " << clid  << " label: " << label << " and index: " << index << " readonly: " << m_readonly);

  if ( HLT::TypeMaps::holders().count(clid) == 0 ) {
    ATH_MSG_ERROR("createHolder: holder can't be done, no predefined storage found for CLID: " << clid);
    return 0;
  }

  auto holder = HLT::TypeMaps::holders()[clid]->clone(m_prefix, label, index);
  if(!holder){
    ATH_MSG_ERROR("createHolder: clone of holder failed clid: " << clid);
    return 0;
  }

  ATH_MSG_DEBUG("preparing holder with msg: " << &msg() << " storegate: " << m_storeGate << " and prefix " << m_prefix);

  holder->prepare(&msg(), m_storeGate, m_serializerSvc, m_readonly);
  
  ATH_MSG_DEBUG("createHolder: holder prepared " << *holder);

  return holder;
}
