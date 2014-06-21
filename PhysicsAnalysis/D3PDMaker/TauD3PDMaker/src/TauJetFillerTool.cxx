/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauJetFillerTool.cxx
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @author Ryan Reece <ryan.reece@cern.ch>
 * @date Sept, 2009
 * @brief Block filler tool for taujet.
 */


#include "TauJetFillerTool.h"
#include "tauEvent/TauJet.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {
    
    
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
    TauJetFillerTool::TauJetFillerTool (const std::string& type,
					const std::string& name,
					const IInterface* parent)
	: BlockFillerTool<Analysis::TauJet> (type, name, parent)
    {
      book().ignore(); // Avoid coverity warnings.
    }
    
    
/**
 * @brief Book variables for this block.
 */
    StatusCode TauJetFillerTool::book()
    {
	CHECK( addVariable ("author",   m_author)  );
	CHECK( addVariable ("RoIWord",   m_roiword)  );
	CHECK( addVariable ("nProng",   m_nprong)  );
	CHECK( addVariable ("numTrack",   m_numTrack)  );
	CHECK( addVariable ("seedCalo_numTrack",   m_seedCalo_numTrack)  );
	CHECK( addVariable ("seedCalo_nWideTrk", m_seedCalo_nWideTrk)  );
	CHECK( addVariable ("nOtherTrk", m_nOtherTrk)  );
	
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
    StatusCode TauJetFillerTool::fill (const Analysis::TauJet& p)
    {
	/// if it is calo-only, author=1, if track-only author=2, 
	/// if calo+trk author=3, else author = 0
	if (p.hasAuthor(TauJetParameters::tauRec) && !p.hasAuthor(TauJetParameters::tau1P3P))
	    *m_author = 1;
	
	else if(p.hasAuthor(TauJetParameters::tau1P3P) && !p.hasAuthor(TauJetParameters::tauRec))
	    *m_author = 2;

	else if(p.hasAuthor(TauJetParameters::tauRec) && p.hasAuthor(TauJetParameters::tau1P3P))
	    *m_author = 3;

	else
	    *m_author = 0;

	
	/// add ROI word for Trigger
	*m_roiword = p.ROIWord();
	*m_nprong = p.nProng();
	*m_numTrack = p.numTrack();
	*m_seedCalo_numTrack = p.seedCalo_numTrack();
	
	const Analysis::TauCommonDetails*  p_taudetails(0);
	p_taudetails = p.details<const Analysis::TauCommonDetails>();
	if (p_taudetails==0)
	{
	    REPORT_MESSAGE (MSG::ERROR) << "TauJet has no associated CommonDetails!!";
	    return StatusCode::SUCCESS;
	}
	
	*m_seedCalo_nWideTrk = p_taudetails->seedCalo_nWideTrk();
	*m_nOtherTrk = p_taudetails->nOtherTrk();
	
	return StatusCode::SUCCESS;
    }
    
    
} // namespace D3PD
