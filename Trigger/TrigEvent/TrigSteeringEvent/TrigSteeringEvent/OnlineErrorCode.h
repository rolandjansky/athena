/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTEERINGEVENT_OnlineErrorCode_H
#define TRIGSTEERINGEVENT_OnlineErrorCode_H
#include <ostream>

namespace HLT {
  /**
   * @class OnlineErrorCode
   * @brief Error codes saved to ByteStream status word in case of HLT failure online
   **/
  enum class OnlineErrorCode : uint32_t {
    UNCLASSIFIED          = 0,
    BEFORE_NEXT_EVENT     = 1,
    CANNOT_RETRIEVE_EVENT = 2,
    NO_EVENT_INFO         = 3,
    SCHEDULING_FAILURE    = 4,
    CANNOT_ACCESS_SLOT    = 5,
    PROCESSING_FAILURE    = 6,
    NO_HLT_RESULT         = 7,
    OUTPUT_BUILD_FAILURE  = 8,
    OUTPUT_SEND_FAILURE   = 9,
    AFTER_RESULT_SENT     = 10,
    COOL_UPDATE           = 11,
    TIMEOUT               = 12,
    RESULT_TRUNCATION     = 13,
    MISSING_CTP_FRAGMENT  = 14,
    BAD_CTP_FRAGMENT      = 15,
  };

  // There's no cleaner way to map enum to string, but watch out for C++ Reflection TS, it may come one day
  #define OnlineErrorCodeSwitchCase(ENUM_NAME) \
    case OnlineErrorCode::ENUM_NAME: return "OnlineErrorCode::"#ENUM_NAME; break;

  constexpr std::string_view OnlineErrorCodeToString(const OnlineErrorCode code) {
    switch (code) {
      OnlineErrorCodeSwitchCase(UNCLASSIFIED);
      OnlineErrorCodeSwitchCase(BEFORE_NEXT_EVENT);
      OnlineErrorCodeSwitchCase(CANNOT_RETRIEVE_EVENT);
      OnlineErrorCodeSwitchCase(NO_EVENT_INFO);
      OnlineErrorCodeSwitchCase(SCHEDULING_FAILURE);
      OnlineErrorCodeSwitchCase(CANNOT_ACCESS_SLOT);
      OnlineErrorCodeSwitchCase(PROCESSING_FAILURE);
      OnlineErrorCodeSwitchCase(NO_HLT_RESULT);
      OnlineErrorCodeSwitchCase(OUTPUT_BUILD_FAILURE);
      OnlineErrorCodeSwitchCase(OUTPUT_SEND_FAILURE);
      OnlineErrorCodeSwitchCase(AFTER_RESULT_SENT);
      OnlineErrorCodeSwitchCase(COOL_UPDATE);
      OnlineErrorCodeSwitchCase(TIMEOUT);
      OnlineErrorCodeSwitchCase(RESULT_TRUNCATION);
      OnlineErrorCodeSwitchCase(MISSING_CTP_FRAGMENT);
      OnlineErrorCodeSwitchCase(BAD_CTP_FRAGMENT);
      default: return "UNDEFINED_OnlineErrorCode"; break;
    }
  }

  inline std::ostream& operator<< (std::ostream& os, const HLT::OnlineErrorCode code) {
    return os << HLT::OnlineErrorCodeToString(code);
  }

  /**
   * @return true if @c code corresponds to an issue with event data or in processing algorithms,
   *         false if @c code corresponds to a failure of an online framework component
   */
  constexpr bool isEventProcessingErrorCode(const OnlineErrorCode code) {
    switch (code) {
      case OnlineErrorCode::PROCESSING_FAILURE:
      case OnlineErrorCode::TIMEOUT:
      case OnlineErrorCode::RESULT_TRUNCATION:
      case OnlineErrorCode::MISSING_CTP_FRAGMENT:
      case OnlineErrorCode::BAD_CTP_FRAGMENT:
        return true;
      default:
        return false;
    }
  }

} // namespace HLT

#endif // TRIGSTEERINGEVENT_OnlineErrorCode_H
