// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETANALYSISEDM_EventManager_H
#define JETANALYSISEDM_EventManager_H

#include "JetAnalysisEDM/EventObject.h"
#include "JetAnalysisEDM/Particle.h"
class TTree;
class TFile;

namespace JetAnalysisEDM {

  class EventManager : public EventObject {
  public:

    typedef std::map<string_t, JetAnalysisEDM::ParticleContainer*> containermap_t;

    /// Typedef for function with returning void with no arguments.
    /// Used to register function called when changing events.
    typedef void (*FunPtr)();

    /// List of function pointers.
    typedef std::vector<FunPtr> FunPtrList;

    EventManager(TTree* inputTree=NULL) ;
    virtual ~EventManager(){}

    /// Set the input TTree
    virtual void readFrom(TTree* tree);

    /// Set the entry to be read in the input TTree
    void setEntry(Long64_t entryIndex);
    /// Advance the to the next entry in the input TTree. Returns false if current entry before the call was the last.
    bool nextEntry();

    /// Total number of entries in the input TTree
    size_t numberOfEntries();
    /// Entry being read from the input TTree
    size_t currentEntry();

    /// Retrieve a container. If the container is not already known to the EventManager, an attempt to build it 
    /// fromt the input TTree is done. Returns NULL in last resort.
    const ParticleContainer * retrieve(const string_t& name);
    ParticleContainer * retrieveNonConst(const string_t& name);


    /// Record a container under a given name. EventManager takes ownership of this container and will delete it when changing entry.
    /// To permanently record a container for output writing see declareOutputContainer
    void record(const string_t& name, ParticleContainer* cont);

    /// Set an output file and TTree
    void declareOutputFile(const string_t& filename, const string_t& treename="");
    void declareOutputTree(TTree *tree){m_outputTree = tree;};

    /// declare an output TTree by name. An ParticleContainer instance will be created and owned by EventManager,and set to be written out to the declared output TTree
    void declareOutputContainer(const string_t& name);
    /// declare an output TTree by pointer. The ParticleContainer will be recorded by its own name, owned by EventManager and set to be written out to the declared output TTree
    void declareOutputContainer(ParticleContainer* cont);

    /// Declare a variable of type T from container contname to be written in the output TTree
    template<typename T>
    void declareOutputContainerVar(const string_t& contname, const string_t& bname);

    /// Find all variables of any type and matching pattern (a regexp) present in container modelcontname. Declare all these variable as output variables of   container 'contname'
    void declareOutputContainerVarAsIn(const string_t& contname, const string_t & pattern, const string_t & modelcontname )  ;


    /// Fill the output TTree with all containers/variables declared to be written out. Must be explicitely called by client when desired.
    void writeEvent();

    void close();

    void registerNewEventFunction(FunPtr pfun);

  protected:

    ParticleContainer* retrieveInInput( string_t name);
    
    containermap_t m_contMap;
    //std::vector<JetAnalysisEDM::ParticleContainer*> m_eventContainers;  /// containers to be deleted when a new event is loaded.  
    std::vector<containermap_t::iterator> m_eventContainers;  /// containers to be deleted when a new event is loaded.  
    std::vector<JetAnalysisEDM::ParticleContainer*> m_outputContainers; /// containers saved  to output

    TTree *m_inputTree;
    Long64_t m_masterIndex;
    //Long64_t m_curentIndex;
    Long64_t m_lastEventInTree;

    TTree *m_outputTree;
    bool m_acceptOutputDeclaration;
    TFile *m_outFile;

    //********************************************
    /// ContainerDesc 
  public:
    struct ContainerDesc {
      ContainerDesc(){};
      ContainerDesc(const string_t& p, const string_t& s1, const string_t& s2, const string_t& s3, const string_t& s4, float m=-1.) :
        prefix(p), pt(s1), eta(s2), phi(s3), e(s4),forcedMass(m) {};
      string_t prefix;
      string_t pt,eta,phi,e;
      float forcedMass;
    };
    void declareSpecialContainer(const string_t& key, ContainerDesc d){m_specialContMap[key] = d;}
    bool retrieveSpecialContainer(const string_t& key, ContainerDesc& d)const ;
  protected:
    std::map<JetAnalysisEDM::string_t, ContainerDesc> m_specialContMap;

  private:
    FunPtrList m_evtfuns;
    
  };
  


  template<typename T>
  void EventManager::declareOutputContainerVar(const string_t& contname, const string_t& bname){
    if(!m_outputTree) { VarHandleLog::vlog.Warning("EventManager::declareOutputContainerVar", "Can't declare output if no output tree declared"); return ;}
    const ParticleContainer * cont = retrieve(contname);
    if(! cont ) return;
    cont->declareOutPutBranch<T>(bname);
  }

}
#endif
