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
    base_class(t,n,p),
    m_vertexImageMaker("Trk::VertexImageMaker", this),
    m_VertexClusterFinder( "Trk::SimpleVertexClusterFinder", this)
  {   

    declareProperty("VertexImageMaker"        , m_vertexImageMaker         );
    declareProperty("VertexCluster"           , m_VertexClusterFinder      );
  } //End ImagingSeedFinder constructor


  ImagingSeedFinder::~ImagingSeedFinder()
  {
  }


  // --------------------------------------------------------------------------------
  // ImagingSeedFinder inistialize
  StatusCode ImagingSeedFinder::initialize() 
  { 
    ATH_MSG_INFO( "ImagingSeedFinder initialization..."  );

    ATH_CHECK( m_vertexImageMaker.retrieve() );
    ATH_CHECK( m_VertexClusterFinder.retrieve() );

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
  Amg::Vector3D ImagingSeedFinder::findSeed(const std::vector<const Trk::Track*> & vectorTrk,const xAOD::Vertex * constraint) const
  {
    std::vector<Amg::Vector3D> seeds =
      findMultiSeeds( vectorTrk, constraint);

    if (seeds.empty()) {
      return Amg::Vector3D(0.,0.,0.);
    }
    return seeds[0];
  } // End ImagingSeedFinder find seed - based on vector of Trk::Track's


  // --------------------------------------------------------------------------------
  // ImagingSeedFinder find seed - based on vector of Trk::ParametersBase
  Amg::Vector3D ImagingSeedFinder::findSeed(const std::vector<const Trk::TrackParameters*> & parametersList,const xAOD::Vertex * constraint) const
  {
    std::vector<Amg::Vector3D> seeds =
      findMultiSeeds( parametersList, constraint);

    if (seeds.empty()) {
      return Amg::Vector3D(0.,0.,0.);
    }
    return seeds[0];
  } // End  ImagingSeedFinder find seed - based on vector of Trk::ParametersBase

  // --------------------------------------------------------------------------------
  // ImagingSeedFinder find multiseed - based on vector of Trk::Track's
  std::vector<Amg::Vector3D> ImagingSeedFinder::findMultiSeeds(const std::vector<const Trk::Track*>&  vectorTrk ,const xAOD::Vertex *  constraint ) const {
 
    //create perigees from track list
    std::vector<const Trk::TrackParameters*> perigeeList;
    perigeeList.reserve (vectorTrk.size());

    for (const Trk::Track* track : vectorTrk) {
      if (isnan(track->perigeeParameters()->parameters()[Trk::d0])) {
	continue;
      }  
      perigeeList.push_back(track->perigeeParameters());
    }
   
    //create seed from perigee list
    return findMultiSeeds(perigeeList,constraint);

  } // End ImagingSeedFinder find multiseed - based on vector of Trk::Track's


  // --------------------------------------------------------------------------------
  // ImagingSeedFinder find multiseed - based on vector of Trk::ParametersBase
  std::vector<Amg::Vector3D>
  ImagingSeedFinder::findMultiSeeds(const std::vector<const Trk::TrackParameters*>& parametersList,
                                    const xAOD::Vertex * constraint) const
  {
    ATH_MSG_DEBUG( "findMultiSeeds using ImagingSeedFinder has been called"  );

    // Make the image from tracks, and immediately
    std::unique_ptr<VertexImage> image =
      m_vertexImageMaker->makeVertexImage( parametersList, constraint );

    //Run clustering tool to find vertices
    //Coordinates are relative to the center of the histogram (constraint or (0,0,0) if no constraint)
    std::vector<Amg::Vector3D> vertices =
      m_VertexClusterFinder->findVertexClusters( *image );

    ATH_MSG_DEBUG( "Found " << vertices.size() << " candidate clusters"  );
 
    // Loop through the vertices in relative coordinate space, and set x,y to constraint if we have one.

    int counter=0;
    for (Amg::Vector3D& v : vertices) {
      if(constraint) {
        v = Amg::Vector3D (constraint->position().x(),
                           constraint->position().y(),
                           constraint->position().z() + v.z());
      }		
      ATH_MSG_DEBUG( "  Seed " << counter << " = ( " << v.x() << ", " << v.y() << ", " << v.z() << " )"  );

      counter++;
    }


    return vertices;

  } // End ImagingSeedFinder find multiseed - based on vector of Trk::ParametersBase


} //End Trk namespace
