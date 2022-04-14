/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                         InDetPriVxDummyFinder.cxx  -  Description
                             -------------------
 ***************************************************************************/
#include "InDetPriVxFinder/InDetPriVxDummyFinder.h"
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
      m_vxCandidatesOutputName ( "VxPrimaryCandidate" )
  {
    declareProperty ( "VxCandidatesOutputName",m_vxCandidatesOutputName );
  }

  InDetPriVxDummyFinder::~InDetPriVxDummyFinder() = default;

  StatusCode InDetPriVxDummyFinder::initialize()
  {
    ATH_CHECK(m_beamSpotKey.initialize());
    return StatusCode::SUCCESS;
  }

  StatusCode InDetPriVxDummyFinder::execute()
  {
    xAOD::VertexContainer* theVxContainer = new xAOD::VertexContainer();
    xAOD::VertexAuxContainer* aux = new xAOD::VertexAuxContainer();
    theVxContainer->setStore( aux );
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
    xAOD::Vertex* dummyVertex = new xAOD::Vertex();
    theVxContainer->push_back ( dummyVertex ); // have to add vertex to container here first so it can use its aux store - David S.
    dummyVertex->setPosition(beamSpotHandle->beamVtx().position());
    dummyVertex->setCovariancePosition(beamSpotHandle->beamVtx().covariancePosition());
    dummyVertex->setVertexType(xAOD::VxType::NoVtx);

    //---- Recording section: write the results to StoreGate ---//
    if ( evtStore()->record ( theVxContainer, m_vxCandidatesOutputName,false ).isFailure() )
    {
      if (msgLvl(MSG::ERROR)) msg() << "Unable to record Vertex Container in StoreGate" << endmsg;
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }
  
  StatusCode InDetPriVxDummyFinder::finalize()
  {
    return StatusCode::SUCCESS;
  }
  
} // end namespace InDet
