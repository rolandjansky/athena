// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/ClusterSamplingFillerTool.h
 * @author Maarten Boonekamp <maarten.boonekamp@cea.fr>
 * @date Sep, 2009
 * @brief Block filler tool for samplings from a CaloCluster.
 */

#ifndef CALOD3PDMAKER_CLUSTERSAMPLINGFILLERTOOL_H
#define CALOD3PDMAKER_CLUSTERSAMPLINGFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloCellDetPos.h"
#include <vector>
class CaloCluster;


namespace D3PD {

/**
 * @brief Block filler tool for EM samplings from a CaloCluster.
 */
class ClusterSamplingFillerTool
  : public BlockFillerTool<Types<CaloCluster, xAOD::CaloCluster> >
{
public:
  typedef BlockFillerTool<Types<CaloCluster, xAOD::CaloCluster> > Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  ClusterSamplingFillerTool (const std::string& type,
			     const std::string& name,
			     const IInterface* parent);


  virtual StatusCode initialize() override;

  /// Book variables for this block.
  virtual StatusCode book() override;


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const CaloCluster& p) override;


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const xAOD::CaloCluster& p) override;


private:
  /**
   * @brief Fill etamax/reco status.
   * @param p The input object.
   */
  template <class T>
  StatusCode fillEtamax (const T& p);


  /**
   * @brief Fill sampling variables.
   * @param eSamp Energy per sample.
   * @param etaSamp Eta per sample.
   * @param phiSamp Phi per sample.
   */
  StatusCode fillSamplings (const std::vector<double>& eSamp,
                            const std::vector<double>& etaSamp,
                            const std::vector<double>& phiSamp);


  static const unsigned int NSAMP = 24;

  /// Parameter: Samplings to write.  Empty means to write all.
  std::vector<unsigned int> m_samplings;

  /// parameters
  bool m_writeEmHadEnergies;
  bool m_writeSamplingEnergies;
  bool m_writeSamplingEtaPhi;
  bool m_writeSamplingEtaPhiRaw;
  bool m_writeSamplingEtamax;
  bool m_writeRecoStatus;

  /// Variable: total EM and HAD energies
  float* m_Eem;
  float* m_Ehad;

  /// Variable: Sampling energies.
  float* m_Es[NSAMP];

  /// Variable: Sampling eta centroids.
  float* m_etas[NSAMP];

  /// Variable: Sampling phi centroids.
  float* m_phis[NSAMP];

  /// Variable: Sampling raw eta centroids.
  float* m_raw_etas[NSAMP];

  /// Variable: Sampling raw phi centroids.
  float* m_raw_phis[NSAMP];

  /// Variable: Eta of maximum energy.
  float* m_etamax[NSAMP];
  
  unsigned int* m_RecoStatus;
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloDetDescrMgrKey{
    this,
    "CaloDetDescrManager",
    "CaloDetDescrManager",
    "SG Key for CaloDetDescrManager in the Condition Store"
  };
};

} // namespace D3PD


#endif // not CALOD3PDMAKER_CLUSTERSAMPLINGFILLERTOOL_H
