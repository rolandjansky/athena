/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// KinkTrkZeeTagTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_KINKTRKZEETAGTOOL_H
#define DERIVATIONFRAMEWORK_KINKTRKZEETAGTOOL_H 1

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigObjectMatching/TrigMatchTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include<string>
#include<vector>

namespace DerivationFramework {

  /** @class KinkTrkZeeTagTool
      @author shimpei.yamamoto@cern.ch
     */
  class KinkTrkZeeTagTool : public AthAlgTool, public IAugmentationTool {
   
  public: 
    /** Constructor with parameters */
    KinkTrkZeeTagTool( const std::string& t, const std::string& n, const IInterface* p );
   
    /** Destructor */
    ~KinkTrkZeeTagTool();
   
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
 
    /** Check that the current event passes this filter */
    virtual StatusCode addBranches() const;

  private:
    bool passTrigger(const std::vector<std::string>& triggers) const;
    bool checkTagElectron(const xAOD::Electron *ele) const;
    bool checkCluster(const xAOD::CaloCluster *clu) const;
    bool checkEleClusPair(const xAOD::Electron *ele, const xAOD::CaloCluster *clu) const;
    bool passElectronQuality(const xAOD::Electron *ele) const;
    bool passClusterQuality(const xAOD::CaloCluster *clu) const;
    bool passElectronTrigMatch(const xAOD::Electron *ele, const std::vector<std::string>& triggers) const;

    ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
    ToolHandle<TrigMatchTool> m_trigMatchTool;
    std::vector<std::string> m_trigNames;
    float m_trigMatchDeltaR;
    bool m_doTrigMatch;

    std::string m_electronSGKey;
    std::vector<std::string> m_electronIDKeys;
    float m_electronPtCut;
    float m_electronEtaMax;

    std::string m_clusterSGKey;
    float m_clusterEtCut;
    float m_clusterEtaMax;

    float m_diEleMassLow;
    float m_diEleMassHigh;
    float m_dPhiMax;

    std::string m_sgKeyPrefix;

  }; 
 
}
#endif
