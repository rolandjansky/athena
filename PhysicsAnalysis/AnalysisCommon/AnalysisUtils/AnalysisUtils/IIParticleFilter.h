///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IIParticleFilter.h 
// Header file for class IIParticleFilter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISUTILS_IIPARTICLEFILTER_H 
#define ANALYSISUTILS_IIPARTICLEFILTER_H 

/** IIParticleFilter is a templated class which inherits the capabilities
 *  of the Charge- Momentum- and PdgIdFilter classes.
 *  So it can filter every objects which honours the IParticle interface.
 *  An object is accepted if all its properties (charge, pdg ID and momentum)
 *  fulfill the requirements.
 */

// FrameWork includes
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

// AnalysisUtils includes
#include "AnalysisUtils/IFilter.h"
#include "AnalysisUtils/MomentumFilter.h"
#include "AnalysisUtils/PdgIdFilter.h"
#include "AnalysisUtils/ChargeFilter.h"
#include "AnalysisUtils/CategoryIdFilter.h"

template<typename T>
class IIParticleFilter : virtual public MomentumFilter<T>,
                         virtual public PdgIdFilter<T>,
                         virtual public ChargeFilter<T>,
                         virtual public CategoryIdFilter<T>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  IIParticleFilter();

  /** Copy constructor: 
   */
  IIParticleFilter( const IIParticleFilter<T>& rhs );

  // Constructor with parameters: 

  /** Destructor: 
   */
  virtual ~IIParticleFilter(); 

  /** Assignment operator: 
   */
  IIParticleFilter<T>& operator=( const IIParticleFilter<T>& rhs ); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Main filter method. This method takes the decision whether or not
   *  the filter has to accept the element.
   *  See the ChargeFilter, MomentumFilter and PdgIdFilter classes for detailed explanations.
   */
  virtual bool isAccepted( const T * part ) const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Copies the IFilterCuts properties of the given object to the current
   *  IFilterCuts object.
   */
  void setFilter( const IFilterCuts * filter );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
template<typename T>
IIParticleFilter<T>::IIParticleFilter() :
  IFilterCuts(),
  IFilter<T>(),
  MomentumFilter<T>(),
  PdgIdFilter<T>(),
  ChargeFilter<T>(),
  CategoryIdFilter<T>()
{}

template<typename T>
IIParticleFilter<T>::IIParticleFilter( const IIParticleFilter<T>& rhs ) :
  IFilterCuts(),
  IFilter<T>(rhs),
  MomentumFilter<T>(rhs),
  PdgIdFilter<T>(rhs),
  ChargeFilter<T>(rhs),
  CategoryIdFilter<T>(rhs)
{}

/// Destructor
///////////////
template<typename T>
IIParticleFilter<T>::~IIParticleFilter() {}

template<typename T>
IIParticleFilter<T>& 
IIParticleFilter<T>::operator=( const IIParticleFilter<T>& rhs )
{
  if ( this != &rhs ) {
    IFilterCuts::operator=(rhs);
    IFilter<T>::operator=(rhs);
    MomentumFilter<T>::operator=(rhs);
    PdgIdFilter<T>::operator=(rhs);
    ChargeFilter<T>::operator=(rhs);
    CategoryIdFilter<T>::operator=(rhs);
  }
  return *this;
}

template<typename T>
bool IIParticleFilter<T>::isAccepted( const T * part ) const
{
  return MomentumFilter<T>::isAccepted( part ) &&
         PdgIdFilter<T>::isAccepted( part ) &&
         ChargeFilter<T>::isAccepted( part ) &&
         CategoryIdFilter<T>::isAccepted( part );
}

template<typename T>
void IIParticleFilter<T>::setFilter( const IFilterCuts * ifilter )
{
  if ( ifilter ) {
    const IIParticleFilter<T> * filter = dynamic_cast<const IIParticleFilter<T> *>(ifilter);
    if ( filter ) {
      IIParticleFilter<T>::operator=( *filter );
    } else {
      MsgStream log( Athena::getMessageSvc(), "IIParticleFilter" );
      log << MSG::ERROR
	  << "Can't dynamic_cast " << typeid(ifilter).name() 
	  << " to a IIParticleFilter<T>"
	  << endreq;
    }
  }
}

#endif //> ANALYSISUTILS_IIPARTICLEFILTER_H
