/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/L2MuonCB2FillerTool.h
 * @author srivas prasad <srivas.prasad@cern.ch>
 * @date Jan 2010
 * @brief Block filler tool for CombinedMuonFeature charge, detail level 2
 *
 * ??? This should be changed to operate on TrigInDetTrack and
 *     moved to TriggerD3PDMaker.
 */

#include "L2MuonCB2FillerTool.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
L2MuonCB2FillerTool::L2MuonCB2FillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
  : BlockFillerTool<CombinedMuonFeature> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode L2MuonCB2FillerTool::book()
{
  CHECK( addVariable ("nPixHits"      ,   m_NPixelSpacePoints) );
  CHECK( addVariable ("nSCTHits"      ,   m_NSCT_SpacePoints) );
  CHECK( addVariable ("nTRTHits"      ,   m_NStrawHits) );
  CHECK( addVariable ("nTRTHighTHits" ,   m_NTRHits) );
 
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode L2MuonCB2FillerTool::fill (const CombinedMuonFeature& p)
{
  const TrigInDetTrack* idtrk = p.IDTrack();
  *m_NPixelSpacePoints = (idtrk ? idtrk->NPixelSpacePoints() : -999);
  *m_NSCT_SpacePoints = (idtrk ? idtrk->NSCT_SpacePoints() : -999);
  *m_NStrawHits = (idtrk ? idtrk->NStrawHits() : -999);
  *m_NTRHits = (idtrk ? idtrk->NTRHits() : -999);
  return StatusCode::SUCCESS;
}


} // namespace D3PD
