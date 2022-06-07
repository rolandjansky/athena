/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionWriteAccessorSys.h>

#include <AsgMessaging/MessageCheck.h>
#include <SystematicsHandles/ISystematicsSvc.h>

//
// method implementations
//

namespace CP
{
  SelectionWriteAccessorSys ::
  SelectionWriteAccessorSys (const std::string& val_selectionName)
    : m_selectionName (val_selectionName)
  {}



  void SelectionWriteAccessorSys ::
  setBits (const SG::AuxElement& element,
           SelectionType selection,
           const CP::SystematicSet *sys) const
  {
    if (sys == nullptr)
      throw std::logic_error ("trying to write systematics accessor without systematic");
    auto iter = m_dataCache.find (*sys);
    if (iter == m_dataCache.end())
      throw std::logic_error ("unknown systematic: " + sys->name());
    return std::get<1>(iter->second)->setBits (element, selection, sys);
  }



  void SelectionWriteAccessorSys ::
  setBool (const SG::AuxElement& element,
           bool selection,
           const CP::SystematicSet *sys) const
  {
    if (sys == nullptr)
      throw std::logic_error ("trying to write systematics accessor without systematic");
    auto iter = m_dataCache.find (*sys);
    if (iter == m_dataCache.end())
      throw std::logic_error ("unknown systematic: " + sys->name());
    return std::get<1>(iter->second)->setBool (element, selection, sys);
  }



  std::string SelectionWriteAccessorSys ::
  label () const
  {
    return m_selectionName;
  }



  StatusCode SelectionWriteAccessorSys ::
  fillSystematics (const ISystematicsSvc& svc,
                   const CP::SystematicSet& fullAffecting,
                   const std::vector<CP::SystematicSet>& sysList,
                   const std::string& objectName)
  {
    using namespace msgSelectionHelpers;

    std::string baseName = m_selectionName;
    std::string suffix;
    if (auto split = baseName.find (',');
        split != std::string::npos)
    {
      suffix = baseName.substr (split);
      baseName = baseName.substr (0, split);
    }

    for (auto& sys : sysList)
    {
      std::string decorName;
      ANA_CHECK (svc.makeSystematicsName (decorName, baseName, sys));
      ANA_MSG_DEBUG ("SelectionWriteAccessorSys: " << decorName << " (" << sys.name() << ")");
      std::unique_ptr<ISelectionWriteAccessor> accessor;
      ANA_CHECK (makeSelectionWriteAccessor (decorName + suffix, accessor));
      m_dataCache.emplace (sys, std::make_tuple (decorName, std::move (accessor)));
    }
    ANA_CHECK (svc.setDecorSystematics (objectName, baseName, fullAffecting));
    return StatusCode::SUCCESS;
  }
}
