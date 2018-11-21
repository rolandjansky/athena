/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelRDOCollectionGetterTool.h"
#include "GaudiKernel/MsgStream.h"


namespace D3PD {

PixelRDOCollectionGetterTool::PixelRDOCollectionGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
}

StatusCode PixelRDOCollectionGetterTool::initialize()
{
  CHECK( Base::initialize() );

  // Get Particle Data Table

  return StatusCode::SUCCESS;
}

const std::type_info& PixelRDOCollectionGetterTool::typeinfo() const
{
  return typeid(PixelRDO_Container);
}


const std::type_info& PixelRDOCollectionGetterTool::elementTypeinfo() const
{
  return typeid(PixelRDORawData);
}


size_t PixelRDOCollectionGetterTool::sizeHint(bool allowMissing)
{

  const PixelRDO_Container* cont = get(allowMissing);


  if (!cont) {
    return 0;
  }

  int nRDOs = 0;

  PixelRDO_Container::const_iterator col_It  = cont->begin();
  PixelRDO_Container::const_iterator lastCol = cont->end();
  for( ; col_It!= lastCol ; ++col_It){

     const PixelRDO_Collection* Pixel_Collection(*col_It);
     if (!Pixel_Collection) continue;  // select only Pixel RDOs
     nRDOs += Pixel_Collection->size();
   }

  return nRDOs;
}


StatusCode PixelRDOCollectionGetterTool::reset(bool allowMissing)
{
  
  const PixelRDO_Container* cont = get(allowMissing);

  if (!cont) {
    m_contItr = m_contEnd;
    m_collItr = m_collEnd;
    return StatusCode::FAILURE;
  }

  m_contItr = cont->begin();
  m_contEnd = cont->end();

  if(m_contItr == m_contEnd){
    m_collItr = m_collEnd;
    return StatusCode::SUCCESS;
  }

  m_collItr = (*m_contItr)->begin();
  m_collEnd = (*m_contItr)->end();

  return StatusCode::SUCCESS;
}

const void* PixelRDOCollectionGetterTool::nextUntyped()
{

  const PixelRDORawData* rawDataObject = 0;

  do{

    if(m_contItr == m_contEnd){
      return 0;
    }

    if (m_collItr == m_collEnd)
      rawDataObject = 0;
    else {
      rawDataObject = *m_collItr;
      m_collItr++;
    }

    if(m_collItr == m_collEnd){
      m_contItr++;
      if(m_contItr != m_contEnd){
	m_collItr = (*m_contItr)->begin();
	m_collEnd = (*m_contItr)->end();
      }
    }

  } while(!rawDataObject);

  return rawDataObject;

}

// Dummies --- the default versions of these don't compile.
// But these are overridden by the derived class here.
template<>
const std::type_info&
CollectionGetterTool<PixelRDO_Container>::elementTypeinfo() const
{
  return typeid(PixelRDORawData);
}

template<>
const void*
CollectionGetterTool<PixelRDO_Container>::nextUntyped()
{
  return 0;
}


} // namespace D3PD
