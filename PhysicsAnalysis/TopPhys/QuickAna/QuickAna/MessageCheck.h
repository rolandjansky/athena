/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__MESSAGE_CHECK_H
#define QUICK_ANA__MESSAGE_CHECK_H

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <QuickAna/Global.h>

#include <AsgTools/MessageCheck.h>

namespace ana
{
  ANA_MSG_HEADER (msgToolHandle)
  ANA_MSG_HEADER (msgConfiguration)
  ANA_MSG_HEADER (msgIsolationTools)
  ANA_MSG_HEADER (msgStore)
  ANA_MSG_HEADER (msgScheduler)
  ANA_MSG_HEADER (msgSelection)
  ANA_MSG_HEADER (msgObjectDefinition)
  ANA_MSG_HEADER (msgKinSelect)
  ANA_MSG_HEADER (msgUserCode)

  void enableStatusCodeFailure ();
}


/// \brief check whether the given expression was successful, updating
/// the cut value as appropriate
///
/// This is the same as \ref ANA_CHECK, except for updating the cut
/// value and checking whether the status code is out of validity.  If
/// it is out of valitidy range, it will still end the function, but
/// report success.
#define QA_CHECK_CUT(CUT,EXP)						\
  { const auto sc__ = EXP;						\
    typedef typename std::decay<decltype(sc__)>::type scType__;		\
    if (::asg::CheckHelper<scType__>::isSuccess (sc__)) {		\
      (CUT).setPassedIf (true);						\
    } else if (::asg::CheckHelper<scType__>::isOutOfValidityRange (sc__)) { \
      (CUT).setFailedIf (true);						\
      return ::asg::CheckHelper<AsgToolsCheckResultType>::successCode(); \
    } else {								\
      ANA_MSG_ERROR ("Failed to call \"" << #EXP << "\"");		\
      return ::asg::CheckHelper<AsgToolsCheckResultType>::failureCode(); \
    } }

/// \brief check whether the given expression was successful, updating
/// the weight on success
///
/// Since the exact type of the weight argument varies across scale
/// factor tools, it is passed in as an extra argument, and the macro
/// performs a type conversion.
///
/// This is the same as \ref ANA_CHECK, except for updating the weight
/// value and checking whether the status code is out of validity.  If
/// it is out of valitidy range, it will *not* end the function nor
/// set the weight, but print out a warning message that this
/// happened.
#define QA_CHECK_WEIGHT(TYPE,WEIGHT,EXP)				\
  { auto& weight__ = WEIGHT;						\
    TYPE WEIGHT = 1;							\
    const auto sc__ = EXP;						\
    typedef typename std::decay<decltype(sc__)>::type scType__;		\
    if (::asg::CheckHelper<scType__>::isSuccess (sc__)) {		\
      weight__ = WEIGHT;						\
    } else if (::asg::CheckHelper<scType__>::isOutOfValidityRange (sc__)) { \
      ANA_MSG_WARNING("OutOfValidityRange for: " << #EXP);		\
    } else {								\
      ANA_MSG_ERROR ("Failed to call \"" << #EXP << "\"");		\
      return ::asg::CheckHelper<AsgToolsCheckResultType>::failureCode(); \
    } }

#endif
