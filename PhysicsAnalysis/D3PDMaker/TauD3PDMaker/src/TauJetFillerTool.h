// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauJetFillerTool.h
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @author Ryan Reece <ryan.reece@cern.ch>
 * @date September, 2009
 * @brief Block filler tool for taujet variables.
 */

#ifndef TAUD3PDMAKER_TAUJETFILLERTOOL_H
#define TAUD3PDMAKER_TAUJETFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
namespace Analysis { class TauJet; }


namespace D3PD {


/**
 * @brief Block filler tool for taujet.
 *
 */
    class TauJetFillerTool
	: public BlockFillerTool<Analysis::TauJet>
    {
    public:
	/**
	 * @brief Standard Gaudi tool constructor.
	 * @param type The name of the tool type.
	 * @param name The tool name.
	 * @param parent The tool's Gaudi parent.
	 */
	TauJetFillerTool (const std::string& type,
			  const std::string& name,
			  const IInterface* parent);
	
	
	/// Book variables for this block.
	virtual StatusCode book();
	
	
	/**
	 * @brief Fill one block --- type-safe version.
	 * @param p The input object.
	 *
	 * This is called once per object.  The caller
	 * is responsible for arranging that all the pointers for booked variables
	 * are set appropriately upon entry.
	 */
	virtual StatusCode fill (const Analysis::TauJet& p);
	
	
    private:
	
	/// Variables from each stored discriminant output
	/// if it is calo-only, author=1, if track-only author=2, 
	/// if calo+trk author=3, else author = 0
	int* m_author;
	int* m_roiword; /// ROIWord for trigger
	int* m_nprong;
	int* m_numTrack;
	int* m_seedCalo_numTrack;
	
	int* m_seedCalo_nWideTrk;
	int* m_nOtherTrk;
	
    };
    
    
} // namespace D3PD


#endif // not TAUD3PDMAKER_TAUJETFILLERTOOL_H
