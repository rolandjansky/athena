/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EMCLUSTERTOOL_H
#define EGAMMATOOLS_EMCLUSTERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ToolHandle.h"
#include "egammaBaseTool.h"
#include "egammaInterfaces/IEMClusterTool.h"

#include "egammaInterfaces/IegammaMVASvc.h"
#include "egammaInterfaces/IegammaSwTool.h"

#include "StoreGate/WriteHandleKey.h"
#include "xAODCaloEvent/CaloCluster.h" // cannot use CaloClusterFwd b/c of ClusterSize
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/EgammaFwd.h"

#include "CaloUtils/CaloCellDetPos.h"
class CaloClusterCellLink;

/**
   @class EMClusterTool
   This tool makes the EM CaloCluster used by egamma objects.
   It also defines the cluster size
       - in barrel
         3*7 for electrons
         3*7 for converted photons with Rconv<800mm
         3*5 for converted photons with Rconv>800mm
         3*5 for unconverted photons
       - in end-caps
         5*5 for electrons
         5*5 for converted photons with Rconv<800mm
         5*5 for converted photons with Rconv>800mm
         5*5 for unconverted photons
    and applies the right cluster corrections depending on classification as
   electron, unconverted photon or converted photon

   @author Thomas Koffas
   @author F. Derue
   @author B. Lenzi
   @author C. Anastopoulos
*/

class EMClusterTool
  : public AthAlgTool
  , virtual public IEMClusterTool
{

public:
  /** @brief constructor */
  EMClusterTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);

  /** @brief destructor */
  virtual ~EMClusterTool() = default;

  /** @brief initialize method */
  virtual StatusCode initialize() override final;
  /** @brief execute on container */
  virtual StatusCode contExecute(
    const EventContext& ctx,
    const CaloDetDescrManager& mgr,
    xAOD::ElectronContainer* electronContainer,
    xAOD::PhotonContainer* photonContainer) const override final;

private:
  /** @brief Set new cluster to the egamma object, decorate the new cluster
   * with a link to the old one **/
  void setNewCluster(const EventContext& ctx,
                     const CaloDetDescrManager& mgr,
                     xAOD::Egamma* eg,
                     xAOD::CaloClusterContainer* outputClusterContainer,
                     xAOD::EgammaParameters::EgammaType egType) const;

  /** @brief creation of new cluster based on existing one
   * Return a new cluster using the seed eta0, phi0 from the existing one,
   * applying cluster corrections and MVA calibration (requires the egamma
   * object). The cluster size depends on the given EgammaType
   */
  xAOD::CaloCluster* makeNewCluster(const EventContext&,
                                    const xAOD::CaloCluster&,
                                    const CaloDetDescrManager& mgr,
                                    xAOD::Egamma* eg,
                                    xAOD::EgammaParameters::EgammaType) const;

  /** @brief creation of new cluster based on existing one
   * Return a new cluster with the given size using the seed eta0, phi0 from the
   * existing cluster and applying cluster corrections.
   * If doDecorate is true, copy the cal to the raw signal state
   * and set the raw one to the cal e,eta,phi from the existing cluster
   */
  xAOD::CaloCluster* makeNewCluster(
    const EventContext& ctx,
    const xAOD::CaloCluster&,
    const CaloDetDescrManager& mgr,
    const xAOD::CaloCluster::ClusterSize&) const;

  /** @brief creation of new super cluster based on existing one */
  xAOD::CaloCluster* makeNewSuperCluster(const xAOD::CaloCluster& cluster,
                                         xAOD::Egamma* eg) const;

  /** @brief build extra positions in calo frame*/
  void fillPositionsInCalo(xAOD::CaloCluster* cluster,
                           const CaloDetDescrManager& mgr) const;

  /** @brief Key of the output cluster container **/
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_outputClusterContainerKey{
    this,
    "OutputClusterContainerName",
    "",
    "Name of the output cluster container"
  };

  /** @brief Key of the output cluster container cell links:
      name taken from containter name; only dummy configurable **/
  SG::WriteHandleKey<CaloClusterCellLinkContainer>
    m_outputClusterContainerCellLinkKey{
      this,
      "DoNotSet_OutputClusterContainerLinks",
      "",
      "Key of the output cluster container cell links; Do not set! Name taken "
      "from associated container"
    };

  /** @brief Key of the output cluster container for topo-seeded clusters **/
  SG::WriteHandleKey<xAOD::CaloClusterContainer>
    m_outputTopoSeededClusterContainerKey{
      this,
      "OutputTopoSeededClusterContainerName",
      ""
      "Name of the output cluster container for topo-seeded clusters (can be "
      "the same as the other clusters)"
    };

  /** @brief Key of the output cluster container cell links for topo-seeded
   * clusters: name taken from containter name; only dummy configurable */
  SG::WriteHandleKey<CaloClusterCellLinkContainer>
    m_outputTopoSeededClusterContainerCellLinkKey{
      this,
      "DoNotSet_OutputTopoSeededClusterContainerLinks",
      "",
      "Key of the output cluster container cell links for topo-seeded "
      "clusters; Do not set! Name taken from associated container"
    };

  /** Handle to the MVA calibration service **/
  ServiceHandle<IegammaMVASvc> m_MVACalibSvc{ this,
                                              "MVACalibSvc",
                                              "egammaMVASvc",
                                              "calibration service" };

  /** @brief Tool to handle cluster corrections */
  ToolHandle<IegammaSwTool> m_clusterCorrectionTool{
    this,
    "ClusterCorrectionTool",
    "egammaSwTool/egammaswtool",
    "tool that applies cluster corrections"
  };

  /** @brief do super clusters **/
  Gaudi::Property<bool> m_doSuperClusters{ this,
                                           "doSuperCluster",
                                           true,
                                           "Do Super Cluster Reco" };

  /** @brief flag to protect against applying the MVA to super Clusters **/
  Gaudi::Property<bool> m_applySuperClusters{
    this,
    "applyMVAToSuperCluster",
    true,
    "Protection to not do anything for superClusters"
  };

  /** @brief Position in Calo frame**/
  const CaloCellDetPos m_caloCellDetPos;

  // derived variable (not set by JOs)
  bool m_doTopoSeededContainer;
};

#endif // EGAMMATOOLS_EMCLUSTERTOOL_H
