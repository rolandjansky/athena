// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODJET_JETCONSTITUENTSVECTOR_H
#define XAODJET_JETCONSTITUENTSVECTOR_H
/////////////////////////////////////////////////
/// \file JetConstituentVector.h
/// \brief This file defines helper classes to deal with jet constituents.
/// 
/// Helper classes are necessary because the default 4-vectors of the constituents (ex: clusters) 
/// possibly differ from the 4-vector used in the jet finding procedure (ex: when finding jets using 
/// the non calibrated "EM" scale of clusters).
/// These classes allow to retrieve the expected scale of the constituents automatically.
/// 2 classes are defined for this.
/// JetConstituentVector : an object behaving like a vector<const IParticle*>. It actually "contains" 
/// JetConstituents which are no more than a 4-vector object (JetFourMom_t) plus a few methods allowing it 
/// to act as an IParticle* and to return the underlying exact IParticle*
///
/// Usage is as simple as :
///     xAOD::JetConstituentVector vec = j->getConstituents();
///     xAOD::JetConstituentVector::iterator it = vec.begin();
///     xAOD::JetConstituentVector::iterator itE = vec.end();
///     for( ; it != itE; it++){
///       float pt = (*it)->pt(); ... etc ...
///      }
/// or 
///     JetConstituent firstC = vec[0];
///     float pt = firstC->pt(); // JetConstituent also behaves like a IParticle*
/// 
/// Although it behaves like a vector<const IParticle*> it is not one.
/// To retrieve the constituents as vector<const IParticle*> use JetConstituentVector::asIParticleVector()
///  !!! WARNING !!! when doing so, the particles in the returned vector might be at a wrong scale w.r.t the jet.
///
///  To retrieve a std::vector<JetConstituent>  with the same content as JetConstituentVector ,
//   use JetConstituentVector::asSTLVector(). 
///     This method is slighly under-performant since it will create and fill a new std::vector whereas
///     using JetConstituentVector::iterator only cost a light proxy object to the internal link vector.
/////////////////////////////////////////////////
 

// Core include(s):
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"

#include "xAODJet/JetTypes.h"

namespace xAOD {
  
  /////////////////////////////////////////////////
  /// \class JetConstituent
  /// \brief 4-vector of jet constituent at the scale used during jet finding.
  /// 
  /// JetConstituent is a 4-vector and a proxy to the IParticle* used as a constituent of a jet.
  /// it inherits xAOD::JetFourMom_t and provides an interface very similar to IParticle.
  class JetConstituent : public xAOD::JetFourMom_t {
  public:
    friend class JetConstituentVector;

    
    JetConstituent(const IParticle* part=0) : xAOD::JetFourMom_t(), m_part(part){}
    
    double           pt() const { return Pt();}
    /// The pseudorapidity (\f$\eta\f$) of the particle
    double           eta() const { return Eta();}
    /// The azimuthal angle (\f$\phi\f$) of the particle
    double           phi() const { return Phi();}
    /// The invariant mass of the particle
    double           m() const { return M();}
    /// The total energy of the particle
    double           e() const { return E();}
    /// The true rapidity (y) of the particle
    double           rapidity() const { return Rapidity();}
    
    /// Definition of the 4-momentum type
    typedef IParticle::FourMom_t FourMom_t;
    
    /// The full 4-momentum of the particle
    // 
    //const FourMom_t& p4() const { return FourMom_t(Px(), Py(), Pz(), E());}
 
    /// @}
    
    
    /// The type of the object as a simple enumeration
    Type::ObjectType type() const { return m_part->type();}

    /// Access the underlying IParticle's aux data.
    template< class T >
    const T& auxdata( const std::string& name,
                      const std::string& clsname = "" ) const;

    /// operator-> allows to use JetConstituentVector with same syntax as vector<IParticle*>  
    const JetConstituent* operator->() const {return this;};

    /// Access the real underlying IParticle
    const IParticle* rawConstituent() const { return m_part; }
  protected:
    
    const IParticle * m_part;

  };


  /////////////////////////////////////////////////
  /// \class JetConstituentVector
  /// \brief A vector of jet constituents at the scale used during jet finding.
  /// 
  /// This class can be used as a vector<const IParticle*>. 
  /// It provides iterators and the usual function of vector<>
  /// 
  class JetConstituentVector {

  public: 
    
    class iterator {
    public:
      friend class JetConstituentVector;
      typedef std::vector< ElementLink< IParticleContainer > >::const_iterator ELiterator;
      
      iterator(ELiterator it, JetConstitScale s) : m_index(it), m_sigState(s) {}

      iterator & operator++();
      iterator   operator++(int);
      iterator & operator--();
      iterator   operator--(int);
      bool operator==( const iterator & other) const;
      bool operator!=( const iterator & other) const;

      const JetConstituent* operator*();
      const JetConstituent* operator->();

      // for python, we alias operator++/--
      void inc(){ this->operator++();}
      void dec(){ this->operator--();}


    protected:


      void update4Mom();
      
      ELiterator m_index;
      ELiterator m_cachedMomIndex;
      JetConstituent m_4mom;

      JetConstitScale m_sigState; 
      
    };

    typedef iterator const_iterator;
    typedef std::vector< ElementLink< IParticleContainer > > ELvector;

    JetConstituentVector(const ELvector* elv , JetConstitScale s ) : m_elVector(elv) , m_sigState(s) {}

    /// Check if element links are valid.
    bool isValid() const;


    /// true if vector is empty()
    bool empty() const ;
    /// number of constituents
    size_t size() const ;

    /// iterator on the first constituent
    iterator begin() const ;
    /// iterator after the last constituent
    iterator end() const ;

    /// iterator at scale \param s on the first constituent
    iterator begin(JetConstitScale s) const ;
    /// iterator at scale \param s after the last constituent
    iterator end(JetConstitScale s) const ;


    /// Constituent proxy at position i
    JetConstituent operator[](size_t i) const ;
    /// Constituent proxy at position i
    JetConstituent at(size_t i) const ;
    /// first constituent proxy
    JetConstituent front() const ;
    /// last constituent proxy
    JetConstituent back()  const ;

    /// vector of pointer to the underlying IParticles. 
    /// WARNING !! these raw IParticle constituents might have a scale incompatible with the jet they're coming from.
    std::vector<const IParticle*> asIParticleVector() const ;

    /// Returns this vector as a std::vector
    ///  Provided for convenience when dealing with the JetConstituentVector::iterator is not possible.
    ///  This method is slighly under-performant since it will create and fill a new std::vector. 
    std::vector<JetConstituent> asSTLVector();

  protected:
    
    const std::vector< ElementLink< IParticleContainer > > * m_elVector;
    JetConstitScale m_sigState; 

  };

// Hide the implementation of the template function(s) from gccxml:
#ifndef __GCCXML__


  // use TT as the template parameter to avoid conflicts with LorentzVector::T() !
  template<class TT>  const TT& JetConstituent::auxdata( const std::string& name,
                                    const std::string& clsname ) const {
    return m_part->template auxdata<TT>(name, clsname);
  }


#endif // not __GCCXML__

} // namespace xAOD

#endif
