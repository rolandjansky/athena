// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//JetConstituentModSequence.h

#ifndef JETRECTOOLS_JETCONSTITUENTMODSEQUENCE_H
#define JETRECTOOLS_JETCONSTITUENTMODSEQUENCE_H

//
// // Michael Nelson, CERN & Univesity of Oxford
// // February, 2016

#include <string>
#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetExecuteTool.h"
#include "JetInterface/IJetConstituentModifier.h"
#include "AsgTools/ToolHandleArray.h"
#include "xAODCore/ShallowCopy.h"

#include "xAODBase/IParticleContainer.h"
// #include "xAODCaloEvent/CaloCluster.h" 
#include "xAODCaloEvent/CaloClusterContainer.h"
// #include "xAODTruth/TruthParticle.h" 
#include "xAODTruth/TruthParticleContainer.h" 
// #include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
// #include "xAODPFlow/PFO.h"
#include "xAODPFlow/PFOContainer.h"


class JetConstituentModSequence: public asg::AsgTool, virtual public IJetExecuteTool { // Changed from IJetExecute
  ASG_TOOL_CLASS(JetConstituentModSequence, IJetExecuteTool)
  public:
  JetConstituentModSequence(const std::string &name); // MEN: constructor 
  StatusCode initialize();
  int execute() const;
  void setInputClusterCollection(const xAOD::IParticleContainer* cont);
  
protected:
  std::string m_inputContainer = "";
  std::string m_outputContainer = "";

  // used in trigger context only
  const xAOD::IParticleContainer* m_trigInputClusters; 
  bool m_trigger;
  
  // P-A : a property defining the type name of the input constituent
  std::string m_inputTypeName = "CaloCluster"; // MEN: Check this 
  // P-A : the actual type
  xAOD::Type::ObjectType m_inputType; // 
  
  
  ToolHandleArray<IJetConstituentModifier> m_modifiers;

  bool m_saveAsShallow = true;

  // note: not all keys will be used for a particular instantiation
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_caloClusterKey;
  SG::WriteHandleKey<xAOD::TruthParticleContainer> m_truthParticleKey;
  SG::WriteHandleKey<xAOD::TrackParticleContainer> m_trackParticleKey;

  SG::WriteHandleKey<xAOD::PFOContainer> m_pfoChargedKey{
    "ChargedParticleFlowObjects"};

  SG::WriteHandleKey<xAOD::PFOContainer> m_pfoNeutralKey{
    "NeutralParticleFlowObjects"};

  SG::WriteHandleKey<xAOD::PFOContainer> m_pfoKey{};

  SG::ReadHandleKey<xAOD::IParticleContainer> m_inputKey{};
  SG::ReadHandleKey<xAOD::IParticleContainer> m_inputPFOChargedKey{};
  SG::ReadHandleKey<xAOD::IParticleContainer> m_inputPFONeutralKey{};


  StatusCode copyModRecordPFO() const;

  /// helper function to cast, shallow copy and record a container.
  template<class T>
  // xAOD::IParticleContainer*
  StatusCode
  copyModRecord(const xAOD::IParticleContainer* cont,
                // bool record, 
                bool useModifiers,
                const SG::WriteHandleKey<T>&) const;
  // std::string suffix="") const;
  const xAOD::IParticleContainer* 
  getInput(const SG::ReadHandleKey<xAOD::IParticleContainer>&) const;

};

template<class T>
StatusCode
JetConstituentModSequence::copyModRecord(const xAOD::IParticleContainer* cont,
                                         // bool record, 
                                         bool useModifiers,
                                         const SG::WriteHandleKey<T>& key) const {
  //std::string suffix) const {
  
  /*
  const T * clustCont = dynamic_cast<const T *>(cont);
  if(clustCont == nullptr) {
    ATH_MSG_ERROR( "Container "
                   << key.key()
                   << " is not of type "
                   << m_inputType);
    return nullptr;
  }
  */


  auto handle = makeHandle(key);
  ATH_CHECK(handle.record(std::make_unique<T>(),
                          std::make_unique<xAOD::ShallowAuxContainer>()));

  for(auto o: *cont){
    auto po = dynamic_cast<typename T::value_type>(o);
    if(po == nullptr){
      ATH_MSG_ERROR( "Error downcasting from IParticle to  "
                     << m_inputType
                     << " for container key "
                     << key.key());
      return StatusCode::FAILURE;
    }
    
    handle->push_back(po);
  }

  std::pair< T*, xAOD::ShallowAuxContainer* > newclust = 
    // xAOD::shallowCopyContainer(*clustCont );    
    xAOD::shallowCopyContainer(*handle);    
  newclust.second->setShallowIO(m_saveAsShallow);
  
  if(useModifiers){
    for (auto t : m_modifiers) {ATH_CHECK(t->process(newclust.first));}
  }
  
  // if(record){

  /*
  if(evtStore()->record( newclust.first, m_outputContainer+suffix ).isFailure() || 
     evtStore()->record( newclust.second, m_outputContainer+suffix+"Aux." ).isFailure() ){
    ATH_MSG_ERROR("Unable to record cluster collection" << m_outputContainer+suffix );
    return NULL;
    }
  */
  // }
  //newclust.second->setShallowIO(false);
  // return newclust.first;
  return StatusCode::SUCCESS;
}

#endif
