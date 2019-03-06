/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SELECTION_HELPERS__SELECTION_ACCESSOR_NULL_H
#define SELECTION_HELPERS__SELECTION_ACCESSOR_NULL_H

#include <SelectionHelpers/ISelectionAccessor.h>

namespace CP
{
  /// \brief a \ref SelectionAccesor that can be used instead of a
  /// nullptr
  ///
  /// This makes it easier to set up code that just uses an \ref
  /// ISelectionAccessor to preselect its objects, and doesn't want a
  /// special code path for the case of no preselections.

  class SelectionAccessorNull final : public ISelectionAccessor
  {
    //
    // public interface
    //

  public:
    SelectionAccessorNull ();

  public:
    virtual SelectionType
    getBits (const SG::AuxElement& element) const override;

  public:
    virtual void setBits (SG::AuxElement& element,
                          SelectionType selection) const override;

  public:
    virtual bool
    getBool (const SG::AuxElement& element) const override;

  public:
    virtual void setBool (SG::AuxElement& element,
                          bool selection) const override;



    //
    // private interface
    //
  };
}

#endif
