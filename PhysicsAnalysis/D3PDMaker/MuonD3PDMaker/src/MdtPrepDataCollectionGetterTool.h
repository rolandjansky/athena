/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MdtPrepDataCollectionGetterTool.h 535250 2013-01-29 13:22:17Z salekd $
#ifndef MUOND3PDMAKER_MDTPREPDATACOLLECTIONGETTERTOOL_H
#define MUOND3PDMAKER_MDTPREPDATACOLLECTIONGETTERTOOL_H

#include "D3PDMakerUtils/SGCollectionGetterTool.h" 
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"

//namespace Muon {
//  class MdtPrepData;
//}

namespace D3PD { 
  
class MdtPrepDataCollectionGetterTool 
  : public SGCollectionGetterTool<Muon::MdtPrepDataContainer>
{ 
public: 
  typedef SGCollectionGetterTool<Muon::MdtPrepDataContainer> Base; 

  MdtPrepDataCollectionGetterTool (const std::string& type,
				  const std::string& name, 
				  const IInterface* parent); 

  StatusCode initialize(); 
  size_t sizeHint(bool=false); 
  StatusCode reset(bool=false); 
  const void* nextUntyped(); 
  const std::type_info& typeinfo() const; 
  const std::type_info& elementTypeinfo() const; 
  
private: 
  Muon::MdtPrepDataContainer::const_iterator m_contItr; 
  Muon::MdtPrepDataContainer::const_iterator m_contEnd; 
  
  Muon::MdtPrepDataCollection::const_iterator m_collItr; 
  Muon::MdtPrepDataCollection::const_iterator m_collEnd; 
}; 

// Dummies --- the default versions of these don't compile.
// But these are overridden by the derived class here.
template<>
const std::type_info&
CollectionGetterTool<Muon::MdtPrepDataContainer>::elementTypeinfo() const;

template<>
const void*
CollectionGetterTool<Muon::MdtPrepDataContainer>::nextUntyped();

} 

#endif
