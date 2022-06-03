/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SELECTION_HELPERS__SELECTION_ACCESSOR_LIST_H
#define SELECTION_HELPERS__SELECTION_ACCESSOR_LIST_H

#include <SelectionHelpers/ISelectionReadAccessor.h>

#include <memory>
#include <vector>

namespace CP
{
  /// \brief the \ref SelectionAccesor for list of selection
  /// decorations

  class SelectionAccessorList final : public ISelectionReadAccessor
  {
    //
    // public interface
    //

  public:
    SelectionAccessorList (std::vector<std::unique_ptr<ISelectionReadAccessor> > val_list);

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

    /// \brief the list of selection accessors I rely on
  private:
    std::vector<std::unique_ptr<ISelectionReadAccessor> > m_list;

    /// \brief the label of the accessor
  private:
    std::string m_label;
  };
}

#endif
