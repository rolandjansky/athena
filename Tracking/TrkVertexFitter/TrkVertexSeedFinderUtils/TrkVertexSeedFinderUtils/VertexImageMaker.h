/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERUTILIS_SIMPLEVERTEXCLUSTERFINDER_H
#define TRKVERTEXSEEDFINDERUTILIS_SIMPLEVERTEXCLUSTERFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkVertexSeedFinderUtils/IVertexImageMaker.h"
#include "TrkVertexSeedFinderUtils/VertexImage.h"
#include "xAODTracking/Vertex.h"

//External library for fourier transforms
#include "fftw3.h"

namespace Trk
{

  /**
   @class VertexImageMaker

   Implementation of vertex imaging from tracks using fast fourier transform with FFTW3 and filtering

   @author Matt Rudolph <matthew.scott.rudolph@cern.ch

   ------------------------------------
   Changes:

   David Shope <david.richard.shope@cern.ch> (2016-04-19)

   EDM Migration to xAOD - from Trk::RecVertex to xAOD::Vertex

   */


  class VertexImageMaker : public AthAlgTool, IVertexImageMaker {
  public:
    StatusCode initialize();
    StatusCode finalize();

    VertexImageMaker(const std::string& t, const std::string& n, const IInterface*  p);

    virtual ~VertexImageMaker() {}

    virtual VertexImage makeVertexImage( const std::vector<const Trk::TrackParameters*>& parametersList,const xAOD::Vertex * constraint );

  private:
    //------------
    //Properties:

    //Number of bins in histogram:
    int    m_xbins;
    int    m_ybins;
    int    m_zbins;

    float m_xrange; //Range around histogram center in position space
    float m_yrange; 
    float m_zrange;
    
    //Filter parameters
    float m_a0Window;
    float m_a1Window;
    float m_a2Window;
    float m_a3Window;
    
    int    m_cutoffFreqDenominator_xy;
    int    m_cutoffFreqDenominator_z;

    float  m_angularCutoffParameter;

    //Other members:

    //The one copy of the image -- used for each event to avoid reallocation and passed back to caller
    VertexImage m_image;

    //Histogram ranges
    float m_x_min, m_x_max;
    float m_y_min, m_y_max; 
    float m_z_min, m_z_max; 

    //Widths of histogram bins
    float m_wx;        
    float m_wy;          
    float m_wz;

    //Total number of filter bins 
    int m_filttot;

    //Pointers to the actual histogram data arrays; for in-place transforms which is usaul they point to the same
    // memory location, but would be different for out-of-place transforms
    //The 'real' space histogram. Used as input and output to filtering algorithm
    float       *m_histRS;
    //The frequency space histogram.
    fftwf_complex *m_histFS;

    //The frequency space filter
    std::vector<float> m_histFSFilter;

    //the FFTW "plans" -- object that saves how the fourier transform is done
    fftwf_plan m_plan_r2c;
    fftwf_plan m_plan_c2r;

    //Method implementing the backprojection into the real space histogram
    void fillHist( std::vector<const Trk::TrackParameters*> parametersList );

    // Filtering methods   
    void filterFSHist(); 
    void initFSFilter();
  };
}
#endif
