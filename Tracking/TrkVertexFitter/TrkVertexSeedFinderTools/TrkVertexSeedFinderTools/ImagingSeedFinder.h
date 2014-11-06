/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERTOOLS_IMAGINGSEEDFINDER_H
#define TRKVERTEXSEEDFINDERTOOLS_IMAGINGSEEDFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"

//External library for fourier transforms
#include "fftw3.h"

//class IBeamCondSvc; //Beam spot constraint from here

namespace Trk
{

  class Vertex;
  class Track;
  class IVertexClusterFinder;
  class VertexCluster;

  // @author Matt Rudolph (matthew.scott.rudolph@cern.ch) Lars Egholm Pedersen (egholm...)
  // This class implements a multiseed finder using a filtering technique inspired by imaging algorithms
  // As a backup, the single findSeed methods are implemented using a cache of the result from findMultiSeed
  // @ATLAS software
  //

  class ImagingSeedFinder : public AthAlgTool, virtual public IVertexSeedFinder
  {
  public:
    StatusCode initialize();
    StatusCode finalize();

    //default constructor due to Athena interface
    ImagingSeedFinder(const std::string& t, const std::string& n, const IInterface*  p);
    
    //destructor
    virtual ~ImagingSeedFinder();

    // Interface for Tracks with starting seed/linearization point
    virtual Vertex findSeed(const std::vector<const Trk::Track*> & vectorTrk,const RecVertex * constraint=0);
    
    /** Interface for MeasuredPerigee with starting point */
    virtual Vertex findSeed(const std::vector<const Trk::TrackParameters*> &parametersList,const RecVertex * constraint=0);

    // Interface for finding vector of seeds from tracks
    virtual std::vector<Vertex> findMultiSeeds(const std::vector<const Trk::Track*>& vectorTrk,const RecVertex * constraint=0);

    // Interface for finding vector of seeds from track parameters
    virtual std::vector<Vertex> findMultiSeeds(const std::vector<const Trk::TrackParameters*>& parametersList,const RecVertex * constraint=0);

  private:

    //Cluster utils
    ToolHandle< Trk::IVertexClusterFinder > m_VertexClusterFinder;

    //Beam condition
    /* ServiceHandle<IBeamCondSvc> m_iBeamCondSvc; //!< pointer to the beam condition service */

    //Chrono service
    ServiceHandle<IChronoStatSvc> m_chronoStatSvc;         


    //bin numbering utility functions
    int getRMBin( const int xbin, const int ybin, const int zbin );
    void getInvRMBin( const int rmbin, int &xbin, int &ybin, int &zbin) ;

    //Method implementing the backprojection into the real space histogram
    void fillHist( std::vector<const Trk::TrackParameters*> parametersList );
    
    //Functions related to histogram backprojection
    /* float findClosestCenter( const float t_curr ); //Find the nearest bin border */
    /*                                                  //Given specified x,y,z and direction */
    /* void incrementBin( const float t_curr );       //Increment bin containing t_curr */
    /*                                                 //Also uses x,y,z and direction pars. */
    /* void updateBinAndCoord( const float t_curr );    */

    // Filtering methods   
    void filterFSHist(); 
    void initFSFilter();

    int    m_xbins;  //Defining parameters for histogram
    int    m_ybins;
    int    m_zbins;
    
    float m_xrange; //Range around beam spot examined
    float m_yrange; 
    float m_zrange;
    
    //Filter parameters
    float m_a0Window;
    float m_a1Window;
    float m_a2Window;
    float m_a3Window;
    
    int    m_cutoffFreqDenominator_xy;
    int    m_cutoffFreqDenominator_z;
    
    float m_x_min, m_x_max; //Histogram ranges
    float m_y_min, m_y_max; //Derived from beamspot
    float m_z_min, m_z_max; //and ranges
    float m_wx;           //Widths of histogram bins
    float m_wy;          
    float m_wz;
    
    int m_binstot; //Total number of bins
    int m_filttot; //Total number of filter bins
    
    float       *m_histRS; //The 'real' space histogram. Used as input and output to filtering algorithm
    fftwf_complex *m_histFS; //The frequency space histogram.
    std::vector<float> m_histFSFilter; //The frequency space filter
    
    //the FFTW "plan" -- object that saves how the fourier transform is done
    fftwf_plan m_plan_r2c;
    fftwf_plan m_plan_c2r;
    
    //where the seeds to return are stored
    std::vector<Vertex> m_seeds;
    //cache control variables
    unsigned int m_cacheRunNumber; ///< cached results for given run/event number
    unsigned int m_cacheEventNumber; ///< cached results for given run/event number
    unsigned int m_currentSeedIdx; ///< keep track of what seeds we've given already

  };
}
#endif
