///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DeltaROverlapFilter.h 
// Header file for class DeltaROverlapFilter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ASSOCIATIONCOMPS_DELTAROVERLAPFILTER_H 
#define ASSOCIATIONCOMPS_DELTAROVERLAPFILTER_H 

// STL includes
#include <functional>

// EventKernel includes
#include "EventKernel/INavigable4Momentum.h"

// AssociationUtils includes
#include "IOverlapFilter.h"

// Forward declaration

namespace AssociationComps {

class DeltaROverlapFilter : public IOverlapFilter
{ 
 public:
  using IOverlapFilter::hasOverlap;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  DeltaROverlapFilter();

  /** Copy constructor: 
   */
  DeltaROverlapFilter( const DeltaROverlapFilter& rhs );

  /** Assignment operator: 
   */
  DeltaROverlapFilter& operator=( const DeltaROverlapFilter& rhs ); 

  /** Constructor with parameters: 
   */
  DeltaROverlapFilter( double maxDeltaR );
  
  /** Constructor with parameters: 
   */
  DeltaROverlapFilter( double minDeltaR, double maxDeltaR );
  
  /** Destructor: 
   */
  virtual ~DeltaROverlapFilter(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** @brief Filtering method. If true then objects overlap.
   */ 
  bool hasOverlap( const INavigable4Momentum& obj1,
		   const INavigable4Momentum& obj2 ) const;

  /** @brief Retrieve the defined lowest value cut for the $\Delta R$
   */
  double minDeltaR() const;

  /** @brief Retrieve the defined highest value cut for the $\Delta R$
   */
  double maxDeltaR() const;


  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  void setMinDeltaR( double minDeltaR );

  void setMaxDeltaR( double maxDeltaR );

  void setDeltaR( double minDeltaR, double maxDeltaR );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Inner crown limit for delta-R check
   */
  double m_minDeltaR;

  /** Outer crown limit for delta-R check
   */
  double m_maxDeltaR;
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline DeltaROverlapFilter::~DeltaROverlapFilter()
{}

inline double DeltaROverlapFilter::minDeltaR() const
{
  return m_minDeltaR;
}

inline double DeltaROverlapFilter::maxDeltaR() const
{
  return m_maxDeltaR;
}

inline void DeltaROverlapFilter::setMinDeltaR( double minDeltaR )
{
  m_minDeltaR = minDeltaR;
}

inline void DeltaROverlapFilter::setMaxDeltaR( double maxDeltaR )
{
  m_maxDeltaR = maxDeltaR;
}

inline 
void DeltaROverlapFilter::setDeltaR( double minDeltaR, double maxDeltaR )
{
  m_minDeltaR = minDeltaR;
  m_maxDeltaR = maxDeltaR;
}

} // namespace AssociationComps

#endif //> ASSOCIATIONCOMPS_DELTAROVERLAPFILTER_H
