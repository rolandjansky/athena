/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          ImagingSeedFinder.cxx - Description in header file
*********************************************************************/
//#define ImagingSeedFinder

#include "TrkVertexSeedFinderTools/ImagingSeedFinder.h"

#include "TrkVertexSeedFinderUtils/IVertexImageMaker.h"
#include "TrkVertexSeedFinderUtils/IVertexClusterFinder.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "GaudiKernel/PhysicalConstants.h"

//Amg
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk
{

  // --------------------------------------------------------------------------------
  // ImagingSeedFinder constructor
  ImagingSeedFinder::ImagingSeedFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
    m_vertexImageMaker("Trk::VertexImageMaker", this),
    m_VertexClusterFinder( "Trk::SimpleVertexClusterFinder", this),
    m_cacheRunNumber(0), m_cacheEventNumber(0), m_currentSeedIdx(0)
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
    msg(MSG::INFO) << "ImagingSeedFinder initialization..." << endmsg;

    ATH_CHECK( m_eventInfoKey.initialize() );

    // Get the image maker tool
    if (m_vertexImageMaker.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve vertex image maker tool" << m_vertexImageMaker << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved vertex cluster tool " << m_vertexImageMaker << endmsg;
    }
    

    /* Get the VertexCluster Tool */
    if ( m_VertexClusterFinder.retrieve().isFailure() )
      {
        msg(MSG::FATAL) << "Failed to retrieve vertex cluster tool" << m_VertexClusterFinder << endmsg;
        return StatusCode::FAILURE;
      }
    else
      {
        msg(MSG::INFO) << "Retrieved vertex cluster tool " << m_VertexClusterFinder << endmsg;
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
  Amg::Vector3D ImagingSeedFinder::findSeed(const std::vector<const Trk::Track*> & VectorTrk,const xAOD::Vertex * constraint) {

    // This gets the EventInfo object from StoreGate
    SG::ReadHandle<xAOD::EventInfo> myEventInfo(m_eventInfoKey);
    if ( !myEventInfo.isValid() )
    {
      msg(MSG::ERROR) << "Failed to retrieve event information" << endmsg;
      return Amg::Vector3D(0.,0.,0.);
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
       return Amg::Vector3D(0.,0.,0.);
      } else {
       Amg::Vector3D TheVertex = m_seeds[m_currentSeedIdx];
       m_currentSeedIdx++;
       return TheVertex;
      }

  } // End ImagingSeedFinder find seed - based on vector of Trk::Track's


  // --------------------------------------------------------------------------------
  // ImagingSeedFinder find seed - based on vector of Trk::ParametersBase
  Amg::Vector3D ImagingSeedFinder::findSeed(const std::vector<const Trk::TrackParameters*> & parametersList,const xAOD::Vertex * constraint) {

    // This gets the EventInfo object from StoreGate
    SG::ReadHandle<xAOD::EventInfo> myEventInfo(m_eventInfoKey);
    if (!myEventInfo.isValid() ) 
      {
      msg(MSG::ERROR) << "Failed to retrieve event information" << endmsg;
      return Amg::Vector3D(0.,0.,0.);
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
       return Amg::Vector3D(0.,0.,0.);
      } else {
       Amg::Vector3D TheVertex = m_seeds[m_currentSeedIdx];
       m_currentSeedIdx++;
       return TheVertex;
      }

  } // End  ImagingSeedFinder find seed - based on vector of Trk::ParametersBase

  // --------------------------------------------------------------------------------
  // ImagingSeedFinder find multiseed - based on vector of Trk::Track's
  std::vector<Amg::Vector3D> ImagingSeedFinder::findMultiSeeds(const std::vector<const Trk::Track*>&  vectorTrk ,const xAOD::Vertex *  constraint ) {
 
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
  std::vector<Amg::Vector3D> ImagingSeedFinder::findMultiSeeds(const std::vector<const Trk::TrackParameters*>& parametersList,const xAOD::Vertex * constraint) {
 
    //implemented to satisfy inheritance but this algorithm only supports one seed at a time
    msg(MSG::DEBUG) << "findMultiSeeds using ImagingSeedFinder has been called" << endmsg;

    // Make the image from tracks, and immediately
    VertexImage image = m_vertexImageMaker->makeVertexImage( parametersList, constraint );

    //Run clustering tool to find vertices
    //Coordinates are relative to the center of the histogram (constraint or (0,0,0) if no constraint)
    std::vector<Amg::Vector3D> relCoordVertices = m_VertexClusterFinder->findVertexClusters( image );

    msg(MSG::DEBUG) << "Found " << relCoordVertices.size() << " candidate clusters" << endmsg;
 
    // Loop through the vertices in relative coordinate space, ask the image to convert to absolute positions, and set x,y to constraint if we have one.

    std::vector<Amg::Vector3D> vertices;
    int counter=0;
    for( std::vector<Amg::Vector3D>::iterator relit = relCoordVertices.begin(); relit != relCoordVertices.end(); relit++ ){
      if(constraint) {
        vertices.push_back( Amg::Vector3D( constraint->position().x(),
                                           constraint->position().y(),
                                           constraint->position().z()+relit->z() ) );

      } else {
        vertices.push_back( Amg::Vector3D( relit->x(),
                                           relit->y(),
                                           relit->z() ) );


      }		
        msg(MSG::DEBUG) << "  Seed " << counter << " = ( " << vertices.back().x() << ", " << vertices.back().y() << ", " << vertices.back().z() << " )" << endmsg;

      counter++;
    }


    return vertices;

  } // End ImagingSeedFinder find multiseed - based on vector of Trk::ParametersBase

  void ImagingSeedFinder::setPriVtxPosition(double /* vx */, double /* vy */) {
    //implemented to satisfy inheritance
  }

  int ImagingSeedFinder::perigeesAtSeed( std::vector<const Trk::TrackParameters*> */* a */ ,
					 const std::vector<const Trk::TrackParameters*> & /* b */ ) const{
    //implemented to satisfy inheritance
    return 0;
  }

  int ImagingSeedFinder::getModes1d(std::vector<float> & /* a */, std::vector<float> & /* b */, 
				    std::vector<float> & /* c */, std::vector<float> & /* d */) const{
   //implemented to satisfy inheritance
    return 0;
  }

  void ImagingSeedFinder::getCorrelationDistance( double &/* cXY */, double &/* cZ */ ){
    //implemented to satisfy inheritance
  }


} //End Trk namespace
