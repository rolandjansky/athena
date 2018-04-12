/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JetCaloClusterThinning.h, (c) ATLAS Detector software
// @author Danilo E. Ferreira de Lima <dferreir@cern.ch>
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORKCALO_JETCALOCLUSTERTHINNING_H
#define DERIVATIONFRAMEWORKCALO_JETCALOCLUSTERTHINNING_H

#include <string>
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

namespace ExpressionParsing {
  class ExpressionParser;
}

class IThinningSvc;

namespace DerivationFramework {

  class JetCaloClusterThinning : public AthAlgTool, public IThinningTool {
    public: 
      JetCaloClusterThinning(const std::string& t, const std::string& n, const IInterface* p);
      ~JetCaloClusterThinning();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode doThinning() const;

    private:
      ServiceHandle<IThinningSvc> m_thinningSvc;
      mutable unsigned int m_ntot, m_ntotTopo, m_npass, m_npassTopo; //, m_ntotFrwd, m_npassFrwd;
      std::string m_sgKey;
      std::string m_TopoClSGKey;
      std::string m_selectionString;
      std::vector<std::string> m_addClustersSGKey;
      bool m_and;
      ExpressionParsing::ExpressionParser *m_parser;

      void setJetClustersMask(std::vector<bool>&, const xAOD::JetContainer*&) const;
      void setJetClustersMask(std::vector<bool>&, std::vector<const xAOD::Jet*>&) const;
  };

}

#endif // DERIVATIONFRAMEWORK_JETCALOCLUSTERTHINNING_H
