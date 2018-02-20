/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
//============================================================================
// 
// Author : Matteo Bedognetti <matteo.bedognetti@cern.ch.>
// Changes:
//
// Store trigger counts for specific chains in the DAOD's MetaData.
// This allows it to store information about triggers upon which events are NOT selected during the derivation
//
// Job options:
// - TriggerList   -- a vector containing all triggers to store as strings
// - TrigDecisionTool -- if one wants to pass this a specific TrigDecisionTool
//
//============================================================================
//
#ifndef DERIVATIONFRAMEWORK_TriggerCountToMetadata_H
#define DERIVATIONFRAMEWORK_TriggerCountToMetadata_H

#include <string>
#include <map>
#include <vector>

#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkBPhys/CfAthAlgTool.h"

namespace Trig{
	class TrigDecisionTool;
}

namespace DerivationFramework {

  class TriggerCountToMetadata : virtual public CfAthAlgTool, virtual public IAugmentationTool {

    public: 
      TriggerCountToMetadata(const std::string& t, const std::string& n, const IInterface* p);
      virtual StatusCode initialize();
      virtual StatusCode finalize();
      virtual StatusCode addBranches() const;

    private: //Don't use protected for this one!

      std::vector<std::string> m_triggerList;
      ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
      mutable ServiceHandle< StoreGateSvc > m_outputMetaStore;

      std::string m_folderName;

  }; // class
} // namespace

#endif // DERIVATIONFRAMEWORK_TriggerCountToMetadata_H
