// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/egammaLayer1ExtraFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Fill additional layer 1 quantities not available directly as details.
 */


#ifndef EGAMMAD3PDMAKER_EGAMMALAYER1EXTRAFILLERTOOL_H
#define EGAMMAD3PDMAKER_EGAMMALAYER1EXTRAFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODEgamma/Egamma.h"


namespace D3PD {


class egammaLayer1ExtraFillerTool
  : public BlockFillerTool<xAOD::Egamma>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  egammaLayer1ExtraFillerTool (const std::string& type,
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
  /// Variable: difference between 2nd maximum and 1st minimum in strips.
  float* m_deltaEs;
};


} // namespace D3PD


#endif // not EGAMMAD3PDMAKER_EGAMMALAYER1EXTRAFILLERTOOL_H
