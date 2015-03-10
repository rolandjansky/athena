/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          LocalMax1DClusterFinder.cxx - Description in header file
*********************************************************************/
#include "TrkVertexSeedFinderUtils/LocalMax1DClusterFinder.h"
#include "VxVertex/Vertex.h"
#include "TrkVertexSeedFinderUtils/VertexImage.h"
#include <algorithm>

namespace Trk
{
  
  namespace {
    bool compareMaxZ( LocalMax1DClusterFinder::Projection m1,  LocalMax1DClusterFinder::Projection m2 ) {
      return (m1.first < m2.first);
    }
  }

  LocalMax1DClusterFinder::LocalMax1DClusterFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
    m_weightThreshold( 0.02 ) ,
    m_mergeParam( 0.5 ),
    m_clusterWindowXY( 3 )
  {   
    //threshold to consider a local max for seeding
    declareProperty("weightThreshold", m_weightThreshold );
    //merging parameter for nearby local maxima
    declareProperty("mergeParameter",m_mergeParam);
    //window in xy bin space within which clusters are formed
    declareProperty("clusterWindowXY", m_clusterWindowXY );
    declareInterface<IVertexClusterFinder>(this);
  }
  
  LocalMax1DClusterFinder::~LocalMax1DClusterFinder() {}
  
  StatusCode LocalMax1DClusterFinder::initialize() { 
    //no initializiation needed
    msg(MSG::INFO) << "Initialize successful" << endreq;
    return StatusCode::SUCCESS;
  }
  
  StatusCode LocalMax1DClusterFinder::finalize() 
  {
    msg(MSG::INFO)  << "Finalize successful" << endreq;
    return StatusCode::SUCCESS;
  }


  // --------------------------------------------------------------------------------
  // Find vertex clusters of input image
  std::vector<Trk::Vertex> LocalMax1DClusterFinder::findVertexClusters( const VertexImage & image ){

    std::vector<float> zproj = image.projectRectangleOnZ( m_clusterWindowXY, m_clusterWindowXY);

    // vector to store local maxima
    std::vector<Projection> vmax;  
  
    for( size_t i=1; i< zproj.size()-1; ++i) {

      // Check if bin is a local max
      if( zproj[i] > m_weightThreshold && zproj[i] > zproj[i-1] && zproj[i] > zproj[i+1] ) {
        //bin i is a local max
        vmax.push_back(std::make_pair(i,zproj[i]));
      }
    }


    //consider closest z neighbours pair wise for merging
    if( vmax.size() > 1 ) {
      for(std::vector<Projection>::iterator it=vmax.begin(); it!=vmax.end(); ) {
        //get the next iterator
        std::vector<Projection>::iterator next = it+1;
        //break out if we are on the last one        
        if(next==vmax.end())
          break;

        //get lowest weight between these two max, stepping between them
        float minw = it->second;

        //must be at least 1 bin between them since they are both local maxima
        for(int i=it->first+1; i<next->first; ++i) {
          if(zproj[i] < minw)
            minw = zproj[i];
        }

        //Calculate merging parameter
        float mergeParam = minw / std::min( it->second, next->second );
        if( mergeParam > m_mergeParam ) { //merge these together
          if( it->second > next->second ) { //we should keep it and delete next
            it = vmax.erase( next ); //it now points at the element after next in the original vector
            it--; //step back so that "it" is pointing at the *same thing* it started on
            //then we can go to the next part of the loop and compare the same local max to the next one in the list
          } else { //keep next and delete it
            it = vmax.erase( it );  //it now points at the element next used to point at
            //don't need to move it, can go to next part of loop and compare to the one after that
          }
        } else { //do not merge
          //go on to the next one directly
          it++;
        }
        
      } //loop over maxima
    } //if we had at least 2 maxima to consider for merging

    // Fill return vector
    std::vector<Vertex> vertices;    


    //Set them to relative center of histogram in x,y
    float x = image.getRelPosX( ((float) image.getNBinsX())/2. );
    float y = image.getRelPosY( ((float) image.getNBinsY())/2. );
    for(auto & m : vmax) {
      vertices.push_back( Vertex(Amg::Vector3D( x, y, image.getRelPosZ(m.first) ) ) );
    }

    msg(MSG::DEBUG) << "returning " << vertices.size() << " clusters" << endreq;
    return vertices;

  } //End findLocalMax1DClusterFinder


 
} //End Trk namespace
