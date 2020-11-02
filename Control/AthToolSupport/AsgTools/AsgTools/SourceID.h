/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak



#ifndef ASG_TOOLS__SOURCE_ID_H
#define ASG_TOOLS__SOURCE_ID_H

#ifndef XAOD_STANDALONE
#include <AthenaKernel/SourceID.h>
#else

namespace SG {

// Type used to identify a metadata source.
// For compatibility with Athena releases
typedef std::string SourceID;

}

#endif

#endif
