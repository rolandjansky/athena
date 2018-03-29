/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigHLTJetRec_Cluster: Provide the xAOD::CaloCluster specific method
// needed by TruggerHLTJetRec_Base to for m jets from clusters.

#include "TrigHLTJetRec/TrigHLTJetRecFromCluster.h"
#include "TrigHLTJetRec/ClusterToPseudoJetConverter.h"

#include <assert.h>
#include <algorithm>


namespace fastjet{
  class PseudoJet;
}

using fastjet::PseudoJet;


TrigHLTJetRecFromCluster::TrigHLTJetRecFromCluster(const std::string& name, 
                                             ISvcLocator* pSvcLocator):
  TrigHLTJetRecBase<xAOD::CaloClusterContainer>(name, pSvcLocator) {
  declareProperty( "secondarypseudoJetGetter", m_secondarypseudoJetGetter = NULL);
  declareProperty( "scan_type", m_scantype);
}

TrigHLTJetRecFromCluster::~TrigHLTJetRecFromCluster(){}

HLT::ErrorCode
TrigHLTJetRecFromCluster::retrieveSecondaryPseudoJetGetter(){
  
	ATH_MSG_DEBUG("Retrieving track pseudojet getter...");
	if  (m_secondarypseudoJetGetter.retrieve().isSuccess()){
      		ATH_MSG_DEBUG("Retrieved  secondary PseudoJetGetter "
                   <<  m_secondarypseudoJetGetter->name());
  	} else {
    		ATH_MSG_DEBUG("Unable to retrieve secondary PseudoJetGetter");
    		return HLT::ERROR;
  	}
 
  return HLT::OK;
}

HLT::ErrorCode
TrigHLTJetRecFromCluster::getPseudoJets(const xAOD::CaloClusterContainer* ic,
                                        LabelIndex* indexMap,
                                        PseudoJetVector& pjv){

  // override of base class function - due to CaloClusters
  // having calibration states. CaloCluster will always be run
  // so as to produce calibrated clusters. Some jet chains use
  // uncalibrated clusters.
  //
  // convert elements of DataVector<CaloCluster> to pseudojets
  // after switching the state of the CaloCluster objects to
  // the calibration determined by clusterCalib, which is
  // set when the Algorithm is configured.

  auto clusterCalib = this->getClusterCalib();  // config parameter
  auto uncalibrated = clusterCalib == "EM" ? true : false;

  indexMap->addLabel(clusterCalib + "Topo");
  AnyToPseudoJet<xAOD::CaloClusterContainer::const_value_type> apj(indexMap);
  
  ClusterToPseudoJetConverter converter(apj, uncalibrated);

  // create the pseudojets
  std::transform(ic -> cbegin(),
                 ic -> cend(),
                 std::back_inserter(pjv),
                 converter);

  ATH_MSG_DEBUG("No of pseudojets: " << pjv.size());
  for(auto ps : pjv) {ATH_MSG_VERBOSE("PseudoJetDump " << ps.Et() << " "
                                      << std::boolalpha
                                      << uncalibrated << " "
                                      << clusterCalib);}


  return HLT::OK;

}


HLT::ErrorCode
TrigHLTJetRecFromCluster::checkforSecondaryPseudoJets(const HLT::TriggerElement* inputTE,
						      LabelIndex* indexMap,
      						      PseudoJetVector& pjv_secondary
                                        					){
  // checking if set up for retrieval of secondary pseudojets.
  //
     	  if (m_scantype == "PS" ) {
		ATH_MSG_DEBUG("Scan type of PS. No ghost associations will be attempted.");
		return HLT::OK;
   	  }
	  ATH_MSG_DEBUG("Of scan type FS: About to check if secondary pseudojetgetter label exists.");		
	
          if (m_secondarylabel == "") {
		ATH_MSG_DEBUG("No secondary label. Will not attempt to retrieve secondary pseudojets.");
		return HLT::OK;
	  }

 	  auto status = this -> getSecondaryPseudoJets(inputTE, indexMap, pjv_secondary);
  	  if (status == HLT::OK) {
    		ATH_MSG_DEBUG("Obtained secondary pseudojets (in case of calo clusters)");
  	  }
          else {
		ATH_MSG_WARNING("Secondary objects not found. Jet will be built with an empty secondary pseudojet set.");
    		//return HLT::OK; // don't return here, as the secondary pseudojet getter still needs to be primed, regardless if with an empty vector of pseudojets.
  	  }

          auto sc_secondarygetter = this->retrieveSecondaryPseudoJetGetter();
  	  if (sc_secondarygetter == HLT::OK) {
        	ATH_MSG_DEBUG("Secondary pseudojet getter succesfully retrieved.");
  	  } else {
    		ATH_MSG_WARNING("Error in retrieving shared PseudoJetGetter. Does the chain require a secondary pseudojetgetter?");
    		return HLT::OK;
	  }

  	  status = this -> loadSecondaryPseudoJets(pjv_secondary);
  	  if (status == HLT::OK) {
    		ATH_MSG_DEBUG("Loaded track pseudojets into pj getter (in case of calo clusters)");
  	  } else {
    		ATH_MSG_WARNING("Failed to load track pseudojets into pj getter. ");
     	        return status; // actually no error handling here.
  	  }
 
          return HLT::OK;
}


HLT::ErrorCode
TrigHLTJetRecFromCluster::getSecondaryPseudoJets(const HLT::TriggerElement* inputTE,
                                        LabelIndex* indexMap,
                                        PseudoJetVector& pjvtracks){
         // retrieving xAOD tracks only if full scan
  
	  ATH_MSG_DEBUG("Full scan reconstruction... retrieving tracks");
	  ATH_MSG_DEBUG("outputTE->getId(): " << inputTE->getId());
 
	  const xAOD::TrackParticleContainer* ictracks = nullptr;
	  ATH_MSG_DEBUG("About to retrieve track container..");
	  auto hltStatus = getFeature(inputTE, ictracks); 
	
	  if (hltStatus == HLT::OK) {
	          ATH_MSG_DEBUG("Retrieved the track container at address " << ictracks);
	        } else {
	          ATH_MSG_WARNING("Failed to retrieve the track input container");
	          return HLT::MISSING_FEATURE; //maybe?
	        }
	      
	  if (ictracks == nullptr){
	          ATH_MSG_WARNING("The track input container address = nullptr, giving up");
	          return HLT::MISSING_FEATURE;
	        }
	     
	  ATH_MSG_DEBUG("Number of secondary incoming (track) objects : " << ictracks->size());

          // now creating pseuodjets for tracks
          int new_index = indexMap->addLabel(m_secondarylabel);
          ATH_MSG_DEBUG("Index of newly added label fortracks: "<<new_index);
          ATH_MSG_DEBUG("Name of newly added label for tracks: "<<m_secondarylabel);
          int thisidxfortracks = -1*new_index; // -1 will scale these pseudojets to ghosts.
          // *****************************************************
	 
  	  ATH_MSG_DEBUG("Index map used for tracks: "<<indexMap->name()); // this should be same index map as used for the clusters.
  	  ATH_MSG_DEBUG("Index map size: "<<indexMap->size());
	  
          AnyToPseudoJet<xAOD::TrackParticleContainer::const_value_type> toPseudoJet(indexMap,thisidxfortracks);
          ATH_MSG_DEBUG("I have chosen this index for the ghost tracks: "<<thisidxfortracks);
          ATH_MSG_DEBUG("If negative, these pseudojets will now be scaled to ghosts.");
	   
	  // create the pseudojets
	  std::transform(ictracks -> cbegin(),
	                 ictracks -> cend(),
	                 std::back_inserter(pjvtracks),
	                 toPseudoJet);
	
	  ATH_MSG_DEBUG("No of secondary pseudojets: " << pjvtracks.size());
  	  for(auto ps : pjvtracks) {ATH_MSG_DEBUG("secondary pseudojet E " << ps.e() << " "<<
                                      "secondary pseudojet pt " << ps.perp() << " "<<
					"eta "<< ps.eta());}
 
  return HLT::OK;
}

HLT::ErrorCode
TrigHLTJetRecFromCluster::loadSecondaryPseudoJets(
                                        PseudoJetVector& pjvtracks){
        ATH_MSG_DEBUG("Loading secondary pseudo jets into getter...");
	m_secondarypseudoJetGetter->prime(&pjvtracks);
	m_secondarypseudoJetGetter->print();
    
        return HLT::OK;
}

const xAOD::JetContainer* TrigHLTJetRecFromCluster::build() const{
    return TrigHLTJetRecBase<xAOD::CaloClusterContainer>::defaultBuild();
}
