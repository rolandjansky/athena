/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CaloClusterThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_CALO_CALOCLUSTERPARTICLETHINNING_H
#define DERIVATIONFRAMEWORK_CALO_CALOCLUSTERPARTICLETHINNING_H

#include <string>
#include <atomic>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "StoreGate/ThinningHandleKey.h"

#include "ExpressionEvaluation/ExpressionParserUser.h"

namespace DerivationFramework {

  class CaloClusterThinning : public extends<ExpressionParserUser<AthAlgTool>,IThinningTool> {
    public:
      CaloClusterThinning(const std::string& t, const std::string& n, const IInterface* p);
      virtual ~CaloClusterThinning();
      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;
      virtual StatusCode doThinning() const override;

    private:
      mutable std::atomic<unsigned int> m_ntot, m_ntotTopo, m_npass, m_npassTopo; //, m_ntotFrwd, m_npassFrwd;
      bool m_run_calo, m_run_topo;

      StringProperty m_streamName
       { this, "StreamName", "", "Name of the stream being thinned" };

      std::string m_sgKey;
      SG::ThinningHandleKey<xAOD::CaloClusterContainer> m_CaloClSGKey
        { this, "CaloClCollectionSGKey", "", "" };
      SG::ThinningHandleKey<xAOD::CaloClusterContainer> m_TopoClSGKey
        { this, "TopoClCollectionSGKey", "", "" };
      //std::string m_FrwdClSGKey;
      std::string m_selectionString;
      float m_coneSize;

      StatusCode setClustersMask(std::vector<bool>& mask, const xAOD::IParticle* particle, const xAOD::CaloClusterContainer* cps,
                                 bool is_muons,
                                 bool is_egamma) const;
      StatusCode particleCluster(std::vector<bool>& mask, const xAOD::IParticle* particle,const xAOD::CaloClusterContainer* cps,
                                 bool is_muons,
                                 bool is_egamma,
                                 bool is_tau) const;
  };
}

#endif // DERIVATIONFRAMEWORK_EGAMMACALOCLUSTERPARTIGLETHINNING_H
