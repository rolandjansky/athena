/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigCaloValidation/TrigCaloValidation.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrigCaloRec/IAlgToolEFCalo.h"
#include "CaloEvent/CaloCellContainer.h"


TrigCaloValidation::TrigCaloValidation(const std::string& name,
	ISvcLocator* pSvcLocator) : Algorithm(name,pSvcLocator),
	m_tcrAlgTools(this,"")
{

	declareProperty ("TCRTools", m_tcrAlgTools);
	// Nothing for the moment
	return;

}


TrigCaloValidation::~TrigCaloValidation() {}

StatusCode TrigCaloValidation::initialize(){

	m_log = new MsgStream(messageService(),name());
	(*m_log) << MSG::INFO << "Initializing" << endreq;

	if ( service("StoreGateSvc",m_storeGate).isFailure() ){
		(*m_log) << MSG::ERROR << "Problems to get StoreGateSvc"
			<< endreq;
		return StatusCode::FAILURE;
	}

	if ( m_tcrAlgTools.retrieve().isFailure() ) {
    		(*m_log) << MSG::ERROR << "Failed to retrieve helper tools: " << m_tcrAlgTools << endreq;
		return StatusCode::FAILURE;
	} else {
		(*m_log) << MSG::INFO << "Retrieved " << m_tcrAlgTools << endreq;
	}
	return StatusCode::SUCCESS;
}


StatusCode TrigCaloValidation::execute(){

	const DataHandle<CaloCellContainer> AllCalo;
	if ( m_storeGate->retrieve(AllCalo,"AllCalo").isFailure() ){
		(*m_log) << MSG::DEBUG << "No Calo Cell Container found"
                        << endreq;
		return StatusCode::SUCCESS;
	}
	(*m_log) << MSG::INFO << "Got container "; 
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

	(*m_log) << MSG::DEBUG << "REGTEST: HLT Container size : "
		<< pCaloCellContainer->size() << endreq;
	CaloCellContainer::const_iterator tbeg = pCaloCellContainer->begin();
	CaloCellContainer::const_iterator tend = pCaloCellContainer->end();
	int count_tcells=0;
	int count_tcellsp=0;
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
				(*m_log) << endreq;
				count_tcellsp++;
			}
		}
		count_tcells++;
		//} // check if tcell caloDDE exists
	}
	(*m_log) << MSG::DEBUG << "REGTEST: Number of cell found " << count_tcells << endreq;
	(*m_log) << MSG::DEBUG << "REGTEST: Number of cell w problems " << count_tcellsp << endreq;
	delete pCaloCellContainer;

	return StatusCode::SUCCESS;
}


StatusCode TrigCaloValidation::finalize(){
	(*m_log) << MSG::INFO << "Finishing" << endreq;
	delete m_log;
	return StatusCode::SUCCESS;
}
