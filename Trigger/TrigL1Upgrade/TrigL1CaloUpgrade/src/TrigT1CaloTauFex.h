/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * Name :	TrigT1CaloTauFex.h
 * PACKAGE :	Trigger/L1CaloUpgrade/TrigT1CaloEFex
 *
 * AUTHOR :	Denis Oliveira Damazio
 *
 * PURPOSE :	Emulate the Tau eFex system, for phase 1, L1Calo upgrade
 *
 * **/

#ifndef TRIGT1CALOEFEX_TRIGT1CALOTAUFEX
#define TRIGT1CALOEFEX_TRIGT1CALOTAUFEX

#include "../src/TrigT1CaloBaseFex.h"

class TrigT1CaloTauFex : public TrigT1CaloBaseFex
{
public :

	// Usual athena stuff
	TrigT1CaloTauFex( const std::string& name, ISvcLocator* pSvcLocator );
	virtual ~TrigT1CaloTauFex();
	StatusCode initialize();
	StatusCode finalize();
	StatusCode execute();

private :

        /** list of prongs */
        std::vector<int> m_true_nprong;
        /** list of true pt */
        std::vector<float> m_true_pt;
        /** list of true neutrals */
        std::vector<bool> m_true_neutrals;
        /** list of visible true taus */
        std::vector<TLorentzVector> m_true_vistau;
        /** list of Isolation cells */
        std::vector<const CaloCell*> m_IsoCells;
        /** list of Isolation cells */
        std::vector<const CaloCell*> m_CoreCells;
        /** list of RoI */
        std::vector<const CaloCell*> m_RoICells;
        /** list of Core  energy */
        std::vector<float> m_CoreEnergy;
        /** list of Iso energy */
        std::vector<float> m_IsoEnergy;
        /** list of RoI above energy */
        std::vector<float> m_RoICellsAbove;
        /** list of vector RoI above energy */
        std::vector<std::vector<float>> m_RoI;

        void ExamineTruthTau(const xAOD::TruthParticle& xTruthParticle) const;
        void Truth(const xAOD::TruthParticleContainer* truthContainer);
        void IsolationCorett(const std::vector<const CaloCell*>& scells,
                             const CaloCell* cell,
                             std::vector<const CaloCell*>& out1,
                             std::vector<const CaloCell*>& out2);
        void IsolationCore(const std::vector<const CaloCell*>& scells,
                           const CaloCell* cell,
                           std::vector<const CaloCell*>& out1,
                           std::vector<const CaloCell*>& out2,
                           std::vector<std::vector<float>>& out);
        void sumEMCoreCells(const std::vector<const CaloCell*>& cell, std::vector<const CaloCell*>& out1, std::vector<float>& out2);
        void sumEMIsoCells(const std::vector<const CaloCell*>& cell, std::vector<float>& out);

	// sorting utils
        template <typename T, typename Compare> std::vector<std::size_t> sort_permutation( const std::vector<T>& vec, Compare compare);
	template <typename T> std::vector<T> apply_permutation(const std::vector<T>& vec, const std::vector<std::size_t>& p);

	// configuration flags
	bool m_enableMon;
	bool m_doTruth;
	float m_cellsEtThrehold;
	int m_NoiseSignificanceCore;
	int m_NoiseSignificanceIso;
	float m_truth_minPt_thr;
	float m_truth_maxEta_thr;

	/** TFile for monitoring */
	TFile * m_histFile;
	/** Histograms with monitoring activity */
	TH1F* m_EtSTau;
	// TODO: add more ...
	
	/** output name */
	std::string m_outputClusterName;
};

#endif
