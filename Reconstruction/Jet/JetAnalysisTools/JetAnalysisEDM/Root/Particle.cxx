/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define JETANALYSISEDM_PARTICLE_CXX  
#include "JetAnalysisEDM/Particle.h"


ClassImp( JetAnalysisEDM::Particle )

namespace JetAnalysisEDM {

  string_t Particle::s_sigStateVars[3][4] = {
    {"emscale_pt","emscale_eta","emscale_phi","emscale_E"},
    {"","","",""},
    {"constscale_pt","constscale_eta","constscale_phi","constscale_E"},
  };

  void Particle::setEMSCALEVars(const string_t& pt, const string_t& eta, const string_t& phi, const string_t& E  ) {
    s_sigStateVars[0][0] = pt;s_sigStateVars[0][1] = eta;s_sigStateVars[0][2] = phi;s_sigStateVars[0][3] = E;
  }
  void Particle::setCONSTSCALEVars(const string_t& pt, const string_t& eta, const string_t& phi, const string_t& E  ) {
    s_sigStateVars[2][0] = pt;s_sigStateVars[2][1] = eta;s_sigStateVars[2][2] = phi;s_sigStateVars[2][3] = E;
  }

   /**
    * This constructor can be used to create new proxy objects from scratch.
    * Since the constructor of such an object is quite complicated and
    * error prone, it is only visible to the parent class ParticleContainer.
    */
   Particle::Particle( size_t index,  ParticleContainer* parent )
     :       fContainer( parent ), fIndex( index ), m_sourcePart(NULL), m_varMap(NULL), m_pseudoJet(NULL) {

   }

   /**
    * This constructor is useful for creating copies of proxy objects.
    * Such objects are fairly cheap to copy, so the user is allowed to have
    * his/her containers of them inside an analysis code. (To select and
    * sort objects according to some criteria for instance.)
    *
    * @param parent The proxy object that should be copied
    */
   Particle::Particle( const Particle& p )
     : LorentzVector_t( (const LorentzVector_t&) p ),
       fContainer( p.fContainer ), fIndex( p.fIndex ), m_sourcePart(p.m_sourcePart), m_varMap(NULL), m_pseudoJet(p.m_pseudoJet) {
   }

   Particle::Particle( const LorentzVector_t& p )
     : LorentzVector_t( p ),
       fContainer( NULL ), fIndex( 0 ), m_sourcePart(NULL), m_varMap(NULL), m_pseudoJet(NULL) {    
   }

   Particle::Particle( float pt, float eta, float phi, float E )
     : LorentzVector_t( pt, eta, phi, E ),
       fContainer( NULL ), fIndex( 0 ), m_sourcePart(NULL), m_varMap(NULL), m_pseudoJet(NULL) {    
   }


  Particle::~Particle(){
    if(m_varMap) delete m_varMap;
  }

  Particle& Particle::operator=(const Particle & other){
    // DON'T modify particles from an input container
    if( (fContainer ) && (fContainer->fFromInput) ) return *this; 

    // if other is from an input container, it's a source !
    if( (other.fContainer ) && (! other.m_sourcePart) ) m_sourcePart = &other;
    else m_sourcePart = other.m_sourcePart; // else forward the source particle.

    // copy attributes this owns from other if needed
    if(m_varMap){
      m_varMap->copyFrom(&other);
    }
    
    LorentzVector_t::operator=( (LorentzVector_t&)other);
    m_pseudoJet = other.m_pseudoJet;
    return *this;
  }

  
  Particle* Particle::clone() const {
    Particle * p = new Particle(*this); // this sets p->m_sourcePart = this->m_sourcePart
    p->fContainer = NULL;
    //if( (fContainer ) && (! m_sourcePart) )  p->m_sourcePart = this;
    if( ! m_sourcePart )   p->m_sourcePart = this;
    p->m_pseudoJet = m_pseudoJet ;
    return p;
  }
  
  /**
   * This function can be used to access the parent object of this
   * proxy object. It can come in handy when optimizing an analysis code.
   *
   * @returns A reference to this object's parent
   */
  const ParticleContainer* Particle::getContainer() const {
    
    return fContainer;
  }
  
  const ParticleContainer* Particle::getSourceContainer() const {
    if(m_sourcePart) return m_sourcePart->fContainer;
    return fContainer;
  }
  
  /**
   * This function can be used to access the index of this proxy object
   * inside the parent container. It can come in handy when optimizing an
   * analysis code.
   *
   * @returns The index of the proxy in the object's parent
   */
  size_t Particle::getIndex() const {
    
    return fIndex;
  }
  
  size_t Particle::getSourceIndex() const {
    if(m_sourcePart) return m_sourcePart->getSourceIndex();
    return fIndex;
  }
  
  const Particle * Particle::getSource() const {
    return m_sourcePart ? m_sourcePart : this;
  }
  
  bool Particle::hasSource() const {
    return m_sourcePart != NULL;
  }
  

  void Particle::readFrom( TTree* inputTree, const string_t& prefix, const Long64_t* master){
    if(!m_varMap) m_varMap = new SimpleAttributeMap(this, prefix);
    m_varMap->setParameters(prefix, master); 
    m_varMap->readFrom(inputTree);
  }

  void Particle::writeTo(TTree* outputTree, const string_t& prefix){
    if(!m_varMap) m_varMap = new SimpleAttributeMap(this,prefix);
    if(prefix != "") m_varMap->setPrefix(prefix);
    m_varMap->addVar<float>("pt",true);
    m_varMap->addVar<float>("eta",true);
    m_varMap->addVar<float>("phi",true);
    m_varMap->addVar<float>("E",true);
    m_varMap->writeTo(outputTree);    
  }

  void Particle::loadCurrentEntry(){
    if(!m_varMap) return;
    bool valid;
    i_SetE( m_varMap->get<float>("E",valid) );
    i_SetEta( m_varMap->get<float>("eta",valid) );
    i_SetPhi( m_varMap->get<float>("phi",valid) );
    i_SetPt( m_varMap->get<float>("pt",valid) );
  }

  void Particle::updateKinematics(){
    if(fContainer){ fContainer->updateKinematics(this); }
    else if(m_varMap)  { m_varMap->updateKinematics(); }
  }


  void Particle::fillAssoContainer(const ParticleContainer& sourceContainer, ParticleContainer& assoParticles, const string_t & indexVariable) const{
    const std::vector<int>  & indInSource = get< std::vector<int> >( indexVariable);
    assoParticles.setSubsetOfInputContainer( &sourceContainer );
    for(size_t i=0; i< indInSource.size(); i++){
      //if(i<3) std::cout <<  indInSource[i] << "  "<< sourceContainer[indInSource[i]]->eta() << std::endl;
      assoParticles.addNewFromSource( sourceContainer[ indInSource[i] ] );
    }
    
  }


  size_t Particle::constitutentsN() const {
    const ParticleContainer* source = getSourceContainer();
    size_t n=0;
    if(source){
      n = (size_t) get<int>(source->m_constIndexName);
    }
    return n;
  }

  void Particle::fillConstituents(ParticleContainer& constituents) const {
    const ParticleContainer* source = getSourceContainer();
    if(source){
      fillAssoContainer(*source->m_constituentSource, constituents, source->m_constIndexName); 
    }
  }



  
  // ****************************************************
  float Particle::pt(Particle::sigstate_t s) const {
    if( s == P4SignalState::JETFINAL ) return pt();
    return get<float>(s_sigStateVars[s][0]);
  }
  float Particle::eta(Particle::sigstate_t s) const {
    if( s == P4SignalState::JETFINAL ) return eta();
    return get<float>(s_sigStateVars[s][1]);
  }
  float Particle::phi(Particle::sigstate_t s) const {
    if( s == P4SignalState::JETFINAL ) return phi();
    return get<float>(s_sigStateVars[s][2]);
  }
  float Particle::E(Particle::sigstate_t s) const {
    if( s == P4SignalState::JETFINAL ) return E();
    return get<float>(s_sigStateVars[s][3]);
  }
  float Particle::mass(Particle::sigstate_t s) const {
    if( s == P4SignalState::JETFINAL ) return mass();
    return hlv(s).mass();
  }
  float Particle::m(Particle::sigstate_t s) const { return mass(s);}

  Particle Particle::hlv(Particle::sigstate_t s) const  {
    Particle p; p =*this;
    if( s != P4SignalState::JETFINAL ) p.SetCoordinates(pt(s), eta(s), phi(s), E(s));
    return p;
  }
  // ****************************************************


  float Particle::getF(const string_t & mName)const {return get<float>(mName);}
  float Particle::getI(const string_t & mName)const {return get<int>(mName);}
  const std::vector<float>& Particle::getVecF(const string_t & mName)const {return get<std::vector<float>&>(mName);}
  const std::vector<int>&   Particle::getVecI(const string_t & mName)const {return get<std::vector<int>&>(mName);}


  Particle& Particle::setPtEtaPhiM( Particle::Scalar a, Particle::Scalar b, Particle::Scalar c, Particle::Scalar d ){
    ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> > v(a,b,c,d);
    this->LorentzVector_t::operator=(v);
    updateKinematics();
    return *this;
  }
  Particle& Particle::setEEtaPhiM( Particle::Scalar e, Particle::Scalar eta, Particle::Scalar phi, Particle::Scalar m ){
    double pt = std::sqrt( e*e - m*m) / std::cosh( eta ); // not really safe.
    return setPtEtaPhiE(pt, eta, phi, e);
  }


}
