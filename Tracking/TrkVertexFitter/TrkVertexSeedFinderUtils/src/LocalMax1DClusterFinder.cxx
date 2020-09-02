/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          LocalMax1DClusterFinder.cxx - Description in header file
*********************************************************************/
#include "TrkVertexSeedFinderUtils/LocalMax1DClusterFinder.h"
#include "TrkVertexSeedFinderUtils/VertexImage.h"
#include <algorithm>

namespace Trk
{
  
  namespace {
#if 0
    bool compareMaxZ( LocalMax1DClusterFinder::Projection m1,  LocalMax1DClusterFinder::Projection m2 ) {
      return (m1.first < m2.first);
    }
#endif
  }

  LocalMax1DClusterFinder::LocalMax1DClusterFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    base_class(t,n,p),
    m_weightThreshold( 1500.0 ) ,
    m_mergeParam( 0.95 ),
    m_clusterWindowXY( 0.34 ),
    m_refineZ( true ),
    m_gaussianWindow( true )
  {   
    //threshold to consider a local max for seeding
    declareProperty("weightThreshold", m_weightThreshold );
    //merging parameter for nearby local maxima
    declareProperty("mergeParameter",m_mergeParam);
    //window in xy bin space within which clusters are formed
    declareProperty("clusterWindowXY", m_clusterWindowXY );
    //do quadratic refinement of peak positions
    declareProperty("refineZ", m_refineZ);
    //use gaussian window for z projection
    declareProperty("gaussianWindow", m_gaussianWindow);
  }
  
  
  // --------------------------------------------------------------------------------
  // Find vertex clusters of input image
  std::vector<Amg::Vector3D> LocalMax1DClusterFinder::findVertexClusters( const VertexImage & image ) const
  {

    std::vector<float> zproj;

    if (m_gaussianWindow) {
      zproj = image.projectGaussianOnZ( m_clusterWindowXY );
    } else {
      zproj = image.projectRectangleOnZ( m_clusterWindowXY, m_clusterWindowXY);
    }

    //for (size_t i = 0; i < zproj.size(); i++) ATH_MSG_ALWAYS(i << " : " << image.getRelPosZ(i)<< " : " << zproj[i]);

    // vector to store local maxima
    std::vector<Projection> vmax;  
  
    for( size_t i=1; i< zproj.size()-1; ++i) {

      // Check if bin is a local max
      if( zproj[i] > m_weightThreshold && zproj[i] > zproj[i-1] && zproj[i] > zproj[i+1] ) {
        //bin i is a local max
        vmax.emplace_back(i,zproj[i]);
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
    std::vector<Amg::Vector3D> vertices;    


    //Set them to relative center of histogram in x,y
    float x = image.getRelPosX( ((float) image.getNBinsX())/2. );
    float y = image.getRelPosY( ((float) image.getNBinsY())/2. );
    for(auto & m : vmax) {
      if (!m_refineZ || zproj[m.first] <= 0) {
          vertices.emplace_back( x, y, image.getRelPosZ(m.first) );
      } else {
	  float z;
	  float z2 = image.getRelPosZ(m.first);
	  // handle edge cases (max at first or last z bin)
	  if (m.first == 0) {
	    float z3 = image.getRelPosZ(m.first+1);
	    if (zproj[m.first+1] > 0 && zproj[m.first + 2] > zproj[m.first+1]) {
	      float w3 = zproj[m.first+1]*(zproj[m.first]/(zproj[m.first]+zproj[m.first+2]));
    	      z = (z2*zproj[m.first] + z3*w3)/(zproj[m.first]+w3);
	    } else if (zproj[m.first+1] > 0) {
	      z = (z2*zproj[m.first] + z3*zproj[m.first+1])/(zproj[m.first] + zproj[m.first+1]);
	    } else {
	      z = image.getRelPosZ(m.first);
	    }
	  } else if (m.first == image.getNBinsZ() - 1) {
	    float z1 = image.getRelPosZ(m.first-1);
	    if (zproj[m.first-1] > 0 && zproj[m.first-2] > zproj[m.first-1]) {
	      float w1 = zproj[m.first-1]*(zproj[m.first]/(zproj[m.first]+zproj[m.first-2]));
	      z = (z1*w1 + z2*zproj[m.first])/(w1 + zproj[m.first]);
	    } else if (zproj[m.first-1] > 0) {
	      z = (z1*zproj[m.first-1] + z2*zproj[m.first])/(zproj[m.first-1] + zproj[m.first]);
	    } else {
	      z = image.getRelPosZ(m.first);
	    }
	  // not at an edge, do quadratic refinement
	  } else {  
	    if (zproj[m.first-1] > 0 && zproj[m.first] > 0 && zproj[m.first+1] > 0 ) {
	      float z1 = image.getRelPosZ(m.first-1);
	      float w2 = zproj[m.first];
	      float w1;
	      float w3;
	      if (m.first > 1 && zproj[m.first-2] > zproj[m.first-1]) {
		w1 = zproj[m.first-1] * w2/(w2 + zproj[m.first-2]);
	      } else {
		w1 = zproj[m.first-1];
	      }
	      if (m.first < image.getNBinsZ() - 2 && zproj[m.first+2] > zproj[m.first+1]) {
		w3 = zproj[m.first+1] * w2/(w2 + zproj[m.first+2]);
	      } else {
		w3 = zproj[m.first+1];
	      }
	      if ( 4 * (w2 - w1) + 2 * (w1 - w3) > 0 ) {
		z = z2 + (z2 - z1) * (w3 - w1)/(4 * (w2 - w1) + 2 * (w1 - w3));
	      } else {  // degenerate (linear) or concave up cases should never happen since z2 is a local maximum
		ATH_MSG_WARNING( "unexpected histogram shape ("<<w1<<","<<w2<<","<<w3<<")"  );
		z = image.getRelPosZ(m.first);
	      }
	    } else {
	      z = image.getRelPosZ(m.first);
	    }
	  }
	  vertices.emplace_back( x, y, z );
      }
    }

    ATH_MSG_DEBUG( "returning " << vertices.size() << " clusters"  );
    return vertices;

  } //End findLocalMax1DClusterFinder


 
} //End Trk namespace
