// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/CellFillerTool.h
 * @author Jet Goodson <JGoodson@cern.ch>
 * @date Sep, 2009
 * @brief Block filler tool for cells
 */

#ifndef CALOD3PDMAKER_CELLFILLERTOOL_H
#define CALOD3PDMAKER_CELLFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "CaloEvent/CaloCellContainer.h"

class CaloCluster;


namespace D3PD {


/**
 * @brief Block filler tool for EM samplings from a CaloCluster.
 */
class CellFillerTool
  : public BlockFillerTool<CaloCell>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  CellFillerTool (const std::string& type,
			     const std::string& name,
			     const IInterface* parent);


  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const CaloCell& p);


private:
 

  /// parameters
  double *m_cellEnergy;
  double *m_cellEta;
  double *m_cellPhi;
  double *m_cellSinTh;
  double *m_cellCosTh;
  double *m_cellCotTh;
  double *m_cellX;
  double *m_cellY;
  double *m_cellZ;
  double *m_cellEt;
  int *m_cellGain;
  bool *m_badCell;
  int *m_samplingID;
  float *m_time;
  uint16_t *m_provenance;
  uint16_t *m_quality;
  
};


} // namespace D3PD


#endif // not CALOD3PDMAKER_CELLFILLERTOOL_H
