/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * NAME : 	TrigT1CaloBaseFex.cxx
 * PACKAGE : 	Trigger/L1CaloUpgrade/TrigT1CaloEFex
 *
 * AUTHOR : 	Denis Oliveira Damazio
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

TrigT1CaloBaseFex::TrigT1CaloBaseFex( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm (name, pSvcLocator), m_deta(0.08), m_dphi(0.11), m_detaTT(0.125), m_dphiTT(0.15)  {
	declareProperty("CleanCellContainer", m_useProvenance=false);
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
	if ( evtStore()->retrieve(scells_from_sg,"SCell").isFailure() ){
		msg << MSG::WARNING << "did not find cell container" << endreq;
		return StatusCode::SUCCESS;
	}
	if ( evtStore()->retrieve(TTs,"xAODTriggerTowers").isFailure() ){
		msg << MSG::WARNING << "did not find TT container" << endreq;
		return StatusCode::SUCCESS;
	}

	if ( m_useProvenance ) {
		if ( !scells ) scells = new CaloCellContainer();
        	for(auto scell : *scells_from_sg) {
			if ( scell->provenance() & 0x40 ) scells->push_back( scell );
		}
	}
	else {
		if ( !scells ) scells = new CaloCellContainer();
        	for(auto scell : *scells_from_sg) {
			scells->push_back( scell );
		}
	}
        for(auto scell : *scells) {
		if ( scell->et() < 3e3 ) continue;
		msg << MSG::DEBUG << "scell : " << scell->et() << " " << scell->eta() << " " << scell->phi() << endreq;
	}
        for(auto TT : *TTs) {
		if ( TT->pt() < 3 ) continue;
		msg << MSG::DEBUG << "TT : " << TT->pt() << " " << TT->eta() << " " << TT->phi() << endreq;
	}
	return StatusCode::SUCCESS;
}

StatusCode TrigT1CaloBaseFex::getContainers(CaloCellContainer*& scells, const xAOD::TriggerTowerContainer*& TTs, float etThresholdGeV){

        MsgStream msg(msgSvc(), name());
        const CaloCellContainer* scells_from_sg;
        if ( evtStore()->retrieve(scells_from_sg,"SCell").isFailure() ){
                msg << MSG::WARNING << "did not find cell container" << endreq;
                return StatusCode::SUCCESS;
        }
        if ( evtStore()->retrieve(TTs,"xAODTriggerTowers").isFailure() ){
                msg << MSG::WARNING << "did not find TT container" << endreq;
                return StatusCode::SUCCESS;
        }

        if ( m_useProvenance ) {
                if ( !scells ) scells = new CaloCellContainer();
                for(auto scell : *scells_from_sg) {
                        if ( scell->provenance() & 0x40 ) scells->push_back( scell );
                }
        }
        else {
                if ( !scells ) scells = new CaloCellContainer();
                for(auto scell : *scells_from_sg) {
                        scells->push_back( scell );
                }
        }
	// TODO: add more quality cuts? ...

        for(auto scell : *scells) {
                if ( scell->et() < etThresholdGeV*1e3 ) continue;
                msg << MSG::DEBUG << "scell : " << scell->et() << " " << scell->eta() << " " << scell->phi() << endreq;
        }
        for(auto TT : *TTs) {
                if ( TT->pt() < etThresholdGeV ) continue;
                msg << MSG::DEBUG << "TT : " << TT->pt() << " " << TT->eta() << " " << TT->phi() << endreq;
        }
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


void TrigT1CaloBaseFex::findCellsAbove(const CaloCellContainer* scells, const float& Thr, std::vector<CaloCell*>& out) {
	out.clear();
	for(auto scell : *scells) {
		if ( scell->et() < Thr )  continue;
		out.push_back(scell);
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
	if ( cell->caloDDE()->getSampling() >= 8 ) return false;
	float cellpt = 1.0001*cell->et(); //make sure you don't get thecell itself
	for(auto scell : scells){ 
		if ( scell->caloDDE()->getSampling() >= 8 ) continue;
		if ( scell->ID() == cell->ID()  ) continue;
		if ( scell->et() > cellpt ) return false;
	}
	return true;
}

float TrigT1CaloBaseFex::sumEmCells(const std::vector<CaloCell*>& scells) const {
	float totalSum = 0.0;
	for(auto scell : scells) {
		if ( scell->caloDDE()->getSampling() <8 ) totalSum+= scell->energy();
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
             if ( (scell->caloDDE()->getSampling() <8) || ( scell->caloDDE()->getSampling()>=20) ) continue;
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

