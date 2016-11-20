/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__SELECTION_STEP_H
#define QUICK_ANA__SELECTION_STEP_H

//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <QuickAna/Global.h>

namespace ana
{
  /// \brief the different levels of selection we have
  ///
  /// if one selection step fails, it implies that the subsequent
  /// steps also fail.

  enum class SelectionStep
  {
    /// \brief selections that go into the MET definition
    MET,

    /// \brief selections that go into overlap removal
    OR,

    /// \brief selections that go into the final analysis objects
    ANALYSIS,

    /// \brief selections that the user can manually include
    ///
    /// there is no actual decoration for this step (unlike the other
    /// ones), but the user can still manually read the individual cut
    /// decorations from the object.
    ///
    /// the purpose of this value is to exclude a cut from the cut
    /// flow without removing the cut decoration from the object.
    MANUAL
  };

  template<> struct EnumSize<SelectionStep>
  {
    static const unsigned size = unsigned (SelectionStep::MANUAL) + 1;

    /// \brief the number of selection steps that actually have
    /// decorations on the object.
    static const unsigned dataSize = unsigned (SelectionStep::MANUAL);
  };
}

#endif
