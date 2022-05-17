///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// METSoftTermsTool.cxx 
// Implementation file for class METSoftTermsTool
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METSoftTermsTool.h"

// MET EDM
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAuxComponentMap.h"

// Tracking EDM
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

// Calo EDM
#include "xAODCaloEvent/CaloClusterContainer.h"

// Calo helpers
#include "xAODCaloEvent/CaloVertexedClusterBase.h"

namespace met {

  using std::vector;
  //
  using xAOD::IParticle;
  //
  using xAOD::TrackParticle;
  using xAOD::TrackParticleContainer;
  using xAOD::VertexContainer;
  //
  using xAOD::CaloCluster;
  using xAOD::CaloClusterContainer;
  //
  using xAOD::MissingET;
  using xAOD::MissingETComposition;
  using xAOD::MissingETComponent;
  using xAOD::MissingETComponentMap;

  // Constructors
  ////////////////
  METSoftTermsTool::METSoftTermsTool(const std::string& name) : 
    AsgTool(name),
    METBuilderTool(name)
  {
  }

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METSoftTermsTool::initialize()
  {
    ATH_CHECK(  METBuilderTool::initialize() );
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");

    // use string property and convert to int?
    if(m_inputType.value() == "Clusters")    m_st_objtype = xAOD::Type::CaloCluster;
    else if(m_inputType.value() == "Tracks") m_st_objtype = xAOD::Type::TrackParticle;
    else {
      ATH_MSG_FATAL("Invalid input collection type " << m_inputType.value() << " supplied!");
    }

    // ReadHandleKey(s)
    ATH_CHECK( m_caloClusterKey.initialize(m_st_objtype==xAOD::Type::CaloCluster));
    ATH_CHECK( m_trackParticleKey.initialize(m_st_objtype==xAOD::Type::TrackParticle));

    return StatusCode::SUCCESS;
  }

  // Will need to have different treatments here for clusters and tracks
  bool METSoftTermsTool::accept(const xAOD::IParticle* object) const
  {

    if(object->type() != m_st_objtype){
      ATH_MSG_WARNING("Type mismatch: Expected " << m_st_objtype << ", given " << object->type());
      return false;
    }

    // Apply cuts 
    if ( m_st_objtype == xAOD::Type::CaloCluster ) {
      ATH_MSG_VERBOSE("Check if cluster is accepted");

      const xAOD::CaloCluster* clus = static_cast<const xAOD::CaloCluster*>(object);
      return (clus) ? accept(clus) : false;

    } // end of if using clusters 
    else if (  m_st_objtype == xAOD::Type::TrackParticle ) {
      ATH_MSG_VERBOSE("Check if track is accepted");

      const xAOD::TrackParticle* track = static_cast<const xAOD::TrackParticle*>(object);
      return (track) ? accept(track) : false;

    } // end of if using tracks
    
    return false; // Default 
  }

  bool METSoftTermsTool::accept(const xAOD::CaloCluster* clus) const
  {
    if(!clus) return false;
    if(m_cl_vetoNegE && clus->e()<0) return false;
    if(m_cl_onlyNegE && clus->e()>0) return false;

    return true;
  }

  bool METSoftTermsTool::accept(const xAOD::TrackParticle* /*trk*/) const
  {
    // if(!trk) return false;

//    if(fabs(trk->pt())<500/*MeV*/ || fabs(trk->eta())>2.5) return false;
//
//    // could add some error checking to make sure we successfully read the details
//    uint8_t nPixHits(0), nSctHits(0);
//    trk->summaryValue(nPixHits,xAOD::numberOfPixelHits);
//    if(nPixHits<1) return false;
//    trk->summaryValue(nSctHits,xAOD::numberOfSCTHits);
//    if(nSctHits<6) return false;

    return true;
  }

  bool METSoftTermsTool::resolveOverlap(const xAOD::IParticle* /*object*/,
                                        xAOD::MissingETComponentMap* metMap,
                                        std::vector<const xAOD::IParticle*>& acceptedSignals,
                                        MissingETBase::Types::weight_t& /*objWeight*/) const
  {

    // Check/Resolve overlap
    bool objsused = false;
    if( m_st_objtype == xAOD::Type::CaloCluster ) {
      ATH_MSG_DEBUG("Check for used clusters");
      objsused = metMap->checkUsage(acceptedSignals,MissingETBase::UsageHandler::OnlyCluster);
    }
    else if( m_st_objtype == xAOD::Type::TrackParticle ) { 
      ATH_MSG_DEBUG("Check for used tracks");
      objsused = metMap->checkUsage(acceptedSignals,MissingETBase::UsageHandler::OnlyTrack);
    }
    if(!objsused) {
      ATH_MSG_DEBUG("No objects used.");
    }

    ATH_MSG_DEBUG( acceptedSignals.size() << " retained after overlap removal");

    return acceptedSignals.empty(); 
  }

  // overload for convenience
  bool METSoftTermsTool::resolveOverlap(xAOD::MissingETComponentMap* metMap,
                                        std::vector<const xAOD::IParticle*>& acceptedSignals) const
  {
    const xAOD::IParticle* dummyObject = 0;                  // Just a dummy object
    MissingETBase::Types::weight_t dummyObjWeight(1.,1.,1.); // Currently use a default value
    return resolveOverlap(dummyObject, metMap, acceptedSignals, dummyObjWeight);
  }

  StatusCode METSoftTermsTool::executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const
  {

    ATH_MSG_DEBUG ("In execute: " << name() << "...");

    vector<const IParticle*> signalList;
    if( m_st_objtype == xAOD::Type::CaloCluster ) {
      // Retrieve the calo container
      SG::ReadHandle<xAOD::CaloClusterContainer> caloClusCont(m_caloClusterKey);
      if (!caloClusCont.isValid()) {
        ATH_MSG_WARNING("Unable to retrieve input calo cluster container");
      }

      signalList.reserve(caloClusCont->size());
      //stateHelperList.reserve(caloClusCont->size());

      MissingETBase::Types::bitmask_t source = MissingETBase::Source::softEvent() | MissingETBase::Source::cluster();
      metTerm->setSource(source);

      MissingETComponentMap::iterator iter = MissingETComposition::find(metMap,metTerm);
      if(iter==metMap->end()) {
        ATH_MSG_WARNING("Could not find current METComponent in MET Map!");
        return StatusCode::SUCCESS;
      }
      MissingETComponent* newComp = *iter;
      newComp->setStatusWord(MissingETBase::Status::contributedSoftTerm());

      // Loop over all clusters
      for( CaloClusterContainer::const_iterator iClus=caloClusCont->begin(); iClus!=caloClusCont->end(); ++iClus ) {
        // Check if cluster satisfies the requirements
        if( this->accept(*iClus)) {
          // Add the selected clusters to the list
          signalList.push_back(*iClus);
        }
      } // end loop over clusters

      ATH_MSG_DEBUG("Selected " << signalList.size() << " topoclusters for soft MET");

    } // end if Clusters
    else if( m_st_objtype == xAOD::Type::TrackParticle ) {

      // Retrieve the track container
      SG::ReadHandle<xAOD::TrackParticleContainer> trackParCont(m_trackParticleKey);
      if (!trackParCont.isValid()) {
        ATH_MSG_WARNING("Unable to retrieve input track particle container");
      }
      signalList.reserve(trackParCont->size());

      MissingETBase::Types::bitmask_t source = MissingETBase::Source::SoftEvent | MissingETBase::Source::idTrack();
      metTerm->setSource(source);

      MissingETComponentMap::iterator iter = MissingETComposition::find(metMap,metTerm);
      if(iter==metMap->end()) {
        ATH_MSG_WARNING("Could not find current METComponent in MET Map!");
        return StatusCode::SUCCESS;
      }
      MissingETComponent* newComp = *iter;
      newComp->setStatusWord(MissingETBase::Status::contributedSoftTerm());

      // Loop over all tracks
      for( TrackParticleContainer::const_iterator iTrk=trackParCont->begin(); iTrk!=trackParCont->end(); ++iTrk ) {
        // Check if track satisfies the requirements
        if( this->accept(*iTrk) ) {
          // Add the selected track particles to the list
          signalList.push_back(*iTrk);
        }
      } // end loop over tracks

      ATH_MSG_DEBUG("Selected " << signalList.size() << " tracks for soft MET");

    }  // end if Tracks

    // Resolve overlap: signalList is accessed by reference and updated w/ content that is not
    // associated to any object. True if signalList size 0, i.e. nothing to add to MET
    if( this->resolveOverlap(metMap,signalList) ) return StatusCode::SUCCESS;

    // Loop over the content and add to MET 
    vector<const IParticle*> dummyList;
    MissingETBase::Types::weight_t unitWeight(1.,1.,1.);

    for(const IParticle* part : signalList) this->addToMET(part,dummyList,metTerm,metMap,unitWeight);

    ATH_MSG_DEBUG( "Map contains " << (*MissingETComposition::find(metMap,metTerm))->objects().size() << " soft signals" );

    return StatusCode::SUCCESS;
  }

}

