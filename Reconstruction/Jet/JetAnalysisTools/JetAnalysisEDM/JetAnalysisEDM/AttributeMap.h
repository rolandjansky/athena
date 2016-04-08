// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETANALYSISEDM_ATTRIBUTEMAP_H
#define JETANALYSISEDM_ATTRIBUTEMAP_H

#include "VarHandleMap.h"


using std::vector;
namespace JetAnalysisEDM {

  //***********************************************************
  //***********************************************************
  typedef vector<int>   vecint;
  typedef vector<float> vecfloat;
  typedef vector<bool>  vecbool;

  class AttributeMap {
  public:


    AttributeMap();
    virtual ~AttributeMap(){};
    void writeTo(TTree* tree);
    void setOutputVar(bool active, const string_t& pattern ) ;
    void readActive();
    void clear();
    void setParameters(const string_t prefix, const ::Long64_t* master) ;
    void setPrefix(const string_t & prefix);
    void buildVars();
    void readFrom(TTree* tree);

    //VarHandleBase* buildNewVarFromTypeName(string_t varName, string_t typeName);

    virtual string_t     prefix() {return "";} 
    virtual const ::Long64_t * master() {return NULL;}
    virtual ::TTree*     inputTree()  {return NULL;}
    virtual void forwardError(const char* , const char* ){}



    vector< VarHandleMapBase* > m_maps;

    bool m_buildVarsCalled;

  };

#ifndef __CINT__

  class VectorAttributeMap : public AttributeMap{
  public:
    
    VectorAttributeMap(ParticleContainer * c);


    virtual string_t     prefix();
    virtual const ::Long64_t * master();
    virtual ::TTree*     inputTree();
    virtual void forwardError(const char* f, const char* m);

    virtual void swap(VectorAttributeMap& other);

    template<typename T>
    vector<T> & get(const string_t& varName, bool & valid){
      VarHandle< vector<T> >* v = getHandle<T>(varName);
      valid=true;
      if( v) return (*v)();
      valid=false;
      return VarHelperDummy< vector<T> >::get();
    }

    template<typename T>
    vector<T> & getNonInputVar(const string_t& varName, bool & valid){
      VarHandle< vector<T> >* v = getHandle<T>(varName);
      valid=true;
      if( (v) && (!v->fromInput())) return (*v)();
      valid=false;
      return VarHelperDummy< vector<T> >::get();
    }

  

    template<typename T>
    VarHandle< vector<T> >* getHandle(string_t varName)  { 
      return getMap<T>()->request( varName) ;
    }
    template<typename T>
    const VarHandle< vector<T> >* getHandle(string_t varName) const { 
      return const_cast<VectorAttributeMap*>(this)->getMap<T>()->request( varName) ;
    }

    template<typename T>
    VarHandle< vector<T> >* addVar(string_t varName, bool outputVar){ 
      return getMap<T>()->addVar( varName, outputVar) ;
    }

    template<typename T>
    void buildVar(string_t varName, bool outputVar){ 
      addVar<T>( varName, outputVar) ;
    }

    template<typename T>
    void fillListOfKeys(std::vector<string_t>& vs) const {
      getMap<T>()->fillListOfKeys(vs);
    }

    template<typename T>
    inline VarHandleMap< vector<T> >* getMap() {return NULL;}

    template<typename T>
    inline const VarHandleMap< vector<T> >* getMap() const {return NULL;}

    
    VarHandleBase* buildNewVarFromTypeName(string_t varName, string_t typeName);
    void addVarAsIn(VectorAttributeMap &other, const string_t& pattern , bool writeOut);
    void copyFrom( VectorAttributeMap & oMap, size_t pSource, size_t pDest);
    void copyFrom( VectorAttributeMap & oMap, vector<int> & vSource);
    void copyFrom(const Particle*p, size_t destIndex);

    void resizeNonInputVar(size_t n);

    VarHandleMap< vector<int> > m_intMap;
    VarHandleMap< vector<float> > m_floatMap;
    VarHandleMap< vector<bool> > m_boolMap;

    VarHandleMap< vector<vecint> > m_vecintMap;
    VarHandleMap< vector<vecfloat> > m_vecfloatMap;

    GenericMap< vector<Particle*> > m_particleMap;
    GenericMap< vector<ParticleContainer*> > m_particleContMap;

    ParticleContainer * m_container;
  };


  // Declare specialization of the getMap methods for each type :
#define MACRO_EACH_TYPE( MAC ) MAC(int) MAC(float) MAC(bool) MAC(vecint) MAC(vecfloat) 
#define GETMAP_IMPL( typ )  template<> VarHandleMap< vector< typ > >* VectorAttributeMap::getMap< typ >() ; 
#define GETMAP_IMPL_CONST( typ )  template<> const VarHandleMap< vector< typ > >* VectorAttributeMap::getMap< typ >() const ; 

MACRO_EACH_TYPE( GETMAP_IMPL )
MACRO_EACH_TYPE( GETMAP_IMPL_CONST )

template<>
vector<Particle*>& VectorAttributeMap::get<Particle*>(const string_t& varName, bool & valid );
template<>
vector<ParticleContainer*>& VectorAttributeMap::get<ParticleContainer*>(const string_t& varName, bool & valid );
template<>
vector<Particle*>& VectorAttributeMap::getNonInputVar<Particle*>(const string_t& varName, bool & valid );
template<>
vector<ParticleContainer*>& VectorAttributeMap::getNonInputVar<ParticleContainer*>(const string_t& varName, bool & valid );
template<>
void VectorAttributeMap::buildVar<Particle*>(string_t varName, bool outputVar);
template<>
void VectorAttributeMap::buildVar<ParticleContainer*>(string_t varName, bool outputVar);
template<> void VectorAttributeMap::fillListOfKeys<ParticleContainer*>(std::vector<string_t>& vs) const ;
template<> void VectorAttributeMap::fillListOfKeys<Particle*>(std::vector<string_t>& vs) const ;

#undef GETMAP_IMPL
#undef GETMAP_IMPL_CONST
#undef MACRO_EACH_TYPE



  class SimpleAttributeMap : public AttributeMap{
  public:
    
    SimpleAttributeMap(Particle * c, const string_t& prefix="");


    virtual string_t     prefix();
    virtual const ::Long64_t * master();
    virtual ::TTree*     inputTree();
    virtual void forwardError(const char* f, const char* m);


    void setParameters(const string_t prefix, const ::Long64_t* master) ;
    void setPrefix(const string_t & prefix);
    void readFrom(TTree* tree);


    template<typename T>
    T& get(const string_t & varName, bool &valid){
      VarHandle< T >* v = getHandle<T>(varName);
      valid=true;
      if( v) return (*v)();
      valid=false;
      return VarHandle<T>::dummyValue();
    }

    template<typename T>
    T& getOrCreate(const string_t & varName){
      VarHandle< T >* v = addVar<T>(varName, false);
      return (*v)();
    }


    template<typename T>
    VarHandle< T >* getHandle(string_t varName)  { 
      return getMap<T>()->request( varName) ;
    }
    template<typename T>
    const VarHandle< T >* getHandle(string_t varName) const { 
      return const_cast<SimpleAttributeMap*>(this)->getMap<T>()->request( varName) ;
    }

    template<typename T>
    VarHandle< T >* addVar(string_t varName, bool active=true){ 
      return getMap<T>()->addVar( varName, active) ;
    }

    template<typename T>
    inline VarHandleMap< T > *getMap()  {return NULL;}

    template<typename T>
    inline const VarHandleMap< T > *getMap() const {return NULL;}

    template<typename T>
    void fillListOfKeys(std::vector<string_t>& vs) const {
      getMap<T>()->fillListOfKeys(vs);
    }


    template<typename T>
    void dump(){
      std::cout << " Dumping a map of "<< this << std::endl;
      getMap<T>()->dump();
    }


    VarHandleBase* buildNewVarFromTypeName(string_t varName, string_t typeName );
    void addVarAsIn(SimpleAttributeMap &other, const string_t& pattern , bool writeOut);

    void copyFrom(const Particle*p);

    VarHandleMap< int > m_intMap;
    VarHandleMap< float > m_floatMap;
    VarHandleMap< bool > m_boolMap;
    
    VarHandleMap< vector<int> > m_vecintMap;
    VarHandleMap< vector<float> > m_vecfloatMap;

    GenericMap< Particle* > m_particleMap;
    GenericMap< ParticleContainer* > m_particleContMap;

    Particle *m_part;

    void updateKinematics();
    string_t m_prefix;
    const ::Long64_t *m_master;
    TTree* m_inputTree;
    //TTree* m_outputTree;
    
  };


  // Declare specialization of the getMap method for each type :
#define MACRO_EACH_TYPE( MAC ) MAC(int) MAC(float) MAC(bool) MAC(vecint) MAC(vecfloat) 
#define GETMAP_IMPL( typ )  template<> VarHandleMap< typ  >* SimpleAttributeMap::getMap< typ >() ; 
#define GETMAP_IMPL_CONST( typ )  template<> const VarHandleMap< typ  >* SimpleAttributeMap::getMap< typ >() const ; 

 MACRO_EACH_TYPE( GETMAP_IMPL )
 MACRO_EACH_TYPE( GETMAP_IMPL_CONST )

 template<>
 Particle*& SimpleAttributeMap::get<Particle*>(const string_t& varName, bool & valid );
 template<>
 ParticleContainer*& SimpleAttributeMap::get<ParticleContainer*>(const string_t& varName, bool & valid );
 template<>
 Particle*& SimpleAttributeMap::getOrCreate<Particle*>(const string_t& varName );
 template<>
 ParticleContainer*& SimpleAttributeMap::getOrCreate<ParticleContainer*>(const string_t& varName);
  template<>
  void SimpleAttributeMap::dump<Particle*>();  
  template<>
  void SimpleAttributeMap::dump<ParticleContainer*>();
template<> void SimpleAttributeMap::fillListOfKeys<ParticleContainer*>(std::vector<string_t>& vs) const ;
template<> void SimpleAttributeMap::fillListOfKeys<Particle*>(std::vector<string_t>& vs) const ;

#undef GETMAP_IMPL
#undef GETMAP_IMPL_CONST
#undef MACRO_EACH_TYPE


#endif // exclude from CINT

}

#endif
