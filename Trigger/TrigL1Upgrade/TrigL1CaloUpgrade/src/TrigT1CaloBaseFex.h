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
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

class TrigT1CaloBaseFex : public AthAlgorithm
{
public :

	// Usual athena stuff
	TrigT1CaloBaseFex( const std::string& name, ISvcLocator* pSvcLocator );
	virtual ~TrigT1CaloBaseFex() { };
	StatusCode initialize();
	StatusCode finalize();
	StatusCode execute() = 0;

protected :
	/** method to get the containers */
	StatusCode getContainers(CaloCellContainer*& scells, const xAOD::TriggerTowerContainer*& TTs);
	StatusCode getContainers(CaloCellContainer*& scells, const xAOD::TriggerTowerContainer*& TTs, float etThresholdGeV);
	StatusCode getContainers(const xAOD::TruthParticleContainer*& truthContainer);
	/** create all supercells list**/
	void createCellList(const CaloCellContainer*, std::vector<CaloCell*>& out);
	/** find the cells above a given threshold in an event to be used as seeds to build electron candidates */
	void findCellsAbove(const CaloCellContainer*, const float& thr, std::vector<CaloCell*>& out) ;
	/** find the cells at EMB2 and EMEC2 above a given threshold in an event to be used as seeds to build electron/tau candidates */
	void findCellsAbove_EMB2_EMEC2(const CaloCellContainer*, const float& thr, std::vector<CaloCell*>& out) ;
	/** finds cells around a seed cell. These cells will be part of the cluster */
	void findCellsAround(const CaloCellContainer*, const CaloCell* cell, std::vector<CaloCell*>& out, const float deta, const float dphi) const;
	void findCellsAround(const CaloCellContainer*, const float eta, const float phi, std::vector<CaloCell*>& out, const float deta, const float dphi) const ;
	/** finds cells around a seed cell whose layer matches the seed. These cells will be part of the cluster */
	void findCellsAroundLayerMatched(const CaloCellContainer*, const CaloCell* cell, std::vector<CaloCell*>& out, const float deta, const float dphi) const;
	/** finds TTs around a seed cell. These TTs will be part of the cluster. This helps to cover the part related to TileCall */
	void findTTsAround(const xAOD::TriggerTowerContainer*, const CaloCell* cell, std::vector<const xAOD::TriggerTower*>& out) const ;
	void findTTsAround(const xAOD::TriggerTowerContainer*, const float eta, const float phi, std::vector<const xAOD::TriggerTower*>& out) const ;
	/** checks if a give (seed) cell is the highest in a vector of cells. This is to make sure we have a real local EM maximum */
	bool isCellEmMaximum(const std::vector<CaloCell*>& scells, const CaloCell* cell) const ;
	/** sum all cells from the vector that are in the EM calorimeter part */
	float sumEmCells(const std::vector<CaloCell*>& scells) const ;
	/** sum all cells from the vector that are in the EM calorimeter part (only 2nd layer) */
	float sumEmCells2nd(const std::vector<CaloCell*>& scells) const ;
	/** sum all cells from the vector that are in the HAD calorimeter part */
	float sumHadCells(const std::vector<CaloCell*>& scells) const ;
	/** sum all TTs from the vector that are in the HAD calorimeter part, but only for |eta|<1.72 (tile region) */
	float sumHadTTs(const std::vector<const xAOD::TriggerTower*>& scells) const ;
	/** detect central cluster position */
	void findCluster(const std::vector<CaloCell*>& scells, float &etaCluster, float &phiCluster) const;
	void findClusterFor(const std::vector<CaloCell*>& scells, float &etaCluster, float &phiCluster, float &radiusCluster) const;
        /** find cells above E/sigm threshold */
	void NoiseThreshold(const CaloCellContainer*, const float& significance, std::vector<CaloCell*>& out);
	/** clustering option : eFEX TDR proposal **/
	float eFEXTDRclus(const std::vector<float>& EM0allCell, const std::vector<float>& EM1allCell, const std::vector<float>& EM2allCell, const std::vector<float>& EM3allCell, const std::vector<float>& HADallCell, const float& seedPlace) const ;
	/** isolation option**/
	float eFEXIsoTDR (const std::vector<float>& EM2allCell, const float& seedPlace) const ;
	/** boolian for TDR isolation**/
	bool IseFEXIsoTDR (const std::vector<float>& EM2allCell, const float& seedPlace, const float& Isovalue) const ;
	/** retrieve the cell ID helper */
	StatusCode getIDHelper(const CaloCell_SuperCell_ID*& m_idHelper);
	/** calculate the ET of an input cell */
	float CaloCellET(CaloCell* const &inputCell, float digitScale, float digitThreshold);
	/** SC from container is returned according to its ID */
	CaloCell* returnCellFromCont(Identifier inputID, const CaloCellContainer* &cellContainer);
	/** returns the SC left/right to the input cell for the barrel */
	CaloCell* NextEtaCell_Barrel(CaloCell* inputCell, bool upwards, const CaloCellContainer* &cellContainer, const  CaloCell_SuperCell_ID* &idHelper);
	/** returns the SC left/right to the input cell for the OW */
	CaloCell* NextEtaCell_OW(CaloCell* inputCell, bool upwards, const CaloCellContainer* &cellContainer, const  CaloCell_SuperCell_ID* &idHelper);
	/** returns the SC left/right to the input cell for the IW */
	CaloCell* NextEtaCell_IW(CaloCell* inputCell, bool upwards, const CaloCellContainer* &cellContainer, const  CaloCell_SuperCell_ID* &idHelper);
	/** helper function calling NextEtaCell_Barrel(), NextEtaCell_OW(), NextEtaCell_IW() according to position of input cell */
	CaloCell* NextEtaCell(CaloCell* inputCell, bool upwards, const CaloCellContainer* &cellContainer, const  CaloCell_SuperCell_ID* &idHelper);
	/** manager function for the phi index */
	int restrictPhiIndex(int input_index, bool is64);
	/** returns the SC above/below the input cell */
	CaloCell* NextPhiCell(CaloCell* inputCell, bool upwards, const CaloCellContainer* &cellContainer, const  CaloCell_SuperCell_ID* &idHelper);
	/** helper function calling localMax() */
	bool localMax(const CaloCellContainer* &inputContainer, CaloCell* inputCell, const CaloCell_SuperCell_ID* &idHelper, float digitScale, float digitThreshold);
	/** tests if the input cell has a local energy maximum with respect to neighbors */
	bool localMax(const CaloCellContainer* &inputContainer, CaloCell* inputCell, int numOthers, const CaloCell_SuperCell_ID* &idHelper, float digitScale, float digitThreshold);
	/** adds SC to vector if the SC is not part of this vector yet */
	void addOnce(CaloCell* inputCell, std::vector<CaloCell*> &outputVector);
	/** form the cluster from cells of the second layer L2 */
	std::vector<CaloCell*> L2cluster(CaloCell* centreCell, int etaWidth, int phiWidth, const CaloCellContainer* scells, const CaloCell_SuperCell_ID* idHelper, float digitScale , float digitThresh );
	/** match SCs from the cluster in L2 to L1 */
	void fromLayer2toLayer1(const CaloCellContainer* &inputContainer, CaloCell* inputCell, std::vector<CaloCell*> &outputVector, const CaloCell_SuperCell_ID* &idHelper);
	/** match SCs from the cluster in L2 to one cell of PS */
	CaloCell* fromLayer2toPS(const CaloCellContainer* &inputContainer, CaloCell* inputCell, const CaloCell_SuperCell_ID* &idHelper);
	/** match SCs from the cluster in L2 to one cell of L3 */
	CaloCell* fromLayer2toLayer3(const CaloCellContainer* &inputContainer, CaloCell* inputCell, const CaloCell_SuperCell_ID* &idHelper);
	/** form the cluster around the central SC */
	std::vector<CaloCell*> TDR_Clus(CaloCell* centreCell, int etaWidth, int phiWidth, const CaloCellContainer* scells, const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh);
	/** calculate cluster energy from all SCs in PS, L1, L2, L3 */
	double sumVectorET (const std::vector<CaloCell*> &inputVector, float digitScale = 0.,float digitThreshold = 0.);
	/** check if conversion from ET to energy after digitization was performed successfully */
	bool checkDig(float EM_ET, float digitScale,float digitThresh);
	/** calculate cluster energy */
	double EMClusET(CaloCell* centreCell, int etaWidth, int phiWidth, const CaloCellContainer* scells, const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh);
	/** check if both input cells belong to the same TT */
	bool SameTT(CaloCell* inputCell1, CaloCell* inputCell2, const CaloCell_SuperCell_ID* &idHelper);
	/** calculate cluster energy of cells in L2 around the central cell in a given eta/phi width */
	double L2clusET(CaloCell* centreCell, int etaWidth, int phiWidth, const CaloCellContainer* scells, const CaloCell_SuperCell_ID* idHelper, float digitScale , float digitThresh);
	/** calculate the energy isolation of the central cell along eta */
	double REta(CaloCell* centreCell, int etaWidth1, int phiWidth1, int etaWidth2, int phiWidth2, const CaloCellContainer* scells, const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh);
	/** convert the TT phi to match the definition of SC phi  */
	double TT_phi(const xAOD::TriggerTower* &inputTower);
	/** calculate deltaR between two points in eta/phi space */
	double dR(double eta1, double phi1, double eta2, double phi2);
	/** Match each SC from L2 to one corresponding TT */
	const xAOD::TriggerTower* matchingHCAL_TT(CaloCell* &inputCell, const xAOD::TriggerTowerContainer* &TTContainer);
	/** Match each SC from L2 to one corresponding HCAL SC */
	CaloCell* matchingHCAL_LAr(CaloCell* &inputCell, const CaloCellContainer* &SCContainer, const  CaloCell_SuperCell_ID* &idHelper);
	/** calculate the energy of an input TT */
	double TT_ET(const xAOD::TriggerTower* &inputTower);
	/** calculate the energy in the HCAL (LAr + Tile) for SC/TT that match the EM cluster cells of L2 */
	double HadronicET(std::vector<CaloCell*> inputVector, const CaloCellContainer* scells, const xAOD::TriggerTowerContainer* &TTContainer, const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh);
	/** calculate the hadronic isolation of the central cell */
	double RHad(CaloCell* centreCell, int etaWidth, int phiWidth, const CaloCellContainer* scells, const xAOD::TriggerTowerContainer* &TTContainer, const CaloCell_SuperCell_ID* idHelper, float digitScale , float digitThresh, float &HadronicET);
	/** calculate the lateral isolation aorund the central cell */
	double L1Width(CaloCell* centreCell, int etaWidth, int phiWidth, const CaloCellContainer* scells, const CaloCell_SuperCell_ID* idHelper, float digitScale , float digitThresh );
	/** find cluster and associated variables using a "loose" algorithm */
	std::vector<std::vector<float>> looseAlg(const CaloCellContainer* SCs, const xAOD::TriggerTowerContainer* TTs, const CaloCell_SuperCell_ID* idHelper);
	/** find cluster and associated variables using a user defined selection */
	std::vector<std::vector<float>> baselineAlg(const CaloCellContainer* scells, const xAOD::TriggerTowerContainer* TTs, const CaloCell_SuperCell_ID* idHelper);
	/** list of all supercells */
	std::vector<CaloCell*> allSuperCells;
	/** list of seeds */
	std::vector<CaloCell*> m_cellsAboveThr;
	/** list of cells around a cell (seed) */
	std::vector<CaloCell*> m_cellsAround;
	/** list of cells around a cell (seed again) */
	std::vector<CaloCell*> m_cellsAround2;
	/** list of cells around a cell (seed) in the window*/
	std::vector<CaloCell*> m_cellsInWindow;
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
	/** clear up container from bad BC by making a new container (Denis, old way) */
	bool m_useProvenanceSkim; 
	/**  clear up container from bad BC by skipping scells*/
	bool m_useProvenance;
	/** Configurable quality bitmask */
	int m_qualBitMask;
	/** name of the input SCell container */
	std::string m_inputCellsName;
	/** value of nominal digitisation */
	float m_nominalDigitization;
	/** noise threshold */
	float m_nominalNoise_thresh;
	/** phi width of the TDR cluster formation given in number of SCs (including the central cell), should be 2 or 3 */
	int m_phiWidth_TDRCluster;
	/** eta width of the TDR cluster formation given in number of SCs (including the central cell), should be >= 1 */
	int m_etaWidth_TDRCluster;
	// eta width for REta isolation given in number of SCs (numerator of fraction)
	int m_etaWidth_REtaIsolation_num;
	// phi width for REta isolation given in number of SCs (numerator of fraction)
	int m_phiWidth_REtaIsolation_num;
	// eta width for REta isolation given in number of SCs (denominator of fraction)
	int m_etaWidth_REtaIsolation_den;
	// phi width for REta isolation given in number of SCs (denominator of fraction)
	int m_phiWidth_REtaIsolation_den;
	// eta width for RHad isolation given in number of SCs
	int m_etaWidth_RHadIsolation;
	// phi width for RHad isolation given in number of SCs
	int m_phiWidth_RHadIsolation;
	// eta width for wstot isolation given in number of SCs
	int m_etaWidth_wstotIsolation;
	// phi width for wstot isolation given in number of SCs
	int m_phiWidth_wstotIsolation;
	/** threshold for minimum cluster energy for the loose eFEX algorithm */
	float m_clustET_looseAlg_thresh;
	/** threshold for minimum cluster energy for the baseline eFEX algorithm */
	float m_clustET_thresh;
	/** boolean for applying the baseline cuts */
	bool m_apply_BaseLineCuts;
	/** threshold for applying cluster isolation cuts */
  	float m_clustET_NoIso_thresh;
	/** threshold for isolation REta*/
  	float m_REta_thresh;
	/** threshold for isolation RHad */
  	float m_RHad_thresh;
	/** threshold for isolation L1Width (wstot) */
  	float m_L1Width_thresh;
	/** max eta for applying cut on L1Width */
  	float m_eta_dropL1Width;
};

#endif
