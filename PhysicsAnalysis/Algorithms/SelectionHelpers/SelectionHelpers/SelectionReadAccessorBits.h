/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SELECTION_HELPERS__SELECTION_ACCESSOR_READ_BITS_H
#define SELECTION_HELPERS__SELECTION_ACCESSOR_READ_BITS_H

#include <SelectionHelpers/ISelectionReadAccessor.h>

namespace CP
{
  /// \brief the \ref SelectionAccesor for standard CP algorithm
  /// selection decorations encoded as bits

  class SelectionReadAccessorBits final : public ISelectionReadAccessor
  {
    //
    // public interface
    //

  public:
    SelectionReadAccessorBits (const std::string& name);

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

    /// \brief the underlying accessor
  private:
    SG::AuxElement::ConstAccessor<SelectionType> m_constAccessor;

    /// \brief the label of the accessor
  private:
    std::string m_label;
  };
}

#endif
