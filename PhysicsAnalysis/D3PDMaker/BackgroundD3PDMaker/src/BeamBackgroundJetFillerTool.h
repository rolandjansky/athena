/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BeamBackgroundJetFillerTool.h 589851 2014-03-27 20:01:30Z ssnyder $
#ifndef D3PDMAKER_BEAMBACKGROUNDJETFILLERTOOL_H
#define D3PDMAKER_BEAMBACKGROUNDJETFILLERTOOL_H

#include "RecBackgroundEvent/BeamBackgroundData.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODJet/Jet.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/errorcheck.h"
#include <vector>
#include <string>


namespace D3PD {

/**
 * @brief Block filler tool for the fake jet results of the Beam Background Identification Method
 *
 * @author David Salek <David.Salek@cern.ch>
 *
 * $Revision: 589851 $
 * $Date: 2014-03-27 21:01:30 +0100 (Thu, 27 Mar 2014) $
 */
class BeamBackgroundJetFillerTool
  : public BlockFillerTool<xAOD::Jet>
{
public:
  BeamBackgroundJetFillerTool (const std::string& type,
      const std::string& name,
      const IInterface* parent);


  // Book variables for this block.
  virtual StatusCode book();

  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const xAOD::Jet& p);

private:
  /// Property: SG key for the beam background object.
  std::string m_beamBackgroundKey;

  BeamBackgroundData* m_beamBackgroundData;

  int* m_bbDecision;
};

} // namespace D3PD

#endif
