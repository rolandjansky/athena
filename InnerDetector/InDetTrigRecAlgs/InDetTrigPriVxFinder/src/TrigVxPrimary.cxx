/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//
// filename: TrigVxPrimary.cxx
//
// author: Iwona Grabowska-Bold, Nov 2005
//         Iwona.Grabowska@cern.ch
//
// Description:  Trigger version of the InDetPriVxFinder
//               (see original package documentation).
//
// **************************************************************************
#include "InDetTrigPriVxFinder/TrigVxPrimary.h"
#include "StoreGate/StoreGateSvc.h"
#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParameters/TrackParameters.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"

#include "TrkEventPrimitives/ParamDefs.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

using Amg::Vector3D;

namespace InDet
{
  
  TrigVxPrimary::TrigVxPrimary(const std::string &n, ISvcLocator *pSvcLoc)
    : HLT::FexAlgo(n, pSvcLoc),
      m_runWithoutField(false),
      m_VertexFinderTool("InDet::InDetPriVxFinderTool/InDetTrigPriVxFinderTool"),
      m_fieldSvc("AtlasFieldSvc", n)
  {
    declareProperty("VertexFinderTool",m_VertexFinderTool);
    declareProperty("RunWithoutField", m_runWithoutField, "It may be unsafe to run vertexing w/o field on");
    
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
    
    msg() << MSG::INFO << "TrigVxPrimary::initialize(). "<< endreq;
    
    /* Get the VertexFinderTool */
    if ( m_VertexFinderTool.retrieve().isFailure() ) {
      msg() << MSG::FATAL << "Failed to retrieve tool " << m_VertexFinderTool << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else{
      msg() << MSG::INFO << "Retrieved tool " << m_VertexFinderTool << endreq;
    }
    
    if (m_fieldSvc.retrieve().isFailure()){
      msg() << MSG::FATAL << "Failed to retrieve tool " << m_fieldSvc << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    } 
    else {
      msg() << MSG::INFO << "Retrieved tool " << m_fieldSvc << endreq;
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
      msg() << MSG::DEBUG << " In execHLTAlgorithm()" << endreq;
    
    //----------------------------------------------------------------------
    // Trigger specific part: navigate throw the trigger element to get the
    // input track collection.
    //----------------------------------------------------------------------
    
    const TrackCollection* trackTES=0;
    
    if ( HLT::OK != getFeature(outputTE, trackTES)) {
      msg() << MSG::ERROR << "Input track collection could not be found " << endreq;
      return HLT::NAV_ERROR;
    } 
   
    if (!trackTES){
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG
	      << "Input tracks by getFeature NULL, a previous algo attached nothing?" << endreq;
      runVtx = false;
    }
    else if(trackTES->size()==0){
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << " Input track collection has 0 size. Algorithm not executed!" << endreq;
      runVtx = false;
    }

    if (!m_runWithoutField && !m_fieldSvc->solenoidOn()){
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Solenoid Off and RunWithoutField=False. Algorithm not executed!" << endreq;
      runVtx = false;
    }

    if (runVtx){
      m_nTracks = trackTES->size();
      if (outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << "REGTEST: Retrieved input track collection with "
	      << m_nTracks << " tracks. " << endreq;
      
      //check whether measured perigees are available, if not vertexing crashes
      for (int i=0; i<m_nTracks; i++){
	const Trk::Perigee *mp = (trackTES->at(i)->perigeeParameters());
	if (mp){
	  if (std::isnan(mp->parameters()[Trk::d0])){
	    if(outputLevel <= MSG::DEBUG)
	      msg() << MSG::DEBUG
		    << "Algorithm not executed as measured perigees are not valid" << endreq;
	    runVtx = false;
	  }
	}
	else {
	  if(outputLevel <= MSG::DEBUG)
	    msg() << MSG::DEBUG
		  << "Algorithm not executed as measured perigees are empty" << endreq;
	  runVtx = false;
	}
      }
    }

    VxContainer* theVxContainer(0);
    if (runVtx){
      theVxContainer = m_VertexFinderTool->findVertex(trackTES);
    }
    else {
      theVxContainer = new VxContainer();
      Trk::VxCandidate * dummyvtx = new Trk::VxCandidate ();
      dummyvtx->setVertexType(Trk::NoVtx);
      theVxContainer->push_back ( dummyvtx );
    }

    //
    //  Attach resolved tracks to the trigger element.
    
    if ( HLT::OK !=  attachFeature(outputTE, theVxContainer, "PrimVx") ) {
      msg() << MSG::ERROR << "Could not attach feature to the TE" << endreq;
      
      return HLT::NAV_ERROR;
    }
    
    m_nVertices = theVxContainer->size();
    if(outputLevel <= MSG::DEBUG){
      msg() << MSG::DEBUG << "Container recorded in StoreGate." << endreq;
      msg() << MSG::DEBUG << "REGTEST: Container size :" << m_nVertices << endreq;
    }    
    
    size_t privtxcount(0), pileupvtxcount(0);
    for (int iv=0; iv<m_nVertices; iv++){
      Trk::VxCandidate *mvtx = theVxContainer->at(iv);
      if ( mvtx ){
	Vector3D vtx;
	vtx = mvtx->recVertex().position();
	const Amg::MatrixX& verr = mvtx->recVertex().covariancePosition();
	if (mvtx->vertexType()==Trk::PriVtx){
	  ++privtxcount;
	  if(outputLevel <= MSG::DEBUG){
	    msg() << MSG::DEBUG << "REGTEST " << privtxcount
		  << std::setw(10)
		  << " x=" << vtx.x() << "+/-" << Amg::error(verr, Trk::x)
		  << " y=" << vtx.y() << "+/-" << Amg::error(verr, Trk::y)
		  << " z=" << vtx.z() << "+/-" << Amg::error(verr, Trk::z)
		  << endreq; 
	  }
	  m_zOfPriVtx.push_back(vtx.z());
	}
	else if (mvtx->vertexType()==Trk::PileUp){
	  ++pileupvtxcount;
	  if(outputLevel <= MSG::DEBUG){
	    msg() << MSG::DEBUG << "REGTEST " << pileupvtxcount
		  << std::setw(10)
		  << " x=" << vtx.x()
	      	  << " y=" << vtx.y()
		  << " z=" << vtx.z()
		  << endreq; 
	  }
	  m_zOfPileUp.push_back(vtx.z());
	}
	else if (mvtx->vertexType()==Trk::NoVtx){
	  m_zOfNoVtx.push_back(vtx.z());
	}
      } else {
	msg() << MSG::DEBUG << "Bad VxCandidate=" << iv << endreq;
      }
    }
    
    return HLT::OK;
  }
  //---------------------------------------------------------------------------
  HLT::ErrorCode TrigVxPrimary::hltFinalize() {

    msg() << MSG::INFO << "TrigVxPrimary::finalize()" << endreq;

    return HLT::OK;
  }

} // end namespace InDet
