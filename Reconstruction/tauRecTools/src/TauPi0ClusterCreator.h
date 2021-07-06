/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_TAUPI0CLUSTERCREATOR_H
#define TAURECTOOLS_TAUPI0CLUSTERCREATOR_H

#include "tauRecTools/TauRecToolBase.h"

#include "xAODPFlow/PFOContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "AsgTools/ToolHandle.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <string>
#include <vector>
#include <map>

/**
 * @brief Creates Pi0 clusters (Pi0 Finder).
 * 
 * @author Will Davey <will.davey@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch> 
 * @author Stephanie Yuen <stephanie.yuen@cern.ch>
 */

class TauPi0ClusterCreator : public TauRecToolBase {

public:
  
  ASG_TOOL_CLASS2(TauPi0ClusterCreator, TauRecToolBase, ITauToolBase)
  
  TauPi0ClusterCreator(const std::string& name);

  virtual ~TauPi0ClusterCreator() = default;

  virtual StatusCode executePi0ClusterCreator(xAOD::TauJet& pTau, xAOD::PFOContainer& neutralPFOContainer, 
  					xAOD::PFOContainer& hadronicClusterPFOContainer,
  					const xAOD::CaloClusterContainer& pi0CaloClusContainer) const override;
  
private:
  
  /** @brief Configure the neutral PFO*/
  StatusCode configureNeutralPFO(const xAOD::CaloCluster& cluster,
                                 const xAOD::CaloClusterContainer& pi0ClusterContainer,
                                 const xAOD::TauJet& tau,
                                 const std::vector<const xAOD::PFO*>& shotPFOs,
                                 const std::map<unsigned, const xAOD::CaloCluster*>& shotsInCluster,
                                 xAOD::PFO& neutralPFO) const;

  /** @brief Configure the haronic PFO*/
  StatusCode configureHadronicPFO(const xAOD::CaloCluster& cluster,
                                  double clusterEnergyHad,
                                  xAOD::PFO& hadronicPFO) const;

  std::map<unsigned, const xAOD::CaloCluster*> getShotToClusterMap(
      const std::vector<const xAOD::PFO*>& shotVector,
      const xAOD::CaloClusterContainer& pi0ClusterContainer,
      const xAOD::TauJet &pTau) const;

  std::vector<unsigned> getShotsMatchedToCluster(
      const std::vector<const xAOD::PFO*>& shotVector,
      const std::map<unsigned, const xAOD::CaloCluster*>& clusterToShotMap,
      const xAOD::CaloCluster& pi0Cluster) const;

  int getNPhotons( const std::vector<const xAOD::PFO*>& shotVector,
                   const std::vector<unsigned>& shotsInCluster) const;

  /** @brief fraction of cluster enegry in central EM1 cells */
  float getEM1CoreFrac(const xAOD::CaloCluster& cluster) const;
  
  /** @brief number of cells from cluster with positive energy in PS, EM1 and EM2 */
  std::vector<int> getNPosECells(const xAOD::CaloCluster& cluster) const;

  /** @brief first eta moment in PS, EM1 and EM2 w.r.t cluster eta */
  std::vector<float> get1stEtaMomWRTCluster(const xAOD::CaloCluster& cluster) const;

  /** @brief second eta moment in PS, EM1 and EM2 w.r.t cluster eta */ 
  std::vector<float> get2ndEtaMomWRTCluster(const xAOD::CaloCluster& cluster) const;

  Gaudi::Property<double> m_clusterEtCut {this, "ClusterEtCut", 0.5 * Gaudi::Units::GeV, "Et threshould for pi0 candidate clusters"};
};

#endif // TAURECTOOLS_TAUPI0CLUSTERCREATOR_H

