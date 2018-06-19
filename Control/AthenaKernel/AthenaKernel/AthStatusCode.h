/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_ATHSTATUSCODE
#define ATHENAKERNEL_ATHSTATUSCODE

#include "GaudiKernel/StatusCode.h"

namespace Athena {

/**
 * Athena specific StatusCode values
 *
 * These can be used in place of Gaudi's StatusCode::XYZ if additional
 * information should be returned. Some of these codes will result in special
 * treatment of the event (e.g. sending them to the debug stream in the HLT).
 *
 * Only generic return codes should be defined here. Algorithm-specific return
 * values should be encoded in separate enums in the respective packages.
 */
enum class Status : StatusCode::code_t {
  TIMEOUT = 10,            ///< Timeout during event processing
  MISSING_DATA = 11        ///< Missing/corrupted input data
};

} // namespace Athena

// Register the enum as StatusCode
STATUSCODE_ENUM_DECL(Athena::Status)

#endif
