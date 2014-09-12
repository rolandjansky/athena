/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include "xAODJet/JetConstituentVector.h"


namespace xAOD {

  double ptFromEEtaM(const double e, const double eta, const double m){
    double p = 0.0;
    if( std::abs( m ) < 0.00001 ) {
      p = e;
    } else {
      p = std::sqrt( e * e - m * m );
      if( e < 0 ) {
	            p = -p;
      }
    }
	
    // Calculate sinTh:
    double aEta = std::abs( eta );
    if( aEta > 710.0 ) {
      aEta = 710.0;
    }
    const double sinTh = 1.0 / std::cosh( aEta );
	
    // Calculate pT from these two:
    return ( p * sinTh );
  }

  
  void fillJetConstituent(const IParticle* part, JetConstituent & constit  , JetConstitScale sigState ){    
    switch( part->type() ){
    case Type::CaloCluster:
      if( sigState == UncalibratedJetConstituent) {
        static SG::AuxElement::Accessor<float>  cl_rawE("rawE")   ;
        static SG::AuxElement::Accessor<float>  cl_rawEta("rawEta") ;
        static SG::AuxElement::Accessor<float>  cl_rawPhi("rawPhi") ;
        static SG::AuxElement::Accessor<float>  cl_rawM  ("rawM")   ;

        constit.SetCoordinates( ptFromEEtaM( cl_rawE(*part) , cl_rawEta(*part), cl_rawM(*part) ),
                                cl_rawEta(*part),
                                cl_rawPhi(*part),
                                cl_rawM(*part) );
        break;
      } 

    default:
      constit.SetCoordinates( part->pt(), part->eta(), 
                              part->phi(), part->m() );
    }
  }
  typedef JetConstituentVector::iterator iterator;
  
  iterator & iterator::operator++(){ m_index++; return *this ; }
  iterator & iterator::operator++(int) { ++m_index; return *this ; }
  iterator & iterator::operator--() { m_index--; return *this ; }
  iterator & iterator::operator--(int) { --m_index; return *this ; }
  bool iterator::operator==( const iterator & other) { return m_index == other.m_index; }
  bool iterator::operator!=( const iterator & other) { return m_index != other.m_index; }
  
  const JetConstituent* iterator::operator*() { update4Mom() ; return &m_4mom;}
  const JetConstituent* iterator::operator->(){ update4Mom() ; return &m_4mom;}

  
  void iterator::update4Mom(){

    if( m_index == m_cachedMomIndex ) return;
    m_cachedMomIndex = m_index;
    const IParticle* part = *(*m_index);
    m_4mom.m_part = part;

    // now get the right scale from the constituent :
    fillJetConstituent(part, m_4mom , m_sigState);
  }

  bool JetConstituentVector::isValid() const {
    typedef std::vector<ElementLink<IParticleContainer> > ELVector;
    for ( ELVector::const_iterator icon=m_elVector->begin(); icon!=m_elVector->end(); ++icon ) {
      ElementLink<IParticleContainer> el = *icon;
      if ( ! el.isValid() ) return false;
    }
    return true;
  }

  bool     JetConstituentVector::empty() const { return m_elVector->empty() ; }
  size_t   JetConstituentVector::size()  const { return m_elVector->size() ; }
  iterator JetConstituentVector::begin() const { return iterator(m_elVector->begin(), m_sigState)  ; }
  iterator JetConstituentVector::end()   const { return iterator(m_elVector->end(), m_sigState)  ; }

  iterator JetConstituentVector::begin(JetConstitScale s) const { return iterator(m_elVector->begin(), s)  ; }
  iterator JetConstituentVector::end(JetConstitScale s)   const { return iterator(m_elVector->end(), s)  ; }
  
  JetConstituent JetConstituentVector::operator[](size_t i) const { return at(i); }

  JetConstituent JetConstituentVector::at(size_t i) const { 
    JetConstituent c; 
    fillJetConstituent(*(m_elVector->at(i)), c , m_sigState);
    return c;
  }

  JetConstituent JetConstituentVector::front() const {return at(0);}

  JetConstituent JetConstituentVector::back()  const {return at(size()-1);}

  std::vector<const IParticle*> JetConstituentVector::asIParticleVector() const {
    std::vector<const IParticle*> v( m_elVector->size() );
    for(size_t i=0;i<v.size(); i++) v[i] = *(m_elVector->at(i));
    return v;
  }


}
