/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 
 -------------------
 ATLAS Collaboration
 ***************************************************************************/

#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h" 
#include "InDetReadoutGeometry/SiLocalPosition.h" 
#include "SiSpacePointFormation/SiElementPropertiesTable.h"

namespace InDet{

SiElementPropertiesTable::SiElementPropertiesTable(const SCT_ID&  idHelper,
    const InDetDD::SiDetectorElementCollection& elements,
    float   epsilonWidth) 
    : m_elements(elements){
  m_maxSCT = idHelper.wafer_hash_max();
  m_properties.reserve(m_maxSCT);
  for (int i = 0; i < m_maxSCT; ++i){
     IdentifierHash hash(i);
     const InDetDD::SiDetectorElement* element = m_elements[hash]; 
     if (element != 0){ 
       SiElementProperties* props = new SiElementProperties(hash, idHelper,*element,epsilonWidth);
       m_properties[i] = props;
     }else{ 
       m_properties[i] = 0;
     }
  }
}
//--------------------------------------------------------------------------

SiElementPropertiesTable::~SiElementPropertiesTable(){
  for (int i=0; i < m_maxSCT; ++i){
    delete m_properties[i];m_properties[i] =0;
  }
}
//--------------------------------------------------------------------------

}
