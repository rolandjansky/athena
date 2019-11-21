/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkGaussianSumFilter/IMultiComponentStateAssembler.h"
#include "TrkGaussianSumFilter/IMultiComponentStateMerger.h"
#include "TrkGaussianSumFilter/SortingClasses.h"
#include "CxxUtils/restrict.h"


typedef float* ATH_RESTRICT floatPtrRestrict;

namespace Trk {

class IMultiComponentStateCombiner;
class TrackStateOnSurface;

class QuickCloseComponentsMultiStateMerger
  : public AthAlgTool
  , virtual public IMultiComponentStateMerger
{

public:
  /** Constructor with parameters to be passed to AlgTool */
  QuickCloseComponentsMultiStateMerger(const std::string&, const std::string&, const IInterface*);

  /** Virtual destructor */
  virtual ~QuickCloseComponentsMultiStateMerger();

  /** AlgTool initialise method */
  StatusCode initialize() override final;

  /** AlgTool finalise method */
  StatusCode finalize() override final;

  virtual std::unique_ptr<MultiComponentState> merge(const MultiComponentState&) const override final;

    /** Method for merging components - ownership of objects is passed */
  virtual std::unique_ptr<MultiComponentState> merge(SimpleMultiComponentState&&) const override final;

private:
  Gaudi::Property<unsigned int> m_maximumNumberOfComponents{ this,
                                                             "MaximumNumberOfComponents",
                                                             12,
                                                             "Maximum number of components" };

  ToolHandle<Trk::IMultiComponentStateCombiner> m_stateCombiner{
    this,
    "CombinerTool",
    "Trk::MultiComponentStateCombiner/CloseComponentsCombiner",
    " Combonent combiner"
  };

  ToolHandle<Trk::IMultiComponentStateAssembler> m_stateAssembler{
    this,
    "MultiComponentStateAssembler",
    "Trk::MultiComponentStateAssembler/CloseComponentsStateAssembler",
    " "
  };

  ServiceHandle<IChronoStatSvc> m_chronoSvc; //!< Timing: The Gaudi time auditing service

  std::unique_ptr<MultiComponentState> mergeFullDistArray(IMultiComponentStateAssembler::Cache& cache,
                                                          SimpleMultiComponentState& ) const;


  // Recalculate the distances for a row of pairs and return the index of the minimum pair
  int recalculateDistances(floatPtrRestrict qonpIn,
                           floatPtrRestrict qonpCovIn,
                           floatPtrRestrict qonpGIn,
                           floatPtrRestrict distancesIn,
                           int mini,
                           int n) const;

  // Calculate the distances for all pairs
  void calculateAllDistances(floatPtrRestrict qonpIn,
                             floatPtrRestrict qonpCovIn,
                             floatPtrRestrict qonpGIn,
                             floatPtrRestrict distancesIn,
                             int n) const;

  // Reset the distances for a row
  void resetDistances(floatPtrRestrict distancesIn, const int mini, const int n) const;

  // Find the 2 pairs with the smallest distance
  std::pair<int, int> findMinimumPair(const floatPtrRestrict distancesIn,
                                      const std::vector<const ComponentParameters*>& comp,
                                      const int n) const;

  // Finds the index of the  pair with the smallest distance
  std::pair<int, int> findMinimumIndex(const floatPtrRestrict distancesIn, const int n) const;

  //////////////////////////////////////////////////////////////////////////////////////
};

template<typename T>
class Aligned
{
public:
  Aligned(Aligned const&) = delete;
  Aligned& operator=(Aligned const&) = delete;

  explicit Aligned(size_t n)
    : m_ad(nullptr)
  {
    size_t const size = n * sizeof(T) + alignof(T);
    m_size = n;
    posix_memalign(&m_ad, 32, size);
    m_storage = new (m_ad) T[n];
  };

  ~Aligned()
  {
    for (std::size_t pos = 0; pos < m_size; ++pos) {
      reinterpret_cast<const T*>(m_storage + pos)->~T();
    }

    free(m_storage);
  }

  operator T*() { return m_storage; }

  T& operator[](std::size_t pos) { return *reinterpret_cast<T*>(m_storage + pos); }

private:
  T* m_storage;
  void* m_ad;
  size_t m_size;
}; // class Aligned

}

#endif
