/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SELECTION_HELPERS__SELECTION_ACCESSOR_INVERT_H
#define SELECTION_HELPERS__SELECTION_ACCESSOR_INVERT_H

#include <SelectionHelpers/ISelectionAccessor.h>

#include <memory>

namespace CP
{
  /// \brief the \ref SelectionAccesor for inverting a selection
  /// decoration

  class SelectionAccessorInvert final : public ISelectionAccessor
  {
    //
    // public interface
    //

  public:
    SelectionAccessorInvert (std::unique_ptr<ISelectionAccessor> val_base);

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

    /// \brief the base selection accessors I invert
  private:
    std::unique_ptr<ISelectionAccessor> m_base;
  };
}

#endif
