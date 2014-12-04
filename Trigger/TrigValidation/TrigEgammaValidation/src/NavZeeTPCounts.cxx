/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <iostream>
#include "TrigEgammaValidation/NavZeeTPCounts.h"
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"

StatusCode NavZeeTPCounts::childInitialize(){

m_eventCounter=0;
m_nProbes[0]=0;
m_nProbesPassed[0]=0;
m_nProbes[1]=0;
m_nProbesPassed[1]=0;
m_nProbes[2]=0;
m_nProbesPassed[2]=0;
std::cout << "child Initialize" << std::endl;

return StatusCode::SUCCESS;
}

StatusCode NavZeeTPCounts::childExecute()
{

m_eventCounter++;
//return StatusCode::SUCCESS;

// Event Wise Selection (independent of the required signatures)
if ( !NavZeeTPBase::EventWiseSelection() ) return StatusCode::SUCCESS;

float ZMassMin=80;
float ZMassMax=100;
unsigned int isEMoffProbe, isEMoffTag;
unsigned int offTagTighteness, offProbeTighteness;
offTagTighteness=1; // 1=tight, 2=medium, 3=loose
offProbeTighteness=1;
isEMoffTag=egammaPID::ElectronTightPPIso;
isEMoffProbe=egammaPID::ElectronTightPP;
bool oppositeCharge=true;
float minTagEt=25;
std::string tagTrigger("e28_tight_iloose");
std::string probeTrigger("e28_tight_iloose");

if ( executeTandP(ZMassMin,ZMassMax,isEMoffProbe,isEMoffTag,offTagTighteness,offProbeTighteness,oppositeCharge,minTagEt,tagTrigger).isFailure() )
	return StatusCode::FAILURE;

// Just for counting
//std::cout << "number of probes to check : " << probeElectrons.size() << std::endl;
for(unsigned int i=0;i<probeElectrons.size();i++){
	if ( probeElectrons[i].first->pt() < 24e3 ) continue;
	const HLT::TriggerElement* feat = probeElectrons[i].second;
	m_nProbes[0]++;
	m_nProbes[1]++;
	m_nProbes[2]++;
	if ( feat ) {

	const xAOD::Electron* offEl = probeElectrons[i].first;
	std::cout << "Off El : " << offEl->pt();
	std::cout << "; eta : " << offEl->eta();
	std::cout << "; phi : " << offEl->phi();
	std::cout << std::endl;
	bool L2cok=false;
	bool L2ok=false;
	bool EFok=false;

        bool passedL2c = ancestorPassed<xAOD::TrigEMCluster>(feat); // L2 check
        const xAOD::TrigEMCluster* l2cEl = getFeature<xAOD::TrigEMCluster>(feat);
	if ( l2cEl != NULL ) std::cout << "L2 EMCluster El : " << l2cEl->et() << " " << l2cEl->eta() << " " << l2cEl->phi() << std::endl;
        if (passedL2c) m_nProbesPassed[2]++;

	bool passedL2 = ancestorPassed<xAOD::TrigElectronContainer>(feat); // L2 check
	const xAOD::TrigElectronContainer* l2El = getFeature<xAOD::TrigElectronContainer>(feat);
	if ( l2El != NULL)
	for(unsigned int j=0;j<l2El->size();j++){
		std::cout << "L2 El : " << l2El->at(j)->pt();
		std::cout << "; eta : " << l2El->at(j)->eta();
		std::cout << "; phi : " << l2El->at(j)->phi();
		if ( passedL2 ){
			L2ok=true;
			std:: cout << " passed";
		}
		else
			std:: cout << " not passed";
		std::cout << std::endl;
	}
	if (L2ok) m_nProbesPassed[0]++;

	bool passedEF = ancestorPassed<xAOD::ElectronContainer>(feat); // EF check
	const xAOD::ElectronContainer* EFEl = getFeature<xAOD::ElectronContainer>(feat);
	if ( EFEl != NULL)
        for(unsigned int j=0;j<EFEl->size();j++){
                std::cout << "EF El : " << EFEl->at(j)->pt();
                std::cout << "; eta : " << EFEl->at(j)->eta();
                std::cout << "; phi : " << EFEl->at(j)->phi();
                if ( passedEF ){
			EFok=true;
                        std:: cout << " passed";
			//m_nProbesPassed[1]++;
		}
                else
                        std:: cout << " not passed";
                std::cout << std::endl;
        }
	if (EFok) m_nProbesPassed[1]++;
        }
}
clearProbeList();

/*
ZMassMin=70;
ZMassMax=110;
if ( executeTandP(ZMassMin,ZMassMax,isEMoffProbe,isEMoffTag,offTagTighteness,offProbeTighteness,oppositeCharge,minTagEt,tagTrigger).isFailure() )
	return StatusCode::FAILURE;
for(unsigned int i=0;i<probeElectrons.size();i++){
	if ( probeElectrons[i].first->pt() < 20e3 ) continue;
        const HLT::TriggerElement* feat = probeElectrons[i].second;
        m_nProbes[1]++;
        if ( feat && ( passedTrigger(feat,probeTrigger)) ) {
                m_nProbesPassed[1]++;
        }
}
clearProbeList();
*/


return StatusCode::SUCCESS;

}

StatusCode NavZeeTPCounts::childFinalize()
{

std::cout << m_nProbesPassed[1] << " " <<  m_nProbes[1] << std::endl;

std::cout << "EC : " << m_eventCounter << "; Prob : "
	<< 100.0*((float)m_nProbesPassed[0]/(float)m_nProbes[0]) << std::endl;
std::cout << "EC : " << m_eventCounter << "; Prob : "
	<< 100.0*((float)m_nProbesPassed[1]/(float)m_nProbes[1]) << std::endl;
std::cout << "EC : " << m_eventCounter << "; Prob : "
	<< 100.0*((float)m_nProbesPassed[2]/(float)m_nProbes[2]) << std::endl;
(*m_log) << MSG::INFO << "Final Number of Events executed : \t\t" << m_eventCounter << endreq; 

return StatusCode::SUCCESS;
}
