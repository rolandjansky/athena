/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SELECTION_HELPERS__SELECTION_READ_ACCESSOR_NULL_H
#define SELECTION_HELPERS__SELECTION_READ_ACCESSOR_NULL_H

#include <SelectionHelpers/ISelectionReadAccessor.h>

namespace CP
{
  /// \brief a \ref SelectionAccesor that can be used instead of a
  /// nullptr
  ///
  /// This makes it easier to set up code that just uses an \ref
  /// ISelectionReadAccessor to preselect its objects, and doesn't want a
  /// special code path for the case of no preselections.

  class SelectionReadAccessorNull final : public ISelectionReadAccessor
  {
    //
    // public interface
    //

  public:
    SelectionReadAccessorNull (bool value = true);

  public:
    virtual SelectionType
    getBits (const SG::AuxElement& element,
             const CP::SystematicSet *sys) const override;

  public:
    virtual bool
    getBool (const SG::AuxElement& element,
             const CP::SystematicSet *sys) const override;

  public:
    virtual std::string label () const override;

  public:
    virtual CP::SystematicSet
    getInputAffecting (const ISystematicsSvc& svc,
                       const std::string& objectName) const override;

  public:
    virtual StatusCode
    fillSystematics (const ISystematicsSvc& svc,
                     const std::vector<CP::SystematicSet>& sysList,
                     const std::string& objectName) override;


    //
    // private interface
    //
  private:
    bool m_value;
  };
}

#endif
