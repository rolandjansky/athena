///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthCheckMacros.h 
// Header file for useful macros when comes to using StatusCodes
// Author: S.Binet<binet@cern.ch>
// $Revision: 446910 $
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENABASECOMPS_ATHCHECKMACROS_H 
#define ATHENABASECOMPS_ATHCHECKMACROS_H 1

// framework includes
#include "AthenaKernel/errorcheck.h"

/*! @copydoc REPORT_ERROR_WITH_CONTEXT
 */
#define ATH_REPORT_ERROR_WITH_CONTEXT REPORT_ERROR_WITH_CONTEXT

/*! @copydoc REPORT_MESSAGE_WITH_CONTEXT
 */
#define ATH_REPORT_MESSAGE_WITH_CONTEXT REPORT_MESSAGE_WITH_CONTEXT

/*! @copydoc REPORT_ERROR
 */
#define ATH_REPORT_ERROR REPORT_ERROR

/*! @copydoc REPORT_MESSAGE
 */
#define ATH_REPORT_MESSAGE REPORT_MESSAGE

/*! @copydoc CHECK_WITH_CONTEXT
 */
#define ATH_CHECK_WITH_CONTEXT CHECK_WITH_CONTEXT

/*! @copydoc CHECK
 */
#define ATH_CHECK CHECK

/*! @copydoc CHECK_RECOVERABLE_WITH_CONTEXT
 */
#define ATH_CHECK_RECOVERABLE_WITH_CONTEXT CHECK_RECOVERABLE_WITH_CONTEXT

/*! @copydoc CHECK_RECOVERABLE
 */
#define ATH_CHECK_RECOVERABLE CHECK_RECOVERABLE

/*! @copydoc CHECK_FATAL_WITH_CONTEXT
 */
#define ATH_CHECK_FATAL_WITH_CONTEXT CHECK_FATAL_WITH_CONTEXT

/*! @copydoc CHECK_FATAL
 */
#define ATH_CHECK_FATAL CHECK_FATAL

#endif //> !ATHENABASECOMPS_ATHCHECKMACROS_H

