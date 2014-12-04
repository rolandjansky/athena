/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <iostream>
#include "TrigEgammaValidation/NavZeeTPEff.h"
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"

StatusCode NavZeeTPEff::childInitialize(){

m_eventCounter=0;
	addDirectory("TestEff");
	addHistogram(new TH1F("OffEtDen","OffEtDen",80,0,80));
	addHistogram(new TH1F("OffEtaDen","OffEtaDen",50,-2.5,2.5));
	addHistogram(new TH1F("OffEtNumL2","OffEtNumL2",80,0,80));
	addHistogram(new TH1F("OffEtaNumL2","OffEtaNumL2",50,-2.5,2.5));
	addHistogram(new TH1F("OffEtNumEF","OffEtNumEF",80,0,80));
	addHistogram(new TH1F("OffEtaNumEF","OffEtaNumEF",50,-2.5,2.5));

return StatusCode::SUCCESS;
}

StatusCode NavZeeTPEff::childExecute()
{

m_eventCounter++;

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
for(unsigned int i=0;i<probeElectrons.size();i++){
	if ( probeElectrons[i].first->pt() < 24e3 ) continue;
	const xAOD::Electron* offEl = probeElectrons[i].first;
	float denOffEl_et = offEl->caloCluster()->et()/1e3;
	float denOffEl_eta = offEl->caloCluster()->eta();
        bool passedL2=false;
        bool passedEF=false;
	const HLT::TriggerElement* feat = probeElectrons[i].second;
	hist1("OffEtDen")->Fill(denOffEl_et);
	hist1("OffEtaDen")->Fill(denOffEl_eta);

	if ( feat ) {

	passedL2 = ancestorPassed<xAOD::TrigElectronContainer>(feat);
	passedEF = ancestorPassed<xAOD::ElectronContainer>(feat);

	if ( passedL2 ) {
	hist1("OffEtNumL2")->Fill(denOffEl_et);
	hist1("OffEtaNumL2")->Fill(denOffEl_eta);
	}
	if ( passedEF ) {
	hist1("OffEtNumEF")->Fill(denOffEl_et);
	hist1("OffEtaNumEF")->Fill(denOffEl_eta);
	}
	}
}
clearProbeList();


return StatusCode::SUCCESS;

}

StatusCode NavZeeTPEff::childFinalize()
{

return StatusCode::SUCCESS;
}
