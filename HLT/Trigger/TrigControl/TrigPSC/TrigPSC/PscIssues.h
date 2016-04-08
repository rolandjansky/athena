// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file PscIssues.h
 * @author Frank Winklmeier, Ricardo Abreu
 *
 * @brief ERS Issues for PSC
 */

#ifndef PSC_ISSUES_H
#define PSC_ISSUES_H

#include "ers/ers.h"
#include <string>

// The base PSC Issue
ERS_DECLARE_ISSUE(
  psc,
  Issue,
  ERS_EMPTY,
  ERS_EMPTY
)

// A configuration issue in the PSC
ERS_DECLARE_ISSUE_BASE(
  psc,
  ConfigurationIssue,
  psc::Issue,
  "Configuration Issue: " << reason,
  ERS_EMPTY,
  ((const char *) reason)
)

// TODO - check whether we need to keep the stuff below
// Declare a PSC Message Issue
ERS_DECLARE_ISSUE( ers, PSCMessage, ERS_EMPTY, ERS_EMPTY)

#define ERS_PSC_WARNING( message ) \
{ \
    ERS_REPORT_IMPL( ers::warning, ers::PSCMessage, message, ); \
}

#define ERS_PSC_ERROR( message ) \
{ \
    ERS_REPORT_IMPL( ers::error, ers::PSCMessage, message, ); \
}

#define ERS_PSC_FATAL( message ) \
{ \
    ERS_REPORT_IMPL( ers::fatal, ers::PSCMessage, message, ); \
}
  
#endif /* PSC_ISSUES_H */
