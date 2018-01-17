/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigHLTJetHemisphereRec:Trigger algorithm that reads jet collection from
// an incoming trigger element, looks at all combinatorics for a hypothesis of
// two megajet objects. Hemispheres are chosen such that they
// minimize the mass of the di-hemisphere system. The resulting jet collection 
// contains 2 hemisphere 4-vectors and is attatched to the outgoing trigger element.
//

#include "TrigHLTJetHemisphereRec/TrigHLTJetHemisphereRec.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetTrigAuxContainer.h"
#include "JetEDM/IndexedConstituentUserInfo.h"

#include "xAODJet/Jet.h"

#include <algorithm>

TrigHLTJetHemisphereRec::TrigHLTJetHemisphereRec(const std::string& name, ISvcLocator* pSvcLocator ):
	HLT::FexAlgo( name, pSvcLocator ),
	m_totalTimer(nullptr),
	m_hemiFindingTimer(nullptr)
{
	declareProperty( "jetPtCut", m_jetPtCut = 30000. );
	declareProperty( "jetEtaCut", m_jetEtaCut = 3.2 );
	declareProperty( "nJetPassThrough", m_nJetPassThrough = 10 );
}


TrigHLTJetHemisphereRec::~TrigHLTJetHemisphereRec() { }


HLT::ErrorCode TrigHLTJetHemisphereRec::hltInitialize() {

	// StatusCode sc;
	ATH_MSG_INFO("Initializing " << name() << "...");

	// Initialize timing service
	//------------------------------
	if(timerSvc() ){
		m_totalTimer       = addTimer("TrigHLTJetHemisphereRec.TrigHLTJetHemisphereRecTot");
		m_hemiFindingTimer = addTimer("TrigHLTJetHemisphereRec.TrigHLTJetHemisphereRecHemiFind");
	}

	ATH_MSG_INFO("Leaving initialize in "  << name()  );

	return HLT::OK;

}


HLT::ErrorCode TrigHLTJetHemisphereRec::hltFinalize() {
	ATH_MSG_INFO ("Finalizing " << name() << "...");
	return HLT::OK;
}


HLT::ErrorCode TrigHLTJetHemisphereRec::acceptInput(const HLT::TriggerElement* inputTE, bool& pass){

	// Time total TrigEFRazorAllTE execution time.
	// -------------------------------------
	if (timerSvc() ) m_totalTimer->start();


	ATH_MSG_DEBUG("In acceptInput() for " << name() << "...");


	// Incoming jet container //////////////////////////////////////////

	const xAOD::JetContainer*  jetContainer = 0;
	HLT::ErrorCode ec = getFeature(inputTE, jetContainer);

	if(ec!=HLT::OK) {
		msg() << MSG::WARNING << " Failed to get the Jets " << endmsg;
		return ec;
	}

	if (jetContainer == 0){
		ATH_MSG_ERROR("jetContainer address = 0, giving up");
		return HLT::ERROR;
	}

	ATH_MSG_DEBUG("Number of incoming jets: " << jetContainer->size());


	// Got input jet collection /////////////////////////////////////////

	// Preselected jet container ////////////////////////////////////////

	m_selectedInputJetContainer.clear();

	// Attempt to do with STL below but failed for the time being... //
	for (xAOD::JetContainer::const_iterator jet = jetContainer->begin(); jet != jetContainer->end(); jet++) {
		if((*jet)->pt() < m_jetPtCut ) continue;
		if( fabs((*jet)->eta() ) > m_jetEtaCut ) continue;
		m_selectedInputJetContainer.push_back( (*jet)->jetP4() );
	}

	// remove_copy_if attempted but never got to work //
	// std::remove_copy_if( jetContainer->begin(), jetContainer->end(), std::back_inserter(m_selectedInputJetContainer), passJet);

	if (m_selectedInputJetContainer.size() < 2 ){ // njet < 2 is undefined
		pass = false;
	} else {
		pass = true;
	}

	return HLT::OK;
}




HLT::ErrorCode TrigHLTJetHemisphereRec::hltExecute(const HLT::TriggerElement* inputTE,
	HLT::TriggerElement* outputTE) {  

	ATH_MSG_DEBUG("Executing " << name() << "...");

	ATH_MSG_DEBUG("outputTE->getId(): " << outputTE->getId());
	ATH_MSG_DEBUG("inputTE->getId(): " << inputTE->getId());


	// Output jet hemisphere container creation ///////////////////////////

	xAOD::JetContainer* jetHemisphereContainer = new xAOD::JetContainer();
	xAOD::JetTrigAuxContainer theJetHemisphereContainer;
	jetHemisphereContainer->setStore(&theJetHemisphereContainer);

	if (timerSvc() ) m_hemiFindingTimer->start();


	// Only looking at first NJetMax jets to cap the algo time. //////////
	// Replaces trigger fail if NJet > NJetMax ///////////////////////////
	// New as of 4/22 - LL ///////////////////////////////////////////////

	m_NJets = m_selectedInputJetContainer.size() ;

	ATH_MSG_DEBUG("size of m_selectedInputJetContainer " << m_NJets );

	if (m_NJets > m_nJetPassThrough){
		ATH_MSG_DEBUG("truncating jets to NJetMax of " << m_nJetPassThrough );
		m_NJets = m_nJetPassThrough;
	}

	// Sorting jets by pT
	std::sort(m_selectedInputJetContainer.begin(), m_selectedInputJetContainer.end(), 
		[](xAOD::JetFourMom_t a, xAOD::JetFourMom_t b){return a.Pt() > b.Pt();});




	/////////// Starting Hemisphere Reconstruction ////////////////////////
	/////////// Adapted from Chris Rogan //////////////////////////////////

	xAOD::JetFourMom_t h1,h2;
	xAOD::JetFourMom_t h1temp,h2temp;


	// m_NJets == 2 is trivial - just return two jets as the hems
	if(m_NJets == 2){
		h1 = m_selectedInputJetContainer.at(0);
		h2 = m_selectedInputJetContainer.at(1);
	} else {

		// m_NJets > 2 - do the same thing for all cases

		// _now_ we fill our 4-vector arrays, along
		// with some other info

		m_Ptot[0] = m_Ptot[1] = m_Ptot[2] = m_Ptot[3] = 0.;
		for(int i = 0; i < m_NJets; i++){
			m_jetP[i][0] = m_selectedInputJetContainer.at(i).Px();
			m_jetP[i][1] = m_selectedInputJetContainer.at(i).Py();
			m_jetP[i][2] = m_selectedInputJetContainer.at(i).Pz();
			m_jetP[i][3] = m_selectedInputJetContainer.at(i).E();
			m_Ptot[0] += m_jetP[i][0];
			m_Ptot[1] += m_jetP[i][1];
			m_Ptot[2] += m_jetP[i][2];
			m_Ptot[3] += m_jetP[i][3];
		}
		// calculate boost to CM
		if(m_Ptot[3]){
			m_beta[0] = m_Ptot[0]/m_Ptot[3];
			m_beta[1] = m_Ptot[1]/m_Ptot[3];
			m_beta[2] = m_Ptot[2]/m_Ptot[3];
		} else {
			m_beta[0] = 0.;
			m_beta[1] = 0.;
			m_beta[2] = 0.;
		}

		if(1.-m_beta[0]*m_beta[0]-m_beta[1]*m_beta[1]-m_beta[2]*m_beta[2]){
			m_gamma = 1./sqrt(1.-m_beta[0]*m_beta[0]-m_beta[1]*m_beta[1]-m_beta[2]*m_beta[2]);
		} else {
			m_gamma = 0.;
		}
		// do boost to CM
		for(int i = 0; i < m_NJets; i++){
			m_dot = m_beta[0]*m_jetP[i][0]+m_beta[1]*m_jetP[i][1]+m_beta[2]*m_jetP[i][2];

			if((m_gamma+1)){
				m_dotgamma = m_dot*m_gamma/(m_gamma+1) - m_jetP[i][3];
			} else {
				m_dotgamma = 0.;
			}
			m_jetP[i][0] = m_jetP[i][0] + m_gamma*m_beta[0]*m_dotgamma;
			m_jetP[i][1] = m_jetP[i][1] + m_gamma*m_beta[1]*m_dotgamma;
			m_jetP[i][2] = m_jetP[i][2] + m_gamma*m_beta[2]*m_dotgamma;
			m_jetP[i][3] = m_gamma*(m_jetP[i][3] - m_dot);
		}
		
		m_P_max = 0.;

		// Loop over all 2-jet seed probes
		for(m_ip[0] = 0; m_ip[0] < m_NJets-1; m_ip[0]++){
			for(m_ip[1] = m_ip[0]+1; m_ip[1] < m_NJets; m_ip[1]++){
				m_Pref[0] = m_jetP[m_ip[0]][1]*m_jetP[m_ip[1]][2]-m_jetP[m_ip[0]][2]*m_jetP[m_ip[1]][1];
				m_Pref[1] = m_jetP[m_ip[0]][2]*m_jetP[m_ip[1]][0]-m_jetP[m_ip[0]][0]*m_jetP[m_ip[1]][2];
				m_Pref[2] = m_jetP[m_ip[0]][0]*m_jetP[m_ip[1]][1]-m_jetP[m_ip[0]][1]*m_jetP[m_ip[1]][0];

				m_Nhem[0] = 0;
				m_Nhem[1] = 0;
				m_Phem[0] = 0.;
				m_Phem[1] = 0.;
				m_Phem[2] = 0.;

				// Loop over all jets
				for(int i = 0; i < m_NJets; i++){
					if((i == m_ip[0]) || (i ==m_ip[1])) continue;
					int ihem = int(m_Pref[0]*m_jetP[i][0]+m_Pref[1]*m_jetP[i][1]+m_Pref[2]*m_jetP[i][2] > 0.);
					m_Nhem[ihem]++;
					if(ihem == 1) continue;
					m_Phem[0] += m_jetP[i][0];
					m_Phem[1] += m_jetP[i][1];
					m_Phem[2] += m_jetP[i][2];
				}

			// assign 2 probes
				for(m_jp[0] = 0; m_jp[0] < 2; m_jp[0]++){
					for(m_jp[1] = 0; m_jp[1] < 2; m_jp[1]++){
						if(m_jp[0] == m_jp[1] && m_Nhem[(m_jp[0]+1)%2] == 0) continue;

						m_Phem_probe[0] = m_Phem[0];
						m_Phem_probe[1] = m_Phem[1];
						m_Phem_probe[2] = m_Phem[2];

						if(m_jp[0] == 0){
							m_Phem_probe[0] += m_jetP[m_ip[0]][0];
							m_Phem_probe[1] += m_jetP[m_ip[0]][1];
							m_Phem_probe[2] += m_jetP[m_ip[0]][2];
						}
						if(m_jp[1] == 0){
							m_Phem_probe[0] += m_jetP[m_ip[1]][0];
							m_Phem_probe[1] += m_jetP[m_ip[1]][1];
							m_Phem_probe[2] += m_jetP[m_ip[1]][2];
						}

						double P_trial = m_Phem_probe[0]*m_Phem_probe[0]+
						m_Phem_probe[1]*m_Phem_probe[1]+m_Phem_probe[2]*m_Phem_probe[2];

						if(P_trial > m_P_max){
							m_P_max = P_trial;
							m_ip_max[0] = m_ip[0];
							m_ip_max[1] = m_ip[1];
							m_jp_max[0] = m_jp[0];
							m_jp_max[1] = m_jp[1];
						}
					}
				}
			}
		}

		// Now we build two return hemispheres
		m_Pret[0][0] = m_Pret[0][1] = m_Pret[0][2] = m_Pret[0][3] = 0.;
		m_Pret[1][0] = m_Pret[1][1] = m_Pret[1][2] = m_Pret[1][3] = 0.;
		m_Pret[m_jp_max[0]][0] += m_jetP[m_ip_max[0]][0];
		m_Pret[m_jp_max[0]][1] += m_jetP[m_ip_max[0]][1];
		m_Pret[m_jp_max[0]][2] += m_jetP[m_ip_max[0]][2];
		m_Pret[m_jp_max[0]][3] += m_jetP[m_ip_max[0]][3];
		m_Pret[m_jp_max[1]][0] += m_jetP[m_ip_max[1]][0];
		m_Pret[m_jp_max[1]][1] += m_jetP[m_ip_max[1]][1];
		m_Pret[m_jp_max[1]][2] += m_jetP[m_ip_max[1]][2];
		m_Pret[m_jp_max[1]][3] += m_jetP[m_ip_max[1]][3];

		m_Pref[0] = m_jetP[m_ip_max[0]][1]*m_jetP[m_ip_max[1]][2]-m_jetP[m_ip_max[0]][2]*m_jetP[m_ip_max[1]][1];
		m_Pref[1] = m_jetP[m_ip_max[0]][2]*m_jetP[m_ip_max[1]][0]-m_jetP[m_ip_max[0]][0]*m_jetP[m_ip_max[1]][2];
		m_Pref[2] = m_jetP[m_ip_max[0]][0]*m_jetP[m_ip_max[1]][1]-m_jetP[m_ip_max[0]][1]*m_jetP[m_ip_max[1]][0];

		for(int i = 0; i < m_NJets; i++){
			if((i == m_ip_max[0]) || (i == m_ip_max[1])) continue;

			int ihem = (m_Pref[0]*m_jetP[i][0]+m_Pref[1]*m_jetP[i][1]+m_Pref[2]*m_jetP[i][2] > 0.);

			m_Pret[ihem][0] += m_jetP[i][0];
			m_Pret[ihem][1] += m_jetP[i][1];
			m_Pret[ihem][2] += m_jetP[i][2];
			m_Pret[ihem][3] += m_jetP[i][3];
		}

		// boost return hemispheres back to lab frame
		// and fill return vector
		for(int i = 0; i < 2; i++){
			m_dot = -m_beta[0]*m_Pret[i][0]-m_beta[1]*m_Pret[i][1]-m_beta[2]*m_Pret[i][2];
			if((m_gamma+1)){
				m_dotgamma = m_dot*m_gamma/(m_gamma+1) - m_Pret[i][3];
			} else {
				m_dotgamma = 0.;
			}
			m_Pret[i][0] = m_Pret[i][0] - m_gamma*m_beta[0]*m_dotgamma;
			m_Pret[i][1] = m_Pret[i][1] - m_gamma*m_beta[1]*m_dotgamma;
			m_Pret[i][2] = m_Pret[i][2] - m_gamma*m_beta[2]*m_dotgamma;
			m_Pret[i][3] = m_gamma*(m_Pret[i][3] - m_dot);
		}

		h1.SetPxPyPzE(m_Pret[0][0],m_Pret[0][1],m_Pret[0][2],m_Pret[0][3]);
		h2.SetPxPyPzE(m_Pret[1][0],m_Pret[1][1],m_Pret[1][2],m_Pret[1][3]);

	}




	// Why not have hemisphere 1 be the harder one?... ///////

	if(h2.Pt() > h1.Pt() ){
		xAOD::JetFourMom_t temp = h1;
		h1 = h2;
		h2 = temp;
	}

	// Final jet objects that will go into the output collection /////////////

	xAOD::Jet* aJet = new xAOD::Jet();
	xAOD::Jet* bJet = new xAOD::Jet();

	ATH_MSG_DEBUG("About to fill hemisphere container: " << jetHemisphereContainer->size() << " " << h1.Pt() << " " << h2.Pt() );

	jetHemisphereContainer->push_back(aJet);
	jetHemisphereContainer->push_back(bJet);

	aJet->setJetP4( h1 );
	bJet->setJetP4( h2 );


	HLT::ErrorCode hltStatus;

	std::string key = "";

	// Put the hemisphere container into the output TE ///////////////////////

	hltStatus=recordAndAttachFeature(outputTE, jetHemisphereContainer,key,"TrigHLTJetHemisphereRec");

	ATH_MSG_DEBUG("Number of output hemispheres (should be 2): " << jetHemisphereContainer->size());

	// Time total TrigEFRazorAllTE execution time.
	// -------------------------------------

	if (timerSvc() ) m_totalTimer->stop();
	if (timerSvc() ) m_hemiFindingTimer->stop();

	if (timerSvc() ) ATH_MSG_DEBUG("Algorithm took time: " << m_totalTimer->elapsed() ) ;
	if (timerSvc() ) ATH_MSG_DEBUG("Hemisphere finding took time: " << m_hemiFindingTimer->elapsed() ) ;

	return hltStatus;
}



HLT::ErrorCode TrigHLTJetHemisphereRec::attachJetCollection(HLT::TriggerElement* outputTE,
	const xAOD::JetContainer* j_container){

	// Function stolen from existing jet trigger code

	// We have to explicitly delete the aux store, so get a pointer to it.
	auto auxStore = j_container->getStore();
	std::string label = "TrigHLTJetHemisphereRec";
	std::string key = "";

	HLT::ErrorCode hltStatus = 
	recordAndAttachFeature(outputTE, j_container, key, label);

	// cleanup
	if (hltStatus != HLT::OK) {
		// this is unrecoverable. report error, and do not try to clean up.
		ATH_MSG_ERROR("Failed to attach xAOD::JetContainer into outputTE- status"
			<< hltStatus);
	} else {
		// j_container.setStore(0) is called in downstream code, so the
		// aux container needs to deleted explcitly here.
		delete auxStore;
		ATH_MSG_DEBUG("Attached xAOD::JetContainer into outputTE");
	}
	return hltStatus;
}


// bool TrigHLTJetHemisphereRec::passJet(xAOD::Jet* jet){
//     if(jet->pt() < m_jetPtCut ) return false;
//     if( fabs(jet->eta() ) > m_jetEtaCut ) return false;

//     return true;
// }


