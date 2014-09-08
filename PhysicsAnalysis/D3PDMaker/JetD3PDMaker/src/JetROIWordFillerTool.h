// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file   JetD3PDMaker/src/JetROIWordFillerTool.h
 * @author Haifeng Li <Haifeng.Li@cern.ch>
 * @date   July 13, 2010
 * @brief Block filler tool for Jet ROIWord
 */

#ifndef JETD3PDMAKER_JETROIWORDFILLERTOOL_H
#define JETD3PDMAKER_JETROIWORDFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
class Jet;


namespace D3PD {


/**
 * @brief Block filler tool for Jet Moments.
 */
class JetROIWordFillerTool
  : public BlockFillerTool<Jet>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetROIWordFillerTool (const std::string& type,
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
  virtual StatusCode fill (const Jet& p);
  
  
private:

  /// Variable: Jet RoIword
  unsigned int *m_RoIword ;
  
};


} // namespace D3PD


#endif // not JETD3PDMAKER_JETROIWORDFILLERTOOL_H

