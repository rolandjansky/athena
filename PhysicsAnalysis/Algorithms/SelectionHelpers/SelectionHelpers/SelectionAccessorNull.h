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
    SelectionAccessorNull (bool value = true);

  public:
    virtual SelectionType
    getBits (const SG::AuxElement& element,
             const CP::SystematicSet *sys) const override;

  public:
    virtual void setBits (const SG::AuxElement& element,
                          SelectionType selection,
                          const CP::SystematicSet *sys) const override;

  public:
    virtual bool
    getBool (const SG::AuxElement& element,
             const CP::SystematicSet *sys) const override;

  public:
    virtual void setBool (const SG::AuxElement& element,
                          bool selection,
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
