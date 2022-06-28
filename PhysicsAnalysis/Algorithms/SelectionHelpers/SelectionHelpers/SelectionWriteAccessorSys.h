/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SELECTION_HELPERS__SELECTION_WRITE_ACCESSOR_SYS_H
#define SELECTION_HELPERS__SELECTION_WRITE_ACCESSOR_SYS_H

#include <PATInterfaces/SystematicSet.h>
#include <SelectionHelpers/ISelectionWriteAccessor.h>

#include <memory>
#include <vector>

namespace CP
{
  /// \brief the \ref SelectionAccesor for reading systematically
  /// varied decorations

  class SelectionWriteAccessorSys final : public ISelectionWriteAccessor
  {
    //
    // public interface
    //

  public:
    SelectionWriteAccessorSys (const std::string& val_selectionName);


    virtual void setBits (const SG::AuxElement& element,
                          SelectionType selection,
                          const CP::SystematicSet *sys) const override;


    virtual void setBool (const SG::AuxElement& element,
                          bool selection,
                          const CP::SystematicSet *sys) const override;


    virtual std::string label () const override;


    virtual StatusCode
    fillSystematics (const ISystematicsSvc& svc,
                     const CP::SystematicSet& fullAffecting,
                     const std::vector<CP::SystematicSet>& sysList,
                     const std::string& objectName) override;



    //
    // private interface
    //

    /// \brief the map of accessor we use
  private:
    std::unordered_map<CP::SystematicSet,std::tuple<std::string,std::unique_ptr<ISelectionWriteAccessor>>> m_dataCache;

    /// \brief the selectionName for this accessor
  private:
    std::string m_selectionName;
  };
}

#endif
