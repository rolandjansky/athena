// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigHLTIssues.h
 * @author <a href="mailto:Werner.Wiedenmann@cern.ch">Werner Wiedenmann</a>
 *
 * @brief ERS issue classes for HLT
 */

#ifndef TRIG_HLT_ISSUES_H
#define TRIG_HLT_ISSUES_H

#include "ers/ers.h"

ERS_DECLARE_ISSUE( ers, HLTMessage, , )
ERS_DECLARE_ISSUE( ers, HLTAbort, "Fatal error during event processing", )
  
#define ERS_HLT_WARNING( message ) \
{ \
    ERS_REPORT_IMPL( ers::warning, ers::HLTMessage, message, ); \
}

#define ERS_HLT_ERROR( message ) \
{ \
    ERS_REPORT_IMPL( ers::error, ers::HLTMessage, message, ); \
}

#define ERS_HLT_FATAL( message ) \
{ \
    ERS_REPORT_IMPL( ers::fatal, ers::HLTMessage, message, ); \
}
  
#endif /* TRIG_HLT_ISSUES_H */
