///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METPhotonTool.cxx 
// Implementation file for class METPhotonTool
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METPhotonTool.h"

// MET EDM
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAuxComponentMap.h"

// Egamma EDM
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronxAODHelpers.h"

namespace met {

  using std::vector;
  //
  using xAOD::IParticle;
  //
  using xAOD::Photon;
  using xAOD::PhotonContainer;
  //
  using xAOD::CaloCluster;
  using xAOD::CaloClusterContainer;
  //
  using xAOD::Vertex;
  using xAOD::TrackParticle;
  //
  using xAOD::MissingET;
  using xAOD::MissingETComposition;
  using xAOD::MissingETComponent;
  using xAOD::MissingETComponentMap;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 

  // Constructors
  ////////////////
  METPhotonTool::METPhotonTool(const std::string& name) : 
    AsgTool(name),
    METBuilderTool(name),
    METEgammaTool(name)
  {
  }

  // Destructor
  ///////////////
  METPhotonTool::~METPhotonTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METPhotonTool::initialize()
  {
    ATH_CHECK( METEgammaTool::initialize() );
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");

    // Provide parser of input data string here? 
    // Or take a list of inputs?

    return StatusCode::SUCCESS;
  }

  StatusCode METPhotonTool::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 

  bool METPhotonTool::resolveOverlap(const xAOD::IParticle* object,
				     xAOD::MissingETComponentMap* metMap,
				     std::vector<const xAOD::IParticle*>& acceptedSignals,
				     MissingETBase::Types::weight_t& objWeight) const
  {

    if(object->type() != xAOD::Type::Photon) {
      ATH_MSG_WARNING("METPhotonTool::resolveOverlap given an object of type " << object->type());
      return false;
    }
    const Photon* ph = static_cast<const Photon*>(object);

    ATH_MSG_DEBUG("Identifying signals overlapping this photon");

    const CaloClusterContainer* tcCont(0);
    if( evtStore()->retrieve(tcCont, m_tcCont_key).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve topocluster container for overlap removal");
      return true;
    }
    ATH_MSG_DEBUG("Successfully retrieved topocluster collection");    

    // retrieve topoclusters associated to the photon
    vector<const IParticle*> tcList;
    tcList.reserve(ph->nCaloClusters());
    matchTopoClusters(ph, tcList, tcCont);
    // test the clusters for matches to other objects
    bool clustersUsed = metMap->checkUsage(tcList,MissingETBase::UsageHandler::OnlyCluster);
    if(clustersUsed) { // true implies some cluster has been used
      for(vector<const IParticle*>::const_iterator iClus = tcList.begin();
	  iClus!=tcList.end(); ++iClus) {
	acceptedSignals.push_back(*iClus);
      } // loop over photon-matched topoclusters
      objWeight = MissingETBase::Types::weight_t(0.,0.,0.);
    } else {
      acceptedSignals = tcList;
      objWeight = MissingETBase::Types::weight_t(1.,1.,1.);
    }

    if(m_eg_doTracks) {
      // retrieve tracks associated to the photon
      vector<const IParticle*> trkList;
      trkList.reserve(2*ph->nVertices());
      matchTracks(ph, trkList);
      // test the tracks for matches to other objects
      metMap->checkUsage(trkList,MissingETBase::UsageHandler::OnlyTrack);
      for(vector<const IParticle*>::const_iterator iTrk = trkList.begin();
	  iTrk!=trkList.end(); ++iTrk) {
	acceptedSignals.push_back(*iTrk);
      } // loop over photon-matched tracks
    }

    return !clustersUsed; // return true if the photon shares no clusters with another object
  }

  void METPhotonTool::matchTracks(const xAOD::Photon* ph, std::vector<const xAOD::IParticle*>& trklist) const
  {
    for(size_t iVtx=0; iVtx<ph->nVertices(); ++iVtx) {
      const Vertex* phvx = ph->vertex(iVtx);
      if(phvx) {
	for(size_t iTrk=0; iTrk<phvx->nTrackParticles(); ++iTrk) {
	  const TrackParticle* phtrk = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(phvx->trackParticle(iTrk));
	  if(phtrk) {
	    trklist.push_back(phtrk);
	  } else {
	    ATH_MSG_WARNING("Null pointer given for photon ID track!");
	  }
	}
      } else {
	ATH_MSG_WARNING("Null pointer given for photon vertex!");
      }
    }
    METEgammaTool::matchExtraTracks(ph,trklist);
    ATH_MSG_VERBOSE("Photon has " << trklist.size() << " linked tracks");
  }

  StatusCode METPhotonTool::executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const {
    ATH_MSG_DEBUG ("In execute: " << name() << "...");

    const PhotonContainer* phCont = 0;
    if( evtStore()->retrieve(phCont, m_input_data_key).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input photon container");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Successfully retrieved photon collection");

    MissingETBase::Types::bitmask_t source = MissingETBase::Source::photon();
    metTerm->setSource(source);

    MissingETComponentMap::iterator iter = MissingETComposition::find(metMap,metTerm);
    if(iter==metMap->end()) {
      ATH_MSG_WARNING("Could not find current METComponent in MET Map!");
      return StatusCode::SUCCESS;
    }
    MissingETComponent* newComp = *iter;
    newComp->setStatusWord(MissingETBase::Status::contributedHardTerm());

    ///////////////////////////////////////////////////////////////////
    // FIXME
    // may need to ensure that the photon container is properly
    // sorted for the overlap removal procedure.
    // descending pt order should suffice
    vector<const IParticle*> signalList;
    for(PhotonContainer::const_iterator iPh=phCont->begin();
	iPh!=phCont->end(); ++iPh) {
      if ( this->accept(*iPh) ) {
	signalList.clear();
	signalList.reserve((*iPh)->nCaloClusters()+2*(*iPh)->nVertices());
	MissingETBase::Types::weight_t objWeight;

	if ( this->resolveOverlap(*iPh,metMap,signalList,objWeight) ) {
	  ATH_MSG_VERBOSE(signalList.size() << " tracks/clusters in signal list");
	  ATH_MSG_VERBOSE("Non-overlapped photon, add this to MET");
	  this->addToMET(*iPh,signalList,metTerm,metMap,objWeight);
	} // add if not overlapping anything
      } // if passing selection
    } // loop on ph container

    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

}

