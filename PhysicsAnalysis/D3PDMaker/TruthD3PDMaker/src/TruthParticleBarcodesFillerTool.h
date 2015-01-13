/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TruthD3PDMaker/src/TruthParticleBarcodesFillerTool.h
 * @author sss
 * @date Mar, 2012
 * @brief Block filler tool for TruthParticle barcode lists (parent/child)
 */

#ifndef TRUTHD3PDMAKER_TRUTHPARTICLEBARCODESFILLERTOOL_H
#define TRUTHD3PDMAKER_TRUTHPARTICLEBARCODESFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "McParticleEvent/TruthParticle.h"
#include <vector>


namespace D3PD {

class TruthParticleBarcodesFillerTool : public BlockFillerTool<TruthParticle>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TruthParticleBarcodesFillerTool (const std::string& type,
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
  virtual StatusCode fill (const TruthParticle& p);


private:
  /// Variable: Barcodes of parents.
  std::vector<int>* m_parents;

  /// Variable: Barcodes of children.
  std::vector<int>* m_children;
};


} // namespace D3PD


#endif // not TRUTHD3PDMAKER_TRUTHPARTICLEBARCODESFILLERTOOL_H
