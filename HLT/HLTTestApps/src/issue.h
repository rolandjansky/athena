/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file issue.h
 * @author <a href="mailto:andre.dos.anjos@cern.ch">Andre Anjos</a> 
 *
 * @brief ERS issues that can be raised by our Boost.Python framework
 */

#ifndef HLTTESTAPPS_ISSUE_H 
#define HLTTESTAPPS_ISSUE_H

#include <ers/ers.h>
#include <boost/format.hpp>

ERS_DECLARE_ISSUE(HLTTestApps, Issue, " - HLTTestApps base issue", )
#define HLTTESTAPPS_ISSUE HLTTestApps::Issue(ERS_HERE)

ERS_DECLARE_ISSUE_BASE(HLTTestApps, Unclassified, HLTTestApps::Issue, 
    ": " << reason, , ((std::string)reason))
#define HLTTESTAPPS_UNCLASSIFIED(x) HLTTestApps::Unclassified(ERS_HERE, x)

#endif /* HLTTESTAPPS_ISSUE_H */

