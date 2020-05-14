// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ByteStreamErrors.h
 * @brief Define SCT byte stream errors and utility methods.
 * @author Susumu.Oda@cern.ch
 **/

#ifndef SCT_ByteStreamErrors_h
#define SCT_ByteStreamErrors_h

#include <array>
#include <vector>

/**
 * @def SCT_ERRORTYPELIST
 * @brief For enum to string conversion
 * http://stackoverflow.com/questions/21456262/enum-to-string-in-c11
 **/
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
  XYZ(RODSimulatedData)        \
  XYZ(NUM_ERROR_TYPES) // always have this one last, so we can use it as a loop index
#endif // SCT_ERRORTYPELIST

#ifndef SCT_DO_ENUM
#define SCT_DO_ENUM(e) e,
#endif // SCT_DO_ENUM

#ifndef SCT_DO_DESCRIPTION
#define SCT_DO_DESCRIPTION(e) #e,
#endif // SCT_DO_DESCRIPTION

namespace SCT_ByteStreamErrors {
  /**
   * @enum ErrorType
   * @brief SCT byte stream error type enums used in SCT_RodDecoder,
   * SCT_ByteStreamErrorsTool, SCTErrMonAlg.
   **/
  enum ErrorType {
    SCT_ERRORTYPELIST(SCT_DO_ENUM)
  };
  /**
   * @var ErrorTypeDescription
   * @brief SCT byte stream error type strings used in SCTErrMonAlg.
   **/
  static const std::vector<std::string> ErrorTypeDescription = {
    SCT_ERRORTYPELIST(SCT_DO_DESCRIPTION)
  };
  /**
   * @var BadErrors
   * @brief Bad error enums used in SCT_ByteStreamErrorsTool
   * and SCTErrMonAlg.
   **/
  static const std::vector<ErrorType> BadErrors = {
    TimeOutError,
    BCIDError,
    LVL1IDError,
    HeaderTrailerLimitError,
    MaskedLink,
    TruncatedROD,
    ROBFragmentError,
    MissingLinkHeaderError,
    MaskedROD
  };
  /**
   * @var LinkLevelBadErrors
   * @brief Bad error enums in FE-link level used in SCTErrMonAlg.
   **/
  static const std::vector<ErrorType> LinkLevelBadErrors = {
    TimeOutError,
    BCIDError,
    LVL1IDError,
    HeaderTrailerLimitError,
    MaskedLink
  };
  /**
   * @var RodLevelBadErrors
   * @brief Bad error enums in ROD level used in SCTErrMonAlg.
   **/
  static const std::vector<ErrorType> RodLevelBadErrors = {
    TruncatedROD,
    ROBFragmentError,
    MissingLinkHeaderError, // We cannot know which FE-link does not have header. We assign this error to all the FE-links of the ROD.
    MaskedROD
  };
  /**
   * @var LinkLevelErrors
   * @brief Error enums in FE-link level used in SCTErrMonAlg (assigned by SCT_RodDecoder::addSingleError)
   **/
  static const std::vector<ErrorType> LinkLevelErrors = {
    ByteStreamParseError,
    TimeOutError,
    BCIDError,
    LVL1IDError,
    PreambleError,
    FormatterError,
    TrailerError,
    TrailerOverflowError,
    HeaderTrailerLimitError,
    ABCDError,
    RawError,
    MaskedLink,
    ABCDError_Chip0,
    ABCDError_Chip1,
    ABCDError_Chip2,
    ABCDError_Chip3,
    ABCDError_Chip4,
    ABCDError_Chip5,
    ABCDError_Error1,
    ABCDError_Error2,
    ABCDError_Error4,
    TempMaskedChip0,
    TempMaskedChip1,
    TempMaskedChip2,
    TempMaskedChip3,
    TempMaskedChip4,
    TempMaskedChip5,
    ABCDError_Error7
  };
  /**
   * @var RodLevelErrors
   * @brief Error enums in ROD level used in SCTErrMonAlg (assigned by SCT_RodDecoder::addRODError)
   **/
  static const std::vector<ErrorType> RodLevelErrors = {
    RODClockError,
    TruncatedROD,
    ROBFragmentError,
    MissingLinkHeaderError, // We cannot know which FE-link does not have header. We assign this error to all the FE-links of the ROD.
    MaskedROD
  };

  template<ErrorType et> static constexpr uint64_t maskUpTo() { return  ( uint64_t(1) << et ) - 1; }
  static constexpr uint64_t ABCDErrorMask() { return maskUpTo<ABCDError_Error4>() & ~(maskUpTo<ABCDError_Chip0>()); }
  static constexpr uint64_t TempMaskedChipsMask() { return maskUpTo<TempMaskedChip5>() & ~(maskUpTo<TempMaskedChip0>()); }
  inline ErrorType TempMaskedChipToBit(int chip){ return std::array<ErrorType, 6>{{
         TempMaskedChip0,
         TempMaskedChip1,
         TempMaskedChip2,
         TempMaskedChip3,
         TempMaskedChip4,
         TempMaskedChip5}}[chip]; }

  /**
   * @struct ROOT6_NamespaceAutoloadHook
   * @brief Ensure that the enums are available from ROOT
   */
  struct ROOT6_NamespaceAutoloadHook{};
}

#endif // SCT_ByteStreamErrors_h
