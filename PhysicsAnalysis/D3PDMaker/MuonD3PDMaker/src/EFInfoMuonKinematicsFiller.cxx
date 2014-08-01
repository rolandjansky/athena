/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/EFInfoMuonKinematicsFiller.h
 * @author srivas prasad <srivas.prasad@cern.ch>
 * @date Jan 2010
 * @brief Block filler tool for TrigMuonEFInfo, detail level 1
 */

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "EFInfoMuonKinematicsFiller.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"

#include "AthenaKernel/errorcheck.h"



namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
EFInfoMuonKinematicsFiller::EFInfoMuonKinematicsFiller
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
  : BlockFillerTool<TrigMuonEFInfo> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode EFInfoMuonKinematicsFiller::book()
{
  CHECK( addVariable ("cb_eta",   m_cb_eta) );
  CHECK( addVariable ("cb_phi",   m_cb_phi) );
  CHECK( addVariable ("cb_pt" ,   m_cb_pt) );

  CHECK( addVariable ("ms_eta",   m_ms_eta) );
  CHECK( addVariable ("ms_phi",   m_ms_phi) );
  CHECK( addVariable ("ms_pt" ,   m_ms_pt) );

  CHECK( addVariable ("me_eta",   m_me_eta) );
  CHECK( addVariable ("me_phi",   m_me_phi) );
  CHECK( addVariable ("me_pt" ,   m_me_pt) );

  //  CHECK( addVariable ("etadiff",   m_etadiff) );
  //  CHECK( addVariable ("phidiff",   m_phidiff) );
 
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
StatusCode EFInfoMuonKinematicsFiller::fill (const TrigMuonEFInfo& p)
{
  TrigMuonEFInfo &cp = const_cast<TrigMuonEFInfo &>(p);
  TrigMuonEFCbTrack* cbtrk = cp.CombinedTrack();
  *m_cb_eta          = (cbtrk ? cbtrk->eta() : -10);
  *m_cb_phi          = (cbtrk ? cbtrk->phi() : -10);
  *m_cb_pt           = (cbtrk && cbtrk->iPt()!= 0 ? cbtrk->pt()  : -1.e9);

  const TrigMuonEFTrack* mstrk = cp.SpectrometerTrack();
  *m_ms_eta          = (mstrk ? mstrk->eta() : -10);
  *m_ms_phi          = (mstrk ? mstrk->phi() : -10);
  *m_ms_pt           = (mstrk ? mstrk->pt()  : -1.e9);

  const TrigMuonEFTrack* metrk = cp.ExtrapolatedTrack();
  *m_me_eta          = (metrk ? metrk->eta() : -10);
  *m_me_phi          = (metrk ? metrk->phi() : -10);
  *m_me_pt           = (metrk ? metrk->pt()  : -1.e9);

  return StatusCode::SUCCESS;
}


} // namespace D3PD
