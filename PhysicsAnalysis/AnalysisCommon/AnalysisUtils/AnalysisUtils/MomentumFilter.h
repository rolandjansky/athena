///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MomentumFilter.h 
// Header file for class MomentumFilter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISUTILS_MOMENTUMFILTER_H 
#define ANALYSISUTILS_MOMENTUMFILTER_H 

/** MomentumFilter can filter objects upon their four-momentum properties.
 *  It can select particles based on their px,py,pz and energy as well
 *  as their mass, pt, eta or phi.
 *  One can requires minimum or maximum cuts (or both).
 */

#include <typeinfo>

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "AnalysisUtils/IFilter.h"
#include "AnalysisUtils/FilterRange.h"
#include "AnalysisUtils/PhiFilterRange.h"

template <typename T>
class MomentumFilter : virtual public IFilter<T>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  MomentumFilter();

  /** Copy constructor: 
   */
  MomentumFilter( const MomentumFilter<T>& filter );

  // Constructor with parameters: 

  /** Destructor: 
   */
  virtual ~MomentumFilter(); 

  /** Assignment operator: 
   */
  MomentumFilter<T>& operator=(const MomentumFilter<T>& rhs); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Main filter method. This method takes the decision whether or not
   *  the filter has to accept the element. It returns true or false
   *  accordingly if all the thresholds defined by the set_xyzMin or set_xyzMax
   *  are met. If no threshold has been defined, it returns true by default.
   *  <b>Ex</b>: a particle will be accepted if its px is as : pxMin() <= px <= pxMax()
   *
   *  <b>Warning</b> : it will not complain if you require a non-consistent 
   *  set of cuts ( eg: ptMin < 10.*GeV and ptMax < 5.*GeV )
   */
  virtual bool isAccepted( const T * element ) const;

  /** Returns the minimum px required by the filter
   */
  double pxMin() const;

  /** Returns the maximum px required by the filter
   */
  double pxMax() const;

  /** Returns the minimum py required by the filter
   */
  double pyMin() const;

  /** Returns the maximum py required by the filter
   */
  double pyMax() const;

  /** Returns the minimum pz required by the filter
   */
  double pzMin() const;

  /** Returns the maximum pz required by the filter
   */
  double pzMax() const;

  /** Returns the minimum energy required by the filter
   */
  double eneMin() const;

  /** Returns the maximum energy required by the filter
   */
  double eneMax() const;

  /** Returns the minimum eta required by the filter
   */
  double etaMin() const;

  /** Returns the maximum eta required by the filter
   */
  double etaMax() const;

  /** Returns the minimum phi required by the filter
   */
  double phiMin() const;

  /** Returns the maximum phi required by the filter
   */
  double phiMax() const;

  /** Returns the minimum mass required by the filter
   */
  double massMin() const;

  /** Returns the maximum mass required by the filter
   */
  double massMax() const;

  /** Returns the minimum pt required by the filter
   */
  double ptMin() const;

  /** Returns the maximum pt required by the filter
   */
  double ptMax() const;

  /** Convert a phi angle in the ATLAS conventional range -PI->PI
   *  inspired from FourMom package (could also use CaloPhiRange)
   */
  double atlasPhi( const double phi ) const;
  
  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Copies the IFilterCuts properties of the given object to the current
   *  IFilterCuts object (ie: it copies all the ranges in px,py,... to the
   *  current object)
   */
  void setFilter( const IFilterCuts * filter );

  /** Sets the minimum px required by the filter
   */
  void setPxMin( const double pxMin );

  /** Sets the maximum px required by the filter
   */
  void setPxMax( const double pxMax );

  /** Sets the minimum py required by the filter
   */
  void setPyMin( const double pyMin );

  /** Sets the maximum py required by the filter
   */
  void setPyMax( const double pyMax );

  /** Sets the minimum pz required by the filter
   */
  void setPzMin( const double pzMin );

  /** Sets the maximum pz required by the filter
   */
  void setPzMax( const double pzMax );

  /** Sets the minimum energy required by the filter
   */
  void setEneMin( const double eneMin );

  /** Sets the maximum energy required by the filter
   */
  void setEneMax( const double eneMax );

  /** Sets the minimum eta required by the filter
   */
  void setEtaMin( const double etaMin );

  /** Sets the maximum eta required by the filter
   */
  void setEtaMax( const double etaMax );

  /** Sets the minimum phi required by the filter
   */
  void setPhiMin( const double phiMin );

  /** Sets the maximum phi required by the filter
   */
  void setPhiMax( const double phiMax );

  /** Sets the minimum mass required by the filter
   */
  void setMassMin( const double massMin );

  /** Sets the maximum mass required by the filter
   */
  void setMassMax( const double massMax );

  /** Sets the minimum pt required by the filter
   */
  void setPtMin( const double ptMin );

  /** Sets the maximum pt required by the filter
   */
  void setPtMax( const double ptMax );

  /** Set range for a given momentum component (by name)
   */
  void setRange( const std::string& name,
		 const double min, const double max );

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
  
  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
  
  /** The range in px required by the filter.
   */
  FilterRange m_pxRange;
  
  /** The range in py required by the filter.
   */
  FilterRange m_pyRange;
  
  /** The range in pz required by the filter.
   */
  FilterRange m_pzRange;
  
  /** The range in energy required by the filter.
   */
  FilterRange m_eneRange;
  
  /** The range in eta required by the filter.
   */
  FilterRange m_etaRange;
  
  /** The range in phi required by the filter.
   */
  PhiFilterRange m_phiRange;
  
  /** The range in mass required by the filter.
   */
  FilterRange m_massRange;
  
  /** The range in pt required by the filter.
   */
  FilterRange m_ptRange;

  private :

}; 

// I/O operators
//////////////////////
//MsgStream& operator<<( MsgStream & msg, const MomentumFilter &obj );

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

template<typename T>
inline MomentumFilter<T>::MomentumFilter() :
  IFilterCuts(),
  IFilter<T>(),
  m_pxRange(),
  m_pyRange(),
  m_pzRange(),
  m_eneRange(),
  m_etaRange(),
  m_phiRange(),
  m_massRange(),
  m_ptRange()
{}

template<typename T>
inline MomentumFilter<T>::MomentumFilter( const MomentumFilter<T>& rhs ) :
  IFilterCuts(rhs),
  IFilter<T>( rhs ),
  m_pxRange( rhs.m_pxRange ),
  m_pyRange( rhs.m_pyRange ),
  m_pzRange( rhs.m_pzRange ),
  m_eneRange(rhs.m_eneRange ),
  m_etaRange(rhs.m_etaRange ),
  m_phiRange(rhs.m_phiRange ),
  m_massRange(rhs.m_massRange ),
  m_ptRange( rhs.m_ptRange )
{}

template<typename T>
MomentumFilter<T>::~MomentumFilter() {}

template<typename T>
inline
MomentumFilter<T>& MomentumFilter<T>::operator=(const MomentumFilter<T>& rhs)
{
  if ( this != &rhs ) {
    IFilterCuts::operator=(rhs);
    IFilter<T>::operator=(rhs);
    m_pxRange   = rhs.m_pxRange;
    m_pyRange   = rhs.m_pyRange;
    m_pzRange   = rhs.m_pzRange;
    m_eneRange  = rhs.m_eneRange;
    m_etaRange  = rhs.m_etaRange;
    m_phiRange  = rhs.m_phiRange;
    m_massRange = rhs.m_massRange;
    m_ptRange   = rhs.m_ptRange;
  }
  return *this;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

template<typename T>
bool MomentumFilter<T>::isAccepted( const T * element ) const
{
  bool isAccepted = true;
  
  if ( m_pxRange.isActive() ) {
    isAccepted = isAccepted && m_pxRange.isInRange  ( element->px()  );
  }
  if ( m_pyRange.isActive() ) {
    isAccepted = isAccepted && m_pyRange.isInRange  ( element->py()  );
  }
  if ( m_pzRange.isActive() ) {
    isAccepted = isAccepted && m_pzRange.isInRange  ( element->pz()  );
  }
  if ( m_eneRange.isActive() ) {
    isAccepted = isAccepted && m_eneRange.isInRange ( element->e()   );
  }
  if ( m_etaRange.isActive() ) {
    isAccepted = isAccepted && m_etaRange.isInRange ( element->eta() );
  }
  if ( m_phiRange.isActive() ) {
    isAccepted = isAccepted && m_phiRange.isInRange( element->phi() );
  }
  if ( m_massRange.isActive() ) {
    isAccepted = isAccepted && m_massRange.isInRange( element->m()   );
  }
  if ( m_ptRange.isActive() ) {
    isAccepted = isAccepted && m_ptRange.isInRange  ( element->pt()  );
  }

  return isAccepted;
}

template<typename T>
inline double MomentumFilter<T>::pxMin() const
{
  return m_pxRange.lower();
}

template<typename T>
inline double MomentumFilter<T>::pxMax() const
{
  return m_pxRange.upper();
}

template<typename T>
inline double MomentumFilter<T>::pyMin() const
{
  return m_pyRange.lower();
}

template<typename T>
inline double MomentumFilter<T>::pyMax() const
{
  return m_pyRange.upper();
}

template<typename T>
inline double MomentumFilter<T>::pzMin() const
{
  return m_pzRange.lower();
}

template<typename T>
inline double MomentumFilter<T>::pzMax() const
{
  return m_pzRange.upper();
}

template<typename T>
inline double MomentumFilter<T>::eneMin() const
{
  return m_eneRange.lower();
}

template<typename T>
inline double MomentumFilter<T>::eneMax() const
{
  return m_eneRange.upper();
}

template<typename T>
inline double MomentumFilter<T>::etaMin() const
{
  return m_etaRange.lower();
}

template<typename T>
inline double MomentumFilter<T>::etaMax() const
{
  return m_etaRange.upper();
}

template<typename T>
inline double MomentumFilter<T>::phiMin() const
{
  return m_phiRange.lower();
}

template<typename T>
inline double MomentumFilter<T>::phiMax() const
{
  return m_phiRange.upper();
}

template<typename T>
inline double MomentumFilter<T>::massMin() const
{
  return m_massRange.lower();
}

template<typename T>
inline double MomentumFilter<T>::massMax() const
{
  return m_massRange.upper();
}

template<typename T>
inline double MomentumFilter<T>::ptMin() const
{
  return m_ptRange.lower();
}

template<typename T>
inline double MomentumFilter<T>::ptMax() const
{
  return m_ptRange.upper();
}

template<typename T>
inline double MomentumFilter<T>::atlasPhi( const double phi ) const
{
  return m_phiRange.atlasPhi( phi );
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 
template<typename T>
void MomentumFilter<T>::setFilter( const IFilterCuts * filter )
{
  if ( filter ) {
    try {
      const MomentumFilter<T> * momFilter = 
	dynamic_cast<const MomentumFilter<T> *>(filter);

      if ( momFilter ) {
	operator=(*momFilter);
      } else {
	MsgStream log( Athena::getMessageSvc(), "MomentumFilter" );
	log << MSG::ERROR
	    << "Can't dynamic_cast " << typeid(filter).name() 
	    << " to a MomentumFilter"
	    << endreq;
      }
    } catch (...) {
      MsgStream log( Athena::getMessageSvc(), "MomentumFilter" );
      log << MSG::ERROR
	  << "Can't dynamic_cast " << filter << " to a MomentumFilter"
	  << endreq;
    }
  } //> filter is a valid pointer
}

template<typename T>
inline void MomentumFilter<T>::setPxMin( const double pxMin )
{
  m_pxRange.setMin(pxMin);
}

template<typename T>
inline void MomentumFilter<T>::setPxMax( const double pxMax )
{
  m_pxRange.setMax(pxMax);
}

template<typename T>
inline void MomentumFilter<T>::setPyMin( const double pyMin )
{
  m_pyRange.setMin(pyMin);
}

template<typename T>
inline void MomentumFilter<T>::setPyMax( const double pyMax )
{
  m_pyRange.setMax(pyMax);
}

template<typename T>
inline void MomentumFilter<T>::setPzMin( const double pzMin )
{
  m_pzRange.setMin(pzMin);
}

template<typename T>
inline void MomentumFilter<T>::setPzMax( const double pzMax )
{
  m_pzRange.setMax(pzMax);
}

template<typename T>
inline void MomentumFilter<T>::setEneMin( const double eneMin )
{
  m_eneRange.setMin(eneMin);
}

template<typename T>
inline void MomentumFilter<T>::setEneMax( const double eneMax )
{
  m_eneRange.setMax(eneMax);
}

template<typename T>
inline void MomentumFilter<T>::setEtaMin( const double etaMin )
{
  m_etaRange.setMin(etaMin);
}

template<typename T>
inline void MomentumFilter<T>::setEtaMax( const double etaMax )
{
  m_etaRange.setMax(etaMax);
}

template<typename T>
inline void MomentumFilter<T>::setPhiMin( const double phiMin )
{
  /// Phi range convention -PI,PI is enforced within the PhiFilterRange
  m_phiRange.setMin(phiMin);
}

template<typename T>
inline void MomentumFilter<T>::setPhiMax( const double phiMax )
{
  /// Phi range convention -PI,PI is enforced within the PhiFilterRange
  m_phiRange.setMax(phiMax);
}

template<typename T>
inline void MomentumFilter<T>::setMassMin( const double massMin )
{
  m_massRange.setMin(massMin);
}

template<typename T>
inline void MomentumFilter<T>::setMassMax( const double massMax )
{
  m_massRange.setMax(massMax);
}

template<typename T>
inline void MomentumFilter<T>::setPtMin( const double ptMin )
{
  m_ptRange.setMin(ptMin);
}

template<typename T>
inline void MomentumFilter<T>::setPtMax( const double ptMax )
{
  m_ptRange.setMax(ptMax);
}

template<typename T>
void MomentumFilter<T>::setRange( const std::string& name,
				  const double min, const double max )
{
  if        ( name == "px" )   { m_pxRange   .setRange( min, max );
  } else if ( name == "py" )   { m_pyRange   .setRange( min, max );
  } else if ( name == "pz" )   { m_pzRange   .setRange( min, max );
  } else if ( name == "ene" )  { m_eneRange  .setRange( min, max );
  } else if ( name == "eta" )  { m_etaRange  .setRange( min, max );
  } else if ( name == "phi" )  { m_phiRange  .setRange( min, max );
  } else if ( name == "mass" ) { m_massRange .setRange( min, max );
  } else if ( name == "pt" )   { m_ptRange   .setRange( min, max );
  } else {
    const std::string error = "Range of name <"+name+"> is UNKNOWN !!";
    throw std::invalid_argument(error);
  }

  return;
}

#endif //> ANALYSISUTILS_MOMENTUMFILTER_H
