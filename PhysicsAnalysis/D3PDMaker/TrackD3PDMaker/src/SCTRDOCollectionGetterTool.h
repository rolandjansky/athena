/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_SCTRDOCOLLECTIONGETTERTOOL_H
#define TRACKD3PDMAKER_SCTRDOCOLLECTIONGETTERTOOL_H

#include "D3PDMakerUtils/SGCollectionGetterTool.h"

#include "AthenaKernel/CLASS_DEF.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/SCT_RDO_Collection.h"
#include "InDetRawData/SCT_RDORawData.h"
#include "InDetRawData/SCT_RDO_Collection.h" // Needed to pick up CLID.

class SCT_ID;


namespace D3PD {

class SCTRDOCollectionGetterTool
  : public D3PD::SGCollectionGetterTool<SCT_RDO_Container>
{
public:
  typedef D3PD::SGCollectionGetterTool<SCT_RDO_Container> Base;

  SCTRDOCollectionGetterTool (const std::string& type,
			const std::string& name,
			const IInterface* parent);

  StatusCode initialize();
  size_t sizeHint(bool=false);
  StatusCode reset(bool=false);
  const void* nextUntyped();

  const std::type_info& typeinfo() const;
  const std::type_info& elementTypeinfo() const;

private:

  /* Container iterators **/
  SCT_RDO_Container::const_iterator m_contItr;
  SCT_RDO_Container::const_iterator m_contEnd;

  /* Collection iterators **/
  SCT_RDO_Collection::const_iterator m_collItr;
  SCT_RDO_Collection::const_iterator m_collEnd;
 
}; // class SCTRDOCollectionGetterTool

// Dummies --- the default versions of these don't compile.
// But these are overridden by the derived class here.
template<>
const std::type_info&
CollectionGetterTool<SCT_RDO_Container>::elementTypeinfo() const;

template<>
const void*
CollectionGetterTool<SCT_RDO_Container>::nextUntyped();

} // namespace D3PD

#endif // not TRACKD3PDMAKER_SCTRDOCOLLECTIONGETTERTOOL_H
