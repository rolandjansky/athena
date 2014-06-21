// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauClustersDetailsFillerTool.h
 * @date December, 2009
 * @brief Block filler tool for TauClustersDetails variables.
 */

#ifndef TAUD3PDMAKER_TAUCLUSTERSDETAILSFILLERTOOL_H
#define TAUD3PDMAKER_TAUCLUSTERSDETAILSFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
namespace Analysis { class TauJet; }


namespace D3PD {
    
    
    /**
     * @brief Block filler tool for tau cluster details.
     *
     */
    class TauClustersDetailsFillerTool
	: public BlockFillerTool<Analysis::TauJet>
    {
    public:
	/**
	 * @brief Standard Gaudi tool constructor.
	 * @param type The name of the tool type.
	 * @param name The tool name.
	 * @param parent The tool's Gaudi parent.
	 */
	TauClustersDetailsFillerTool (const std::string& type,
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
	std::vector<float>* m_clusterenergy;
	std::vector<float>* m_clustereta;
	std::vector<float>* m_clusterphi;
	std::vector<float>* m_clustereta_atTJVA;
	std::vector<float>* m_clusterphi_atTJVA;
	std::vector<float>* m_clusterPSSF;
	std::vector<float>* m_clusterEM2F;
	std::vector<float>* m_clusterEM3F;
	unsigned int* m_numclusters;
    };
    
    
} // namespace D3PD


#endif // not TAUD3PDMAKER_TAUCLUSTERSDETAILSFILLERTOOL_H
