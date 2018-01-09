/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * Name :	TrigT1CaloBaseFex.h
 * PACKAGE :	Trigger/L1CaloUpgrade/TrigT1CaloEFex
 *
 * AUTHOR :	Denis Oliveira Damazio
 *
 * PURPOSE :	Emulate the eFex system, for phase 1, L1Calo upgrade
 *
 * **/

#ifndef TRIGT1CALOEFEX_TRIGT1CALOBASEFEX
#define TRIGT1CALOEFEX_TRIGT1CALOBASEFEX

#include "AthenaBaseComps/AthAlgorithm.h"
class CaloCellContainer;
class CaloCell;
class TruthParticleContainer;
class TruthParticle;
class TruthParticles;
class TruthVertexContainer;
class TruthVertex;
class TFile;
class TH1F;
class TH2F;
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthVertexContainer.h"

class TrigT1CaloBaseFex : public AthAlgorithm
{
public :

	// Usual athena stuff
	TrigT1CaloBaseFex( const std::string& name, ISvcLocator* pSvcLocator );
	virtual ~TrigT1CaloBaseFex() { }
	StatusCode initialize();
	StatusCode finalize();
	StatusCode execute() = 0;

protected :
	/** method to get the containers */
	StatusCode getContainers(std::vector<const CaloCell*>& scells,
                                 const xAOD::TriggerTowerContainer*& TTs);
	StatusCode getContainers(std::vector<const CaloCell*>& scells,
                                 const xAOD::TriggerTowerContainer*& TTs, float etThresholdGeV);
	StatusCode getContainers(const xAOD::TruthParticleContainer*& truthContainer);
	/** find the cells above a given threshold in an event to be used as seeds to build electron candidates */
	void findCellsAbove(const std::vector<const CaloCell*>&,
                            const float thr,
                            std::vector<const CaloCell*>& out) ;
	/** finds cells around a seed cell. These cells will be part of the cluster */
	void findCellsAround(const std::vector<const CaloCell*>&,
                             const CaloCell* cell,
                             std::vector<const CaloCell*>& out,
                             const float deta,
                             const float dphi) const;
	void findCellsAround(const std::vector<const CaloCell*>&,
                             const float eta,
                             const float phi,
                             std::vector<const CaloCell*>& out,
                             const float deta,
                             const float dphi) const ;
	/** finds TTs around a seed cell. These TTs will be part of the cluster. This helps to cover the part related to TileCall */
	void findTTsAround(const xAOD::TriggerTowerContainer*, const CaloCell* cell, std::vector<const xAOD::TriggerTower*>& out) const ;
	void findTTsAround(const xAOD::TriggerTowerContainer*, const float eta, const float phi, std::vector<const xAOD::TriggerTower*>& out) const ;
	/** checks if a give (seed) cell is the highest in a vector of cells. This is to make sure we have a real local EM maximum */
	bool isCellEmMaximum(const std::vector<const CaloCell*>& scells, const CaloCell* cell) const ;
	/** sum all cells from the vector that are in the EM calorimeter part */
	float sumEmCells(const std::vector<const CaloCell*>& scells) const ;
	/** sum all cells from the vector that are in the EM calorimeter part (only 2nd layer) */
	float sumEmCells2nd(const std::vector<const CaloCell*>& scells) const ;
	/** sum all cells from the vector that are in the HAD calorimeter part */
	float sumHadCells(const std::vector<const CaloCell*>& scells) const ;
	/** sum all TTs from the vector that are in the HAD calorimeter part, but only for |eta|<1.72 (tile region) */
	float sumHadTTs(const std::vector<const xAOD::TriggerTower*>& scells) const ;
	/** detect central cluster position */
	void findCluster(const std::vector<const CaloCell*>& scells, float &etaCluster, float &phiCluster) const;
        /** find cells above E/sigm threshold */
	void NoiseThreshold(const std::vector<const CaloCell*>&,
                            const float significance,
                            std::vector<const CaloCell*>& out);

	/** list of seeds */
	std::vector<const CaloCell*> m_cellsAboveThr;
	/** list of cells around a cell (seed) */
	std::vector<const CaloCell*> m_cellsAround;
	/** list of cells around a cell (seed again) */
	std::vector<const CaloCell*> m_cellsAround2;
	/** list of TTs around a cell (seed) */
	std::vector<const xAOD::TriggerTower*> m_TTsAround;
	/** deta for the cluster definition */
	float m_deta;
	/** dphi for the cluster definition */
	float m_dphi;
	/** deta for the cluster to TT definition */
	float m_detaTT;
	/** dphi for the cluster to TT definition */
	float m_dphiTT;
	/** clear up container from bad BC */
	bool m_useProvenance;

};

#endif
