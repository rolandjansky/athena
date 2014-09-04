///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ChargeFilter.h 
// Header file for class ChargeFilter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISUTILS_CHARGEFILTER_H 
#define ANALYSISUTILS_CHARGEFILTER_H 

/** ChargeFilter can filter objects upon their charge.
 *  One can only require a charge (not a range in charges).
 *  But it could be extended to a range.
 */

// FrameWork includes
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

// Boost includes
#include <boost/optional.hpp>

// EventKernel includes
#include "EventKernel/IParticle.h"

// AnalysisUtils includes
#include "AnalysisUtils/IFilter.h"

template <typename T>
class ChargeFilter : virtual public IFilter<T>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  ChargeFilter();

  /** Copy constructor: 
   */
  ChargeFilter( const ChargeFilter<T>& rhs );

  // Constructor with parameters: 

  /** Destructor: 
   */
  virtual ~ChargeFilter(); 

  /** Assignment operator: 
   */
  ChargeFilter<T>& operator=( const ChargeFilter<T>& rhs ); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Main filter method. This method takes the decision whether or not
   *  the filter has to accept the element. It returns true or false
   *  accordingly if the charge of the particle matchs the one of the filter, it is accepted.
   *  If no cut has been defined, it returns true by default.
   */
  virtual bool isAccepted( const T * element ) const;

  /** Returns the charge the filter is looking for
   */
  double charge() const;
  
  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Copies the IFilterCuts properties of the given object to the current
   *  IFilterCuts object (ie: it copies the required charge() to the
   *  current object)
   */
  void setFilter( const IFilterCuts * filter );

  /** Set the filter to look for the given charge
   *  \param charge : the charge the filter will be looking for.
   */
  void setCharge( const ChargeType& charge );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** The charge the filter is looking for. It is unitialized by default (in
   *  the boost::optional sense)
   */
  boost::optional<ChargeType> m_charge;

}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Implementation
/////////////////////////////////////////////////////////////////// 
template<typename T>
ChargeFilter<T>::ChargeFilter() :
  IFilterCuts(),
  IFilter<T>(),
  m_charge()
{}

template<typename T>
ChargeFilter<T>::ChargeFilter( const ChargeFilter<T>& rhs ) :
  IFilterCuts(rhs),
  IFilter<T>(rhs),
  m_charge(rhs.m_charge)
{}

template<typename T>
inline 
ChargeFilter<T>& ChargeFilter<T>::operator=( const ChargeFilter<T>& rhs ) 
{
  if ( this != &rhs ) {
    IFilterCuts::operator=(rhs);
    IFilter<T>::operator=(rhs);
    m_charge = rhs.m_charge;
  }
  return *this;
}


template<typename T>
ChargeFilter<T>::~ChargeFilter(){}

template<typename T>
bool ChargeFilter<T>::isAccepted( const T * element ) const
{
  /// Check if a charge cut has been set-up
  if ( !m_charge ) {
    /// The charge cut is un-initialized so by convention
    /// we return true;
    return true;
  } else {
    const ChargeType charge = element->charge();
    if ( *m_charge == charge ) { 
      return true;
    } else { 
      return false; 
    }
  }

  /// Otherwise return true by convention
  return true;
}

template<typename T>
double ChargeFilter<T>::charge() const
{
  if (m_charge) {
    return *m_charge;
  }
  throw std::domain_error( "No charge selection defined" );
}

template<typename T>
void ChargeFilter<T>::setFilter( const IFilterCuts * filter )
{
  if ( filter ) {
    const ChargeFilter<T> * chFilter =
      dynamic_cast<const ChargeFilter<T> *>(filter);
    if ( chFilter ) {
      operator=(*chFilter);
    } else {
      MsgStream log( Athena::getMessageSvc(), "ChargeFilter" );
      log << MSG::ERROR
	  << "Can't dynamic_cast " << typeid(filter).name() 
	  << " to a ChargeFilter"
	  << endreq;
    }
  }
}

template<typename T>
void ChargeFilter<T>::setCharge( const ChargeType& charge )
{
  m_charge = charge;
}

#endif //> ANALYSISUTILS_CHARGEFILTER_H
