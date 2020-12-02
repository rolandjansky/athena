/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MAGFIELDCONDITIONS_ATLASMTFIELDMAPCONDOBJ
#define MAGFIELDCONDITIONS_ATLASMTFIELDMAPCONDOBJ

// MagField includes
#include "AthenaKernel/CondCont.h" 
#include "MagFieldElements/AtlasFieldMap.h"

class AtlasFieldMapCondObj {

public:
    AtlasFieldMapCondObj();

    ~AtlasFieldMapCondObj();

    // access to field map 
    const MagField::AtlasFieldMap* fieldMap () const;

    // setter
    void setFieldMap(std::unique_ptr<MagField::AtlasFieldMap> fieldMap);
    
private:
    // field map 
    std::unique_ptr<MagField::AtlasFieldMap> m_fieldMap;
};


#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( AtlasFieldMapCondObj, 122915602, 1)
CONDCONT_DEF (AtlasFieldMapCondObj, 249359246);


#endif // MAGFIELDCONDITIONS_ATLASMTFIELDMAPCONDOBJ

