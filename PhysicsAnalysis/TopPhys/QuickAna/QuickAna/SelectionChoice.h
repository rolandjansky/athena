/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__SELECTION_CHOICE_H
#define QUICK_ANA__SELECTION_CHOICE_H

#include <QuickAna/Global.h>

namespace ana
{
  /// \brief the different values for a cut selection
  ///
  /// the main purpose of this is to know whether a selection flag has
  /// been set or not.  but it is also nice to give it names instead
  /// of true and false

  enum class SelectionChoice
  {
    /// \brief the cut has failed
    FAIL,

    /// \brief the cut has passed
    PASS,

    /// \brief the cut has not been assigned
    UNASSIGNED
  };

  template<> struct EnumSize<SelectionChoice>
  {
    static const unsigned size = unsigned (SelectionChoice::UNASSIGNED) + 1;
  };
}

#endif
