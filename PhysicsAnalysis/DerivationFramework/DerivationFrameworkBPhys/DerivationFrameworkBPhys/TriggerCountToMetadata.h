/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
// - w.w., 2017-01-22: Added use of BPhysMetaDataTool.
//
// Store JO metadata in the output file.
//
// It uses the BPhysMetaDataTool (default) or the IOVDbMetaDataTool to
// store job option information as metadata in a specific branch whose
// name needs to prefixed by the deriviation format name.
// Note: Metadata stored by the IOVDbMetaDataTool is not readable on
// 'RootCore' level.
//
// This is a base class.  Inherit from it to add the job options you want
// to store.  For a usage example, see
//   Bmumu_metadata.h / Bmumu_metadata.cxx
// and
//   BPHY8.py .
//
//============================================================================
//
#ifndef DERIVATIONFRAMEWORK_TriggerCountToMetadata_H
#define DERIVATIONFRAMEWORK_TriggerCountToMetadata_H

#include <string>
#include <map>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trig{
	class TrigDecisionTool;
}

namespace DerivationFramework {

  class TriggerCountToMetadata : virtual public AthAlgTool, virtual public IAugmentationTool {

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
