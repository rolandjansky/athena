/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SELECTION_HELPERS__SELECTION_WRITE_ACCESSOR_OR_H
#define SELECTION_HELPERS__SELECTION_WRITE_ACCESSOR_OR_H

#include <SelectionHelpers/ISelectionWriteAccessor.h>

namespace CP
{
  /// \brief the \ref SelectionAccesor for OR tool selection
  /// decorations

  class SelectionWriteAccessorChar final : public ISelectionWriteAccessor
  {
    //
    // public interface
    //

  public:
    SelectionWriteAccessorChar (const std::string& name);

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

  public:
    virtual StatusCode
    fillSystematics (const ISystematicsSvc& svc,
                     const CP::SystematicSet& fullAffecting,
                     const std::vector<CP::SystematicSet>& sysList,
                     const std::string& objectName) override;


    //
    // private interface
    //

    /// \brief the underlying accessor
  private:
    SG::AuxElement::Decorator<char> m_accessor;

    /// \brief the label of the accessor
  private:
    std::string m_label;
  };
}

#endif
