/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDetByteStreamErrors package
 -----------------------------------------
 ***************************************************************************/


#ifndef TRT_BYTESTREAMERRORS_TRT_BSERRCONTAINER_H
#define TRT_BYTESTREAMERRORS_TRT_BSERRCONTAINER_H

#include <stdint.h>
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include <stdint.h>

typedef DataVector<std::pair<uint8_t, uint32_t> > TRT_BSErrContainer; // <type, index>

CLASS_DEF(TRT_BSErrContainer,1324847776,1)


#endif // TRT_BYTESTREAMERRORS_TRT_BSERRCONTAINER_H
