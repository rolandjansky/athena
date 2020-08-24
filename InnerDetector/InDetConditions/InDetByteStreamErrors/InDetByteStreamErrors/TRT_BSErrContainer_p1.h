/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDetByteStreamErrors package
 -----------------------------------------
 ***************************************************************************/


#ifndef TRT_BYTESTREAMERRORS_TRT_BSERRCONTAINER_P1_H
#define TRT_BYTESTREAMERRORS_TRT_BSERRCONTAINER_P1_H

#include <stdint.h>
#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <stdint.h>

typedef DataVector<std::pair<uint8_t, uint32_t> > TRT_BSErrContainer_p1; // <type, index>

CLASS_DEF(TRT_BSErrContainer_p1,1114762661,1)


#endif // TRT_BYTESTREAMERRORS_TRT_BSERRCONTAINER_H
