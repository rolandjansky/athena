/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/L2MuonCB1FillerTool.h
 * @author srivas prasad <srivas.prasad@cern.ch>
 * @date Jan 2010
 * @brief Block filler tool for CombinedMuonFeature charge, detail level 1
 */

#include "L2MuonCB1FillerTool.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "AthenaKernel/errorcheck.h"



namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
L2MuonCB1FillerTool::L2MuonCB1FillerTool
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
StatusCode L2MuonCB1FillerTool::book()
{
  CHECK( addVariable ("id_pt" ,   m_id_pt) );
  CHECK( addVariable ("ms_pt" ,   m_ms_pt) );

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
StatusCode L2MuonCB1FillerTool::fill (const CombinedMuonFeature& p)
{
  const TrigInDetTrack* idtrk = p.IDTrack();
  const MuonFeature* mstrk = p.muFastTrack();
  const TrigInDetTrackFitPar* idfp = 0;

  //double ideta = 100;
  //double idphi = 100;

  if(idtrk) idfp = idtrk->param();
  if(idfp) {
    *m_id_pt = idfp->pT();
    //ideta = idfp->eta();
    //idphi = idfp->phi0();
  } else {
    *m_id_pt = -1.e9; // momenta are signed...small values not obviously junk. 1 PeV :)
  }

  if(mstrk) {
    *m_ms_pt = mstrk->pt() ;
  }else {
    *m_ms_pt = -1.e9;
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
