/*

Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Author: Sami Kama 2017
*/

#ifndef ALP_ISSUES_H
#define ALP_ISSUES_H

ERS_DECLARE_ISSUE(ALPIssues,                // namespace name
		  CommandLineIssue,   // issue name
		  "Command-line parameter issue: " << type << ".",        // message
		  ((const char *)type )             // first attribute
		  )

ERS_DECLARE_ISSUE(ALPIssues,                // namespace name
		  DLLIssue,   // issue name
		  "Can't load dll: \"" << type << "\".",        // message
		  ((const char *)type )             // first attribute
		  )

ERS_DECLARE_ISSUE(ALPIssues,                // namespace name
		  UnexpectedIssue,   // issue name
		  "Unexpected Issue: \"" << type << "\".",        // message
		  ((const char *)type )             // first attribute
		  )

ERS_DECLARE_ISSUE(ALPIssues,                // namespace name
		  ConfigurationIssue,   // issue name
		  "Configuration Issue: \"" << type << "\".",        // message
		  ((const char *)type )             // first attribute
		  )

ERS_DECLARE_ISSUE(ALPIssues,                // namespace name
		  TransitionIssue,   // issue name
		  "Transition Issue: \"" << type << "\".",        // message
		  ((const char *)type )             // first attribute
		  )

ERS_DECLARE_ISSUE(ALPIssues,                // namespace name
		  ChildIssue,   // issue name
		  "Child Issue: \"" << type << "\".",        // message
		  ((const char *)type )             // first attribute
		  )

#endif
