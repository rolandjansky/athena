/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHILAYERHOUGH_H
#define MUONPHILAYERHOUGH_H

#include "MuonLayerHough/Hit.h"
#include <vector>
#include <iostream>
#include <string>
#include <cmath>

class TH1;

namespace MuonHough {

  struct MuonPhiLayerHough {

    struct Maximum {
      Maximum() : max(0.), pos(0.), binpos(-1), binposmin(-1), binposmax(-1), sector(-1), hough(nullptr) {}

      float max;
      float pos;

      int binpos;
      int binposmin;
      int binposmax;
      
      int sector;

      std::vector<PhiHit*> hits;
      
      const MuonPhiLayerHough* hough;
    };
  
    MuonPhiLayerHough( int nbins, float rangemin, float rangemax, Muon::MuonStationIndex::DetectorRegionIndex region_ );
    ~MuonPhiLayerHough();

    void reset() const;
    void setDebug( bool d ) { m_debug = d; }


    bool findMaximum( Maximum& maximum, float maxval ) const;
    void associateHitsToMaximum( Maximum& maximum, const std::vector<PhiHit*>& hits ) const;


    std::pair<int,int> range( float /*r*/, float phi1, float phi2 ) const {

      float phimin = std::min( phi1, phi2 );
      float phimax = std::max( phi1, phi2 );
      // if( phimin < m_rangemin ) phimin = m_rangemin;
      // if( phimax > m_rangemax ) phimax = m_rangemax;
      int bphimin = (phimin-m_rangemin)*m_invbinsize;
      int bphimax = (phimax-m_rangemin)*m_invbinsize;
      // add neighbouring bin
      if( (m_rangemin + m_binsize*bphimin - phimin)*m_invbinsize > 0 ) bphimin -= 1;
      if( (m_rangemin + m_binsize*bphimax - phimax)*m_invbinsize < 0 ) bphimax += 1;
      return std::make_pair(bphimin,bphimax);
    }

    void fillLayer( const std::vector<PhiHit*>& hits, bool substract = false ) const;
    void fillLayer2( const std::vector<PhiHit*>& hits, bool substract = false ) const;

    float maximum( float r, float phimin, float phimax, int& posbin ) const {
      unsigned int max = 0;
      posbin = -1;

      std::pair<int,int> minMax = range( r,phimin,phimax );
      for( int n=minMax.first;n<=minMax.second;++n ) {

	if( max < m_histo[n] ) {
	  max = m_histo[n];
	  posbin = n;
	}
      }
      if( max > 100000 ){
	std::cout << " maximum too large " << max*0.001 << " min " << minMax.first << " max " << minMax.second << " nbins " << m_nbins
		  << " phimin " << phimin << " phimax " << phimax << std::endl;
	for( int n=minMax.first;n<=minMax.second;++n ) {
	  std::cout << " bin " << n << " val " <<  m_histo[n] << std::endl;
	}
      }

      return 0.001*max;
    }


    std::vector<TH1*> rootHistos(const std::string& prefix, const float* phimin=0,const float* phimax=0) const;

    float m_binsize; 
    float m_invbinsize; 
    float m_rangemin;
    float m_rangemax; 
    
    Muon::MuonStationIndex::DetectorRegionIndex m_region;
    int  m_nbins;
    bool m_debug;
    unsigned int* m_histo;

    private:
      // fake copy constructor and assignment operator
      MuonPhiLayerHough(const MuonPhiLayerHough&);
      MuonPhiLayerHough & operator=(const MuonPhiLayerHough &right);

  };

}
#endif 
