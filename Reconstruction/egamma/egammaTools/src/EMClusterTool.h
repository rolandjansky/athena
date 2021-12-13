/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EMCLUSTERTOOL_H
#define EGAMMATOOLS_EMCLUSTERTOOL_H

#include "egammaInterfaces/IEMClusterTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/WriteHandleKey.h"

#include "EgammaAnalysisInterfaces/IegammaMVASvc.h"
#include "egammaInterfaces/IegammaSwTool.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/EgammaFwd.h"

class CaloClusterCellLink;

/**
   @class EMClusterTool
   This tool makes the EM CaloCluster used by egamma objects.
    and applies the right cluster corrections depending on classification as
   electron, unconverted photon or converted photon

   @author Thomas Koffas
   @author F. Derue
   @author B. Lenzi
   @author C. Anastopoulos
*/

class EMClusterTool final
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
    xAOD::ElectronContainer* electronContainer,
    xAOD::PhotonContainer* photonContainer) const override final;

private:
  /** @brief Set new cluster to the egamma object, decorate the new cluster
   * with a link to the old one **/
  void setNewCluster(const EventContext& ctx,
                     xAOD::Egamma* eg,
                     xAOD::CaloClusterContainer* outputClusterContainer) const;
  /** @brief creation of new super cluster based on existing one */
  xAOD::CaloCluster* makeNewSuperCluster(const xAOD::CaloCluster& cluster,
                                         xAOD::Egamma* eg) const;

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

  /** Handle to the MVA calibration service **/
  ServiceHandle<IegammaMVASvc> m_MVACalibSvc{ this,
                                              "MVACalibSvc",
                                              "egammaMVASvc",
                                              "calibration service" };
};

#endif // EGAMMATOOLS_EMCLUSTERTOOL_H
