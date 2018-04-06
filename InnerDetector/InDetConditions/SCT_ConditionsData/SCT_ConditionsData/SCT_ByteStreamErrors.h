/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ByteStreamErrors.h
 * Define SCT byte stream errors
 * @author Susumu.Oda@cern.ch
 **/

#ifndef SCT_ByteStreamErrors_h
#define SCT_ByteStreamErrors_h

// http://stackoverflow.com/questions/21456262/enum-to-string-in-c11
#ifndef SCT_ERRORTYPELIST
#define SCT_ERRORTYPELIST(XYZ) \
  XYZ(ByteStreamParseError)    \
  XYZ(TimeOutError)            \
  XYZ(BCIDError)               \
  XYZ(LVL1IDError)             \
  XYZ(PreambleError)           \
  XYZ(FormatterError)          \
  XYZ(TrailerError)            \
  XYZ(TrailerOverflowError)    \
  XYZ(HeaderTrailerLimitError) \
  XYZ(ABCDError)               \
  XYZ(RawError)                \
  XYZ(MaskedLink)              \
  XYZ(RODClockError)           \
  XYZ(TruncatedROD)            \
  XYZ(ROBFragmentError)        \
  XYZ(MissingLinkHeaderError)  \
  XYZ(MaskedROD)               \
  XYZ(ABCDError_Chip0)         \
  XYZ(ABCDError_Chip1)         \
  XYZ(ABCDError_Chip2)         \
  XYZ(ABCDError_Chip3)         \
  XYZ(ABCDError_Chip4)         \
  XYZ(ABCDError_Chip5)         \
  XYZ(ABCDError_Error1)        \
  XYZ(ABCDError_Error2)        \
  XYZ(ABCDError_Error4)        \
  XYZ(TempMaskedChip0)         \
  XYZ(TempMaskedChip1)         \
  XYZ(TempMaskedChip2)         \
  XYZ(TempMaskedChip3)         \
  XYZ(TempMaskedChip4)         \
  XYZ(TempMaskedChip5)         \
  XYZ(ABCDError_Error7)        \
  XYZ(ABCDError_Invalid)       \
  XYZ(NUM_ERROR_TYPES) // always have this one last, so we can use it as a loop index
#endif // SCT_ERRORTYPELIST

#ifndef SCT_DO_ENUM
#define SCT_DO_ENUM(e) e,
#endif // SCT_DO_ENUM

#ifndef SCT_DO_DESCRIPTION
#define SCT_DO_DESCRIPTION(e) #e,
#endif // SCT_DO_DESCRIPTION

namespace SCT_ByteStreamErrors {
  enum errorTypes {
    SCT_ERRORTYPELIST(SCT_DO_ENUM)
  };
  static const std::string errorTypesDescription[]{
    SCT_ERRORTYPELIST(SCT_DO_DESCRIPTION)
  };
}

#endif // SCT_ByteStreamErrors_h
