/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SELECTION_HELPERS__SELECTION_HELPERS_H
#define SELECTION_HELPERS__SELECTION_HELPERS_H

#include <cstdint>

namespace Root
{
  class TAccept;
}

namespace CP
{
  /// \brief the type for selection decorations that are meant to hold
  /// a \ref Root::TAccept
  ///
  /// This is matched to the number of bits in a TAccept, since that
  /// holds 32 bits, so do we.
  typedef uint32_t SelectionType;


  /// \brief the selection decoration to apply for objects that are
  /// selected
  ///
  /// This is to be used when making a selection decoration for a
  /// single cut and no TAccept object is at hand, as well as to check
  /// whether a particular selection passed.
  inline constexpr SelectionType selectionAccept () {
    return ~SelectionType (0);}


  /// \brief the selection decoration to apply for objects that are
  /// rejected
  ///
  /// This is to be used when making a selection decoration for a
  /// single cut and no TAccept object is at hand.
  inline constexpr SelectionType selectionReject () {
    return ~SelectionType (1);}


  /// \brief the selection decoration made from the given boolean
  /// (true = accept)
  SelectionType selectionFromBool (bool accept);


  /// \brief the selection decoration made from the given TAccept
  /// object
  SelectionType selectionFromAccept (const Root::TAccept& accept);
}

#endif
