/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SELECTION_HELPERS__SELECTION_ACCESSOR_OR_H
#define SELECTION_HELPERS__SELECTION_ACCESSOR_OR_H

#include <SelectionHelpers/ISelectionAccessor.h>

namespace CP
{
  /// \brief the \ref SelectionAccesor for OR tool selection
  /// decorations

  class SelectionAccessorOR final : public ISelectionAccessor
  {
    //
    // public interface
    //

  public:
    SelectionAccessorOR (const std::string& name);

  public:
    virtual SelectionType
    getBits (const xAOD::IParticle& particle) const override;

  public:
    virtual void setBits (xAOD::IParticle& particle,
                          SelectionType selection) const override;

  public:
    virtual bool
    getBool (const xAOD::IParticle& particle) const override;

  public:
    virtual void setBool (xAOD::IParticle& particle,
                          bool selection) const override;



    //
    // private interface
    //

    /// \brief th underlying accessor
  private:
    SG::AuxElement::Accessor<char> m_accessor;
  };
}

#endif
