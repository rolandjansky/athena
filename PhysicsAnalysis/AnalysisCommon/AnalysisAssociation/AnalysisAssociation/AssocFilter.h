/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// AssocFilter.h 
// Header file for class AssocFilter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISASSOCIATION_ASSOCFILTER_H 
#define ANALYSISASSOCIATION_ASSOCFILTER_H 1

/** \class AssocFilter is a (templated) class to filter a collection of T.
 *  One can construct a filter to associate 2 instances of T based on 
 *  topological requirements.
 *  One can require associations in px, py, pz, energy, eta, phi, mass, pt,
 *  deltaR and cosTheta and specify min and max values for each of these variables.
 *  Hence, the templated class must have the same interface than IParticle !
 */

// STL includes

// HepMC / CLHEP includes

// Gaudi includes
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

// Boost includes
#include <boost/optional.hpp>

// FourMom includes
#include "FourMomUtils/P4Helpers.h"

// AnalysisUtils includes
#include "AnalysisUtils/FilterRange.h"
#include "AnalysisUtils/PhiFilterRange.h"
#include "AnalysisUtils/AnalysisMisc.h"

// AnalysisAssociation
#include "AnalysisAssociation/IAssocFilter.h"

// Forward declaration

template< typename OBJ, typename ASSO>
class AssocFilter : virtual public IAssocFilter<OBJ,ASSO>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  AssocFilter();

  /** Copy constructor: 
   */
  AssocFilter( const AssocFilter& rhs );

  // Constructor with parameters: 

  /** Destructor: 
   */
  virtual ~AssocFilter(); 

  /** Assignment operator: 
   */
  AssocFilter &operator=( const AssocFilter& rhs ); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Main filter method. This method takes the decision whether or not
   *  the filter has to accept the association between both objects.
   *  It returns true or false accordingly.
   */
  virtual bool isAccepted( const OBJ  * object,
			   const ASSO * associated ) const;

  /** Returns the minimum px required by the filter
   */
  double deltaPxMin() const;

  /** Returns the maximum px required by the filter
   */
  double deltaPxMax() const;

  /** Returns the minimum py required by the filter
   */
  double deltaPyMin() const;

  /** Returns the maximum py required by the filter
   */
  double deltaPyMax() const;

  /** Returns the minimum pz required by the filter
   */
  double deltaPzMin() const;

  /** Returns the maximum pz required by the filter
   */
  double deltaPzMax() const;

  /** Returns the minimum energy required by the filter
   */
  double deltaEneMin() const;

  /** Returns the maximum energy required by the filter
   */
  double deltaEneMax() const;

  /** Returns the minimum eta required by the filter
   */
  double deltaEtaMin() const;

  /** Returns the maximum eta required by the filter
   */
  double deltaEtaMax() const;

  /** Returns the minimum phi required by the filter
   */
  double deltaPhiMin() const;

  /** Returns the maximum phi required by the filter
   */
  double deltaPhiMax() const;

  /** Returns the minimum mass required by the filter
   */
  double deltaMassMin() const;

  /** Returns the maximum mass required by the filter
   */
  double deltaMassMax() const;

  /** Returns the minimum pt required by the filter
   */
  double deltaPtMin() const;

  /** Returns the maximum pt required by the filter
   */
  double deltaPtMax() const;

  /** Returns the minimum deltaR required by the filter
   */
  double deltaRMin() const;

  /** Returns the maximum deltaR required by the filter
   */
  double deltaRMax() const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Reset the range cuts for the internal variables (px,py,...)
   *  After calling this method, the filter will accept any association
   */
  virtual void reset();

  /** Copies the IFilterCuts properties of the given object to the current
   *  IFilterCuts object (ie: it copies all the ranges in px,py,... to the
   *  current object)
   */
  virtual void setFilter( const IFilterCuts * ifilter );

  void setDeltaPxMin( const double deltaPxMin );
  void setDeltaPxMax( const double deltaPxMax );
  void setDeltaPx( const double deltaPxMin, const double deltaPxMax );

  void setDeltaPyMin( const double deltaPyMin );
  void setDeltaPyMax( const double deltaPyMax );
  void setDeltaPy( const double deltaPyMin, const double deltaPyMax );

  void setDeltaPzMin( const double deltaPzMin );
  void setDeltaPzMax( const double deltaPzMax );
  void setDeltaPz( const double deltaPzMin, const double deltaPzMax );

  void setDeltaEneMin( const double deltaEneMin );
  void setDeltaEneMax( const double deltaEneMax );
  void setDeltaEne( const double deltaEneMin, const double deltaEneMax );

  void setDeltaEtaMin( const double deltaEtaMin );
  void setDeltaEtaMax( const double deltaEtaMax );
  void setDeltaEta( const double deltaEtaMin, const double deltaEtaMax );

  void setDeltaPhiMin( const double deltaPhiMin );
  void setDeltaPhiMax( const double deltaPhiMax );
  void setDeltaPhi( const double deltaPhiMin, const double deltaPhiMax );

  void setDeltaMassMin( const double massMin );
  void setDeltaMassMax( const double massMax );
  void setDeltaMass( const double deltaMassMin, const double deltaMassMax );

  void setDeltaPtMin( const double deltaPtMin );
  void setDeltaPtMax( const double deltaPtMax );
  void setDeltaPt( const double deltaPtMin, const double deltaPtMax );

  void setDeltaRMin( const double deltaRMin );
  void setDeltaRMax( const double deltaRMax );
  void setDeltaR( const double deltaRMin, const double deltaRMax );

  void setDeltaThetaMin( const double deltaThetaMin );
  void setDeltaThetaMax( const double deltaThetaMax );
  void setDeltaTheta( const double deltaThetaMin, 
		       const double deltaThetaMax );

  void setDeltaCosThetaMin( const double deltaCosThetaMin );
  void setDeltaCosThetaMax( const double deltaCosThetaMax );
  void setDeltaCosTheta( const double deltaCosThetaMin, 
			  const double deltaCosThetaMax );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  // Cut variables

  /** Required range in px (min/max)
   */
  FilterRange m_deltaPx;

  /** Required range in py (min/max)
   */
  FilterRange m_deltaPy;

  /** Required range in pz (min/max)
   */
  FilterRange m_deltaPz;

  /** Required range in energy (min/max)
   */
  FilterRange m_deltaEne;

  /** Required range in eta (min/max)
   */
  FilterRange m_deltaEta;

  /** Required range in phi (min/max)
   */
  PhiFilterRange m_deltaPhi;

  /** Required range in mass (min/max)
   */
  FilterRange m_deltaMass;

  /** Required range in pt (min/max)
   */
  FilterRange m_deltaPt;

  /** Required range in deltaR (min/max)
   */
  FilterRange m_deltaR;

  /** Required range in theta (min/max)
   */
  FilterRange m_deltaTheta;

  /** Required range in cos(theta) (min/max)
   */
  FilterRange m_deltaCosTheta;
  
  /////////////////////////////////////////////////////////////////// 
  // Private methods:
  /////////////////////////////////////////////////////////////////// 
  private :
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Implementation : 
/////////////////////////////////////////////////////////////////// 
template<typename OBJ, typename ASSO>
AssocFilter<OBJ, ASSO>::AssocFilter() :
  IAssocFilter<OBJ,ASSO>(),
  m_deltaPx(),
  m_deltaPy(),
  m_deltaPz(),
  m_deltaEne(),
  m_deltaEta(),
  m_deltaPhi(),
  m_deltaMass(),
  m_deltaPt(),
  m_deltaR(),
  m_deltaTheta(),
  m_deltaCosTheta()
{}

template<typename OBJ, typename ASSO>
AssocFilter<OBJ, ASSO>::AssocFilter(const AssocFilter<OBJ,ASSO>& rhs) :
  IAssocFilter<OBJ,ASSO>(rhs),
  m_deltaPx( rhs.m_deltaPx ),
  m_deltaPy( rhs.m_deltaPy ),
  m_deltaPz( rhs.m_deltaPz ),
  m_deltaEne( rhs.m_deltaEne ),
  m_deltaEta( rhs.m_deltaEta ),
  m_deltaPhi( rhs.m_deltaPhi ),
  m_deltaMass( rhs.m_deltaMass ),
  m_deltaPt( rhs.m_deltaPt ),
  m_deltaR( rhs.m_deltaR ),
  m_deltaTheta( rhs.m_deltaTheta ),
  m_deltaCosTheta( rhs.m_deltaCosTheta )
{}

template<typename OBJ, typename ASSO>
AssocFilter<OBJ,ASSO>& 
AssocFilter<OBJ, ASSO>::operator=( const AssocFilter& rhs )
{
  if ( this != &rhs ) {
    IAssocFilter<OBJ,ASSO>::operator=(rhs);
    m_deltaPx = rhs.m_deltaPx;
    m_deltaPy = rhs.m_deltaPy;
    m_deltaPz = rhs.m_deltaPz;
    m_deltaEne = rhs.m_deltaEne;
    m_deltaEta = rhs.m_deltaEta;
    m_deltaPhi = rhs.m_deltaPhi;
    m_deltaMass = rhs.m_deltaMass;
    m_deltaPt = rhs.m_deltaPt;
    m_deltaR = rhs.m_deltaR;
    m_deltaTheta = rhs.m_deltaTheta;
    m_deltaCosTheta = rhs.m_deltaCosTheta;
  }
  return *this;
}

template<typename OBJ, typename ASSO>
AssocFilter<OBJ, ASSO>::~AssocFilter() {}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////
template<typename OBJ, typename ASSO>
bool AssocFilter<OBJ, ASSO>::isAccepted( const OBJ * object,
					 const ASSO * associated ) const
{
  bool isAccepted = true;

  if ( m_deltaPx.isActive() ) {
    isAccepted = isAccepted &&
      m_deltaPx.isInRange( std::fabs( object->px() - associated->px() ) );
  }

  if ( m_deltaPy.isActive() ) {
    isAccepted = isAccepted &&
      m_deltaPy.isInRange( std::fabs( object->py() - associated->py() ) );
  }

  if ( m_deltaPz.isActive() ) {
    isAccepted = isAccepted &&
      m_deltaPz.isInRange( std::fabs( object->pz() - associated->pz() ) );
  }

  if ( m_deltaEne.isActive() ) {
    isAccepted = isAccepted &&
      m_deltaEne.isInRange( std::fabs( object->e() - associated->e() ) );
  }

  if ( m_deltaEta.isActive() ) {
    isAccepted = isAccepted &&
      m_deltaEta.isInRange( std::fabs(object->eta() - associated->eta()) );
  }

  if ( m_deltaPhi.isActive() ) {
    isAccepted = isAccepted &&
      m_deltaPhi.isInRange ( std::fabs(P4Helpers::deltaPhi(object,associated)) );
  }

  if ( m_deltaMass.isActive() ) {
    isAccepted = isAccepted &&
      m_deltaMass.isInRange( std::fabs( object->m()  - associated->m() ) );
  }

  if ( m_deltaPt.isActive() ) {
    isAccepted = isAccepted &&
      m_deltaPt.isInRange  ( std::fabs( object->pt() - associated->pt() ) );
  }

  if ( m_deltaR.isActive() ) {
    isAccepted = isAccepted && 
      m_deltaR.isInRange( P4Helpers::deltaR(object, associated ) );
  }

  /*
  if ( m_deltaTheta.isActive() ) {
    isAccepted = isAccepted &&
      m_deltaTheta.isInRange( );
  }

  if ( m_deltaCosTheta.isActive() ) {
    isAccepted = isAccepted &&
      m_deltaCosTheta.isInRange( );
  }
  */

  return isAccepted;
}

template<typename OBJ, typename ASSO>
double AssocFilter<OBJ, ASSO>::deltaPxMin() const
{
  return m_deltaPx.lower();
}

template<typename OBJ, typename ASSO>
double AssocFilter<OBJ, ASSO>::deltaPxMax() const
{
  return m_deltaPx.upper();
}

template<typename OBJ, typename ASSO>
double AssocFilter<OBJ, ASSO>::deltaPyMin() const
{
  return m_deltaPy.lower();
}

template<typename OBJ, typename ASSO>
double AssocFilter<OBJ, ASSO>::deltaPyMax() const
{
  return m_deltaPy.upper();
}

template<typename OBJ, typename ASSO>
double AssocFilter<OBJ, ASSO>::deltaPzMin() const
{
  return m_deltaPz.lower();
}

template<typename OBJ, typename ASSO>
double AssocFilter<OBJ, ASSO>::deltaPzMax() const
{
  return m_deltaPz.upper();
}

template<typename OBJ, typename ASSO>
double AssocFilter<OBJ, ASSO>::deltaEneMin() const
{
  return m_deltaEne.lower();
}

template<typename OBJ, typename ASSO>
double AssocFilter<OBJ, ASSO>::deltaEneMax() const
{
  return m_deltaEne.upper();
}

template<typename OBJ, typename ASSO>
double AssocFilter<OBJ, ASSO>::deltaEtaMin() const
{
  return m_deltaEta.lower();
}

template<typename OBJ, typename ASSO>
double AssocFilter<OBJ, ASSO>::deltaEtaMax() const
{
  return m_deltaEta.upper();
}

template<typename OBJ, typename ASSO>
double AssocFilter<OBJ, ASSO>::deltaPhiMin() const
{
  return m_deltaPhi.lower();
}

template<typename OBJ, typename ASSO>
double AssocFilter<OBJ, ASSO>::deltaPhiMax() const
{
  return m_deltaPhi.upper();
}

template<typename OBJ, typename ASSO>
double AssocFilter<OBJ, ASSO>::deltaMassMin() const
{
  return m_deltaMass.lower();
}

template<typename OBJ, typename ASSO>
double AssocFilter<OBJ, ASSO>::deltaMassMax() const
{
  return m_deltaMass.upper();
}

template<typename OBJ, typename ASSO>
double AssocFilter<OBJ, ASSO>::deltaPtMin() const
{
  return m_deltaPt.lower();
}

template<typename OBJ, typename ASSO>
double AssocFilter<OBJ, ASSO>::deltaPtMax() const
{
  return m_deltaPt.upper();
}

template<typename OBJ, typename ASSO>
double AssocFilter<OBJ, ASSO>::deltaRMin() const
{
  return m_deltaR.lower();
}

template<typename OBJ, typename ASSO>
double AssocFilter<OBJ, ASSO>::deltaRMax() const
{
  return m_deltaR.upper();
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 
template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::reset() 
{
  m_deltaR.includeAll();
  m_deltaPx.includeAll();
  m_deltaPy.includeAll();
  m_deltaPz.includeAll();
  m_deltaEne.includeAll();
  m_deltaEta.includeAll();
  m_deltaPhi.includeAll();
  m_deltaMass.includeAll();
  m_deltaPt.includeAll();
  m_deltaTheta.includeAll();
  m_deltaCosTheta.includeAll();
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setFilter( const IFilterCuts * ifilter ) 
{
  if ( ifilter ) {
    const AssocFilter<OBJ,ASSO> * assocFilter = 
      dynamic_cast<const AssocFilter<OBJ,ASSO> *>(ifilter );

    if ( assocFilter ) {
      m_deltaPx   = assocFilter->m_deltaPx;
      m_deltaPy   = assocFilter->m_deltaPy;
      m_deltaPz   = assocFilter->m_deltaPz;
      m_deltaEne  = assocFilter->m_deltaEne;
      m_deltaEta  = assocFilter->m_deltaEta;
      m_deltaPhi  = assocFilter->m_deltaPhi;
      m_deltaMass = assocFilter->m_deltaMass;
      m_deltaPt   = assocFilter->m_deltaPt;
      m_deltaR    = assocFilter->m_deltaR;
      m_deltaTheta    = assocFilter->m_deltaTheta;
      m_deltaCosTheta = assocFilter->m_deltaCosTheta;
    } else {
      MsgStream log( Athena::getMessageSvc(), "AssocFilter" );
      log << MSG::ERROR
	  << "Can't dynamic_cast " << ifilter << " to an AssocFilter"
	  << endreq;
    }
  } //> ifilter is a valid pointer
  return;
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaPxMin( const double deltaPxMin )
{
  m_deltaPx.setMin( deltaPxMin );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaPxMax( const double deltaPxMax )
{
  m_deltaPx.setMax( deltaPxMax );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaPx( const double deltaPxMin,
					  const double deltaPxMax )
{
  m_deltaPx.setRange( deltaPxMin, deltaPxMax );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaPyMin( const double deltaPyMin )
{
  m_deltaPy.setMin( deltaPyMin );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaPyMax( const double deltaPyMax )
{
  m_deltaPy.setMax( deltaPyMax );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaPy( const double deltaPyMin,
					  const double deltaPyMax )
{
  m_deltaPy.setRange( deltaPyMin, deltaPyMax );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaPzMin( const double deltaPzMin )
{
  m_deltaPz.setMin( deltaPzMin );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaPzMax( const double deltaPzMax )
{
  m_deltaPz.setMax( deltaPzMax );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaPz( const double deltaPzMin,
					  const double deltaPzMax )
{
  m_deltaPz.setRange( deltaPzMin, deltaPzMax );
}


template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaEneMin( const double deltaEneMin )
{
  m_deltaEne.setMin( deltaEneMin );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaEneMax( const double deltaEneMax )
{
  m_deltaEne.setMax( deltaEneMax );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaEne( const double deltaEneMin,
					   const double deltaEneMax )
{
  m_deltaEne.setRange( deltaEneMin, deltaEneMax );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaEtaMin( const double deltaEtaMin )
{
  m_deltaEta.setMin(deltaEtaMin);
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaEtaMax( const double deltaEtaMax )
{
  m_deltaEta.setMax(deltaEtaMax);
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaEta( const double deltaEtaMin,
					   const double deltaEtaMax )
{
  m_deltaEta.setRange( deltaEtaMin, deltaEtaMax );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaPhiMin( const double deltaPhiMin )
{
  m_deltaPhi.setMin(deltaPhiMin);
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaPhiMax( const double deltaPhiMax )
{
  m_deltaPhi.setMax(deltaPhiMax);
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaPhi( const double deltaPhiMin,
					   const double deltaPhiMax )
{
  m_deltaPhi.setRange( deltaPhiMin, deltaPhiMax );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaMassMin( const double deltaMassMin )
{
  m_deltaMass.setMin(deltaMassMin);
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaMassMax( const double deltaMassMax )
{
  m_deltaMass.setMax(deltaMassMax);
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaMass( const double deltaMassMin,
					    const double deltaMassMax )
{
  m_deltaMass.setRange( deltaMassMin, deltaMassMax );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaPtMin( const double deltaPtMin )
{
  m_deltaPt.setMin(deltaPtMin);
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaPtMax( const double deltaPtMax )
{
  m_deltaPt.setMax(deltaPtMax);
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaPt( const double deltaPtMin,
					  const double deltaPtMax )
{
  m_deltaPt.setRange( deltaPtMin, deltaPtMax );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaRMin( const double deltaRMin )
{
  m_deltaR.setMin( deltaRMin );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaRMax( const double deltaRMax )
{
  m_deltaR.setMax( deltaRMax );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaR( const double deltaRMin,
					 const double deltaRMax )
{
  m_deltaR.setRange( deltaRMin, deltaRMax );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaThetaMin( const double deltaThetaMin )
{
  m_deltaTheta.setMin( deltaThetaMin );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaThetaMax( const double deltaThetaMax )
{
  m_deltaTheta.setMax( deltaThetaMax );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaTheta( const double deltaThetaMin,
					     const double deltaThetaMax )
{
  m_deltaTheta.setRange( deltaThetaMin, deltaThetaMax );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaCosThetaMin( const double deltaCosThetaMin )
{
  m_deltaCosTheta.setMin( deltaCosThetaMin );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaCosThetaMax( const double deltaCosThetaMax )
{
  m_deltaCosTheta.setMax( deltaCosThetaMax );
}

template<typename OBJ, typename ASSO>
void AssocFilter<OBJ, ASSO>::setDeltaCosTheta( const double deltaCosThetaMin,
						const double deltaCosThetaMax )
{
  m_deltaCosTheta.setRange( deltaCosThetaMin, deltaCosThetaMax );
}


#endif //> ANALYSISASSOCIATION_ASSOCFILTER_H
