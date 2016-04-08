/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetAnalysisEDM/Particle.h"
#include "JetAnalysisEDM/AttributeMap.h"



#include "JetAnalysisEDM/VarHandleMap.h"

#include "JetAnalysisEDM/Particle.icc"
#include "JetAnalysisEDM/VarHandleMap.icc"

inline void _donothing(){} // just here to allow emacs to correctly autoindent
#define FOREACH_MAP( code ) for(size_t i=0;i<m_maps.size();i++){VarHandleMapBase* m=m_maps[i]; code ;} _donothing()

namespace JetAnalysisEDM {

  AttributeMap::AttributeMap( ):
    m_buildVarsCalled(false)
  {
    
  }
  
  void AttributeMap::writeTo(TTree* tree){
    FOREACH_MAP( m->writeTo(tree) ) ;
  }
  void AttributeMap::setOutputVar(bool active, const string_t & pattern ) {
    FOREACH_MAP( m->setOutputVar(active, pattern) ) ;
  }
  void AttributeMap::readActive( ) {
    FOREACH_MAP( m->readActive() ) ;
  }

  void AttributeMap::buildVars( ) {
    if( m_buildVarsCalled) return;
    FOREACH_MAP( m->buildVars() ) ;
    m_buildVarsCalled = true;
  }

  void AttributeMap::clear( ) {
    FOREACH_MAP( m->clear() ) ;
  }
  void AttributeMap::setParameters(const string_t prefix, const ::Long64_t* master) {
    FOREACH_MAP( m->setParameters(prefix,master); ) ;
  }
  void AttributeMap::setPrefix(const string_t &prefix) {
    FOREACH_MAP( m->setPrefix(prefix); ) ;
  }
  void AttributeMap::readFrom(TTree* tree){
     FOREACH_MAP( m->readFrom(tree) ) ;
  }



  // ************************************************************
  // ************************************************************
  // VectorAttributeMap 
  // ************************************************************
  
  VectorAttributeMap::VectorAttributeMap( ParticleContainer* c): 
    AttributeMap(),
    m_intMap(this),
    m_floatMap(this),
    m_boolMap(this),
    m_vecintMap(this),
    m_vecfloatMap(this),
    m_particleMap(this),
    m_particleContMap(this),
    m_container(c)
  {
    m_maps.reserve(5);

    m_maps.push_back( &m_intMap) ;
    m_maps.push_back( &m_boolMap) ;
    m_maps.push_back( &m_floatMap) ;
    m_maps.push_back( &m_vecintMap) ;
    m_maps.push_back( &m_vecfloatMap) ;
    
  }
  

  
  string_t VectorAttributeMap::prefix(){return m_container->getPrefix();}
  const ::Long64_t* VectorAttributeMap::master(){return m_container->getMaster();}
  TTree* VectorAttributeMap::inputTree(){return m_container->getInputTree();}
  
  void VectorAttributeMap::forwardError(const char* f, const char* m){
    m_container->Error( f, m);
  }

  void VectorAttributeMap::swap(VectorAttributeMap& other){
    m_intMap.swap(other.m_intMap) ;
    m_floatMap.swap(other.m_floatMap) ;
    m_boolMap.swap(other.m_boolMap) ;
    m_vecintMap.swap(other.m_vecintMap);
    m_vecfloatMap.swap(other.m_vecfloatMap);
    m_particleMap.swap(other.m_particleMap) ;
    m_particleContMap.swap(other.m_particleContMap) ;    
  }

#define ADDASIN( typ ) m_##typ##Map.addVarAsIn(oMap.m_##typ##Map, pattern, writeOut) 

  void VectorAttributeMap::addVarAsIn(VectorAttributeMap &oMap, const string_t& pattern , bool writeOut){
    ADDASIN( int );
    ADDASIN( bool );
    ADDASIN( float );
    ADDASIN( vecint );
    ADDASIN( vecfloat );
  }

#define COPYMAP( typ ) m_##typ##Map.copyFrom(oMap.m_##typ##Map, source, dest) 
  void VectorAttributeMap::copyFrom( VectorAttributeMap & oMap, size_t source, size_t dest){
    COPYMAP( int );
    COPYMAP( bool );
    COPYMAP( float );
    COPYMAP( vecint );
    COPYMAP( vecfloat );
    m_particleMap.copyFrom(oMap.m_particleMap, source , dest);
    m_particleContMap.copyFrom(oMap.m_particleContMap, source , dest);
  }
#define COPYMAPV( typ ) m_##typ##Map.copyFrom(oMap.m_##typ##Map, source) 
  
  void VectorAttributeMap::copyFrom( VectorAttributeMap & oMap, vector<int> & source){
    COPYMAPV( int );
    COPYMAPV( bool );
    COPYMAPV( float );
    COPYMAPV( vecint );
    COPYMAPV( vecfloat );
    m_particleMap.copyFrom(oMap.m_particleMap, source);
    m_particleContMap.copyFrom(oMap.m_particleContMap, source);

  }
#undef COPYMAP
#undef COPYMAPV

#define COPYFROMPART( typ )  m_##typ##Map.copyFrom(p, destIndex) 
  void VectorAttributeMap::copyFrom(const Particle *p, size_t destIndex){
    COPYFROMPART( int );
    COPYFROMPART( bool );
    COPYFROMPART( float );
    COPYFROMPART( vecint );
    COPYFROMPART( vecfloat );    
  }
#undef COPYFROMPART

  VarHandleBase* VectorAttributeMap::buildNewVarFromTypeName(string_t varName, string_t typeName){
#define CHECK_TYPE( typ ) if( (typeName == "vector<"#typ">") || (typeName == "vector<"#typ" >") ) return addVar< typ >(varName, false);
    CHECK_TYPE( int ) ;
    CHECK_TYPE( bool ) ;
    CHECK_TYPE( float ) ;
    CHECK_TYPE( vector<int>  ) ;
    CHECK_TYPE( vector<float>  ) ;

    return NULL;
  }
#undef CHECK_TYPE

  void VectorAttributeMap::resizeNonInputVar(size_t n){
    FOREACH_MAP( m->resizeVars(n)) ;
    m_particleMap.resizeVars(n);
    m_particleContMap.resizeVars(n);
  }  

#define GETMAP_IMPL( typ ) template<> VarHandleMap< vector< typ > >* VectorAttributeMap::getMap< typ >()   { return &m_##typ##Map;}
#define GETMAP_IMPL_CONST( typ ) template<> const VarHandleMap< vector< typ > >* VectorAttributeMap::getMap< typ >()  const { return &m_##typ##Map;}
  
  
#define MACRO_EACH_TYPE( MAC ) MAC(int) MAC(float) MAC(bool) MAC(vecint) MAC(vecfloat) 
  MACRO_EACH_TYPE( GETMAP_IMPL ) 
  MACRO_EACH_TYPE( GETMAP_IMPL_CONST ) 
  template<>
  vector<Particle*>& VectorAttributeMap::get<Particle*>(const string_t& varName, bool & valid ) {return m_particleMap.get(varName,valid);}
  template<>
  vector<ParticleContainer*>& VectorAttributeMap::get<ParticleContainer*>(const string_t& varName, bool & valid ) {return m_particleContMap.get(varName,valid);}
  template<>
  vector<Particle*>& VectorAttributeMap::getNonInputVar<Particle*>(const string_t& varName, bool & valid ){ return get<Particle*>(varName,valid); }
  template<>
  vector<ParticleContainer*>& VectorAttributeMap::getNonInputVar<ParticleContainer*>(const string_t& varName, bool & valid ){ return get<ParticleContainer*>(varName,valid); }
  template<>
  void VectorAttributeMap::buildVar<Particle*>(string_t varName, bool outputVar){m_particleMap[varName];}
  template<>
  void VectorAttributeMap::buildVar<ParticleContainer*>(string_t varName, bool outputVar){m_particleContMap[varName];}
  template<>
  void VectorAttributeMap::fillListOfKeys<ParticleContainer*>(std::vector<string_t>& vs) const { m_particleContMap.fillListOfKeys(vs); }
  template<>
  void VectorAttributeMap::fillListOfKeys<Particle*>(std::vector<string_t>& vs) const { m_particleMap.fillListOfKeys(vs); }
  
#undef GETMAP_IMPL
#undef GETMAP_IMPL_CONST
#undef MACRO_EACH_TYPE





  // ************************************************************
  // ************************************************************
  // SimpleAttributeMap 
  // ************************************************************


  SimpleAttributeMap::SimpleAttributeMap( Particle* c, const string_t &prefix): 
    AttributeMap(),
    m_intMap(this),
    m_floatMap(this),
    m_boolMap(this),
    m_vecintMap(this),
    m_vecfloatMap(this),
    m_particleMap(this),
    m_particleContMap(this),
    m_part(c),
    m_prefix(prefix),
    m_master(NULL),
    m_inputTree(NULL)
  {
    m_maps.reserve(5);

    m_maps.push_back( &m_intMap) ;
    m_maps.push_back( &m_boolMap) ;
    m_maps.push_back( &m_floatMap) ;
    m_maps.push_back( &m_vecintMap) ;
    m_maps.push_back( &m_vecfloatMap) ;
    m_maps.push_back( &m_particleMap) ;
    m_maps.push_back( &m_particleContMap) ;
    
  }
  
  
  
  string_t SimpleAttributeMap::prefix(){return m_prefix;}
  const ::Long64_t* SimpleAttributeMap::master(){return m_master;}
  TTree* SimpleAttributeMap::inputTree(){return m_inputTree;}
  
  void SimpleAttributeMap::setParameters(const string_t prefix, const ::Long64_t* master) {
    m_prefix = prefix; m_master = master;
    AttributeMap::setParameters(prefix,master);
  }
  void SimpleAttributeMap::setPrefix(const string_t & prefix){
    m_prefix = prefix;
    AttributeMap::setPrefix(prefix);
  }
  void SimpleAttributeMap::readFrom(TTree* tree){
    m_inputTree = tree;
    AttributeMap::readFrom(tree);
  }


  void SimpleAttributeMap::forwardError(const char* f, const char* m){
    //m_container->Error( f, m);
  }



#define ADDASIN( typ ) m_##typ##Map.addVarAsIn(oMap.m_##typ##Map, pattern, writeOut) 

  void SimpleAttributeMap::addVarAsIn(SimpleAttributeMap &oMap, const string_t& pattern , bool writeOut){
    ADDASIN( int );
    ADDASIN( bool );
    ADDASIN( float );
    ADDASIN( vecint );
    ADDASIN( vecfloat );
  }

#define COPYFROMPART( typ )  m_##typ##Map.copyFrom(p) 
  void SimpleAttributeMap::copyFrom(const Particle *p){
    COPYFROMPART( int );
    COPYFROMPART( bool );
    COPYFROMPART( float );
    COPYFROMPART( vecint );
    COPYFROMPART( vecfloat );    
  }
#undef COPYFROMPART

  VarHandleBase* SimpleAttributeMap::buildNewVarFromTypeName(string_t varName, string_t typeName){
#define CHECK_TYPE( typ ) if( (typeName == "vector<"#typ">") || (typeName == "vector<"#typ" >") ) return addVar< typ >(varName);
    CHECK_TYPE( int ) ;
    CHECK_TYPE( bool ) ;
    CHECK_TYPE( float ) ;
    CHECK_TYPE( vector<int>  ) ;
    CHECK_TYPE( vector<float>  ) ;

    return NULL;
  }
#undef CHECK_TYPE



  void SimpleAttributeMap::updateKinematics(){
    if(!getHandle<float>("eta")) return ; // no kinematic to update.
    (*getHandle<float>("eta"))() = m_part->eta();
    (*getHandle<float>("phi"))() = m_part->phi();
    (*getHandle<float>("pt"))() = m_part->pt();
    (*getHandle<float>("E"))() = m_part->E();
  }

#define GETMAP_IMPL( typ ) template<> VarHandleMap< typ  >* SimpleAttributeMap::getMap< typ >()  { return &m_##typ##Map;}
#define GETMAP_IMPL_CONST( typ ) template<> const VarHandleMap< typ  >* SimpleAttributeMap::getMap< typ >()  const { return &m_##typ##Map;}


#define MACRO_EACH_TYPE( MAC ) MAC(int) MAC(float) MAC(bool) MAC(vecint) MAC(vecfloat) 
  MACRO_EACH_TYPE( GETMAP_IMPL )
  MACRO_EACH_TYPE( GETMAP_IMPL_CONST )

  template<>
  Particle*& SimpleAttributeMap::get<Particle*>(const string_t& varName, bool & valid ){return m_particleMap.get(varName,valid);}
  template<>
  ParticleContainer*& SimpleAttributeMap::get<ParticleContainer*>(const string_t& varName, bool & valid ){return m_particleContMap.get(varName, valid);}
  template<>
  Particle*& SimpleAttributeMap::getOrCreate<Particle*>(const string_t& varName ){return m_particleMap[varName];}
  template<>
  ParticleContainer*& SimpleAttributeMap::getOrCreate<ParticleContainer*>(const string_t& varName){return m_particleContMap[varName];}

  template<>
  void SimpleAttributeMap::fillListOfKeys<ParticleContainer*>(std::vector<string_t>& vs) const { m_particleContMap.fillListOfKeys(vs); }
  template<>
  void SimpleAttributeMap::fillListOfKeys<Particle*>(std::vector<string_t>& vs) const {  m_particleMap.fillListOfKeys(vs); }


  template<>
  void SimpleAttributeMap::dump<Particle*>(){
    m_particleMap.dump();
  }
  template<>
  void SimpleAttributeMap::dump<ParticleContainer*>(){
    m_particleContMap.dump();
  }

#undef GETMAP_IMPL
#undef GETMAP_IMPL_CONST
#undef MACRO_EACH_TYPE



}

