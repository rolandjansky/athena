/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigCaloValidation/TrigFEBValidation.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrigT2CaloCommon/ITrigDataAccess.h"
#include "CaloEvent/CaloCellContainer.h"
#include "LArIdentifier/LArIdManager.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "GaudiKernel/IToolSvc.h"
#include "TrigCaloRec/IAlgToolEFCalo.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloSampling.h"



TrigFEBValidation::TrigFEBValidation(const std::string& name,
	ISvcLocator* pSvcLocator) : Algorithm(name,pSvcLocator),
//	m_data("TrigDataAccess",this,"TrigDataAccess"),
	m_onlineHelper(0),
	m_cablingSvc("LArCablingService"),
	m_noisetool("CaloNoiseTool/CaloNoiseToolDefault"),
	m_threshold(100.0),
	m_nsigma(5.0),
	m_useloadfullcoll(false),
	m_difflimit(0.)
{

//	declareProperty ("TrigDataAccess", m_data);
	declareProperty("NoiseTool",m_noisetool);
	declareProperty ("threshold", m_threshold);
	declareProperty ("sigma", m_nsigma);
	declareProperty ("useLoadFullCollection", m_useloadfullcoll);
	declareProperty ("DiffLimit", m_difflimit);
	declareProperty ("TCRTools", m_tcrAlgTools);
	// Nothing for the moment
	return;

}


TrigFEBValidation::~TrigFEBValidation() {}

StatusCode TrigFEBValidation::initialize(){

	m_log = new MsgStream(messageService(),name());
	(*m_log) << MSG::INFO << "Initializing" << endreq;

	if ( service("StoreGateSvc",m_storeGate).isFailure() ){
		(*m_log) << MSG::ERROR << "Problems to get StoreGateSvc"
			<< endreq;
		return StatusCode::FAILURE;
	}

//	if ( m_data.retrieve().isFailure() ) {
	if ( toolSvc()->retrieveTool("TrigDataAccess",m_data).isFailure() ){
    		(*m_log) << MSG::ERROR << "Failed to retrieve helper tools: " << m_data << endreq;
		return StatusCode::FAILURE;
	} else {
		(*m_log) << MSG::INFO << "Retrieved " << m_data << endreq;
	}

	if ( m_cablingSvc.retrieve().isFailure() ) {
    		(*m_log) << MSG::ERROR << "Failed to retrieve helper tools: " << m_cablingSvc << endreq;
		return StatusCode::FAILURE;
	} else {
		(*m_log) << MSG::INFO << "Retrieved " << m_cablingSvc << endreq;
	}

	ServiceHandle<StoreGateSvc> detStore("DetectorStore",name());
	if ( detStore.retrieve().isFailure() ) {
		(*m_log) << MSG::ERROR << "Unable to retrieve DetectorStore" << endreq;
		return StatusCode::FAILURE;
	}
	const LArIdManager* larMgr;
	if ( (detStore->retrieve(larMgr)).isFailure() ) {
		(*m_log) << MSG::ERROR << "Unable to retrieve LArIdManager from DetectorStore" << endreq;
		return StatusCode::FAILURE;
	} else {
		(*m_log) << MSG::DEBUG << "Successfully retrieved LArIdManager from DetectorStore" << endreq;
	}
	m_onlineHelper = larMgr->getOnlineID();
	if (!m_onlineHelper) {
		(*m_log) << MSG::ERROR << "Could not access LArOnlineID helper" << endreq;
		return StatusCode::FAILURE;
	} else {
	(*m_log) << MSG::DEBUG << "Successfully accessed LArOnlineID helper" << endreq;
	}

   if (m_noisetool.retrieve().isFailure()) {
     (*m_log) << MSG::ERROR << "Could not retrieve CaloNoiseTool " << m_noisetool << endreq;
     return StatusCode::FAILURE;
   }
   else
     (*m_log) << MSG::DEBUG << "Successfully retrieved CaloNoiseTool " << endreq;

        if ( m_tcrAlgTools.retrieve().isFailure() ) {
                (*m_log) << MSG::ERROR << "Failed to retrieve helper tools: " << m_tcrAlgTools << endreq;
                return StatusCode::FAILURE;
        } else {
                (*m_log) << MSG::INFO << "Retrieved " << m_tcrAlgTools << endreq;
        }


	return StatusCode::SUCCESS;
}


StatusCode TrigFEBValidation::execute(){

	const DataHandle<CaloCellContainer> AllCalo;
	if ( m_storeGate->retrieve(AllCalo,"AllCalo").isFailure() ){
		(*m_log) << MSG::DEBUG << "No Calo Cell Container found"
                        << endreq;
		return StatusCode::SUCCESS;
	}
	(*m_log) << MSG::DEBUG << "Got container "; 
	(*m_log) << "Size : " << AllCalo->size() << endreq;

	CaloCellContainer* pCaloCellContainer
                = new CaloCellContainer(SG::VIEW_ELEMENTS);
        ToolHandleArray<IAlgToolEFCalo>::iterator itrtcr = m_tcrAlgTools.begin();
        ToolHandleArray<IAlgToolEFCalo>::iterator endtcr = m_tcrAlgTools.end();
        double etamin, etamax,phimin, phimax;
        etamin=-4.8;
        etamax=4.8;
        for (; itrtcr!=endtcr; ++itrtcr) {

        (*m_log) << MSG::VERBOSE << "Tool name : "
                << (*itrtcr).name() << endreq;
        phimin=-M_PI;
        phimax=M_PI;
        // Fix for stupid RS problem
        if ( (*itrtcr).name().find("Tile")!=std::string::npos ){
                phimin=0;
                phimax=2*M_PI;
        }
        StatusCode sc;
        if((*itrtcr).name() == "FullCaloCellContMaker") {
                sc= (*itrtcr)->execute(*pCaloCellContainer);
        } else {
                sc = (*itrtcr)->execute(*pCaloCellContainer,etamin,etamax,phimin,phimax);
        }
        if ( sc.isFailure() ) {
                (*m_log) << MSG::ERROR << "Problem with filling the cont"
                        << endreq;
        }
        } // End of loop over tools

	CaloCellContainer::const_iterator beg = pCaloCellContainer->begin();
	CaloCellContainer::const_iterator end = pCaloCellContainer->end();

	std::map<HWIdentifier,LArFebEnergy> larfebmap;
	std::map<HWIdentifier,LArFebEnergy> larfebmap_fcal;
	std::map<HWIdentifier,LArFebEnergy> larfebmap_hec, larfebmap_em;
	std::map<HWIdentifier,LArFebEnergy>::iterator it;

	for(; beg!=end ; ++beg){
		CaloCell* cell = (*beg);
		if ( cell && cell->caloDDE()
			&& cell->caloDDE()->getSubCalo() < CaloCell_ID::TILE ){
			Identifier idh = cell->ID();
                        const CaloDetDescrElement *detdescr = cell->caloDDE();
			//IdentifierHash idh = cell->caloDDE()->calo_hash();
			HWIdentifier channel_hw = m_cablingSvc->createSignalChannelID(idh);
			HWIdentifier feb_hw = m_onlineHelper->feb_Id(channel_hw);
                        Identifier32::value_type feb_hw_n = feb_hw.get_identifier32().get_compact();
			if ( m_nsigma != 0 )
			m_threshold = m_nsigma * m_noisetool->totalNoiseRMS(cell->caloDDE(),cell->gain());
                        //std::cout << "cell energy = " << cell->energy() << std::endl;
			if ( cell->energy() > m_threshold ) {
				int ienergy = (int) floorf(cell->energy()/64.0);
				float energy = (float)(64.0*ienergy);
				float et, ex, ey, ez;
				et = energy * cell->caloDDE()->sinTh();
				ex= et * cell->caloDDE()->cosPhi();
				ey= et * cell->caloDDE()->sinPhi();
				ez= et * cell->caloDDE()->cotTh();
				std::cout << "just test " << cell->caloDDE()->sinTh() << " " << 1./cosh ( cell->eta() ) << " " << cell->caloDDE()->cosPhi() << " " << cos ( cell->phi() ) << " " << cell->caloDDE()->sinTh()* cell->caloDDE()->cotTh() << " " << tanh(cell->eta() ) << " " << m_threshold << std::endl;;
#ifndef NDEBUG
				if ( energy > 0 ) {
					std::cout << " All cells that compose feb : ";
					std::cout << std::hex;
					std::cout << feb_hw << " ";
					std::cout << idh << " ";
					std::cout << channel_hw << " ";
					std::cout << std::dec;
					std::cout << energy << " ";
					std::cout << cell->energy() << " ";
					std::cout << et << " ";
					std::cout << ex << " ";
					std::cout << ey << " ";
					std::cout << ez << std::endl;
				}
#endif
				if ( (it = larfebmap.find(feb_hw)) != larfebmap.end() ){
					// fill
					/*std::cout << "filling created FEB " << std::endl;
					std::cout << "with size = " << larfebmap.size() << std::endl;*/
					LArFebEnergy* larfebenergy =
						&((*it).second);
					double tmpex = larfebenergy->getFebEx();
					double tmpey = larfebenergy->getFebEy();
					double tmpez = larfebenergy->getFebEz();
                                        /*std::cout << "current ex = " << tmpex << std::endl;
                                        std::cout << "current ey = " << tmpey << std::endl;
                                        std::cout << "current ez = " << tmpez << std::endl;*/
					larfebenergy->setFebEx(
					  ex + tmpex);
					larfebenergy->setFebEy(
					  ey + tmpey);
					larfebenergy->setFebEz(
					  ez + tmpez);
				} else {
					LArFebEnergy larfebenergy =
					  LArFebEnergy(feb_hw_n,ex,ey,ez,0.);
					larfebmap.insert(std::pair<HWIdentifier,LArFebEnergy>(feb_hw,larfebenergy));
				}
                                CaloCell_ID::CaloSample s = detdescr->getSampling();
	                        std::map<HWIdentifier,LArFebEnergy>::iterator myit;
                                switch(s){
                                  case CaloCell_ID::PreSamplerB:
                                  case CaloCell_ID::PreSamplerE:
                                  case CaloCell_ID::EMB1:
                                  case CaloCell_ID::EMB2:
                                  case CaloCell_ID::EMB3:
                                  case CaloCell_ID::EME1:
                                  case CaloCell_ID::EME2:
                                  case CaloCell_ID::EME3:
	                            //std::map<HWIdentifier,LArFebEnergy>::iterator myit;
                                    if( (myit = larfebmap_em.find(feb_hw)) != larfebmap_em.end() ){
                                        LArFebEnergy* larfebenergy =
                                                &((*myit).second);
                                        double tmpex = larfebenergy->getFebEx();
                                        double tmpey = larfebenergy->getFebEy();
                                        double tmpez = larfebenergy->getFebEz();
                                        larfebenergy->setFebEx(
                                          ex + tmpex);
                                        larfebenergy->setFebEy(
                                          ey + tmpey);
                                        larfebenergy->setFebEz(
                                          ez + tmpez);
                                    } else {
                                        LArFebEnergy larfebenergy =
                                          LArFebEnergy(feb_hw_n,ex,ey,ez,0.);
                                        larfebmap_em.insert(std::pair<HWIdentifier,LArFebEnergy>(feb_hw,larfebenergy));
                                    }
                                    break;
                                  case CaloCell_ID::HEC0:
                                  case CaloCell_ID::HEC1:
                                  case CaloCell_ID::HEC2:
                                  case CaloCell_ID::HEC3:
	                            //std::map<HWIdentifier,LArFebEnergy>::iterator myit;
                                    if( (myit = larfebmap_hec.find(feb_hw)) != larfebmap_hec.end() ){
                                        LArFebEnergy* larfebenergy =
                                                &((*myit).second);
                                        double tmpex = larfebenergy->getFebEx();
                                        double tmpey = larfebenergy->getFebEy();
                                        double tmpez = larfebenergy->getFebEz();
                                        larfebenergy->setFebEx(
                                          ex + tmpex);
                                        larfebenergy->setFebEy(
                                          ey + tmpey);
                                        larfebenergy->setFebEz(
                                          ez + tmpez);
                                    } else {
                                        LArFebEnergy larfebenergy =
                                          LArFebEnergy(feb_hw_n,ex,ey,ez,0.);
                                        larfebmap_hec.insert(std::pair<HWIdentifier,LArFebEnergy>(feb_hw,larfebenergy));
                                    }
                                    break;
                                  case CaloCell_ID::FCAL0:
                                  case CaloCell_ID::FCAL1:
                                  case CaloCell_ID::FCAL2:
	                            //std::map<HWIdentifier,LArFebEnergy>::iterator myit;
                                    if( (myit = larfebmap_fcal.find(feb_hw)) != larfebmap_fcal.end() ){
                                        LArFebEnergy* larfebenergy =
                                                &((*myit).second);
                                        double tmpex = larfebenergy->getFebEx();
                                        double tmpey = larfebenergy->getFebEy();
                                        double tmpez = larfebenergy->getFebEz();
                                        larfebenergy->setFebEx(
                                          ex + tmpex);
                                        larfebenergy->setFebEy(
                                          ey + tmpey);
                                        larfebenergy->setFebEz(
                                          ez + tmpez);
                                    } else {
                                        LArFebEnergy larfebenergy =
                                          LArFebEnergy(feb_hw_n,ex,ey,ez,0.);
                                        larfebmap_fcal.insert(std::pair<HWIdentifier,LArFebEnergy>(feb_hw,larfebenergy));
                                    }
                                    break;
                                  case CaloCell_ID::TileBar0:
                                  case CaloCell_ID::TileBar1:
                                  case CaloCell_ID::TileBar2:
                                  case CaloCell_ID::TileExt0:
                                  case CaloCell_ID::TileExt1:
                                  case CaloCell_ID::TileExt2:
                                  case CaloCell_ID::TileGap1:
                                  case CaloCell_ID::TileGap2:
                                  case CaloCell_ID::TileGap3:
                                  case CaloCell_ID::MINIFCAL0:
                                  case CaloCell_ID::MINIFCAL1:
                                  case CaloCell_ID::MINIFCAL2:
                                  case CaloCell_ID::MINIFCAL3:
                                  case CaloCell_ID::Unknown:
                                     break;
                                }
			}
		} // End of basic cell checks
		
	}
	(*m_log) << MSG::INFO << " Number of Offline FEBs : "
		<< larfebmap.size() << endreq;
	(*m_log) << MSG::DEBUG << "COMPARISON " << endreq;
	//LArFebEnergyCollection m_larfebcol;
        m_larfebcol.clear();
        m_larfebcol_hec.clear();
        m_larfebcol_em.clear();
        m_larfebcol_fcal.clear();
	//m_larfebcol.clear(SG::VIEW_ELEMENTS);
	//LArFebEnergyCollection::const_iterator tit,tbegin,tend;
	//LArFebEnergyCollection::const_iterator tit;
        bool prepare=true;
        if(m_useloadfullcoll){
          if(m_data->LoadFullCollections(tbegin,tend,TTEM,prepare).isFailure()){
             (*m_log) << MSG::ERROR << "Problems reading LoadFullCollection"
                    << endreq;
		return StatusCode::FAILURE;
          }
        } else {
        double etamin = -4.8;
        double etamax = 4.8;
        double phimin = -M_PI;
        double phimax = M_PI;
	//if ( m_data->LoadFullCollections(tbegin,tend,TTEM).isFailure() ) {
        m_data->RegionSelector(0,etamin,etamax,phimin,phimax, TTEM);
	//LArFebEnergyCollection::const_iterator titem0,tbeginem0,tendem0;
	if ( m_data->LoadCollections(tbegin,tend,0).isFailure() ) {
		(*m_log) << MSG::ERROR << "Problems to read FEB info"
			<< endreq;
		return StatusCode::FAILURE;
	}
	for(LArFebEnergyCollection::const_iterator tit= tbegin; tit!=tend; ++tit) {
                LArFebEnergy *feb = new LArFebEnergy((*tit)->getFebId());
                feb->setFebEx((*tit)->getFebEx());
                feb->setFebEy((*tit)->getFebEy());
                feb->setFebEz((*tit)->getFebEz());
                (*m_log) << MSG::VERBOSE << "TTEM 0 FEB ID = " << feb->getFebId() << endreq;
                (*m_log) << MSG::VERBOSE << "TTEM 0 FEB Ex = " << feb->getFebEx() << endreq;
                (*m_log) << MSG::VERBOSE << "TTEM 0 FEB Ey = " << feb->getFebEy() << endreq;
                (*m_log) << MSG::VERBOSE << "TTEM 0 FEB Ez = " << feb->getFebEz() << endreq;
		m_larfebcol_em.push_back(feb);
	}
        m_data->RegionSelector(1,etamin,etamax,phimin,phimax,TTEM);
	//LArFebEnergyCollection::const_iterator tbeginem1;
	//LArFebEnergyCollection::const_iterator tendem1;
	if ( m_data->LoadCollections(tbegin,tend,1).isFailure() ) {
		(*m_log) << MSG::ERROR << "Problems to read FEB info"
			<< endreq;
		return StatusCode::FAILURE;
	}
	for(LArFebEnergyCollection::const_iterator tit = tbegin; tit!=tend; ++tit) {
                LArFebEnergy *feb = new LArFebEnergy((*tit)->getFebId());
                feb->setFebEx((*tit)->getFebEx());
                feb->setFebEy((*tit)->getFebEy());
                feb->setFebEz((*tit)->getFebEz());
                (*m_log) << MSG::VERBOSE << "TTEM 1 FEB ID = " << (*tit)->getFebId() << endreq;
                (*m_log) << MSG::VERBOSE << "TTEM 1 FEB Ex = " << (*tit)->getFebEx() << endreq;
                (*m_log) << MSG::VERBOSE << "TTEM 1 FEB Ey = " << (*tit)->getFebEy() << endreq;
                (*m_log) << MSG::VERBOSE << "TTEM 1 FEB Ez = " << (*tit)->getFebEz() << endreq;
		m_larfebcol_em.push_back(feb);
	}
        m_data->RegionSelector(2,etamin,etamax,phimin,phimax, TTEM);
	//LArFebEnergyCollection::const_iterator titem2,tbeginem2,tendem2;
	if ( m_data->LoadCollections(tbegin,tend,2).isFailure() ) {
		(*m_log) << MSG::ERROR << "Problems to read FEB info"
			<< endreq;
		return StatusCode::FAILURE;
	}
	for( LArFebEnergyCollection::const_iterator tit= tbegin; tit!=tend; ++tit) {
                LArFebEnergy *feb = new LArFebEnergy((*tit)->getFebId());
                feb->setFebEx((*tit)->getFebEx());
                feb->setFebEy((*tit)->getFebEy());
                feb->setFebEz((*tit)->getFebEz());
                (*m_log) << MSG::VERBOSE << "TTEM 2 FEB ID = " << (*tit)->getFebId() << endreq;
                (*m_log) << MSG::VERBOSE << "TTEM 2 FEB Ex = " << (*tit)->getFebEx() << endreq;
                (*m_log) << MSG::VERBOSE << "TTEM 2 FEB Ey = " << (*tit)->getFebEy() << endreq;
                (*m_log) << MSG::VERBOSE << "TTEM 2 FEB Ez = " << (*tit)->getFebEz() << endreq;
		m_larfebcol_em.push_back(feb);
	}
        m_data->RegionSelector(3,etamin,etamax,phimin,phimax, TTEM);
	//LArFebEnergyCollection::const_iterator titem3,tbeginem3,tendem3;
	if ( m_data->LoadCollections(tbegin,tend,3).isFailure() ) {
		(*m_log) << MSG::ERROR << "Problems to read FEB info"
			<< endreq;
		return StatusCode::FAILURE;
	}
	for(LArFebEnergyCollection::const_iterator  tit= tbegin; tit!=tend; ++tit) {
                LArFebEnergy *feb = new LArFebEnergy((*tit)->getFebId());
                feb->setFebEx((*tit)->getFebEx());
                feb->setFebEy((*tit)->getFebEy());
                feb->setFebEz((*tit)->getFebEz());
                (*m_log) << MSG::VERBOSE << "TTEM 3 FEB ID = " << (*tit)->getFebId() << endreq;
                (*m_log) << MSG::VERBOSE << "TTEM 3 FEB Ex = " << (*tit)->getFebEx() << endreq;
                (*m_log) << MSG::VERBOSE << "TTEM 3 FEB Ey = " << (*tit)->getFebEy() << endreq;
                (*m_log) << MSG::VERBOSE << "TTEM 3 FEB Ez = " << (*tit)->getFebEz() << endreq;
		m_larfebcol_em.push_back(feb);
	}
        m_data->RegionSelector(0,etamin,etamax,phimin,phimax, TTHEC);
	//LArFebEnergyCollection::const_iterator tithec0,tbeginhec0,tendhec0;
	if ( m_data->LoadCollections(tbegin,tend).isFailure() ) {
		(*m_log) << MSG::ERROR << "Problems to read FEB info"
			<< endreq;
		return StatusCode::FAILURE;
	}
	for(LArFebEnergyCollection::const_iterator  tit = tbegin; tit!=tend; ++tit) {
                LArFebEnergy *feb = new LArFebEnergy((*tit)->getFebId());
                feb->setFebEx((*tit)->getFebEx());
                feb->setFebEy((*tit)->getFebEy());
                feb->setFebEz((*tit)->getFebEz());
                (*m_log) << MSG::VERBOSE << "TTHEC 0 FEB ID = " << (*tit)->getFebId() << endreq;
                (*m_log) << MSG::VERBOSE << "TTHEC 0 FEB Ex = " << (*tit)->getFebEx() << endreq;
                (*m_log) << MSG::VERBOSE << "TTHEC 0 FEB Ey = " << (*tit)->getFebEy() << endreq;
                (*m_log) << MSG::VERBOSE << "TTHEC 0 FEB Ez = " << (*tit)->getFebEz() << endreq;
		//m_larfebcol.push_back(feb);
		m_larfebcol_hec.push_back(feb);
	}
        m_data->RegionSelector(0,etamin,etamax,phimin,phimax, FCALHAD);
	if ( m_data->LoadCollections(tbegin,tend).isFailure() ) {
		(*m_log) << MSG::ERROR << "Problems to read FEB info"
			<< endreq;
		return StatusCode::FAILURE;
	}
	for(LArFebEnergyCollection::const_iterator tit = tbegin; tit!=tend; ++tit) {
                LArFebEnergy *feb = new LArFebEnergy((*tit)->getFebId());
                feb->setFebEx((*tit)->getFebEx());
                feb->setFebEy((*tit)->getFebEy());
                feb->setFebEz((*tit)->getFebEz());
                (*m_log) << MSG::VERBOSE << "FCALHAD 0 FEB ID = " << (*tit)->getFebId() << endreq;
                (*m_log) << MSG::VERBOSE << "FCALHAD 0 FEB Ex = " << (*tit)->getFebEx() << endreq;
                (*m_log) << MSG::VERBOSE << "FCALHAD 0 FEB Ey = " << (*tit)->getFebEy() << endreq;
                (*m_log) << MSG::VERBOSE << "FCALHAD 0 FEB Ez = " << (*tit)->getFebEz() << endreq;
		m_larfebcol_fcal.push_back(feb);
	}
        m_data->RegionSelector(1,etamin,etamax,phimin,phimax, FCALHAD);
	if ( m_data->LoadCollections(tbegin,tend).isFailure() ) {
		(*m_log) << MSG::ERROR << "Problems to read FEB info"
			<< endreq;
		return StatusCode::FAILURE;
	}
	for(LArFebEnergyCollection::const_iterator tit = tbegin; tit!=tend; ++tit) {
                LArFebEnergy *feb = new LArFebEnergy((*tit)->getFebId());
                feb->setFebEx((*tit)->getFebEx());
                feb->setFebEy((*tit)->getFebEy());
                feb->setFebEz((*tit)->getFebEz());
                (*m_log) << MSG::VERBOSE << "FCALHAD 1 FEB ID = " << (*tit)->getFebId() << endreq;
                (*m_log) << MSG::VERBOSE << "FCALHAD 1 FEB Ex = " << (*tit)->getFebEx() << endreq;
                (*m_log) << MSG::VERBOSE << "FCALHAD 1 FEB Ey = " << (*tit)->getFebEy() << endreq;
                (*m_log) << MSG::VERBOSE << "FCALHAD 1 FEB Ez = " << (*tit)->getFebEz() << endreq;
		m_larfebcol_fcal.push_back(feb);
	}
        m_data->RegionSelector(0,etamin,etamax,phimin,phimax, FCALEM);
	if ( m_data->LoadCollections(tbegin,tend).isFailure() ) {
		(*m_log) << MSG::ERROR << "Problems to read FEB info"
			<< endreq;
		return StatusCode::FAILURE;
	}
	for(LArFebEnergyCollection::const_iterator tit = tbegin; tit!=tend; ++tit) {
                LArFebEnergy *feb = new LArFebEnergy((*tit)->getFebId());
                feb->setFebEx((*tit)->getFebEx());
                feb->setFebEy((*tit)->getFebEy());
                feb->setFebEz((*tit)->getFebEz());
                (*m_log) << MSG::VERBOSE << "FCALEM 0 FEB ID = " << (*tit)->getFebId() << endreq;
                (*m_log) << MSG::VERBOSE << "FCALEM 0 FEB Ex = " << (*tit)->getFebEx() << endreq;
                (*m_log) << MSG::VERBOSE << "FCALEM 0 FEB Ey = " << (*tit)->getFebEy() << endreq;
                (*m_log) << MSG::VERBOSE << "FCALEM 0 FEB Ez = " << (*tit)->getFebEz() << endreq;
		m_larfebcol_fcal.push_back(feb);
	}
	}
/** start comparison between FEB and cells **/
	/*int count = 0;
	int count_not_found = 0;
	int countp = 0;
        double total = 0;
        double totalx = 0;
        double totaly = 0;
        double totalz = 0;
        doComparison(&m_larfebcol,&larfebmap,count,count_not_found,countp,total,totalx,totaly,totalz);
        float totene = sqrt(pow(totalx,2) + pow(totaly,2) + pow(totalz,2));
	(*m_log) << MSG::INFO << "HLT FEBs found : " << count << endreq;
	(*m_log) << MSG::INFO << "FEB total energy = " << totene;
	(*m_log) << MSG::INFO << "; FEB total energy x = " << totalx;
	(*m_log) << MSG::INFO << "; FEB total energy y = " << totaly;
	(*m_log) << MSG::INFO << "; FEB total energy z = " << totalz << endreq;
        
	(*m_log) << " not found : " << count_not_found << endreq;
	(*m_log) << " with problems : " << countp << endreq;
	(*m_log) << " using difflimit  : " << m_difflimit << endreq;*/
	int counthec = 0;
	int counthec_not_found = 0;
	int countphec = 0;
        double totalhec = 0;
        double totalhecx = 0;
        double totalhecy = 0;
        double totalhecz = 0;
        double offlinehecx = 0;
        double offlinehecy = 0;
        double offlinehecz = 0;
	(*m_log) << MSG::INFO << "Compare FEBs in HEC" << endreq;
        doComparison(&m_larfebcol_hec,&larfebmap_hec,counthec,counthec_not_found,countphec,totalhec,totalhecx,totalhecy,totalhecz, offlinehecx,offlinehecy,offlinehecz);
        float tothecene = sqrt(pow(totalhecx,2) + pow(totalhecy,2) + pow(totalhecz,2));
	(*m_log) << MSG::INFO << "HLT HEC FEBs found : " << counthec << endreq;
	(*m_log) << "HEC FEBs not found : " << counthec_not_found << endreq;
	(*m_log) << "HEC FEBs with problems : " << countphec << endreq;
	(*m_log) << "HEC FEBs using difflimit  : " << m_difflimit << endreq;

	(*m_log) << MSG::INFO << "HEC HLT FEB total energy = " << tothecene;
	(*m_log) << MSG::INFO << "; HEC HLT FEB total energy x = " << totalhecx;
	(*m_log) << MSG::INFO << "; HEC HLT FEB total energy y = " << totalhecy;
	(*m_log) << MSG::INFO << "; HEC HLT FEB total energy z = " << totalhecz << endreq;
        float offlinehecene = sqrt(offlinehecx*offlinehecx + offlinehecy*offlinehecy + offlinehecz*offlinehecz);
	(*m_log) << MSG::INFO << "HEC offline FEB total energy = " << offlinehecene;
	(*m_log) << MSG::INFO << "; HEC offline FEB total energy x = " << offlinehecx;
	(*m_log) << MSG::INFO << "; HEC offline FEB total energy y = " << offlinehecy;
	(*m_log) << MSG::INFO << "; HEC offline FEB total energy z = " << offlinehecz << endreq;
// now do EM 
	int countem = 0;
	int countem_not_found = 0;
	int countpem = 0;
        double totalem = 0;
        double totalemx = 0;
        double totalemy = 0;
        double totalemz = 0;
        double offlineemx = 0;
        double offlineemy = 0;
        double offlineemz = 0;
	(*m_log) << MSG::INFO << "Compare FEBs in EM " << endreq;
        doComparison(&m_larfebcol_em,&larfebmap_em,countem,countem_not_found,countpem,totalem,totalemx,totalemy,totalemz,offlineemx,offlineemy,offlineemz);
        float totemene = sqrt(pow(totalemx,2) + pow(totalemy,2) + pow(totalemz,2));
	(*m_log) << MSG::INFO << "HLT EM FEBs found : " << countem << endreq;
	(*m_log) << "EM FEBs not found : " << countem_not_found << endreq;
	(*m_log) << "EM FEBs with problems : " << countpem << endreq;
	(*m_log) << "EM FEBs using difflimit  : " << m_difflimit << endreq;
	(*m_log) << MSG::INFO << "EM HLT FEB total energy = " << totemene;
	(*m_log) << MSG::INFO << "; EM HLT FEB total energy x = " << totalemx;
	(*m_log) << MSG::INFO << "; EM HLT FEB total energy y = " << totalemy;
	(*m_log) << MSG::INFO << "; EM HLT FEB total energy z = " << totalemz << endreq;
        float offlineemene = sqrt(offlineemx*offlineemx + offlineemy*offlineemy + offlineemz*offlineemz);
	(*m_log) << MSG::INFO << "EM offline FEB total energy = " << offlineemene;
	(*m_log) << MSG::INFO << "; EM offline FEB total energy x = " << offlineemx;
	(*m_log) << MSG::INFO << "; EM offline FEB total energy y = " << offlineemy;
	(*m_log) << MSG::INFO << "; EM offline FEB total energy z = " << offlineemz << endreq;
        
// now do FCAL
	int countfcal = 0;
	int countfcal_not_found = 0;
	int countpfcal = 0;
        double totalfcal = 0;
        double totalfcalx = 0;
        double totalfcaly = 0;
        double totalfcalz = 0;
        double offlinefcalx = 0;
        double offlinefcaly = 0;
        double offlinefcalz = 0;
	(*m_log) << MSG::INFO << "Compare FEBs in FCAL " << endreq;
        doComparison(&m_larfebcol_fcal,&larfebmap_fcal,countfcal,countfcal_not_found,countpfcal,totalfcal,totalfcalx,totalfcaly,totalfcalz,offlinefcalx,offlinefcaly,offlinefcalz);
        float totfcalene = sqrt(pow(totalfcalx,2) + pow(totalfcaly,2) + pow(totalfcalz,2));
	(*m_log) << MSG::INFO << "HLT FCAL FEBs found : " << countfcal << endreq;
	(*m_log) << "FCAL FEBs not found : " << countfcal_not_found << endreq;
	(*m_log) << "FCAL FEBs with problems : " << countpfcal << endreq;
	(*m_log) << "FCAL FEBs using difflimit  : " << m_difflimit << endreq;
	(*m_log) << MSG::INFO << "FCAL HLT FEB total energy = " << totfcalene;
	(*m_log) << MSG::INFO << "; FCAL HLT FEB total energy x = " << totalfcalx;
	(*m_log) << MSG::INFO << "; FCAL HLT FEB total energy y = " << totalfcaly;
	(*m_log) << MSG::INFO << "; FCAL HLT FEB total energy z = " << totalfcalz << endreq;
        float offlinefcalene = sqrt(offlinefcalx*offlinefcalx + offlinefcaly*offlinefcaly + offlinefcalz*offlinefcalz);
	(*m_log) << MSG::INFO << "FCAL offline FEB total energy = " << offlinefcalene;
	(*m_log) << MSG::INFO << "; FCAL offline FEB total energy x = " << offlinefcalx;
	(*m_log) << MSG::INFO << "; FCAL offline FEB total energy y = " << offlinefcaly;
	(*m_log) << MSG::INFO << "; FCAL offline FEB total energy z = " << offlinefcalz << endreq;
        
#ifndef NDEBUG
        double febcellx = 0;
        double febcelly = 0;
        double febcellz = 0;
	for( it = larfebmap.begin(); it!=larfebmap.end();++it){
                LArFebEnergy larfebenergy = (*it).second;
		(*m_log) << MSG::VERBOSE << "FEB from cell ID ";
		(*m_log) << MSG::VERBOSE << std::hex;
		(*m_log) << MSG::VERBOSE << larfebenergy.getFebId() << " ";
		(*m_log) << MSG::VERBOSE << std::dec;
		(*m_log) << MSG::VERBOSE << endreq;
		(*m_log) << MSG::VERBOSE << "FEB from cell ex = " << larfebenergy.getFebEx();
		(*m_log) << MSG::VERBOSE << "FEB from cell ey = " << larfebenergy.getFebEy();
		(*m_log) << MSG::VERBOSE << "FEB from cell ez = " << larfebenergy.getFebEz() << endreq;
                febcellx += larfebenergy.getFebEx();
                febcelly += larfebenergy.getFebEy();
                febcellz += larfebenergy.getFebEz();
		// Delete the pointers now. Don't use after this point.
		//delete larfebenergy;
	}
        double febcelltot = sqrt(pow(febcellx,2) + pow(febcelly,2) + pow(febcellz,2));
        (*m_log) << MSG::INFO << "FEB from cell total ene = " << febcelltot;
        (*m_log) << MSG::INFO << "; FEB from cell Ex = " << febcellx;
        (*m_log) << MSG::INFO << "; FEB from cell Ey = " << febcelly;
        (*m_log) << MSG::INFO << "; FEB from cell Ez = " << febcellz << endreq;

#endif
	//larfebmap.clear();
        //m_larfebcol.clear();

	return StatusCode::SUCCESS;
}


StatusCode TrigFEBValidation::finalize(){
	(*m_log) << MSG::INFO << "Finishing" << endreq;
	delete m_log;
	return StatusCode::SUCCESS;
}

void TrigFEBValidation::doComparison(LArFebEnergyCollection* febcoll,std::map<HWIdentifier,LArFebEnergy>* larfebmap,int &count,int &count_not_found,int &countp,double &total,double &totalx,double &totaly,double &totalz,double &offlinefebex, double &offlinefebey, double &offlinefebez){

    std::map<HWIdentifier,LArFebEnergy>::iterator it;
    for(it = larfebmap->begin(); it!=larfebmap->end();++it){
         LArFebEnergy larfebenergy = (*it).second;
         offlinefebex += larfebenergy.getFebEx();
         offlinefebey += larfebenergy.getFebEy();
         offlinefebez += larfebenergy.getFebEz();
    }
//
    for( LArFebEnergyCollection::const_iterator tit= febcoll->begin(); tit!=febcoll->end(); ++tit) {
                double ex = (*tit)->getFebEx();
                double ey = (*tit)->getFebEy();
                double ez = (*tit)->getFebEz();
		total = ex*ex;
		total += ey*ey;
		total += ez*ez;
		total = sqrt (total);
                totalx += ex;
                totaly += ey;
                totalz += ez;
		//if ( total > 1e3 ) {
                if(!(ex==0&&ey==0&&ez==0)){
                   count++;
#ifdef DONTDO
                   (*m_log) << MSG::VERBOSE << "validation FEB ID = " << std::hex << (*tit)->getFebId() << std::dec ;
                   (*m_log) << MSG::VERBOSE << "FEB Ex = " << ex;
                   (*m_log) << MSG::VERBOSE << "FEB Ey = " << ey;
                   (*m_log) << MSG::VERBOSE << "FEB Ez = " << ez << endreq;
#endif
                }
		bool found = false;
                for(it = larfebmap->begin(); it!=larfebmap->end();++it){
                        LArFebEnergy larfebenergy = (*it).second;
                        unsigned int febid = (*tit)->getFebId();
                        unsigned int febidshort = febid/0x1000000;
                        if ( febid == larfebenergy.getFebId() ){
                          found = true;
                          float diffx = fabsf(larfebenergy.getFebEx()
                                - (*tit)->getFebEx() );
                          float diffxp = 0;
                          if ( fabsf ((*tit)->getFebEx()) > 0.2 )
                                diffxp = 100*diffx/(*tit)->getFebEx();
                          float diffy = fabsf(larfebenergy.getFebEy()
                                - (*tit)->getFebEy() );
                          float diffyp = 0;
                          if ( fabsf ((*tit)->getFebEy()) > 0.2 )
                                diffyp = 100*diffy/(*tit)->getFebEy();
                          float diffz = fabsf(larfebenergy.getFebEz()
                                - (*tit)->getFebEz() );
                          float diffzp = 0;
                          if ( fabsf ((*tit)->getFebEz()) > 0.2 )
                                diffzp = 100*diffz/(*tit)->getFebEz();
                          if ( (diffx >= m_difflimit) ||
                               (diffy >= m_difflimit) ||
                               (diffz >= m_difflimit) ){
                                countp++; 
                                (*m_log) << MSG::DEBUG;
                                (*m_log) << "Found Prob in : ";
                                //(*m_log) << std::hex;
                                (*m_log) << std::hex;
                                (*m_log) << "Offline ID " << larfebenergy.getFebId() << " ";
                                (*m_log) << "HLT ID " << (*tit)->getFebId() << " ";
                                (*m_log) << std::dec;
                                (*m_log) << "Offline Ex " << larfebenergy.getFebEx() << " ";
                                (*m_log) << "Online Ex " << (*tit)->getFebEx() << " ";
                                (*m_log) << "Offline Ey " << larfebenergy.getFebEy() << " ";
                                (*m_log) << "Online Ey " << (*tit)->getFebEy() << " ";
                                (*m_log) << "Offline Ez " << larfebenergy.getFebEz() << " ";
                                (*m_log) << "Online Ez " << (*tit)->getFebEz() << " ";
                                (*m_log) << endreq;
                                if(febidshort == 0x38) 
                                   (*m_log) << "This is in the C-side EM barrel" << endreq;
                                else if(febidshort == 0x39)
                                   (*m_log) << "This is in the A-side EM barrel" << endreq;
                                else if(febidshort == 0x3a)
                                   switch(febid){
                                     case 0x3a300000:
                                     case 0x3a308000:
                                     case 0x3a310000:
                                     case 0x3a318000:
                                     case 0x3a320000:
                                     case 0x3a328000:
                                     case 0x3a330000:
                                     case 0x3a340000:
                                     case 0x3a348000:
                                     case 0x3a350000:
                                     case 0x3a358000:
                                     case 0x3a360000:
                                     case 0x3a368000:
                                     case 0x3a370000:
                                        (*m_log) << "This is in the C-side fcal" << endreq;
                                        break;
                                     case 0x3a520000:
                                     case 0x3a528000:
                                     case 0x3a1a0000:
                                     case 0x3a1a8000:
                                     case 0x3ab20000:
                                     case 0x3ab28000:
                                     case 0x3a820000:
                                     case 0x3a828000:
                                     case 0x3a530000:
                                     case 0x3a538000:
                                     case 0x3a1b0000:
                                     case 0x3a1b8000:
                                     case 0x3ab30000:
                                     case 0x3ab38000:
                                     case 0x3a830000:
                                     case 0x3a838000:
                                     case 0x3a540000:
                                     case 0x3a548000:
                                     case 0x3a1c0000:
                                     case 0x3a1c8000:
                                     case 0x3ab40000:
                                     case 0x3ab48000:
                                     case 0x3a840000:
                                     case 0x3a848000:
                                        (*m_log) << "This is in the C-side HEC " << endreq;
                                        break;
                                     default: 
                                        (*m_log) << "This is in the C-side EM endcap" << endreq;
                                        break;
                                   }
                                else if(febidshort == 0x3b)
                                   switch(febid){
                                     case 0x3b300000:
                                     case 0x3b308000:
                                     case 0x3b310000:
                                     case 0x3b318000:
                                     case 0x3b320000:
                                     case 0x3b328000:
                                     case 0x3b330000:
                                     case 0x3b340000:
                                     case 0x3b348000:
                                     case 0x3b350000:
                                     case 0x3b358000:
                                     case 0x3b360000:
                                     case 0x3b368000:
                                     case 0x3b370000:
                                        (*m_log) << "This is in the A-side fcal" << endreq;
                                        break;
                                     case 0x3b520000:
                                     case 0x3b528000:
                                     case 0x3b1a0000:
                                     case 0x3b1a8000:
                                     case 0x3bb20000:
                                     case 0x3bb28000:
                                     case 0x3b820000:
                                     case 0x3b828000:
                                     case 0x3b530000:
                                     case 0x3b538000:
                                     case 0x3b1b0000:
                                     case 0x3b1b8000:
                                     case 0x3bb30000:
                                     case 0x3bb38000:
                                     case 0x3b830000:
                                     case 0x3b838000:
                                     case 0x3b540000:
                                     case 0x3b548000:
                                     case 0x3b1c0000:
                                     case 0x3b1c8000:
                                     case 0x3bb40000:
                                     case 0x3bb48000:
                                     case 0x3b840000:
                                     case 0x3b848000:
                                        (*m_log) << "This is in the A-side HEC " << endreq;
                                        break;
                                     default: 
                                        (*m_log) << "This is in the A-side EM endcap" << endreq;
                                        break;
                                   }
                          } // end if matched FEB within limit
                          break;
                        } // enf if matching FEB Ids
                } // end for loop over FEBs from offline
           
		if ( (!found)&&(ex!=0||ey!=0||ez!=0) ) count_not_found++;
		//}
	}
}
