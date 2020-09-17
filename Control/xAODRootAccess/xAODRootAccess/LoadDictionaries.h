// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODROOTACCESS_LOADDICTIONARIES_H
#define XAODROOTACCESS_LOADDICTIONARIES_H

// Local include(s):
#include "xAODRootAccess/tools/TReturnCode.h"

namespace xAOD {

   /// Function force-loading all the "DataVector holding" xAOD dictionaries
   ///
   /// This function is here to help with working around the issues described in
   /// https://sft.its.cern.ch/jira/browse/ROOT-10663 and
   /// https://sft.its.cern.ch/jira/browse/ROOT-10940.
   ///
   /// This is something that should not be moved to the master branch, there
   /// we need to find a cleaner solution. However for 21.2 we should just go
   /// with a simple/pragmatic solution like this.
   ///
   TReturnCode LoadDictionaries();

} // namespace xAOD

#endif // XAODROOTACCESS_LOADDICTIONARIES_H
