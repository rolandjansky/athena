// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file photonD3PDMaker/src/PAUElectronFillerTool.h
 * @author Mike Hance <hance@hep.upenn.edu>
 * @date March, 2010
 * @brief Fill PAU related variables
 */


#ifndef PHOTOND3PDMAKER_PAUELECTRONFILLERTOOL_H
#define PHOTOND3PDMAKER_PAUELECTRONFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include <vector>
#include <string>

namespace PAU{
  class egamma;
}


namespace D3PD {


/**
 * @brief Fill author code and related flags.
 */
class PAUElectronFillerTool
  : public BlockFillerTool<PAU::egamma>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  PAUElectronFillerTool (const std::string& type,
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
  virtual StatusCode fill (const PAU::egamma& p);


private:

  float * m_ambiguityResult;
  
};


} // namespace D3PD


#endif // not PHOTOND3PDMAKER_PAUELECTRONFILLERTOOL_H
