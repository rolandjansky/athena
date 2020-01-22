/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PscErrorCode_H
#define PscErrorCode_H
#include <ostream>

namespace hltonl {
  /**
   * @class PSCErrorCode
   * @brief Error codes saved to ByteStream status word in case of HLT failure
   **/
  enum class PSCErrorCode : uint32_t {
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
  #define PSCErrorCodeSwitchCase(ENUM_NAME) \
    case PSCErrorCode::ENUM_NAME: return "PSCErrorCode::"#ENUM_NAME; break;

  constexpr std::string_view PSCErrorCodeToString(const PSCErrorCode code) {
    switch (code) {
      PSCErrorCodeSwitchCase(UNCLASSIFIED);
      PSCErrorCodeSwitchCase(BEFORE_NEXT_EVENT);
      PSCErrorCodeSwitchCase(CANNOT_RETRIEVE_EVENT);
      PSCErrorCodeSwitchCase(NO_EVENT_INFO);
      PSCErrorCodeSwitchCase(SCHEDULING_FAILURE);
      PSCErrorCodeSwitchCase(CANNOT_ACCESS_SLOT);
      PSCErrorCodeSwitchCase(PROCESSING_FAILURE);
      PSCErrorCodeSwitchCase(NO_HLT_RESULT);
      PSCErrorCodeSwitchCase(OUTPUT_BUILD_FAILURE);
      PSCErrorCodeSwitchCase(OUTPUT_SEND_FAILURE);
      PSCErrorCodeSwitchCase(AFTER_RESULT_SENT);
      PSCErrorCodeSwitchCase(COOL_UPDATE);
      PSCErrorCodeSwitchCase(TIMEOUT);
      PSCErrorCodeSwitchCase(RESULT_TRUNCATION);
      default: return "UNDEFINED PSCErrorCode"; break;
    }
  }
}

inline std::ostream& operator<< (std::ostream& os, const hltonl::PSCErrorCode code) {
  return os << hltonl::PSCErrorCodeToString(code);
}
#endif
