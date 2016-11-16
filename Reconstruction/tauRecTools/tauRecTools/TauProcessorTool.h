/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUPROCESSORTOOL_H
#define TAUREC_TAUPROCESSORTOOL_H

#include "tauRecTools/TauRecToolBase.h"
#include "tauRecTools/ITauToolExecBase.h"
#include "AsgTools/ToolHandleArray.h"

/**
 * @brief Implementation of tauRec/TauProcessor as a tool.
 * 
 * @author Justin Griffiths
 *                                                                              
 */

class TauProcessorTool : public asg::AsgTool, virtual public ITauToolExecBase {
 public:

  ASG_TOOL_CLASS1( TauProcessorTool, ITauToolExecBase )
    
  TauProcessorTool(const std::string& type);
  ~TauProcessorTool();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

  inline void setIsConfigured(bool v=true){ m_configured=v; }

 private:
  std::string find_file(const std::string& fname) const;
  std :: string                 m_ConfigPath;
  std :: string                 m_tauContainerName;
  std :: string                 m_tauAuxContainerName; 
  bool                          m_configured;
  bool                          m_AODmode;
  bool                          m_deep_copy_TauJetContainer;
  bool                          m_deep_copy_chargedPFOContainer;
  bool                          m_deep_copy_hadronicPFOContainer;
  bool                          m_deep_copy_neutralPFOContainer;
  bool                          m_deep_copy_tauShotPFOContainer;
  bool                          m_deep_copy_SecVtxContainer;
  bool                          m_deep_copy_TauTrackContainer;
  TauEventData m_data;
  ToolHandleArray<ITauToolBase>  m_tools;

 public:

  StatusCode readConfig();

  //-------------------------------------------------------------------------
  // make a deep copy of conatiner a
  // xAOD::TauJetContainer *cont(0);
  // xAOD::TauJetAuxContainer* contAux(0);
  // xAOD::TauJet* tau(0);
  // ATH_CHECK( ITauToolBase::deepCopy( cont, contAux, tau, "TauJets", "TauJetsAux.") );
  // tau is only used to infer type of object in T...
  //-------------------------------------------------------------------------
  template<class T, class U, class V>
    StatusCode deepCopy(T*& containerOut, U*& containerStoreOut, const V* dummyContainerType, 
			const std::string& containername,  std::string containerNameAux="");


};

template<class T, class U, class V>
  StatusCode TauProcessorTool::deepCopy(T*& container, U*& containerStore, const V* /*dummyContainerElementType*/, 
				    const std::string& containerName, std::string containerNameAux){
  if(containerNameAux=="") {
    containerNameAux=containerName;
    containerNameAux+="Aux.";
  }

  const T* constContainer(0);
  const U* constAuxContainer(0);

  ATH_CHECK( evtStore()->retrieve(constContainer, containerName) );
  ATH_CHECK( evtStore()->retrieve(constAuxContainer, containerNameAux) );
  if(container==0 && containerStore==0){
    container = new T();
    containerStore = new U();
    container->setStore(containerStore);
  }
  else{
    ATH_MSG_FATAL("Proviced non-null containters, not initializing please provide null containers: ");
    return StatusCode::FAILURE;
  }
  for( const V* v : *constContainer ){
    V* newV = new V();
    container->push_back(newV);
    *newV = *v;    
  }  

#ifdef ASGTOOL_STANDALONE
  ATH_CHECK( evtStore()->record(container, containerName+"Fix") ); //TODO? make "Fix" configurable
  ATH_CHECK( evtStore()->record(containerStore, containerName+"FixAux.") );
#else
  ATH_CHECK( evtStore()->overwrite(container, containerName, true, false) );
  ATH_CHECK( evtStore()->overwrite(containerStore, containerNameAux, true, false) );
#endif

  return StatusCode::SUCCESS;
}


#endif //TAUREC_TAUPROCESSORTOOL_H
