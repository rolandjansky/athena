/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLESINCONETOOLS_IPARTICLELOOKUPTABLE_H
#define PARTICLESINCONETOOLS_IPARTICLELOOKUPTABLE_H

#include <math.h>
#include "AthContainers/DataVector.h"

namespace xAOD {


  /** 2D look up table for iParticles */
  template<class T>
  class IParticlesLookUpTable {
  public:
    /** constructor taking the desired binsize */
    IParticlesLookUpTable( unsigned int nbins = 50 ) : m_nphiBins(nbins), m_phiBinSize(m_2PI/m_nphiBins) {} 

    /** initialize the look up table with an iParticle container */
    void init( const DataVector<T>& particles );
      
    /** clear the look up table */
    void clear();

    /** collect IParticles in a given cone */
    bool iParticlesInCone( float eta, float phi, float dr, std::vector< const T* >& output ) const;

    /** return whether table is initialized */
    bool isInitialized() const;

  private:
    /// define 2*Pi
    static constexpr float m_2PI = 2*M_PI;

    /// hepler function to ensure phi is within +-Pi
    float phiInRange(float phi) const { 
      while (phi >= M_PI) phi -= m_2PI;
      while (phi < -M_PI) phi += m_2PI;
      return phi;
    }

    /// calculate phi index for a given phi
    int phiIndex(float phi) const { return (phi + M_PI)/m_phiBinSize; };

    int   m_nphiBins;    /// number of bins
    float m_phiBinSize;  /// bin size 
    std::vector< std::vector< const T* > > m_phiBinnedLookUpTable; /// the look-up table
  };

  template<class T>
  bool IParticlesLookUpTable<T>::isInitialized() const { return !m_phiBinnedLookUpTable.empty(); }


  template<class T>
  void IParticlesLookUpTable<T>::clear() {
    m_phiBinnedLookUpTable.clear();
  }

  template<class T>
  void IParticlesLookUpTable<T>::init( const DataVector<T>& particles ) {
    /// resize hash table
    m_phiBinnedLookUpTable.resize(m_nphiBins);
    /// loop over iparticles and copy them into the look-up struct
    /// use hashing for phi look-up and sorting for eta look-up
    unsigned int size = particles.size();
    for( unsigned int i=0; i<size;++i ){
      int index = phiIndex(particles[i]->phi());
      m_phiBinnedLookUpTable[index].push_back(particles[i]);
    }
    for( auto& vec : m_phiBinnedLookUpTable ) {
      std::stable_sort(vec.begin(),vec.end(),[](const T* tp1,const T* tp2 ) { return tp1->eta() < tp2->eta(); });
    }
  }

  template<class T>
  bool IParticlesLookUpTable<T>::iParticlesInCone( float eta, float phi, float dr, std::vector< const T* >& output ) const {

    /// check if initialized
    if( m_phiBinnedLookUpTable.empty() ) return false;
  
    /// comparison functions for upper and lower bound
    auto compEta1 = [](const T* tp,double val ) { return tp->eta() < val; };
    auto compEta2 = [](double val,const T* tp ) { return val < tp->eta(); };

    /// get phi hash ranges
    int indexMin = phiIndex( phiInRange(phi-dr) );
    int indexMax = phiIndex( phiInRange(phi+dr) );
    
    // special treatment for boundary region
    std::vector< std::pair<int,int> > ranges;
    if( indexMin > indexMax ) {
      ranges.push_back( std::make_pair(0,indexMax) );
      ranges.push_back( std::make_pair(indexMin,m_nphiBins-1) );      
    }else{
      ranges.push_back( std::make_pair(indexMin,indexMax) );
    }
    
    float dr2Cut = dr*dr;

    // loop over ranges 
    for( auto& range : ranges ){
      indexMin = range.first;
      indexMax = range.second;
      for( ; indexMin <= indexMax; ++indexMin ){
        // get iterators for iparticles to be included 
        const std::vector< const T* >& tps = m_phiBinnedLookUpTable[indexMin];
        auto it_min = std::lower_bound (tps.begin(),tps.end(),eta-dr,compEta1  );
        auto it_max = std::upper_bound (it_min,tps.end(),eta+dr,compEta2 );
        // add iparticles in cone
        for( ;it_min!=it_max;++it_min ){
          float deta = eta-(*it_min)->eta();
          float dphi = phiInRange(phi-(*it_min)->phi());
          float dr2 = deta*deta + dphi*dphi;
          if( dr2 < dr2Cut ) {
            output.push_back(*it_min);
          }
        }
      }
    }
    return true;
  }


}	// end of namespace

#endif


