/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauLeadTrackFillerTool.cxx 490131 2012-03-22 13:19:50Z ssnyder $
/**
 * @file TauD3PDMaker/src/TauLeadTrackFillerTool.cxx
 * @author Michel Trottier-McDonald <mtm@cern.ch>
 * @date March, 2012
 * @brief Block filler tool for lead track information
 */

#include "TauLeadTrackFillerTool.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {
    
    
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    TauLeadTrackFillerTool::TauLeadTrackFillerTool (const std::string& type,
						    const std::string& name,
						    const IInterface* parent)
	: BlockFillerTool<Analysis::TauJet> (type, name, parent),
	  m_leadTrack_eta(), m_leadTrack_phi()
    {}
    
    
    /**
     * @brief Book variables for this block.
     */
    StatusCode TauLeadTrackFillerTool::book()
    {
	CHECK( addVariable ("leadTrack_eta",  m_leadTrack_eta )  );
	CHECK( addVariable ("leadTrack_phi",  m_leadTrack_phi )  );
	
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
    StatusCode TauLeadTrackFillerTool::fill (const Analysis::TauJet& p)
    {
	int nTracks = p.numTrack();
	float leadTrackEta = -1111;
	float leadTrackPhi = -1111;
	float maxPtFound = 0.0;
	
	if(nTracks > 0)
	    for(int i = 0; i < nTracks; ++i)
	    {
		const Rec::TrackParticle *track = p.track(i);
		float leadTrackPt = track->pt();

		if(leadTrackPt > maxPtFound)
		{
		    leadTrackEta = track->eta();
		    leadTrackPhi = track->phi();
		    maxPtFound   = leadTrackPt;  
		}
	    }
	
	*m_leadTrack_eta = leadTrackEta;
	*m_leadTrack_phi = leadTrackPhi;
	
	return StatusCode::SUCCESS;
    } // end fill method
    
} // namespace D3PD
