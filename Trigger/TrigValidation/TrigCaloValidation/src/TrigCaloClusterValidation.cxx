/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigCaloValidation/TrigCaloClusterValidation.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrigCaloRec/IAlgToolEFCalo.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "TMath.h"
#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEvent/TrigEMClusterContainer.h"

TrigCaloClusterValidation::TrigCaloClusterValidation(const std::string& name,
	ISvcLocator* pSvcLocator) : Algorithm(name,pSvcLocator),
	m_tcrAlgTools(this,"")
{
	declareProperty("CaloOfflineClusterContainer", m_CaloClusterOfflineContainerName = "egClusterCollection");
	declareProperty("CaloEFOnlineClusterContainer", m_CaloClusterEFOnlineContainerName = "HLT_TrigCaloClusterMaker");
	declareProperty("CaloL2OnlineClusterContainer", m_CaloClusterL2OnlineContainerName = "HLT_TrigT2CaloEgamma");
	declareProperty ("TCRTools", m_tcrAlgTools);
	declareProperty("EtCut", m_EtCut = 10000);
	declareProperty("DeltaRCut", m_DeltaRCut =0.0375);


	// Nothing for the moment
	return;

}


TrigCaloClusterValidation::~TrigCaloClusterValidation() {}

StatusCode TrigCaloClusterValidation::initialize(){

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


StatusCode TrigCaloClusterValidation::execute(){

        const CaloClusterContainer* caloOFF = 0;
        StatusCode sc=m_storeGate->retrieve(caloOFF, m_CaloClusterOfflineContainerName);
        if( sc.isFailure()  ||  !caloOFF ) {
                       (*m_log) << MSG::DEBUG << "No OFFline Calo Cell Container found"
                        << endreq;
                return StatusCode::SUCCESS;
        }
        (*m_log) << MSG::INFO << "Got Offline container ";

        const CaloClusterContainer* caloEFON = 0;
        sc=m_storeGate->retrieve(caloEFON, m_CaloClusterEFOnlineContainerName);
        if( sc.isFailure()  ||  !caloEFON ) {
                       (*m_log) << MSG::DEBUG << "No EF  Calo Cell Container found"
                        << endreq;
                return StatusCode::SUCCESS;
        }
        const TrigEMClusterContainer* caloL2ON = 0;
        sc=m_storeGate->retrieve(caloL2ON,m_CaloClusterL2OnlineContainerName);
        if( sc.isFailure()  ||  !caloL2ON ) {
                       (*m_log) << MSG::DEBUG << "No L2 Calo Cell Container found"
                        << endreq;
                return StatusCode::SUCCESS;
        }
        CaloClusterContainer::const_iterator ONEFbeg = caloEFON->begin();
        CaloClusterContainer::const_iterator ONEFend = caloEFON->end();
        TrigEMClusterContainer::const_iterator ONL2beg = caloL2ON->begin();
        TrigEMClusterContainer::const_iterator ONL2end = caloL2ON->end();
        CaloClusterContainer::const_iterator OFFbeg = caloOFF->begin();
        CaloClusterContainer::const_iterator OFFend = caloOFF->end();
        CaloClusterContainer::const_iterator MatchEF;
        TrigEMClusterContainer::const_iterator MatchL2;
 
	if(caloEFON->size()!=0&&caloOFF->size()!=0){
		for(;OFFbeg!=OFFend;++OFFbeg){
			MatchEF =caloEFON->begin();
			ONEFbeg=caloEFON->begin();
			for(;ONEFbeg!=ONEFend;++ONEFbeg){
				if(deltaR((*ONEFbeg)->phi(),(*OFFbeg)->phi(),(*ONEFbeg)->eta(),(*OFFbeg)->eta())<deltaR((*MatchEF)->phi(),(*OFFbeg)->phi(),(*MatchEF)
->eta(),(*OFFbeg)->eta())) MatchEF=ONEFbeg;
			}
			if((*OFFbeg)->et()>m_EtCut&&deltaR((*MatchEF)->phi(),(*OFFbeg)->phi(),(*MatchEF)->eta(),(*OFFbeg)->eta())<m_DeltaRCut){
				(*m_log) << MSG::DEBUG << "\nREGTEST: ";
				(*m_log) << "Offline cluster matched with online EF cluster";
				(*m_log) << "\nREGTEST: Online EF energy : " << (*MatchEF)->et();
				(*m_log) << "\nREGTEST: Offline energy : " << (*OFFbeg)->et();
				(*m_log) << "\nREGTEST: Transverse energy fraction (on/off) : " << (*MatchEF)->et()/(*OFFbeg)->et();
				(*m_log) << "\nREGTEST: Offline cluster eta : " << (*OFFbeg)->eta();
				(*m_log) << "\nREGTEST: Offline cluster phi : " << (*OFFbeg)->phi();
				(*m_log) << endreq;
			}
		}
	}
	if(caloL2ON->size()!=0&&caloOFF->size()!=0){
		OFFbeg = caloOFF->begin();		
		for(;OFFbeg!=OFFend;++OFFbeg){
			MatchL2 =caloL2ON->begin();
			ONL2beg=caloL2ON->begin();

			for(;ONL2beg!=ONL2end;++ONL2beg){
				if(deltaR((*ONL2beg)->phi(),(*OFFbeg)->phi(),(*ONL2beg)->eta(),(*OFFbeg)->eta())<deltaR((*MatchL2)->phi(),(*OFFbeg)->phi(),(*MatchL2)->eta(),(*OFFbeg)->eta())) MatchL2=ONL2beg;
			}
			if((*OFFbeg)->et()>m_EtCut&&deltaR((*MatchL2)->phi(),(*OFFbeg)->phi(),(*MatchL2)->eta(),(*OFFbeg)->eta())<m_DeltaRCut){
				(*m_log) << MSG::DEBUG << "\nREGTEST: ";
				(*m_log) << "Offline cluster matched with L2 online cluster";
				(*m_log) << "\nREGTEST: Online energy : " << (*MatchL2)->et();
				(*m_log) << "\nREGTEST: Offline energy : " << (*OFFbeg)->et();
				(*m_log) << "\nREGTEST: Transverse energy fraction (on/off) : " << (*MatchL2)->et()/(*OFFbeg)->et();
				(*m_log) << "\nREGTEST: Offline cluster eta : " << (*OFFbeg)->eta();
				(*m_log) << "\nREGTEST: Offline cluster phi : " << (*OFFbeg)->phi();
				(*m_log) << endreq;
			}
		}
	}
	return StatusCode::SUCCESS;
}


StatusCode TrigCaloClusterValidation::finalize(){
	(*m_log) << MSG::INFO << "Finishing" << endreq;
	delete m_log;
	return StatusCode::SUCCESS;
}

float TrigCaloClusterValidation::deltaR( float Phi1, float Phi2, float Eta1, float Eta2){
	
	return sqrt(pow(fabsf(Eta1-Eta2),2)+pow(deltaPhi(Phi1,Phi2),2));
}
float TrigCaloClusterValidation::deltaPhi( float PhiA, float PhiB){
	double deltaphi=fabsf(PhiA-PhiB);
	deltaphi=fabsf(TMath::Pi()-deltaphi);
	deltaphi=fabsf(TMath::Pi()-deltaphi);
	return deltaphi;
}




