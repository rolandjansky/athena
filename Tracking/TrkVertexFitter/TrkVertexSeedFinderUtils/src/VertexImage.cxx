/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexSeedFinderUtils/VertexImage.h"
#include "EventPrimitives/EventPrimitives.h"
#include <cmath>
#include <iostream>

namespace Trk {

  VertexImage::VertexImage( double* array,
                            arrayDeleter* deleter,
                            int nx, int ny, int nz,
                            float xrange, float yrange, float zrange) :
    m_hist_3d(array),
    m_deleter(deleter),
    m_nbinsx(nx),
    m_nbinsy(ny),
    m_nbinsz(nz),
    m_xrange(xrange),
    m_yrange(yrange),
    m_zrange(zrange)
  {
    //Calculate size of histogram and storage array
    m_binstot = m_nbinsx*m_nbinsy*m_nbinsz;
    m_arraytot = m_nbinsx*m_nbinsy*(m_nbinsz+2);

    //Calculate binwidths
    m_wx = 2*m_xrange/((float) m_nbinsx);
    m_wy = 2*m_yrange/((float) m_nbinsy);
    m_wz = 2*m_zrange/((float) m_nbinsz);
  }


  // Given indices (x,y,z), return overall array index of that bin
  int VertexImage::getRMBin(const int & x, const int & y, const int & z) const {
    int ret = z + (m_nbinsz+2)*y + (m_nbinsz+2)*m_nbinsy*x;
    return ret;
  }

  // Given overall array index i of a bin, return indices (x,y,z) by reference
  void VertexImage::getInvRMBin( const int & i, int & x, int & y, int &z ) const {
    z = i % (m_nbinsz+2);
    y = ( (i-z)/(m_nbinsz+2) ) % m_nbinsy;
    x = ( i - z - y*(m_nbinsz+2) )/( (m_nbinsz+2)*m_nbinsy );
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

  // Project on Z around fitted centroid using Gaussian weighting
  std::vector<float> VertexImage::projectGaussianOnZ(float scale) const {
    float x = 0.0;
    float y = 0.0;
    float sigmax = 0.0;
    float sigmay = 0.0;
    fitCentroid(x, y, sigmax, sigmay);
    return projectGaussianOnZ(x, y, scale * sigmax, scale * sigmay);
  }

  // Fit the centroid by projecting on x,y
  void VertexImage::fitCentroid(float& x, float& y, float& sigmax, float& sigmay) const {
    // defaults in case something goes wrong
    x = m_nbinsx/2;
    y = m_nbinsy/2;
    sigmax = m_nbinsx/2;
    sigmay = m_nbinsy/2;

    float* hxy = new float[m_nbinsx*m_nbinsy];
    float sumPos = 0.0;
    int nPos = 0;
    float sumx = 0;
    float sumy = 0;
    float sumx2 = 0;
    float sumy2 = 0;
    for (int ix = 0; ix < m_nbinsx; ix++) {
      for (int iy = 0; iy < m_nbinsy; iy++) {
	int ibin = ix + iy * m_nbinsx;
	hxy[ibin] = 0.0;
	for (int iz = 0; iz < m_nbinsz; iz++) {
	  int idx = getRMBin(ix, iy, iz);
	  hxy[ibin] += m_hist_3d[idx];
	}
	if (hxy[ibin] > 0) {
	  sumPos += hxy[ibin];
	  nPos++;
	  sumx += (ix * hxy[ibin]);
	  sumy += (iy * hxy[ibin]);
	  sumx2+= (ix * ix * hxy[ibin]);
	  sumy2+= (iy * iy * hxy[ibin]);
	}
      }
    }
    // compute average position and width of positive weights
    if (nPos <= 0) {
      //msg(MSG::WARNING) << "XY projection of image has no positive weights" << endmsg;
      if (hxy) delete[] hxy;
      return;
    }
    // update fallback quantities to something better
    x = sumx/sumPos;
    y = sumy/sumPos;
    sigmax = sqrt(sumx2/sumPos - x*x);
    sigmay = sqrt(sumy2/sumPos - y*y);
    //std::cout << "VertexImage: fallback values " << x << ", " << y << " : " << sigmax << ", " << sigmay << std::endl;

    // we will fit the centroid using only bins which are higher than the average of positive bins
    // this avoids numerical problems with negative bins

    // if we don't have at least 5 qualifying bins, we can't fit the five unknowns
    if (nPos < 5) {
      std::cout << "VertexImage: less than five bins available for fitting centroid" << std::endl;
      if (hxy) delete[] hxy;
      return;
    }

    float avePos = sumPos/nPos;
    double s_x4 = 0;
    double s_y4 = 0;
    double s_x2y2 = 0;
    double s_x3 = 0;
    double s_y3 = 0;
    double s_x2y = 0;
    double s_xy2 = 0;
    double s_x2 = 0;
    double s_y2 = 0;
    double s_xy = 0;
    double s_x = 0;
    double s_y = 0;
    double s_x2q = 0;
    double s_y2q = 0;
    double s_xq = 0;
    double s_yq = 0;
    double s_q = 0;
    int s_n = 0;
    for (int ix = 0; ix < m_nbinsx; ix++) {
      for (int iy = 0; iy < m_nbinsy; iy++) {
	int ibin = ix + iy * m_nbinsx;
	if (hxy[ibin] < avePos) continue;
	float q = log(hxy[ibin]);
	s_q += q;
	s_x += (double) ix;
	s_y += (double) iy;
	s_xq += (double) ix * q;
	s_yq += (double) iy * q;
	s_x2 += (double) ix*ix;
	s_xy += (double) ix*iy;
	s_y2 += (double) iy*iy;
	s_x2q += (double) ix*ix*q;
	s_y2q += (double) iy*iy*q;
	s_x3 += (double) ix*ix*ix;
	s_y3 += (double) iy*iy*iy;
	s_x2y += (double) ix*ix*iy;
	s_xy2 += (double) ix*iy*iy;
	s_x4 += (double) ix*ix*ix*ix;
	s_y4 += (double) iy*iy*iy*iy;
	s_x2y2 += (double) ix*ix*iy*iy;
	s_n++;
      }
    }
    if (hxy) delete[] hxy;
    //AmgMatrix(5,5) m;
    Eigen::Matrix<double,5,5> m;
    m << s_x4, s_x2y2, s_x3, s_x2y, s_x2,
      s_x3, s_xy2, s_x2, s_xy, s_x,
      s_x2y2, s_y4, s_xy2, s_y3, s_y2,
      s_x2y, s_y3, s_xy, s_y2, s_y,
      s_x2, s_y2, s_x, s_y, s_n;
    Eigen::Matrix<double, 5, 1> v;
    v << s_x2q, s_xq, s_y2q, s_yq, s_q;
    Eigen::Matrix<double, 5, 1> sol = m.colPivHouseholderQr().solve(v);

    // transform the solution variables back to the ones we want
    double sigmax2 = -1/(2*sol[0]);
    double sigmay2 = -1/(2*sol[1]);
    double mux = sol[2] * sigmax2;
    double muy = sol[3] * sigmay2;

    if (sigmax2 > 0 && sigmay2 > 0) {
      sigmax = (float) sqrt(sigmax2);
      sigmay = (float) sqrt(sigmay2);
      x = (float) mux;
      y = (float) muy;
      //std::cout << "VertexImage: Successful gaussian fit: " << x << ", " << y << " : " << sigmax << ", " << sigmay << std::endl;
    } else {
      std::cout << "Bad result from gaussian fit" << std::endl;
      std::cout << "VertexImage : " << sol << std::endl;
    }
  }

  // Project on z using Gaussian weighting in x,y
  std::vector<float> VertexImage::projectGaussianOnZ(float centx, float centy, float sigmax, float sigmay) const {
    //create return structure
    std::vector<float> vret(m_nbinsz);

    // Loop over z
    for( int z=0; z<m_nbinsz; ++z ) {
      float totalWeight = 0.0;
      //Do the projection
      for(int x=0; x < m_nbinsx; ++x) {
        for(int y=0; y < m_nbinsy; ++y) {
          int idx = getRMBin(x,y,z);
	  float gauss = exp(-(x-centx)*(x-centx)/(2*sigmax*sigmax))*exp(-(y-centy)*(y-centy)/(2*sigmay*sigmay));
          vret[z] += gauss*m_hist_3d[idx];
	  totalWeight += gauss;
        }
      }
      vret[z] /= totalWeight;
    }

    return vret;
  }

}
