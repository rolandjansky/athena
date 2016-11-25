/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Extra documentation (doxygen) in header file

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthContainers/DataVector.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTracking/TrackParticle.h"

#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Photon.h"
#include "xAODTrigger/TrigPassBits.h"
#include "TrigTauHypo/EFPhotonTauFex.h"

//Constructor
EFPhotonTauFex::EFPhotonTauFex(const std::string& name, ISvcLocator* pSvcLocator) : HLT::ComboAlgo(name, pSvcLocator){

	m_cont = 0;

}

//Destructor
EFPhotonTauFex::~EFPhotonTauFex(){}


HLT::ErrorCode EFPhotonTauFex::hltInitialize()
{
	msg() << MSG::INFO << "in initialize()" << endmsg;
	return HLT::OK;
}

HLT::ErrorCode EFPhotonTauFex::hltExecute(HLT::TEConstVec& /*inputTE*/, HLT::TriggerElement* outputTE)
{
	ATH_MSG_DEBUG("EFPhotonTauFex Execute");
	
	m_cont=new xAOD::TrigCompositeContainer();
  	xAOD::TrigCompositeAuxContainer contaux;
	m_cont->setStore(&contaux);
        for(const auto &mass:m_mvis) {
	   xAOD::TrigComposite *comp=new xAOD::TrigComposite();
	   m_cont->push_back(comp);
	   comp->setName("EFPhotonTau_mVis");
    	   comp->setDetail("mVis",mass);
        }
	attachFeature(outputTE,m_cont,"EFPhotonTauInfo"); 

	ATH_MSG_DEBUG("EFPhotonTauInfo attached feature");
	return HLT::OK;
}

HLT::ErrorCode EFPhotonTauFex::hltFinalize()
{
	ATH_MSG_DEBUG("EFPhotonTauFex Finalize");
	return HLT::OK;
}

HLT::ErrorCode EFPhotonTauFex::acceptInputs(HLT::TEConstVec& inputTE, bool& pass )
{
	pass = false;
	m_mvis.clear();	

  	ATH_MSG_DEBUG("Running EFPhotonTauFex::acceptInputs");

  	if ( inputTE.size() != 2 ) {
    		ATH_MSG_ERROR( "Got diferent than 2 number of input TEs: " <<  inputTE.size() << " job badly configured");
    		return HLT::BAD_JOB_SETUP;
  	}

        std::vector<const xAOD::TauJetContainer*> vectorTauContainers;
        std::vector<const xAOD::PhotonContainer*> vectorEgammaContainers; 

	const HLT::TriggerElement* te1 = inputTE[0];
  	const HLT::TriggerElement* te2 = inputTE[1];

	HLT::ErrorCode status1 = getFeatures(te1, vectorTauContainers);
	HLT::ErrorCode status2 = getFeatures(te2, vectorEgammaContainers);
	if (status1 != HLT::OK || status2 != HLT::OK) {
	  msg() << MSG::WARNING << "Failed to get xAOD::PhotonContainer and xAOD::TauJetContainer collections" << endmsg;
	  return HLT::MISSING_FEATURE;
	} else {
	  msg() << MSG::DEBUG << "xAOD::PhotonContainer and xAOD::TauJetContainer collections successfully retrieved" << endmsg;
	}

  	if (vectorEgammaContainers.size() < 1 || vectorTauContainers.size() < 1) {
    		msg() << MSG::WARNING << " empty xAOD::PhotonContainer or xAOD::TauJetContainer from the trigger elements" << endmsg;
    		return HLT::OK;
  	}  

  	const xAOD::PhotonContainer* photonContainer1 = vectorEgammaContainers.back();
        const xAOD::TauJetContainer* TauJetContainer1 = vectorTauContainers.back();

        const xAOD::TrigPassBits* bits(0);
	if (getFeature(te2, bits, "passbits") != HLT::OK) {
           ATH_MSG_WARNING(" Failed to get TrigPassBits ");
           return HLT::MISSING_FEATURE;
        }

	if(bits == 0){
           ATH_MSG_WARNING(" Failed to get TrigPassBits ");
           return HLT::MISSING_FEATURE;
        }

	// loop over all combinations
	xAOD::PhotonContainer::const_iterator photon;
	xAOD::TauJetContainer::const_iterator tau;
	
        //TODO: add monitoring of container sizes
        
        for ( photon = photonContainer1->begin(); photon != photonContainer1->end(); ++photon ) {

		if((*photon)==0){ 
                    msg() << MSG::WARNING << "Null pointer in egammaContainer. Skipping." << endmsg;
		    continue;
                }

                if(!bits->isPassing((*photon),photonContainer1)){
			msg() << MSG::WARNING << "Photon found not passing Hypo object" << endmsg;
			continue;
		}

		const xAOD::CaloCluster* clus = (*photon)->caloCluster();
		if(!clus) { msg() << MSG::WARNING << "REGTEST no cluster pointer in egamma object " << endmsg;
			continue;}
		
		float gphi = clus->phi();
		float geta = clus->eta();
		float gpt  = clus->et();
	
		for ( tau = TauJetContainer1->begin(); tau != TauJetContainer1->end(); ++tau ) {

			TLorentzVector tlv_photon_tau(0.,0.,0.,0.);
                	tlv_photon_tau.SetPtEtaPhiM(gpt,geta,gphi,0.);

			if((*tau)->nTracks()==0){
				msg() << MSG::DEBUG << " 0prong tau skipped " <<endmsg;
				continue;
			}			

			for (unsigned int i=0;i<(*tau)->nTracks();++i) {
				const xAOD::TrackParticle* trk = 0;
				TLorentzVector tmpKaon(0.,0.,0.,0.);
				
				try 
      				{
                                        #ifndef XAODTAU_VERSIONS_TAUJET_V3_H
       					trk = (*tau)->track(i);
					#else
        				trk = (*tau)->track(i)->track();
					#endif
      				}
      				catch(std::exception e)
      				{
        				msg() << MSG::WARNING << " REGTEST: EFPhotonTauFex, failed to get tau track link! " <<endmsg;
      				} 
      				if(trk) {
         				tmpKaon.SetPtEtaPhiM(trk->pt(), trk->eta(), trk->phi(), 493.677); //TODO: kaon mass?
         				tlv_photon_tau = tlv_photon_tau + tmpKaon;
      				}

			}			

			// store vismass in container
			float mass = tlv_photon_tau.M();
			m_mvis.push_back(mass);
			msg() << MSG::DEBUG << " photon+tau mass " << mass <<endmsg;

		} // end tau loop
	} // end photon loop

	pass = true;
	return HLT::OK;
}
