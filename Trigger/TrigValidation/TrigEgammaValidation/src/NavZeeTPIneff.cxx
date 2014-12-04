/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <iostream>
#include "TrigEgammaValidation/NavZeeTPIneff.h"
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"

StatusCode NavZeeTPIneff::childInitialize(){

m_eventCounter=0;
m_probesTried=0;
	addDirectory("TestIneff");
	addHistogram(new TH1F("IsEmFail","IsEmFail",33,0,33));
	if ( m_electronOnlPPCutIDTool.retrieve().isFailure() ){
		std::cout << "retrieve failed for tools" << std::endl;
	}
	m_labels.push_back("All");
	for(unsigned int i=0;i<m_labels.size();i++)
		hist1("IsEmFail")->GetXaxis()->SetBinLabel(i+1,m_labels.at(i).c_str());

return StatusCode::SUCCESS;
}

StatusCode NavZeeTPIneff::childExecute()
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
	m_probesTried++;
	const xAOD::Electron* offEl = probeElectrons[i].first;
	//float denOffEl_et = offEl->caloCluster()->et()/1e3;
	float denOffEl_eta = offEl->caloCluster()->eta();
	float denOffEl_phi = offEl->caloCluster()->phi();
        bool passedEF=false;
	const HLT::TriggerElement* feat = probeElectrons[i].second;
	const xAOD::Electron* selEF = NULL;

	if ( feat ) {

	passedEF = ancestorPassed<xAOD::ElectronContainer>(feat);

	const xAOD::ElectronContainer* EFEl = getFeature<xAOD::ElectronContainer>(feat);
	if ( EFEl != NULL )
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
	if ( !passedEF && (feat) && (selEF!=NULL) ) {
	// Now use accept method
        // All propoerties for trigger set via properties
            m_electronOnlPPCutIDTool[2]->accept(selEF);
		m_electronOnlPPCutIDTool[1]->accept(selEF);
		m_electronOnlPPCutIDTool[0]->accept(selEF);
		//unsigned int loose = m_electronOnlPPCutIDTool[2]->IsemValue();
		//unsigned int medium= m_electronOnlPPCutIDTool[1]->IsemValue();
		unsigned int tight = m_electronOnlPPCutIDTool[0]->IsemValue();
		for(int ii=0;ii<32;ii++)
			if ( (tight>>ii) & 0x1 )
			hist1("IsEmFail")->Fill(ii+0.5);
	}
}
clearProbeList();


return StatusCode::SUCCESS;

}

StatusCode NavZeeTPIneff::childFinalize()
{

//hist1("IsEmFail")->Scale(100.0/(float)m_probesTried);
for(unsigned int ii=0;ii<m_probesTried;++ii)
hist1("IsEmFail")->Fill(32.5);
return StatusCode::SUCCESS;
}
