// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/ClusterEMSamplingFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Block filler tool for EM samplings from a CaloCluster.
 */

#ifndef CALOD3PDMAKER_CLUSTEREMSAMPLINGFILLERTOOL_H
#define CALOD3PDMAKER_CLUSTEREMSAMPLINGFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODCaloEvent/CaloCluster.h"
#include <vector>
#include <algorithm>
class CaloCluster;


namespace D3PD {


/**
 * @brief Block filler tool for EM samplings from a CaloCluster.
 */
class ClusterEMSamplingFillerTool
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
  ClusterEMSamplingFillerTool (const std::string& type,
                               const std::string& name,
                               const IInterface* parent);


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
   * @brief Fill one block.
   * @param p The input object.
   *
   * This handles either cluster type.
   */
  template <class T>
  StatusCode fillT (const T& p);


  static const unsigned int NEMSAMP = 4;

  /// Property: Should energies be written?
  bool m_writeEnergy;

  /// Property: Should eta be written?
  bool m_writeEta;

  /// Property: Should phi be written?
  bool m_writePhi;

  /// Property: List of samplings to write (in the range 0-3).
  /// An empty list means to write all.
  std::vector<int> m_samplings;

  /// Variable: Sampling energies.
  float* m_Es[NEMSAMP];

  /// Variable: Sampling eta centroids.
  float* m_etas[NEMSAMP];

  /// Variable: Sampling phi centroids.
  float* m_phis[NEMSAMP];
};


} // namespace D3PD


#endif // not CALOD3PDMAKER_CLUSTEREMSAMPLINGFILLERTOOL_H
