/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <string>
#include <iostream>

// Framework
#include "GaudiKernel/MsgStream.h"

// Reconstruction and MC
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "TrigInDetEvent/TrigVertex.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/RecVertex.h"
#include "TrkEventPrimitives/TrkEventPrimitivesDict.h"


// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostData/Vertex.h"
#include "TrigCostAthena/TrigNtVertexTool.h"

// Move to EIGEN
#define EIGEN_MATRIXBASE_PLUGIN "EventPrimitives/AmgMatrixPlugin.h"
#define EIGEN_MATRIX_PLUGIN "EventPrimitives/SymmetricMatrixHelpers.h"
#define EIGEN_TRANSFORM_PLUGIN "EventPrimitives/AmgTransformPlugin.h"
#include <Eigen/Core>
#include <Eigen/Dense>


//---------------------------------------------------------------------------------------
Trig::TrigNtVertexTool::TrigNtVertexTool(const std::string &name,
					 const std::string &type,
					 const IInterface  *parent)
  :AlgTool(name, type, parent),
   m_log(0),
   m_storeGate("StoreGateSvc", name),
   m_histSvc("THistSvc/THistSvc", name),
   m_vertexColl(0),
   m_offvtxColl(0),
   m_hltvtxColl(0),
   m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool")
{
  declareInterface<Trig::ITrigNtTool>(this);
  
  declareProperty("vertexContainerName", m_vertexContainerName = "HLT_TrigBeamSpotVertex_SiTrack");
  declareProperty("offvtxContainerName", m_offvtxContainerName = "VxPrimaryCandidate");
  declareProperty("hltvtxContainerName", m_hltvtxContainerName = "HLT_PrimVx");
  declareProperty("debug",               m_debug               = false);

  declareProperty("fillVertexVector",    m_fillVertexVector    = true);

  declareProperty("trigDec",             m_trigDec);

}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtVertexTool::initialize()
{    
//   std::cout << "Trig::TrigNtVertexTool::initialize - SJS" << std::endl;
  //
  // Get services
  //
  m_log = new MsgStream(msgSvc(), name());
  
  if(m_storeGate.retrieve().isFailure()) {
    log() << MSG::ERROR << "Could not retrieve StoreGateSvc!" << endreq;
    return StatusCode::FAILURE;
  }
  
  if(m_histSvc.retrieve().isFailure()) {
    log() << MSG::ERROR << "Failed to retrieve THistSvc" << endreq;
    return StatusCode::FAILURE;
  }

  if(m_fillVertexVector) {
    m_vertexVector = Anp::NtupleSvc::Instance("event").Branch< std::vector<Trig::Vertex> >("vtx");
    m_vertexVector.SetType("std::vector<Trig::Vertex>");

    m_offvtxVector = Anp::NtupleSvc::Instance("event").Branch< std::vector<Trig::Vertex> >("offvtx");
    m_offvtxVector.SetType("std::vector<Trig::Vertex>");

    m_hltvtxVector = Anp::NtupleSvc::Instance("event").Branch< std::vector<Trig::Vertex> >("hltvtx");
    m_hltvtxVector.SetType("std::vector<Trig::Vertex>");
  }

  // Get TrigDecisionTool
  if(m_trigDec.retrieve().isFailure()) {
    log() << MSG::ERROR << "Could not retrieve TrigDecisionTool!" << endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtVertexTool::finalize()
{
//   std::cout << "Trig::TrigNtVertexTool::finalize - SJS" << std::endl;
  //
  // Clean up internal state
  //
  log() << MSG::DEBUG << "finalize()" << endreq;

  delete m_log; m_log = 0;

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtVertexTool::Fill(TrigMonConfig *confg)
{
//   std::cout << "Trig::TrigNtVertexTool::Fill(TrigMonConfig *confg) - SJS" << std::endl;

  //
  // Copy configuration
  //
  if(!confg) {
    log() << MSG::WARNING << "Null TrigMonConfig pointer" << endreq;
    return false;
  }

  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtVertexTool::Fill(TrigMonEvent &event)
{
//   std::cout << "Trig::TrigNtVertexTool::Fill(TrigMonEvent &event) - SJS" << std::endl;

  //
  // Process current event
  //
  if(m_debug && outputLevel() <= MSG::DEBUG) {
    log() << MSG::INFO << "dumpStore--------------------------------------------" << endreq;
    m_storeGate->dump();
  }

  //std::cout << "Trig::TrigNtVertexTool::Fill(TrigMonEvent &event) - get TrigVertexCollection - SJS" << std::endl;
  bool TrigVertexCollectionExists = true;
  if(!m_storeGate->contains<TrigVertexCollection>(m_vertexContainerName)) {
    log() << MSG::DEBUG << "No Vertex Container with key " << m_vertexContainerName  << endreq;
    //return true;
    TrigVertexCollectionExists &= false;
  }

  //std::cout << "Trig::TrigNtVertexTool::Fill(TrigMonEvent &event) - get DataHandle<TrigVertexCollection> - SJS" << std::endl;
  const DataHandle<TrigVertexCollection> vertexColl;
  if(m_storeGate->retrieve(vertexColl, m_vertexContainerName).isFailure()) {
    log() << MSG::DEBUG << "No TrigVertexCollection with key " << m_vertexContainerName  << endreq;
//     return false;
    TrigVertexCollectionExists &= false;
  }

  if (TrigVertexCollectionExists == true) {
    std::vector<Trig::Vertex> TriggerVertices;
//     std::cout << "TrigVertexCollection size is " << vertexColl->size() << std::endl;

    TrigVertexCollection::const_iterator aVertexIterator, vertexCollEnd = vertexColl->end();
    for (aVertexIterator = vertexColl->begin(); aVertexIterator != vertexCollEnd; aVertexIterator++)
      {
	TrigVertex* aVertex = (*aVertexIterator);
	Trig::Vertex aTriggerVertex;

	aTriggerVertex.SetVertexNTracks ( (aVertex->ndof() + 3) / 2  );
	aTriggerVertex.SetVertexQuality ( aVertex->chi2() / aVertex->ndof(),
					  1.0 - Genfun::CumulativeChiSquare(aVertex->ndof())(aVertex->chi2()) );
	aTriggerVertex.SetVertexPosition( aVertex->x(),
					  aVertex->y(),
					  aVertex->z() );
	aTriggerVertex.SetVertexError   ( sqrt(aVertex->cov()[0]),
					  sqrt(aVertex->cov()[2]),
					  sqrt(aVertex->cov()[5]) );
      
	TriggerVertices.push_back( aTriggerVertex );

// 	std::cout 
// 	      << "L2 Vertex Information: "    << std::endl
// 	      << "    Number of Tracks:  "    << aTriggerVertex.GetVertexNTracks() << std::endl
// 	      << "    Vertex chi2:       "    << aTriggerVertex.GetVertexChi2() << std::endl
// 	      << "    Vertex Position:   "    << aTriggerVertex.GetVertexX() << ", " << aTriggerVertex.GetVertexY() << ", " << aTriggerVertex.GetVertexZ() << std::endl
// 	      << "    Vertex Error:      "    << aTriggerVertex.GetVertexXerr() << ", " << aTriggerVertex.GetVertexYerr() << ", " << aTriggerVertex.GetVertexZerr() 
// 	      << std::endl;

      }

    if(m_vertexVector.valid()) {
      m_vertexVector.GetRef()  = TriggerVertices;

      log() << MSG::DEBUG
	    << "m_vertexVector.GetRef() is valid." << endreq;
    } else {
      log() << MSG::DEBUG
	    << "m_vertexVector.GetRef() is not valid." << endreq;
    }

    //
    // Set TrigMonEvent variable - keep this!
    //
    event.addVar(250, TriggerVertices.size());

    log() << MSG::DEBUG
	  << "L2 Vertex multiplicity  = " << TriggerVertices.size()  << endreq;

    
  }

  // offline vertices
  //std::cout << "Trig::TrigNtVertexTool::Fill(TrigMonEvent &event) - get VxContainer - SJS" << std::endl;
  bool OfflVertexCollectionExists = true;
  if(!m_storeGate->contains<VxContainer>(m_offvtxContainerName)) {
    log() << MSG::DEBUG << "No Vertex Container with key " << m_offvtxContainerName  << endreq;
//     return true;
    OfflVertexCollectionExists &= false;
  }

  //std::cout << "Trig::TrigNtVertexTool::Fill(TrigMonEvent &event) - get DataHandle<VxContainer> - SJS" << std::endl;
  const DataHandle<VxContainer> offvtxColl;
  if(m_storeGate->retrieve(offvtxColl, m_offvtxContainerName).isFailure()) {
    log() << MSG::DEBUG << "No VxContainer with key " << m_offvtxContainerName  << endreq;
    OfflVertexCollectionExists &= false;
    //return false;
  }
  
  if (OfflVertexCollectionExists == true) {

    //std::cout << "Trig::TrigNtVertexTool::Fill(TrigMonEvent &event) - do stuff - SJS" << std::endl;
    log() << MSG::DEBUG << "VxContainer size is " << offvtxColl->size() << endreq;
//     std::cout << "VxContainer size is " << offvtxColl->size() << std::endl;



    std::vector<Trig::Vertex> OffVertices;
    VxContainer::const_iterator aOffVtxIterator, offvtxCollEnd = offvtxColl->end();
    for (aOffVtxIterator = offvtxColl->begin(); aOffVtxIterator != offvtxCollEnd; aOffVtxIterator++)
      {
	Trk::VxCandidate* aOffVtx = (*aOffVtxIterator);
	const Trk::RecVertex recvtx = aOffVtx->recVertex();
      
	Trig::Vertex aOffVertex;

	aOffVertex.SetVertexNTracks ( (recvtx.fitQuality().numberDoF() + 3) / 2  );

	if (recvtx.fitQuality().numberDoF() <= 0 ||
	    recvtx.fitQuality().chiSquared() <= 0)
	  {
	    aOffVertex.SetVertexQuality ( 0.0, 0.0 );
	  } 
	else 
	  {
	    aOffVertex.SetVertexQuality ( recvtx.fitQuality().chiSquared() / recvtx.fitQuality().numberDoF(),
					  1.0 - Genfun::CumulativeChiSquare(recvtx.fitQuality().numberDoF())(recvtx.fitQuality().chiSquared()));
	  }
      
	aOffVertex.SetVertexPosition( recvtx.position().x(),
				      recvtx.position().y(),
				      recvtx.position().z() );
				      
	aOffVertex.SetVertexError   ( sqrt(recvtx.covariancePosition()(0,0)),
				      sqrt(recvtx.covariancePosition()(1,1)),
				      sqrt(recvtx.covariancePosition()(2,2)) );
      
	OffVertices.push_back( aOffVertex );

// 	std::cout 
// 	      << "Off Vertex Information: "    << std::endl
// 	      << "    Number of Tracks:   "    << aOffVertex.GetVertexNTracks() << std::endl
// 	      << "    Vertex chi2:        "    << aOffVertex.GetVertexChi2() << std::endl
// 	      << "    Vertex Position:    "    << aOffVertex.GetVertexX() << ", " << aOffVertex.GetVertexY() << ", " << aOffVertex.GetVertexZ() << std::endl
// 	      << "    Vertex Error:       "    << aOffVertex.GetVertexXerr() << ", " << aOffVertex.GetVertexYerr() << ", " << aOffVertex.GetVertexZerr() 
// 	      << std::endl;

      }

    if(m_offvtxVector.valid()) {
      m_offvtxVector.GetRef()  = OffVertices;

      log() << MSG::DEBUG
	    << "m_offvtxVector.GetRef() is valid." << endreq;
    } else {
      log() << MSG::DEBUG
	    << "m_offvtxVector.GetRef() is not valid." << endreq;
    }
    //
    // Set TrigMonEvent variable - keep this!
    //
    event.addVar(251, OffVertices.size());

    log() << MSG::DEBUG
	  << "Offline Vertex multiplicity  = " << OffVertices.size()  << endreq;

  }

  // HLT vertices

  // get the feature container
  //Trig::FeatureContainer tdtFeatures = m_trigDec->features( "EF_.*" );
  Trig::FeatureContainer tdtFeatures = m_trigDec->features( "EF_InDetMon_FS" );
  Trig::FeatureContainer::combination_const_iterator
    tdtFeature(tdtFeatures.getCombinations().begin());
  Trig::FeatureContainer::combination_const_iterator
    tdtFeatureEnd(tdtFeatures.getCombinations().end());


  // get JetCollection.
  std::vector< Trig::Feature< VxContainer > > vertexTDT = tdtFeatures.get< VxContainer >();

  if ( false == vertexTDT.empty() ) {

    // retrieve the VxContainers from the TDT
    std::vector< Trig::Feature< VxContainer > >::iterator vxcIter = vertexTDT.begin();

    // grab the first PrimVx container
    if ( vxcIter != vertexTDT.end() ) {

      const VxContainer* hltvtxColl = (*vxcIter);


//       std::cout << "Trig::TrigNtVertexTool::Fill(TrigMonEvent &event) - do stuff - SJS" << std::endl;
      log() << MSG::DEBUG << "VxContainer size is " << hltvtxColl->size() << endreq;
//       std::cout << "VxContainer size is " << hltvtxColl->size() << std::endl;


      std::vector<Trig::Vertex> HltVertices;
      VxContainer::const_iterator aHltVtxIterator, hltvtxCollEnd = hltvtxColl->end();
      for (aHltVtxIterator = hltvtxColl->begin(); aHltVtxIterator != hltvtxCollEnd; aHltVtxIterator++)
	{
	  Trk::VxCandidate* aHltVtx = (*aHltVtxIterator);
	  const Trk::RecVertex recvtx = aHltVtx->recVertex();


	  // reject the "dummy" vertex
	  if (aHltVtx->vertexType() == Trk::NoVtx)
	    continue;


	  Trig::Vertex aHltVertex;

	  aHltVertex.SetVertexNTracks ( (recvtx.fitQuality().numberDoF() + 3) / 2  );

	  if (recvtx.fitQuality().numberDoF() <= 0 ||
	      recvtx.fitQuality().chiSquared() <= 0)
	    {
	      aHltVertex.SetVertexQuality ( 0.0, 0.0 );
	    } 
	  else 
	    {
	      aHltVertex.SetVertexQuality ( recvtx.fitQuality().chiSquared() / recvtx.fitQuality().numberDoF(),
					    1.0 - Genfun::CumulativeChiSquare(recvtx.fitQuality().numberDoF())(recvtx.fitQuality().chiSquared()));
	    }
      
	  aHltVertex.SetVertexPosition( recvtx.position().x(),
					recvtx.position().y(),
					recvtx.position().z() );
					
	  aHltVertex.SetVertexError   ( sqrt(recvtx.covariancePosition()(0,0)),
					sqrt(recvtx.covariancePosition()(1,1)),
					sqrt(recvtx.covariancePosition()(2,2)) );
      
	  HltVertices.push_back( aHltVertex );

// 	  	std::cout 
// 	  	      << "Hlt Vertex Information: "    << std::endl
// 	  	      << "    Number of Tracks:   "    << aHltVertex.GetVertexNTracks() << std::endl
// 	  	      << "    Vertex chi2:        "    << aHltVertex.GetVertexChi2() << std::endl
// 	  	      << "    Vertex Position:    "    << aHltVertex.GetVertexX() << ", " << aHltVertex.GetVertexY() << ", " << aHltVertex.GetVertexZ() << std::endl
// 		      << "    Vertex Error:       "    << aHltVertex.GetVertexXerr() << ", " << aHltVertex.GetVertexYerr() << ", " << aHltVertex.GetVertexZerr() << std::endl
// 		      << "    Vertex Type:        "    << aHltVtx->vertexType() 
// 	  	      << std::endl;

	}

      if(m_hltvtxVector.valid()) {
	m_hltvtxVector.GetRef()  = HltVertices;

	log() << MSG::DEBUG
	      << "m_hltvtxVector.GetRef() is valid." << endreq;
      } else {
	log() << MSG::DEBUG
	      << "m_hltvtxVector.GetRef() is not valid." << endreq;
      }
      //
      // Set TrigMonEvent variable - keep this!
      //
      event.addVar(252, HltVertices.size());
    
      log() << MSG::DEBUG
	    << "Hltline Vertex multiplicity  = " << HltVertices.size()  << endreq;
    }    
  }
  
  return true;
}

