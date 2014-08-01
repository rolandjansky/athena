/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CscPrepDataCollectionGetterTool.h 535250 2013-01-29 13:22:17Z salekd $
#ifndef MUOND3PDMAKER_CSCPREPDATACOLLECTIONGETTERTOOL_H
#define MUOND3PDMAKER_CSCPREPDATACOLLECTIONGETTERTOOL_H

#include "D3PDMakerUtils/SGCollectionGetterTool.h" 
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"

//namespace Muon {
//  class CscPrepData;
//}

namespace D3PD { 
  
class CscPrepDataCollectionGetterTool 
  : public SGCollectionGetterTool<Muon::CscPrepDataContainer>
{ 
public: 
  typedef SGCollectionGetterTool<Muon::CscPrepDataContainer> Base; 

  CscPrepDataCollectionGetterTool (const std::string& type,
				  const std::string& name, 
				  const IInterface* parent); 

  StatusCode initialize(); 
  size_t sizeHint(bool=false); 
  StatusCode reset(bool=false); 
  const void* nextUntyped(); 
  const std::type_info& typeinfo() const; 
  const std::type_info& elementTypeinfo() const; 
  
private: 
  Muon::CscPrepDataContainer::const_iterator m_contItr; 
  Muon::CscPrepDataContainer::const_iterator m_contEnd; 
  
  Muon::CscPrepDataCollection::const_iterator m_collItr; 
  Muon::CscPrepDataCollection::const_iterator m_collEnd; 
}; 

// Dummies --- the default versions of these don't compile.
// But these are overridden by the derived class here.
template<>
const std::type_info&
CollectionGetterTool<Muon::CscPrepDataContainer>::elementTypeinfo() const;

template<>
const void*
CollectionGetterTool<Muon::CscPrepDataContainer>::nextUntyped();

} 

#endif
