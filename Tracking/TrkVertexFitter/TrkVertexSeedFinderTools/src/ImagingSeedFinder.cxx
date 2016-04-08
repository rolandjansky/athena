/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          ImagingSeedFinder.cxx - Description in header file
*********************************************************************/
//#define ImagingSeedFinder

#include "TrkVertexSeedFinderTools/ImagingSeedFinder.h"

#include "TrkVertexSeedFinderUtils/IVertexImageMaker.h"
#include "TrkVertexSeedFinderUtils/IVertexClusterFinder.h"

#include "TrkParameters/TrackParameters.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/Vertex.h"
#include "TrkTrack/Track.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "xAODEventInfo/EventInfo.h"

//Amg
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk
{

  // --------------------------------------------------------------------------------
  // ImagingSeedFinder constructor
  ImagingSeedFinder::ImagingSeedFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
    m_vertexImageMaker("Trk::VertexImageMaker"),
    m_VertexClusterFinder( "Trk::SimpleVertexClusterFinder" )
  {   

    declareProperty("VertexImageMaker"        , m_vertexImageMaker         );
    declareProperty("VertexCluster"           , m_VertexClusterFinder      );
    declareInterface<IVertexSeedFinder>(this);
  } //End ImagingSeedFinder constructor

  // --------------------------------------------------------------------------------
  // ImagingSeedFinder destructor
  ImagingSeedFinder::~ImagingSeedFinder() {} //End ImagingSeedFinder destructor


  // --------------------------------------------------------------------------------
  // ImagingSeedFinder inistialize
  StatusCode ImagingSeedFinder::initialize() 
  { 
    msg(MSG::INFO) << "ImagingSeedFinder initialization..." << endreq;

    // Get the image maker tool
    if (m_vertexImageMaker.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve vertex image maker tool" << m_vertexImageMaker << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved vertex cluster tool " << m_vertexImageMaker << endreq;
    }
    

    /* Get the VertexCluster Tool */
    if ( m_VertexClusterFinder.retrieve().isFailure() )
      {
        msg(MSG::FATAL) << "Failed to retrieve vertex cluster tool" << m_VertexClusterFinder << endreq;
        return StatusCode::FAILURE;
      }
    else
      {
        msg(MSG::INFO) << "Retrieved vertex cluster tool " << m_VertexClusterFinder << endreq;
      }

    return StatusCode::SUCCESS;

  }// End ImagingSeedFinder initialize


  // --------------------------------------------------------------------------------
  // ImagingSeedFinder finalize
  StatusCode ImagingSeedFinder::finalize() 
  {

    return StatusCode::SUCCESS;
  }// End ImagingSeedFinder finalize


  // --------------------------------------------------------------------------------
  // ImagingSeedFinder find seed - based on vector of Trk::Track's
  Vertex ImagingSeedFinder::findSeed(const std::vector<const Trk::Track*> & VectorTrk,const RecVertex * constraint) {

    // This gets the EventInfo object from StoreGate
    const xAOD::EventInfo* myEventInfo = 0;
    if(evtStore()->retrieve(myEventInfo/*,"MyEvent"*/).isFailure()) {
      // Key "MyEvent" is optional, usually not specified for EventInfo because
      // there'll be only one. When not specified, just takes the first container.
      msg(MSG::ERROR) << "Failed to retrieve event information" << endreq;
      return Vertex(Amg::Vector3D(0.,0.,0.));
    }
    
    unsigned int ei_RunNumber = myEventInfo->runNumber();
    unsigned int ei_EventNumber =myEventInfo->eventNumber();

    if (  (ei_RunNumber != m_cacheRunNumber) || (ei_EventNumber != m_cacheEventNumber) ){
      m_seeds = findMultiSeeds( VectorTrk, constraint);
      m_currentSeedIdx = 0;
      m_cacheRunNumber = ei_RunNumber;
      m_cacheEventNumber = ei_EventNumber;
    } 


     if (m_currentSeedIdx >= m_seeds.size()) {
       return Vertex(Amg::Vector3D(0.,0.,0.));
      } else {
       Vertex TheVertex = m_seeds[m_currentSeedIdx];
       m_currentSeedIdx++;
       return TheVertex;
      }

  } // End ImagingSeedFinder find seed - based on vector of Trk::Track's


  // --------------------------------------------------------------------------------
  // ImagingSeedFinder find seed - based on vector of Trk::ParametersBase
  Vertex ImagingSeedFinder::findSeed(const std::vector<const Trk::TrackParameters*> & parametersList,const RecVertex * constraint) {


    // This gets the EventInfo object from StoreGate
    const xAOD::EventInfo* myEventInfo = 0;
    if(evtStore()->retrieve(myEventInfo/*,"MyEvent"*/).isFailure()) {
      // Key "MyEvent" is optional, usually not specified for EventInfo because
      // there'll be only one. When not specified, just takes the first container.
      msg(MSG::ERROR) << "Failed to retrieve event information" << endreq;
      return Vertex(Amg::Vector3D(0.,0.,0.));
    }
    
    unsigned int ei_RunNumber = myEventInfo->runNumber();
    unsigned int ei_EventNumber =myEventInfo->eventNumber();

    if (  (ei_RunNumber != m_cacheRunNumber) || (ei_EventNumber != m_cacheEventNumber) ){
      m_seeds = findMultiSeeds( parametersList, constraint);
      m_currentSeedIdx = 0;
      m_cacheRunNumber = ei_RunNumber;
      m_cacheEventNumber = ei_EventNumber;
    } 


     if (m_currentSeedIdx >= m_seeds.size()) {
       return Vertex(Amg::Vector3D(0.,0.,0.));
      } else {
       Vertex TheVertex = m_seeds[m_currentSeedIdx];
       m_currentSeedIdx++;
       return TheVertex;
      }

  } // End  ImagingSeedFinder find seed - based on vector of Trk::ParametersBase


  // --------------------------------------------------------------------------------
  // ImagingSeedFinder find multiseed - based on vector of Trk::Track's
  std::vector<Vertex> ImagingSeedFinder::findMultiSeeds(const std::vector<const Trk::Track*>&  vectorTrk ,const RecVertex *  constraint ) {
 
    //create perigees from track list
    std::vector<const Trk::TrackParameters*> perigeeList;
    
    std::vector<const Trk::Track*>::const_iterator begin=vectorTrk.begin();
    std::vector<const Trk::Track*>::const_iterator end=vectorTrk.end();
    
    for (std::vector<const Trk::Track*>::const_iterator iter=begin;
	 iter!=end;++iter) {
      if (isnan((*iter)->perigeeParameters()->parameters()[Trk::d0])) {
	continue;
      }  
      perigeeList.push_back((*iter)->perigeeParameters());
    }
   
    //create seed from perigee list
    return findMultiSeeds(perigeeList,constraint);

  } // End ImagingSeedFinder find multiseed - based on vector of Trk::Track's


  // --------------------------------------------------------------------------------
  // ImagingSeedFinder find multiseed - based on vector of Trk::ParametersBase
  std::vector<Vertex> ImagingSeedFinder::findMultiSeeds(const std::vector<const Trk::TrackParameters*>& parametersList,const RecVertex * constraint) {
 
    //implemented to satisfy inheritance but this algorithm only supports one seed at a time
    msg(MSG::DEBUG) << "findMultiSeeds using ImagingSeedFinder has been called" << endreq;

    // Make the image from tracks, and immediately
    VertexImage image = m_vertexImageMaker->makeVertexImage( parametersList, constraint );

    //Run clustering tool to find vertices
    //Coordinates are relative to the center of the histogram (constraint or (0,0,0) if no constraint)
    std::vector<Vertex> relCoordVertices = m_VertexClusterFinder->findVertexClusters( image );

    msg(MSG::DEBUG) << "Found " << relCoordVertices.size() << " candidate clusters" << endreq;
 
    // Loop through the vertices in relative coordinate space, ask the image to convert to absolute positions, and set x,y to constraint if we have one.

    std::vector<Vertex> vertices;
    int counter=0;
    for( std::vector<Vertex>::iterator relit = relCoordVertices.begin(); relit != relCoordVertices.end(); relit++ ){
      if(constraint) {
        vertices.push_back( Vertex(Amg::Vector3D( constraint->position().x(),
                                                  constraint->position().y(),
                                                  constraint->position().z()+relit->position().z() )) );

      } else {
        vertices.push_back( Vertex(Amg::Vector3D( relit->position().x(),
                                                  relit->position().y(),
                                                  relit->position().z() ) ) );


      }		
        msg(MSG::DEBUG) << "  Seed " << counter << " = ( " << vertices.back().position().x() << ", " << vertices.back().position().y() << ", " << vertices.back().position().z() << " )" << endreq;

      counter++;
    }


    return vertices;

  } // End ImagingSeedFinder find multiseed - based on vector of Trk::ParametersBase


} //End Trk namespace
