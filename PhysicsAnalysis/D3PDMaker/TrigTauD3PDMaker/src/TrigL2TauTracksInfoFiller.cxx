/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TrigL2TauTracksInfoFiller.cxx
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @author E. Ptacek <eptacek@uoregon.edu>
 * @date Feb, 2010
 * @brief Block filler tool for a trigtautracksinfo object.
 */


#include "TrigL2TauTracksInfoFiller.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
   TrigL2TauTracksInfoFiller::TrigL2TauTracksInfoFiller (const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
					     : BlockFillerTool<TrigTauTracksInfo> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}

/**
* @brief Book variables for this block.
*/
  StatusCode TrigL2TauTracksInfoFiller::book()
  {
    CHECK( addVariable ("nCoreTracks",   m_nCoreTracks)  );
    CHECK( addVariable ("nSlowTracks",   m_nSlowTracks)  );
    CHECK( addVariable ("nIsoTracks",   m_nIsoTracks)  );
    CHECK( addVariable ("charge",   m_charge)  );
    CHECK( addVariable ("leadingTrackPt", m_leadingTrackPt) );
    CHECK( addVariable ("scalarPtSumCore",   m_scalarPtSumCore)  );
    CHECK( addVariable ("scalarPtSumIso",   m_scalarPtSumIso)  );

    CHECK( addVariable ("3fastest_pt",   m_3fastest_pt)  );
    CHECK( addVariable ("3fastest_eta",   m_3fastest_eta)  );
    CHECK( addVariable ("3fastest_phi",   m_3fastest_phi)  );
    CHECK( addVariable ("3fastest_m",   m_3fastest_m)  );

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
  StatusCode TrigL2TauTracksInfoFiller::fill (const TrigTauTracksInfo& p)
  {
    /// add vars
    *m_nCoreTracks = p.nCoreTracks();
    *m_nSlowTracks = p.nSlowTracks();
    *m_nIsoTracks = p.nIsoTracks();
    *m_charge = p.charge();
    *m_leadingTrackPt = p.leadingTrackPt();
    *m_scalarPtSumCore = p.scalarPtSumCore();
    *m_scalarPtSumIso  = p.scalarPtSumIso();

    *m_3fastest_pt = p.threeFastestTracks().pt();
    *m_3fastest_eta = p.threeFastestTracks().eta();
    *m_3fastest_phi = p.threeFastestTracks().phi();
    *m_3fastest_m = p.threeFastestTracks().m();

    return StatusCode::SUCCESS;
  }





} // namespace D3PD
