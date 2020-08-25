/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONLAYERHOUGH_H
#define MUONLAYERHOUGH_H

#include "MuonStationIndex/MuonStationIndex.h"
#include "MuonLayerHough/Hit.h"
#include "MuonLayerHough/MuonLayerHoughSelector.h"

#include <vector>
#include <iostream>
#include <string>
#include <cmath>

class TH1;

namespace MuonHough {

  /** struct containing all information to build a Hough transform for a given chamber index */
  struct RegionDescriptor {
    RegionDescriptor( int sector_, Muon::MuonStationIndex::DetectorRegionIndex region_, Muon::MuonStationIndex::ChIndex chIndex_,
                      float referencePosition_, float yMinRange_, float yMaxRange_, float yBinSize_, float thetaStep_,
                      unsigned int nthetaSamples_ ) :  sector(sector_),region(region_),chIndex(chIndex_),
                                                       referencePosition(referencePosition_),yMinRange(yMinRange_),yMaxRange(yMaxRange_),
                                                       yBinSize(yBinSize_),thetaStep(thetaStep_),nthetaSamples(nthetaSamples_){}
    RegionDescriptor() :  sector(0),region(Muon::MuonStationIndex::DetectorRegionUnknown),
                          chIndex(Muon::MuonStationIndex::ChUnknown),referencePosition(0),yMinRange(0),yMaxRange(0),
                          yBinSize(1),thetaStep(1),nthetaSamples(1){}

    int                                         sector;
    Muon::MuonStationIndex::DetectorRegionIndex region;
    Muon::MuonStationIndex::ChIndex             chIndex;
    float referencePosition;
    float yMinRange;
    float yMaxRange;
    float yBinSize;
    float thetaStep;
    unsigned int nthetaSamples;
  };
  typedef std::vector<RegionDescriptor> RegionDescriptionVec;

  struct MuonLayerHough {

    /// struct representing the maximum in the hough space 
    struct Maximum {
      Maximum() : max(0.), pos(0.), theta(0.), refpos(0.), refregion(0), refchIndex(0), binpos(-1), binposmin(-1),
                        binposmax(-1), binsize(0.), bintheta(-1), triggerConfirmed(0), hough(0) {}

      float max;    // value of the maximum
      float pos;    // spacial position
      float theta;  // angle

      float refpos; // reference position
      int refregion;  // reference region
      int refchIndex;  // reference chamber index
      
      int binpos;    // position bin
      int binposmin; // lower edge of the maximum
      int binposmax; // upper edge of the maximu 
      float binsize; // size of bin
      int bintheta;  // theta bin
      int triggerConfirmed; // number of in time trigger hits associated with the maximum
      std::vector<Hit*> hits; // vector of associated hits
       
      const MuonLayerHough* hough;  // pointer to the corresponding hough

      bool isEndcap() const{   
        // refers to the chamber orientation!!!! so BEE is a barell in this def
        Muon::MuonStationIndex::DetectorRegionIndex region = hough->m_descriptor.region;
        Muon::MuonStationIndex::ChIndex chIndex = hough->m_descriptor.chIndex;
        //need to make sure BEE's reference plane is the same as barrel
        if (region != Muon::MuonStationIndex::Barrel && chIndex != Muon::MuonStationIndex::BEE) {return true;}
        return false;
      };
      float getGlobalR() const{
        if (isEndcap()){return pos;}
        return refpos;
      };
      float getGlobalZ() const{
        if (isEndcap()){return refpos;}
        return pos;
      };
      float getGlobalTheta() const{
        if (isEndcap()){
          //return M_PI/2.0 - theta;
          if (theta > 0) {return M_PI/2.0 - theta;}
          else {return -M_PI/2.0 - theta;}
        }
        return theta;
      };
    };
  

    /// constructor 
    MuonLayerHough( const RegionDescriptor& descriptor );

    /// destructor 
    ~MuonLayerHough();

    /// reset the transform 
    void reset();

    /// enable debug output 
    void setDebug( bool d ) { m_debug = d; }
    
    /// calculate the position bin the hit will endup in 
    int bin( const Hit& hit ) const;

    /// calculate the bin corresponing to the given x,y position
    int bin( float x, float y ) const;
    
    /// access to y coordinate of a given bin
    float yval( int posBin ) const;

    /// calculate x,y for the given position bin
    void pars( int posBin, int /*thetaBin*/, float& x, float& y ) const;

    /// calculate the highest value of the hough transform within the specified range for the given hit 
    float layerConfirmation( const Hit& hit, float range = 1000. ) const;

    /// calculate the highest value of the hough transform within the specified range for the given hit position
    float layerConfirmation( float x, float y, float range = 1000. ) const;

    /// calculate the highest value of the hough transform within the specified range for the given hit position
    std::pair<float,float> layerConfirmation( unsigned int thetaBin, float x, float y, float range = 1000. ) const;

    /// find the highest maximum that is above maxval
    bool findMaximum( Maximum& maximum, const MuonLayerHoughSelector& selector ) const;

    /// associates the list of input hits to the provided maximum
    void associateHitsToMaximum( Maximum& maximum, const std::vector<Hit*>& hits ) const;

    /// calculates the first and last bin the hit should be filled in for a given theta bin
    std::pair<int,int> range(const float x, const float y1, const float y2, const int bintheta) const;

    /// returns a pair with the position and angle corresponing to the input x,y values
    std::pair<float,float> maximum( float x, float y, int& posbin, int& thetabin ) const;

    /// fill the hough space with a single position
    void fill( const Hit& hit );

    /// fill the hough space with a single position
    void fill( float x, float y, float weight );

    /// fill the hough space with a vector of hits using the layer mode
    void fillLayer( const std::vector<Hit*>& hits, bool substract = false );

    //  fill the hough space with a vector of hits using the layer mode
    void fillLayer2( const std::vector<Hit*>& hits, bool subtract = false );

    /// returns a vector with all the histograms of the hough as TH1*
    std::vector<TH1*> rootHistos(std::string prefix, float* rmin=0,float* rmax=0) const;

    // members

    float m_binsize;      /// binsize 
    float m_invbinsize;   /// inverse binsize

    // unsigned int m_nthetasamples;
    int m_nbins;

    unsigned int max;
    int maxhist;
    int maxbin;
    bool m_debug;
    std::vector< unsigned int* > m_histos;//the maximum contents of all the histograms, overlayed
    RegionDescriptor m_descriptor;
  };


  inline int MuonLayerHough::bin( const Hit& hit ) const {
    return bin(hit.x,hit.ymin);
  }

  inline int MuonLayerHough::bin( float x, float y ) const {
    float yref = y*(m_descriptor.referencePosition-x)/x+y;
    int in = (yref-m_descriptor.yMinRange)/m_binsize;
    if( in < 0 || in >= m_nbins ) in = -1;
    return in;
  }

  inline void MuonLayerHough::pars( int posBin, int /*thetaBin*/, float& x, float& y ) const {
    x = m_descriptor.referencePosition;
    y = yval(posBin);
  }

  inline float MuonLayerHough::yval( int posBin ) const {
    return m_descriptor.yMinRange + posBin*m_binsize;
  }

  inline float MuonLayerHough::layerConfirmation( const Hit& hit, float range ) const {
    return layerConfirmation(hit.x,hit.ymin,range);
  }


  inline void MuonLayerHough:: fill( const Hit& hit ) {
    fill(hit.x,hit.ymin,hit.w);
  }

  //function for linear/parabolic extrapolation from maxima to maxima in a different station
  float extrapolate(const MuonLayerHough::Maximum& ref, const MuonLayerHough::Maximum& ex, bool doparabolic=false);
}
#endif 
