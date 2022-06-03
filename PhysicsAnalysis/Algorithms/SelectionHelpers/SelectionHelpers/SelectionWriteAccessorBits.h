/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SELECTION_HELPERS__SELECTION_ACCESSOR_WRITE_BITS_H
#define SELECTION_HELPERS__SELECTION_ACCESSOR_WRITE_BITS_H

#include <SelectionHelpers/ISelectionWriteAccessor.h>

namespace CP
{
  /// \brief the \ref SelectionAccesor for standard CP algorithm
  /// selection decorations encoded as bits

  class SelectionWriteAccessorBits final : public ISelectionWriteAccessor
  {
    //
    // public interface
    //

  public:
    SelectionWriteAccessorBits (const std::string& name);

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


    //
    // private interface
    //

    /// \brief the underlying accessor
  private:
    SG::AuxElement::Decorator<SelectionType> m_accessor;

    /// \brief the label of the accessor
  private:
    std::string m_label;
  };
}

#endif
