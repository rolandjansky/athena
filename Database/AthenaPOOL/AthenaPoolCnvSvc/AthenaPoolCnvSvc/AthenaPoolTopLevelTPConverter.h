/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaPoolTopLevelTPConverter_H
#define AthenaPoolTopLevelTPConverter_H

#include "TPTools/TopLevelTPConverter.h"

template< class MAIN_CNV, class TL_PERS >
using AthenaPoolTopLevelTPConverter = TopLevelTPConverter<MAIN_CNV, TL_PERS>;

#endif
