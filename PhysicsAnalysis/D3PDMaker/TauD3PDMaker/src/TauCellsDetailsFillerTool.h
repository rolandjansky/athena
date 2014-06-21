// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauCellsDetailsFillerTool.h
 * @date December, 2009
 * @brief Block filler tool for TauCellsDetails variables.
 */

#ifndef TAUD3PDMAKER_TAUCELLSDETAILSFILLERTOOL_H
#define TAUD3PDMAKER_TAUCELLSDETAILSFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
namespace Analysis { class TauJet; }


namespace D3PD {
    
    
/**
 * @brief Block filler tool for tau track details.
 *
 */
    class TauCellsDetailsFillerTool
	: public BlockFillerTool<Analysis::TauJet>
    {
    public:
	/**
	 * @brief Standard Gaudi tool constructor.
	 * @param type The name of the tool type.
	 * @param name The tool name.
	 * @param parent The tool's Gaudi parent.
	 */
	TauCellsDetailsFillerTool (const std::string& type,
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
	std::vector<float>* m_cellenergy;
	std::vector<float>* m_celleta;
	std::vector<float>* m_cellphi;
	std::vector<float>* m_celleta_atTJVA;
	std::vector<float>* m_cellphi_atTJVA;
	std::vector<int>* m_samplingID;
	unsigned int* m_numcells;
	float* m_numEffCells;
	float* m_numEffStripCells;
    };
    
    
} // namespace D3PD


#endif // not TAUD3PDMAKER_TAUTRACKDETAILSFILLERTOOL_H
