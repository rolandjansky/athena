/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Stores properties that are needed in SiSpacePointFinder.
 --------------------------------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: SiElementPropertiesTable.h,v 1.3 2009-01-07 17:50:36 markuse Exp $

#ifndef SISPACEPOINTFORMATION_SIELEMENTPROPERTIESTABLE_H
#define SISPACEPOINTFORMATION_SIELEMENTPROPERTIESTABLE_H

#include <vector>
#include "Identifier/IdentifierHash.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h" 
#include "SiSpacePointFormation/SiElementProperties.h"

class SiDetectorElementCollection;

namespace InDet {

class SiElementPropertiesTable
{
public:
    
    SiElementPropertiesTable(const SCT_ID&					idHelper,
			     const InDetDD::SiDetectorElementCollection&	elements, 
			     float						epsilonWidth); 
    ~SiElementPropertiesTable();

    std::vector<IdentifierHash>*	neighbours(const IdentifierHash& waferID);
    float				halfWidth(IdentifierHash hashID);
    
private:
    std::vector<SiElementProperties*>		m_properties;
    const InDetDD::SiDetectorElementCollection&	m_elements; 
    int						m_maxSCT;
    std::vector<SiElementProperties*>::const_iterator m_propertiesBegin;
    
};


inline std::vector<IdentifierHash>*
SiElementPropertiesTable::neighbours(const IdentifierHash& waferID)
{
    return (m_properties[(unsigned int)waferID])->neighbours();
}

inline float
SiElementPropertiesTable::halfWidth(IdentifierHash waferID)
{
    return (m_properties[(unsigned int)waferID])->halfWidth();
}

}
#endif // SISPACEPOINTFORMATION_SIELEMENTPROPERTIESTABLE_H
