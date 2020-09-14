/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelConditionsData_PixelByteStreamErrors_h
#define PixelConditionsData_PixelByteStreamErrors_h
#include "InDetByteStreamErrors/IDCInDetBSErrContainer.h"

// errors can coexist per hashID so we need to reserve separate bit for them
namespace PixelByteStreamErrors {

  //!< Possible errors in pixel data decoding, exactl clone of definition in: PixelConditionsTools/IPixelByteStreamErrorsTool.h, the later will be removed
  enum PixelErrorsEnum {TimeOut=0, firstErrType=TimeOut, BCID, LVL1ID, Preamble, Trailer,
			Flagged, DisableFE, BadFE, ROD, Decoding, Invalid, LinkMaskedByPPC, Limit, TruncatedROB, MaskedROB, lastErrType=MaskedROB, ErrorsNumber=lastErrType+1 };

  //!< @brief for cases when error doe snot need to be accumulated
  inline IDCInDetBSErrContainer::ErrorCode makeError( PixelErrorsEnum errType ) { return IDCInDetBSErrContainer::ErrorCode{1} << errType; }

  //!< @brief helper to be used in clients to fetch error information
  inline bool hasError(IDCInDetBSErrContainer::ErrorCode errWord,  PixelErrorsEnum errType ) { return errWord & makeError( errType ); }

  //!< @brief helper to set the error: @example errors[hashId] = addError( errors[hashId], PixelByteStreamErrors::Invalid )
  inline void addError(IDCInDetBSErrContainer::ErrorCode& errWord,  PixelErrorsEnum errType ) { errWord |= makeError( errType ); }

  inline void addServiceRecords(IDCInDetBSErrContainer::ErrorCode& errWord, uint64_t errorCount ) { errWord = errorCount; }

}


#endif //PixelConditionsData_PixelByteStreamErrors_h
