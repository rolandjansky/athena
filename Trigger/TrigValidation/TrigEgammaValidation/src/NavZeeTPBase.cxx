/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "TrigEgammaValidation/NavZeeTPBase.h"

NavZeeTPBase::NavZeeTPBase(const std::string & name, ISvcLocator * pSvcLocator) :
                IValidationAlgo(name,pSvcLocator,"ValidationBase"),
		m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool") {

	declareProperty("ElectronPPSelector", m_electronPPCutIDTool);
	declareProperty("TrigDecisionTool", m_trigdec, "iTool to access the trigger decision");
	declareProperty("minTrigList",m_minTrig);
	// just for compile

	HLT::TriggerElement* t = NULL;
	const xAOD::TrigElectronContainer* a = getFeature<xAOD::TrigElectronContainer>(t);
	const xAOD::ElectronContainer* b = getFeature<xAOD::ElectronContainer>(t);
	bool a1 = ancestorPassed<xAOD::TrigElectronContainer>(t);
	bool b1 = ancestorPassed<xAOD::ElectronContainer>(t);
	(void)a; (void)b;
	(void)a1; (void)b1;
	
}

StatusCode
NavZeeTPBase::childInitialize() {
	if ( (m_trigdec.retrieve()).isFailure() ){
		(*m_log) << MSG::ERROR << "Could not retrieve Trigger Decision Tool! Can't work" << endreq;
		return StatusCode::FAILURE;
	}
	if ( (m_electronPPCutIDTool.retrieve()).isFailure() ){
		(*m_log) << MSG::ERROR << "Could not retrieve Trigger Decision Tool! Can't work" << endreq;
		return StatusCode::FAILURE;
	}
	return StatusCode::SUCCESS;
}

bool
NavZeeTPBase::EventWiseSelection(){

    // Check Size of Electron Container
    if ( (m_storeGate->retrieve(offElectrons,"ElectronCollection")).isFailure() ){
        (*m_log) << MSG::ERROR << "Failed to retrieve offline Electrons " << endreq;
	return false;
    }
    if ( offElectrons->size() < 2 ) { // Not enough events for T&P
	(*m_log) << MSG::INFO << "Not enough Electrons for T&P" << endreq;
	return false;
    }

    // missing more selections
    // check Minimal Trigger Requirements
    if ( !MinimalTriggerRequirement() ) return false;

    return true;
}

bool
NavZeeTPBase::MinimalTriggerRequirement(){
	for(unsigned int i=0;i<m_minTrig.size();i++){
		std::string& trigItem = m_minTrig[i];
		if ( m_trigdec->isPassed("HLT_"+trigItem) )
			return true;

	}
	return false; // nothing passed
}

StatusCode
NavZeeTPBase::executeTandP(const float ZMassMin, const float ZMassMax,
		const unsigned int isEMoffTag,
                const unsigned int isEMoffProbe,
		const unsigned int offTagTighteness,
		const unsigned int offProbeTighteness,
		const bool oppositeCharge,
		const float tagMinEt,
		const std::string tagTrigItem){

    probeElectrons.clear();
    for(const auto& elTag : *offElectrons){
        if( ! isTagElectron(elTag,offTagTighteness,isEMoffTag,tagMinEt,tagTrigItem) ) continue;
        for(const auto& elProbe : *offElectrons){
            if(elProbe==elTag) continue;
	    // Check opposite charge
            if(oppositeCharge && (elProbe->charge() == elTag->charge()) ) continue;
            if(!oppositeCharge && (elProbe->charge() != elTag->charge()) ) continue;

            //Must be an easy way with IParticle
            TLorentzVector el1;
            TLorentzVector el2;
            el1.SetPtEtaPhiE(elTag->pt(), elTag->trackParticle()->eta(), elTag->trackParticle()->phi(), elTag->e());
            el2.SetPtEtaPhiE(elProbe->pt(), elProbe->trackParticle()->eta(), elProbe->trackParticle()->phi(), elProbe->e());
            float tpPairMass = (el1 + el2).M();
            if( !((tpPairMass > ZMassMin*1.e3) && (tpPairMass < ZMassMax*1.e3))){
                (*m_log) << MSG::DEBUG << "tag and probe pair not in Z mass window" << endreq;
                continue;
            } else {
		// Probe available. Good Probe?
		const HLT::TriggerElement *finalFC;
		if ( NavZeeTPBase::isProbeElectron(elProbe, tagTrigItem, finalFC)){
		  std::pair<const xAOD::Electron*,const HLT::TriggerElement*> pairProbe(elProbe,finalFC);
		  probeElectrons.push_back(pairProbe);
		} // end of check Probe
		else {
		  std::pair<const xAOD::Electron*,const HLT::TriggerElement*> pairProbe(elProbe,NULL);
		  probeElectrons.push_back(pairProbe);
		} // still include the probe
		
	    }
	} // end of for in Probe
     } // end of for in Tags


return StatusCode::SUCCESS;
}

void
NavZeeTPBase::clearProbeList(){
/*
for(int i=0;i<probeElectrons.size();i++){
        //if ( probeElectrons[i].second  ) delete probeElectrons[i].second;
}
*/
probeElectrons.clear();
}

bool
NavZeeTPBase::isTagElectron(const xAOD::Electron *el, const unsigned int TagTightness,
	const unsigned int isEM, const float tagMinEt , const std::string trigItem){

    // Tag the event
    // Require offline tight electron
    // Match to e24_tight1_L1EM20V
    //ATH_MSG_DEBUG("Selecting Tag Electron");
     double GeV = 1000.;

     //Check constituents
     if(!el->trackParticle()){
         //ATH_MSG_DEBUG("No track Particle");
         return false;
     }
     if(!el->caloCluster()){
         //ATH_MSG_DEBUG("No caloCluster");
         return false;
     }

     //Require offline Tight++
     //Change to accept and confiugre properties
     unsigned int isEMobtained = m_electronPPCutIDTool[TagTightness-1]->execute(el);
     (void)isEMobtained;
     if( (isEM && m_electronPPCutIDTool[TagTightness-1]->IsemValue()) ) {
         return false;
     }
    
     //Require Et > 25 GeV
     if( !(el->e()/cosh(el->trackParticle()->eta())  > tagMinEt*GeV) ){
       return false;
     }
    
     //fiducial detector acceptance region
     if ( (fabs(el->eta())>1.37 && fabs(el->eta())<1.52) || fabs(el->eta())>2.47 ){
       return false;
     }

     // Check matching to a given trigger
     // The statement below is more general
     if(!m_trigdec->isPassed("HLT_"+trigItem) ) {
        (*m_log) << MSG::DEBUG << "Failed trigger " << trigItem << endreq;
        return false;
     }
     // Get the container of online electrons associated to passed items
     Trig::FeatureContainer fc = m_trigdec->features("HLT_"+trigItem);
     const std::vector< Trig::Feature<xAOD::ElectronContainer> > vec_el = fc.get<xAOD::ElectronContainer>();

     TLorentzVector eloffLV;
     eloffLV.SetPtEtaPhiE(el->pt(), el->trackParticle()->eta(), el->trackParticle()->phi(), el->e());

     bool matched=false;
     for(auto elfeat : vec_el){
	const xAOD::ElectronContainer *elCont = elfeat.cptr();
        for(const auto& eg : *elCont){
            TLorentzVector elLV;
            elLV.SetPtEtaPhiE(eg->pt(), eg->trackParticle()->eta(), eg->trackParticle()->phi(), eg->e());
            if(elLV.DeltaR(eloffLV) < 0.07) matched = true;
        }
     }
     if(!matched) return false; // otherwise, someone matched!

     //ATH_MSG_DEBUG("Found a tag electron");
    return true;
}

bool
NavZeeTPBase::isProbeElectron(const xAOD::Electron *el,
        const std::string trigItem,
	const HLT::TriggerElement*& finalFC){
	finalFC=NULL;

	//Check constituents
	if(!el->trackParticle()){
             //ATH_MSG_DEBUG("No track Particle");
             return false;
        }
        if(!el->caloCluster()){
             //ATH_MSG_DEBUG("No caloCluster");
             return false;
        }

	//fiducial detector acceptance region
	if ( (fabs(el->eta())>1.37 && fabs(el->eta())<1.52) || fabs(el->eta())>2.47 
){
       		return false;
        }

	// Check matching to a given trigger
	// The statement below is more general

        TLorentzVector eloffLV;
        eloffLV.SetPtEtaPhiE(el->pt(), el->trackParticle()->eta(), el->trackParticle()->phi(), el->e());

 	// Get the container of online electrons associated to passed items
	Trig::FeatureContainer fc = (m_trigdec->features("HLT_"+trigItem,TrigDefs::alsoDeactivateTEs));

        const std::vector< Trig::Feature<xAOD::ElectronContainer> > vec_el = fc.get<xAOD::ElectronContainer>("",TrigDefs::alsoDeactivateTEs);

        bool matched=false;
        for(auto elfeat : vec_el){
              const xAOD::ElectronContainer *elCont = elfeat.cptr();
              for(const auto& eg : *elCont){
                 TLorentzVector elLV;
                 elLV.SetPtEtaPhiE(eg->pt(), eg->trackParticle()->eta(), eg->trackParticle()->phi(), eg->e());
                 if(elLV.DeltaR(eloffLV) < 0.07){
		 	finalFC = (elfeat.te());
			return true;
		 }
              }
        }

	const std::vector< Trig::Feature<xAOD::TrigElectronContainer> > vec_l2el = fc.get<xAOD::TrigElectronContainer>("",TrigDefs::alsoDeactivateTEs);

        matched=false;
        for(auto elfeat : vec_l2el){
              const xAOD::TrigElectronContainer *elCont = elfeat.cptr();
              for(const auto& eg : *elCont){
                 TLorentzVector elLV;
                 elLV.SetPtEtaPhiE(eg->pt(), eg->eta(), eg->phi(), eg->e());
                 if(elLV.DeltaR(eloffLV) < 0.07){
			finalFC = (elfeat.te());
			return true;
		 }
              }
        }

        const std::vector< Trig::Feature<TrigRoiDescriptor> > initRois = fc.get<TrigRoiDescriptor>();
	if ( initRois.size() < 1 ) return false;
	Trig::Feature<xAOD::EmTauRoI> itEmTau = m_trigdec->ancestor<xAOD::EmTauRoI>(initRois[0]);

        const xAOD::EmTauRoI *l1 = itEmTau.cptr();
        TLorentzVector elLV;
        elLV.SetPtEtaPhiM(l1->emClus(), l1->eta(), l1->phi(), 0);
        if(elLV.DeltaR(eloffLV) < 0.15) matched = true;
         
	if ( matched ){
		 finalFC = (itEmTau.te());
		 return true;
	}

        return false; // otherwise, someone matched!
}

bool
NavZeeTPBase::passedTrigger(const HLT::TriggerElement* obj){
	bool passed=false;
	if ( obj->getActiveState() ) passed = true;
	return passed;
}

