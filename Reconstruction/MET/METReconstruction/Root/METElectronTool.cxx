/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////// -*- C++ -*- /////////////////////////////
// METElectronTool.cxx 
// Implementation file for class METElectronTool
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 

// STL includes
#include <algorithm>

// METReconstruction includes
#include "METReconstruction/METElectronTool.h"

// MET EDM
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAuxComponentMap.h"

// Egamma EDM
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronxAODHelpers.h"

// Tracking EDM
#include "xAODTracking/TrackParticle.h"

namespace met {

  using std::vector;
  //
  using xAOD::IParticle;
  //
  using xAOD::Electron;
  using xAOD::ElectronContainer;
  //
  using xAOD::CaloCluster;
  using xAOD::CaloClusterContainer;
  //
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
  METElectronTool::METElectronTool(const std::string& name) : 
    AsgTool(name),
    METBuilderTool(name),
    METEgammaTool(name)
  {
  }

  // Destructor
  ///////////////
  METElectronTool::~METElectronTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METElectronTool::initialize()
  {
    ATH_CHECK( METEgammaTool::initialize() );
    ATH_MSG_INFO ("Initializing " << name() << "...");

    // Should put some info about the configuration here

    return StatusCode::SUCCESS;
  }

  StatusCode METElectronTool::finalize()
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

  bool METElectronTool::resolveOverlap(const xAOD::IParticle* object,
				       xAOD::MissingETComponentMap* metMap,
				       std::vector<const xAOD::IParticle*>& acceptedSignals,
				       MissingETBase::Types::weight_t& objWeight) const
  {
    if(object->type() != xAOD::Type::Electron) {
      ATH_MSG_WARNING("METElectronTool::resolveOverlap given an object of type " << object->type());
      return false;
    }
    const Electron* el = dynamic_cast<const Electron*>(object);

    ATH_MSG_DEBUG("Identifying signals overlapping this electron");

    const CaloClusterContainer* tcCont(0);
    if( evtStore()->retrieve(tcCont, m_tcCont_key).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve topocluster container for overlap removal");
      return true;
    }
    ATH_MSG_DEBUG("Successfully retrieved topocluster collection");    

    // retrieve topoclusters associated to the electron
    vector<const IParticle*> tcList;
    tcList.reserve(el->nCaloClusters());
    matchTopoClusters(el, tcList, tcCont);
    // test the clusters for matches to other objects
    bool clustersUsed = metMap->checkUsage(tcList,MissingETBase::UsageHandler::OnlyCluster);
    if(clustersUsed) { // true implies some cluster has been used
      for(vector<const IParticle*>::const_iterator iClus = tcList.begin();
	  iClus!=tcList.end(); ++iClus) {
	acceptedSignals.push_back(*iClus);
      } // loop over electron-matched topoclusters
      objWeight = MissingETBase::Types::weight_t(0.,0.,0.);
    } else {
      acceptedSignals = tcList;
      objWeight = MissingETBase::Types::weight_t(1.,1.,1.);
    }

    if(m_eg_doTracks) {
      // retrieve tracks associated to the electron
      vector<const xAOD::IParticle*> trkList;
      trkList.reserve(el->nTrackParticles());
      matchTracks(el, trkList);
      // test the tracks for matches to other objects
      ATH_MSG_VERBOSE("Check track usage");
      metMap->checkUsage(trkList,MissingETBase::UsageHandler::OnlyTrack);
      ATH_MSG_VERBOSE("Checked track usage");
      for(vector<const IParticle*>::const_iterator iTrk = trkList.begin();
	  iTrk!=trkList.end(); ++iTrk) {
	acceptedSignals.push_back(*iTrk);
      } // loop over electron-matched tracks
      ATH_MSG_VERBOSE("End track loop");
    }

    return !clustersUsed; // return true if the electron shares no clusters with another object
  }

  void METElectronTool::matchTracks(const xAOD::Electron* el, std::vector<const xAOD::IParticle*>& trklist) const
  {
    for(size_t iTrk=0; iTrk<el->nTrackParticles(); ++iTrk) {
      const TrackParticle* eltrk = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(el->trackParticle(iTrk));
      if(eltrk) {
	trklist.push_back(eltrk);
      } else {
	ATH_MSG_WARNING("Null pointer given for electron ID track!");
      }
    }
    METEgammaTool::matchExtraTracks(el,trklist);
    ATH_MSG_VERBOSE("Electron has " << trklist.size() << " linked tracks");
  }

  StatusCode METElectronTool::executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const
  {
    ATH_MSG_DEBUG ("In execute: " << name() << "...");

    const ElectronContainer* elCont = 0;
    if( evtStore()->retrieve(elCont, m_input_data_key).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input electron container");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Successfully retrieved electron collection");

    MissingETComponentMap::iterator iter = MissingETComposition::find(metMap,metTerm);
    if(iter==metMap->end()) {
      ATH_MSG_WARNING("Could not find current METComponent in MET Map!");
      return StatusCode::SUCCESS;
    }
    MissingETComponent* newComp = *iter;
    newComp->setStatusWord(MissingETBase::Status::contributedHardTerm());

    MissingETBase::Types::bitmask_t source = MissingETBase::Source::electron();
    metTerm->setSource(source);

    ///////////////////////////////////////////////////////////////////
    // FIXME
    // may need to ensure that the electron container is properly
    // sorted for the overlap removal procedure.
    // descending pt order should suffice
    vector<const IParticle*> signalList;
    for(ElectronContainer::const_iterator iEl=elCont->begin();
	iEl!=elCont->end(); ++iEl) {
      if ( this->accept(*iEl) ) {
	signalList.clear();
	signalList.reserve((*iEl)->nCaloClusters()+(*iEl)->nTrackParticles());
	MissingETBase::Types::weight_t objWeight;

	if ( this->resolveOverlap(*iEl,metMap,signalList,objWeight) ) {
	  ATH_MSG_VERBOSE(signalList.size() << " tracks/clusters in signal list");
	  ATH_MSG_VERBOSE("Non-overlapped electron, add this to MET");
	  this->addToMET(*iEl,signalList,metTerm,metMap,objWeight);
	} // add if not overlapping anything
      } // if passing selection
    } // loop on el container

    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 


}
