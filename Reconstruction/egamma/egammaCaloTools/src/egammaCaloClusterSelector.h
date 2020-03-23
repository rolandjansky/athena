/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef EGAMMACALOTOOLS_EGAMMACALOCLUSTERSELECTOR_H
#define EGAMMACALOTOOLS_EGAMMACALOCLUSTERSELECTOR_H

// Package includes
#include "egammaInterfaces/IegammaCaloClusterSelector.h"
#include "CaloEvent/CaloCellContainer.h"
#include "egammaInterfaces/IegammaCheckEnergyDepositTool.h"
#include "egammaInterfaces/IegammaMiddleShape.h"
#include "egammaInterfaces/IegammaIso.h"
// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "CLHEP/Units/SystemOfUnits.h"

// STL includes
#include <string>

/**
 * @class egammaCaloClusterSelector
 * @brief A tool which specifies whether a cluster passes a selection
 **/
class egammaCaloClusterSelector : public extends<AthAlgTool, IegammaCaloClusterSelector> {
public:
  egammaCaloClusterSelector(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~egammaCaloClusterSelector() override;

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  bool passSelection(const xAOD::CaloCluster* cluster,
                     const CaloDetDescrManager& cmgr) const override final;

private:
  /** @brief Name of the cluster intput collection*/
  SG::ReadHandleKey<CaloCellContainer>   m_cellsKey {this,
      "CellContainerName", "AllCalo", 
      "Names of containers which contain cells"};

  /** @brief Pointer to the egammaCheckEnergyDepositTool*/
  ToolHandle<IegammaCheckEnergyDepositTool> m_egammaCheckEnergyDepositTool {this, 
      "egammaCheckEnergyDepositTool", "",
      "Optional tool that performs basic checks of viability of cluster"};

  /** @brief Middle shape tool; only used if cuts needing it are defined */
  ToolHandle<IegammaMiddleShape> m_egammaMiddleShape {this, 
      "egammaMiddleShapeTool", "egammaMiddleShape/egammamiddleshape",
      "Optional tool that performs cuts on middle shape variables"};

  /** @brief Tool for hadronic leakage calculation; onlud used if cuts needing are defined*/
  ToolHandle<IegammaIso> m_HadronicLeakageTool {this,
    "HadronicLeakageTool", "egammaIso", 
    "Handle of the EMCaloIsolationTool for Hadronic leakage"};

  Gaudi::Property<double>  m_ClusterEtCut {this,
      "ClusterEtCut", 0.0, "Cut on cluster EM+Had Et"};

  Gaudi::Property<double> m_EMEtCut {this,
      "EMEtCut", 0.0, "EM Et cut"};

  Gaudi::Property<double>  m_EMEtSplittingFraction {this,
      "EMEtSplittingFraction", 1.0, "Apply only fraction of EMEt cut for crack region"};

  Gaudi::Property<double>  m_EMFCut {this,
      "EMFCut", 0.0, "Cut on cluster EM fraction"};

  static constexpr double RETA_DEFAULT_NO_CUT = 0.0;
  Gaudi::Property<double>  m_RetaCut {this,
      "RetaCut", RETA_DEFAULT_NO_CUT, "Cut on cluster Reta"};

  static constexpr double HAD_LEAK_DEFAULT_NO_CUT = 999.;
  Gaudi::Property<double>  m_HadLeakCut {this,
      "HadLeakCut", HAD_LEAK_DEFAULT_NO_CUT, "Cut on cluster Hadronic Leakage"};

  // these variables are set at initialize based on the configuration
  bool m_doReta{false}; 
  bool m_doHadLeak{false};
};

#endif // RECONSTRUCTION/EGAMMA/EGAMMACALOTOOLS_EGAMMACALOCLUSTERSELECTOR_H
