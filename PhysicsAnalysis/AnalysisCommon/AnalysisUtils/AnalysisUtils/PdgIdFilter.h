///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PdgIdFilter.h 
// Header file for class PdgIdFilter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISUTILS_PDGIDFILTER_H 
#define ANALYSISUTILS_PDGIDFILTER_H 

/** PdgIdFilter selects particles from their pdg id code.
 *  Up to now, it is based on a ParticleCandidateList but it is very likely
 *  that internally it will be based on DecayPattern
 *  See AnalysisExamples/FilterExample.cxx for a concrete example
 */

// STL includes
#include <list>

// HepMC / CLHEP includes

// Gaudi includes
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

// EventKernel includes
#include "EventKernel/PdtPdg.h"

// AnalysisUtils includes
#include "AnalysisUtils/IFilter.h"
#include "AnalysisUtils/ParticleCandidateList.h"

// Forward declaration

template <typename T>
class PdgIdFilter : virtual public IFilter<T>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  PdgIdFilter();

  /** Copy constructor: 
   */
  PdgIdFilter( const PdgIdFilter<T>& rhs );

  // Constructor with parameters: 

  /** Destructor: 
   */
  virtual ~PdgIdFilter(); 

  /** Assignment operator: 
   */
  PdgIdFilter<T>& operator=( const PdgIdFilter<T>& rhs ); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Main filter method. This method takes the decision whether or not
   *  the filter has to accept the element. It returns true or false
   *  accordingly if the particle's id matches one of the required one.
   */
  virtual bool isAccepted( const T * element ) const;

  /** Return the list of pdg ids the filter is looking for
   */
  const ParticleCandidateList * pdgIds() const;

  /** Return true if the filter requires a strict sign matching
   */
  bool matchSign() const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
  void setFilter( const IFilterCuts * filter );

  void setPdgId ( const PDG::pidType& pdgId );
  void setPdgIds( const std::list<PDG::pidType>& pdgIds );
  void setPdgIds( const ParticleCandidateList& pdgIds );

  void setMatchSign( const bool matchSign );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
  
  /** The list of particles ids which might be accepted
   */
  ParticleCandidateList m_pdgIds;

  /** Tells to also match the sign of the pdg id of particles
   *  => true by default
   */
  bool m_matchSign;

}; 

/// I/O operators
//////////////////////
//MsgStream& operator<<( MsgStream & msg, const PdgIdFilter &obj );

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

template<typename T>
inline PdgIdFilter<T>::PdgIdFilter() :
  IFilterCuts(),
  IFilter<T>(),
  m_pdgIds(),
  m_matchSign( true )
{}

template<typename T>
inline PdgIdFilter<T>::PdgIdFilter( const PdgIdFilter<T>& rhs ) :
  IFilterCuts(rhs),
  IFilter<T>(rhs),
  m_pdgIds( rhs.m_pdgIds ),
  m_matchSign( rhs.m_matchSign )
{}

template<typename T>
PdgIdFilter<T>::~PdgIdFilter(){}

template<typename T>
inline
PdgIdFilter<T>& PdgIdFilter<T>::operator=(const PdgIdFilter<T>& rhs)
{
  if ( this != &rhs ) {
    IFilterCuts::operator=(rhs);
    IFilter<T>::operator=(rhs);
    m_pdgIds    = rhs.m_pdgIds;
    m_matchSign = rhs.m_matchSign;
  }
  return *this;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

template<typename T>
bool PdgIdFilter<T>::isAccepted( const T * element ) const
{
  /// Check if a pdg-id set has been set-up
  if ( false == m_pdgIds.empty() ) {
    const PDG::pidType pdgId = element->pdgId();
    //std::cout << "Received PDG: " << pdgId << std::endl;
    //m_pdgIds.dropList();
    if ( m_pdgIds.hasInList( pdgId, m_matchSign ) ) return true;
    else return false;
  }

  /// Otherwise return true by convention
  return true;
}

template<typename T>
inline const ParticleCandidateList * PdgIdFilter<T>::pdgIds() const
{
  return &m_pdgIds;
}

template<typename T>
inline bool PdgIdFilter<T>::matchSign() const
{
  return m_matchSign;
}

template<typename T>
void PdgIdFilter<T>::setFilter( const IFilterCuts * filter )
{
  if ( filter ) {
    const PdgIdFilter<T> * pdgFilter = 
      dynamic_cast<const PdgIdFilter<T> *>(filter );
    
    if ( pdgFilter ) {
      operator=(*pdgFilter);
    } else {
      MsgStream log( Athena::getMessageSvc(), "PdgIdFilter" );
      log << MSG::ERROR
	  << "Can't dynamic_cast " << typeid(filter).name() 
	  << " to a PdgIdFilter"
	  << endreq;
    }
  } //> filter is not a NULL pointer
}

template<typename T>
inline void PdgIdFilter<T>::setPdgId( const PDG::pidType& pdgId )
{
  m_pdgIds.add( pdgId );
}

template<typename T>
void PdgIdFilter<T>::setPdgIds( const std::list<PDG::pidType>& pdgIds )
{
  for ( std::list<PDG::pidType>::const_iterator itr = pdgIds.begin();
	itr != pdgIds.end();
	++itr ) {
    m_pdgIds.add( *itr );
  }
}

template<typename T>
inline void PdgIdFilter<T>::setPdgIds( const ParticleCandidateList& pdgIds )
{
  setPdgIds( pdgIds.list() );
}

template<typename T>
inline void PdgIdFilter<T>::setMatchSign( const bool matchSign )
{
  m_matchSign = matchSign;
}

#endif //> ANALYSISUTILS_PDGIDFILTER_H
