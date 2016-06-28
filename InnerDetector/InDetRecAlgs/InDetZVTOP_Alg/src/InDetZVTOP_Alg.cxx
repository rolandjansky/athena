/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetZVTOP_Alg.cxx, (c) ATLAS Detector software
// begin   : 30-10-2006
// authors : Tatjana Lenz
// email   : tatjana.lenz@cern.ch
// changes :
///////////////////////////////////////////////////////////////////

#include "InDetZVTOP_Alg/InDetZVTOP_Alg.h"
#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "VxVertex/VxContainer.h"

//================ Constructor =================================================

InDet::InDetZVTOP_Alg::InDetZVTOP_Alg(const std::string& name, ISvcLocator* pSvcLocator)
  :
  AthAlgorithm(name,pSvcLocator),
  m_tracksName("Tracks"),
  m_vxCollectionOutputName("VxCollection"),
  m_VertexFinderTool("InDet::ZVTOP_Tool")
{
  //  template for property declaration
    declareProperty("TracksName",m_tracksName);
    declareProperty("VxCollectionOutputName",m_vxCollectionOutputName);
    declareProperty("VertexFinderTool",m_VertexFinderTool);
}

//================ Destructor =================================================

InDet::InDetZVTOP_Alg::~InDetZVTOP_Alg()
{}


//================ Initialisation =================================================

StatusCode InDet::InDetZVTOP_Alg::initialize()
{
  // Code entered here will be executed once at program start.
  
  msg (MSG::INFO) << MSG::INFO << name() << " initialize()" << endreq;

  /* Get the VertexFinderTool */
   if ( m_VertexFinderTool.retrieve().isFailure() ) {
      msg( MSG::FATAL ) << "Failed to retrieve tool " << m_VertexFinderTool << endreq;
      return StatusCode::FAILURE;
   } else msg ( MSG::INFO ) << "Retrieved tool " << m_VertexFinderTool<< endreq;


  msg ( MSG::INFO ) << "initialize() successful in " << name() << endreq;
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::InDetZVTOP_Alg::finalize()
{
  // Code entered here will be executed once at the end of the program run.
  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode InDet::InDetZVTOP_Alg::execute()
{
  //VxContainer* theVxContainer(0); --David S.
  xAOD::VertexContainer* theXAODContainer = 0;
  xAOD::VertexAuxContainer* theXAODAuxContainer = 0;
  std::pair<xAOD::VertexContainer*,xAOD::VertexAuxContainer*> theXAODContainers
      = std::make_pair( theXAODContainer, theXAODAuxContainer );
  std::string vxContainerAuxName = m_vxCollectionOutputName + "Aux.";

  //---- Retrieve tracks from StoreGate section ----------------------------------------//
  if(evtStore()->contains<TrackCollection>(m_tracksName))
    {
      const TrackCollection *trackTES(0);
      if (evtStore()->retrieve(trackTES, m_tracksName).isFailure())
	{
	  if(msgLvl(MSG::DEBUG)) msg() << "Could not find TrackCollection " << m_tracksName << " in StoreGate." << endreq;
	  return StatusCode::SUCCESS;
	} else if (msgLvl(MSG::VERBOSE)) msg() << "Find TrackCollection " << m_tracksName << " in StoreGate." << endreq;
      if (msgLvl(MSG::VERBOSE)) msg() << "Number of tracks  = " << trackTES->size() << endreq;
      //theVxContainer = m_VertexFinderTool->findVertex(trackTES); --David S.
      theXAODContainers = m_VertexFinderTool->findVertex(trackTES);
    }
  else if(evtStore()->contains<Trk::TrackParticleBaseCollection>(m_tracksName))
    {
      const Trk::TrackParticleBaseCollection *trackParticleBaseCollection(0);
      if(evtStore()->retrieve(trackParticleBaseCollection, m_tracksName).isFailure())
	{
	  if(msgLvl(MSG::DEBUG)) msg() << "Could not find Trk::TrackParticleBaseCollection" << m_tracksName << " in StoreGate." << endreq;
	  return StatusCode::SUCCESS;
	}
      //theVxContainer = m_VertexFinderTool->findVertex(trackParticleBaseCollection); --David S.
      theXAODContainers = m_VertexFinderTool->findVertex(trackParticleBaseCollection);
    }
  else {
    if (msgLvl(MSG::DEBUG)) msg() << "Neither a TrackCollection nor a TrackParticleBaseCollection with key " << m_tracksName << " exist." << endreq;
    //add check for xAOD::TrackParticleContainer --David S.
    if(evtStore()->contains<xAOD::TrackParticleContainer>(m_tracksName))
    {
      if (msgLvl (MSG::DEBUG)) msg() << "xAOD::TrackParticleContainer with key " << m_tracksName << " found, but no findVertex method implemented yet for these objects." << endreq; 
    }
  }
  //---- Recording section: write the results to StoreGate ---//
  //if (msgLvl(MSG::VERBOSE)) msg() << "Recording to StoreGate: " << m_vxCollectionOutputName << endreq; --David S.
  if (msgLvl(MSG::VERBOSE)) msg() << "Recording to StoreGate: " << m_vxCollectionOutputName << " with AuxContainer " << vxContainerAuxName << endreq;
  //if (evtStore()->record(theVxContainer,m_vxCollectionOutputName,false).isFailure()) --David S.
  if (evtStore()->record(theXAODContainers.first,m_vxCollectionOutputName,false).isFailure())
    {
      //if (msgLvl(MSG::INFO)) msg() << "Unable to record VxContainer in TDS" << endreq; --David S.
      if (msgLvl(MSG::INFO)) msg() << "Unable to record VertexContainer in TDS" << endreq;
      return StatusCode::FAILURE;
    }
  //add record AuxContainer to StoreGate --David S.
  if (evtStore()->record(theXAODContainers.second,vxContainerAuxName).isFailure())
    {
      if (msgLvl(MSG::INFO)) msg() << "Unable to record VertexAuxContainer in TDS" << endreq;
      return StatusCode::FAILURE;
    }
  return StatusCode::SUCCESS;
}

//============================================================================================

