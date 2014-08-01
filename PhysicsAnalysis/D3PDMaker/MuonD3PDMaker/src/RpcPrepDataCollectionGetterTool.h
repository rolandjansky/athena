/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RpcPrepDataCollectionGetterTool.h 535250 2013-01-29 13:22:17Z salekd $
#ifndef MUOND3PDMAKER_RPCPREPDATACOLLECTIONGETTERTOOL_H
#define MUOND3PDMAKER_RPCPREPDATACOLLECTIONGETTERTOOL_H

#include "D3PDMakerUtils/SGCollectionGetterTool.h" 
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"

//namespace Muon {
//  class RpcPrepData;
//}

namespace D3PD { 
  
class RpcPrepDataCollectionGetterTool 
  : public SGCollectionGetterTool<Muon::RpcPrepDataContainer>
{ 
public: 
  typedef SGCollectionGetterTool<Muon::RpcPrepDataContainer> Base; 

  RpcPrepDataCollectionGetterTool (const std::string& type,
				  const std::string& name, 
				  const IInterface* parent); 

  StatusCode initialize(); 
  size_t sizeHint(bool=false); 
  StatusCode reset(bool=false); 
  const void* nextUntyped(); 
  const std::type_info& typeinfo() const; 
  const std::type_info& elementTypeinfo() const; 
  
private: 
  Muon::RpcPrepDataContainer::const_iterator m_contItr; 
  Muon::RpcPrepDataContainer::const_iterator m_contEnd; 
  
  Muon::RpcPrepDataCollection::const_iterator m_collItr; 
  Muon::RpcPrepDataCollection::const_iterator m_collEnd; 
}; 

// Dummies --- the default versions of these don't compile.
// But these are overridden by the derived class here.
template<>
const std::type_info&
CollectionGetterTool<Muon::RpcPrepDataContainer>::elementTypeinfo() const;

template<>
const void*
CollectionGetterTool<Muon::RpcPrepDataContainer>::nextUntyped();

} 

#endif
