/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
#ifndef TRIGINTERFACES_ENUMS_H
#define TRIGINTERFACES_ENUMS_H

#include <vector>
#include <string>
#include <stdint.h>

namespace HLT {
  struct Action {
    typedef enum  {
      CONTINUE    = 0,  //!< if all is OK the processing should be continued
      ABORT_CHAIN = 1,  //!< if things went wrong but it is not severe, other unrelated chains will continue
      ABORT_EVENT = 2,  //!< if things went wrong severely, event corruption suspected
      ABORT_JOB   = 3,   //!< if things go really wrong, i.e. user suspects that the job is badly configured and continuing it will lead to taking rubbish data
      UNSPECIFIED_ = -1 //!< do not use

    } Code;
  };

  struct Reason {
    typedef enum {
      UNKNOWN=0,
      MISSING_FEATURE,   //!< the feataure was expected but could not be found
      GAUDI_EXCEPTION,   //!< GAUDI exception was caught
      EFORMAT_EXCEPTION, //!< eformat exception was caught
      STD_EXCEPTION,     //!< std exeption was caught
      UNKNOWN_EXCEPTION, //!< unknown exception thrown
      NAV_ERROR,         //!< suspected Navigation error
      MISSING_ROD,       //!< while unpacking detector data ROD fragment is missing
      CORRUPTED_ROD,     //!< ROD with the detector data seems to be corrupted
      TIMEOUT,           //!< the timeout occured @see TrigInterfaces/AlgoConfig::timeout() for details
      BAD_JOB_SETUP,     //!< the job is possibly badly configured (2 places where this can occur: configuration, and runtime i.e. when FEX before given Hypo is always attaching feature but this time it is not there)

      USERDEF_1,         //!< this is custom user defined code: for clarity users should define consts i.e. const int NO_HITS = HLT::Reason::USERDEF_1 or #define NO_HITS (HLT::Reason::USERDEF_1) and then use this while creating ErrorCodes
      USERDEF_2,
      USERDEF_3,
      USERDEF_4,
      LAST_            =0xf,        //!< and can't be higher because of serialization in the chain
      UNSPECIFIED_ = -1 //!< do not use
    } Code;
  };




  struct SteeringInternalReason {
    typedef enum {
      UNKNOWN=0, //!<
      NO_LVL1_ITEMS,    //!< in the LVL1 result delivered to LVL2 none of LVL1 items are present, that means no seeding information is available
      NO_LVL2_CHAINS,   //!< in the LVL2 result unpacked at EF no active LVL2 chains are present, that means no seeding information is available
      NO_LVL1_RESULT,   //!< LVL1 result is missing
      WRONG_HLT_RESULT, //!< error while unpacking HLT reult (happnes at EF when LVL2 result is unpacked)
      NO_HLT_RESULT,    //!< no HLT result is present (means that LVL2 result was not delivered to EF)
      ALGO_ERROR,       //!< algorithms returned errors
      TIMEOUT,          //!< timeout forced by the steering
      BUSY,             //!< to flag that the event was busy
      BAD_JOB_SETUP,    //!< job is badly configured
      MISSING_CALO_ROI, //!< one ore more missing CALO RoIs
      MISSING_MUON_ROI, //!< one ore more missing MUON RoIs
      MISSING_OTHER_ROI,//!< one ore more missing RoI except CALO/MUON (e.g. MBTS, NIM, ...)
      LAST_,            //!< there is basically no restriction on size since these errors are not saved per chain so do not need to be serialized in 6 bits.
      UNSPECIFIED_ = -1 //!< do not use
    } Code;
  };

  /**
   * @class ErrorCode
   * @brief The definition of error codes in HLT.
   * This is a class (note; before was simple enum) to enable safer operations.
   * Each ErrorCode is composed out of 3 parts:
   * -# desired action which is one of enums defined in class Action
   * -# explnation of the reson of that action, defined in class Reason
   * -# explnation of the reson which is internal to the steering rather than algorithms, defined in SteeringInternalReason
   */
  class ErrorCode {
  public:
    ErrorCode();                                           //!< for algo developers
    ErrorCode(Action::Code a);                             //!< for algo developers
    ErrorCode(Action::Code a, Reason::Code r);             //!< for algo developers


    ErrorCode(uint32_t c);
    ErrorCode(const ErrorCode c, Reason::Code r);

    ErrorCode(Action::Code a, SteeringInternalReason::Code s);
    ErrorCode(const ErrorCode c, SteeringInternalReason::Code s);

    ErrorCode(Action::Code a, Reason::Code r, SteeringInternalReason::Code s);

    // comapricon operators
    bool operator==(const ErrorCode& ec) const;
    bool operator!=(const ErrorCode& ec) const;

    bool operator<(const ErrorCode& ec) const;
    bool operator>(const ErrorCode& ec) const;

    // conversions
    operator uint32_t() const {return code;}

    //    bool operator<(const ErrorCode& e1, const ErrorCode& e2 ) {return e1.code & 0x3;}// < e.code &0x3; }
    Action::Code action() const;
    Reason::Code reason() const;
    SteeringInternalReason::Code steeringInternalReason() const;

    std::string str();

    uint32_t code; //!< storage of error code where 4 least significant bits(0-3) are storing Reason, 2 next (4-5) are for Action and bits higher than 8 for SteeringInternalReason
  };

  std::string strErrorCode(const ErrorCode code);
  //  const std::string& str(const Reason::Code code) { return "U"; }


  //bunch of defines for backward compatibility
  static const ErrorCode OK(Action::CONTINUE);
  static const ErrorCode MISSING_FEATURE    (Action::CONTINUE,   Reason::MISSING_FEATURE);
  static const ErrorCode STOP_THIS_CHAIN_   (Action::CONTINUE,   Reason::MISSING_FEATURE);

  static const ErrorCode ERROR              (Action::ABORT_CHAIN, Reason::UNKNOWN);
  static const ErrorCode INVALID_TE         (Action::ABORT_CHAIN, Reason::UNKNOWN);
  static const ErrorCode TOOL_FAILURE       (Action::ABORT_CHAIN, Reason::UNKNOWN);
  static const ErrorCode NUM_ERROR          (Action::ABORT_CHAIN, Reason::UNKNOWN);
  static const ErrorCode SG_ERROR           (Action::ABORT_CHAIN, Reason::NAV_ERROR);
  static const ErrorCode NAV_ERROR          (Action::ABORT_CHAIN, Reason::NAV_ERROR);
  static const ErrorCode GAUDI_EXCEPTION    (Action::ABORT_CHAIN, Reason::GAUDI_EXCEPTION);
  static const ErrorCode EFORMAT_EXCEPTION  (Action::ABORT_CHAIN, Reason::EFORMAT_EXCEPTION);
  static const ErrorCode STD_EXCEPTION      (Action::ABORT_CHAIN, Reason::STD_EXCEPTION);
  static const ErrorCode UNKNOWN_EXCEPTION  (Action::ABORT_CHAIN, Reason::UNKNOWN_EXCEPTION);
  static const ErrorCode TIMEOUT            (Action::ABORT_EVENT, Reason::TIMEOUT, SteeringInternalReason::TIMEOUT);
  static const ErrorCode STOP_EVENT_        (Action::ABORT_EVENT);

  static const ErrorCode NO_LVL1_ITEMS      (Action::ABORT_EVENT, SteeringInternalReason::NO_LVL1_ITEMS);
  static const ErrorCode NO_LVL2_CHAINS     (Action::ABORT_EVENT, SteeringInternalReason::NO_LVL2_CHAINS);
  static const ErrorCode NO_LVL1_RESULT     (Action::ABORT_EVENT, SteeringInternalReason::NO_LVL1_RESULT);
  static const ErrorCode WRONG_HLT_RESULT   (Action::ABORT_EVENT, SteeringInternalReason::WRONG_HLT_RESULT);
  static const ErrorCode NO_HLT_RESULT      (Action::ABORT_EVENT, SteeringInternalReason::NO_HLT_RESULT);

  static const ErrorCode STOP_JOB_          (Action::ABORT_JOB);
  static const ErrorCode BAD_JOB_SETUP      (Action::ABORT_JOB, Reason::BAD_JOB_SETUP);
  static const ErrorCode BAD_ALGO_CONFIG    (Action::ABORT_JOB, Reason::BAD_JOB_SETUP);
  static const ErrorCode FATAL              (Action::ABORT_JOB, Reason::UNKNOWN);
  static const ErrorCode LAST_ERROR         (Action::ABORT_JOB, Reason::LAST_, SteeringInternalReason::LAST_);


  // keep order so that worst is most right!
  /*  enum ErrorCode {
    OK=0, MISSING_FEATURE,
    STOP_THIS_CHAIN_,
    ERROR,
    INVALID_TE, TOOL_FAILURE,
    NUM_ERROR, SG_ERROR, NAV_ERROR,
    GAUDI_EXCEPTION, EFORMAT_EXCEPTION, STD_EXCEPTION, UNKNOWN_EXCEPTION, // bunch of exceptions we handle
    STOP_EVENT_,
    NO_LVL1_ITEMS, NO_LVL2_CHAINS,
    NO_LVL1_RESULT, WRONG_HLT_RESULT, NO_HLT_RESULT,
    TIMEOUT,
    STOP_JOB_,
    BAD_JOB_SETUP, BAD_ALGO_CONFIG, FATAL, LAST_ERROR };


  const std::string& strErrorCode( ErrorCode ec );
  */
  // search for the ErrorCode and return the position. return -1 if it can not be found.
  int getErrorCodePosFromStr(const std::string& ec);

  void reportErrorCode(ErrorCode& e1, ErrorCode e2);

  enum HLTLevel { L2 = 0, EF, HLT, UNKNOWN };

}



// definitions
inline HLT::ErrorCode::ErrorCode() { code = 0; }
inline HLT::ErrorCode::ErrorCode(HLT::Action::Code a) { code = a << 4; }
inline HLT::ErrorCode::ErrorCode(HLT::Action::Code a, HLT::Reason::Code r) { code = a << 4; code |= r; }


inline HLT::ErrorCode::ErrorCode(uint32_t c) { code = c; }
inline HLT::ErrorCode::ErrorCode(const HLT::ErrorCode c, HLT::Reason::Code r) { code = c.code; code &= 0x30; code |= r; }

inline HLT::ErrorCode::ErrorCode(HLT::Action::Code a, HLT::SteeringInternalReason::Code s) { code = a << 4; code |= s<<8; }
inline HLT::ErrorCode::ErrorCode(const HLT::ErrorCode c, HLT::SteeringInternalReason::Code s) { code = c.code; code |= s<<8; }

inline HLT::ErrorCode::ErrorCode(HLT::Action::Code a, HLT::Reason::Code r, HLT::SteeringInternalReason::Code s) { code = a << 4; code |= r; code |= s<<8; }

// comapricon operators
inline bool HLT::ErrorCode::operator==(const HLT::ErrorCode& ec) const { return uint32_t(*this) == uint32_t(ec); }
inline bool HLT::ErrorCode::operator!=(const HLT::ErrorCode& ec) const { return !(*this == ec); }

inline bool HLT::ErrorCode::operator<(const HLT::ErrorCode& ec) const { return uint32_t(*this) < uint32_t(ec); }
inline bool HLT::ErrorCode::operator>(const HLT::ErrorCode& ec) const { return !(*this<ec) && !(*this == ec); }

// conversions
//HLT::ErrorCode::operator uint32_t() const { return code; }

//    bool operator<(const ErrorCode& e1, const ErrorCode& e2 ) {return e1.code & 0x3;}// < e.code &0x3; }
inline HLT::Action::Code HLT::ErrorCode::action() const { return HLT::Action::Code((code>>4)&0x3); }
inline HLT::Reason::Code HLT::ErrorCode::reason() const { return HLT::Reason::Code(code&0xf); }
inline HLT::SteeringInternalReason::Code HLT::ErrorCode::steeringInternalReason() const { return HLT::SteeringInternalReason::Code(code>>8); }



#endif
