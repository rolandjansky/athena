/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_TRUTHMETADATAWRITER_H
#define DERIVATIONFRAMEWORK_TRUTHMETADATAWRITER_H

// Base classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

// Handles to services
#include "GaudiKernel/ServiceHandle.h"

// EDM classes - typedefs, so have to #include them
#include "xAODTruth/TruthMetaDataContainer.h"

// Standard library includes
#include <string>
#include <unordered_set>

// Forward declarations
class IHepMCWeightSvc;

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
      /// Service for retrieving the weight names
      ServiceHandle< IHepMCWeightSvc > m_weightSvc;
      /// The meta data container to be written out
      xAOD::TruthMetaDataContainer* m_tmd;
      /// SG key and name for meta data
      std::string m_metaName;
      /// Set for tracking the mc channels for which we already added meta data
      mutable std::unordered_set<uint32_t> m_existingMetaDataChan; 

  }; 
}

#endif 
