/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AUTHOR: Noel Dawe
#ifndef TYPES_H
#define TYPES_H

namespace TauID
{
    namespace Types
    {
        enum MethodType
        {
            DUMMY,
            CUTS,
            BDT,
            LLH,
            TRANSFORM
        };
    }
}

#endif // TYPES_H
