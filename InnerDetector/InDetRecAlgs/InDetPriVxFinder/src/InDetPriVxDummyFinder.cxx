/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                         InDetPriVxDummyFinder.cxx  -  Description
                             -------------------
 ***************************************************************************/
#include "InDetPriVxFinder/InDetPriVxDummyFinder.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include <vector>

namespace InDet
{

  InDetPriVxDummyFinder::InDetPriVxDummyFinder ( const std::string &n, ISvcLocator *pSvcLoc )
      : AthAlgorithm ( n, pSvcLoc ),
      m_iBeamCondSvc("BeamCondSvc",n),
      m_vxCandidatesOutputName ( "VxPrimaryCandidate" )
  {
    declareProperty ( "BeamPositionSvc", m_iBeamCondSvc);
    declareProperty ( "VxCandidatesOutputName",m_vxCandidatesOutputName );
  }

  InDetPriVxDummyFinder::~InDetPriVxDummyFinder() {}

  StatusCode InDetPriVxDummyFinder::initialize()
  {
    if ( m_iBeamCondSvc.retrieve().isFailure() )
    {
      msg(MSG::ERROR) << "Could not find BeamCondSvc." << endreq;
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  StatusCode InDetPriVxDummyFinder::execute()
  {
    xAOD::VertexContainer* theVxContainer = new xAOD::VertexContainer();
    xAOD::VertexAuxContainer* aux = new xAOD::VertexAuxContainer();
    theVxContainer->setStore( aux );

    xAOD::Vertex* dummyVertex = new xAOD::Vertex();
    theVxContainer->push_back ( dummyVertex ); // have to add vertex to container here first so it can use its aux store - David S.
    dummyVertex->setPosition(m_iBeamCondSvc->beamVtx().position());
    dummyVertex->setCovariancePosition(m_iBeamCondSvc->beamVtx().covariancePosition());
    dummyVertex->setVertexType(xAOD::VxType::NoVtx);

    //---- Recording section: write the results to StoreGate ---//
    if ( evtStore()->record ( theVxContainer, m_vxCandidatesOutputName,false ).isFailure() )
    {
      if (msgLvl(MSG::ERROR)) msg() << "Unable to record Vertex Container in StoreGate" << endreq;
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }
  
  StatusCode InDetPriVxDummyFinder::finalize()
  {
    return StatusCode::SUCCESS;
  }
  
} // end namespace InDet
