/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include "JetAnalysisEDM/EventManager.h"
#include "TTree.h"
#include "TFile.h"

namespace JetAnalysisEDM {


  EventManager::EventManager(TTree* inputTree) : EventObject(m_masterIndex, NULL)
                                               , m_contMap()
                                               , m_inputTree(inputTree)
                                               , m_masterIndex(-1)
                                               , m_lastEventInTree(-1)
                                               , m_outputTree(NULL)
                                               , m_acceptOutputDeclaration(true)
                                               , m_outFile(NULL)
                                               , m_specialContMap()
  {
    //std::cout << " aaXXXXX "<< std::endl;

    if(inputTree) readFrom(inputTree);

    // By default, pre-declare some special containers (based on COMMON ntuple)
    declareSpecialContainer("cl_lc", 
                            ContainerDesc("cl", "lc_pt", "lc_eta", "lc_phi", "", 0.0) );
    declareSpecialContainer("cl", 
                            ContainerDesc("cl", "lc_pt", "lc_eta", "lc_phi", "", 0.0) );
    declareSpecialContainer("cl_em", 
                            ContainerDesc("cl", "em_pt", "em_eta", "em_phi", "", 0.0) );

    //ContainerDesc cl_em("cl", "lc_pt", "lc_eta", "lc_phi", "", 0.0);   
  }

  void EventManager::readFrom(TTree* inputTree) {
    EventObject::readFrom(inputTree);
    m_lastEventInTree = inputTree->GetEntries() - 1;
  }

  size_t EventManager::numberOfEntries(){ return m_inputTree->GetEntries(); }

  size_t EventManager::currentEntry(){ return m_inputTree->GetReadEntry(); }
  

  void EventManager::setEntry(Long64_t eventIndex){
    for(size_t i=0;i<m_eventContainers.size();i++){
      m_contMap.erase( m_eventContainers[i] );
      delete m_eventContainers[i]->second;
    }
    m_eventContainers.clear();
    m_masterIndex = m_inputTree->LoadTree(eventIndex);
    // Notify others that the event is changing.
    for ( FunPtrList::const_iterator ifun=m_evtfuns.begin(); ifun!=m_evtfuns.end(); ++ifun ) {
      FunPtr pfun = *ifun;
      pfun();
    }
  }

  bool EventManager::nextEntry(){
    // Get next entry in tree.
    int e =m_inputTree->GetReadEntry();
    e = e <0 ? -1:e;
    if(e<m_lastEventInTree){
      setEntry(e+1);      
      return true;
    }
    return false;
  }

  const ParticleContainer * EventManager::retrieve(const string_t& name){
    return retrieveNonConst(name);
  }

  ParticleContainer * EventManager::retrieveNonConst(const string_t& name){
    containermap_t::iterator it = m_contMap.find(name);
    if( it != m_contMap.end() ) return it->second;
    return retrieveInInput(name);

  }

  ParticleContainer * EventManager::retrieveInInput( string_t name){
    ParticleContainer* cont;
    // first test if some specialdescription exists for this name :
    ContainerDesc d;
    if( retrieveSpecialContainer( name, d ) ){

      cont  = new ParticleContainer(d.prefix, m_masterIndex);
      cont->declareKinematicVar(d.pt,  d.eta,  d.phi,  d.e );

      if(d.forcedMass>=0.) cont->forceInputMass(d.forcedMass);
      //std::cout << "retrieved special "<< name << " d.prefix="<< d.prefix<< "  d.eta="<< d.eta << "  "<< d.forcedMass << std::endl;


    } else 
      cont = new ParticleContainer(name, m_masterIndex);

    if( ! cont->readFrom(m_inputTree, true) ){
      delete cont;
      return NULL;
    }
    m_contMap[name] = cont;
    return cont;
  }

  void EventManager::record(const string_t& name, ParticleContainer* cont){
    // check for existing container
    containermap_t::iterator it = m_contMap.find(name);
    if( it != m_contMap.end() ) {VarHandleLog::vlog.Warning("EventManager::record","%s already existing. Can't record a new instance",name.c_str());};
    
    // ok record it
    it = m_contMap.insert( containermap_t::value_type(name,cont) ).first;
    //m_contMap[name] = cont;
    // and remember it has ti be deleted at end of event
    m_eventContainers.push_back(it); 
  }


#define CHECK_CAN_DECLARE if(!m_acceptOutputDeclaration) {VarHandleLog::vlog.Error("EventManager::declareOutput","Can't declareany output after 1 event has been written out") ; return;}
  void EventManager::declareOutputFile(const string_t& filename, const string_t& treename){
    CHECK_CAN_DECLARE ;
    m_outFile = new TFile(filename.c_str(),"recreate");
    m_outFile->cd();
    
    m_outputTree = new TTree( treename == "" ? m_inputTree->GetName() : treename.c_str() , "Output Tree");
  }


  void EventManager::declareOutputContainer(const string_t& name){
    CHECK_CAN_DECLARE ;
    const ParticleContainer* cont = NULL;
    try{ 
      cont = retrieve(name);
    } catch ( VarNotFound e) {cont = NULL; }
    if(cont) declareOutputContainer(const_cast<ParticleContainer*>(cont));
    else{
      ParticleContainer* newcont = new ParticleContainer(name, m_masterIndex);
      declareOutputContainer(newcont); 
    }
  }

  void EventManager::declareOutputContainer(ParticleContainer* cont){
    CHECK_CAN_DECLARE ;
    string_t name = cont->getName();
    containermap_t::iterator it = m_contMap.find(name);
    if( (it != m_contMap.end()) && (cont != it->second) ){VarHandleLog::vlog.Warning("EventManager::declareOutputContainer","%s already existing. Can't declare a new instance",name.c_str());};

    cont->writeTo(m_outputTree);
    cont->setMaster( &m_masterIndex );
    if(!m_outputTree) { VarHandleLog::vlog.Warning("EventManager::declareOutputContainer", "Can't declare output if no output tree declared"); return;}
    m_outputContainers.push_back(cont);
    m_contMap[name] = cont;
  }

  void EventManager::declareOutputContainerVarAsIn(const string_t& contname, const string_t & pattern, const string_t & modelcontname )  {
    CHECK_CAN_DECLARE ;
    if(!m_outputTree) { VarHandleLog::vlog.Warning("EventManager::declareOutputContainerVar", "Can't declare output if no output tree declared"); return ;}
    const ParticleContainer * cont = retrieve(contname);
    if(! cont ) return;
    const ParticleContainer* modelcont = retrieve(modelcontname);
    if(! modelcont) { VarHandleLog::vlog.Warning("EventManager::declareOutputContainerVarAsIn", "Model container %s  invalid", modelcontname.c_str()); return ;}
    cont->declareOutPutBranchAsIn(modelcont, pattern);    
  }


  void EventManager::writeEvent(){
    for(size_t i=0;i<m_outputContainers.size();i++){
      if(m_acceptOutputDeclaration) m_outputContainers[i]->writeTo(m_outputTree); // make sure all attributes are written out

      if(m_outputContainers[i]->isInputContainer()) m_outputContainers[i]->readAllActive();      
    }
    m_outputTree->Fill();
    m_acceptOutputDeclaration = false;
  }

  void EventManager::close(){
    if(m_outFile){
      m_outFile->cd();
      m_outputTree->Write();
      m_outFile->Close();
    }
  }

  bool EventManager::retrieveSpecialContainer(const string_t& key, ContainerDesc& d)const {
    std::map<string_t, ContainerDesc>::const_iterator it = m_specialContMap.find(key);    
    if (it == m_specialContMap.end() ) return false;
    d = (*it).second;
    return true;
  }

  void EventManager::registerNewEventFunction(FunPtr pfun) {
    for ( FunPtrList::const_iterator ifun=m_evtfuns.begin(); ifun!=m_evtfuns.end(); ++ifun ) {
      if ( *ifun == pfun ) return;
    }
    m_evtfuns.push_back(pfun);
  }
}
