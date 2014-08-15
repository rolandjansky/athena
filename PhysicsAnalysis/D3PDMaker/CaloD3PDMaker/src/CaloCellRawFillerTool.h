// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/CaloCellRawFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2012
 * @brief Block filler tool for calo cell raw eta/phi.
 */


#ifndef CALOD3PDMAKER_CALOCELLRAWFILLERTOOL_H
#define CALOD3PDMAKER_CALOCELLRAWFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
class CaloCell;


namespace D3PD {


/**
 * @brief Block filler tool for calo cell raw eta/phi.
 */
class CaloCellRawFillerTool
  : public BlockFillerTool<CaloCell>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  CaloCellRawFillerTool (const std::string& type,
                         const std::string& name,
                         const IInterface* parent);


  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   * @param p Object from which to fill.
   */
  virtual StatusCode fill (const CaloCell& p);


private:
  /// Variable: Cell raw eta.
  float* m_raw_eta;

  /// Variable: Cell raw phi.
  float* m_raw_phi;
};


} // namespace D3PD



#endif // not CALOD3PDMAKER_CALOCELLRAWFILLERTOOL_H
