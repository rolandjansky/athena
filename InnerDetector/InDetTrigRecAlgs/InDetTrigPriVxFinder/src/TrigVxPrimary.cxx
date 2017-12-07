/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//
// filename: TrigVxPrimary.cxx
//
// author: Iwona Grabowska-Bold, Nov 2005
//         Iwona.Grabowska@cern.ch
//         Jiri Masik <jiri.masik@manchester.ac.uk>,  May 2007
//
// Description:  Trigger version of the InDetPriVxFinder
// **************************************************************************

#include "InDetTrigPriVxFinder/TrigVxPrimary.h"
#include "StoreGate/StoreGateSvc.h"
#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkTrackLink/ITrackLink.h"
#include "TrkTrack/LinkToTrack.h"

#include "TrkEventPrimitives/ParamDefs.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

using Amg::Vector3D;

namespace InDet
{
  
  TrigVxPrimary::TrigVxPrimary(const std::string &n, ISvcLocator *pSvcLoc)
    : HLT::FexAlgo(n, pSvcLoc),
      m_runWithoutField(false),
      m_useTracksAsInput(false),
      m_createVtxTPLinks(false),
      m_VertexFinderTool("InDet::InDetPriVxFinderTool/InDetTrigPriVxFinderTool"),
      m_fieldSvc("AtlasFieldSvc", n),
      m_BeamCondSvc("BeamCondSvc", n)
  {
    declareProperty("VertexFinderTool",m_VertexFinderTool);
    declareProperty("RunWithoutField", m_runWithoutField, "It may be unsafe to run vertexing w/o field on");
    declareProperty("BeamCondSvc", m_BeamCondSvc);
    declareProperty("UseTracksAsInput", m_useTracksAsInput, "If TrackParticle exist, do not use tracks");
    declareProperty("CreateVtxTrackParticleLinks", m_createVtxTPLinks);

    
    declareMonitoredVariable("numTracks", m_nTracks   );
    declareMonitoredVariable("numVertices", m_nVertices    );
    declareMonitoredStdContainer("zOfPriVtx", m_zOfPriVtx);
    declareMonitoredStdContainer("zOfPileUp", m_zOfPileUp);
    declareMonitoredStdContainer("zOfNoVtx", m_zOfNoVtx);
  }

  //-------------------------------------------------------------------------
  TrigVxPrimary::~TrigVxPrimary()
  {}
  //-------------------------------------------------------------------------
  HLT::ErrorCode TrigVxPrimary::hltInitialize() {
    
    ATH_MSG_INFO("TrigVxPrimary::initialize(). ");
    
    /* Get the VertexFinderTool */
    if ( m_VertexFinderTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_VertexFinderTool);
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else{
      ATH_MSG_INFO("Retrieved tool " << m_VertexFinderTool);
    }
    
    if (m_fieldSvc.retrieve().isFailure()){
      ATH_MSG_FATAL("Failed to retrieve tool " << m_fieldSvc);
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    } 
    else {
      ATH_MSG_INFO("Retrieved service " << m_fieldSvc);
    }

    if (m_BeamCondSvc.retrieve().isFailure()){
      ATH_MSG_FATAL("Failed to retrieve tool " << m_BeamCondSvc);
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    } 
    else {
      ATH_MSG_INFO("Retrieved service " << m_fieldSvc);
    }

    ATH_MSG_INFO("UseTracksAsInput: " << m_useTracksAsInput << " CreateVtxTrackParticleLinks: " << m_createVtxTPLinks);
    return HLT::OK;
  }

  ///////////////////////////////////////////////////////////////////
  // Execute HLT algorithm
  ///////////////////////////////////////////////////////////////////
  HLT::ErrorCode TrigVxPrimary::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE) {
    
    
    m_nTracks = 0;
    m_nVertices = 0;
    m_zOfPriVtx.clear();
    m_zOfPileUp.clear();
    m_zOfNoVtx.clear();
    bool runVtx(true);
    
    
    ATH_MSG_DEBUG(" In execHLTAlgorithm()");

    if (!m_runWithoutField && !m_fieldSvc->solenoidOn()){
      ATH_MSG_DEBUG("Solenoid Off and RunWithoutField=False. Algorithm not executed!");
      runVtx = false;
    }

    
    //----------------------------------------------------------------------
    // get input track/ trackparticle  collection.
    //----------------------------------------------------------------------
    
    const TrackCollection* trackTES=0;
    if (m_useTracksAsInput){

      if  (HLT::OK != getFeature(outputTE, trackTES)) {
	ATH_MSG_ERROR("Input track collection could not be found ");
	return HLT::NAV_ERROR;
      } 

      if (!trackTES){
	ATH_MSG_DEBUG("Input tracks by getFeature NULL, a previous algo attached nothing?");
	runVtx = false;
      }
      else if(trackTES->size()==0){
	ATH_MSG_DEBUG("Input track collection has 0 size. Algorithm not executed!");
	runVtx = false;
      }
      
      //this is probably not needed anymore / will remove in the next iteration
      if (runVtx){
	m_nTracks = trackTES->size();
	ATH_MSG_DEBUG("REGTEST: Retrieved input track collection with "
		      << m_nTracks << " tracks. ");
	
	//
	for (int i=0; i<m_nTracks; i++){
	  const Trk::Perigee *mp = (trackTES->at(i)->perigeeParameters());
	  if (mp){
	    if (std::isnan(mp->parameters()[Trk::d0])){
	      ATH_MSG_WARNING("Algorithm not executed as measured track parameters are not valid");
	      runVtx = false;
	    }
	  }
	  else {
	    ATH_MSG_WARNING("Algorithm not executed as measured perigees are empty");
	    runVtx = false;
	  }
	}
      }
    }

    const xAOD::TrackParticleContainer *xTPCont = 0;
    if (!m_useTracksAsInput || m_createVtxTPLinks) {
      if ( HLT::OK != getFeature(outputTE, xTPCont)) {
	ATH_MSG_ERROR("xAOD Track particle collection could not be found ");
	return HLT::NAV_ERROR;
      } 

      if (!xTPCont){
	ATH_MSG_DEBUG("Input track particles by getFeature NULL, a previous algo attached nothing?");
	runVtx = false;
      }
      else if(xTPCont->size()==0){
	ATH_MSG_DEBUG("Input track particles have 0 size. Algorithm not executed!");
	runVtx = false;
      }
    }
   


    //
    // Create the xAOD container and its auxiliary store:

    xAOD::VertexContainer* theVxContainer = 0;
    xAOD::VertexAuxContainer* theVxAuxContainer = 0;
    std::pair<xAOD::VertexContainer*,xAOD::VertexAuxContainer*> theVxContainers
	= std::make_pair( theVxContainer, theVxAuxContainer );
    
    //
    // Run the vertexing

    if (runVtx) {
      if (m_useTracksAsInput) {
	theVxContainers = m_VertexFinderTool->findVertex(trackTES);
      } else {
	theVxContainers = m_VertexFinderTool->findVertex(xTPCont);
      }
      theVxContainer = theVxContainers.first;
      theVxAuxContainer = theVxContainers.second;  // the vertex finder has already called setStore
    }
    
    if (!runVtx || !theVxContainers.first){  // create and fill place-holder VertexContainer if necessary
      theVxContainer = new xAOD::VertexContainer();
      theVxAuxContainer = new xAOD::VertexAuxContainer(); 
      
      theVxContainer->setStore(theVxAuxContainer);
      theVxContainers.first = theVxContainer;
      theVxContainers.second = theVxAuxContainer;
      
      xAOD::Vertex *dummyvtx = new xAOD::Vertex();
      theVxContainer->push_back(dummyvtx);  // put vertex in container so it has Aux store
      dummyvtx->setVertexType(xAOD::VxType::NoVtx);  // now safe to set member variable
      dummyvtx->setPosition( m_BeamCondSvc->beamVtx().position() );
      dummyvtx->setCovariancePosition( m_BeamCondSvc->beamVtx().covariancePosition() );
      dummyvtx->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();

    }



    //
    // Create links to TrackParticles if they are available but Trk::Tracks were used on input

    if (m_useTracksAsInput && m_createVtxTPLinks){
      if (trackTES->size() == xTPCont->size()){
	auto itr = theVxContainer->begin();
	auto end = theVxContainer->end();
	for ( ; itr != end; itr++){
	  std::vector<Trk::VxTrackAtVertex> trks = (*itr)->vxTrackAtVertex();
	  size_t VTAVsize = trks.size();
	  for (size_t i=0; i<VTAVsize; i++){
	    Trk::ITrackLink*      trklink = trks[i].trackOrParticleLink();
	    Trk::LinkToTrack* linkToTrackPB = dynamic_cast<Trk::LinkToTrack*>(trklink);  
	    if (!linkToTrackPB) 
	      {
		ATH_MSG_DEBUG ("Cast of element link failed, skip this track !!!!!");
	      } 
	    else
	      {
		ElementLink<xAOD::TrackParticleContainer> newLink(*xTPCont, linkToTrackPB->index());
		//Now set the newlink to the new xAOD vertex
		(*itr)->addTrackAtVertex(newLink, trks[i].vtxCompatibility()); 
	      } 
	  }
	}
      } else {
	ATH_MSG_WARNING("Inconsistent set of tracks and track particles, linking not done");
      }
    }

    //
    //  Attach resolved tracks to the trigger element.
    
    if ( HLT::OK !=  attachFeature(outputTE, theVxContainer, "xPrimVx") ) {
      ATH_MSG_ERROR("Could not attach feature to the TE");

      delete theVxAuxContainer; theVxAuxContainer=0;
      return HLT::NAV_ERROR;
    }
    
    m_nVertices = theVxContainer->size();
    ATH_MSG_DEBUG("Container recorded in StoreGate.");
    ATH_MSG_DEBUG("REGTEST: Container size :" << m_nVertices);

    
    size_t privtxcount(0), pileupvtxcount(0);
    for (int iv=0; iv<m_nVertices; iv++){
      xAOD::Vertex *mvtx = theVxContainer->at(iv);
      if ( mvtx ){
	Vector3D vtx;
	vtx = mvtx->position();
	const Amg::MatrixX& verr = mvtx->covariancePosition();
	if (mvtx->vertexType()==xAOD::VxType::PriVtx){
	  ++privtxcount;
	  ATH_MSG_DEBUG("REGTEST " << privtxcount
			<< std::setw(10)
			<< " x=" << vtx.x() << "+/-" << Amg::error(verr, Trk::x)
			<< " y=" << vtx.y() << "+/-" << Amg::error(verr, Trk::y)
			<< " z=" << vtx.z() << "+/-" << Amg::error(verr, Trk::z)); 
	  
	  m_zOfPriVtx.push_back(vtx.z());
	}
	else if (mvtx->vertexType()==xAOD::VxType::PileUp){
	  ++pileupvtxcount;
	  ATH_MSG_DEBUG("REGTEST " << pileupvtxcount
			<< std::setw(10)
			<< " x=" << vtx.x()
			<< " y=" << vtx.y()
			<< " z=" << vtx.z());
	  m_zOfPileUp.push_back(vtx.z());
	}
	else if (mvtx->vertexType()==xAOD::VxType::NoVtx){
	  m_zOfNoVtx.push_back(vtx.z());
	}
      } else {
	ATH_MSG_DEBUG("Bad VxCandidate=" << iv);
      }
    }
    
    delete theVxAuxContainer; theVxAuxContainer=0;

    return HLT::OK;
  }
  //---------------------------------------------------------------------------
  HLT::ErrorCode TrigVxPrimary::hltFinalize() {

    ATH_MSG_DEBUG("TrigVxPrimary::finalize()");

    return HLT::OK;
  }

} // end namespace InDet
