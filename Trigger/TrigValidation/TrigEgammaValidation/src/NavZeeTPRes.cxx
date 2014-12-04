/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <iostream>
#include "TrigEgammaValidation/NavZeeTPRes.h"
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"

StatusCode NavZeeTPRes::childInitialize(){

m_eventCounter=0;
addDirectory("TestRes");
addHistogram(new TH1F("EFEtRes","EFEtRes",160,-40.0,40.0));
addHistogram(new TH2F("EFEtRes_eta","EFEtRes_eta",50,-2.5,2.5,160,-40.0,40.0));
addHistogram(new TH2F("EFEtRes_et","EFEtRes_et",80,0,80,160,-40.0,40.0));
addHistogram(new TH1F("L2EtRes","L2EtRes",160,-40.0,40.0));
addHistogram(new TH2F("L2EtRes_eta","L2EtRes_eta",50,-2.5,2.5,160,-40.0,40.0));
addHistogram(new TH2F("L2EtRes_et","L2EtRes_et",80,0,80,160,-40.0,40.0));

return StatusCode::SUCCESS;
}

StatusCode NavZeeTPRes::childExecute()
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
	float denOffEl_phi = offEl->caloCluster()->phi();
        bool passedL2=false;
        bool passedEF=false;
	const HLT::TriggerElement* feat = probeElectrons[i].second;

	const xAOD::Electron* selEF = NULL;
	const xAOD::TrigElectron* selL2 = NULL;
	if ( feat ) {

	passedL2 = ancestorPassed<xAOD::TrigElectronContainer>(feat);
        const xAOD::TrigElectronContainer* L2El = getFeature<xAOD::TrigElectronContainer>(feat);
        if (passedL2 && (L2El!=NULL) )
        for(unsigned int j=0;j<L2El->size();j++){
                float deltaEta = TMath::Abs(denOffEl_eta- L2El->at(j)->eta() );
                if ( deltaEta < 0.05 ) {
                float deltaPhi = TMath::Abs(denOffEl_phi- L2El->at(j)->phi() );
                deltaPhi = TMath::Abs( TMath::Pi() - deltaPhi );
                deltaPhi = TMath::Abs( TMath::Pi() - deltaPhi );
                if ( deltaPhi < 0.05 ) {
                        selL2 = L2El->at(j);
                } // deltaPhi
                } // deltaEta
        } // loop over L2El


	passedEF = ancestorPassed<xAOD::ElectronContainer>(feat);
	const xAOD::ElectronContainer* EFEl = getFeature<xAOD::ElectronContainer>(feat);
	if (passedEF && (EFEl!=NULL) )
        for(unsigned int j=0;j<EFEl->size();j++){
		float deltaEta = TMath::Abs(denOffEl_eta- EFEl->at(j)->eta() );
		if ( deltaEta < 0.05 ) {
		float deltaPhi = TMath::Abs(denOffEl_phi- EFEl->at(j)->phi() );
		deltaPhi = TMath::Abs( TMath::Pi() - deltaPhi );
		deltaPhi = TMath::Abs( TMath::Pi() - deltaPhi );
		if ( deltaPhi < 0.05 ) {
			selEF = EFEl->at(j);
		} // deltaPhi
		} // deltaEta
	} // loop over EFEl

	}
	if ( (offEl!=NULL) && (selEF!=NULL) ) {
		float offET = offEl->e()/TMath::CosH(offEl->trackParticle()->eta());
		float onlET = selEF->e()/TMath::CosH(selEF->trackParticle()->eta());
		float resEF = 100.0*(offET - onlET)/offET;
		hist1("EFEtRes")->Fill(resEF);
		hist2("EFEtRes_eta")->Fill(offEl->eta(),resEF);
		hist2("EFEtRes_et")->Fill(offET/1e3,resEF);
	}
        if ( (offEl!=NULL) && (selL2!=NULL) ) {
                float offET = offEl->e()/TMath::CosH(offEl->trackParticle()->eta());
                float onlET = selL2->pt();
                float resL2 = 100.0*(offET - onlET)/offET;
                hist1("L2EtRes")->Fill(resL2);
                hist2("L2EtRes_eta")->Fill(offEl->eta(),resL2);
                hist2("L2EtRes_et")->Fill(offET/1e3,resL2);
        }
}
clearProbeList();


return StatusCode::SUCCESS;

}

StatusCode NavZeeTPRes::childFinalize()
{

return StatusCode::SUCCESS;
}
