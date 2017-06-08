/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TruthMetaDataWriter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TRUTHMETADATAWRITER_H
#define DERIVATIONFRAMEWORK_TRUTHMETADATAWRITER_H

#include <string>
#include <map>
#include <unordered_set>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "xAODTruth/TruthMetaDataContainer.h"
#include "GaudiKernel/ToolHandle.h"

namespace DerivationFramework {

  class TruthMetaDataWriter : public AthAlgTool, public IAugmentationTool {
    public: 
      TruthMetaDataWriter(const std::string& t, const std::string& n, const IInterface* p);
      ~TruthMetaDataWriter();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      /// Connection to the metadata store
      ServiceHandle< StoreGateSvc > m_metaStore;
      ServiceHandle<StoreGateSvc> inputMetaStore;
      /// The meta data container to be written out
      xAOD::TruthMetaDataContainer* m_tmd;
      /// SG key and name for meta data
      std::string m_metaName;
      /// Set for tracking the mc channels for which we already added meta data
      mutable std::unordered_set<uint32_t> m_existingMetaDataChan; 
      /// Weight names map (should be set from Python level)
      std::map<std::string, int> m_weightNamesMap; 

  }; 
}

#endif 
