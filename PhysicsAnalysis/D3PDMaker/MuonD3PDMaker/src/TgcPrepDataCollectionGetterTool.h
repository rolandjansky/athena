/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TgcPrepDataCollectionGetterTool.h 535250 2013-01-29 13:22:17Z salekd $
#ifndef MUOND3PDMAKER_TGCPREPDATACOLLECTIONGETTERTOOL_H
#define MUOND3PDMAKER_TGCPREPDATACOLLECTIONGETTERTOOL_H

#include "D3PDMakerUtils/SGCollectionGetterTool.h" 
#include "MuonPrepRawData/TgcPrepData.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"

//namespace Muon {
//  class TgcPrepData;
//}

namespace D3PD { 
  
class TgcPrepDataCollectionGetterTool 
  : public SGCollectionGetterTool<Muon::TgcPrepDataContainer>
{ 
public: 
  typedef SGCollectionGetterTool<Muon::TgcPrepDataContainer> Base; 

  TgcPrepDataCollectionGetterTool (const std::string& type,
				  const std::string& name, 
				  const IInterface* parent); 

  StatusCode initialize(); 
  size_t sizeHint(bool=false); 
  StatusCode reset(bool=false); 
  const void* nextUntyped(); 
  const std::type_info& typeinfo() const; 
  const std::type_info& elementTypeinfo() const; 
  
private: 
  Muon::TgcPrepDataContainer::const_iterator m_contItr; 
  Muon::TgcPrepDataContainer::const_iterator m_contEnd; 
  
  Muon::TgcPrepDataCollection::const_iterator m_collItr; 
  Muon::TgcPrepDataCollection::const_iterator m_collEnd; 
}; 

// Dummies --- the default versions of these don't compile.
// But these are overridden by the derived class here.
template<>
const std::type_info&
CollectionGetterTool<Muon::TgcPrepDataContainer>::elementTypeinfo() const;

template<>
const void*
CollectionGetterTool<Muon::TgcPrepDataContainer>::nextUntyped();

} 

#endif
