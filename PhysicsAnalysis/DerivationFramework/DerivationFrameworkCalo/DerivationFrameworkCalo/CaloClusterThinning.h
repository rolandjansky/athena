/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CaloClusterThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_CALO_CALOCLUSTERPARTICLETHINNING_H
#define DERIVATIONFRAMEWORK_CALO_CALOCLUSTERPARTICLETHINNING_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODCaloEvent/CaloCluster.h"

namespace ExpressionParsing {
  class ExpressionParser;
}

class IThinningSvc;

namespace DerivationFramework {

  class CaloClusterThinning : public AthAlgTool, public IThinningTool {
    public:
      CaloClusterThinning(const std::string& t, const std::string& n, const IInterface* p);
      ~CaloClusterThinning();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode doThinning() const;

    private:
      ServiceHandle<IThinningSvc> m_thinningSvc;
      mutable unsigned int m_ntot, m_ntotTopo, m_npass, m_npassTopo; //, m_ntotFrwd, m_npassFrwd;
      mutable bool m_is_muons, m_is_egamma, m_is_tau, m_run_calo, m_run_topo;
      std::string m_sgKey;
      std::string m_CaloClSGKey;
      std::string m_TopoClSGKey;
      //std::string m_FrwdClSGKey;
      std::string m_selectionString;
      float m_coneSize;
      bool m_and;
      ExpressionParsing::ExpressionParser *m_parser;

      StatusCode setClustersMask(std::vector<bool>& mask, const xAOD::IParticle* particle, const xAOD::CaloClusterContainer* cps) const;
      StatusCode particleCluster(std::vector<bool>& mask, const xAOD::IParticle* particle,const xAOD::CaloClusterContainer* cps) const;
  };
}

#endif // DERIVATIONFRAMEWORK_EGAMMACALOCLUSTERPARTIGLETHINNING_H
