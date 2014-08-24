// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/egammaRetaphiFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Fill reta/rphi.
 */


#ifndef EGAMMAD3PDMAKER_EGAMMARETAPHIFILLERTOOL_H
#define EGAMMAD3PDMAKER_EGAMMARETAPHIFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODEgamma/Egamma.h"


namespace D3PD {


class egammaRetaphiFillerTool
  : public BlockFillerTool<xAOD::Egamma>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  egammaRetaphiFillerTool (const std::string& type,
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
  virtual StatusCode fill (const xAOD::Egamma& p);


private:
  /// Variable: E237 / E277.
  float* m_reta;

  /// Variable: E233 / E237.
  float* m_rphi;
};


} // namespace D3PD


#endif // not EGAMMAD3PDMAKER_EGAMMARETAPHIFILLERTOOL_H
