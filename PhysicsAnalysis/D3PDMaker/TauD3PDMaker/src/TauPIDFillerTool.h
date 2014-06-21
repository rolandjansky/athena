// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauPIDFillerTool.h
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @date September, 2009
 * @brief Block filler tool for tau PID variables.
 */

#ifndef TAUD3PDMAKER_TAUPIDFILLERTOOL_H
#define TAUD3PDMAKER_TAUPIDFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
namespace Analysis { class TauJet; }


namespace D3PD {


/**
 * @brief Block filler tool for tau PID.
 *
 */
    class TauPIDFillerTool
	: public BlockFillerTool<Analysis::TauJet>
    {
    public:
	/**
	 * @brief Standard Gaudi tool constructor.
	 * @param type The name of the tool type.
	 * @param name The tool name.
	 * @param parent The tool's Gaudi parent.
	 */
	TauPIDFillerTool (const std::string& type,
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
	float* m_BDTEleScore;
	float* m_BDTJetScore;
	float* m_likelihood;
	float* m_SafeLikelihood;

	/// Now the "bits" which have "bool" accessors in EDM
	int* m_electronVetoLoose;
	int* m_electronVetoMedium;
	int* m_electronVetoTight;
	int* m_muonVeto;
	// int* m_tauCutLoose;
	// int* m_tauCutMedium;
	// int* m_tauCutTight;
	int* m_tauLlhLoose;
	int* m_tauLlhMedium;
	int* m_tauLlhTight;
	int* m_JetBDTSigLoose;
	int* m_JetBDTSigMedium;
	int* m_JetBDTSigTight;
	int* m_EleBDTLoose;
	int* m_EleBDTMedium;
	int* m_EleBDTTight;
        //int* m_SafeLlhLoose;

    };


} // namespace D3PD


#endif // not TAUD3PDMAKER_TAUPIDFILLERTOOL_H
