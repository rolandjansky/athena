/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ElementTable_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ELEMENTTABLE_P1_TRK_H
#define ELEMENTTABLE_P1_TRK_H

#include <vector>
#include <climits>
#include <string>
#include "AthenaPoolUtilities/TPObjRef.h"
#include "TrkDetDescrTPCnv/TrkGeometry/Material_p1.h"

namespace Trk
{
    /** @class ElementTable_p1 
            
         elements & fractions as unsigned char - if filled
        */
    class ElementTable_p1
    {
      public:
        ElementTable_p1() :
         table(),
         names()
        {}
        
        mutable std::vector<Material_p1>    table;
        mutable std::vector<std::string>    names;
    };
}

#endif // ELEMENTTABLE_P1_TRK_H
