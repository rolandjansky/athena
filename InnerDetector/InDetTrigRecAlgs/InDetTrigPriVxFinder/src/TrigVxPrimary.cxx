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
      m_VertexFinderTool("InDet::InDetPriVxFinderTool/InDetTrigPriVxFinderTool"),
      m_fieldSvc("AtlasFieldSvc", n),
      m_BeamCondSvc("BeamCondSvc", n)
  {
    declareProperty("VertexFinderTool",m_VertexFinderTool);
    declareProperty("RunWithoutField", m_runWithoutField, "It may be unsafe to run vertexing w/o field on");
    declareProperty("BeamCondSvc", m_BeamCondSvc);

    
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
    
    msg() << MSG::INFO << "TrigVxPrimary::initialize(). "<< endmsg;
    
    /* Get the VertexFinderTool */
    if ( m_VertexFinderTool.retrieve().isFailure() ) {
      msg() << MSG::FATAL << "Failed to retrieve tool " << m_VertexFinderTool << endmsg;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else{
      msg() << MSG::INFO << "Retrieved tool " << m_VertexFinderTool << endmsg;
    }
    
    if (m_fieldSvc.retrieve().isFailure()){
      msg() << MSG::FATAL << "Failed to retrieve tool " << m_fieldSvc << endmsg;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    } 
    else {
      msg() << MSG::INFO << "Retrieved service " << m_fieldSvc << endmsg;
    }

    if (m_BeamCondSvc.retrieve().isFailure()){
      msg() << MSG::FATAL << "Failed to retrieve tool " << m_BeamCondSvc << endmsg;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    } 
    else {
      msg() << MSG::INFO << "Retrieved service " << m_fieldSvc << endmsg;
    }

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
    
    int outputLevel = msgLvl();
    
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << " In execHLTAlgorithm()" << endmsg;
    
    //----------------------------------------------------------------------
    // Trigger specific part: navigate throw the trigger element to get the
    // input track collection.
    //----------------------------------------------------------------------
    
    const TrackCollection* trackTES=0;
    
    if ( HLT::OK != getFeature(outputTE, trackTES)) {
      msg() << MSG::ERROR << "Input track collection could not be found " << endmsg;
      return HLT::NAV_ERROR;
    } 
   
    if (!trackTES){
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG
	      << "Input tracks by getFeature NULL, a previous algo attached nothing?" << endmsg;
      runVtx = false;
    }
    else if(trackTES->size()==0){
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << " Input track collection has 0 size. Algorithm not executed!" << endmsg;
      runVtx = false;
    }

    if (!m_runWithoutField && !m_fieldSvc->solenoidOn()){
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Solenoid Off and RunWithoutField=False. Algorithm not executed!" << endmsg;
      runVtx = false;
    }

    if (runVtx){
      m_nTracks = trackTES->size();
      if (outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << "REGTEST: Retrieved input track collection with "
	      << m_nTracks << " tracks. " << endmsg;
      
      //check whether measured perigees are available, if not vertexing crashes
      for (int i=0; i<m_nTracks; i++){
	const Trk::Perigee *mp = (trackTES->at(i)->perigeeParameters());
	if (mp){
	  if (std::isnan(mp->parameters()[Trk::d0])){
	    if(outputLevel <= MSG::DEBUG)
	      msg() << MSG::DEBUG
		    << "Algorithm not executed as measured perigees are not valid" << endmsg;
	    runVtx = false;
	  }
	}
	else {
	  if(outputLevel <= MSG::DEBUG)
	    msg() << MSG::DEBUG
		  << "Algorithm not executed as measured perigees are empty" << endmsg;
	  runVtx = false;
	}
      }
    }

    // Create the xAOD container and its auxiliary store:
    xAOD::VertexContainer* theVxContainer = 0;
    xAOD::VertexAuxContainer* theVxAuxContainer = 0;
    std::pair<xAOD::VertexContainer*,xAOD::VertexAuxContainer*> theVxContainers
	= std::make_pair( theVxContainer, theVxAuxContainer );
    
    if (runVtx) {
      theVxContainers = m_VertexFinderTool->findVertex(trackTES);
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
    //  Attach resolved tracks to the trigger element.
    
    if ( HLT::OK !=  attachFeature(outputTE, theVxContainer, "xPrimVx") ) {
      msg() << MSG::ERROR << "Could not attach feature to the TE" << endmsg;

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
	  if(outputLevel <= MSG::DEBUG){
	    msg() << MSG::DEBUG << "REGTEST " << privtxcount
		  << std::setw(10)
		  << " x=" << vtx.x() << "+/-" << Amg::error(verr, Trk::x)
		  << " y=" << vtx.y() << "+/-" << Amg::error(verr, Trk::y)
		  << " z=" << vtx.z() << "+/-" << Amg::error(verr, Trk::z)
		  << endmsg; 
	  }
	  m_zOfPriVtx.push_back(vtx.z());
	}
	else if (mvtx->vertexType()==xAOD::VxType::PileUp){
	  ++pileupvtxcount;
	  if(outputLevel <= MSG::DEBUG){
	    msg() << MSG::DEBUG << "REGTEST " << pileupvtxcount
		  << std::setw(10)
		  << " x=" << vtx.x()
	      	  << " y=" << vtx.y()
		  << " z=" << vtx.z()
		  << endmsg; 
	  }
	  m_zOfPileUp.push_back(vtx.z());
	}
	else if (mvtx->vertexType()==xAOD::VxType::NoVtx){
	  m_zOfNoVtx.push_back(vtx.z());
	}
      } else {
	msg() << MSG::DEBUG << "Bad VxCandidate=" << iv << endmsg;
      }
    }
    
    delete theVxAuxContainer; theVxAuxContainer=0;

    return HLT::OK;
  }
  //---------------------------------------------------------------------------
  HLT::ErrorCode TrigVxPrimary::hltFinalize() {

    msg() << MSG::INFO << "TrigVxPrimary::finalize()" << endmsg;

    return HLT::OK;
  }

} // end namespace InDet
