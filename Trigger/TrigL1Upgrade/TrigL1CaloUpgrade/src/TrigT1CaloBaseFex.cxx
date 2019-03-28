/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * NAME : 	TrigT1CaloBaseFex.cxx
 * PACKAGE : 	Trigger/L1CaloUpgrade/TrigT1CaloEFex
 *
 * AUTHOR : 	Denis Oliveira Damazio
 * UPDATED :	Philipp Ott
 *
 *
 * **/

#include "TrigT1CaloBaseFex.h"
#include "CaloEvent/CaloCellContainer.h"
#include <math.h>
#include <string>
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

TrigT1CaloBaseFex::TrigT1CaloBaseFex( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm (name, pSvcLocator), m_deta(0.08), m_dphi(0.11), m_detaTT(0.125), m_dphiTT(0.15), m_inputCellsName("SCell")  {
	declareProperty("CleanCellContainerSkim", m_useProvenanceSkim=false);
	declareProperty("CleanCellContainer", m_useProvenance=true);
	declareProperty("QualBitMask", m_qualBitMask=0x40);
	declareProperty("SuperCellContainer", m_inputCellsName);
	declareProperty("EtaClusterFormation", m_deta);
	declareProperty("PhiClusterFormation", m_dphi);
	declareProperty("NominalDigitizationValue", m_nominalDigitization = 25.);
	declareProperty("NominalNoiseThreshold", m_nominalNoise_thresh = 100.);
	declareProperty("PhiWidthTDRCluster", m_phiWidth_TDRCluster = 2);
	declareProperty("EtaWidthTDRCluster", m_etaWidth_TDRCluster = 3);
	declareProperty("ClusterEnergyThresholdLooseEFEX", m_clustET_looseAlg_thresh = 10.);
	declareProperty("ClusterEnergyThreshold", m_clustET_thresh = 28.);
	declareProperty("ApplyBaseLineSelection", m_apply_BaseLineCuts = true);
	declareProperty("EtaWidthREtaIsolationNumerator", m_etaWidth_REtaIsolation_num = 3);
	declareProperty("PhiWidthREtaIsolationNumerator", m_phiWidth_REtaIsolation_num = 2);
	declareProperty("EtaWidthREtaIsolationDenominator", m_etaWidth_REtaIsolation_den = 7);
	declareProperty("PhiWidthREtaIsolationDenominator", m_phiWidth_REtaIsolation_den = 3);
	declareProperty("EtaWidthRHadIsolation", m_etaWidth_RHadIsolation = 3);
	declareProperty("PhiWidthRHadIsolation", m_phiWidth_RHadIsolation = 3);
	declareProperty("EtaWidthWStotIsolation", m_etaWidth_wstotIsolation = 5);
	declareProperty("PhiWidthWStotIsolation", m_phiWidth_wstotIsolation = 3);
  	declareProperty("EnergyThresholdToApplyIsolation", m_clustET_NoIso_thresh = 60.);
  	declareProperty("REtaThreshold", m_REta_thresh = 0.12);
  	declareProperty("RHadThreshold", m_RHad_thresh = 0.16);
  	declareProperty("L1WidthThreshold", m_L1Width_thresh = 0.02);
  	declareProperty("EtaThresholdToApplyL1Width", m_eta_dropL1Width = 2.3);
}

StatusCode TrigT1CaloBaseFex::initialize(){
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "initializing TrigT1CaloBaseFex" << endreq;
	m_cellsAboveThr.reserve(200);
	m_cellsAround.reserve(200);
	return StatusCode::SUCCESS;
}

StatusCode TrigT1CaloBaseFex::finalize(){
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "finalizing TrigT1CaloBaseFex" << endreq;
	m_cellsAboveThr.clear();
	m_cellsAround.clear();
	return StatusCode::SUCCESS;
}

StatusCode TrigT1CaloBaseFex::getContainers(CaloCellContainer*& scells, const xAOD::TriggerTowerContainer*& TTs){
	
	MsgStream msg(msgSvc(), name());
        const CaloCellContainer* scells_from_sg;
	if ( evtStore()->retrieve(scells_from_sg,m_inputCellsName).isFailure() ){
		msg << MSG::WARNING << "did not find cell container" << endreq;
		return StatusCode::FAILURE;
	}
	if ( evtStore()->retrieve(TTs,"xAODTriggerTowers").isFailure() ){
		msg << MSG::WARNING << "did not find TT container" << endreq;
		return StatusCode::FAILURE;
	}

	if ( m_useProvenanceSkim ) {
		if ( !scells ) scells = new CaloCellContainer(SG::VIEW_ELEMENTS);
		scells->reserve( scells_from_sg->size() ); // max possible size
        	for(auto scell : *scells_from_sg) {
			if ( scell->provenance() & m_qualBitMask ) scells->push_back( scell );
		}
	}
	else {
		if ( !scells ) scells = new CaloCellContainer(SG::VIEW_ELEMENTS);
		scells->reserve( scells_from_sg->size() ); // max possible size
        	for(auto scell : *scells_from_sg) {
			scells->push_back( scell );
		}
	}
#ifndef NDEBUG
        for(auto scell : *scells) {
		if ( scell->et() < 3e3 ) continue;
		msg << MSG::DEBUG << "scell : " << scell->et() << " " << scell->eta() << " " << scell->phi() << endreq;
	}
        for(auto TT : *TTs) {
		if ( TT->pt() < 3 ) continue;
		msg << MSG::DEBUG << "TT : " << TT->pt() << " " << TT->eta() << " " << TT->phi() << endreq;
	}
#endif
	return StatusCode::SUCCESS;
}

StatusCode TrigT1CaloBaseFex::getContainers(CaloCellContainer*& scells, const xAOD::TriggerTowerContainer*& TTs, float etThresholdGeV){

        MsgStream msg(msgSvc(), name());
        const CaloCellContainer* scells_from_sg;
        if ( evtStore()->retrieve(scells_from_sg,m_inputCellsName).isFailure() ){
                msg << MSG::WARNING << "did not find cell container" << endreq;
                return StatusCode::SUCCESS;
        }
        if ( evtStore()->retrieve(TTs,"xAODTriggerTowers").isFailure() ){
                msg << MSG::WARNING << "did not find TT container" << endreq;
                return StatusCode::SUCCESS;
        }

        if ( m_useProvenanceSkim ) {
                if ( !scells ) scells = new CaloCellContainer(SG::VIEW_ELEMENTS);
		scells->reserve( scells_from_sg->size() ); // max possible size
                for(auto scell : *scells_from_sg) {
                        if ( scell->provenance() & m_qualBitMask ) scells->push_back( scell );
                }
        }
        else {
                if ( !scells ) scells = new CaloCellContainer(SG::VIEW_ELEMENTS);
		scells->reserve( scells_from_sg->size() ); // max possible size
                for(auto scell : *scells_from_sg) {
                        scells->push_back( scell );
                }
        }
//#ifndef NDEBUG
        for(auto scell : *scells) {
		if ( scell->et() < etThresholdGeV*1e3 ) continue;
                ATH_MSG_DEBUG( "scell : " << scell->et() << " " << scell->eta() << " " << scell->phi());
        }
        for(auto TT : *TTs) {
                if ( TT->pt() < etThresholdGeV ) continue;
                ATH_MSG_DEBUG( "TT : " << TT->pt() << " " << TT->eta() << " " << TT->phi());
        }
//#endif
        return StatusCode::SUCCESS;
}

StatusCode TrigT1CaloBaseFex::getContainers(const xAOD::TruthParticleContainer*& truthContainer){

	MsgStream msg(msgSvc(), name());
        if ( evtStore()->retrieve(truthContainer,"TruthParticles").isFailure() ){
                msg << MSG::WARNING << "did not find Truth container" << endreq;
                return StatusCode::SUCCESS;
        }
 
        return StatusCode::SUCCESS;
}

void TrigT1CaloBaseFex::createCellList(const CaloCellContainer* scells, std::vector<CaloCell*>& out) {
	out.clear();
	for(auto scell : *scells) {
		out.push_back(scell);
	}
	return;
}

void TrigT1CaloBaseFex::findCellsAbove(const CaloCellContainer* scells, const float& Thr, std::vector<CaloCell*>& out) {
	out.clear();
	for(auto scell : *scells) {
		if ( scell->et() < Thr )  continue;
		out.push_back(scell);
	}
	return;
}

void TrigT1CaloBaseFex::findCellsAbove_EMB2_EMEC2(const CaloCellContainer* scells, const float& Thr, std::vector<CaloCell*>& out) {
	out.clear();
	for(auto scell : *scells) {
		if ( scell->et() < Thr )  continue;
		if ( scell->caloDDE()->getSampling()==2 || scell->caloDDE()->getSampling()==6 ) out.push_back(scell);
	}
	return;
}

void TrigT1CaloBaseFex::findCellsAround(const CaloCellContainer* scells, const CaloCell* cell, std::vector<CaloCell*>& out, const float detaSize, const float dphiSize) const {
	out.clear();
	if ( !cell ) return;
	float etacell = cell->eta();
	float phicell = cell->phi();
	for(auto scell : *scells) {
		if ( fabsf( scell->eta() - etacell) > detaSize ) continue;
		float dphi = fabsf( scell->phi() - phicell);
		dphi = fabsf( M_PI - dphi );
		dphi = fabsf( M_PI - dphi );
		if ( fabsf( dphi ) > dphiSize ) continue;
		out.push_back(scell);
	}
	return;
}

void TrigT1CaloBaseFex::findCellsAround(const CaloCellContainer* scells, const float etacell, const float phicell, std::vector<CaloCell*>& out, const float detaSize, const float dphiSize) const {
        out.clear();
        for(auto scell : *scells) {
                if ( fabsf( scell->eta() - etacell) > detaSize ) continue;
                float dphi = fabsf( scell->phi() - phicell);
                dphi = fabsf( M_PI - dphi );
                dphi = fabsf( M_PI - dphi );
                if ( fabsf( dphi ) > dphiSize ) continue;
                out.push_back(scell);
        }
        return;
}

void TrigT1CaloBaseFex::findCellsAroundLayerMatched(const CaloCellContainer* scells, const CaloCell* cell, std::vector<CaloCell*>& out, const float detaSize, const float dphiSize) const {
	out.clear();
	if ( !cell ) return;
	float etacell = cell->eta();
	float phicell = cell->phi();
	for(auto scell : *scells) {
		if (scell->caloDDE()->getSampling() !=  cell->caloDDE()->getSampling() ) continue;
		if ( fabsf( scell->eta() - etacell) > detaSize ) continue;
		float dphi = fabsf( scell->phi() - phicell);
		dphi = fabsf( M_PI - dphi );
		dphi = fabsf( M_PI - dphi );
		if ( fabsf( dphi ) > dphiSize ) continue;
		out.push_back(scell);
	}
	return;
}

void TrigT1CaloBaseFex::findTTsAround(const xAOD::TriggerTowerContainer* scells, const CaloCell* cell, std::vector<const xAOD::TriggerTower*>& out) const {
	out.clear();
	if ( !cell ) return;
	float etacell = cell->eta();
	float phicell = cell->phi();
	for(auto scell : *scells) {
		if ( fabsf( scell->eta() - etacell) > m_detaTT ) continue;
		float dphi = fabsf( scell->phi() - phicell);
		dphi = fabsf( M_PI - dphi );
		dphi = fabsf( M_PI - dphi );
		if ( fabsf( dphi ) > m_dphiTT ) continue;
		out.push_back(scell);
	}
	return;
}

void TrigT1CaloBaseFex::findTTsAround(const xAOD::TriggerTowerContainer* scells, const float etacell, const float phicell, std::vector<const xAOD::TriggerTower*>& out) const {
        out.clear();
        for(auto scell : *scells) {
                if ( fabsf( scell->eta() - etacell) > m_detaTT ) continue;
                float dphi = fabsf( scell->phi() - phicell);
                dphi = fabsf( M_PI - dphi );
                dphi = fabsf( M_PI - dphi );
                if ( fabsf( dphi ) > m_dphiTT ) continue;
                out.push_back(scell);
        }
        return;
}

bool TrigT1CaloBaseFex::isCellEmMaximum(const std::vector<CaloCell*>& scells, const CaloCell* cell) const {
        if ( !cell ) return false;
	int samp = cell->caloDDE()->getSampling();
	if ( (samp >= 8) && (samp!=21) ) return false; // include FCAL0 EM
	float cellpt = 1.0001*cell->et(); //make sure you don't get thecell itself
	for(auto scell : scells){ 
		int samp1 = scell->caloDDE()->getSampling();
		if ( ( samp1 >= 8 ) && (samp1!=21) ) continue;
		if ( scell->ID() == cell->ID()  ) continue;
		if ( scell->et() > cellpt ) return false;
	}
	return true;
}

float TrigT1CaloBaseFex::sumEmCells(const std::vector<CaloCell*>& scells) const {
	float totalSum = 0.0;
	for(auto scell : scells) {
		int samp1 = scell->caloDDE()->getSampling();
		if ( (samp1<8) || (samp1==21) ) totalSum+= scell->energy();
	}
	return totalSum;
}

float TrigT1CaloBaseFex::sumEmCells2nd(const std::vector<CaloCell*>& scells) const {
	float totalSum = 0.0;
	for(auto scell : scells) {
		if ( (scell->caloDDE()->getSampling()==2) ||(scell->caloDDE()->getSampling()==6) ) {
			totalSum+= scell->energy();
		}
	}
	return totalSum;
}

float TrigT1CaloBaseFex::sumHadCells(const std::vector<CaloCell*>& scells) const {
	float totalSum = 0.0;
	for(auto scell : scells){
             if ( (scell->caloDDE()->getSampling() <8) || ( scell->caloDDE()->getSampling()>=22) ) continue;
             //totalSum+= (scell->et())*TMath::CosH(scell->eta());
             totalSum+= (scell->energy());
	}
	return totalSum;
}

float TrigT1CaloBaseFex::sumHadTTs(const std::vector<const xAOD::TriggerTower*>& scells) const {
	float totalSum = 0.0;
	for(auto scell : scells){
             if ( fabsf( scell->eta() ) > 1.56 ) continue;
	     if ( scell->sampling() == 0 ) continue;
             totalSum+= (scell->pt())*TMath::CosH(scell->eta());
	}
	return totalSum * 1e3; // express in MeV
}

void TrigT1CaloBaseFex::findCluster(const std::vector<CaloCell*>& scells, float &etaCluster, float &phiCluster) const {
	etaCluster=0.0;
	phiCluster=0.0;
	double etaClusterD=0.0;
	double phiClusterD=0.0;

	double energyCluster=0.0;
	bool cross_phi_bound=false;
        int last_sign=0;
	for(auto scell : scells){
	  if ( fabsf( scell->phi() ) < 2.7 ) continue;
	  int layer = scell->caloDDE()->getSampling();
          if ( ( layer != 2 ) && ( layer != 6 ) ) continue;
	  int cell_sign = ( scell->phi() >=0 ? 1 : -1 );
          if ( ( last_sign!=0 ) && ( last_sign != cell_sign ) ) cross_phi_bound = true;
	  last_sign = cell_sign;
	}

	for(auto scell : scells){
	     int layer = scell->caloDDE()->getSampling();
             if ( ( layer != 2 ) && ( layer != 6 ) ) continue;
	     double scelleta = scell->eta();
	     double scellphi = scell->phi();
	     double scellet = scell->et();
             etaClusterD+= (scellet * scelleta);
	     if (cross_phi_bound && scellphi < 0 ) scellphi += 2 * M_PI;
             phiClusterD+= (scellet * scellphi);
             energyCluster+= (scellet) ;
	}
	if ( energyCluster > 0.1 ) {
		etaClusterD/=energyCluster;
		phiClusterD/=energyCluster;
		etaCluster = (float)etaClusterD;
		phiCluster = (float)phiClusterD;
		if ( phiCluster > M_PI ) phiCluster-=2*M_PI;
	} else {
		etaCluster=-999.0;
		phiCluster=-999.0;
	}
}

void TrigT1CaloBaseFex::findClusterFor(const std::vector<CaloCell*>& scells, float &etaCluster, float &phiCluster, float &zCluster) const {
        etaCluster=0.0;
        phiCluster=0.0;
	zCluster=0.0;
        double etaClusterD=0.0;
        double phiClusterD=0.0;
	double zClusterD=0.0;

        double energyCluster=0.0;
        bool cross_phi_bound=false;
        int last_sign=0;
        for(auto scell : scells){
          if ( fabsf( scell->phi() ) < 2.7 ) continue;
          int layer = scell->caloDDE()->getSampling();
          if ( ( layer != 21 ) && ( layer != 6 ) ) continue;
          int cell_sign = ( scell->phi() >=0 ? 1 : -1 );
          if ( ( last_sign!=0 ) && ( last_sign != cell_sign ) ) cross_phi_bound = true;
          last_sign = cell_sign;
        }

        for(auto scell : scells){
             int layer = scell->caloDDE()->getSampling();
             if ( ( layer != 21 ) && ( layer != 6 ) ) continue;
             double scelleta = scell->eta();
             double scellz = scell->z();
             double scellphi = scell->phi();
             double scellet = scell->et();
             etaClusterD+= (scellet * scelleta);
             zClusterD+= (scellet * scellz);
             if (cross_phi_bound && scellphi < 0 ) scellphi += 2 * M_PI;
             phiClusterD+= (scellet * scellphi);
             energyCluster+= (scellet) ;
        }
        if ( energyCluster > 0.1 ) {
                etaClusterD/=energyCluster;
                phiClusterD/=energyCluster;
                zClusterD/=energyCluster;
                etaCluster = (float)etaClusterD;
                phiCluster = (float)phiClusterD;
                zCluster = (float)zClusterD;
                if ( phiCluster > M_PI ) phiCluster-=2*M_PI;
        } else {
                etaCluster=-999.0;
                phiCluster=-999.0;
        }
}

void TrigT1CaloBaseFex::NoiseThreshold(const CaloCellContainer* scells, const float& significance, std::vector<CaloCell*>& out) {
	out.clear();
	for(auto scell : *scells) {
	  float thr(0.);
	  //energy = scell->et();
	  float energy = scell->e();
	  ///if(layer<1 || layer >4 ) continue;
	  if(scell->caloDDE()->getSampling()==1 || scell->caloDDE()->getSampling()==5) {energy-=-0.004593; thr = 0.6819;}  // to be tuned
	  if(scell->caloDDE()->getSampling()==2 || scell->caloDDE()->getSampling()==6) {energy-=0.02849; thr = 0.3054;}  // 0.066, 0.293
	  if(scell->caloDDE()->getSampling()==3 || scell->caloDDE()->getSampling()==7) {energy-=0.004317; thr = 0.483;}  // 0.094, 0.492
	  if(scell->caloDDE()->getSampling()==4 || scell->caloDDE()->getSampling()==8) {energy-=0.0159; thr = 0.3057;}  // 0.050, 0.327
	  thr *= significance;
	  if(fabs(energy)<thr) continue;
	  out.push_back(scell);
	}
	return;
}

float TrigT1CaloBaseFex::eFEXTDRclus (const std::vector<float>& EM0allCell, const std::vector<float>& EM1allCell, const std::vector<float>& EM2allCell, const std::vector<float>& EM3allCell, const std::vector<float>& HADallCell, const float& seedPlace) const {
	int EM2seedindex;
	if (seedPlace==0.) EM2seedindex=16;
	if (seedPlace==0.0125) EM2seedindex=17;
	if (seedPlace==0.0375) EM2seedindex=18;
	if (seedPlace==0.05) EM2seedindex=19;

	int SumUpOrDown = -1;//0:up,1:down
	int EM2TTup = 0;
	int EM2TTdown = 0;
	for (int i=0;i<4;i++) {
		EM2TTup += EM2allCell[i+28];
		EM2TTdown += EM2allCell[i+4];
	}
	if (EM2TTup >= EM2TTdown) SumUpOrDown = 0;//up
	if (EM2TTdown > EM2TTup)  SumUpOrDown = 1;//down

	int SumRightOrLeft = -1;//0:left,1:right
	if (EM2seedindex == 16 or EM2seedindex == 17) SumRightOrLeft = 0;//left
	if (EM2seedindex == 18 or EM2seedindex == 19) SumRightOrLeft = 1;//right

	float EM0Sum = 0;
	if (SumUpOrDown == 1) EM0Sum = EM0allCell[1] + EM0allCell[4];//down
	if (SumUpOrDown == 0) EM0Sum = EM0allCell[4] + EM0allCell[7];//#up

	float EM1Sum = 0;
	for (int i=0; i<3; i++) {
		if (SumUpOrDown == 1) { //up
			EM1Sum += EM1allCell[EM2seedindex-13+i];
			EM1Sum += EM1allCell[EM2seedindex-1+i];
		}
		if (SumUpOrDown == 0) { //down
			EM1Sum += EM1allCell[EM2seedindex-1+i];
			EM1Sum += EM1allCell[EM2seedindex+11+i];
		}
	}

	float EM2Sum = 0;
	for (int i=0; i<5; i++) {
		if (SumUpOrDown == 1) { //up
			EM2Sum += EM2allCell[EM2seedindex-14+i];
			EM2Sum += EM2allCell[EM2seedindex-2+i];
		}
		if (SumUpOrDown == 0) { //down
			EM2Sum += EM2allCell[EM2seedindex-2+i];
			EM2Sum += EM2allCell[EM2seedindex+10+i];
		}

	}

	float EM3Sum = 0.;
	float HADSum = 0.;
	if (SumUpOrDown == 1 and SumRightOrLeft == 0) {//down,left
		EM3Sum=EM3allCell[0] + EM3allCell[1] + EM3allCell[3] + EM3allCell[4];
		HADSum=HADallCell[0] + HADallCell[1] + HADallCell[3] + HADallCell[4];
	}
	if (SumUpOrDown == 1 and SumRightOrLeft == 1) {//down,right
		EM3Sum=EM3allCell[1] + EM3allCell[2] + EM3allCell[4] + EM3allCell[5];
		HADSum=HADallCell[1] + HADallCell[2] + HADallCell[4] + HADallCell[5];
	}
	if (SumUpOrDown == 0 and SumRightOrLeft == 0) {//up,left
		EM3Sum=EM3allCell[3] + EM3allCell[4] + EM3allCell[6] + EM3allCell[7];
		HADSum=HADallCell[3] + HADallCell[4] + HADallCell[6] + HADallCell[7];
	}
	if (SumUpOrDown == 0 and SumRightOrLeft == 1) {//up,right
		EM3Sum=EM3allCell[4] + EM3allCell[5] + EM3allCell[7] + EM3allCell[8];
		HADSum=HADallCell[4] + HADallCell[5] + HADallCell[7] + HADallCell[8];
	}
	float mySum = EM0Sum + EM1Sum + EM2Sum + EM3Sum + HADSum;
	return mySum/1000.;//[GeV]
}

float TrigT1CaloBaseFex::eFEXIsoTDR (const std::vector<float>& EM2allCell, const float& seedPlace) const {
	int EM2seedindex;
	if (seedPlace==0.) EM2seedindex=16;
	if (seedPlace==0.0125) EM2seedindex=17;
	if (seedPlace==0.0375) EM2seedindex=18;
	if (seedPlace==0.05) EM2seedindex=19;

	int SumUpOrDown = -1;//0:up,1:down
	int EM2TTup = 0;
	int EM2TTdown = 0;
	for (int i=0;i<4;i++) {
		EM2TTup += EM2allCell[i+28];
		EM2TTdown += EM2allCell[i+4];
	}
	if (EM2TTup >= EM2TTdown) SumUpOrDown = 0;//up
	if (EM2TTdown > EM2TTup)  SumUpOrDown = 1;//down

	float EM2SmallSum = 0;
	if (SumUpOrDown == 0) {//up
		for (int i=0;i<3;i++) {
			EM2SmallSum += EM2allCell[EM2seedindex-1+i];
			EM2SmallSum += EM2allCell[EM2seedindex-13+i];
		}
	}
	if (SumUpOrDown == 1) {//down
		for (int i=0;i<3;i++) {
			EM2SmallSum += EM2allCell[EM2seedindex-1+i];
			EM2SmallSum += EM2allCell[EM2seedindex+11+i];
		}
	}

	float EM2LargeSum = 0;
	for (int i=0;i<9;i++) {
		EM2LargeSum += EM2allCell[EM2seedindex-4+i];
		EM2LargeSum += EM2allCell[EM2seedindex-16+i];
		EM2LargeSum += EM2allCell[EM2seedindex+8+i];
	}

	if(EM2LargeSum > 0)
	  return EM2SmallSum/EM2LargeSum;
	else
	  return 0;

}

bool TrigT1CaloBaseFex::IseFEXIsoTDR ( const std::vector<float>& EM2allCell, const float& seedPlace, const float& Isovalue) const {
	bool Is_eFEXIsoTDR = false;
	if (TrigT1CaloBaseFex::eFEXIsoTDR(EM2allCell,seedPlace) > Is_eFEXIsoTDR) Is_eFEXIsoTDR = true;
	return Is_eFEXIsoTDR;
}

StatusCode TrigT1CaloBaseFex::getIDHelper(const CaloCell_SuperCell_ID*& m_idHelper){
	const CaloIdManager* caloMgr(0);
	if (detStore()->retrieve(caloMgr).isFailure()){
		return StatusCode::FAILURE;
	}
	m_idHelper = caloMgr->getCaloCell_SuperCell_ID();
	if (!m_idHelper) return StatusCode::FAILURE;
	else return StatusCode::SUCCESS;
}

float TrigT1CaloBaseFex::CaloCellET(CaloCell* const &inputCell, float digitScale, float digitThreshold){
	if (inputCell==NULL) return 0.;
	// Check that timing is correct 
        if ( m_useProvenance ) {
	  bool correctProv = (inputCell->provenance() & m_qualBitMask);
	  if (!correctProv) return 0.;
	}
        // Calculates the ET (before digitization)
        float inputCell_energy = inputCell->energy();
        float inputCell_eta = inputCell->eta();
        float inputCell_ET = inputCell_energy / cosh(inputCell_eta);
        // Check to see if negative ET values are allowed
        bool allowNegs = false;
        if (digitScale < 0.){
	        digitScale = fabs(digitScale);
	        allowNegs = true;
        }
        if (inputCell_ET==0) return 0.;
        else if (digitScale==0) return inputCell_ET;
        if (allowNegs || inputCell_ET>0.){
	        // Split up ET into magnitude & whether it's positive or negative
	        float posOrNeg = inputCell_ET / fabs(inputCell_ET);
	        inputCell_ET = fabs(inputCell_ET);
	        // If no digitisation, return ET following noise cut
	        if (digitScale == 0){
	                if (inputCell_ET>digitThreshold) return inputCell_ET*posOrNeg;
	  	        else return 0.;
	        }
	        // Apply digitization & then noise cut
	        else {
		        float divET = inputCell_ET / digitScale;
		        int roundET = divET;
		        float result = digitScale * roundET;
		        if (digitThreshold == 0) return result*posOrNeg;
		        else if (result >= digitThreshold) return result*posOrNeg;
		        else return 0;
	        }
        }
	else return 0.;
}

CaloCell* TrigT1CaloBaseFex::returnCellFromCont(Identifier inputID, const CaloCellContainer* &cellContainer){
	for (auto iCell : *cellContainer){
	  bool sameIndices = (iCell->ID() == inputID);
	  if (sameIndices) return iCell;
	}
	return NULL;
}

CaloCell* TrigT1CaloBaseFex::NextEtaCell_Barrel(CaloCell* inputCell, bool upwards, const CaloCellContainer* &cellContainer, const CaloCell_SuperCell_ID* &idHelper){
	MsgStream msg(msgSvc(), name());
	const Identifier ithID = inputCell->ID();
        const int ithEta_index = idHelper->eta(ithID);
        const int ithPhi_index = idHelper->phi(ithID);
        const int ithSampling = idHelper->sampling(ithID);
        const int ithSub_calo = idHelper->sub_calo(ithID);
        const int ithPos_neg = idHelper->pos_neg(ithID);
        const int ithRegion = idHelper->region(ithID);

        // Extreme indices of each region
        int maxEta_index = 0;
        int minEta_index = 0;
        if (ithRegion==0){
		if (ithSampling == 0) maxEta_index = 14;
		else if (ithSampling == 1 || ithSampling == 2) maxEta_index = 55;
		else if (ithSampling == 3) maxEta_index = 13;
		else msg << MSG::WARNING << "ERROR: " << __LINE__ << endreq;
        }
        else if (ithRegion==1){
		if (ithSampling == 1) maxEta_index =2;
		else if (ithSampling == 2) maxEta_index=0;
		else msg << MSG::WARNING << "ERROR: " << __LINE__ << endreq;
        }
        else msg << MSG::WARNING << "ERROR: " << __LINE__ << endreq;
    
        // Declare next values, default initialisation is the same as cell
        int nextEta_index = ithEta_index;
        // Phi shouldn't change!
        // Sampling shouldn't change!
        int nextSub_calo = ithSub_calo;
        int nextPos_neg = ithPos_neg;
        int nextRegion = ithRegion;

        // Calculate the increment for eta: it depends on whether we are moving 'up' & which side we are on
        int incrementEta;
        if (upwards) incrementEta = ithPos_neg;
        else incrementEta = -1*ithPos_neg;

        int tracker = 0;    
    
        // If first cell in region & moving more inwards
        if (ithEta_index==minEta_index && incrementEta==-1){
		if (ithRegion == 0){
			nextEta_index = 0;
			nextPos_neg = ithPos_neg * -1;
			tracker = 1;
		}
		else if (ithRegion == 1){
			nextEta_index = 55;
			nextRegion = 0;
			tracker = 2;
		}
		else msg << MSG::WARNING << "ERROR: " << __LINE__ << endreq;
        }

        // If last cell in region & moving outwards
        else if ((ithEta_index == maxEta_index) && (incrementEta == 1)) {
		// Reg 0, Layers 1 & 2 go to barrel region 1
		if ((ithRegion == 0)&&(ithSampling == 1 || ithSampling == 2)){
			nextRegion = 1;
        		nextEta_index = 0;
        		tracker = 3;
		}
		// Reg 0, Layer 0 & 3 go to OW region 0
		else if ((ithRegion == 0)&&(ithSampling == 0 || ithSampling == 3)){
			nextEta_index = 0;
			nextRegion = 0;
			nextPos_neg = 2*ithPos_neg;
			tracker = 4;
		}
		// Reg 1, Layer 1 go to OW region 2
		else if ((ithRegion == 1)&&(ithSampling == 1)){
			nextEta_index=0;
			nextRegion = 2;
			nextPos_neg = 2 * ithPos_neg;
			tracker = 5;
		}
		// Reg 1, Layers 1 & 2 go to OW region 1
		else if ((ithRegion == 1)&&(ithSampling == 2)){
			nextEta_index=0;
			nextRegion = 1;
			nextPos_neg = 2 * ithPos_neg;
			tracker = 6;
		}
		else msg << MSG::WARNING << "ERROR: " << __LINE__ << endreq;
        }
        // Otherwise 'simply' next cell along
        else {
		nextEta_index = ithEta_index + incrementEta;
		tracker = 7;
	}
	//msg << MSG::DEBUG << "B Tracker = " << tracker << endreq;
        // Form identifier, find cell & return it
        // sub_calo, left_pos_neg, 2, region, eta_index, down_phi_index
        Identifier nextCellID = idHelper->CaloCell_SuperCell_ID::cell_id(nextSub_calo, nextPos_neg, ithSampling, nextRegion, nextEta_index, ithPhi_index);
        CaloCell* nextCell = returnCellFromCont(nextCellID, cellContainer);      
        if (nextCell == NULL) {
		msg << MSG::WARNING << "ERROR: " << __LINE__ << endreq;
		msg << MSG::WARNING << "Barrel Tracker = " << tracker << endreq;
		msg << MSG::WARNING << "from nextCellID: "<<idHelper->sub_calo(nextCellID)<<", "<<idHelper->pos_neg(nextCellID)<<", "<<idHelper->sampling(nextCellID)<<", "<<idHelper->region(nextCellID)<<", "<<idHelper->eta(nextCellID)<<", "<<idHelper->phi(nextCellID)<<", "<<idHelper->calo_cell_hash(nextCellID)<<", "<<nextCellID << endreq;            
	}
        else {
		Identifier newID = nextCell->ID();
		int IDsample = idHelper->sampling(nextCell->ID());
		if (IDsample!=ithSampling){
			msg << MSG::WARNING << "Layer has changed " << " tracker = " << tracker << endreq;
			msg << MSG::WARNING << "from nextCellID: "<<idHelper->sub_calo(nextCellID)<<", "<<idHelper->pos_neg(nextCellID)<<", "<<idHelper->sampling(nextCellID)<<", "<<idHelper->region(nextCellID)<<", "<<idHelper->eta(nextCellID)<<", "<<idHelper->phi(nextCellID)<<", "<<idHelper->calo_cell_hash(nextCellID)<<", "<<nextCellID << endreq;
			msg << MSG::WARNING << "from ID from new cell: "<<idHelper->sub_calo(newID)<<", "<<idHelper->pos_neg(newID)<<", "<<idHelper->sampling(newID)<<", "<<idHelper->region(newID)<<", "<<idHelper->eta(newID)<<", "<<idHelper->phi(newID)<<", "<<idHelper->calo_cell_hash(newID)<<", "<<newID << endreq;
        		msg << MSG::WARNING << "comp indices: "<< (nextCellID == newID) << endreq;
      		}
	}
	if (nextCell && (nextCell->ID() != nextCellID)) msg << MSG::WARNING << __LINE__ << " does not match" << endreq;
	return nextCell;
}

CaloCell* TrigT1CaloBaseFex::NextEtaCell_OW(CaloCell*inputCell, bool upwards, const CaloCellContainer* &cellContainer, const  CaloCell_SuperCell_ID* &idHelper){
	MsgStream msg(msgSvc(), name());
	Identifier ithID = inputCell->ID();
        int ithEta_index = idHelper->eta(ithID);
        const int ithPhi_index = idHelper->phi(ithID);
        const int ithSampling = idHelper->sampling(ithID);
        int ithSub_calo = idHelper->sub_calo(ithID);
        int ithPos_neg = idHelper->pos_neg(ithID);
        int ithRegion = idHelper->region(ithID);

        // Declare next values, default initialisation is the same as cell
        int nextEta_index = ithEta_index;
        int nextPhi_index = ithPhi_index;
        // Sampling shouldn't change!
        int nextSub_calo = ithSub_calo;
        int nextPos_neg = ithPos_neg;
        int nextRegion = ithRegion;

        // Maximum indices for barrel region 0:
        int maxEta_index = 0;
        int minEta_index = 0;
        // Set max / min values based on ithRegion
        if (ithSampling==0) maxEta_index = 2;
        else if (ithSampling==2 && ithRegion==0) maxEta_index = 0;
        else if (ithSampling==2 && ithRegion==1) maxEta_index = 42; 
        else if (ithSampling==3) maxEta_index=9;
        else if (ithSampling==1){
		switch(ithRegion){
		  case 0:
                    maxEta_index=0;
                    break;
                  case 1:
                    msg << MSG::WARNING << "ERROR " << __LINE__ << endreq;
                    break;
                  case 2:
                    maxEta_index=11;
                    break;
                  case 3:
                    maxEta_index=11;// Should this be 11? - it was 7
                    break;
                  case 4:
                    maxEta_index=15;  
                    break;
                  case 5:
                    maxEta_index=0;
                    break;
                  default:
		    msg << MSG::WARNING << "OW region is not covered: " << ithRegion << endreq;
                }
        }        
        else msg << MSG::WARNING << "ERROR: "<< __LINE__<< endreq;
        // Calculate the increment for eta: it depends on whether we are moving 'up' & which side we are on
        int incrementEta;
        int ithSide = ithPos_neg / abs(ithPos_neg);
        if (upwards) incrementEta = ithSide;
        else incrementEta = ithSide * -1;
        int tracker = 0;
        // Lower end of OW, going inwards
        if (ithEta_index==minEta_index && ithRegion==0 && incrementEta==-1){
		nextPos_neg = ithSide;
                if (ithSampling==0){
			nextRegion = 0;
			nextEta_index = 14;
                        tracker = 1;
                  }
                  else if (ithSampling==1){
                        nextRegion = 1;
                        nextEta_index = 2;
                        tracker = 2;
                  }
                  else if (ithSampling==2){
                        nextRegion = 1;
                        nextEta_index = 0;
                        tracker = 3;
                  }
                  else if (ithSampling==3){
                        nextRegion = 0;
                        nextEta_index = 13;
                        tracker = 4;
                  }
        }
        // Higher end of OW, going outwards
        else if (ithEta_index==maxEta_index && incrementEta==1){
		// Layers 0 & 3 aren't in IW
                if (ithSampling==0 || ithSampling==3) return NULL;
                else if (ithSampling==2 && ithRegion==0){
			nextRegion = 1;
                        nextEta_index = 0;
                        tracker = 5;
                }
                else if ((ithSampling==2 && ithRegion==1)||(ithSampling==1 && ithRegion==5)){
			// Reaches IW
			nextEta_index=0;
                        nextRegion=0;
                        nextPhi_index=ithPhi_index/2;
                        nextPos_neg=3*ithSide;
                        tracker=11;
                }
                else if (ithSampling==1 && ithRegion==0){
			// Unsure what to do??
                        nextRegion = 2;
                        nextEta_index = 0;  
                        tracker = 6;
                }
                else if (ithSampling==1){
                        nextRegion=ithRegion + 1;
                        nextEta_index=0;
                        tracker = 7;
                }
        }
        // Lower end of region in OW, going inwards
        else if (ithEta_index==minEta_index && incrementEta==-1){
		// Shouldn't apply to layers 0 & 3
                // Only case for layer 2 should be in region 1
                if (ithSampling==2){
			nextEta_index=0;
                        nextRegion=0;
                        tracker = 8;
                }
                else if (ithSampling==1){
			if (ithRegion==0){
				nextPos_neg = ithSide;
				nextRegion = 1;
                                nextEta_index = 2;
                                tracker = 9;
                        }
			else {
				tracker = 10;
                                // Layer one has muliple regions
                                nextRegion = ithRegion-1;
                                if (nextRegion==0) {
					nextEta_index=0;
					msg << MSG::WARNING << "ERROR: "<< __LINE__<< endreq;
                                }
                                else if (nextRegion==1) {
					nextRegion = 0;
					nextEta_index= 0;
                                }
				else if (nextRegion==2) nextEta_index=11;
				else if (nextRegion==3) nextEta_index=7;
				else if (nextRegion==4) nextEta_index=15;
			}
                }
        }
        // Middle of region in middle of endcap
        else {
		nextEta_index = ithEta_index+incrementEta;
		tracker = 11;
        }
        Identifier nextCellID = idHelper->CaloCell_SuperCell_ID::cell_id(nextSub_calo, nextPos_neg, ithSampling, nextRegion, nextEta_index, nextPhi_index);
        CaloCell* nextCell = returnCellFromCont(nextCellID, cellContainer);
        if (nextCell == NULL) {
		msg << MSG::WARNING << "ERROR: "<<__LINE__<< endreq;
		msg << MSG::WARNING << "OW Tracker = "<<tracker<< endreq;
		msg << MSG::WARNING << "from nextCellID: "<<idHelper->sub_calo(nextCellID)<<", "<<idHelper->pos_neg(nextCellID)<<", "<<idHelper->sampling(nextCellID)<<", "<<idHelper->region(nextCellID)<<", "<<idHelper->eta(nextCellID)<<", "<<idHelper->phi(nextCellID)<<", "<<idHelper->calo_cell_hash(nextCellID)<<", "<<nextCellID<< endreq;
		msg << MSG::WARNING << "Increment eta = "<<incrementEta<<", max_eta = "<<maxEta_index<<", min_eta = "<<minEta_index<< endreq;     
        }
        else {
          	Identifier newID = nextCell->ID();
          	int IDsample = idHelper->sampling(nextCell->ID());
          	if (IDsample!=ithSampling){
            		msg << MSG::WARNING << "Layer has changed "<<" tracker = "<<tracker<< endreq;
			msg << MSG::WARNING << "from nextCellID: "<<idHelper->sub_calo(nextCellID)<<", "<<idHelper->pos_neg(nextCellID)<<", "<<idHelper->sampling(nextCellID)<<", "<<idHelper->region(nextCellID)<<", "<<idHelper->eta(nextCellID)<<", "<<idHelper->phi(nextCellID)<<", "<<idHelper->calo_cell_hash(nextCellID)<<", "<<nextCellID<< endreq;
			msg << MSG::WARNING << "from ID from new cell: "<<idHelper->sub_calo(newID)<<", "<<idHelper->pos_neg(newID)<<", "<<idHelper->sampling(newID)<<", "<<idHelper->region(newID)<<", "<<idHelper->eta(newID)<<", "<<idHelper->phi(newID)<<", "<<idHelper->calo_cell_hash(newID)<<", "<<newID<< endreq;
			msg << MSG::WARNING << "comp indices: "<<(nextCellID == newID) << endreq;
          	}
        }
        if (nextCell && (nextCell->ID() != nextCellID)) msg << MSG::WARNING <<__LINE__<< " does not match" << endreq;
        return nextCell;
}

CaloCell* TrigT1CaloBaseFex::NextEtaCell_IW(CaloCell*inputCell, bool upwards, const CaloCellContainer* &cellContainer, const  CaloCell_SuperCell_ID* &idHelper){
	MsgStream msg(msgSvc(), name());
	const Identifier ithID = inputCell->ID();
	const int ithEta_index = idHelper->eta(ithID);
        const int ithPhi_index = idHelper->phi(ithID);
        const int ithSampling = idHelper->sampling(ithID);
        const int ithSub_calo = idHelper->sub_calo(ithID);
        const int ithPos_neg = idHelper->pos_neg(ithID);
        const int ithRegion = idHelper->region(ithID);
        //int tracker =0;
        // Declare next values, default initialisation is the same as cell
        int nextEta_index = ithEta_index;
        int nextPhi_index = ithPhi_index;
        // Sampling shouldn't change!
        int nextSub_calo = ithSub_calo;
        int nextPos_neg = ithPos_neg;
        int nextRegion = ithRegion;

        // Maximum indices for barrel region 0:
        int maxEta_index = 0;
        int minEta_index = 0;

        if (ithRegion==0){
		maxEta_index=2;
		minEta_index=0;
        }
        else if (ithRegion!=1) msg << MSG::WARNING << "ERROR: " <<__LINE__<< endreq;

        // Calculate the increment for eta: it depends on whether we are moving 'up' & which side we are on
        int incrementEta;
        int ithSide = ithPos_neg / abs(ithPos_neg);
        if (upwards) incrementEta = ithSide;
        else incrementEta = ithSide * -1;

        // Lower end of region IW, going inwards
        if (ithEta_index==minEta_index&& incrementEta==-1){
		// Goes to OW
                if (ithRegion == 0){
			nextPos_neg = 2*ithSide;
			nextPhi_index=2*ithPhi_index;
			if (ithSampling==1){	
				// tracker=1;
				nextRegion=5;
				nextEta_index=0;
			}
			else if (ithSampling==2){
				// tracker=2;
				nextRegion=1;
				nextEta_index=42;
			}
			else msg << MSG::WARNING << "ERROR: " <<__LINE__<< endreq;
              }
              // Goes to IW region 0
              else if (ithRegion == 1){
		      // tracker=3;
                      nextRegion=0;
                      nextEta_index=2;
              }
        }
        // Upper end of region in IW
        else if (ithEta_index==maxEta_index && incrementEta==1){
		// Goes to region 1
                if (ithRegion==0){
			// tracker=4;
			nextRegion=1;
			nextEta_index=0;      
		}
		// Reaches FCAL
		else if (ithRegion==1) return NULL; 
        }
        // Increment eta like normal
        else {
		// tracker=5;
		nextEta_index=ithEta_index+incrementEta;
        }
        Identifier nextCellID = idHelper->CaloCell_SuperCell_ID::cell_id(nextSub_calo, nextPos_neg, ithSampling, nextRegion, nextEta_index, nextPhi_index);
        CaloCell* nextCell = returnCellFromCont(nextCellID, cellContainer);
        if (nextCell && (nextCell->ID() != nextCellID)) msg << MSG::WARNING <<__LINE__<<" does not match" << endreq;
        return nextCell;
}

CaloCell* TrigT1CaloBaseFex::NextEtaCell(CaloCell* inputCell, bool upwards, const CaloCellContainer* &cellContainer, const  CaloCell_SuperCell_ID* &idHelper){
	MsgStream msg(msgSvc(), name());
	if (inputCell==NULL) return NULL;
        Identifier ithID = inputCell->ID();
        int ithSub_calo = idHelper->sub_calo(ithID);
        int ithPos_neg = idHelper->pos_neg(ithID);
        CaloCell* tempCell = NULL;
        // Only works for LArEM
        if (ithSub_calo==0){
		// Barrel regions
                if (abs(ithPos_neg)==1) tempCell = NextEtaCell_Barrel(inputCell, upwards, cellContainer, idHelper);
                // EC OW
                else if (abs(ithPos_neg)==2) tempCell = NextEtaCell_OW(inputCell, upwards, cellContainer, idHelper);
                // EC IW  
                else if (abs(ithPos_neg)==3) tempCell = NextEtaCell_IW(inputCell, upwards, cellContainer, idHelper);
                // Not barrel or end cap
                else {
			msg << MSG::WARNING << "Layer 2 cell not passed to specific method at" << inputCell->eta() << " , " << inputCell->phi() << endreq;
                	return NULL;
                }
                return tempCell;
        }
        // Is FCAL
        else {
		msg << MSG::WARNING << "Next eta cell called for non-EM SC!" << endreq;
		return NULL;
        }
}

int TrigT1CaloBaseFex::restrictPhiIndex(int input_index, bool is64){
	if (is64&&input_index<0) return input_index+64;
        else if (is64&&input_index>63) return input_index-64;
        else if (!(is64)&&input_index<0) return input_index+32;
        else if (!(is64)&&input_index>31) return input_index-32;
        else return input_index;
}

CaloCell* TrigT1CaloBaseFex::NextPhiCell(CaloCell*inputCell, bool upwards, const CaloCellContainer* &cellContainer, const  CaloCell_SuperCell_ID* &idHelper){
        MsgStream msg(msgSvc(), name());
	if (inputCell==NULL) return NULL;
        const Identifier ithID = inputCell->ID();
        const int ithEta_index = idHelper->eta(ithID);
        const int ithPhi_index = idHelper->phi(ithID);
        const int ithSampling = idHelper->sampling(ithID);
        const int ithSub_calo = idHelper->sub_calo(ithID);
        const int ithPos_neg = idHelper->pos_neg(ithID);
        const int ithRegion = idHelper->region(ithID);

        bool is64;
        if (abs(ithPos_neg)==3) is64 = false;
        else is64 = true;

        int incrementPhi;
        if (upwards==true) incrementPhi=1;
        else incrementPhi=-1;

        const int nextPhi_index = restrictPhiIndex(ithPhi_index+incrementPhi, is64);
        Identifier nextCellID = idHelper->CaloCell_SuperCell_ID::cell_id(ithSub_calo, ithPos_neg, ithSampling, ithRegion, ithEta_index, nextPhi_index);
        CaloCell* nextCell = returnCellFromCont(nextCellID, cellContainer);
        if (nextCell && (nextCell->ID() != nextCellID)) msg << MSG::WARNING << __LINE__ << " does not match" << endreq;
        if (nextCell == NULL) msg << MSG::WARNING << "Next phi cell is NULL at " << __LINE__ << endreq;
        return nextCell;
}

bool TrigT1CaloBaseFex::localMax(const CaloCellContainer* &inputContainer, CaloCell* inputCell, const CaloCell_SuperCell_ID* &idHelper, float digitScale, float digitThreshold){
	return localMax(inputContainer, inputCell, 0, idHelper, digitScale, digitThreshold);
}

bool TrigT1CaloBaseFex::localMax(const CaloCellContainer* &inputContainer, CaloCell* inputCell, int numOthers, const CaloCell_SuperCell_ID* &idHelper, float digitScale, float digitThreshold){
        MsgStream msg(msgSvc(), name());
        if (inputCell == NULL) return false;
        // Get ID info
        const Identifier inputID = inputCell->ID();
        const int sub_calo = idHelper->sub_calo(inputID);
        const int pos_neg = idHelper->pos_neg(inputID);
        if (!(sub_calo == 0 || sub_calo == 1) || !(abs(pos_neg) < 4)){
	        msg << MSG::WARNING << "ERROR with local max logic" << endreq;
	        return false;
        }
        double seedCandidateEnergy = CaloCellET(inputCell, digitScale, digitThreshold);
        int nCellsMoreEnergetic = 0;
        CaloCell* leftCell = NextEtaCell(inputCell, true, inputContainer, idHelper); 
        if (leftCell != NULL){
		double leftEnergy = CaloCellET(leftCell, digitScale, 0.);
                if (leftEnergy>=seedCandidateEnergy)  nCellsMoreEnergetic++;
        }

        CaloCell* rightCell = NextEtaCell(inputCell, false, inputContainer, idHelper); 
        if (rightCell != NULL){
                double rightEnergy = CaloCellET(rightCell, digitScale, 0.);
		if (rightEnergy>seedCandidateEnergy)  nCellsMoreEnergetic++;
        }

        CaloCell* upCell = NextPhiCell(inputCell, true, inputContainer, idHelper);
        if (upCell != NULL){
                double upEnergy = CaloCellET(upCell, digitScale, 0.);
                if (upEnergy>seedCandidateEnergy) nCellsMoreEnergetic++;
        }

        CaloCell* downCell = NextPhiCell(inputCell, false, inputContainer, idHelper);
        if (downCell != NULL){
                double downEnergy = CaloCellET(downCell, digitScale, 0.);
                if (downEnergy>=seedCandidateEnergy) nCellsMoreEnergetic++;
        }
	
        if (upCell != NULL){
                CaloCell* upRightCell = NextEtaCell(upCell, false, inputContainer, idHelper);
                if (upRightCell != NULL){
                        double upRightEnergy = CaloCellET(upRightCell, digitScale, 0.);
			if (upRightEnergy>seedCandidateEnergy) nCellsMoreEnergetic++;
                }

                CaloCell* upLeftCell = NextEtaCell(upCell, true, inputContainer, idHelper);
                if (upLeftCell != NULL){
                        double upLeftEnergy = CaloCellET(upLeftCell, digitScale, 0.);
                        if (upLeftEnergy>seedCandidateEnergy) nCellsMoreEnergetic++;
                }
        }
  
        if (downCell != NULL){
                CaloCell* downRightCell = NextEtaCell(downCell, false, inputContainer, idHelper);
                if (downRightCell != NULL){
                        double downRightEnergy = CaloCellET(downRightCell, digitScale, 0.);
                        if (downRightEnergy>seedCandidateEnergy) nCellsMoreEnergetic++;
                }

                CaloCell* downLeftCell = NextEtaCell(downCell, true, inputContainer, idHelper);
                if (downLeftCell != NULL){
                        double downLeftEnergy = CaloCellET(downLeftCell, digitScale, 0.);
                        if (downLeftEnergy>seedCandidateEnergy) nCellsMoreEnergetic++;
                }
        }
        // If candidate is more energetic than all of neighbours, it is a local max 
        if (nCellsMoreEnergetic <= numOthers) return true;
        else return false;
}

void TrigT1CaloBaseFex::addOnce(CaloCell* inputCell, std::vector<CaloCell*> &outputVector){
	MsgStream msg(msgSvc(), name());
	if (inputCell==NULL) return;
        bool alreadyThere = false;
        for (auto oCell : outputVector){
          if (oCell==NULL) msg << MSG::WARNING << "NULL cell in vector" << endreq;
          if (inputCell->ID() == oCell->ID()) alreadyThere=true;
        }
        if (!alreadyThere) outputVector.push_back(inputCell);
}

std::vector<CaloCell*> TrigT1CaloBaseFex::L2cluster(CaloCell* centreCell, int etaWidth, int phiWidth, const CaloCellContainer* scells, const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh ){
	// Forms the central band of cells, spread in phi
	MsgStream msg(msgSvc(), name());
        std::vector<CaloCell*> centCells;
        centCells.push_back(centreCell);
        CaloCell* upPhiCell = NextPhiCell(centreCell,true,scells,idHelper);
        CaloCell* downPhiCell = NextPhiCell(centreCell,false,scells,idHelper);
        CaloCell* energeticPhiCell;
        // Finds the most energetic phi neighbour, defaulting to the 'down' side if they are equal
        if (CaloCellET(upPhiCell, digitScale, digitThresh) > CaloCellET(downPhiCell, digitScale, digitThresh)) energeticPhiCell = upPhiCell;
        else energeticPhiCell = downPhiCell;
        if (phiWidth == 2) addOnce(energeticPhiCell, centCells); //centCells.push_back(energeticPhiCell);
        else if (phiWidth == 3){
		addOnce(upPhiCell, centCells); //centCells.push_back(upPhiCell);
                addOnce(downPhiCell, centCells); //centCells.push_back(downPhiCell);
        }
        else if (phiWidth > 3) msg << MSG::WARNING << "ERROR: phiWidth not 2 or 3!!!" << endreq;
        // Forms the main cluster. Starts with each SC in the central band and spreads outward in eta
        std::vector<CaloCell*> clusCells;
        int halfEtaWidth = (etaWidth-1)/2;
        int backToEta = (2*halfEtaWidth)+1;
        if (backToEta != etaWidth) msg << MSG::WARNING << "Eta width doesn't match! " << backToEta << " -> " << halfEtaWidth << " -> " << etaWidth << "  " << __LINE__ << endreq;
        for (auto ithCentCell : centCells){
          addOnce(ithCentCell, clusCells); //clusCells.push_back(ithCentCell);
          if (etaWidth > 1){
		  CaloCell* tempRightCell = NextEtaCell(ithCentCell,true,scells,idHelper);
                  CaloCell* tempLeftCell = NextEtaCell(ithCentCell,false,scells,idHelper);
                  addOnce(tempRightCell, clusCells); //clusCells.push_back(tempRightCell);
                  addOnce(tempLeftCell, clusCells); //clusCells.push_back(tempLeftCell);
                  for (int i = 1; i < halfEtaWidth; i++){
                    tempRightCell = NextEtaCell(tempRightCell,true,scells,idHelper);
                    tempLeftCell = NextEtaCell(tempLeftCell,false,scells,idHelper);
                    addOnce(tempRightCell, clusCells); //clusCells.push_back(tempRightCell);
                    addOnce(tempLeftCell, clusCells); //clusCells.push_back(tempLeftCell);
                  }
          }
        }
        return clusCells;
}

void TrigT1CaloBaseFex::fromLayer2toLayer1(const CaloCellContainer* &inputContainer, CaloCell* inputCell, std::vector<CaloCell*> &outputVector, const CaloCell_SuperCell_ID* &idHelper){
	MsgStream msg(msgSvc(), name());
	if (inputCell==NULL) return;
        // Gets ID info
        Identifier inputID = inputCell->ID();
        int sampling = idHelper->sampling(inputID);
        const int sub_calo = idHelper->sub_calo(inputID);
        const int pos_neg = idHelper->pos_neg(inputID);
        int region = idHelper->region(inputID);
        int eta_index = idHelper->eta(inputID);
        const int phi_index = idHelper->phi(inputID);
        int tracker = 0;
        if (sampling != 2) return;
        // Default values are same as input
        int outputRegion = region;
        int outputEta = eta_index;
        bool oneCell = false; // True if layer 2 SC only matches to a single layer 1 SC
        // Barrel reg 0
        if ((abs(pos_neg) == 1)&&(region == 0)){
		oneCell = true;
		tracker = 1;
        }
        // Barrel reg 1: 3 layer 1 SCs for 1 layer 2 SC
        else if ((abs(pos_neg) == 1)&&(region == 1)){
		tracker = 2;
		for (unsigned int i = 0; i < 3; i++){
		  Identifier resultID = idHelper->CaloCell_SuperCell_ID::cell_id(sub_calo, pos_neg, 1, region, i, phi_index);
		  CaloCell* resultCell = returnCellFromCont(resultID, inputContainer);
		  addOnce(resultCell,outputVector);
		}
        }
        else if (abs(pos_neg)==2&&((region==0)||(region==1 && eta_index <= 2))){
		// OW region 0: 1 layer 1 SC for 4 layer 2 SCs
                tracker = 3;
                outputRegion = 0;
                outputEta = 0;
                oneCell = true;
        }
        else if (abs(pos_neg)==2&&region==1 && eta_index <= 14){
                // OW region 1 (on doc): 1:1 match
                tracker = 4;
                outputRegion = 2;
                outputEta = eta_index - 3;
                oneCell = true;
        }
        else if (abs(pos_neg) == 2 && region == 1 && eta_index <= 22){
                // OW Reg 2 (on doc): 6:4 split!!
                outputRegion = 3;
                // Middle 2 layer cells match central 2 layer 1 cells
                if (eta_index%4 == 0 || eta_index%4 ==1){
                        tracker = 5;
                        oneCell = true;
                        if (eta_index < 20) outputEta = eta_index -14;
                        else outputEta = eta_index - 12;
                }
                // Edges have a 2:1 ratio. 2 L1s for each L2
                else {
			tracker = 6;
                        int offset = 0;
                        if (eta_index == 15) offset = 15;
                        else if (eta_index == 18) offset = 14;
                        else if (eta_index == 19) offset = 13;
                        else if (eta_index == 22) offset = 12;
                        else {
                                msg << MSG::WARNING << "ERROR: " << __LINE__ << endreq;
                        }
                        for (unsigned int  i = 0; i < 2; i++){
                          outputEta = i+eta_index - offset;
                          Identifier resultID = idHelper->CaloCell_SuperCell_ID::cell_id(sub_calo, pos_neg, 1, outputRegion, outputEta, phi_index);
                          CaloCell* resultCell = returnCellFromCont(resultID, inputContainer);
                          addOnce(resultCell,outputVector);
                        }
                } 
        }
        else if (abs(pos_neg)==2 && region == 1 && eta_index <= 38){
                // OW Reg 3 (on doc): 1:1 match
                tracker = 7;
                oneCell = true;
                outputRegion = 4;
                outputEta = eta_index - 23;
        }
        else if (abs(pos_neg)==2 && region == 1 && eta_index <= 42){
                // OW Reg 4 (on doc): 1 L1 for all 4 L2s
                tracker = 8;
                oneCell = true;
                outputEta = 0;
                outputRegion = 5;
        }
        if (oneCell){
                Identifier resultID = idHelper->CaloCell_SuperCell_ID::cell_id(sub_calo, pos_neg, 1, outputRegion, outputEta, phi_index);
                CaloCell* resultCell = returnCellFromCont(resultID, inputContainer);
                addOnce(resultCell,outputVector);
        }
        //msg << MSG::DEBUG << "L2->L1 tracker = " << tracker << endreq;
}

CaloCell* TrigT1CaloBaseFex::fromLayer2toPS(const CaloCellContainer* &inputContainer, CaloCell* inputCell, const CaloCell_SuperCell_ID* &idHelper){
	// Gets ID info
	MsgStream msg(msgSvc(), name());
        if (inputCell==NULL) return NULL;
        CaloCell* resultCell = NULL;
        Identifier inputID = inputCell->ID();
        int sampling = idHelper->sampling(inputID);
        const int sub_calo = idHelper->sub_calo(inputID);
        const int pos_neg = idHelper->pos_neg(inputID);
        int region = idHelper->region(inputID);
        int eta_index = idHelper->eta(inputID);  
        const int phi_index = idHelper->phi(inputID);
        int tracker = 0;
        if (sampling != 2) return NULL;
        if (abs(pos_neg)==2 && (eta_index<3 || eta_index>14)) return NULL;
        if (abs(pos_neg)==3) return NULL;
        // Default values are same as input
        int outputRegion = region;
        int outputEta = eta_index;
        // Is barrel Reg 0
        if (abs(pos_neg)==1 && region ==0){
                int outputEta = eta_index/4;
                Identifier resultID = idHelper->CaloCell_SuperCell_ID::cell_id(sub_calo, pos_neg, 0, outputRegion, outputEta, phi_index);
                resultCell = returnCellFromCont(resultID, inputContainer);
                tracker = 1;  
        }
        else if (abs(pos_neg)==1 && region ==1){
                Identifier resultID = idHelper->CaloCell_SuperCell_ID::cell_id(sub_calo, pos_neg, 0, 0, 14, phi_index);
                resultCell = returnCellFromCont(resultID, inputContainer);
                tracker = 2;
        }
        else if (abs(pos_neg)==2 && region ==1){
                outputEta = (eta_index - 3)/4;
                outputRegion = 0;
                Identifier resultID = idHelper->CaloCell_SuperCell_ID::cell_id(sub_calo, pos_neg, 0, outputRegion, outputEta, phi_index);
                resultCell = returnCellFromCont(resultID, inputContainer); 
                tracker = 3;
        }
        //msg << MSG::DEBUG << "L2->PS tracker = " << tracker << endreq;
        return resultCell;
}

CaloCell* TrigT1CaloBaseFex::fromLayer2toLayer3(const CaloCellContainer* &inputContainer, CaloCell* inputCell, const CaloCell_SuperCell_ID* &idHelper){
	// Gets ID info
	MsgStream msg(msgSvc(), name());
        if (inputCell==NULL) return NULL;;
        CaloCell* resultCell = NULL;
        Identifier inputID = inputCell->ID();
        int sampling = idHelper->sampling(inputID);
        const int sub_calo = idHelper->sub_calo(inputID);
        const int pos_neg = idHelper->pos_neg(inputID);
        int region = idHelper->region(inputID);
        int eta_index = idHelper->eta(inputID);  
        const int phi_index = idHelper->phi(inputID);
        int tracker = 0;
        if (sampling != 2) return NULL;
        else if (abs(pos_neg)==1 && ((region==0 && eta_index>53)||region==1)) return NULL;
        else if ((abs(pos_neg)==2) && (region == 0 || (region == 1 && eta_index < 3))) return NULL;
        else if (abs(pos_neg)==3) return NULL;
        // Default values are same as input
        int outputRegion = region;
        int outputEta = eta_index;
        // Is barrel Reg 0
        if (abs(pos_neg)==1 && region ==0){
                int outputEta = eta_index/4;
                Identifier resultID = idHelper->CaloCell_SuperCell_ID::cell_id(sub_calo, pos_neg, 3, outputRegion, outputEta, phi_index);
                resultCell = returnCellFromCont(resultID, inputContainer);
                tracker = 1;  
        }
        else if (abs(pos_neg)==2 && region ==1){
                outputEta = (eta_index - 3)/4;
                outputRegion = 0;
                Identifier resultID = idHelper->CaloCell_SuperCell_ID::cell_id(sub_calo, pos_neg, 3, outputRegion, outputEta, phi_index);
                resultCell = returnCellFromCont(resultID, inputContainer); 
                tracker = 2;
        }
        //msg << MSG::DEBUG << "L2->L3 tracker = " << tracker << endreq;
        return resultCell;
}

std::vector<CaloCell*> TrigT1CaloBaseFex::TDR_Clus(CaloCell* centreCell, int etaWidth, int phiWidth, const CaloCellContainer* scells,const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh){
	// Find the L2 cells
	MsgStream msg(msgSvc(), name());
        std::vector<CaloCell*> L2cells = L2cluster(centreCell, etaWidth, phiWidth, scells, idHelper, digitScale, digitThresh);
        // Forms a vector of the centre L2 cells (to be used to find L0/3 SCs)
        std::vector<CaloCell*> centCells;
        centCells.push_back(centreCell);
        CaloCell* upPhiCell = NextPhiCell(centreCell,true,scells,idHelper);
        CaloCell* downPhiCell = NextPhiCell(centreCell,false,scells,idHelper);
        CaloCell* energeticPhiCell;
        // If the phi width is 2, the most energetic neighbour is chosen (defaulting to the 'down' side)
        // If the phi width is 3, both neighbours are added
        if (phiWidth > 1){  
		if (CaloCellET(upPhiCell, digitScale, digitThresh) > CaloCellET(downPhiCell, digitScale, digitThresh)) energeticPhiCell = upPhiCell;
                else energeticPhiCell = downPhiCell;
                if (phiWidth == 2) addOnce(energeticPhiCell, centCells); //centCells.push_back(energeticPhiCell);
                else if (phiWidth == 3){
                        addOnce(upPhiCell, centCells); //centCells.push_back(upPhiCell);
			addOnce(downPhiCell, centCells); //centCells.push_back(downPhiCell);
                }
                else if (phiWidth > 3) msg << MSG::WARNING << "ERROR: phiWidth not 2 or 3!!!" << endreq;
        }
        // The actual cluster is initialised
        std::vector<CaloCell*> fullClus;
        // The L1&2 SCs are added that match the full width
        for (auto ithL2Cell : L2cells){
          fullClus.push_back(ithL2Cell);
          fromLayer2toLayer1(scells, ithL2Cell, fullClus, idHelper);
        }
        // The L0&3 SCs are added that match the central L2 cells
        for (auto ithL2CentCell : centCells){
          addOnce(fromLayer2toPS(scells, ithL2CentCell, idHelper),fullClus);
          addOnce(fromLayer2toLayer3(scells, ithL2CentCell, idHelper),fullClus);
        }
        return fullClus;
}

double TrigT1CaloBaseFex::sumVectorET(const std::vector<CaloCell*> &inputVector, float digitScale, float digitThreshold){
	double TotalET=0.0;
        for (auto ithCell : inputVector){
          if (ithCell!=NULL) TotalET += CaloCellET(ithCell, digitScale, digitThreshold);
        }
        return TotalET;
}

bool TrigT1CaloBaseFex::checkDig(float EM_ET, float digitScale, float digitThresh){
	MsgStream msg(msgSvc(), name());
	if (EM_ET == 0 || digitScale == 0) return true;
        else {
		int div = EM_ET / digitScale;
		if (div * digitScale == EM_ET) return true;
		else {
			msg << MSG::WARNING << "ET = " << EM_ET << ", digitThresh = " << digitThresh << " digitScale = " << digitScale << " div = " << div << " " << " -> div * digitScale" << endreq;
			return false;
		}
	}
}

double TrigT1CaloBaseFex::EMClusET(CaloCell* centreCell, int etaWidth, int phiWidth, const CaloCellContainer* scells, const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh){
	MsgStream msg(msgSvc(), name());
	// Sums the ET of the vector
        std::vector<CaloCell*> fullClus = TDR_Clus(centreCell, etaWidth, phiWidth, scells, idHelper, digitScale,digitThresh);
        double EMcomp = sumVectorET(fullClus, digitScale, digitThresh);  
        bool EMcheck = checkDig(EMcomp, digitScale, digitThresh);
        if (!EMcheck) msg << MSG::WARNING << "EMcomp not digitised  " << EMcomp << "  " << digitScale << "  " << digitThresh << endreq;
        double total = EMcomp;
        return total;
}

bool TrigT1CaloBaseFex::SameTT(CaloCell* inputCell1, CaloCell* inputCell2, const CaloCell_SuperCell_ID* &idHelper){
	const Identifier ID1 = inputCell1->ID();
        int phi1 = idHelper->phi(ID1);
        const Identifier ID2 = inputCell2->ID();
        int phi2 = idHelper->phi(ID2);
        if (phi1 != phi2) return false;
        int pn1 = idHelper->pos_neg(ID1);
        int pn2 = idHelper->pos_neg(ID2);
        if (pn1 != pn2) return false;      
        // Is barrel
        if (abs(pn1)==1){
		int reg1 = idHelper->region(ID1);
                int reg2 = idHelper->region(ID2);
                if (reg1 != reg2) return false;
                int etaDiv1 = idHelper->eta(ID1)/4;
                int etaDiv2 = idHelper->eta(ID2)/4;
                if (etaDiv1 == etaDiv2) return true;
                else return false;
        }
        // OW
        else if (abs(pn1)==2){
                int reg1 = idHelper->region(ID1);
                int reg2 = idHelper->region(ID2);
                int eta1 = idHelper->eta(ID1);
                int eta2 = idHelper->eta(ID2);
                if ((reg1 == 0 && reg2 == 1 && eta2 < 3 ) || (reg2 == 0 && reg1 == 1 && eta1 < 3 )) return true;
                else {
                        if (reg1 != reg2) return false;
                        int etaDiv1 = (idHelper->eta(ID1) - 3)/4;
                        int etaDiv2 = (idHelper->eta(ID2) - 3)/4;
                        if (etaDiv1 == etaDiv2) return true;
                        else return false;   
                }
        }  
        else return false;
}

double TrigT1CaloBaseFex::L2clusET(CaloCell* centreCell, int etaWidth, int phiWidth, const CaloCellContainer* scells, const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh){
	return sumVectorET(L2cluster(centreCell, etaWidth, phiWidth, scells, idHelper, digitScale, digitThresh), digitScale, digitThresh);
}

double TrigT1CaloBaseFex::REta(CaloCell* centreCell, int etaWidth1, int phiWidth1, int etaWidth2, int phiWidth2, const CaloCellContainer* scells, const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh){
	MsgStream msg(msgSvc(), name());
	// Check windows sizes are right way round
        if (etaWidth1 > etaWidth2) msg << MSG::WARNING << "REta ERROR: eta1 = " << etaWidth1 << ", eta2 = " << etaWidth2 << endreq;
        if (phiWidth1 > phiWidth2) msg << MSG::WARNING << "Rphi ERROR: phi1 = " << phiWidth1 << ", phi2 = " << phiWidth2 << endreq;
        // Finds ET of windows
        double inner_ET = L2clusET(centreCell, etaWidth1, phiWidth1, scells, idHelper, digitScale, digitThresh);
        double outer_ET = L2clusET(centreCell, etaWidth2, phiWidth2, scells, idHelper, digitScale, digitThresh);
        // Find normal value of REta & changes it to my version
        double normal_REta;
        if (inner_ET != 0. && outer_ET==0.) normal_REta = 0.;
        else if (inner_ET==0.) normal_REta = 0.;
        else normal_REta = inner_ET / outer_ET;
        if (normal_REta < 0) normal_REta = 0.;
        double my_REta = 1-normal_REta;
        return my_REta;
}

double TrigT1CaloBaseFex::TT_phi(const xAOD::TriggerTower* &inputTower){
	MsgStream msg(msgSvc(), name());
	if (inputTower == NULL){
		msg << MSG::WARNING << "Tower is NULL in phi transformation!" << endreq;
		return 0.;
	}
	else {
		double phi = inputTower->phi();
		if (phi > M_PI) phi = phi - 2*M_PI;
		return phi;
	} 
}

double TrigT1CaloBaseFex::dR(double eta1, double phi1, double eta2, double phi2){
	double etaDif = eta1 - eta2;
	double phiDif = fabs(phi1 - phi2);
	if (phiDif > M_PI) phiDif = phiDif - (2*M_PI);
	double result = sqrt(pow(etaDif,2)+pow(phiDif,2));
	return result;
}

const xAOD::TriggerTower* TrigT1CaloBaseFex::matchingHCAL_TT(CaloCell* &inputCell, const xAOD::TriggerTowerContainer* &TTContainer){
	MsgStream msg(msgSvc(), name());
	std::vector<const xAOD::TriggerTower*> matchingTTs;
        if (TTContainer==NULL) return NULL;
        if (TTContainer->size()==0) return NULL;
        if (inputCell==NULL) return NULL;
        for (auto ithTT : *TTContainer){
          if (ithTT->sampling()==1){
		  float ithTT_eta = ithTT->eta();
                  float ithTT_phi = TT_phi(ithTT);
                  float ithdR = dR(ithTT_eta, ithTT_phi, inputCell->eta(), inputCell->phi());
                  if (ithdR < 0.05) matchingTTs.push_back(ithTT);
          }
        }
        if (matchingTTs.size()==1) return matchingTTs[0];
        else if (matchingTTs.size()!=0){	
		msg << MSG::WARNING << "ERROR: More than one matching HCAL TT!!! (Returned Null)" << endreq;
	}
	return NULL;
}

CaloCell* TrigT1CaloBaseFex::matchingHCAL_LAr(CaloCell* &inputCell, const CaloCellContainer* &SCContainer, const  CaloCell_SuperCell_ID* &idHelper){
	MsgStream msg(msgSvc(), name());
	std::vector<CaloCell*> matchingCells;
        if (inputCell==NULL) return NULL;
        for (auto ithSC : *SCContainer){
          Identifier ithID = ithSC->ID();
          int ithSub_calo = idHelper->sub_calo(ithID);
          if (ithSub_calo == 1){
                  double ithdR = dR(inputCell->eta(), inputCell->phi(), ithSC->eta(), ithSC->phi());
                  if (ithdR < 0.05) matchingCells.push_back(ithSC);
          }
        }
        if (matchingCells.size()==1) return matchingCells[0];
        else if (matchingCells.size()==0){
                msg << MSG::WARNING << "ERROR: No match betweem LAr ECAL SC and LAr HCAL SC!!! Input coords: " << inputCell->eta() << ", " << inputCell->phi() << endreq;
        }
        else if (matchingCells.size()!=0) {
                msg << MSG::WARNING << "ERROR: More than one matching LAr HCAL SC!!! (Returned Null)" << endreq;
                msg << MSG::WARNING << "Input cell coords: " << inputCell->eta() << " x " << inputCell->phi() << endreq;
                for (auto ithMatch : matchingCells){
                  msg << MSG::WARNING << "    " << ithMatch->eta() << " x " << ithMatch->phi() << ", dR = " << dR(inputCell->eta(), inputCell->phi(), ithMatch->eta(), ithMatch->phi()) << endreq;
                }
        }
        return NULL;
}

double TrigT1CaloBaseFex::TT_ET(const xAOD::TriggerTower* &inputTower){
	MsgStream msg(msgSvc(), name());
	if (inputTower == NULL){
		msg << MSG::WARNING << "Tower is NULL!" << endreq;
		return 0.;
        }
        else if (inputTower->cpET() < 0.) return 0;
	else {
		return 500*inputTower->cpET();
	}
}

double TrigT1CaloBaseFex::HadronicET(std::vector<CaloCell*> inputVector, const CaloCellContainer* scells, const xAOD::TriggerTowerContainer* &TTContainer, const CaloCell_SuperCell_ID* idHelper,float digitScale, float digitThresh){
	// Finds the HCAL SCs & TTs matching the input cluster
        std::vector<CaloCell*> HCAL_LAr_vector;
        std::vector<const xAOD::TriggerTower*> HCAL_TT_vector;
        for (auto ithCell : inputVector){
          if (fabs(ithCell->eta())<1.5){
                  const xAOD::TriggerTower* tempTT = matchingHCAL_TT(ithCell, TTContainer);
                  if (tempTT != NULL) HCAL_TT_vector.push_back(tempTT);
          }
          else if (fabs(ithCell->eta())<2.5){
                  CaloCell* tempLArHad = matchingHCAL_LAr(ithCell, scells, idHelper);
                  if (tempLArHad != NULL) HCAL_LAr_vector.push_back(tempLArHad);
          }
        }
        // Sums the ET in the HCAL
        double HadET = 0.;
        for (auto ithTT : HCAL_TT_vector) {HadET += TT_ET(ithTT);}
        for (auto ithSC : HCAL_LAr_vector) {HadET += CaloCellET(ithSC, digitScale, digitThresh);}
        return HadET;
}

double TrigT1CaloBaseFex::RHad(CaloCell* centreCell, int etaWidth, int phiWidth, const CaloCellContainer* scells, const xAOD::TriggerTowerContainer* &TTContainer, const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh, float &HadET){
	MsgStream msg(msgSvc(), name());
	std::vector<CaloCell*> fullClus = TDR_Clus(centreCell, etaWidth, phiWidth, scells, idHelper, digitScale, digitThresh);
        double EMcomp = sumVectorET(fullClus, digitScale, digitThresh);
        double HCALcomp = HadronicET(L2cluster(centreCell, etaWidth, phiWidth, scells, idHelper, digitScale, digitThresh), scells, TTContainer, idHelper, digitScale, digitThresh);
	HadET = HCALcomp/1e3;
        double result = HCALcomp/(EMcomp+HCALcomp);
        if (result < 0. || result > 1.){
                msg << MSG::WARNING << "RHAD ERROR -> " << etaWidth << " * " << phiWidth << endreq;
                msg << MSG::WARNING << "fullClus count = " << fullClus.size() << ", EMcomp = " << EMcomp << ", HCALcomp = " << HCALcomp << endreq;
        }
        return result;
}

double TrigT1CaloBaseFex::L1Width(CaloCell* centreCell, int etaWidth, int phiWidth, const CaloCellContainer* scells, const CaloCell_SuperCell_ID* idHelper, float digitScale, float digitThresh){
	// Finds a L2 cluster and the corresponding L1 cells
        std::vector<CaloCell*> L2cells = L2cluster(centreCell, etaWidth, phiWidth, scells, idHelper,digitScale, digitThresh);
        std::vector<CaloCell*> frontLayerCells;
        for (auto ithL2Cell : L2cells){
          fromLayer2toLayer1(scells, ithL2Cell,frontLayerCells, idHelper);
        }
        float frontCentreEta;
        // Finds the most energetic L1 cell
        float largET=-1;
        CaloCell* largCell = NULL;
        for (auto ithL1Cell : frontLayerCells){
          if (CaloCellET(ithL1Cell, digitScale, digitThresh) > largET){
                  largET = CaloCellET(ithL1Cell, digitScale, digitThresh);
                  largCell = ithL1Cell;
          }
        }
        if (!(largET > 0.)) return 1.;
        frontCentreEta = largCell->eta();
        // Finds the 'width' for the cluster, applying defaults
        float sumET = 0, sumET_Eta2=0;
        for (auto ithCell : frontLayerCells){
          float ithET = CaloCellET(ithCell, digitScale, digitThresh);
          float ithEta = ithCell->eta();
          sumET_Eta2 += ithET*pow(ithEta-frontCentreEta,2);
          sumET += ithET;
        }
        float result;
        if (sumET > 0.) result = sqrt(sumET_Eta2/sumET);
        else return 1.;
        return result;
}

std::vector<std::vector<float>> TrigT1CaloBaseFex::looseAlg(const CaloCellContainer* SCs, const xAOD::TriggerTowerContainer* TTs, const CaloCell_SuperCell_ID* idHelper){
	MsgStream msg(msgSvc(), name());
        std::vector< std::vector<float>> result;
        // Loops through and find L2 SCs that are local maxes and adds to list of local maxes if cluster ET is at least 10GeV
        std::vector<CaloCell*> potentialCentres;
        for (auto ithCell : *SCs){
          Identifier ithID = ithCell->ID();
          int ithLayer = idHelper->sampling(ithID);
          if (ithLayer != 2) continue;
          int ithSub_calo = idHelper->sub_calo(ithID);
          if (ithSub_calo != 0) continue;
          int ithPos_neg = idHelper->pos_neg(ithID);
          bool inEfexCoverage = false;
          if (abs(ithPos_neg) < 3) inEfexCoverage = true;
          if (!inEfexCoverage) continue;
          bool passedLocalMax = localMax(SCs, ithCell, idHelper, m_nominalDigitization, m_nominalNoise_thresh);
          if (!passedLocalMax) continue;
          float clustET = EMClusET(ithCell, m_etaWidth_TDRCluster, m_phiWidth_TDRCluster, SCs, idHelper, m_nominalDigitization, m_nominalNoise_thresh)/1000.;
          if (clustET < m_clustET_looseAlg_thresh) continue;
          potentialCentres.push_back(ithCell);
        }
        // Looops through the local maxes and skips the less energetic ones that belong to the same TT
        for (auto ithCell : potentialCentres){
          bool useSC = true;
          for (auto jthCell : potentialCentres){
            if (jthCell == ithCell) continue;
            if (!SameTT(ithCell, jthCell, idHelper)) continue;
            float ithEt = CaloCellET(ithCell, m_nominalDigitization, m_nominalNoise_thresh);
            float jthEt = CaloCellET(jthCell, m_nominalDigitization, m_nominalNoise_thresh);
            if (ithEt > jthEt) continue;
            if (ithEt == jthEt && ithCell->eta() < jthCell->eta()) continue;
            useSC = false; 
          }
          if (useSC){
	    float HadET = -999;
            float ithEta = ithCell->eta();
            float ithPhi = ithCell->phi();
            float clustET = EMClusET(ithCell, m_etaWidth_TDRCluster, m_phiWidth_TDRCluster, SCs, idHelper, m_nominalDigitization, m_nominalNoise_thresh)/1000.;
            float ithREta = REta(ithCell, m_etaWidth_REtaIsolation_num, m_phiWidth_REtaIsolation_num, m_etaWidth_REtaIsolation_den, m_phiWidth_REtaIsolation_den, SCs, idHelper, m_nominalDigitization, m_nominalNoise_thresh);    
            float ithRHad = RHad(ithCell, m_etaWidth_RHadIsolation, m_phiWidth_RHadIsolation, SCs, TTs, idHelper, m_nominalDigitization, m_nominalNoise_thresh, HadET);
            float ithL1Width = L1Width(ithCell, m_etaWidth_wstotIsolation, m_phiWidth_wstotIsolation, SCs, idHelper, m_nominalDigitization, m_nominalNoise_thresh);
	    float L2ClusterET33 = L2clusET(ithCell, 3, 3, SCs, idHelper, m_nominalDigitization, m_nominalNoise_thresh)/1e3;
	    float L2ClusterET37 = L2clusET(ithCell, 7, 3, SCs, idHelper, m_nominalDigitization, m_nominalNoise_thresh)/1e3;
            std::vector<float> ithResult = {ithEta, ithPhi, clustET, ithREta, ithRHad, ithL1Width, HadET, L2ClusterET33, L2ClusterET37};
            result.push_back(ithResult);
            }
          }
        return result;
}

std::vector<std::vector<float>> TrigT1CaloBaseFex::baselineAlg(const CaloCellContainer* scells, const xAOD::TriggerTowerContainer* TTs, const CaloCell_SuperCell_ID* idHelper){
	MsgStream msg(msgSvc(), name());
  	std::vector<std::vector<float>> looseClusters = looseAlg(scells, TTs, idHelper);
	if (! m_apply_BaseLineCuts) return looseClusters;
  	else {
		std::vector<std::vector<float>> baselineClusters;
		for (auto ithCluster : looseClusters){
		  if (ithCluster[2] < m_clustET_thresh) continue;
  		  else if (ithCluster[2] > m_clustET_NoIso_thresh) baselineClusters.push_back(ithCluster);
  		  if (ithCluster[3] > m_REta_thresh || ithCluster[4] > m_RHad_thresh) continue;
  		  if (fabs(ithCluster[0]) > m_eta_dropL1Width || ithCluster[5] < m_L1Width_thresh) baselineClusters.push_back(ithCluster);
		}
	return baselineClusters;
  	}
}
