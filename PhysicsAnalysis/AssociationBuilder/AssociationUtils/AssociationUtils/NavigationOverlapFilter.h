/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// NavigationOverlapFilter.h 
// Header file for class NavigationOverlapFilter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ASSOCIATIONUTILS_NAVIGATIONOVERLAPFILTER_H 
#define ASSOCIATIONUTILS_NAVIGATIONOVERLAPFILTER_H 

// STL includes
#include <functional>

// EventKernel includes
#include "EventKernel/INavigable4Momentum.h"

// AssociationUtils includes
#include "AssociationUtils/IOverlapFilter.h"

// Forward declaration

class NavigationOverlapFilter : public IOverlapFilter
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  NavigationOverlapFilter();

  /** Copy constructor: 
   */
  NavigationOverlapFilter( const NavigationOverlapFilter& rhs );

  /** Assignment operator: 
   */
  NavigationOverlapFilter& operator=( const NavigationOverlapFilter& rhs ); 

  /** Destructor: 
   */
  virtual ~NavigationOverlapFilter(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** @brief Filtering method. If true then objects overlap.
   */ 
  bool hasOverlap( const INavigable4Momentum& obj1,
		   const INavigable4Momentum& obj2 ) const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline NavigationOverlapFilter::~NavigationOverlapFilter()
{}

#endif //> ASSOCIATIONUTILS_NAVIGATIONOVERLAPFILTER_H
