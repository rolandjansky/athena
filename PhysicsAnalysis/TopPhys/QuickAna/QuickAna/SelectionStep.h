/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__SELECTION_STEP_H
#define QUICK_ANA__SELECTION_STEP_H

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
