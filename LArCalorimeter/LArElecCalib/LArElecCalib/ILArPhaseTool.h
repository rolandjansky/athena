// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ILArPhaseTool.h,v 1.2 2006-06-27 18:25:52 ssnyder Exp $

/**
 * @file  LArElecCalib/ILArPhaseTool.h
 * @author scott snyder
 * @date Feb 2006
 * @brief Retrieve the phase for a LAr EM cell.
 */


#ifndef LARELECCALIB_ILARPHASETOOL_H
#define LARELECCALIB_ILARPHASETOOL_H

#include "GaudiKernel/IAlgTool.h"


class Identifier;

// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_ILArPhaseTool("ILArPhaseTool", 1 , 0);

/*
 * @brief Interface for retrieving the LAr phase on a cell-by-cell basis.
 *
 * The phase is the time offset (in ns) between the start of the pulse
 * in the cell and the 25ns digitization clock.  Normally,
 * this should be close to zero.
 * However, in the case of out-of-time hits, such as from cosmics,
 * it can be different from zero, and may also vary from cell to cell.
 * The phase may also be non-zero at the test beam.
 */
class ILArPhaseTool
  : virtual public IAlgTool
{
public: 
  /*
   * @brief Retrieve the phase for a specific LAr cell.
   * @param cell The cell for which we want the phase.
   * @param[out] phase The phase for the cell.
   */
  virtual StatusCode phase (const Identifier& cell, float& phase) = 0;

  /// Standard Gaudi interface ID declaration.
  static const InterfaceID& interfaceID() { return IID_ILArPhaseTool; }
};


#endif  // not LARELECCALIB_ILARPHASETOOL_H

