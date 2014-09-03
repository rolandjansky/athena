/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaPoolTopLevelTPConverter_H
#define AthenaPoolTopLevelTPConverter_H

//============  #define part
#define TopLevelTPConverter  AthenaPoolTopLevelTPConverter

// include "T_AthenaPoolTPConverter.h" to get #defines only, they need to come first
#include "T_AthenaPoolTPConverter.h"

//===========================================

#include "AthenaPoolTopLevelExtTPConverter.h"
#include "TPTools/TopLevelTPConverter.h"


//============== typedef part
#if 0
#ifndef __GCCXML__

template< class MAIN_CNV, class TL_PERS >
using AthenaPoolTopLevelTPConverter = TopLevelTPConverter<MAIN_CNV, TL_PERS>;

#else   // __GCCXML__

#define AthenaPoolTopLevelTPConverter  TopLevelTPConverter

#endif // __GCCXML__
#endif
//=========================================

#endif
