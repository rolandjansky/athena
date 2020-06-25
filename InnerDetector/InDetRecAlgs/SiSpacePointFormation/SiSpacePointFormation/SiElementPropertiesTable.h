/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Stores properties that are needed in SiSpacePointFinder.
 --------------------------------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: SiElementPropertiesTable.h,v 1.3 2009-01-07 17:50:36 markuse Exp $

#ifndef SISPACEPOINTFORMATION_SIELEMENTPROPERTIESTABLE_H
#define SISPACEPOINTFORMATION_SIELEMENTPROPERTIESTABLE_H

#include "SiSpacePointFormation/SiElementProperties.h"

#include "Identifier/IdentifierHash.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include <vector>

namespace InDet {

class SiElementPropertiesTable final 
{
public:
    
    SiElementPropertiesTable(const SCT_ID&					idHelper,
			     const InDetDD::SiDetectorElementCollection&	elements, 
			     float						epsilonWidth); 
    ~SiElementPropertiesTable() = default;

    const std::vector<IdentifierHash>*	neighbours(const IdentifierHash& waferID) const;
    float				halfWidth(IdentifierHash hashID) const;
    
private:
    std::vector<SiElementProperties>		m_properties;
    
};


inline const std::vector<IdentifierHash>*
SiElementPropertiesTable::neighbours(const IdentifierHash& waferID) const
{
    return (m_properties[(unsigned int)waferID]).neighbours();
}

inline float
SiElementPropertiesTable::halfWidth(IdentifierHash waferID) const
{
    return (m_properties[(unsigned int)waferID]).halfWidth();
}

}

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( InDet::SiElementPropertiesTable , 713446 , 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( InDet::SiElementPropertiesTable , 92686144 );

#endif // SISPACEPOINTFORMATION_SIELEMENTPROPERTIESTABLE_H
