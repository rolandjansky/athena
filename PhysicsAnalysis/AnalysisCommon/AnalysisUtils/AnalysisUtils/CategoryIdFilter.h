///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CategoryIdFilter.h 
// Header file for class CategoryIdFilter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISUTILS_CATEGORYIDFILTER_H 
#define ANALYSISUTILS_CATEGORYIDFILTER_H 

/** CategoryIdFilter filters objects based on their PDG Id.
 *  It accepts them if they belong to some given category (or a set of 
 *  categories).
 *  Please not that if you give a list of categories, every particle which
 *  fulfills <b>at least one</b> of the categories will be accepted : 
 *  <i>logical OR</i>.
 */

// STL includes

// HepMC / CLHEP includes
#include "HepPDT/ParticleData.hh"

// Gaudi includes
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IPartPropSvc.h"

// AnalysisUtils includes
#include "AnalysisUtils/Categories.h"
#include "AnalysisUtils/IFilter.h"

// Forward declaration

template <typename T>
class CategoryIdFilter : virtual public IFilter<T>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  CategoryIdFilter();

  /** Copy constructor: 
   */
  CategoryIdFilter( const CategoryIdFilter<T>& rhs );

  // Constructor with parameters: 

  /** Destructor: 
   */
  virtual ~CategoryIdFilter(); 

  /** Assignment operator: 
   */
  CategoryIdFilter & operator=(const CategoryIdFilter<T>& rhs ); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Main filter method. This method takes the decision whether or not
   *  the filter has to accept the element. It returns true or false
   *  accordingly if the particle's id matches one of the required one.
   */
  virtual bool isAccepted( const T * element ) const;

  /** Return the list of categories the object might match (OR condition)
   */
  std::vector<PDGCat::category> getCategories() const;

  /** Return true if the element belongs to a given Category
   */
  bool belongsTo( const T * element, const PDGCat::category& cat ) const;

  /** Return true if the element is a Lepton
   */
  bool isLepton( const T * element ) const;
  
  /** Return true if the element is a Hadron
   */
  bool isHadron( const T * element ) const;
  
  /** Return true if the element is a Meson
   */
  bool isMeson( const T * element ) const;
  
  /** Return true if the element is a Baryon
   */
  bool isBaryon( const T * element ) const;
  
  /** Return true if the element is a diquark
   */
  bool isDiQuark( const T * element ) const;
  
  /** Return true if elements contains a quark up
   */
  bool hasUp( const T * element ) const;
  
  /** Return true if elements contains a quark down
   */
  bool hasDown( const T * element ) const;
  
  /** Return true if elements contains a quark strange
   */
  bool hasStrange( const T * element ) const;
  
  /** Return true if elements contains a quark charm
   */
  bool hasCharm( const T * element ) const;
  
  /** Return true if elements contains a quark bottom
   */
  bool hasBottom( const T * element ) const;
  
  /** Return true if elements contains a quark top
   */
  bool hasTop( const T * element ) const;
  
  /** Return true if the element is a Boson (gauge+Higgs boson)
   */
  bool isBoson( const T * element ) const;
  
  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
  void setFilter( const IFilterCuts * filter );

  void addCategory( const PDGCat::category& cat );
  void setCategory( const PDGCat::category& cat );
  void setCategories( const std::vector<PDGCat::category>& cat );

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Retrieve a pointer to the Particle Property Service
   */
  void setPartPropSvc();

  /** Return true if the element belongs to a given Category.
   *  The ParticleData has been made from the pdgId() of the element
   */
  bool isInCategory( const T * t,
		     const PDGCat::category& cat ) const;

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** The list of categories (Lepton, BBaryon, BMeson,...) which
   *  might be accepted
   */
  std::vector<PDGCat::category> m_categories;

  /** Pointer to the Particle Property Service : this is to retrieve
   *  informations about the particles (and to get the predicates for
   *  the isLepton, isBaryon and so forth)
   */
  IPartPropSvc* m_ppSvc;

  // 
  // The following is temporary :
  // It is there waiting for the CLHEP corresponding methods/enums to
  // be fixed (see Savannah bug#7495 )
  //
 protected:
  enum location { nj=0, nq1, nq2, nq3, nl, nr, n, n8, n9, n10, nDIGITS };

  /** Decode the pdg id (nDIGITS) to retrieve its quark content
   */
  std::vector<int> decodePdgId( const int pdgId ) const;
  int extraBits( const int id ) const { return std::abs(id) /10000000; }
  unsigned short digit( const int id, const location loc ) const;
  int fundamentalID( const int id ) const;
  bool hasQuark( const T* element, const unsigned int i ) const;
  bool hasQuark( const int PDGid, const unsigned int i ) const;
}; 

/// I/O operators
//////////////////////
//MsgStream& operator<<( MsgStream & msg, const CategoryIdFilter &obj );

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

//m_nDigits =7;  

template<typename T>
CategoryIdFilter<T>::CategoryIdFilter() :
  IFilterCuts(),
  IFilter<T>(),
  m_categories(),
  m_ppSvc(0)
{
  setPartPropSvc();
}

template<typename T>
CategoryIdFilter<T>::CategoryIdFilter( const CategoryIdFilter<T>& rhs ) :
  IFilterCuts(rhs),
  IFilter<T>(rhs),
  m_categories(rhs.m_categories),
  m_ppSvc( rhs.m_ppSvc )
{}

template<typename T>
CategoryIdFilter<T>& CategoryIdFilter<T>::operator=(const CategoryIdFilter<T>& rhs )
{
  if ( this != &rhs ) {
    IFilterCuts::operator=(rhs);
    IFilter<T>::operator=( rhs );
    m_categories = rhs.m_categories;
    m_ppSvc      = rhs.m_ppSvc;
  }
  return *this;
}

template<typename T>
CategoryIdFilter<T>::~CategoryIdFilter() {}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////
template<typename T>
bool CategoryIdFilter<T>::isAccepted( const T * p ) const
{
  if ( m_categories.empty() ) {
    return true;
  }

  //const HepPDT::ParticleData * partData = 0;
  //partData = m_ppSvc->PDT()->particle(HepPDT::ParticleID(std::abs(p->pdgId())));
  for ( std::vector<PDGCat::category>::const_iterator itr = m_categories.begin();
	itr != m_categories.end();
	++itr ) {
    if ( isInCategory( p, *itr ) ) {
      return true;
    }
  }
  return false;
}

template<typename T>
inline std::vector<PDGCat::category> CategoryIdFilter<T>::getCategories() const
{
  return m_categories;
}

template<typename T>
bool CategoryIdFilter<T>::belongsTo( const T * t, 
				     const PDGCat::category& cat ) const
{
  //const HepPDT::ParticleData * partData = 0;
  //partData = m_ppSvc->PDT()->particle(HepPDT::ParticleID(std::abs(t->pdgId())));
  return isInCategory( t, cat );
}

template<typename T>
bool CategoryIdFilter<T>::isLepton( const T * element ) const
{
  if ( element ) {
    if ( std::abs( element->pdgId() ) <= 18 &&
	 std::abs( element->pdgId() ) >= 11 ) {
      return true;
    }
  }
  return false;
}
  
template<typename T>
bool CategoryIdFilter<T>::isHadron( const T * element ) const
{
  if ( element ) {
    if ( extraBits(element->pdgId()) > 0 ) return false;
    if ( isMeson(element) ||
	 isBaryon(element) ) {
      return true;
    }
  }
  return false;
}
  
template<typename T>
bool CategoryIdFilter<T>::isMeson( const T * element ) const
{
  if ( element ) {
    const int id = element->pdgId();
    const unsigned int absid = std::abs(id);
    if( extraBits(absid) > 0 ) { return false; }
    if( absid <= 100 )         { return false; }
    if( absid == 130 || absid == 310 || absid == 210 ) { return true; }
    if( id == 110 || id == 220 || id == 330 || id == 440 ) { return true; }
    if( digit(id,nj)  > 0 && 
	digit(id,nq1) > 0 && digit(id,nq2) > 0 && digit(id,nq3) == 0 ) {
      // check for illegal antiparticles
      if( digit(id,nq3) == digit(id,nq2) && id < 0 ) {
	return false;
      } else {
	return true;
      }
    }
  }
  return false;
}
  
template<typename T>
bool CategoryIdFilter<T>::isBaryon( const T * element ) const
{
  if ( element ) {
    const int id = element->pdgId();
    const unsigned absid = std::abs(id);
    if( extraBits(id) > 0 ) { return false; }
    if( absid <= 100 )      { return false; }
    if( fundamentalID(id) <= 100 && fundamentalID(id) > 0 ) { return false; }
    if( absid == 2110 || absid == 2210 ) { return true; }
    if( digit(id,nj)  > 0 && 
	digit(id,nq1) > 0 && digit(id,nq2) > 0 && digit(id,nq3) > 0 ) { 
      return true; 
    }
  }
  return false;
}

template<typename T>
bool CategoryIdFilter<T>::isDiQuark( const T * element ) const
{
  if ( element ) {
    const int id = element->pdgId();
    const unsigned absid = std::abs(id);
    if( extraBits(id) > 0 ) { return false; }
    if( absid <= 100 ) { return false; }
    if( fundamentalID(id) <= 100 && fundamentalID(id) > 0 ) { return false; }

    // diquark signature
    if( digit(id,nj) > 0 && 
	digit(id,nq1) == 0 && digit(id,nq2) > 0 && digit(id,nq3) > 0 ) {
      // illegal
      if( digit(id,nj) == 1 && digit(id,nq2) == digit(id,nq3) ) {
        return false; 
      } else {
        return true;
      }
    }//> diquark?
  }
  return false;
}
  
template<typename T>
bool CategoryIdFilter<T>::hasUp( const T * element ) const
{
  return hasQuark(element,PDG::u);
}
  
template<typename T>
bool CategoryIdFilter<T>::hasDown( const T * element ) const
{
  return hasQuark(element,PDG::d);
}
  
template<typename T>
bool CategoryIdFilter<T>::hasStrange( const T * element ) const
{
  return hasQuark(element,PDG::s);
}
  
template<typename T>
bool CategoryIdFilter<T>::hasCharm( const T * element ) const
{
  return hasQuark(element,PDG::c);
}
  
template<typename T>
bool CategoryIdFilter<T>::hasBottom( const T * element ) const
{
  return hasQuark(element,PDG::b);
}
  
template<typename T>
bool CategoryIdFilter<T>::hasTop( const T * element ) const
{
  return hasQuark(element,PDG::t);
}
  
template<typename T>
bool CategoryIdFilter<T>::isBoson( const T * element ) const
{
  if ( element &&
       std::abs( element->pdgId() ) <= 37 &&
       std::abs( element->pdgId() ) >= 21 ) {
    return true;
  }
  return false;
}

template<typename T>
bool CategoryIdFilter<T>::isInCategory( const T * t,
					const PDGCat::category& cat ) const
{
  /*
  const HepPDT::ParticleData * p = 0;
  p = m_ppSvc->PDT()->particle(HepPDT::ParticleID(std::abs(t->pdgId())));
  */

  if ( PDGCat::Untagged == cat ) {        return ( PDG::null == t->pdgId() );
  } else if ( PDGCat::Lepton == cat ) {   return isLepton(t);
  } else if ( PDGCat::Hadron == cat ) {   return isHadron(t);
  } else if ( PDGCat::Meson  == cat ) {   return isMeson(t);
  } else if ( PDGCat::Baryon == cat ) {   return isBaryon(t);
  } else if ( PDGCat::DiQuark == cat ) {  return isDiQuark(t);
  } else if ( PDGCat::BMeson == cat ) {   return isMeson(t) && hasBottom(t);
  } else if ( PDGCat::BBaryon == cat ) {  return isBaryon(t)&& hasBottom(t);
  } else if ( PDGCat::DFlavour == cat ) { return hasDown(t);
  } else if ( PDGCat::UFlavour == cat ) { return hasUp(t);
  } else if ( PDGCat::SFlavour == cat ) { return hasStrange(t);
  } else if ( PDGCat::CFlavour == cat ) { return hasCharm(t);
  } else if ( PDGCat::BFlavour == cat ) { return hasBottom(t);
  } else if ( PDGCat::TFlavour == cat ) { return hasTop(t);
  } else if ( PDGCat::LightFlavour == cat ) { 
    return (hasDown(t) || hasUp(t) || hasStrange(t) || hasCharm(t) );
  } else if ( PDGCat::HeavyFlavour == cat ) { 
    return hasBottom(t) || hasTop(t);
  } else if ( PDGCat::Boson == cat )    { return isBoson(t);
  } else {
    std::cerr << "Category NOT recognised : <" << cat << "> !!" << std::endl;
    throw std::string( "ERROR" );
  }

  return false;
}

template<typename T>
std::vector<int> CategoryIdFilter<T>::decodePdgId( const int pdgId ) const
{
  const unsigned int id = std::abs(pdgId);
  std::vector<int> digits(nDIGITS);

  /*
  for ( int i = nDIGITS-1; i >= 0; --i ) {
    unsigned int modi = static_cast<unsigned int>(std::pow(10.,i*1.) );
    unsigned int modj = static_cast<unsigned int>(std::pow(10.,(i-1)*1.) );
    digits[i] = ( id % modi )/ modj;
  }
  */
  for ( unsigned int i = 0; i < nDIGITS; ++i ) {
    unsigned int modi = static_cast<unsigned int>(std::pow(10.,(i+1)*1.) );
    unsigned int modj = static_cast<unsigned int>(std::pow(10., i*1.) );
    digits[(nDIGITS-1)-i] = ( id % modi )/ modj;
  }

  return digits;
}

template<typename T>
unsigned short 
CategoryIdFilter<T>::digit( const int pdgId, const location loc ) const
{
  //  PID digits (base 10) are: n nr nl nq3 nq2 nq1 nj
  //  the location enum provides a convenient index into the PID
  const unsigned int num = static_cast<unsigned int>(std::pow(10.0,(loc)));
  return (std::abs(pdgId)/num)%10;
}

template<typename T>
int CategoryIdFilter<T>::fundamentalID( const int id ) const
{
  if( digit(id,nq2) == 0 && digit(id,nq3) == 0) {
    return std::abs(id)%10000;
  } else if( std::abs(id) <= 100 ) {
    return std::abs(id);
  } else {
    return 0;
  }
}

template<typename T>
bool CategoryIdFilter<T>::hasQuark( const T * element, 
				    const unsigned int i ) const
{
  if ( element ) {
    return hasQuark( element->pdgId(), i );
  }
  return false;
}
template<typename T>
bool CategoryIdFilter<T>::hasQuark( const int pdgId, 
				    const unsigned int i ) const
{
  const unsigned absid = std::abs(pdgId);
  if( extraBits(pdgId) > 0 ) { return false; }
  if( absid <= 100 && absid > 8 )      { return false; }
  if( fundamentalID(pdgId) <= 100 && 
      fundamentalID(pdgId) > 0 ) { 
    return (absid==i); 
  }
  if( digit(pdgId,nq3) == i || 
      digit(pdgId,nq2) == i || 
      digit(pdgId,nq1) == i ) { 
    return true; 
  }
  return false;
}
/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

template<typename T>
void CategoryIdFilter<T>::setFilter( const IFilterCuts * filter )
{
  if ( filter ) {
    const CategoryIdFilter<T> * catFilter = 
      dynamic_cast<const CategoryIdFilter<T> *>(filter );
    
    if ( catFilter ) {
      operator=(*catFilter);
    } else {
      MsgStream log( Athena::getMessageSvc(), "CategoryIdFilter" );
      log << MSG::ERROR
	  << "Can't dynamic_cast " << typeid(filter).name() 
	  << " to a CategoryIdFilter"
	  << endreq;
    }
  } //> filter is not a NULL pointer
}

template<typename T>
inline void CategoryIdFilter<T>::addCategory(const PDGCat::category& cat)
{
  m_categories.push_back( cat );
}

template<typename T>
void CategoryIdFilter<T>::setCategory( const PDGCat::category& cat )
{
  m_categories.clear();
  addCategory(cat);
}

template<typename T>
inline void CategoryIdFilter<T>::setCategories( const std::vector<PDGCat::category>& cat )
{
  m_categories = cat;
}

template<typename T>
void CategoryIdFilter<T>::setPartPropSvc()
{
  /*
  StatusCode sc = Gaudi::svcLocator()->service( "PartPropSvc", m_ppSvc, true );
  if ( sc.isFailure() || 0 == m_ppSvc ) {
    const std::string error("Could NOT locate PartPropSvc !!");
    throw GaudiException( error, "CategoryIdFilter<T>::setPartPropSvc", 
			  StatusCode::FAILURE );
  }
  */
}
#endif //> ANALYSISUTILS_CATEGORYIDFILTER_H
