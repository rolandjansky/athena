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
 * Specifications of the data format and errors is found in
 * [1] http://www-eng.lbl.gov/~jmjoseph/Atlas-SiROD/Manuals/usersManual-v164.pdf
 * [2] http://cds.cern.ch/record/683741/files/
 * [3] https://twiki.cern.ch/twiki/bin/view/Atlas/SCTDaqErrors
 * [4] https://twiki.cern.ch/twiki/bin/view/Atlas/ROBINFragmentErrors#Definition_of_the_first_status_e
 *
 * The following is almost a copy of description of 
 * https://its.cern.ch/jira/browse/ATLASRECTS-4444
 *
 * Link header : table 27 of [1]
 * 001ptlbKdMMMMMMM
 * d = link masked by DSP (MaskedLink)
 * t = time out error (TimeOutError)
 * p = Preamble Error (PreambleError)
 * l = L1 error (LVL1IDError)
 * b = BCID error (BCIDError)
 * M = link number (FormatterError if M>11)
 *
 * Link trailer : table 16 of [1]
 * 010zhvxxxxxxmmmm
 * z = trailer bit error (TrailerError)
 * h = header trailer limit error (HeaderTrailerLimitError)
 * v = data overflow error (HeaderTrailerLimitError)
 * m = dynamically masked chips to reduced data size at high pileup 
 *     Not written in [1]. (TempMaskedChipX, X=0-5)
 *
 *   0 means no masked chip (always has been 0 until April 2017)
 *
 *   If Rx redundacy is not used,
 *   1 means chips 0-5 are temporarily masked.
 *     (TempMaskedChip0-TempMaskedChip5 in link-0)
 *   6 means chip 5 is temporarily masked.
 *     (TempMaskedChip5 in link-0)
 *   7 means chips 6-11 are temporarily masked.
 *     (TempMaskedChip0-TempMaskedChip5 in link-1)
 *   12 means chip 11 is temporarily masked.
 *     (TempMaskedChip5 in link-1)
 *
 *   If Rx redundacy is used and link-1 is not used,
 *   1 means chips 0-11 are temporarily masked.
 *     (TempMaskedChip0-TempMaskedChip5 in both link-0 and link-1)
 *   6 means chips 5-11 are temporarily masked.
 *     (TempMaskedChip5 in link-0 and TempMaskedChip0-TempMaskedChip5 in link-1)
 *   7 means chips 6-11 are temporarily masked.
 *     (TempMaskedChip0-TempMaskedChip5 in link-1)
 *   12 means chip 11 is temporarily masked.
 *     (TempMaskedChip5 in link-1)
 *
 *   If Rx redundacy is used and link-0 is not used,
 *   1 means chips 0-5 are temporarily masked.
 *     (TempMaskedChip0-TempMaskedChip5 in link-0)
 *   6 means chip 5 is temporarily masked.
 *     (TempMaskedChip5 in link-0)
 *   7 means chips 6-11, 0-5 are temporarily masked.
 *     (TempMaskedChip0-TempMaskedChip5 in both link-0 and -1)
 *   12 means chips 11, 0-5 are temporarily masked.
 *     (TempMaskedChip0-TempMaskedChip5 in link-0 and TempMaskedChip5 in link-1)
 * 
 * ABCD error : table 16 of [1]
 * 000xxxxxxFFFFEEE
 * F = FE number (ABCDError_ChipX, ABCDError_Invalid)
 * E = ABC error code (ABCDError_ErrorX, ABCDError_Invalid)
 *
 * Raw data : table 16 of [1]
 * 011nnnxxWWWWWWWW
 * n = count of raw data bits + 1
 * W = raw data
 * (RawError)
 * 
 * ByteStreamParseError
 * When invalid data word is found, this BS error is assigned.
 * 
 * MissingLinkHeaderError
 * When hit, ABCD error or raw data appear earlier than header, this BS error is 
 * assigned. Since we cannot know which FE-link does not have header, we assign
 * this error to all the FE-links without hit, ABCD error or raw data of the ROD.
 *
 * ROD status words : table 32 of [1]
 * Bit 16 TIM Clock Error (RODClockError)
 * Bit 17 BOC Clock Error (RODClockError)
 * Bit 20-23 DCS HV bits (Expanded. Not written in [1])
 *  
 * ROB status word : [4]
 * Non-zero status word (ROBFragmentError)
 * Bit 31 Discard: the ROBIN did not have a fragment for the requested L1ID 
 *        because it is in "discard" mode. 
 *        It therefore generated an empty fragment
 * Bit 29 Lost: the ROBIN did not have a fragment for the requested L1ID.
 *        It therefore generated an empty fragment (MaskedROD)
 * Bit 27 Truncation: the amount of data sent across S-Link for this fragment 
 *        was larger than the maximum fragment size the ROBIN was configured to
 *        handle. Therefore this fragment has been truncated (TruncatedROD)
 *
 * The detector event type of ROB (do not know where these are documented)
 * Bit 20 : RODSimulatedData
 * Bit 21 : Super-condensed mode (not BS error)
 **/

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
    MissingLinkHeaderError, 
    // We cannot know which FE-link does not have header. We assign this error
    // to all the FE-links without hit, ABCD error or raw data of the ROD.
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
  // Bit mask for ABCDError_Chip0, ABCDError_Chip1, ..., ABCDError_Chip5
  static constexpr uint64_t ABCDErrorMask() { return maskUpTo<ABCDError_Chip5>() & ~(maskUpTo<ABCDError_Chip0>()); }
  // Bit mask for TempMaskedChip0, TempMaskedChip1, ..., TempMaskedChip5
  static constexpr uint64_t TempMaskedChipsMask() { return maskUpTo<TempMaskedChip5>() & ~(maskUpTo<TempMaskedChip0>()); }
  inline ErrorType TempMaskedChipToBit(const int chip){ return std::array<ErrorType, 6>{{
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
