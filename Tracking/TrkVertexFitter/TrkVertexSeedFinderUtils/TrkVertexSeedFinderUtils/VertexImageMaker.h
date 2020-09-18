/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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


  class VertexImageMaker : public extends<AthAlgTool, IVertexImageMaker> {
  public:
    VertexImageMaker(const std::string& t, const std::string& n, const IInterface*  p);

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;


    virtual std::unique_ptr<VertexImage>
    makeVertexImage( const std::vector<const Trk::TrackParameters*>& parametersList,
                     const xAOD::Vertex * constraint ) const override;


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

    //Widths of histogram bins
    float m_wx;
    float m_wy;
    float m_wz;

    //Total number of filter bins
    int m_filttot;

    //The frequency space filter
    std::vector<float> m_histFSFilter;

    //the FFTW "plans" -- object that saves how the fourier transform is done
    FFTW_MANGLE_DOUBLE(plan) m_plan_r2c;
    FFTW_MANGLE_DOUBLE(plan) m_plan_c2r;

    //Method implementing the backprojection into the real space histogram
    void fillHist( VertexImage& image,
                   const std::vector<const Trk::TrackParameters*>& parametersList,
                   const xAOD::Vertex * constraint) const;

    // Filtering methods
    void filterFSHist (VertexImage& image) const;
    void initFSFilter();
  };
}
#endif
