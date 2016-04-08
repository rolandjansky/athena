/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLESINCONETOOLS_IPARTICLELOOKUPTABLE_H
#define PARTICLESINCONETOOLS_IPARTICLELOOKUPTABLE_H

#include <limits>
#include <cmath>
#include "AthContainers/DataVector.h"

namespace xAOD {


  /** 2D look up table for iParticles */
  template<class T>
  class IParticlesLookUpTable {
  public:
    /** constructor taking the desired binsize */
  IParticlesLookUpTable( unsigned int nbins = 50 , float minPt=1e-3) :m_container(0), 
      m_nphiBins(nbins), 
      m_phiBinSize(m_2PI/m_nphiBins),
      m_minPt(minPt),
      m_phiBinnedLookUpTable()
      {} 

    /** initialize the look up table with an iParticle container */
    void init( const DataVector<T>& particles );
      
    /** clear the look up table */
    void clear();

    /** collect IParticles in a given cone */
    template<class O>
    bool iParticlesInCone( float eta, float phi, float dr, O& output ) const;

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

    /// add an entry into a vector of pointers 
    void addEntry( int i, std::vector< const T* >& output ) const;

    /// add an entry into a vector of ElementLinks 
    void addEntry( int i, std::vector< ElementLink<DataVector<T> > >& output ) const;

    
    const DataVector<T>* m_container; //internal container
    int   m_nphiBins;    /// number of bins
    float m_phiBinSize;  /// bin size 
    float m_minPt;       /// cut on minimum Pt for the considered particles  
    std::vector< std::vector< int > > m_phiBinnedLookUpTable; /// the look-up table
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
    m_container = &particles;
    /// loop over iparticles and copy them into the look-up struct
    /// use hashing for phi look-up and sorting for eta look-up
    unsigned int size = particles.size();
    for( unsigned int i=0; i<size;++i ){
      if(particles[i]->pt()>m_minPt){//sanity check (mainly due to Truth)
	int index = phiIndex(particles[i]->phi());
	m_phiBinnedLookUpTable[index].push_back(i);
      }
    }
    //std::cout << " initializing lookup " << m_container->size() << std::endl;
    for( auto& vec : m_phiBinnedLookUpTable ) {
      if( vec.empty() ) continue;
      std::stable_sort(vec.begin(),vec.end(),[&](int i, int j) { return (*m_container)[i]->eta() < (*m_container)[j]->eta(); });
      // std::cout << " new phi slice " << vec.size() << " eta " ;
      // for( auto index : vec ){
      //   std::cout << "  " << (*m_container)[index]->eta();
      // }
      // std::cout << std::endl;
    }
  }

  template<class T> template<class O>
  bool IParticlesLookUpTable<T>::iParticlesInCone( float eta, float phi, float dr, O& output ) const {

    /// check if initialized
    if( m_phiBinnedLookUpTable.empty() ) return false;
  
    /// comparison functions for upper and lower bound
    auto compEta1 = [&](int i,double val ) { return (*m_container)[i]->eta() < val; };
    auto compEta2 = [&](double val,int i ) { return val < (*m_container)[i]->eta(); };

    /// get phi hash ranges
    int indexMin = phiIndex( phiInRange(phi-dr) );
    int indexMax = phiIndex( phiInRange(phi+dr) );
    //std::cout << " eta " << eta << " phi " << phi << " phi ranges " << indexMin << " " << indexMax << std::endl;
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
        const std::vector< int >& tps = m_phiBinnedLookUpTable[indexMin];
        auto it_min = std::lower_bound (tps.begin(),tps.end(),eta-dr,compEta1  );
        auto it_max = std::upper_bound (it_min,tps.end(),eta+dr,compEta2 );
        //std::cout << " new range, entries in bounds " << std::distance(it_max,it_min) << std::endl;
        // add iparticles in cone
        for( ;it_min!=it_max;++it_min ){
          const T* entry = (*m_container)[*it_min];
          float deta = eta- entry->eta();
          float dphi = phiInRange(phi-entry->phi());
          float dr2 = deta*deta + dphi*dphi;
          //std::cout << "   new entry: " << *it_min << " eta,phi " << entry->eta() << " phi " << entry->phi() << " dr " << sqrt(dr2) << std::endl;
          if( dr2 < dr2Cut )  addEntry( *it_min, output );
        }
      }
    }
    return true;
  }

  template<class T>
  void IParticlesLookUpTable<T>::addEntry( int i, std::vector< const T* >& output ) const {
     output.push_back( (*m_container)[i]);
  }

  template<class T>
  void IParticlesLookUpTable<T>::addEntry( int i, std::vector< ElementLink<DataVector<T> > >& output ) const {
    output.push_back( ElementLink<DataVector<T> >(*m_container,i) );
  }

}	// end of namespace

#endif


