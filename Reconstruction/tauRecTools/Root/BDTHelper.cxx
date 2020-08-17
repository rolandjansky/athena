/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/BDTHelper.h"

#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TObjString.h"

namespace tauRecTools {

BDTHelper::BDTHelper() : 
  asg::AsgMessaging("BDTHelper"),
  m_BDT(nullptr) {
}



BDTHelper::~BDTHelper() {
}



StatusCode BDTHelper::initialize(const TString& weightFileName) {
  
  std::unique_ptr<TFile> file(TFile::Open(weightFileName));
  if (!file) {
    ATH_MSG_ERROR("Cannot find input BDT file: " << weightFileName);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Open file: " << weightFileName);

  TTree* tree = dynamic_cast<TTree*> (file->Get("BDT"));
  if (!tree) {
    ATH_MSG_ERROR("Cannot find input BDT tree");
    return StatusCode::FAILURE;
  }
  m_BDT = std::make_unique<MVAUtils::BDT>(tree);

  TNamed* varList = dynamic_cast<TNamed*> (file->Get("varList"));
  if (!varList) {
    ATH_MSG_ERROR("No variable list in file: " << weightFileName);
    return StatusCode::FAILURE;
  }
  TString names = varList->GetTitle();
  delete varList;

  // abtain the list of input variables  
  m_inputVariableNames = parseString(names);

  file->Close();

  return StatusCode::SUCCESS;
}

std::vector<TString> BDTHelper::parseString(const TString& str, const TString& delim/*=","*/) const {
  std::vector<TString> parsedString;

  TObjArray* objList = str.Tokenize(delim);
  size_t arraySize = objList->GetEntries();
  
  // split the string with ",", and put them into a vector
  for(size_t i = 0; i < arraySize; ++i) {
    TString var = dynamic_cast<TObjString*> (objList->At(i))->String();
    var.ReplaceAll(" ", "");
    if(var.Contains(":=")) {
      var=var(var.Index(":=")+2, var.Length()-var.Index(":=")-2);
    }
    if(0==var.Length()) continue;
    parsedString.push_back(var);
  }
 
  delete objList;

  return parsedString;
}

std::vector<float> BDTHelper::getInputVariables(const std::map<TString, float> &availableVariables) const {  
  std::vector<float> values;

  // sort the input variables by the order in varList (from BDT)
  for (const TString& name : m_inputVariableNames) {
    std::map<TString, float>::const_iterator itr = availableVariables.find(name);
    if(itr==availableVariables.end()) {
      ATH_MSG_ERROR(name << " not available");
    }
    else {
      values.push_back(itr->second);
    }
  }

  return values;
}

std::vector<float> BDTHelper::getInputVariables(const std::map<TString, float*> &availableVariables) const {  
  std::vector<float> values;

  // sort the input variables by the order in varList (from BDT)
  for (const TString& name : m_inputVariableNames) {
    std::map<TString, float*>::const_iterator itr = availableVariables.find(name);
    if(itr==availableVariables.end()) {
      ATH_MSG_ERROR(name << " not available");
    }
    else {
      values.push_back(*itr->second);
    }
  }

  return values;
}

std::vector<float> BDTHelper::getInputVariables(const xAOD::TauJet& tau) const {
  std::vector<float> values;

  // obtain the values of input variables by the name
  // all the variables should be decorated to tau already
  for (TString name : m_inputVariableNames) {
    // remove prefix (::TauJets.centFrac -> cenFrac) 
    if(name.Index(".")>=0){
      name = name(name.Last('.')+1, name.Length()-name.Last('.')-1);
    }
  
    SG::AuxElement::ConstAccessor<float> accessor(name.Data());
    float value = accessor(tau);
    values.push_back(value);
  }

  return values;
}



float BDTHelper::getGradBoostMVA(const std::map<TString, float> &availableVariables) const {
  std::vector<float> values = getInputVariables(availableVariables);

  float score = -999;
  if (values.size() < m_inputVariableNames.size()) {
    ATH_MSG_ERROR("There are missing variables when calculating the BDT score, will return -999");
  }
  else {  
    score = m_BDT->GetGradBoostMVA(values);
  }

  return score;
}

float BDTHelper::getResponse(const std::map<TString, float*> &availableVariables) const {
  std::vector<float> values = getInputVariables(availableVariables);

  float score = -999;
  if (values.size() < m_inputVariableNames.size()) {
    ATH_MSG_ERROR("There are missing variables when calculating the BDT score, will return -999");
  }
  else {  
    score = m_BDT->GetResponse(values);
  }

  return score;
}

float BDTHelper::getGradBoostMVA(const xAOD::TauJet& tau) const {
  std::vector<float> values = getInputVariables(tau);

  float score = -999;
  if (values.size() < m_inputVariableNames.size()) {
    ATH_MSG_ERROR("There are missing variables when calculating the BDT score, will return -999");
  }
  else {
    score = m_BDT->GetGradBoostMVA(values);
  }

  return score;
}

} // end of namespace tauRecTools
