// this file is -*- C++ -*-
/*
  Copyright (C) 2020, 2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_TCCHELPERS_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_TCCHELPERS_H
#include "GaudiKernel/MsgStream.h"
#
#include <AsgMessaging/MessageCheck.h>
/////////////////////////////////////////////////////////
/// \bried Internal helper class for TCC & UFO building.
/// 
/// This header is meant to be included only from dedicated implementation files.
///
/// \author P-A Delsart

namespace TCCHelpers {

  ///////////////////////////////////////////////////////
  /// Implements a loop over tracks and pflow object to build UFOs
  ///
  /// Building UFO implies running a nested loop on tracks and pflows 2 times :
  ///  - to collect and normalize energy sharing weights
  ///  - to use these weights to actually build the UFO
  /// The loop contains several condition and tests and so is relatively complex.
  /// Thus it is factorized in this helper base class so we avoid to copy-paste it
  /// and risk to introduce buggy differences.
  ///
  /// The 2 clients : TrackCaloClusterInfoAlg (in TrackCaloClusterRecAlgs/) UFOTool (in TrackCaloClusterRecTools/)
  /// define derived classes which implement the only differing parts : the 
  /// methods processPFO() and processTrk()
  ///
  struct CombinedUFOLoop {

    virtual ~CombinedUFOLoop(){}
    
    // These members are used to collect info.
    // They are expected to be set by the clients.
    std::string m_orig_pfoK;
    std::string m_orig_FlowElemK;
    std::string m_clustersLinkK;

  
    
    const CP::ITrackVertexAssociationTool* m_trackVertexAssoTool;
    float m_clusterEcut;
    bool m_useEnergy;
    const SG::ReadDecorHandleKey<xAOD::TrackParticleContainer>* m_linkdecorkey=nullptr;   // in order to access ReadDecorHandles inside the struct, pass the (initialised) key as a pointer from the parent algorithm into the struct, so we can build the ReadDecorHandle inside the struct function. 

    virtual void processPFO(const xAOD::TrackParticle* trk, const xAOD::FlowElement* pfo) = 0;
    virtual void processTrk(const xAOD::TrackParticle* trk) = 0;
    
    virtual void combinedUFOLoop(const TrackCaloClusterInfo *tccInfo, const xAOD::FlowElementContainer* pfos){
      SG::AuxElement::ConstAccessor<ElementLink<xAOD::IParticleContainer> > orig_pfo(m_orig_pfoK);
      SG::AuxElement::ConstAccessor< std::vector<ElementLink<xAOD::CaloClusterContainer> > > clusterLinks(m_clustersLinkK);
      
      
      
      // For performance reasons, we create a map<track, bool> before looping on the tracks.
      std::map<const xAOD::TrackParticle*, bool> isIsolatedMatchedTrack;
      for(const xAOD::FlowElement* pfo: *pfos ){
	if( ! pfo->isCharged() ) continue;
        	
	static const SG::AuxElement::ConstAccessor<int> acc_isInDenseEnvironment("IsInDenseEnvironment");
	if(!acc_isInDenseEnvironment.isAvailable(*pfo)){
	    asg::msgUserCode::ANA_MSG_ERROR("TCCHelpers.h: Dense environ variable not found, skipping given FE");
	    continue;
	}
	if(acc_isInDenseEnvironment(*pfo)==1){
	    continue;
	}
	const xAOD::IParticle* pfo_iparticle=pfo->chargedObjects().at(0);
	//cast to track, since output of chargedObjects is an IParticle
	const xAOD::TrackParticle* pfoTrack=dynamic_cast<const xAOD::TrackParticle*>(pfo_iparticle);
	isIsolatedMatchedTrack[pfoTrack] = true;
      }

      if(m_linkdecorkey==nullptr){
	  asg::msgUserCode::ANA_MSG_ERROR("TCCHelpers.h m_linkdecorkey is not initialised in the struct to do anything, not sure what this means? Skip this case");
	  return;
      }
      const EventContext& ctx =Gaudi::Hive::currentContext();
      SG::ReadDecorHandle<xAOD::TrackParticleContainer, std::vector<ElementLink<xAOD::CaloClusterContainer>> > trackcalodecor(*m_linkdecorkey,ctx); //init the handle

      if(!trackcalodecor.isPresent()){	    
	asg::msgUserCode::ANA_MSG_ERROR("TCCHelpers.h: no link between track and cluster. We need these links : "<<m_linkdecorkey->key());
      }
      
      for(const xAOD::TrackParticle * trk: *tccInfo->allTracks){
	// Only include tracks which aren't associated to a charged PFO -- don't want to double count!
	if (isIsolatedMatchedTrack.find(trk) != isIsolatedMatchedTrack.end() ) continue;

	// if not matched to PV0, ignore track
	if (! m_trackVertexAssoTool->isCompatible(*trk, *tccInfo->pv0) ) continue ;
	//access the link ReadDecorHandle (saved as struct), and then apply it
	
	
	
	
	
        const auto& clustLinks=trackcalodecor(*trk);
	if(clustLinks.empty() ){ 
	  continue;
	}	

	
	// follow the link to the calorimeter clusters
	for( const auto& clLink : clustLinks) {
	    
	  const xAOD::CaloCluster* cluster = *clLink;
      
	  // See if any of the PFO are matched to this cluster
	  for ( const xAOD::FlowElement* pfo : *pfos ){
	    if(pfo->pt()<=0.) continue;
	    
	    const xAOD::IParticle* pfo_orig = nullptr;
	    if ( orig_pfo.isAvailable(*pfo) ) pfo_orig = *orig_pfo( *pfo ) ;
	    else pfo_orig = pfo;
	    
	    if( !(std::abs(pfo_orig->eta() - cluster->rawEta() )<0.01 && std::abs( pfo_orig->phi()  - cluster->rawPhi() )<0.01)) continue; // temp disable origin logic. Matt A: not 100% sure if its needed now anyway	    
	    
	    if( ! pfo->isCharged() ){
	      // Only want to apply this to PFO which aren't able to be subtracted with E/p
	      if(cluster->rawE() < m_clusterEcut) continue;
	    }

	    // this pfo is matched to a cluster associated to the track -->
	    // Call the specialized function to accumulate either :
	    // - E sharing weights (ex: in TrackCaloClusterInfoAlg.cxx)
	    // - clusters/pfo mometum associated to the current trk (ex: TrackCaloClusterTool)
	    processPFO(trk, pfo);

	  } // pfo loop
	} // cluster associated to trk loop

	// When building UFO, the specialized func below will create a combined UFO from this
	// track and the accumulated momentum from the associated cluster/pfo
	processTrk(trk);
    
      } // track loop

      
    }
    
  };

}

#endif
