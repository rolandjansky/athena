/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigHLTJetRec_Cluster: Provide the xAOD::CaloCluster specific method
// needed by TruggerHLTJetRec_Base to for m jets from clusters.

#include "TrigHLTJetRec/TrigHLTJetRecFromCluster.h"
// #include "TrigHLTJetRec/ClusterToPseudoJetConverter.h"

#include <assert.h>
#include <algorithm>


namespace fastjet{
  class PseudoJet;
}

using fastjet::PseudoJet;


TrigHLTJetRecFromCluster::TrigHLTJetRecFromCluster(const std::string& name, 
                                             ISvcLocator* pSvcLocator):
  TrigHLTJetRecBase<xAOD::CaloClusterContainer>(name, pSvcLocator) {
  declareProperty( "scan_type", m_scantype);
}

TrigHLTJetRecFromCluster::~TrigHLTJetRecFromCluster(){}

//PS 3/4/18 HLT::ErrorCode
//PS 3/4/18 TrigHLTJetRecFromCluster::getPseudoJets(const xAOD::CaloClusterContainer* ic,
//PS 3/4/18                                       jet::LabelIndex* indexMap,
//PS 3/4/18                                       jet::PseudoJetVector& pjv){

  // override of base class function - due to CaloClusters
  // having calibration states. CaloCluster will always be run
  // so as to produce calibrated clusters. Some jet chains use
  // uncalibrated clusters.
  //
  // convert elements of DataVector<CaloCluster> to pseudojets
  // after switching the state of the CaloCluster objects to
  // the calibration determined by clusterCalib, which is
  // set when the Algorithm is configured.

//PS 3/4/18 auto clusterCalib = this->getClusterCalib();  // config parameter
//PS 3/4/18 auto uncalibrated = clusterCalib == "EM" ? true : false;

//PS 3/4/18 indexMap->addLabel(clusterCalib + "Topo");
//PS 3/4/18 AnyToPseudoJet<xAOD::CaloClusterContainer::const_value_type> apj(indexMap);
  
//PS 3/4/18 ClusterToPseudoJetConverter converter(apj, uncalibrated);

  // create the pseudojets
//PS 3/4/18 std::transform(ic -> cbegin(),
//PS 3/4/18                ic -> cend(),
//PS 3/4/18                std::back_inserter(pjv),
//PS 3/4/18                converter);

//PS 3/4/18 ATH_MSG_DEBUG("No of pseudojets: " << pjv.size());
//PS 3/4/18 for(auto ps : pjv) {ATH_MSG_VERBOSE("PseudoJetDump " << ps.Et() << " "
//PS 3/4/18                                     << std::boolalpha
//PS 3/4/18                                     << uncalibrated << " "
//PS 3/4/18                                     << clusterCalib);}


//PS 3/4/18 return HLT::OK;

//PS 3/4/18}
//PS 3/4/18 

//PS 3/4/18 HLT::ErrorCode
//PS 3/4/18 TrigHLTJetRecFromCluster::checkforSecondaryPseudoJets(const HLT::TriggerElement* inputTE,
//PS 3/4/18						      jet::LabelIndex* indexMap,
//PS 3/4/18     						      jet::PseudoJetVector& pjv_secondary
//PS 3/4/18                              ){
// checking if set up for retrieval of secondary pseudojets.
//
//PS 3/4/18         if ( secondaryLabelisEmpty() ) {
//PS 3/4/18		ATH_MSG_DEBUG("No secondary label. Will not attempt to retrieve secondary pseudojets.");
//PS 3/4/18		return HLT::OK;
//PS 3/4/18	  }

//PS 3/4/18 	  auto status = this -> getSecondaryPseudoJets(inputTE, indexMap, pjv_secondary);
//PS 3/4/18  	  if (status == HLT::OK) {
//PS 3/4/18ATH_MSG_INFO("Obtained secondary pseudojets (in case of calo clusters)");
//PS 3/4/18  	  }
//PS 3/4/18          else {
//PS 3/4/18		ATH_MSG_WARNING("Secondary objects not found. Jet will be built with an empty secondary pseudojet set.");
    		// don't return here, as the secondary pseudojet getter still needs to be primed, regardless if with an empty vector of pseudojets.
//PS 3/4/18  	  }

//PS 3/4/18  	  status = this -> loadSecondaryPseudoJets(pjv_secondary);
//PS 3/4/18  	  if (status == HLT::OK) {
//PS 3/4/18    		ATH_MSG_INFO("Loaded track pseudojets into pj getter (in case of calo clusters)");
//PS 3/4/18  	  } else {
//PS 3/4/18    		ATH_MSG_WARNING("Failed to load track pseudojets into pj get//PS 3/4/18ter. ");
//PS 3/4/18     	        return status;
//PS 3/4/18  	  }
 
//PS 3/4/18          return HLT::OK;
//PS 3/4/18}

//PS 3/4/18HLT::ErrorCode
//PS 3/4/18TrigHLTJetRecFromCluster::getSecondaryPseudoJets(const HLT::TriggerElement* inputTE,
//PS 3/4/18                                        jet::LabelIndex* indexMap,
//PS 3/4/18                                        jet::PseudoJetVector& pjvtracks){
// retrieving xAOD tracks only if full scan
  
//PS 3/4/18	  ATH_MSG_DEBUG("Full scan reconstruction... retrieving tracks");
//PS 3/4/18	  ATH_MSG_DEBUG("outputTE->getId(): " << inputTE->getId());
 
//PS 3/4/18	  const xAOD::TrackParticleContainer* ictracks = nullptr;
//PS 3/4/18	  ATH_MSG_INFO("About to retrieve track container..");
//PS 3/4/18	  auto hltStatus = getFeature(inputTE, ictracks); 
	
//PS 3/4/18	  if (hltStatus == HLT::OK) {
//PS 3/4/18	          ATH_MSG_INFO("Retrieved the track container at address " << ictracks);
//PS 3/4/18	        } else {
//PS 3/4/18	          ATH_MSG_WARNING("Failed to retrieve the track input container");
//PS 3/4/18	          return HLT::MISSING_FEATURE;
//PS 3/4/18	        }
	      
//PS 3/4/18	  if (ictracks == nullptr){
//PS 3/4/18	          ATH_MSG_WARNING("The track input container address = nullptr, giving up");
//PS 3/4/18	          return HLT::MISSING_FEATURE;
//PS 3/4/18	        }
	     
//PS 3/4/18	  ATH_MSG_DEBUG("Number of secondary incoming (track) objects : " << ictracks->size());

          // now creating pseuodjets for tracks
//PS 3/4/18          auto secondarylabel = getSecondaryLabel();
//PS 3/4/18          int new_index = indexMap->addLabel(secondarylabel);
//PS 3/4/18          ATH_MSG_DEBUG("Index of newly added label fortracks: "<<new_index);
//PS 3/4/18          ATH_MSG_DEBUG("Name of newly added label for tracks: "<<secondarylabel);
//PS 3/4/18          int thisidxfortracks = -1*new_index; // -1 will scale these pseudojets to ghosts.
          // *****************************************************
	 
//PS 3/4/18  	  ATH_MSG_DEBUG("Index map used for tracks: "<<indexMap->name()); // this should be same index map as used for the clusters.
//PS 3/4/18  	  ATH_MSG_DEBUG("Index map size: "<<indexMap->size());
	  
//PS 3/4/18          AnyToPseudoJet<xAOD::TrackParticleContainer::const_value_type> toPseudoJet(indexMap,thisidxfortracks);
//PS 3/4/18          ATH_MSG_DEBUG("I have chosen this index for the ghost tracks: "<<thisidxfortracks);
//PS 3/4/18          ATH_MSG_DEBUG("If negative, these pseudojets will now be scaled to ghosts.");
	   
	  // create the pseudojets
//PS 3/4/18	  std::transform(ictracks -> cbegin(),
//PS 3/4/18	                 ictracks -> cend(),
//PS 3/4/18	                 std::back_inserter(pjvtracks),
//PS 3/4/18	                 toPseudoJet);
	
//PS 3/4/18	  ATH_MSG_INFO("No of secondary pseudojets: " << pjvtracks.size());
//PS 3/4/18  	  for(auto ps : pjvtracks) {ATH_MSG_DEBUG("secondary pseudojet E " << ps.e() << " "<<
//PS 3/4/18                                      "secondary pseudojet pt " << ps.perp() << " "<<
//PS 3/4/18					"eta "<< ps.eta());}
 
//PS 3/4/18  return HLT::OK;
//PS 3/4/18}

//PS 3/4/18HLT::ErrorCode
//PS 3/4/18TrigHLTJetRecFromCluster::loadSecondaryPseudoJets(
//PS 3/4/18                                        PseudoJetVector& pjvtracks){
//PS 3/4/18        ATH_MSG_DEBUG("Loading secondary pseudo jets into getter...");
//PS 3/4/18	if ( !m_secondarypseudoJetGetter.isEnabled() ) {
//PS 3/4/18		ATH_MSG_ERROR("Something went wrong, the secondary PseudoJetGetter is not set.");
//PS 3/4/18		return HLT::ERROR;
//PS 3/4/18	}
//PS 3/4/18	m_secondarypseudoJetGetter->prime(&pjvtracks);
//PS 3/4/18	m_secondarypseudoJetGetter->print();
    
//PS 3/4/18        return HLT::OK;
//PS 3/4/18}

const xAOD::JetContainer* TrigHLTJetRecFromCluster::build() const{
    return TrigHLTJetRecBase<xAOD::CaloClusterContainer>::defaultBuild();
}
