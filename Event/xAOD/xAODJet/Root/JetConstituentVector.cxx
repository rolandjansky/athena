/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include "xAODJet/JetConstituentVector.h"
#include "xAODCaloEvent/CaloCluster.h"
#if !defined(SIMULATIONBASE) and !defined(GENERATIONBASE)
#include "xAODPFlow/PFO.h"
#endif // not SIMULATIONBASE or GENERATIONBASE

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
    if( sigState == UncalibratedJetConstituent) {
      switch( part->type() ){
      case Type::CaloCluster: {
        const xAOD::CaloCluster *cl = dynamic_cast<const xAOD::CaloCluster*>(part);
        if( cl ) { 
          constit.SetCoordinates( ptFromEEtaM( cl->rawE(), cl->rawEta(), cl->rawM() ),
                                  cl->rawEta(),
                                  cl->rawPhi(),
                                  cl->rawM() );
        } 
        return;
      }
#if !defined(SIMULATIONBASE) and !defined(GENERATIONBASE)
      case Type::ParticleFlow: {
        const xAOD::PFO *pfo = dynamic_cast<const xAOD::PFO*>(part);
        if(pfo->ptEM()!=0) constit.SetCoordinates( pfo->ptEM(), pfo->etaEM(), pfo->phiEM(), pfo->mEM() );
        else constit.SetCoordinates( 0, 1, 1, 0 ); // To avoid Warnings from root.
        return;
      }
#endif // not SIMULATIONBASE or GENERATIONBASE
      default: 
        break;// fall back on default kinematics
      }
    }
    // if we have not returned above, the fall back  is using the default scale :
    constit.SetCoordinates( part->pt(), part->eta(), 
                            part->phi(), part->m() );    
  }


  using iterator = JetConstituentVector::iterator;
  
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
    using ELVector = std::vector<ElementLink<IParticleContainer> >;
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
    c.m_part = *(m_elVector->at(i));
    return c;
  }

  JetConstituent JetConstituentVector::front() const {return at(0);}

  JetConstituent JetConstituentVector::back()  const {return at(size()-1);}

  std::vector<const IParticle*> JetConstituentVector::asIParticleVector() const {
    std::vector<const IParticle*> v( m_elVector->size() );
    for(size_t i=0;i<v.size(); i++) v[i] = *(m_elVector->at(i));
    return v;
  }

  std::vector<JetConstituent> JetConstituentVector::asSTLVector(){
    size_t N = size();
    std::vector<JetConstituent> vec(N);
    for ( size_t i=0;i<N;i++ ) {
      fillJetConstituent( *(m_elVector->at(i)) , vec[i], m_sigState ); 
      vec[i].m_part = *(m_elVector->at(i));
    }
    return vec;
  }


}
