/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigElectronThinningTool.h"

TrigElectronThinningTool::TrigElectronThinningTool(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type, name, parent), 
    m_thinningSvc("IThinningSvc/ThinningSvc", name),
    m_ptThreshold(-1.e30),
    m_followLinks(false)
{
  declareInterface<DerivationFramework::IThinningTool>(this);
  declareProperty("ThinningSvc", m_thinningSvc, "Filter electrons going to the stream ");
  declareProperty("Keys", m_keys, "SG keys of TrigElectron collections over which the thinning should run. No complains objects not found.");
  declareProperty("PtThreshold", m_ptThreshold, "Electrons above this pT will be recorded (unit is MeV)");
  declareProperty("FollowLinks", m_followLinks, "Mark clusters and tracks for thinning");
}

StatusCode TrigElectronThinningTool::initialize() {
  CHECK( m_thinningSvc.retrieve() ); 
  return StatusCode::SUCCESS;
}

namespace {
  bool firstEvent=true;
}
StatusCode TrigElectronThinningTool::doThinning() const {   
  //  ATH_MSG_DEBUG( "what I see" << evtStore()->dump() );
  for ( const auto& key: m_keys ) {
    if ( evtStore()->contains(ClassID_traits<xAOD::TrigElectronContainer>::ID(),key) ) {
      const xAOD::TrigElectronContainer* el(0);
      CHECK( evtStore()->retrieve( el, key) );
      ATH_MSG_DEBUG( "Going over the xAOD::TrigParticleContainer " << key );
      CHECK( selectElectrons(el) );      
    } else {
      if ( firstEvent ) {
	ATH_MSG_DEBUG( "TrigElectron container of key " << key << " absent" );
      }
    }
  }

  firstEvent=false;
  return StatusCode::SUCCESS;
}

StatusCode TrigElectronThinningTool::finalize() {
  return StatusCode::SUCCESS;
}

template<class C>
StatusCode TrigElectronThinningTool::thinLinks(const C* container, const std::set<typename C::const_value_type>& tokeep) const {
  if ( container ) {
    IThinningSvc::VecFilter_t flags(container->size(), false) ;
    for ( size_t i = 0; i < container->size(); ++i ) {
      if ( tokeep.find(container->at(i)) == tokeep.end() ) {
	flags[i] = true;
      }
    }
    CHECK( m_thinningSvc->filter(*container, flags) );
  }
  return StatusCode::SUCCESS;
}



StatusCode TrigElectronThinningTool::selectElectrons(const xAOD::TrigElectronContainer* electronContainer) const {
  ATH_MSG_DEBUG("Electron container of size " << electronContainer->size() );
  IThinningSvc::VecFilter_t electronFlags(electronContainer->size(), false); // we drop all

  const xAOD::TrackParticleContainer* trackContainer(nullptr);
  std::set<const xAOD::TrackParticle*> trackSet;

  const xAOD::TrigEMClusterContainer* clusterContainer(nullptr);
  std::set<const xAOD::TrigEMCluster*> clusterSet;


  for ( size_t i = 0; i <  electronContainer->size(); ++i ) {
    auto electron = electronContainer->at(i);
    if ( electron->pt() >= m_ptThreshold ) {
      electronFlags[i] = true; // keep this
      ATH_MSG_VERBOSE("....... Saving  pT= " << electron->pt() << " eta= " << electron->eta() << " phi= " << electron->phi());
      if ( m_followLinks ) {
	ATH_MSG_VERBOSE("....... Following element links" );
	if ( electron->trackParticleLink().isValid() ) {
	  ATH_MSG_VERBOSE("....... Following element link to track" );
	  trackContainer = electron->trackParticleLink().getStorableObjectPointer();	  
	  const xAOD::TrackParticle*  track = *(electron->trackParticleLink());
	  trackSet.insert(track);
	}
	if ( electron->emClusterLink() ) {
	  ATH_MSG_VERBOSE("....... Following element link to cluster" );
	  clusterContainer = electron->emClusterLink().getStorableObjectPointer();	  
	  const xAOD::TrigEMCluster*  cluster = *(electron->emClusterLink());
	  clusterSet.insert(cluster);	  
	}
      }
    } else {
      ATH_MSG_VERBOSE("....... Dropping pT= " << electron->pt());
    }        
  }
  CHECK( m_thinningSvc->filter(*electronContainer, electronFlags) );

  CHECK( thinLinks(trackContainer, trackSet) );
  CHECK( thinLinks(clusterContainer, clusterSet) );

  return StatusCode::SUCCESS;
}
