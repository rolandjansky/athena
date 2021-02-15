/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
    RESULT_TRUNCATION     = 13
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
      default: return "UNDEFINED OnlineErrorCode"; break;
    }
  }

inline std::ostream& operator<< (std::ostream& os, const HLT::OnlineErrorCode code) {
  return os << HLT::OnlineErrorCodeToString(code);
}

} // namespace HLT

#endif // TRIGSTEERINGEVENT_OnlineErrorCode_H
