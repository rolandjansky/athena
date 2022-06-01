/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SELECTION_HELPERS__SELECTION_READ_ACCESSOR_INVERT_H
#define SELECTION_HELPERS__SELECTION_READ_ACCESSOR_INVERT_H

#include <SelectionHelpers/ISelectionReadAccessor.h>

#include <memory>

namespace CP
{
  /// \brief the \ref SelectionAccesor for inverting a selection
  /// decoration

  class SelectionReadAccessorInvert final : public ISelectionReadAccessor
  {
    //
    // public interface
    //

  public:
    SelectionReadAccessorInvert (std::unique_ptr<ISelectionReadAccessor> val_base);

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

    /// \brief the base selection accessors I invert
  private:
    std::unique_ptr<ISelectionReadAccessor> m_base;
  };
}

#endif
