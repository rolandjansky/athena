/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

// STL includes
#include <string>

/**
 * @class egammaCaloClusterSelector
 * @brief A tool which specifies whether a cluster passes a selection
 * 
 * The tool implements the IegammaCaloClusterSelector interface which provides the passSelection method.
 * It selects xAOD::CaloCluster based on their properties in this order:
 * 
 * - minimum et
 * - cut on the fraction of the energy in the layers, by default using egammaCheckEnergyDepositTool
 *   (e.g. minimum energy fraction in 2nd layer, maximum fraction in PS, ...)
 * - having a second sampling (CaloSampling::EMB2 or CaloSampling::EME2)
 * - |eta| in the second layer not above 10
 * - energy in the second layer not below 50 MeV
 * - minimum transverse EM energy (considering rescaling, EM energy -> EM energy / EMEtSplittingFraction,
 *   for cluster in the crack; usually the rescaling increase the EM energy so loosing the cut,
 *   see ATLASRECTS-4772)
 * - minimum EM fraction
 * - maximum Reta
 * - maximum hadronic fraction: ethad / EMEt (ethad1 is used outside 0.8-1.37 eta2)
 * 
 * Several selection can be disabled. The cut on Reta and hadronic fraction are done if cut values are
 * specified. The cut on the fraction of the energy is done only if egammaCheckEnergyDepositTool is
 * spefified. See ATLASRECTS-5765.
 * 
 * The tool is generic, but it is used in egamma reconstruction by
 * CaloClusterROI_Selector and egammaSelectedTrackCopy
 * 
 **/
class egammaCaloClusterSelector : public extends<AthAlgTool, IegammaCaloClusterSelector> {
public:
  egammaCaloClusterSelector(const std::string& type, const std::string& name, const IInterface* parent);

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  /** @brief pass the selection described in the class egammaCaloClusterSelector */
  bool passSelection(const xAOD::CaloCluster* cluster,
                     const CaloDetDescrManager& cmgr) const override final;

private:
  /** @brief Name of the cluster intput collection*/
  SG::ReadHandleKey<CaloCellContainer> m_cellsKey {this,
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

  Gaudi::Property<double> m_ClusterEtCut {this,
      "ClusterEtCut", 0.0, "Cut on cluster EM+Had Et"};

  Gaudi::Property<double> m_EMEtCut {this,
      "EMEtCut", 0.0, "EM Et cut"};

  Gaudi::Property<double> m_EMEtSplittingFraction {this,
      "EMEtSplittingFraction", 1.0, "Apply only fraction of EMEt cut for crack region"};

  Gaudi::Property<double> m_EMFCut {this,
      "EMFCut", 0.0, "Cut on cluster EM fraction"};

  static constexpr double RETA_DEFAULT_NO_CUT = 0.0;
  Gaudi::Property<double> m_RetaCut {this,
      "RetaCut", RETA_DEFAULT_NO_CUT, "Cut on cluster Reta"};

  static constexpr double HAD_LEAK_DEFAULT_NO_CUT = 999.;
  Gaudi::Property<double> m_HadLeakCut {this,
      "HadLeakCut", HAD_LEAK_DEFAULT_NO_CUT, "Cut on cluster Hadronic Leakage"};

  // these variables are set at initialize based on the configuration
  bool m_doReta{false}; 
  bool m_doHadLeak{false};
};

#endif // RECONSTRUCTION/EGAMMA/EGAMMACALOTOOLS_EGAMMACALOCLUSTERSELECTOR_H
