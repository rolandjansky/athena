/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERUTILS_VERTEXIMAGE_H
#define TRKVERTEXSEEDFINDERUTILS_VERTEXIMAGE_H

#include <vector>

namespace Trk {


  // Class used to access vertex image resulting from track back-projection and filtering in ImagingSeedFinder

  class VertexImage {

  public:

    // Construct with pointer to the array, numbers of bins in x,y,z
    // and flag for whether fourier transforms were done in place.
    // in-place filtering has extra padding in the array to fit the complex frequency space histogram
    VertexImage( float * hist_3d, int nx, int ny, int nz, 
                 float xrange, float yrange, float zrange, bool inplace=true );

    // Default constructor (won't produce useable image), but used to keep image as member setup during init
    VertexImage();

    // Direct access to the histogram array
    const float * getHist() const { return m_hist_3d; }

    // Get the row major index of an x,y,z bin
    int getRMBin( const int & x, const int & y, const int & z ) const;
    // Get the x,y,z values of bin index i
    void getInvRMBin( const int & i, int & x, int & y, int &z ) const;

    // Get the number of bins or array entries
    int getNBinsX() const { return m_nbinsx; }
    int getNBinsY() const { return m_nbinsy; }
    int getNBinsZ() const { return m_nbinsz; }
    int getNBins() const { return m_binstot; }
    int getArraySize() const { return m_arraytot; }

    // Get the actual relative (to histogram center) position
    float getRelPosX(float binx) const { return -m_xrange + binx*m_wx; }
    float getRelPosY(float biny) const { return -m_yrange + biny*m_wy; }
    float getRelPosZ(float binz) const { return -m_zrange + binz*m_wz; } 

    // Get the indices of all adjacent bins in 3D
    std::vector<int> getAdjacentBins( int index ) const;

    // -----------------------------------------------
    // Get rectangular projections onto 1D z-direction
    
    // Project full range
    std::vector<float> projectRectangleOnZ() const;
    // Project specified rectangle from {x,y}min inclusive to {x,y}max exclusive
    std::vector<float> projectRectangleOnZ( int xmin, int xmax, int ymin, int ymax ) const;
    // Project bins within nsigma standard deviations of the mean of the image
    std::vector<float> projectRectangleOnZ( float nsigmax, float nsigmay) const;

    // -----------------------------------------------
    // Fit centroid to 2D Gaussian by projecting onto xy-plane
    // Returned values are in bins
    void fitCentroid(float& x, float& y, float& sigmax, float& sigmay) const;
    // Project on z using a Gaussian window
    // Input parameters are in bins
    std::vector<float> projectGaussianOnZ(float x, float y, float sigmax, float sigmay) const;
    // Project on z using a Gaussian window
    // Parameters of the Gaussian are found by fitting the xy-projection
    std::vector<float> projectGaussianOnZ(float scaleFactor) const;

  private:

    // The actual histogram array
    const float * m_hist_3d;    

    // Store number of bins in x,y,z
    int m_nbinsx;
    int m_nbinsy;
    int m_nbinsz;
    
    // Physical (half) range of the histogram in x,y,z (spans from -range to +range)
    float m_xrange;
    float m_yrange;
    float m_zrange;

    // Keep track of in-place array
    bool m_inplace;

    // Total number of bins in the 3D histogram
    int m_binstot;
    // Total entries in the histogram array (different than binstot for in-place transforms)
    int m_arraytot;

    // Bin widths in the histogram
    float m_wx;
    float m_wy;
    float m_wz;

  };

}

#endif
