/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauDiscriminant/MethodBase.h"

using std::string;
using std::map;
using std::pair;
using std::vector;

namespace TauID{

  void MethodBase::registerVariable(const string& name, char type){
    if(type == 'F'){
      if(m_floatVars.count(name) == 0){
	m_floatVariableStore.push_back(0.);
	m_floatVars.insert(pair<string, float*>(name, &m_floatVariableStore.back()));
	m_floatAccessors.emplace(name, SG::AuxElement::ConstAccessor<float>(name));
      }
    }else if(type == 'I'){
      if(m_intVars.count(name) == 0){
	m_intVariableStore.push_back(0);
	m_intVars.insert(pair<string, int*>(name, &m_intVariableStore.back()));
	m_intAccessors.emplace(name, SG::AuxElement::ConstAccessor<int>(name));
      }
    }else{
      ATH_MSG_VERBOSE("Unsupported variable type!");
    }
  }
  
  void MethodBase::registerVariables(vector<string>& names, char type){
    for(const auto& name : names) registerVariable(name, type);
  }

  const map<string, const float*>* MethodBase::getFloatPointers(){
    return &m_floatVars;
  }

  const map<string, const int*>* MethodBase::getIntPointers(){
    return &m_intVars;
  }
  
  bool MethodBase::updateVariables(xAOD::TauJet& tau){
    for(auto& var : m_floatVars){
      if(m_floatAccessors.at(var.first).isAvailable(tau)){
	*(const_cast<float*>(var.second)) = m_floatAccessors.at(var.first)(tau);
      }else{
	ATH_MSG_ERROR("Aux variable "+var.first+" not available in tau jet");
	return false;
      }
    }
    for(auto& var : m_intVars){
      if(m_intAccessors.at(var.first).isAvailable(tau)){
	*(const_cast<int*>(var.second)) = m_intAccessors.at(var.first)(tau);
      }else{
	ATH_MSG_ERROR("Aux variable "+var.first+" not available in tau jet");
	return false;
      }
    }
    return true;
  }

}
