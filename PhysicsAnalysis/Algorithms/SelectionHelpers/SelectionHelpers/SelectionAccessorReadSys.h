/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SELECTION_HELPERS__SELECTION_ACCESSOR_READ_SYS_H
#define SELECTION_HELPERS__SELECTION_ACCESSOR_READ_SYS_H

#include <PATInterfaces/SystematicSet.h>
#include <SelectionHelpers/ISelectionReadAccessor.h>

#include <memory>
#include <vector>

namespace CP
{
  /// \brief the \ref SelectionAccesor for reading systematically
  /// varied decorations

  class SelectionAccessorReadSys final : public ISelectionReadAccessor
  {
    //
    // public interface
    //

  public:
    SelectionAccessorReadSys (const std::string& val_selectionName);

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

    /// \brief the map of accessor we use
  private:
    std::unordered_map<CP::SystematicSet,std::tuple<std::string,std::unique_ptr<ISelectionReadAccessor>>> m_dataCache;

    /// \brief the selectionName for this accessor
  private:
    std::string m_selectionName;
  };
}

#endif
