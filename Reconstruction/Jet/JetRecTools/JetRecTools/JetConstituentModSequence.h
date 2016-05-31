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
  
protected:
  std::string m_inputContainer = "";
  std::string m_outputContainer = "";
  
  // P-A : a property defining the type name of the input constituent
  std::string m_inputTypeName = "CaloCluster"; // MEN: Check this 
  // P-A : the actual type
  xAOD::Type::ObjectType m_inputType; // 
  
  
  ToolHandleArray<IJetConstituentModifier> m_modifiers;

  bool m_saveAsShallow = true;

  /// helper function to cast, shallow copy and record a container.
  template<class T>
  xAOD::IParticleContainer* copyAndRecord(const xAOD::IParticleContainer* cont) const {
    const T * clustCont = dynamic_cast<const T *>(cont);
    if(clustCont == 0) {
      ATH_MSG_ERROR( "Container "<<m_inputContainer<< " is not of type "<< m_inputType);
      return NULL;
    }
    std::pair< T*, xAOD::ShallowAuxContainer* > newclust = xAOD::shallowCopyContainer(*clustCont );    
    newclust.second->setShallowIO(m_saveAsShallow);
    if(evtStore()->record( newclust.first, m_outputContainer ).isFailure() || evtStore()->record( newclust.second, m_outputContainer+"Aux." ).isFailure() ){
      ATH_MSG_ERROR("Unable to record cluster collection" << m_outputContainer );
      return NULL;
    }
    //newclust.second->setShallowIO(false);
    return newclust.first;
  }

};

#endif
