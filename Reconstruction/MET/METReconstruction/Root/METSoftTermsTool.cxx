///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  using xAOD::PFO;
  using xAOD::PFOContainer;
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
  METSoftTermsTool::METSoftTermsTool(const std::string& name) : 
    AsgTool(name),
    METBuilderTool(name)
  {
    declareProperty( "InputComposition", m_inputType = "Clusters" ); // Options : Clusters (default) OR Tracks OR PFOs
    //declareProperty( "InputPVKey",      m_pv_inputkey = "PrimaryVertices"    );
    declareProperty( "VetoNegEClus",     m_cl_vetoNegE = true     );
    declareProperty( "OnlyNegEClus",     m_cl_onlyNegE = false    );
  }

  // Destructor
  ///////////////
  METSoftTermsTool::~METSoftTermsTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METSoftTermsTool::initialize()
  {
    ATH_CHECK(  METBuilderTool::initialize() );
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");

    // use string property and convert to int?
    if(m_inputType == "Clusters")    m_st_objtype = 0;
    else if(m_inputType == "Tracks") m_st_objtype = 1;
    else if(m_inputType == "PFlow") {
      m_st_objtype = 2;
      if( m_pfotool.retrieve().isFailure() ) {
	ATH_MSG_FATAL("Failed to retrieve tool: " << m_pfotool->name());
	return StatusCode::FAILURE;
      };
    }
    else {
      ATH_MSG_FATAL("Invalid input collection type " << m_inputType << " supplied!");
    }
    // ReadHandleKey(s)

    ATH_CHECK( m_pv_inputkey.initialize() );
    if(m_st_objtype==0){
      ATH_CHECK( m_caloClusterKey.assign(m_input_data_key));
      ATH_CHECK( m_caloClusterKey.initialize());
    }
    else if(m_st_objtype==1){
      ATH_CHECK( m_trackParticleKey.assign(m_input_data_key));
      ATH_CHECK( m_trackParticleKey.initialize());

    }
    return StatusCode::SUCCESS;
  }

  StatusCode METSoftTermsTool::finalize()
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

  // Will need to have different treatments here for clusters and tracks
  bool METSoftTermsTool::accept(const xAOD::IParticle* object) const
  {
    // Apply cuts 
    if ( m_st_objtype==0 ) {
      ATH_MSG_VERBOSE("Check if cluster is accepted");

      if(object->type()!=xAOD::Type::CaloCluster) {
	ATH_MSG_WARNING("Expected CaloCluster, given " << object->type());
	return false;
      }
      const xAOD::CaloCluster* clus = static_cast<const xAOD::CaloCluster*>(object);
      return (clus) ? accept(clus) : false;

    } // end of if using clusters 
    else if (  m_st_objtype==1 ) {
      ATH_MSG_VERBOSE("Check if track is accepted");

      if(object->type()!=xAOD::Type::TrackParticle) {
	ATH_MSG_WARNING("Expected TrackParticle, given " << object->type());
	return false;
      }
      const xAOD::TrackParticle* track = static_cast<const xAOD::TrackParticle*>(object);
      return (track) ? accept(track) : false;

    } // end of if using tracks
    else if (  m_st_objtype==2 ) {
      ATH_MSG_VERBOSE("Check if PFO is accepted");

      if(object->type()!=xAOD::Type::ParticleFlow) {
	ATH_MSG_WARNING("Expected PFlow Object, given " << object->type());
	return false;
      }

      ATH_MSG_ERROR("Should use PFO accept() overload with PV check.");
    } // end of if using PFOs
    
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

  bool METSoftTermsTool::accept(const xAOD::PFO* pfo, const xAOD::Vertex* pv) const
  {
    if(!pfo->isCharged()) return true;
    if(fabs(pfo->track(0)->z0() - pv->z())>2) return false;
    return true;
  }

  bool METSoftTermsTool::resolveOverlap(const xAOD::IParticle* /*object*/,
				        xAOD::MissingETComponentMap* metMap,
				        std::vector<const xAOD::IParticle*>& acceptedSignals,
				        MissingETBase::Types::weight_t& /*objWeight*/) const
  {

    // Check/Resolve overlap
    bool objsused = false;
    if( m_st_objtype==0 ) {
      ATH_MSG_DEBUG("Check for used clusters");
      objsused = metMap->checkUsage(acceptedSignals,MissingETBase::UsageHandler::OnlyCluster);
    }
    else if( m_st_objtype==1 ) { 
      ATH_MSG_DEBUG("Check for used tracks");
      objsused = metMap->checkUsage(acceptedSignals,MissingETBase::UsageHandler::OnlyTrack);
    }
    else if( m_st_objtype==2 ) { 
      ATH_MSG_DEBUG("Check for used PFOs: DUMMY");
      //      objsused = metMap->checkUsage(acceptedSignals,MissingETBase::UsageHandler::OnlyTrack);
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

    // First retrieve the necessary container
    // Currently rely on only one: either CaloClusterContainer or TrackParticleContainer
    const PFOContainer* pfoCont = 0;
     vector<const IParticle*> signalList;
    if( m_st_objtype == 0 ) {
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
    else if( m_st_objtype == 1 ) {

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
    else if( m_st_objtype == 2 ) {
      // Retrieve the pfo container
      pfoCont = m_pfotool->retrievePFO(CP::EM, CP::all);
      if(!pfoCont) {
        ATH_MSG_WARNING("Unable to retrieve input pfo container");
        return StatusCode::SUCCESS;
      }
      SG::ReadHandle<xAOD::VertexContainer> pv_cont(m_pv_inputkey);
      if (!pv_cont.isValid()) {
        ATH_MSG_WARNING("Unable to retrieve input primary vertex container");
        return StatusCode::SUCCESS;
      }
      const xAOD::Vertex* pv(0);
      for(const auto& vx : *pv_cont) {
	if(vx->vertexType()==xAOD::VxType::PriVtx) {pv = vx; break;}
      }
      if(pv) {
	ATH_MSG_DEBUG("Main primary vertex has z = " << pv->z());
      } else{
	ATH_MSG_WARNING("Event has no primary vertices");
	return StatusCode::SUCCESS;
      }

      signalList.reserve(pfoCont->size());

      MissingETBase::Types::bitmask_t source = MissingETBase::Source::SoftEvent; // need to add PFlow source tag
      metTerm->setSource(source);

      MissingETComponentMap::iterator iter = MissingETComposition::find(metMap,metTerm);
      if(iter==metMap->end()) {
	ATH_MSG_WARNING("Could not find current METComponent in MET Map!");
	return StatusCode::SUCCESS;
      }
      MissingETComponent* newComp = *iter;
      newComp->setStatusWord(MissingETBase::Status::contributedSoftTerm());

      // Loop over all pfos
      for( PFOContainer::const_iterator iPfo=pfoCont->begin(); iPfo!=pfoCont->end(); ++iPfo ) {
        // Check if pfo satisfies the requirements
        if( this->accept(*iPfo,pv) ) {
	  // Add the selected pfo particles to the list
	  signalList.push_back(*iPfo);
	}
      } // end loop over pfos

      ATH_MSG_DEBUG("Selected " << signalList.size() << " pfos for soft MET");

    }  // end if PFOs

    // Resolve overlap: signalList is accessed by reference and updated w/ content that is not
    // associated to any object. True if signalList size 0, i.e. nothing to add to MET
    if( this->resolveOverlap(metMap,signalList) ) return StatusCode::SUCCESS;

    // Loop over the content and add to MET 
    vector<const IParticle*> dummyList;
    MissingETBase::Types::weight_t unitWeight(1.,1.,1.);

    if(m_st_objtype == 2) {
      for( vector<const IParticle*>::const_iterator iPart=signalList.begin();
	   iPart!=signalList.end(); ++iPart) {
	const PFO* pfo = dynamic_cast<const PFO*>(*iPart);
        if(pfo) {
	    metTerm->add(pfo->ptEM()*cos(pfo->phiEM()),
		         pfo->ptEM()*sin(pfo->phiEM()),
		         pfo->ptEM());
	  } else {
	  // In principle for the charged PFOs we should perhaps add the weights
	  // but this shouldn't happen if we don't have a jet. 
	  if(!pfo->isCharged()) {
	    metTerm->add(pfo->pt()*cos(pfo->phi()),
		         pfo->pt()*sin(pfo->phi()),
		         pfo->pt());
	  }

	  MissingETComposition::insert(metMap,metTerm,pfo,dummyList,unitWeight);
        }
      }
    } else {
      for( vector<const IParticle*>::const_iterator iPart=signalList.begin();
           iPart!=signalList.end(); ++iPart) {
        this->addToMET(*iPart,dummyList,metTerm,metMap,unitWeight);
      }
    }

    ATH_MSG_DEBUG( "Map contains " << (*MissingETComposition::find(metMap,metTerm))->objects().size() << " soft signals" );

    if(pfoCont) delete pfoCont;
    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

}

