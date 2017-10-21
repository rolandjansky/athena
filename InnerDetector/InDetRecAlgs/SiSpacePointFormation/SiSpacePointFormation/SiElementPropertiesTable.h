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

class SiElementPropertiesTable final 
{
public:
    
    SiElementPropertiesTable(const SCT_ID&					idHelper,
			     const InDetDD::SiDetectorElementCollection&	elements, 
			     float						epsilonWidth); 
    ~SiElementPropertiesTable();

    const std::vector<IdentifierHash>*	neighbours(const IdentifierHash& waferID) const;
    float				halfWidth(IdentifierHash hashID) const;
    
private:
    std::vector<SiElementProperties*>		m_properties;
    
};


inline const std::vector<IdentifierHash>*
SiElementPropertiesTable::neighbours(const IdentifierHash& waferID) const
{
    return (m_properties[(unsigned int)waferID])->neighbours();
}

inline float
SiElementPropertiesTable::halfWidth(IdentifierHash waferID) const
{
    return (m_properties[(unsigned int)waferID])->halfWidth();
}

}
#endif // SISPACEPOINTFORMATION_SIELEMENTPROPERTIESTABLE_H
