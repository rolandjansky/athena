/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexSeedFinderUtils/VertexImage.h"
#include <cmath>

namespace Trk {

  VertexImage::VertexImage( float * hist_3d, int nx, int ny, int nz, 
                            float xrange, float yrange, float zrange,
                            bool inplace ) :
    m_hist_3d(hist_3d),
    m_nbinsx(nx),
    m_nbinsy(ny),
    m_nbinsz(nz),
    m_xrange(xrange),
    m_yrange(yrange),
    m_zrange(zrange),
    m_inplace(inplace) {

    //Calculate size of histogram and storage array
    m_binstot = m_nbinsx*m_nbinsy*m_nbinsz;
    m_arraytot = m_inplace ? m_nbinsx*m_nbinsy*(m_nbinsz+2) :
      m_nbinsx*m_nbinsy*m_nbinsz;

    //Calculate binwidths
    m_wx = 2*m_xrange/((float) m_nbinsx);
    m_wy = 2*m_yrange/((float) m_nbinsy);
    m_wz = 2*m_zrange/((float) m_nbinsz);
  }

  // Given indices (x,y,z), return overall array index of that bin
  int VertexImage::getRMBin(const int & x, const int & y, const int & z) const {
    int ret = m_inplace ? z + (m_nbinsz+2)*y + (m_nbinsz+2)*m_nbinsy*x :
      z + m_nbinsz*y + m_nbinsz*m_nbinsy*x;
    return ret;
  }

  // Given overall array index i of a bin, return indices (x,y,z) by reference
  void VertexImage::getInvRMBin( const int & i, int & x, int & y, int &z ) const {
    if(m_inplace) {
      z = i % (m_nbinsz+2);
      y = ( (i-z)/(m_nbinsz+2) ) % m_nbinsy;
      x = ( i - z - y*(m_nbinsz+2) )/( (m_nbinsz+2)*m_nbinsy );
    } else {
      z = i % (m_nbinsz);
      y = ( (i-z)/(m_nbinsz) ) % m_nbinsy;
      x = ( i - z - y*(m_nbinsz) )/( (m_nbinsz)*m_nbinsy );
    }
  }

  // Find array indices of all bins adjacent to index
  std::vector<int> VertexImage::getAdjacentBins( int index ) const {
    
    int x,y,z;
    
    getInvRMBin( index, x,y,z );
   
    std::vector<int> v;
    
    if(x!=0) v.push_back(getRMBin( x-1, y, z));
    if(y!=0) v.push_back(getRMBin( x, y-1, z));
    if(z!=0) v.push_back(getRMBin( x, y, z-1));
    
    if(x<m_nbinsx-1) v.push_back(getRMBin( x+1, y, z));
    if(y<m_nbinsy-1) v.push_back(getRMBin( x, y+1, z));
    if(z<m_nbinsz-1) v.push_back(getRMBin( x, y, z+1));
    
    return v;

  }

  //Project to a 1D distribution in z using full xy range
  std::vector<float> VertexImage::projectRectangleOnZ() const {
    return projectRectangleOnZ( 0, m_nbinsx, 0, m_nbinsy);
  }

  // Given a rectangle in xy, project down to a 1D distribution in z
  // min is inclusive, and max is exclusive (so full range is 0 to m_nbins)
  std::vector<float> VertexImage::projectRectangleOnZ( int xmin, int xmax, int ymin, int ymax ) const {

    //create return structure
    std::vector<float> vret(m_nbinsz);

    // Loop over z
    for( int z=0; z<m_nbinsz; ++z ) {

      //Do the projection
      for(int x=xmin; x < xmax; ++x) {
        for(int y=ymin; y < ymax; ++y) {
          int idx = getRMBin(x,y,z);
          vret[z] += m_hist_3d[idx];
        }
      }
 
    }

    return vret;
  }

  // Project bins within nsigma standard deviations of the mean of the image
  std::vector<float> VertexImage::projectRectangleOnZ( float nsigmax, float nsigmay) const {

    //Calculate the mean and standard deviation in x and y
    double meanx=0;
    double mx2=0;
    double meany=0;
    double my2=0;
    double tot=0;

    //----------------------------------------
    //One pass variant -- having precision issues when including negative weights, so cut them
    // Loop through all actual bins in the histogram
    for(int x=0; x < m_nbinsx; ++x) {
      for(int y=0; y < m_nbinsy; ++y) {

        double w=0;
        //Add up whole z range to help with computational problems with negative weights
        for( int z=0; z<m_nbinsz; ++z ) {
          int i = getRMBin(x,y,z);
          w+= m_hist_3d[i];
        }

        if(w < 0)
          continue;

        tot += w;

        // for tot to be 0 w has to be also (so shouldn't update the values
        if(tot==0)
          continue;

        //diff to last x mean
        double diffx = ((double) x+0.5) - meanx;
        //update mean x
        meanx += w*diffx/tot;
        //update squared difference 
        mx2 += w * diffx * (((double) x+0.5 ) - meanx);
        
        //diff to last y mean
        double diffy = ((double) y+0.5) - meany;
        //update mean y
        meany += w*diffy/tot;
        //update squared difference 
        my2 += w * diffy * (((double) y +0.5) - meany);
        
      }
    }
    
    //If for some reason the total weight is 0 (empty histogram?), just project everything
    if( tot == 0 ) {
      return projectRectangleOnZ();
    }

    // Finalize standard deviation
    float sigmax = sqrt( mx2/tot );
    float sigmay = sqrt( my2/tot );

    //----------------------------------------
    //End 1 pass variant
  
    // Project using any bin touched by n standard deviations:
    // Cast bounds to int, add 1 to the max 
    
    int xmin = meanx - sigmax*nsigmax;
    int xmax = meanx + sigmax*nsigmax + 1.;

    int ymin = meany - sigmay*nsigmay;
    int ymax = meany + sigmay*nsigmay + 1.;

    // Ensure no boundary issues (could result from large std. dev.)
    if(xmin < 0)
      xmin = 0;
    if(xmax > m_nbinsx)
      xmax = m_nbinsx;
    if(ymin < 0)
      ymin = 0;
    if(ymax > m_nbinsy)
      ymax = m_nbinsy;

    return projectRectangleOnZ( xmin, xmax, ymin, ymax );

  }

        

}
