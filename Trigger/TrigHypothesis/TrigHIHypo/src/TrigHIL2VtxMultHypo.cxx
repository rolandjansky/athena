/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2VtxMultHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHIHypo
//
// AUTHOR:  
//           Performs the cuts on the TrigVertex multiplicity 
//           M. Sutton Jan 2013
//
// ********************************************************************

#include <list>
#include <iterator>
#include <sstream>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigHIL2VtxMultHypo.h"

#include "GaudiKernel/ITHistSvc.h"

#include "TrigInDetEvent/TrigVertexCollection.h"

class ISvcLocator;



TrigHIL2VtxMultHypo::TrigHIL2VtxMultHypo(const std::string& name, ISvcLocator* pSvcLocator):
    HLT::HypoAlgo(name, pSvcLocator),
    m_acceptAll(false),
    m_minVertices(1), 
    m_weightThreshold(0)
{
  declareProperty( "AcceptAll",       m_acceptAll );
  declareProperty( "MinVertices",     m_minVertices );
  declareProperty( "WeightThreshold", m_weightThreshold );

  declareMonitoredVariable("numVertices",	m_numVertices);
  declareMonitoredVariable("vertexWeightMax",m_vertexWeightMax);
  declareMonitoredStdContainer("vertexWeight",m_vertexWeight);
}

TrigHIL2VtxMultHypo::~TrigHIL2VtxMultHypo(){
}


///
/// Initialise - beginRun
///
HLT::ErrorCode TrigHIL2VtxMultHypo::hltBeginRun(){

  msg() << MSG::INFO << "TrigHIL2VtxMultHypo::beginRun()" << endreq;
  return HLT::OK;

}


///
/// Initialise
///
HLT::ErrorCode TrigHIL2VtxMultHypo::hltInitialize() {

  msg() << MSG::INFO << "TrigHIL2VtxMultHypo initialize()" << endreq;

  msg() << MSG::INFO
      << "Initialization of TrigHIL2VtxMultHypo completed successfully"
      << endreq;

  return HLT::OK;
}


///
/// Finalise
///
HLT::ErrorCode TrigHIL2VtxMultHypo::hltFinalize() {

  msg() << MSG::INFO << "TrigHIL2VtxMultHypo finalize()" << endreq;

  return HLT::OK;
}


/// 
/// Execute - per event execution
///
HLT::ErrorCode TrigHIL2VtxMultHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {

  // default value, it will be set to true if selection satisfied
  pass = false;

  int outputLevel = msgLvl();

  if(outputLevel <= MSG::DEBUG)  msg() << MSG::DEBUG << name() << ": in execute()" << endreq;

  // AcceptAll property = true means selection cuts should not be applied
  if (m_acceptAll) {
    if(outputLevel <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" << endreq;
    pass=true;
    return HLT::OK;
  }
  else {
    if(outputLevel <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection"
	    << endreq;
  }
  
  /// select based on maximum allowed vertex multiplicity  

  /// get TrigVertexCollection
  
  const TrigVertexCollection* vertexCollection(0);
  
  if ( HLT::OK != getFeature(outputTE, vertexCollection) ) { 
    msg() << MSG::ERROR << "Failed to get TrigVertexCollection from the TE " << endreq;
    return HLT::NAV_ERROR;
  }
  
  if ( !vertexCollection ) {
    msg() << MSG::DEBUG << "No TrackVertexCollection" << endreq;
    return HLT::OK;
  }
  
  if(outputLevel <= MSG::DEBUG) {
    msg() << MSG::DEBUG << " REGTEST: Got TrigVertexCollection associated with the TE " << endreq;
  }    



  /// NB: This is VERY IMPORTANT !! This hypo is a dirty hack to access the weights from the 
  ///     IDScanZFinder which are not stored in any class that athena can move around, so 
  ///     instead for the *ZFinder Only* mode for the HI run, we write the vertex *weight* 
  ///     into the TrigVertex z value NOT the actual z value, which is never used.
  ///     this should NEVER have been done, but it was the only way, since something that 
  ///     worked was needed in only a few days

  int nVertices = 0;
  m_vertexWeightMax = 0;  
  m_vertexWeight.clear();
  for( TrigVertexCollection::const_iterator vtxIt = vertexCollection->begin();
       vtxIt != vertexCollection->end(); vtxIt++){
    m_vertexWeight.push_back( (*vtxIt)->z() );
    if ( (*vtxIt)->z() > m_vertexWeightMax ) m_vertexWeightMax = (*vtxIt)->z();	//looking for the biggest weight
    if ( (*vtxIt)->z() > -999. && (*vtxIt)->z() > m_weightThreshold ) nVertices++; 
  }
  m_numVertices = nVertices;		//how many of vertices are above the treshold

  if ( nVertices >= m_minVertices ) pass = true;

  if(outputLevel <= MSG::DEBUG) { 
    msg() << MSG::DEBUG << " found " << nVertices << " vertices"  << endreq;
  }

  if(outputLevel <= MSG::DEBUG) { 
    if ( pass ) msg() << MSG::DEBUG << " REGTEST: Event accepted " << endreq;
    else        msg() << MSG::DEBUG << " REGTEST: Event rejected " << endreq;
  }
  
  // Time total TrigHIL2VtxMultHypo execution time.
  // ---------------------------------------------
  
  return HLT::OK;
}
