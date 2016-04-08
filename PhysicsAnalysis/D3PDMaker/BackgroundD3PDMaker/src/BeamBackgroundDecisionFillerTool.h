/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BeamBackgroundDecisionFillerTool.h 502891 2012-05-29 11:04:54Z salekd $
#ifndef D3PDMAKER_BEAMBACKGROUNDDECISIONFILLERTOOL_H
#define D3PDMAKER_BEAMBACKGROUNDDECISIONFILLERTOOL_H

#include "RecBackgroundEvent/BeamBackgroundData.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/errorcheck.h"
#include <string>

class BeamBackgroundData;

namespace D3PD {

/**
 * @brief Block filler tool for the event-level summary of the Beam Background Identification Method
 *
 * @author David Salek <David.Salek@cern.ch>
 *
 * $Revision: 502891 $
 * $Date: 2012-05-29 13:04:54 +0200 (Tue, 29 May 2012) $
 */
class BeamBackgroundDecisionFillerTool
  : public BlockFillerTool<BeamBackgroundData>
{
public:
  BeamBackgroundDecisionFillerTool (const std::string& type,
      const std::string& name,
      const IInterface* parent);

  StatusCode initialize();

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
  virtual StatusCode fill (const BeamBackgroundData& beamBackgroundData);

private:
  StoreGateSvc* m_storeGate;

  int* m_decision;
  int* m_numSegment;
  int* m_numSegmentEarly;
  int* m_numSegmentACNoTime;
  int* m_numSegmentAC;
  int* m_numMatched;
  int* m_numNoTimeLoose;
  int* m_numNoTimeMedium;
  int* m_numNoTimeTight;
  int* m_numOneSidedLoose;
  int* m_numOneSidedMedium;
  int* m_numOneSidedTight;
  int* m_numTwoSidedNoTime;
  int* m_numTwoSided;
  int* m_numClusterShape;
  int* m_direction;
};

} // namespace D3PD

#endif
