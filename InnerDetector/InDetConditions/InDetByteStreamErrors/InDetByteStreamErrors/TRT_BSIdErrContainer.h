/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDetByteStreamErrors package
 -----------------------------------------
 ***************************************************************************/


#ifndef TRT_BYTESTREAMERRORS_TRT_BSIDERRCONTAINER_H
#define TRT_BYTESTREAMERRORS_TRT_BSIDERRCONTAINER_H


#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

#include <stdint.h>

typedef              // <type, <index, header> >
  DataVector<std::pair<uint8_t, std::pair<uint32_t, uint8_t> > > TRT_BSIdErrContainer; 

CLASS_DEF(TRT_BSIdErrContainer,1119715969,1)


#endif // TRT_BYTESTREAMERRORS_TRT_BSIDERRCONTAINER_H
