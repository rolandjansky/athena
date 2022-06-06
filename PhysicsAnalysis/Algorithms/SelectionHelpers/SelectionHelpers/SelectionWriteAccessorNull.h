/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SELECTION_HELPERS__SELECTION_WRITE_ACCESSOR_NULL_H
#define SELECTION_HELPERS__SELECTION_WRITE_ACCESSOR_NULL_H

#include <SelectionHelpers/ISelectionWriteAccessor.h>

namespace CP
{
  /// \brief a \ref SelectionAccesor that can be used instead of a
  /// nullptr
  ///
  /// This makes it easier to set up code that just uses an \ref
  /// ISelectionWriteAccessor to preselect its objects, and doesn't want a
  /// special code path for the case of no preselections.

  class SelectionWriteAccessorNull final : public ISelectionWriteAccessor
  {
    //
    // public interface
    //

  public:
    SelectionWriteAccessorNull ();

  public:
    virtual void setBits (const SG::AuxElement& element,
                          SelectionType selection,
                          const CP::SystematicSet *sys) const override;

  public:
    virtual void setBool (const SG::AuxElement& element,
                          bool selection,
                          const CP::SystematicSet *sys) const override;

  public:
    virtual std::string label () const override;

  public:
    virtual StatusCode
    fillSystematics (const ISystematicsSvc& svc,
                     const CP::SystematicSet& fullAffecting,
                     const std::vector<CP::SystematicSet>& sysList,
                     const std::string& objectName) override;
  };
}

#endif
