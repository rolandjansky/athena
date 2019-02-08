/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SELECTION_HELPERS__SELECTION_ACCESSOR_OR_H
#define SELECTION_HELPERS__SELECTION_ACCESSOR_OR_H

#include <SelectionHelpers/ISelectionAccessor.h>

namespace CP
{
  /// \brief the \ref SelectionAccesor for OR tool selection
  /// decorations

  class SelectionAccessorChar final : public ISelectionAccessor
  {
    //
    // public interface
    //

  public:
    SelectionAccessorChar (const std::string& name);

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

    /// \brief th underlying accessor
  private:
    SG::AuxElement::Accessor<char> m_accessor;

    /// \brief th underlying accessor
  private:
    SG::AuxElement::ConstAccessor<char> m_constAccessor;
  };
}

#endif
