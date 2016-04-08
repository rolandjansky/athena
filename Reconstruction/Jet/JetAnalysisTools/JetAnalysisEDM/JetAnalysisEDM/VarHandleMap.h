// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETANALYSISEDM_VARHANDLEMAP_H
#define JETANALYSISEDM_VARHANDLEMAP_H

#include <string>
#include <vector>

#include "VarHandle.h"
//#include "Particle.h"
#include "TPRegexp.h"

/////////////////////////////////////////////////////////////////////
/// This file defines maps of variables of a given type.
///  They hold the actual map<string, var_type> object containing the variable.
///  They provide interfaces 
///       - to pass information to their contained variable (input tree, prefix, output tree,...)
///       - to retrieve variables, build them if necessary, clear
///
///////////////////////////////////////////////////////////////////


using std::vector;
namespace JetAnalysisEDM {
  
  class Particle;
  class ParticleContainer;
  class AttributeMap;

  ///////////////////////////////////////////////////////////////
  /// \class VarHandleMapBase
  /// 
  ///  The base class for map of variable of a given type
  ///  defines interface.
  ///  Holds a pointer to the generic parent AttributeMap 
  ///////////////////////////////////////////////////////////////
  struct VarHandleMapBase {
    //    typedef std::map< string_t , size_t > handlemap_t;

    VarHandleMapBase(AttributeMap *c): m_attributeMap(c){}
    virtual ~VarHandleMapBase(){}

    virtual void writeTo(TTree* /*tree*/){}
    virtual void setOutputVar(bool /*active*/, const string_t & /*pattern*/ ) {}
    //virtual void addVar(bool active, const string_t & pattern ) {std::cout<< "bla "<< std::endl;}
    virtual void readActive(){}
    virtual void clear() {}
    virtual void setParameters(const string_t& /*prefix*/, const ::Long64_t* /*master*/) {}
    virtual void setPrefix(const string_t& /*prefix*/){}
    virtual void readFrom(TTree* /*tree*/){}
    virtual void copyFrom(const Particle* /*p*/, size_t /*destIndex*/) {}
    virtual void buildVars(){}
    virtual void resizeVars(size_t ){}

    string_t     prefix();
    const ::Long64_t * master();
    ::TTree*     inputTree();
    void forwardError(const char* f, const char* m);
    
        
    static const size_t invalid = size_t(-1);

    AttributeMap* m_attributeMap;
    
  };

  // exclude CINT seems unable to understand what is below.
#ifndef __CINT__    

  //***********************************************************
  //***********************************************************
#define VARHMAP_LOOP( code )    \
  iterator_t itr = m_handleMap.begin();   \
  iterator_t end = m_handleMap.end();     \
  for( ; itr != end; ++itr ) { var_t* v = itr->second; code ;}


  ///////////////////////////////////////////////////////////////
  /// \class VarHandleMap<T>
  ///
  /// A wrapper around map< string, VarHandle<T>* >
  /// The class is a map of pointer to VarHandle<T>. Pointers are void by default. 
  ///  Provides methods to add, pointers, instanciate object, copy efficiently values from a map to an other.
  /////////////////////////////////////////////////////////////////////
  template <typename Type>
  struct VarHandleMap : public VarHandleMapBase {
    typedef VarHandle<Type> var_t;
    typedef std::map< string_t , var_t* > HandleMap_t;
    typedef typename HandleMap_t::iterator  iterator_t;
    typedef typename HandleMap_t::const_iterator  const_iterator_t;

    //typedef std::vector< VarHandle<Type> * >  handlestore_t ;

    VarHandleMap(AttributeMap *c) : VarHandleMapBase(c) {}
    virtual ~VarHandleMap();
    

    bool hasVar(string_t varName){ return m_handleMap.find( varName) != m_handleMap.end(); }
    var_t * addVar( const string_t & varName, bool outputVar=false) {
      iterator_t it = m_handleMap.find( varName);
      if( it == m_handleMap.end() ) {
        var_t * v = new var_t(  prefix()+varName, master()); // don't set input tree
        m_handleMap[ varName ] = v;
        v->setOutputVar(outputVar);
        if(! outputVar  ) {
          v->build(); // if active, it will be build when writeTo is called
          //std::cout << " build explicitly "<< varName << std::endl;
        }
        return v;
      }
      return it->second;
    }

    void addVarAsIn(VarHandleMap &otherMap, const string_t & pattern , bool writeOut){
      ::TPRegexp re( pattern );

      iterator_t itr = otherMap.m_handleMap.begin();
      iterator_t end = otherMap.m_handleMap.end();
      for( ; itr != end; ++itr ) {
        TString name = itr->first;
        if( ! re.Match( name ) ) continue;
        var_t * v = addVar(itr->first);
        v->setOutputVar(writeOut);
        if(! writeOut ) v->build(); // if active, it will be build when writeTo is called
      }
    }
    
    var_t * request(const string_t& varName, bool createIfMissing )  {
      if(createIfMissing) return addVar(varName, true);
      iterator_t it = m_handleMap.find( varName);      
      if( it == m_handleMap.end() ) { return NULL ; }
      return it->second;
    }
    var_t * request(const string_t& varName) const {
      typename HandleMap_t::const_iterator it = m_handleMap.find( varName);      
      if( it == m_handleMap.end() ) { return NULL ; }
      return it->second;
    }
    
    // void insertVar(const string_t& varName, var_t* var){
    //   iterator_t it = m_handleMap.find( varName);
    //   if( it != m_handleMap.end() ) { forwardError("insertVar", ("variable  already in : "+varName).c_str() );return;}
    //   m_handleMap[varName] = var;
    // }

    void fillListOfKeys(std::vector<string_t> &vs) const {
      const_iterator_t it = m_handleMap.begin();
      const_iterator_t itE = m_handleMap.end();
      for( ; it!=itE; ++it) vs.push_back(it->first);
    }

    void dump(){
      iterator_t it = m_handleMap.begin();
      iterator_t itE = m_handleMap.end();
      std::cout << " Dumping map "<< this<< std::endl;
      for( ; it!=itE; ++it){
        std::cout<< "           " << it->first << "  : ?" << std::endl;
      }
      std::cout << " done dumping map "<< this << std::endl;
    }
    

    /// Copy values from oMap. Has effect only for map of vectors.
    ///  For each key of this, values at index pSource are copied to this map at index pDest
    void copyFrom( VarHandleMap & oMap, size_t pSource, size_t pDest);

    /// Copy values from oMap. Has effect only for map of vectors
    ///  For each key of this, values at indices vSource[i]  are copied to this map at index i for all i.     
    void copyFrom( VarHandleMap & oMap, vector<int> & vSource);

    /// Copy attributes from Particle p. Has effect only for map of vectors
    ///  For each key of this, values existing in P are copied to this map at index destIndex
    virtual void copyFrom(const Particle*p, size_t destIndex);

    /// Copy attributes from Particle p. 
    ///  For each key of this, values existing in P are copied to this.
    virtual void copyFrom(const Particle*p);

    /// Connect this map variables to the tree. Only connect the vars declared as output variables (see setOutputVar)
    virtual void writeTo(TTree* tree){
      VARHMAP_LOOP( v->writeTo(tree, false) ) // don't forceOutputVar, connect only already declared
    }

    /// Instantiate pointer to contained variables.
    virtual void buildVars(){
      VARHMAP_LOOP( v->build() )
    }
    
    /// Has effect only for map of vectors. Resize all contained vector, which are not input variable, to n
    virtual void resizeVars(size_t n){
      VARHMAP_LOOP( if(!v->fromInput() ) {v->build() ; v->resize(n);} )
    }

    /// Flag (or unflag if active=false) contained variables (those which key matches pattern) as output variables.
    virtual void setOutputVar(bool active, const string_t & pattern ) {
      if( (active) &&  !inputTree()) {forwardError("setOutputVar","No input tree. Can not set active branches"); return;}

      ::TPRegexp re( pattern );
      
      iterator_t itr = m_handleMap.begin();
      iterator_t end = m_handleMap.end();
      for( ; itr != end; ++itr ) {
        TString name = itr->first;
        if( ! re.Match( name ) ) continue;
        itr->second->setOutputVar(active);
      }
     }


    /// Force reading variables from input tree. 
    ///  This is done only for output var : the purpose of this call is to ensure they are in memory so that 
    ///  the outputTree->Fill() call really saves the content in the output tree. 
    virtual void readActive(){
      VARHMAP_LOOP( if(v->outputVar()) v->readCurrentEntry() )
    }

    virtual void clear(){
      VARHMAP_LOOP(v->clear())
    }

    virtual void setParameters(const string_t& prefix_, const ::Long64_t* masterIndex){
      VARHMAP_LOOP(v->setName(prefix_+itr->first); v->setMaster(masterIndex))
    }

    virtual void setPrefix(const string_t & prefix_){
      VARHMAP_LOOP( v->setName(prefix_+itr->first) )
    }
    virtual void readFrom(TTree* tree){
      VARHMAP_LOOP( v->readFrom(tree) )
    }

    void swap(VarHandleMap &other) { m_handleMap.swap(other.m_handleMap);}

    HandleMap_t     m_handleMap;

  };


  ///////////////////////////////////////////////////////////////
  /// \class GenericMap<T>
  ///
  ///  A wrapper around map<string_t, T>.
  ///   This class has the same property of VarHandleMap but concerns attributes which
  ///   do not need I/O (such as Particle* ParticleContainer*, vector<Particle*>). 
  ///   These attributes do not need to be encapsulated into VarHandle<>. 
  ///   This map provides more direct and simple access to the attributes.
  ///////////////////////////////////////////////////////////////
  template <typename Type>
  struct GenericMap : public VarHandleMapBase {
  public:
    typedef Type var_t;
    typedef std::map< string_t , var_t > HandleMap_t;
    typedef typename HandleMap_t::iterator  iterator_t;
    typedef typename HandleMap_t::const_iterator  const_iterator_t;

    GenericMap(AttributeMap *c) : VarHandleMapBase(c) {}
    
    bool hasVar(string_t varName){ return m_ptrMap.find( varName) != m_ptrMap.end(); }

    var_t & get(const string_t& varName, bool& valid) {
      iterator_t it = m_ptrMap.find(varName);
      valid = (it != m_ptrMap.end() ); 
      return valid  ?  it->second : VarHelperDummy<var_t>::get()  ;
    }

    var_t & operator[](const string_t& varName){
      return m_ptrMap[varName];
    }

    virtual void clear() { m_ptrMap.clear();}

    /// Has an effect only if it Type is some vector<> type. Resizes all contained vector to n.
    void resizeVars(size_t n) ;


    void copyFrom( GenericMap & oMap, size_t pSource, size_t pDest);   
    void copyFrom( GenericMap & oMap, vector<int> & vSource);    
    virtual void copyFrom(const Particle*p, size_t destIndex);
    virtual void copyFrom(const Particle*p);
    
    void fillListOfKeys(std::vector<string_t>& vs) const {
      const_iterator_t it = m_ptrMap.begin();
      const_iterator_t itE = m_ptrMap.end();
      for( ; it!=itE; ++it) vs.push_back(it->first);
    }


    void dump(){
      iterator_t it = m_ptrMap.begin();
      iterator_t itE = m_ptrMap.end();
      std::cout << " Dumping map "<< this<< std::endl;
      for( ; it!=itE; ++it){
        std::cout<< "           " << it->first << "  : "<< it->second << std::endl;
      }
      std::cout << " done dumping map "<< this << std::endl;
    }


    void set(const string_t & varName, var_t* ptr){ m_ptrMap[varName] = ptr; }
    void swap(GenericMap &other) { m_ptrMap.swap(other.m_ptrMap);}

  protected:
    HandleMap_t m_ptrMap;
  };

#endif // end CINT exlcusion
  
}
#endif
