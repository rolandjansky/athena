/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SELECTION_HELPERS__SELECTION_ACCESSOR_LIST_H
#define SELECTION_HELPERS__SELECTION_ACCESSOR_LIST_H

#include <SelectionHelpers/ISelectionAccessor.h>

#include <memory>
#include <vector>

namespace CP
{
  /// \brief the \ref SelectionAccesor for list of selection
  /// decorations

  class SelectionAccessorList final : public ISelectionAccessor
  {
    //
    // public interface
    //

  public:
    SelectionAccessorList (std::vector<std::unique_ptr<ISelectionAccessor> > val_list);

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

    /// \brief the list of selection accessors I rely on
  private:
    std::vector<std::unique_ptr<ISelectionAccessor> > m_list;
  };
}

#endif
