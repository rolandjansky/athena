// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LArPhaseToolConst.h,v 1.1 2006-06-27 18:32:01 ssnyder Exp $
/**
 * @file  LArCalibUtils/LArPhaseToolConst.h
 * @author scott snyder
 * @date Feb 2006
 * @brief Return a constant LAr phase.
 */

#ifndef LARCALIBUTILS_LARPHASETOOLCONST_H
#define LARCALIBUTILS_LARPHASETOOLCONST_H

#include "LArElecCalib/ILArPhaseTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include <string>


/*
 * @brief Return a constant LAr phase.
 *
 * The phase is the time offset (in ns) between the start of the pulse
 * in the cell and the 25ns digitization clock.  Normally,
 * this should be close to zero.
 * However, in the case of out-of-time hits, such as from cosmics,
 * it can be different from zero, and may also vary from cell to cell.
 * The phase may also be non-zero at the test beam.
 *
 * This version just returns a constant value (set from job options)
 * for all cells.
 */
class LArPhaseToolConst
  : public AthAlgTool,
    virtual public ILArPhaseTool
{
public: 
  /**
   * @brief Constructor.
   * @param type The type of the tool.
   * @param name The name of the tool.
   * @param parent The parent algorithm of the tool.
   */
  LArPhaseToolConst (const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

  /// Standard Gaudi initialize method.
  virtual StatusCode initialize ();

  /*
   * @brief Retrieve the phase for a specific LAr cell.
   * @param cell The cell for which we want the phase.
   * @param[out] phase The phase for the cell.
   */
  virtual StatusCode phase (const Identifier& cell, float& phase);


private:
  /// The phase set from job options.
  float m_phase;
};


#endif  // not LARCALIBUTILS_LARPHASETOOLCONST_H

