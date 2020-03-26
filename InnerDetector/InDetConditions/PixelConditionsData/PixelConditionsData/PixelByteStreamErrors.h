/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelConditionsData_PixelByteStreamErrors_h
#define PixelConditionsData_PixelByteStreamErrors_h
#include "InDetByteStreamErrors/IDCInDetBSErrContainer.h"

// errors can coexist per hashID so we need to reserve separate bit for them
namespace PixelByteStreamErrors {

  //!< Possible errors in pixel data decoding, exactl clone of definition in: PixelConditionsTools/IPixelByteStreamErrorsTool.h, the later will be removed
  enum ErrorType {TimeOut=0, firstErrType=TimeOut, BCID=1, LVL1ID=2, Preamble=3, Trailer=4,
		  Flagged=5, DisableFE=6, ROD=7, Decoding=8, Invalid=9, LinkMaskedByPPC=10, Limit=11, lastErrType=Limit, ErrorsNumber=lastErrType+1 };

  //!< @brief helper to be used in clients to fetch error information
  inline bool hasError(IDCInDetBSErrContainer::ErrorCode errWord,  PixelErrorsEnum errType ) { return errWord & (1<<errType); }

  //!< @brief helper to set the error: @example errors[hashId] = addError( errors[hashId], PixelByteStreamErrors::Invalid ) 
  inline IDCInDetBSErrContainer::ErrorCode addError(IDCInDetBSErrContainer::ErrorCode errWord,  PixelErrorsEnum errType ) { return errWord | (1<<errType); }
}


#endif //PixelConditionsData_PixelByteStreamErrors_h
