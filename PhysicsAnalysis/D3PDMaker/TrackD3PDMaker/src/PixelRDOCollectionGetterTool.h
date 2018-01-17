/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_PIXELRDOCOLLECTIONGETTERTOOL_H
#define TRACKD3PDMAKER_PIXELRDOCOLLECTIONGETTERTOOL_H

#include "D3PDMakerUtils/SGCollectionGetterTool.h"

#include "AthenaKernel/CLASS_DEF.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/PixelRDO_Container.h"
#include "InDetRawData/PixelRDO_Collection.h"
#include "InDetRawData/PixelRDORawData.h"
//#include "InDetRawData/SCT_RDO_Collection.h" // Needed to pick up CLID.

class PixelID;


namespace D3PD {

class PixelRDOCollectionGetterTool
  : public D3PD::SGCollectionGetterTool<PixelRDO_Container>
{
public:
  typedef D3PD::SGCollectionGetterTool<PixelRDO_Container> Base;

  PixelRDOCollectionGetterTool (const std::string& type,
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
  PixelRDO_Container::const_iterator m_contItr;
  PixelRDO_Container::const_iterator m_contEnd;

  /* Collection iterators **/
  PixelRDO_Collection::const_iterator m_collItr;
  PixelRDO_Collection::const_iterator m_collEnd;
 
}; // class PixelRDOCollectionGetterTool

// Dummies --- the default versions of these don't compile.
// But these are overridden by the derived class here.
template<>
const std::type_info&
CollectionGetterTool<PixelRDO_Container>::elementTypeinfo() const;

template<>
const void*
CollectionGetterTool<PixelRDO_Container>::nextUntyped();

} // namespace D3PD

#endif // not TRACKD3PDMAKER_PIXELRDOCOLLECTIONGETTERTOOL_H
