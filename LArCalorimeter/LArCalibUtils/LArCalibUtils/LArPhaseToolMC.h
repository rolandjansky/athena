// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LArPhaseToolMC.h,v 1.1 2006-06-27 18:32:02 ssnyder Exp $
/**
 * @file  LArCalibUtils/LArPhaseToolMC.h
 * @author scott snyder
 * @date Mar 2006
 * @brief Retrieve the phase for a LAr EM cell, for MC data.
 */

#ifndef LARCALIBUTILS_LARPHASETOOLMC_H
#define LARCALIBUTILS_LARPHASETOOLMC_H

#include "LArElecCalib/ILArPhaseTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include <vector>
#include <string>


class CaloCell_ID;
class ITriggerTime;


/*
 * @brief Retrieve the phase for a LAr EM cell, for MC data.
 *
 * The phase is the time offset (in ns) between the start of the pulse
 * in the cell and the 25ns digitization clock.  Normally,
 * this should be close to zero.
 * However, in the case of out-of-time hits, such as from cosmics,
 * it can be different from zero, and may also vary from cell to cell.
 * The phase may also be non-zero at the test beam.
 *
 * This version looks at MC information to try to find the true
 * phase for MC events.  It tries to do the same thing that's
 * done during digitization.
 */
class LArPhaseToolMC
  : public AthAlgTool,
    virtual public ILArPhaseTool,
    virtual public IIncidentListener
{
public: 
  /**
   * @brief Constructor.
   * @param type The type of the tool.
   * @param name The name of the tool.
   * @param parent The parent algorithm of the tool.
   */
  LArPhaseToolMC (const std::string& type,
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

  /*
   * @brief Incident handler.
   * @param incident The incident to handle.
   * 
   * Used to clear cached data at the end of an event.
   */
  virtual void handle (const Incident& incident);

private:
  /// Called the first time that @c phase is called for a given event.
  /// Fills the @c m_phases table.
  StatusCode fill_phases();

  /// Table of phase values, indexed by cell hash index.
  std::vector<float> m_phases;

  /// Tool to calculate cell hashes.
  const CaloCell_ID* m_idhelper;

  /// Pointer to tool to get the global trigger time.
  ITriggerTime* m_trigtime_tool;

  /// Job options.
  /// List of hit container names to process.
  std::vector<std::string> m_container_names;

  /// Name of tool to get the global trigger time.
  /// If it's not set, we just use 0 always.
  std::string m_trigtime_tool_name;
};


#endif  // not LARCALIBUTILS_LARPHASETOOLMC_H

