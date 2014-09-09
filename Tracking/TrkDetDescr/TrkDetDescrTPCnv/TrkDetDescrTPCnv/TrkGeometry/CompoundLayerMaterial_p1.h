/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// CompoundLayerMaterial_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef COMPOUNDLAYERMATERIAL_P1_TRK_H
#define COMPOUNDLAYERMATERIAL_P1_TRK_H

#include <vector>
#include "AthenaPoolUtilities/TPObjRef.h"
#include "TrkDetDescrTPCnv/TrkDetDescrUtils/BinUtility_p1.h"

typedef std::vector<unsigned char> ValueVector;
typedef std::vector< ValueVector > ValueMatrix;

namespace Trk
{
    
    /** @class CompoundLayerMaterial_p1 */
    class CompoundLayerMaterial_p1
    {
      public:
        CompoundLayerMaterial_p1()
        {
          valueMins.reserve(6);
          valueSteps.reserve(6);  
          valueMatrices.reserve(6);
          valueMatrices.reserve(6); 
        }

        BinUtility_p1                      binUtility;
        
        std::vector<float>                 valueMins;
        std::vector<float>                 valueSteps;
        std::vector< ValueMatrix >         valueMatrices;
        
        std::vector < std::vector < std::vector< std::pair<unsigned char, unsigned char> > > > composition;

    };
}

#endif // COMPOUNDLAYERMATERIAL_P1_TRK_H