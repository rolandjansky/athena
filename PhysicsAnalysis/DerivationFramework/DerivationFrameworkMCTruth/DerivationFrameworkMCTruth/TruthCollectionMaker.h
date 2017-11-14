/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_TRUTHCOLLECTIONMAKER_H
#define DERIVATIONFRAMEWORK_TRUTHCOLLECTIONMAKER_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Forward declarations
class StoreGateSvc;
namespace ExpressionParsing {
  class ExpressionParser;
}

namespace DerivationFramework {

  class TruthCollectionMaker : public AthAlgTool, public IAugmentationTool {
    public: 
      TruthCollectionMaker(const std::string& t, const std::string& n, const IInterface* p);
      ~TruthCollectionMaker();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      ExpressionParsing::ExpressionParser *m_vertParser;
      ExpressionParsing::ExpressionParser *m_partParser;
      mutable unsigned int m_ntotpart, m_npasspart;
      std::string m_particlesKey;
      //std::string m_verticesKey;
      std::string m_collectionName; 
      std::string m_partString;
      bool m_do_compress, m_do_sherpa;
      bool m_keep_navigation_info;
      ServiceHandle<StoreGateSvc> m_metaStore; //!< Handle on the metadata store for init
  }; 
}

#endif // DERIVATIONFRAMEWORK_TRUTHCOLLECTIONMAKER_H
