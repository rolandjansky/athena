/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TauD3PDMaker/src/TauLeadTrackFillerTool.h
 * @date March, 2012
 * @brief Block filler tool for the tau lead track
 */

#ifndef TAUD3PDMAKER_TAULEADTRACKFILLERTOOL_H
#define TAUD3PDMAKER_TAULEADTRACKFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
namespace Analysis { class TauJet; }


namespace D3PD {


/**
 * @brief Block filler tool for tau track details.
 *
 */
    class TauLeadTrackFillerTool
	: public BlockFillerTool<Analysis::TauJet>
    {
    public:
	/**
	 * @brief Standard Gaudi tool constructor.
	 * @param type The name of the tool type.
	 * @param name The tool name.
	 * @param parent The tool's Gaudi parent.
	 */
	TauLeadTrackFillerTool (const std::string& type,
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
	
	float* m_leadTrack_eta;
	float* m_leadTrack_phi;
	
    };
    
    
} // namespace D3PD

#endif
