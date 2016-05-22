///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IOverlapFilter.h 
// Header file for class IOverlapFilter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ASSOCIATIONCOMPS_IOVERLAPFILTER_H 
#define ASSOCIATIONCOMPS_IOVERLAPFILTER_H 

// STL includes
#include <functional>

// EventKernel includes
#include "EventKernel/INavigable4Momentum.h"

// Forward declaration

namespace AssociationComps {

class IOverlapFilter : public std::binary_function< 
                                                    const INavigable4Momentum*,
	                                            const INavigable4Momentum*,
	                                            bool 
	                                          >
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~IOverlapFilter(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** @brief Filtering method. 
   *  Return true if objects overlap.
   */ 
  virtual bool hasOverlap( const INavigable4Momentum& obj1,
			   const INavigable4Momentum& obj2 ) const = 0;

  /** @brief Filtering method. 
   *  Return true if objects overlap ( forwards to @c hasOverlap )
   */ 
  bool hasOverlap( const INavigable4Momentum* obj1,
 		   const INavigable4Momentum* obj2 ) const;

  /** @brief Filtering method. 
   *  Return true if objects overlap ( forwards to @c hasOverlap )
   */ 
  bool operator()( const INavigable4Momentum& obj1,
		   const INavigable4Momentum& obj2 ) const;

  /** @brief Filtering method. 
   *  Return true if objects overlap ( forwards to @c hasOverlap )
   */ 
  bool operator()( const INavigable4Momentum* obj1,
		   const INavigable4Momentum* obj2 ) const;

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

inline bool 
IOverlapFilter::hasOverlap( const INavigable4Momentum* obj1,
			    const INavigable4Momentum* obj2 ) const
{
  return hasOverlap(*obj1, *obj2);
}

inline bool IOverlapFilter::operator()( const INavigable4Momentum& obj1,
					const INavigable4Momentum& obj2 ) const
{
  return hasOverlap(obj1, obj2);
}

inline bool IOverlapFilter::operator()( const INavigable4Momentum* obj1,
					const INavigable4Momentum* obj2 ) const
{
  return hasOverlap(*obj1, *obj2);
}

} // namespace AssociationComps

#endif //> ASSOCIATIONCOMPS_IOVERLAPFILTER_H
