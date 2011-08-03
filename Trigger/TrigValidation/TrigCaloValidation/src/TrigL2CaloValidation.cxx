/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigCaloValidation/TrigL2CaloValidation.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"


TrigL2CaloValidation::TrigL2CaloValidation(const std::string& name,
	ISvcLocator* pSvcLocator) : Algorithm(name,pSvcLocator)
{
	declareProperty("L2EGACollectionName",m_L2egname="HLT_TrigT2CaloEgammaCells");
	declareProperty("L2TAUCollectionName",m_L2taname="HLT_TrigT2CaloTauCells");
	declareProperty("L2JETCollectionName",m_L2jename="HLT_TrigT2CaloJetCells");

	// Nothing for the moment
	return;

}


TrigL2CaloValidation::~TrigL2CaloValidation() {}

StatusCode TrigL2CaloValidation::initialize(){

	m_log = new MsgStream(messageService(),name());
	(*m_log) << MSG::INFO << "Initializing" << endreq;

	if ( service("StoreGateSvc",m_storeGate).isFailure() ){
		(*m_log) << MSG::ERROR << "Problems to get StoreGateSvc"
			<< endreq;
		return StatusCode::FAILURE;
	}

	return StatusCode::SUCCESS;
}


StatusCode TrigL2CaloValidation::execute(){

	const DataHandle<CaloCellContainer> AllCalo;
	if ( m_storeGate->retrieve(AllCalo,"AllCalo").isFailure() ){
		(*m_log) << MSG::DEBUG << "No Calo Cell Container found"
                        << endreq;
		return StatusCode::SUCCESS;
	}
	(*m_log) << MSG::INFO << "Got container "; 
	(*m_log) << "Size : " << AllCalo->size() << endreq;
	int count_tcellsL=0;
	int count_tcellspL=0;
	int count_tcellsT=0;
	int count_tcellspT=0;

	const DataHandle<CaloCellContainer> L2EGCalo;
	if ( m_storeGate->retrieve(L2EGCalo,m_L2egname).isFailure() ){
		(*m_log) << MSG::DEBUG << "No T2CaloEgammaCells Container found"
                        << endreq;
	}else
	LoopOver(L2EGCalo.cptr(),AllCalo.cptr(),count_tcellsL, count_tcellspL,count_tcellsT, count_tcellspT);
	
	const DataHandle<CaloCellContainer> L2TACalo;
	if ( m_storeGate->retrieve(L2TACalo,m_L2taname).isFailure() ){
		(*m_log) << MSG::DEBUG << "No T2CaloTauCells Container found"
                        << endreq;
	}else
	LoopOver(L2TACalo.cptr(),AllCalo.cptr(),count_tcellsL, count_tcellspL,count_tcellsT, count_tcellspT);
	(*m_log) << MSG::DEBUG << "REGTEST: Number of LAr cell found " << count_tcellsL << endreq;
	(*m_log) << MSG::DEBUG << "REGTEST: Number of LAr cell w problems " << count_tcellspL << endreq;
	(*m_log) << MSG::DEBUG << "REGTEST: Number of Tile cell found " << count_tcellsT << endreq;
	(*m_log) << MSG::DEBUG << "REGTEST: Number of Tile cell w problems " << count_tcellspT << endreq;

	return StatusCode::SUCCESS;
}


StatusCode TrigL2CaloValidation::finalize(){
	(*m_log) << MSG::INFO << "Finishing" << endreq;
	delete m_log;
	return StatusCode::SUCCESS;
}

void TrigL2CaloValidation::LoopOver(const CaloCellContainer* L2Calo, const CaloCellContainer* AllCalo,int& count_tcellsL, int & count_tcellspL, int& count_tcellsT,
int & count_tcellspT){
	(*m_log) << MSG::INFO << "Got container "; 
	(*m_log) << "Size : " << L2Calo->size() << endreq;
	CaloCellContainer::const_iterator tbeg = L2Calo->begin();
	CaloCellContainer::const_iterator tend = L2Calo->end();

	(*m_log) << MSG::DEBUG << "REGTEST: HLT Container size : "
		<< L2Calo->size() << endreq;
	for(; tbeg!=tend ; ++tbeg){
		CaloCell* tcell = (*tbeg);
		IdentifierHash tid = tcell->caloDDE()->calo_hash();
		//if ( tid < AllCalo->size() ) {
		float tenergy = tcell->energy();
		CaloGain::CaloGain tgain = tcell->gain();
		const CaloCell* cell = AllCalo->findCell(tid);
		if ( cell && cell->caloDDE() ) {
			float energy = cell->energy();
			CaloGain::CaloGain gain = cell->gain();
			float diff = fabsf(tenergy-energy);
			float diffp = 0;
			if ( fabsf (energy) > 0.2 )
				diffp = 100*diff/energy;
			if ( ( (diff>3&&diffp>1) || gain!= tgain) ) {
				(*m_log) << MSG::VERBOSE << "REGTEST: ";
				(*m_log) << "Offline cell found";
				(*m_log) << " energy : " << cell->energy();
				(*m_log) << " tenergy : " << tcell->energy();
				(*m_log) << " gain : " << cell->gain();
				(*m_log) << " tgain : " << tcell->gain();
				(*m_log) << " is_tile : " << cell->caloDDE()->is_tile();
				(*m_log) << endreq;
				if ( cell->caloDDE()->is_tile() )
				count_tcellspT++;
				else
				count_tcellspL++;
			}
		}
		if ( cell->caloDDE()->is_tile() )
		count_tcellsT++;
		else
		count_tcellsL++;
		//}
	}
}
