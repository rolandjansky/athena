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
    {
  size_t maxSCT = idHelper.wafer_hash_max();
  m_properties.assign(maxSCT, nullptr);
  for (size_t i = 0; i < maxSCT; ++i){
     IdentifierHash hash(i);
     const InDetDD::SiDetectorElement* element = elements[hash]; 
     if (element != 0){ 
       SiElementProperties* props = new SiElementProperties(hash, idHelper,*element,epsilonWidth);
       m_properties[i] = props;
     }
  }
}
//--------------------------------------------------------------------------

SiElementPropertiesTable::~SiElementPropertiesTable(){
  size_t maxSCT = m_properties.size();
  for (size_t i=0; i < maxSCT; ++i){
    delete m_properties[i];m_properties[i] =0;
  }
}
//--------------------------------------------------------------------------

}
