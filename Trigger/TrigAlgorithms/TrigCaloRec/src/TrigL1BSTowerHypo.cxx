/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



// **************************************************
//
// NAME	: TrigL1BSTowerHypo
// PACKAGE : Trigger/TrigHypothesis/TrigL1BSTowerHypo
//
// AUTHOR : Denis Oliveira Damazio
//
// **************************************************

#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "TrigCaloRec/TrigL1BSTowerHypo.h"


TrigL1BSTowerHypo::TrigL1BSTowerHypo( const std::string& name,
		ISvcLocator* pSvcLocator) :
	HLT::HypoAlgo(name,pSvcLocator) {

   declareMonitoredStdContainer("EtaEM",m_etaEM);
   declareMonitoredStdContainer("PhiEM",m_phiEM);
   declareMonitoredStdContainer("EtaHAD",m_etaHAD);
   declareMonitoredStdContainer("PhiHAD",m_phiHAD);
   declareMonitoredStdContainer("EtEM",m_EtEM);
   declareMonitoredStdContainer("EtHAD",m_EtHAD);
   declareMonitoredStdContainer("MaxAdcEM",m_AdcEM);
   declareMonitoredStdContainer("MaxAdcHAD",m_AdcHAD);
   declareProperty("adcThreshold",m_adcThreshold=900);

}

TrigL1BSTowerHypo::~TrigL1BSTowerHypo()
{
}

HLT::ErrorCode TrigL1BSTowerHypo::hltInitialize()
{
	msg() << MSG::INFO << "in hltInitialize()" << endmsg;
	m_AdcEM.reserve(100); m_AdcHAD.reserve(100);
        m_EtEM. reserve(100);  m_EtHAD.reserve(100);
        m_etaEM.reserve(100); m_etaHAD.reserve(100);
        m_phiEM.reserve(100); m_phiHAD.reserve(100);
	return HLT::OK;

}
HLT::ErrorCode TrigL1BSTowerHypo::hltFinalize()
{

	msg() << MSG::INFO << "in hltFinalize()" << endmsg;
	return HLT::OK;

}

HLT::ErrorCode TrigL1BSTowerHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {

	pass = false;
	m_AdcEM.clear(); m_AdcHAD.clear();
	m_EtEM.clear();  m_EtHAD.clear();
	m_etaEM.clear(); m_etaHAD.clear();
	m_phiEM.clear(); m_phiHAD.clear();
	const xAOD::TriggerTowerContainer* container(0);
	if ( getFeature(outputTE,container) != HLT::OK ) {
		msg() << MSG::WARNING << " Failed to get TT collection." << endmsg;
		return HLT::ERROR;
	}
	for(unsigned int i=0;i<container->size();i++){
	    const xAOD::TriggerTower* tt = container->at(i);
	    bool adcAbove(false);
	    const std::vector<uint16_t>& vec = tt->adc();
	    const std::vector<uint16_t>::const_iterator end = vec.end();
	    for(std::vector<uint16_t>::const_iterator j=vec.begin();
		j!=end;++j) if ( (*j) > m_adcThreshold ) adcAbove=true;
	    if ( !adcAbove ) continue;
	    pass = true;
	    int isHad = tt->sampling();
	    unsigned int maxAdc(0);
	    for(std::vector<uint16_t>::const_iterator j=vec.begin();
		j!=end;++j) if ( (*j) > maxAdc ) maxAdc=(*j);
	    if ( isHad ) { 
		m_EtHAD.push_back(tt->pt());
		m_etaHAD.push_back(tt->eta());
		m_phiHAD.push_back(tt->phi());
		m_AdcHAD.push_back(maxAdc);
	    } else {
		m_EtEM.push_back(tt->pt());
		m_etaEM.push_back(tt->eta());
		m_phiEM.push_back(tt->phi());
		m_AdcEM.push_back(maxAdc);
	    } 
	}
	return HLT::OK;

}
