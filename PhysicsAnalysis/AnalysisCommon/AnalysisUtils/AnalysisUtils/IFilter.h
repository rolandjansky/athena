///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IFilter.h 
// Header file for class IFilter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISUTILS_IFILTER_H 
#define ANALYSISUTILS_IFILTER_H 

/** IFilter is the main interface to the filters. It inherits from IFilterCuts
 *  to be able to copy a set of cut properties without explicitly knowing
 *  the concrete implementation of the derived filter.
 */

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// AnalysisUtils includes
#include "AnalysisUtils/IFilterCuts.h"
#include "AnalysisUtils/IFilterUtils.h"

// Forward declaration

template <typename T>
class IFilter : virtual public IFilterCuts
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  IFilter();

  /** Copy constructor: 
   */
  IFilter( const IFilter<T>& rhs );

  // Constructor with parameters: 

  /** Destructor: 
   */
  virtual ~IFilter(); 

  /** Assignment operator: 
   */
  IFilter<T>& operator=(const IFilter<T>& rhs ); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Main filter method. This method takes the decision whether or not
   *  the filter has to accept the element. It returns true or false
   *  accordingly.
   */
  virtual bool isAccepted( const T * element ) const =0;
  bool operator() ( const T * element ) const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
template<typename T>
inline IFilter<T>::IFilter() :
  IFilterCuts()
{}

template<typename T>
inline IFilter<T>::IFilter( const IFilter<T>& rhs ) :
  IFilterCuts(rhs)
{}

template<typename T>
inline IFilter<T>::~IFilter() {}

template<typename T>
inline IFilter<T>& IFilter<T>::operator=( const IFilter<T>& rhs )
{
  if ( this != &rhs ) {
    IFilterCuts::operator=(rhs);
  }
  return *this;
}


template<typename T>
inline bool IFilter<T>::operator()( const T * element ) const
{ 
  return isAccepted(element); 
}

#endif //> ANALYSISUTILS_IFILTER_H
