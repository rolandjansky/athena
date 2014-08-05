// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/PhotonTopoIsoAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Calculate isolation cones of topo clusters for egamma objects.
 *        Save as aux data.
 */


#ifndef EGAMMAD3PDANALYSIS_EGAMMATOPOISOALG_H
#define EGAMMAD3PDANALYSIS_EGAMMATOPOISOALG_H


#include "xAODEgamma/Egamma.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>
class IegammaTopoIso;

namespace D3PD {


class ICollectionGetterTool;


/**
 * @brief Calculate isolation cones of topo clusters for egamma objects.
 *        Save as UD.
 */
class egammaTopoIsoAlg
  : public AthAlgorithm
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  egammaTopoIsoAlg (const std::string& name,
                    ISvcLocator* svcloc);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize() override;


  /// Standard Gaudi @c execute method.
  virtual StatusCode execute() override;


private:
  /// Property: Prefix to add to aux data items.
  std::string m_auxPrefix;

  /// Calculate isolation cones in topo clusters around @a eg.
  StatusCode topoClustCones (const xAOD::Egamma& g,
                             const xAOD::CaloClusterContainer& clusts,
                             std::vector<float>& cones)
    const;


  /// Property: Getter for input photons.
  ToolHandle<ICollectionGetterTool> m_egammaGetter;

  /// Property: Getter for topo clusters.
  ToolHandle<ICollectionGetterTool> m_clusterGetter;

  /// Property: TopoIso calculator tool.
  ToolHandle<IegammaTopoIso > m_topoIsoTool;

  /// Property: If true, don't complain if input objects are missing.
  bool m_allowMissing;

  /// Property: List of cone sizes to process.
  std::vector<float> m_coneSizes;

  /// Property: Use TopoClusters at the EM scale.
  bool m_useEMScale;

  /// Property: Use only positive-energy TopoClusters.
  bool m_positiveClusters;

  /// List of UD variable names corresponding to each cone size.
  std::vector<std::string> m_names;
};


} // namespace D3PD


#endif // EGAMMAD3PDANALYSIS_EGAMMATOPOISOALG_H
