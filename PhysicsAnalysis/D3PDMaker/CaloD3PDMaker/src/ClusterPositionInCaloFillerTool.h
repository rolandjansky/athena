// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/ClusterPositionInCaloFillerTool.h
 * @author Bruno Lenzi <Bruno.Lenzi@cern.ch>
 * @date May, 2012
 * @brief Block filler tool for eta/phi positions in calo coordinates from a CaloCluster.
 */

#ifndef CALOD3PDMAKER_CLUSTERPOSITIONINCALOFILLERTOOL_H
#define CALOD3PDMAKER_CLUSTERPOSITIONINCALOFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloCellDetPos.h"
class CaloCluster;
class CaloCellDetPos;

namespace D3PD {


/**
 * @brief Block filler tool for eta/phi positions in calo coordinate from a CaloCluster.
 */
class ClusterPositionInCaloFillerTool
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
  ClusterPositionInCaloFillerTool (const std::string& type,
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
   * @brief Fill one block.
   * @param p The input object.
   *
   * This handles either cluster type.
   */
  template <class T>
  StatusCode fillT (const T& p);


  /// Variable: eta position in calo frame.
  double* m_etaCalo;

  /// Variable: phi position in calo frame.
  double* m_phiCalo;

  /// Variable: seed eta position in calo frame.
  double* m_eta0Calo;

  /// Variable: seed phi position in calo frame.
  double* m_phi0Calo;

  /// Variable: whether to dump the seed coordinates
  bool m_FillSeedCoordinates;

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloDetDescrMgrKey{
    this,
    "CaloDetDescrManager",
    "CaloDetDescrManager",
    "SG Key for CaloDetDescrManager in the Condition Store"
  };

};


} // namespace D3PD


#endif // not CALOD3PDMAKER_CLUSTERPOSITIONINCALOFILLERTOOL_H
