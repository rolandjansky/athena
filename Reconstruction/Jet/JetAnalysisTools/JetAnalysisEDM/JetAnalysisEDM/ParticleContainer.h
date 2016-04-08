// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETANALYSISEDM_PARTICLE_H
// Include first Particle.h 
#include "JetAnalysisEDM/Particle.h"
#endif
#ifndef JETANALYSISEDM_PARTICLECONTAINER_H
#define JETANALYSISEDM_PARTICLECONTAINER_H


#include <vector>
#include <set>
#include <string>


using std::vector;

#include <TObject.h>
#include <TString.h>

#include "VarHandle.h"
#include "AttributeMap.h"

#include <algorithm>

class TTree ;

namespace fastjet {
  class ClusterSequence ;
}

namespace JetAnalysisEDM {
  
  class VectorAttributeMap;
  class Particle;
  template<typename T> class ParticleAccessor;

  
  class ParticleContainer : public TObject {
    
    friend class Particle;
  public:

    typedef std::vector<Particle*>::value_type value_type; 
    typedef std::vector<Particle*>::iterator   iterator;    
    typedef std::vector<Particle*>::const_iterator   const_iterator;


    /// Default Constructor
    ParticleContainer();
    /// Constructor used when reading from a TTree
    ParticleContainer( const string_t & name, const ::Long64_t& master, TTree* tree = 0 );

    /// Constructor when the object is only used for writing data out
    ParticleContainer( const string_t name );
    /// Destructor
    ~ParticleContainer();

    virtual void setParameters(const string_t name, const ::Long64_t* master) const ;

    /// get this container name
    virtual string_t getName() const ;
    /// set this container name
    virtual void setName(const string_t& name) const ;


    ::TTree* getInputTree(){return m_inputTree;}

    const ::Long64_t * getMaster(){return m_master;}
    virtual void setMaster(::Long64_t  * master);

    /// Connect the object to an input TTree
    virtual bool readFrom( ::TTree* tree, bool quietMode=false ) const;
    /// Connect the object to an output TTree
    virtual void writeTo( ::TTree* tree ) const ;

    /// Turn (selected) branches either on or off
    virtual void setOutputVar( ::Bool_t active = kTRUE,
                               string_t pattern = ".*" ) const ;
    /// Read in all the variables that we need to write out as well
    virtual void readAllActive() const ;
    

    /// clear the container. Useful when writing new branches.
    void clear( Option_t* option = "" );
    /// Add one element to an output collection    
    bool push_back(  Particle* el );
    /// Add one element to an output collection    
    bool Add(  Particle* el );

    Particle* addNewFromSource(const Particle* source);    


    /// Access a proxy class describing one element of the container
    Particle* operator[]( size_t index );
    /// Access a proxy class describing one element of the container (constant version)
    const Particle* operator[]( size_t index ) const;

    /// Access a proxy class describing one element of the container
    Particle* at( size_t index );
    /// Access a proxy class describing one element of the container (constant version)
    const Particle* at( size_t index ) const;
    
    


    const_iterator begin() const;
    iterator       begin()  ;

    const_iterator end() const ;
    iterator       end() ;
    
    /// size of the container
    size_t size() const ;

    /// Make this container a view on an other
    bool setSubsetOfInputContainer(const ParticleContainer* c);    
    

    void usePtEtaPhiM(bool v=true) const {m_inputUsesMass=v;}
    void declareKinematicVar(const string_t& pt, const string_t& eta, const string_t& phi, const string_t& E  ) const ;
    void forceInputMass(float m) const {m_forcedMass = m;}

    template<typename T>
    void declareOutputVar(string_t varName, bool writeOut=true) const {
       m_attributes.buildVar<T>( varName, writeOut);
    }

    void declareOutputVarAsIn(const ParticleContainer* p, const string_t & pattern, bool writeOut=true) const ;

    /// Declare a ParticleContainer constSource as the one containing the constituents of the Particle in this ParticleContainer. indexName is the name of the branch containing the index of the constituents withing sourceCont.
    void declareConstituents(const ParticleContainer* constSource, const string_t& indexName) const { m_constituentSource=constSource; m_constIndexName=indexName;};
    
    /// retrieve the ParticleContainer containing the constituents of this ParticleContainer
    const ParticleContainer* constituentsContainer() const { return m_constituentSource; }
    string_t constituentsIndexName()const { return m_constIndexName; }

    /// Force loading of a given entry number. 
    ///  WARNING, this disconnects this ParticleContainer from the master index previously set through constructor,  setParameters() or setMaster().
    ///   Also this can not be used to compare 2 distinct events (at least if the 2 ParticleContainer have the same input TTree), because variables 
    ///   from the TTree are internally shared and not copied (at the exception of the kinematics). As a result, the attributes of 2 ParticleContainers pointing to the same collection, will always contain the value of the last loaded entry.
    void loadEntry(::Long64_t eventI);

    
    template<typename T>
    NonConstParticleAccessor<T>  accessor_nonconst( const string_t& varName )  { 
      typedef  VarHandle< std::vector<T> > vartype_t;

      vartype_t * v = m_attributes.getHandle<T>( varName);
      NonConstParticleAccessor<T> pa(v, this, varName);
      
      if( (v!=NULL) && !v->fromInput() ){
        // OK !
        pa.m_accessType = & ParticleAccessor<T>::fromPartIndex;       
        return pa; 
      }

      pa.m_container = NULL; pa.m_handle = NULL;
      if(!v){
        // should do something sensible here 
        Error("accessor", "%s Can't return a NonConstParticleAccessor for variable %s (unknown variable or not owning it). Programm will crash...", getName().c_str(), varName.c_str());
      }else{
        Error("accessor", "%s Can't return a non ParticleAccessor for variable %s (this is an input variable). Programm will crash...", getName().c_str(), varName.c_str());
      }
      return pa;
    }

    template<typename T>
    ParticleAccessor<T>  accessor( const string_t& varName ) const {
      typedef  VarHandle< std::vector<T> > vartype_t;
      
      vartype_t * v = m_attributes.getHandle<T>( varName);
      ParticleAccessor<T> pa(v, this, varName);

      // check validity of the variable. 
      if( fFromInput && (v!=NULL) ){
        // we're directly reading the var. Particles are matched to input
        std::cout << " accessor "<< varName<< " fromPartIndex "<<std::endl;
        pa.m_accessType = & ParticleAccessor<T>::fromPartIndex; 
      } else if( v) {
        std::cout << " accessor "<< varName<< " fromPartIndex not inputpart "<<std::endl;
        pa.m_handle = v;
        pa.m_accessType = &ParticleAccessor<T>::fromPartIndex;

      } else if( m_sourceContainer != NULL) {
        // This container is a view on m_sourceContainer
        // maybe m_sourceContainer can read this variable ?
        v = m_sourceContainer->m_attributes.getHandle<T> ( varName) ;
        if(v){
          pa.m_handle = v;
          pa.m_accessType = &ParticleAccessor<T>::fromPartSourceIndex;
          std::cout << " accessor "<< varName<< " fromPartSourceIndex "<<std::endl;
        }
        else {
          pa.m_accessType = &ParticleAccessor<T>::fromPartGet ;
          std::cout << " accessor "<< varName<< " fromPartGet"<<std::endl;
        }
      } else {
        // we fall back on slow mode : 
        pa.m_accessType = &ParticleAccessor<T>::fromPartGet ;
        std::cout << " accessor "<< varName<< " fromPartGet"<<std::endl;
      }
      return pa;
    }



    /// Efficiently copy particle from source at index from copyIndices into this container.
    void copyFrom(const ParticleContainer & source, std::vector<int> copyIndices);


    /// exchange the content (including attributes, but no I/O members : this is a transiant opearation only) of this with other
    void swap(ParticleContainer &other);
    
    /// Get the currently configured prefix value
    virtual string_t getPrefix() const;

    bool isInputContainer(){return fFromInput;}

    /// Jet Specific method : access to the internal fastjet object. CAN RETURN NULL, Can be invalid, ONLY for expert us
    const fastjet::ClusterSequence * fastjetClustSeq() const {return m_fastjetClustSeq;} 
    void setFastjetClustSeq(const fastjet::ClusterSequence * cs){ m_fastjetClustSeq = cs;}


  protected:

    /// Base Kinematics variables for this collection
    mutable VarHandle< vector<float> > eta;
    mutable VarHandle< vector<float> > phi;
    mutable VarHandle< vector<float> > pt;
    mutable VarHandle< vector<float> > E;

    /// Set the prefix for the variables
    virtual void setPrefix( const string_t prefix ) const ;

    
    template<typename T>
    VarHandle< std::vector<T> >* getHandle(const string_t& varname) const {
      return m_attributes.getHandle<T>(varname);
    }

    void updateKinematics(Particle* p);
    void loadCurrentEvent() const ;
    void buildContainerFromInput() const ;
    void i_add(Particle *p);
    


    bool fillAssoContainerAtIndex(const ParticleContainer& sourceContainer, ParticleContainer& assoParticles, size_t index, const string_t & indexVariable="constit_index") const ;


  private:

    // Particle storage
    mutable std::vector< Particle* > fProxies; ///< Internal list of  valid Particles in the current event

    // VarHandle storage
    mutable VectorAttributeMap m_attributes;

    mutable ::Bool_t fFromInput; ///< Flag specifying if object is used for D3PD reading
    mutable string_t m_prefix; ///< Prefix to the branch names
    mutable const ::Long64_t *m_master;

    mutable ::Long64_t m_currentEntry;
    mutable TTree* m_inputTree;
    mutable TTree* m_outputTree;

    mutable const ParticleContainer * m_sourceContainer ; /// non void if all contained particle come from the same source. This is the case for input containers.
    mutable const ParticleContainer * m_constituentSource ; /// source container for constituents of Particles in this container
    mutable string_t m_constIndexName ;

    //std::set< string_t> m_activeVars;

    mutable bool m_inputUsesMass;
    mutable float m_forcedMass;

    /// Pointer to the fastjet object this jet is build from. WARNING : this pointer is meant to be used in reco flow only.
    ///  It is transient only,  provided only to ease calculation of some jet quantities, and will be invalid for jets read from files.
    const fastjet::ClusterSequence* m_fastjetClustSeq;


  public:
    /// Deprecated function names. Here only for compatibility
    template<typename T>
    void declareOutPutBranch(string_t varName, bool writeOut=true) const { declareOutputVar<T>(varName, writeOut);}
    void declareOutPutBranchAsIn(const ParticleContainer* p, const string_t & pattern, bool writeOut=true) const {declareOutputVarAsIn(p,pattern,writeOut);}

    ClassDef( ParticleContainer, 0 )
    
  }; // class ParticleContainer
  


} // namespace JetAnalysisEDM

#endif // JETANALYSISEDM_PARTICLECONTAINER_H
