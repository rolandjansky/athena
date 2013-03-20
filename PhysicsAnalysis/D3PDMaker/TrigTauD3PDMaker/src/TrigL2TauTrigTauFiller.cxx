/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TrigL2TauTrigTauFiller.cxx
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @author E. Ptacek <eptacek@uoregon.edu>
 * @date Feb, 2010
 * @brief Block filler tool for trigtau.
 */


#include "TrigL2TauTrigTauFiller.h"
#include "TrigParticle/TrigTau.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
   TrigL2TauTrigTauFiller::TrigL2TauTrigTauFiller (const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
					     : BlockFillerTool<TrigTau> (type, name, parent)
{
  // Avoid coverity warnings.
  m_detailLevel = 1;
  m_detailLvlInc = true;
  book().ignore();

  declareProperty( "DetailLevel", m_detailLevel = 0 );
  declareProperty( "DetailLvlInc", m_detailLvlInc = false);
}

/**
* @brief Book variables for this block.
*/
  StatusCode TrigL2TauTrigTauFiller::book()
  {
    if( m_detailLevel==0 || m_detailLvlInc ){
      CHECK( addVariable ("RoIWord", m_roiword ) );
    }
    if( m_detailLevel==1 || (m_detailLevel>0 && m_detailLvlInc) ){
      CHECK( addVariable ("simpleEtFlow",   m_simpleEtFlow)  );
      CHECK( addVariable ("nMatchedTracks",   m_nMatchedTracks)  );
      CHECK( addVariable ("trkAvgDist",          m_trkAvgDist  )  );
      CHECK( addVariable ("etOverPtLeadTrk",     m_etOverPtLeadTrk)  );
    }
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
  StatusCode TrigL2TauTrigTauFiller::fill (const TrigTau& p)
  {
    if( m_detailLevel==0 || m_detailLvlInc){
      //add ROI word for Trigger
      (*m_roiword) = p.roiId();
    }
    if( m_detailLevel==1 || (m_detailLevel>0 && m_detailLvlInc) ){
      /// add vars
      *m_simpleEtFlow      = p.simpleEtFlow();
      *m_nMatchedTracks    = p.nMatchedTracks();
      *m_trkAvgDist        = p.trkAvgDist();
      *m_etOverPtLeadTrk   = p.etOverPtLeadTrk();
      
    }
    return StatusCode::SUCCESS;
  }

} // namespace D3PD
