/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 
 -------------------
 ATLAS Collaboration
 ***************************************************************************/

#include "SiSpacePointFormation/SiElementPropertiesTable.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiLocalPosition.h" 

namespace InDet{

SiElementPropertiesTable::SiElementPropertiesTable(const SCT_ID&  idHelper,
    const InDetDD::SiDetectorElementCollection& elements,
    float   epsilonWidth) 
    {
  size_t maxSCT = idHelper.wafer_hash_max();
  m_properties.reserve(maxSCT);
  for (size_t i = 0; i < maxSCT; ++i){
     IdentifierHash hash(i);
     const InDetDD::SiDetectorElement* element = elements[hash]; 
     if (element != 0){ 
       m_properties.emplace_back(hash, idHelper,*element,epsilonWidth);
     }
  }
}
//--------------------------------------------------------------------------

}
