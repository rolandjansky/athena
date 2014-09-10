/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTRDOCollectionGetterTool.h"
#include "GaudiKernel/MsgStream.h"


namespace D3PD {

SCTRDOCollectionGetterTool::SCTRDOCollectionGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
}

StatusCode SCTRDOCollectionGetterTool::initialize()
{
  CHECK( Base::initialize() );

  // Get Particle Data Table

  return StatusCode::SUCCESS;
}

const std::type_info& SCTRDOCollectionGetterTool::typeinfo() const
{
  //return typeid(SCT_RDO_Container);
  return typeid(SCT_RDO_Container);
}


const std::type_info& SCTRDOCollectionGetterTool::elementTypeinfo() const
{
  return typeid(SCT_RDORawData);
}


size_t SCTRDOCollectionGetterTool::sizeHint(bool allowMissing)
{

  const SCT_RDO_Container* cont = get(allowMissing);


  if (!cont) {
    return 0;
  }

  int nRDOs = 0;

  SCT_RDO_Container::const_iterator col_It  = cont->begin();
  SCT_RDO_Container::const_iterator lastCol = cont->end();
  for( ; col_It!= lastCol ; ++col_It){

     const SCT_RDO_Collection* SCT_Collection(*col_It);
     if (!SCT_Collection) continue;  // select only SCT RDOs
     nRDOs += SCT_Collection->size();
   }

  return nRDOs;
}


StatusCode SCTRDOCollectionGetterTool::reset(bool allowMissing)
{
  
  const SCT_RDO_Container* cont = get(allowMissing);

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

const void* SCTRDOCollectionGetterTool::nextUntyped()
{

  const SCT_RDORawData* rawDataObject = 0;

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
CollectionGetterTool<SCT_RDO_Container>::elementTypeinfo() const
{
  return typeid(SCT_RDORawData);
}

template<>
const void*
CollectionGetterTool<SCT_RDO_Container>::nextUntyped()
{
  return 0;
}


} // namespace D3PD
