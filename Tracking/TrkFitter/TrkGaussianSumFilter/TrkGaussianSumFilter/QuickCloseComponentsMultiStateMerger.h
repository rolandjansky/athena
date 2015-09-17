/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      QuickCloseComponentsMultiStateMerger.h  -  description
      -------------------------------------------------
begin                : Wednesday 3rd September 2008
author               : amorley
email                : Anthony.Morley@cern.ch
decription           : Class for merging components of a multi-state based on 
                       combination of those which are "close" together as 
                       defined by some metric ... but faster
*********************************************************************************/

#ifndef TrkQuickCloseComponentsMultiStateMerger_H
#define TrkQuickCloseComponentsMultiStateMerger_H

#include "TrkGaussianSumFilter/IMultiComponentStateMerger.h"
#include "TrkGaussianSumFilter/SortingClasses.h"


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 
#include "GaudiKernel/IChronoStatSvc.h"

#include <map>

typedef float * __restrict__ floatPtrRestrict ;

namespace Trk{

  class IMultiComponentStateCombiner;
  class IComponentSeparationDistance;
  class IMultiComponentStateAssembler;
  class TrackStateOnSurface;

 
  class QuickCloseComponentsMultiStateMerger : public AthAlgTool, virtual public IMultiComponentStateMerger {

  public:

    /** Constructor with parameters to be passed to AlgTool */
    QuickCloseComponentsMultiStateMerger(const std::string&, const std::string&, const IInterface*);

    /** Virtual destructor */
    virtual ~QuickCloseComponentsMultiStateMerger();

    /** AlgTool initialise method */
    StatusCode initialize();

    /** AlgTool finalise method */
    StatusCode finalize();

    virtual const MultiComponentState* merge(const MultiComponentState&) const;

  private:  
 
    typedef std::multimap<double, ComponentParameters, SortBySmallerWeight> MultiComponentStateMap;

    double calculateDistance(const ComponentParameters , const ComponentParameters) const;


    unsigned int m_maximumNumberOfComponents;

    ToolHandle<IComponentSeparationDistance>  m_distance;

    ToolHandle<Trk::IMultiComponentStateCombiner>   m_stateCombiner;
    ToolHandle<Trk::IMultiComponentStateAssembler>  m_stateAssembler;
  
    ServiceHandle<IChronoStatSvc>                  m_chronoSvc;           //!< Timing: The Gaudi time auditing service

    //////////////////////////////////////////////////////////////////////////////////////
    // New merging functions.

    bool m_useFullDistanceCalcArray;
    bool m_useFullDistanceCalcVector;
    
    //New function to define a reference component, and calculate
    //KL distances with respect to it.
    float getMinimumKLDistanceComponent(const ComponentParameters* &ref,
          std::vector<const ComponentParameters*>&,
          int& minIndex) const;

    const MultiComponentState* mergeFullDistVector(const MultiComponentState&) const;
    const MultiComponentState* mergeFullDistArray(const MultiComponentState&) const;

    void deleteStateComponents(int ind1, int ind2, std::vector<const ComponentParameters*>&) const;

    std::pair<int,int> getPairOfMinimumDistanceComponents(std::vector<const ComponentParameters*>&) const;

    void mergeStateComponents (std::vector<const ComponentParameters*>&) const;
    
    typedef std::vector<std::vector<float> > IndexDistanceMap;
    mutable bool m_useMap;
    mutable IndexDistanceMap m_indexDistanceMap;

    //NEW: Clear storage for indices / distances.
    inline void clearIndexMap() const {
      while (!m_indexDistanceMap.empty()) {
        m_indexDistanceMap.erase(m_indexDistanceMap.begin());
      }      
    }

    //Routine to get rid of all the distance calculations made with
    //merged components.
    void deleteStoredDistances(int i1, int i2) const;

    //Routine to add a new component to the map & calculate distances.
    //NOTE: New components will always come at the end of the vector,
    //which should simplify routine.
    void addComponentDistances(std::vector<const ComponentParameters*>&) const;
    std::pair<int,int> getMinDistanceIndicesFromMap() const;
    
    
    //Recalculate the distances for a row of pairs and return the index of the minimum pair 
    int  recalculateDistances(  floatPtrRestrict qonpIn,  
                                floatPtrRestrict qonpCovIn,  
                                floatPtrRestrict qonpGIn, 
                                floatPtrRestrict distancesIn, 
                                int mini, 
                                int n) const;

    //Calculate the distances for all pairs 
    void calculateAllDistances( floatPtrRestrict qonpIn,  
                                floatPtrRestrict qonpCovIn,  
                                floatPtrRestrict qonpGIn, 
                                floatPtrRestrict distancesIn, 
                                int  n) const;
                                
                                
    //Reset the distances for a row 
    void resetDistances( floatPtrRestrict  distancesIn,const int mini,const int  n) const;


    //Find the 2 pairs with the smallest distance 
    std::pair<int, int> findMinimumPair( const floatPtrRestrict distancesIn,
                                         const std::vector<const ComponentParameters*>& comp, 
                                         const int n) const;

    //Finds the index of the  pair with the smallest distance 
    std::pair<int, int> findMinimumIndex( const floatPtrRestrict distancesIn, 
                         const int n) const;


    
    //////////////////////////////////////////////////////////////////////////////////////
    
  };


 template <typename T>
  class Aligned {
  public:
    Aligned(Aligned const&) = delete;
    Aligned& operator=(Aligned const&) = delete;

    explicit Aligned(size_t n):m_ad(nullptr)
    {
      size_t const size = n*sizeof(T) + alignof(T);
      m_size = n;
      posix_memalign( &m_ad,32, size );
      m_storage  =  new (m_ad) T[n];
    };
  
    ~Aligned(){
      for(std::size_t pos = 0; pos < m_size; ++pos) {
        reinterpret_cast<const T*>(m_storage+pos)->~T();
      }

      free( m_storage );
    }

    operator T*() {return  m_storage;}

    T& operator[](std::size_t pos)
    {
      return *reinterpret_cast<T*>(m_storage+pos);
    }

  private:
    T*     m_storage;
    void*  m_ad;
    size_t m_size;
  }; // class Aligned




}
   
#endif

