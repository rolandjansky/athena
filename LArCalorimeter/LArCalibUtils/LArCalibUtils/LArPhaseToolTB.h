// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LArPhaseToolTB.h,v 1.1 2006-06-27 18:32:03 ssnyder Exp $
/**
 * @file  LArCalibUtils/LArPhaseToolTB.h
 * @author scott snyder
 * @date Feb 2006
 * @brief Retrieve the phase for a LAr EM cell, for combined test beam data.
 */

#ifndef LARCALIBUTILS_LARPHASETOOLTB_H
#define LARCALIBUTILS_LARPHASETOOLTB_H

#include "LArElecCalib/ILArPhaseTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include <string>


class StoreGateSvc;


/*
 * @brief Retrieve the phase for a LAr EM cell, for combined test beam data.
 *
 * The phase is the time offset (in ns) between the start of the pulse
 * in the cell and the 25ns digitization clock.  Normally,
 * this should be close to zero.
 * However, in the case of out-of-time hits, such as from cosmics,
 * it can be different from zero, and may also vary from cell to cell.
 * The phase may also be non-zero at the test beam.
 *
 * This version retrieves the phase information from the
 * test beam scintillators.
 */
class LArPhaseToolTB
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
  LArPhaseToolTB (const std::string& type,
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
  /// The phase value for the current event.
  float m_tbphase;

  /// True if the value in m_tbphase is valid for this event.
  bool m_have_phase;

  /// JO parameter: the storegate key of the test beam
  /// phase information.
  std::string m_phasekey;
};


#endif  // not LARCALIBUTILS_LARPHASETOOLTB_H

