/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SELECTION_HELPERS__SELECTION_ACCESSOR_WRITE_INVERT_H
#define SELECTION_HELPERS__SELECTION_ACCESSOR_WRITE_INVERT_H

#include <SelectionHelpers/ISelectionWriteAccessor.h>

#include <memory>

namespace CP
{
  /// \brief the \ref SelectionAccesor for inverting a selection
  /// decoration

  class SelectionWriteAccessorInvert final : public ISelectionWriteAccessor
  {
    //
    // public interface
    //

  public:
    SelectionWriteAccessorInvert (std::unique_ptr<ISelectionWriteAccessor> val_base);

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

    /// \brief the base selection accessors I invert
  private:
    std::unique_ptr<ISelectionWriteAccessor> m_base;
  };
}

#endif
