// this file is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETANALYSISEDM_PARTICLE_H
#define JETANALYSISEDM_PARTICLE_H
#include "VarHandle.h"

#include <vector>
#include "Math/Vector4D.h"
#include "TVector3.h"
#include "JetAnalysisEDM/Utils.h"

namespace fastjet {
  class PseudoJet;
}


namespace JetAnalysisEDM {
  
  // Forward declaration(s):
  class ParticleContainer;
  class SimpleAttributeMap;


  /// Copy of Athena's SignalStates
  struct P4SignalState {    
    enum State { 
      UNKNOWN             = -1,
      UNCALIBRATED        =  0, // calorimeter signals
      CALIBRATED          =  1,
      ALTCALIBRATED       =  2,
      JETEMSCALE          =  0, // jet signals
      JETFINAL            =  1,
      JETCONSTITUENTSCALE =  2 }; 
  };

  //typedef TLorentzVector LorentzVector_t;
  /// Use a pt,eta,phi based LorentzVector : we most often read and use particle from these coordinates.
  /// TLorentzVector is px,py,pz based : it requires calculation to read from eta,pt,phi and then to retrieve these quantities.
  typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiE4D<float> > LorentzVector_t;

  class Particle : public LorentzVector_t {
    
    friend class ParticleContainer;

    template<typename T> friend class ParticleAccessor;
    template<typename T> friend class ConstParticleAccessor;
    template<typename T> friend class VarHandleMap;
    friend struct VarHandleMapHelper;
    template<typename T> friend class GenericMapCopier;

  public:
    typedef P4SignalState::State    sigstate_t;

    // **********************************************************
    /// Defines some static members to implement signal states.
    static string_t s_sigStateVars[3][4];
    static void setEMSCALEVars(const string_t& pt, const string_t& eta, const string_t& phi, const string_t& E  ) ;
    static void setCONSTSCALEVars(const string_t& pt, const string_t& eta, const string_t& phi, const string_t& E  ) ;

    
  protected:
    /// Constructor only visible to ParticleContainer
    Particle( size_t index, ParticleContainer* container );
    
  public:

    // ****************************************************    
    // constructors
    Particle() : LorentzVector_t(), fContainer(NULL), fIndex(0), m_sourcePart(NULL), m_varMap(NULL){};
    /// Copy constructor
    Particle( const Particle& p );
    Particle( const LorentzVector_t& v );
    Particle(float pt, float eta, float phi, float E );
    virtual ~Particle();
    
    /// Assignement
    ///  If this Particle is from an input container, this has NO effect (in order not to corrupt input variables).
    ///  else :
    ///  - 4-vectors is copied
    ///  - source of this is set to other (or to source of other if other has a source)
    ///  - existing attributes in this are requested from other and copied if existing in other.
    Particle & operator= (const Particle& other);
    // ****************************************************    

    /// Container object of this proxy object
    const ParticleContainer* getContainer() const;
    /// Alias to getContainer for compatibility with Athena Jet
    const ParticleContainer* parentCollection() const {return getContainer();}
    /// Index of the object inside its container
    size_t getIndex() const;
    /// Index of the source particle in its container.
    size_t getSourceIndex() const;
    /// This Particle Source. Returns this if no source.
    const Particle * getSource() const ;
    /// true if source is set.
    bool hasSource() const ;

    /// Create a NEW copy of the Particle (client must delete it). 
    /// If this Particle comes from an Input container, set it as the source of the returned clone.
    Particle* clone() const ;


    // ****************************************************    
    // Getters & Setters
    /// retrieve an attribute of type TT. 
    ///   tpl::const_if_ref_or_ptr<TT>::type is a template trick. It simply translates to
    ///     - 'TYPE'   when TT is a 'TYPE' (and not a reference)
    ///     - 'const TYPE &'   when TT is a 'TYPE &'
    ///     - 'const TYPE *'   when TT is a 'TYPE *'
    ///   This allows to retrieve a reference (ex : avoiding full copies of vectors) while preserving some constness AND keeping the same interface.
    ///  This get version with only 1 string argument will provoke an abort if the string is not found
    template<typename TT> // NOT 'T' : 'T' this is a member of LorentzVector_t !
    typename tpl::const_if_ref_or_ptr<TT>::type get(const string_t & mName) const ;

    /// retrieve an attribute of type TT. 
    ///  see above for explanation of tpl::const_if_ref_or_ptr<>
    ///  The 'valid' argument is set to false if 'mName' is not found. In this case a ummy value is returned.
    template<typename TT> // NOT 'T' : 'T' this is a member of LorentzVector_t !
    typename tpl::const_if_ref_or_ptr<TT>::type get(const string_t & mName, bool &valid) const ;

    template<typename TT>
    std::vector<string_t> getAttributeKeys() const ;

    /// Some shortcuts 
    float getF(const string_t & mName) const ;
    float getI(const string_t & mName) const ;
    const std::vector<float>& getVecF(const string_t & mName) const ;
    const std::vector<int>& getVecI(const string_t & mName) const ;

    // ****************************************************        
    /// set an attribute of type TT. 
    ///   tpl::to_const_ref<TT>::type is a template trick. It simply translates to
    ///     - 'const TYPE *'   when TT is a 'TYPE*'
    ///     - 'const TYPE &'   when TT is a 'TYPE' or 'TYPE&'   
    template<typename TT>
    void set(const string_t & mName, typename tpl::to_const_ref<TT>::type val) const ; 
    // ****************************************************    


    

    // ****************************************************    
    // I/O for a Particle
    /// Set which tree to read from (NOT FUNCTIONAL YET)
    void readFrom( TTree* inputTree, const string_t& prefix, const Long64_t* master);
    /// Define an output tree. Must provide an output prefix.
    void writeTo(TTree* outputTree, const string_t& prefix);
    // ****************************************************    
    

    // ****************************************************
    // alias some kinematics for compatibility with Athena
    inline float m() const {return mass();}

    // ****************************************************
    /// Signal state access
    using LorentzVector_t::pt;
    using LorentzVector_t::eta;
    using LorentzVector_t::phi;
    using LorentzVector_t::E;
    using LorentzVector_t::mass;
    float pt(sigstate_t s) const ;
    float eta(sigstate_t s) const ;
    float phi(sigstate_t s) const ;
    float mass(sigstate_t s) const ;
    float m(sigstate_t s) const;

    float E(sigstate_t s) const ;
    Particle hlv(sigstate_t s) const ;    

    /// redefine functions : only reason is to generate a cint dictionnary (rootcint seems unable to generate a dic for LorentzVector_t from the LinkDef.h)
    inline float pt() const {return LorentzVector_t::pt();}
    inline float px() const {return LorentzVector_t::px();}
    inline float py() const {return LorentzVector_t::py();}
    inline float pz() const {return LorentzVector_t::pz();}
    inline float eta() const {return LorentzVector_t::eta();}
    inline float phi() const {return LorentzVector_t::phi();}
    inline float e() const {return LorentzVector_t::e();}
    inline float mass() const {return LorentzVector_t::mass();}
    inline float rapidity() const {return LorentzVector_t::Rapidity();}
    // ****************************************************


    // ****************************************************
    void loadCurrentEntry();
    //void loadEntry(::Long64_t eventI);
    // ****************************************************    



    // ****************************************************
    /// we have to redefine all kinematic setting methods so the underlying D3PD variable are also updated.
    template< class OtherLorentzVector >
    inline Particle & operator += ( const OtherLorentzVector & q) ;
    template< class OtherLorentzVector >
    inline Particle & operator -= ( const OtherLorentzVector & q) ;
    template<class ForeignLorentzVector>
    inline Particle & operator= ( const ForeignLorentzVector & v);
    inline Particle & SetCoordinates( const Scalar src[] );
    inline Particle& SetCoordinates( Scalar a, Scalar b, Scalar c, Scalar d );
    inline Particle& setPtEtaPhiE( Scalar a, Scalar b, Scalar c, Scalar d ){return SetCoordinates(a,b,c,d);};
    Particle& setPtEtaPhiM( Scalar a, Scalar b, Scalar c, Scalar d );
    Particle& setEEtaPhiM( Scalar a, Scalar b, Scalar c, Scalar d );

    inline Particle & SetXYZT (Scalar xx, Scalar yy, Scalar zz, Scalar tt) ;
    inline Particle & SetPxPyPzE (Scalar xx, Scalar yy, Scalar zz, Scalar ee) ; 
    inline Particle & setPxPyPzE (Scalar xx, Scalar yy, Scalar zz, Scalar ee) {return SetPxPyPzE(xx,yy,zz,ee);}; 
    inline Particle & operator *= (Scalar a);
    inline Particle & operator /= (Scalar a);
    inline Particle & SetE  ( Scalar a )  ;
    inline Particle & SetEta( Scalar a )  ;
    inline Particle & SetPt  ( Scalar a ) ;
    inline Particle & SetPhi( Scalar a )  ;

    void updateKinematics();
    // ****************************************************


    // ****************************************************
    // Constituents
    /// Mimick Athena's Jet::constitutentsN . Valid only if Particle has a source ParticleContainer with declared constituents
    /// See ParticleContainer::declareConstituents
    size_t constitutentsN() const ;
    
    /// Fill constituents of this Particle into constituents. !!! Assumes Particle has a source ParticleContainer with declared constituents
    /// See ParticleContainer::declareConstituents
    void fillConstituents(ParticleContainer& constituents) const;
    
    /// Fill the 'assoParticles' containers with Particle from the 'sourceContainer'
    ///  The Particles from 'sourceContainer' are picked from the vector<index> of this Particle labelled by 'indexVariable'
    void fillAssoContainer(const ParticleContainer& sourceContainer, ParticleContainer& assoParticles, const string_t & indexVariable="constit_index") const;
    // ****************************************************    

    
    // ****************************************************    
    /// Jet Specific method : access to the internal fastjet object. CAN RETURN NULL, Can be invalid, ONLY for expert use. 
    const fastjet::PseudoJet * fastjetPtr() const {return m_pseudoJet;} 
    void setFastjetPtr(const fastjet::PseudoJet * fj){ m_pseudoJet = fj;}

    // ****************************************************    
    
  protected:

    /// Internal kinematic setting methods : used to build the particle from the underlying variables.
    Particle & i_SetE  ( Scalar a )  { LorentzVector_t::SetE  (a); return *this; }
    Particle & i_SetEta( Scalar a )  { LorentzVector_t::SetEta(a); return *this; }
    Particle & i_SetPt  ( Scalar a ) { LorentzVector_t::SetPt (a); return *this; }
    Particle & i_SetPhi( Scalar a )  { LorentzVector_t::SetPhi(a); return *this; }

    
#ifndef __CINT__    
    template<typename T>
    T& accessAtt(const string_t& mName, bool & valid) const ;
    template<typename T>
    T& accessAttFromSource(const string_t & mName, bool & valid) const ;
#endif // __CINT__

    void setPosition(ParticleContainer* container, size_t ind){
      fContainer= container; fIndex = ind;
    }
    const ParticleContainer* getSourceContainer() const;


  private:
    /// Reference to the container of this object
    ParticleContainer* fContainer;
    /// The index of this object inside the container 
    size_t fIndex;

    /// Source Particle. Attribute requests are redirected to this particle, when attributes are not owned by this
    const Particle * m_sourcePart;
    
    // one can't return a reference to a bool within a vector<bool>, use an ugly workaround.
    mutable bool m_workaround_vectorbool;

    /// this map holds the attributes if this Particle is not part of a container. In order to keep Particle light, the SimpleAttributeMap keeps all the I/O related info.
    mutable SimpleAttributeMap *m_varMap;

    /// Pointer to the fastjet object this jet is build from. WARNING : this pointer is meant to be used in reco flow only.
    ///  It is transient only,  provided only to ease calculation of some jet quantities, and will be invalid for jets read from files.
    const fastjet::PseudoJet *m_pseudoJet;

    ClassDef( Particle, 0 )

  }; // class Particle



#ifndef __CINT__    
  // Specialize  for TVector3
  template<>  TVector3 Particle::get<TVector3>(const string_t &mName, bool &valid) const ;
  template<>  TVector3 Particle::get<TVector3>(const string_t &mName) const ;
  
  template<>  void Particle::set<TVector3>(const string_t & mName, const TVector3 & val) const ;


  // Specialize  for LorentzVector_t
  template<>  LorentzVector_t Particle::get<LorentzVector_t>(const string_t &mName, bool &valid) const ;
  template<>  LorentzVector_t Particle::get<LorentzVector_t>(const string_t &mName) const ;
  
  template<>  void Particle::set<LorentzVector_t>(const string_t & mName, const LorentzVector_t & val) const ;



  template<>
  bool& Particle::accessAtt(const string_t & mName, bool & valid) const ;
  template<>
  bool& Particle::accessAttFromSource(const string_t& mName, bool &valid) const ;

#endif



  // ****************************************************    
  // ****************************************************    

  template<typename T>
  class ParticleAccessor {
  public:
    friend class ParticleContainer;

    
    ParticleAccessor() {}
    ParticleAccessor(VarHandle<std::vector<T> >* v,const ParticleContainer *c, const string_t &name): m_handle(v), m_container(c), m_varName(name){}

    const T& operator[](const Particle& el)   const ;
    const T& operator[](const Particle* el) const   {return this->operator[](*el);}

    void readEvent() ; 


  protected:    

    T& at(size_t ind) const { return ( *m_handle )() [ ind ]; }
    T& fromPartIndex(const Particle& p) const {return at(p.getIndex()) ;}
    T& fromPartSourceIndex(const Particle& p) const {return at(p.m_sourcePart->getIndex()) ;}
    T& fromPartGet(const Particle& p) const {bool valid;return p.accessAtt<T>(m_varName,valid);}

    //ParticleAccessor(VarHandle<std::vector<T> >* handle, const ParticleContainer* cont, string_t varName) : m_handle(handle), m_container(cont), m_sourceVector(NULL), m_varName(varName){}

    VarHandle<std::vector<T> > *m_handle;
    const ParticleContainer*  m_container;
    std::vector< T> * m_sourceVector; 
    string_t m_varName;
    T& (ParticleAccessor::*m_accessType) (const Particle& p) const;
  };



  template<typename T>
  class NonConstParticleAccessor : public ParticleAccessor<T> {
  public:

    friend class ParticleContainer;
    NonConstParticleAccessor(VarHandle<std::vector<T> >* v,const ParticleContainer *c, const string_t &name):ParticleAccessor<T>(v,c,name){}

    T& operator[](const Particle& el)   const ;
    T& operator[](const Particle* el) const   {return this->operator[](*el);}


    T& unsafeAccess(const Particle& el) const ;
    
  };




  // inlined functions definitions :
  template<class ForeignLorentzVector>
  Particle & Particle::operator= ( const ForeignLorentzVector & v) { LorentzVector_t::operator=(v); updateKinematics(); return *this;}
  template< class OtherLorentzVector >
  inline Particle & Particle::operator+= ( const OtherLorentzVector & q){ LorentzVector_t::operator+=<OtherLorentzVector>(q); updateKinematics(); return *this;}
  template< class OtherLorentzVector >
  Particle & Particle::operator -= ( const OtherLorentzVector & q) { LorentzVector_t::operator-=<OtherLorentzVector>(q); updateKinematics(); return *this;} 

  inline Particle & Particle::SetCoordinates( const Scalar src[] ) { LorentzVector_t::SetCoordinates(src); updateKinematics(); return *this;}  
  inline Particle & Particle::SetCoordinates( Scalar a, Scalar b, Scalar c, Scalar d )  { LorentzVector_t::SetCoordinates(a,b,c,d); updateKinematics(); return *this;}
  inline Particle & Particle::SetXYZT (Scalar xx, Scalar yy, Scalar zz, Scalar tt){ LorentzVector_t::SetXYZT(xx,yy,zz,tt); updateKinematics(); return *this;} 
  inline Particle & Particle::SetPxPyPzE (Scalar xx, Scalar yy, Scalar zz, Scalar ee){ LorentzVector_t::SetPxPyPzE(xx,yy,zz,ee); updateKinematics(); return *this;}
  inline Particle & Particle::operator *= (Scalar a) { LorentzVector_t::operator*=(a); updateKinematics(); return *this;}
  inline Particle & Particle::operator /= (Scalar a) { LorentzVector_t::operator/=(a); updateKinematics(); return *this;}  
  inline Particle & Particle::SetE  ( Scalar a )  { LorentzVector_t::SetE  (a); updateKinematics(); return *this; }
  inline Particle & Particle::SetEta( Scalar a )  { LorentzVector_t::SetEta(a); updateKinematics(); return *this; }
  inline Particle & Particle::SetPt  ( Scalar a ) { LorentzVector_t::SetPt (a);updateKinematics(); return *this; }
  inline Particle & Particle::SetPhi( Scalar a )  { LorentzVector_t::SetPhi(a); updateKinematics(); return *this; }
  
}

#include "ParticleContainer.h"
#include "Particle.icc"


#endif // JETANALYSISEDM_PARTICLE_H
