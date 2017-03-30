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


class JetConstituentModSequence: public asg::AsgTool, virtual public IJetExecuteTool { // Changed from IJetExecute
  ASG_TOOL_CLASS(JetConstituentModSequence, IJetExecuteTool)
  public:
  JetConstituentModSequence(const std::string &name); // MEN: constructor 
  StatusCode initialize();
  int execute() const;
  void setInputClusterCollection(const xAOD::IParticleContainer* cont);
  xAOD::IParticleContainer* getOutputClusterCollection();
  
protected:
  std::string m_inputContainer = "";
  std::string m_outputContainer = "";
  const xAOD::IParticleContainer* m_trigInputClusters; // used in trigger context only
  mutable xAOD::IParticleContainer* m_trigOutputClusters;
  bool m_trigger;
  
  // P-A : a property defining the type name of the input constituent
  std::string m_inputTypeName = "CaloCluster"; // MEN: Check this 
  // P-A : the actual type
  xAOD::Type::ObjectType m_inputType; // 
  
  
  ToolHandleArray<IJetConstituentModifier> m_modifiers;

  bool m_saveAsShallow = true;

  /// helper function to cast, shallow copy and record a container (for offline) or deep copy and record a container (for trigger, where shallow copy isn't supported)
  template<class T, class Taux, class Tsingle>
  xAOD::IParticleContainer* copyAndRecord(const xAOD::IParticleContainer* cont, bool record, std::string suffix="") const {
    const T * clustCont = dynamic_cast<const T *>(cont);
    if(clustCont == 0) {
      ATH_MSG_ERROR( "Container "<<m_inputContainer+suffix<< " is not of type "<< m_inputType);
      return NULL;
    }

    if(!m_trigger){
      std::pair< T*, xAOD::ShallowAuxContainer* > newclust = xAOD::shallowCopyContainer(*clustCont );    
      newclust.second->setShallowIO(m_saveAsShallow);
      if(record){
        if(evtStore()->record( newclust.first, m_outputContainer+suffix ).isFailure() ||
	   evtStore()->record( newclust.second, m_outputContainer+suffix+"Aux." ).isFailure() ){
          ATH_MSG_ERROR("Unable to record object collection" << m_outputContainer+suffix );
          return NULL;
        }
      }
      //newclust.second->setShallowIO(false);
      return newclust.first;
    }
    
    
    // This is the trigger case, revert to a deep copy
    
    // Create the new container and its auxiliary store.
    T* clusterCopy = new T();
    Taux* clusterCopyAux = new Taux();
    clusterCopy->setStore( clusterCopyAux ); //< Connect the two
    typename T::const_iterator clust_itr;
    clust_itr = clustCont->begin();
    typename T::const_iterator clust_end = clustCont->end();

    for( ; clust_itr != clust_end; ++clust_itr ) {
      Tsingle* cluster = new Tsingle();
      clusterCopy->push_back( cluster ); // jet acquires the goodJets auxstore
      *cluster= **clust_itr; // copies auxdata from one auxstore to the other
      //clusterCopy->push_back(new Tsingle(**clust_itr));
    }

    // Store and return the container
    m_trigOutputClusters = clusterCopy;
    return m_trigOutputClusters;

  }

};

#endif
