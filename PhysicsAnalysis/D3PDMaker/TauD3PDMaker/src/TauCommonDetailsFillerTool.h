// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauCommonDetailsFillerTool.h
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @date September, 2009
 * @brief Block filler tool for TauCommonDetails variables.
 */

#ifndef TAUD3PDMAKER_TAUCOMMONDETAILSFILLERTOOL_H
#define TAUD3PDMAKER_TAUCOMMONDETAILSFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
namespace Analysis { class TauJet; }


namespace D3PD {
    
    
/**
 * @brief Block filler tool for tau common details.
 *
 */
    class TauCommonDetailsFillerTool
	: public BlockFillerTool<Analysis::TauJet>
    {
    public:
	/**
	 * @brief Standard Gaudi tool constructor.
	 * @param type The name of the tool type.
	 * @param name The tool name.
	 * @param parent The tool's Gaudi parent.
	 */
	TauCommonDetailsFillerTool (const std::string& type,
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
	
	/// 
	float* m_etOverPtLeadTrk;
	float* m_ipZ0SinThetaSigLeadTrk;
	float* m_leadTrkPt;
	int* m_nLooseTrk;
	int* m_nLooseConvTrk;
	int* m_nProngLoose;
	float* m_ipSigLeadTrk;
	float* m_ipSigLeadLooseTrk;
	float* m_etOverPtLeadLooseTrk;
	float* m_leadLooseTrkPt;
	float* m_chrgLooseTrk;
	float* m_massTrkSys;
	float* m_trkWidth2;
	float* m_trFlightPathSig;
	float* m_etEflow;
	float* m_mEflow;
	int* m_nPi0;
	float* m_ele_E237E277;
	float* m_ele_PresamplerFraction;
	float* m_ele_ECALFirstFraction;
	float* m_EM_TES_scale;
	float* m_LC_TES_precalib;
	float* m_TRTRatio;
	
	/// calo seeded
	float* m_seedCalo_EMRadius;
	float* m_seedCalo_hadRadius;
	float* m_seedCalo_etEMAtEMScale;
	float* m_seedCalo_etHadAtEMScale;
	float* m_seedCalo_isolFrac;
	float* m_seedCalo_centFrac;
	float* m_seedCalo_stripWidth2;      // strip width ^2
	int*   m_seedCalo_nStrip;           // number of strips
	float* m_seedCalo_eta;              // eta calculate from calo  
	float* m_seedCalo_phi;              // phi calculated from calo
	float* m_seedCalo_nIsolLooseTrk;    // number of isolated tracks
	float* m_seedCalo_trkAvgDist;       // average track distance to calorimeter seed
	float* m_seedCalo_trkRmsDist;       // RMS of track distance to calorimeter seed
	float* m_seedCalo_caloIso;          // sum of transverse energy clusters in 0.2 < dR < 0.4
	float* m_seedCalo_caloIsoCorrected; // same as above, corrected for pileup
	float* m_seedCalo_dRmax;            // maximal dR of tracks associated to calo-seeded tau
	float* m_seedCalo_lead2ClusterEOverAllClusterE; // Self-explanatory
	float* m_seedCalo_lead3ClusterEOverAllClusterE; // Self-explanatory
	float* m_seedCalo_etHadCalib;
	float* m_seedCalo_etEMCalib;

	int* m_numTopoClusters;             // Number of topological clusters
	float* m_numEffTopoClusters;        // Effective number of topological clusters
	float* m_topoInvMass;               // Invariant mass of the topological clusters
	float* m_effTopoInvMass;            // Invariant mass of the effective topological clusters
	float* m_topoMeanDeltaR;            // Average radius of the topological clusters to the geometrical center
	float* m_effTopoMeanDeltaR;         // Average radius of the effective topological clusters to the geometrical center
	float* m_numCells;                  // Number of cells
	
	/// track seeded
	float* m_seedTrk_EMRadius;
	float* m_seedTrk_isolFrac;
	float* m_seedTrk_etChrgHadOverSumTrkPt;
	float* m_seedTrk_isolFracWide;
	float* m_seedTrk_etHadAtEMScale; 
	float* m_seedTrk_etEMAtEMScale;
	float* m_seedTrk_etEMCL; // E_T of pure EM cells
	float* m_seedTrk_etChrgEM; // E_T of charged EM cells
	float* m_seedTrk_etNeuEM; // E_T of neutral EM cells
	float* m_seedTrk_etResNeuEM; // eflow correction term
	float* m_seedTrk_hadLeakEt; // hadronic leakage in Et summed over cells
	float* m_seedTrk_sumEMCellEtOverLeadTrkPt;
	float* m_seedTrk_secMaxStripEt;
	float* m_seedTrk_stripWidth2;
	int*   m_seedTrk_nStrip;
	float* m_seedTrk_etChrgHad;
	int*   m_seedTrk_nOtherCoreTrk;
	int*   m_seedTrk_nIsolTrk;
	float* m_seedTrk_etIsolEM;
	float* m_seedTrk_etIsolHad;

	float* m_cellBasedEnergyRing1;
	float* m_cellBasedEnergyRing2;
	float* m_cellBasedEnergyRing3;
	float* m_cellBasedEnergyRing4;
	float* m_cellBasedEnergyRing5;
	float* m_cellBasedEnergyRing6;
	float* m_cellBasedEnergyRing7;
    };
    
    
} // namespace D3PD


#endif // not TAUD3PDMAKER_TAUCOMMONDETAILSFILLERTOOL_H
