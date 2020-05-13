/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODBTaggingEfficiency/BTaggingEigenVectorRecompositionTool.h"


BTaggingEigenVectorRecompositionTool::BTaggingEigenVectorRecompositionTool( const std::string & name) : 
  asg::AsgTool( name )
{
  declareProperty("BTaggingEfficiencyTool", m_btageffTool);
  m_initialised = false;
}

BTaggingEigenVectorRecompositionTool::~BTaggingEigenVectorRecompositionTool() 
{

}


StatusCode BTaggingEigenVectorRecompositionTool::initialize()
{
  ATH_MSG_INFO("Hello BTaggingEigenVectorRecompositionTool user... initializing");
  ATH_CHECK(m_btageffTool.retrieve());

  if (m_btageffTool.empty()) {
      ATH_MSG_ERROR("Failed to retrieve BTaggingEfficiencyTool handle");
      return StatusCode::FAILURE;
  }
  m_coefficientMapB = m_btageffTool->getEigenRecompositionCoefficientMap("B");
  m_coefficientMapC = m_btageffTool->getEigenRecompositionCoefficientMap("C");
  m_coefficientMapT = m_btageffTool->getEigenRecompositionCoefficientMap("T");
  m_coefficientMapLight = m_btageffTool->getEigenRecompositionCoefficientMap("Light");

  if(m_coefficientMapB.empty()) {
      ATH_MSG_ERROR("Failed to retrieve coefficient map of B");
      return StatusCode::FAILURE;
  }
  if(m_coefficientMapC.empty()) {
      ATH_MSG_ERROR("Failed to retrieve coefficient map of C");
      return StatusCode::FAILURE;
  }
  if(m_coefficientMapT.empty()) {
      ATH_MSG_ERROR("Failed to retrieve coefficient map of T");
      return StatusCode::FAILURE;
  }
  if(m_coefficientMapLight.empty()) {
      ATH_MSG_ERROR("Failed to retrieve coefficient map of Light");
      return StatusCode::FAILURE;
  }
  
  std::map<std::string, std::map<std::string, double>>::iterator outter;
  
  outter = m_coefficientMapB.begin();
  for (std::map<std::string, double>::iterator inner=outter->second.begin();
       inner!=outter->second.end(); ++inner){
    m_NPnameListB.push_back(inner->first);
  }
  outter = m_coefficientMapC.begin();
  for (std::map<std::string, double>::iterator inner=outter->second.begin();
       inner!=outter->second.end(); ++inner){
    m_NPnameListC.push_back(inner->first);
  }
  outter = m_coefficientMapT.begin();
  for (std::map<std::string, double>::iterator inner=outter->second.begin();
       inner!=outter->second.end(); ++inner){
    m_NPnameListT.push_back(inner->first);
  }
  outter = m_coefficientMapLight.begin();
  for (std::map<std::string, double>::iterator inner=outter->second.begin();
       inner!=outter->second.end(); ++inner){
    m_NPnameListLight.push_back(inner->first);
  }

  m_initialised = true;
  return StatusCode::SUCCESS;

}


CP::CorrectionCode BTaggingEigenVectorRecompositionTool::printListOfOriginalNuisanceParameters() const
{
  return CP::CorrectionCode::Ok;

  // In case of a problem 
  // return CP::CorrectionCode::Error;
  
}


CP::CorrectionCode BTaggingEigenVectorRecompositionTool::printListOfCoefficients() const
{
  ATH_MSG_INFO("=============================================");
  ATH_MSG_INFO(" ");
  ATH_MSG_INFO("Here you print the coefficients for now");
  ATH_MSG_INFO("Tagger name = " << m_btageffTool->getTaggerName());
  ATH_MSG_INFO(" ");
  ATH_MSG_INFO("=============================================");
  return CP::CorrectionCode::Ok;
}

std::vector<std::string> BTaggingEigenVectorRecompositionTool::getListOfOriginalNuisanceParameters(const std::string & label) const
{
  ATH_MSG_INFO("getListOfOriginalNuisanceParameters()");
  if(!m_initialised)
    const_cast<BTaggingEigenVectorRecompositionTool*>(this)->initialize();
  
  std::vector<std::string> dummy;
  if(label.compare("B") == 0)
    return m_NPnameListB;
  else if(label.compare("C") == 0)
    return m_NPnameListC;
  else if(label.compare("T") == 0)
    return m_NPnameListT;
  else if(label.compare("Light") == 0)
    return m_NPnameListLight;
  else{
    ATH_MSG_ERROR("Label is illegal! Available label: B, C, T and Light.");    
    return dummy;
  }

  return dummy;
}

std::map<std::string, std::map<std::string, double>> BTaggingEigenVectorRecompositionTool::getCoefficientMap(const std::string & label, const std::vector<int> evIdxList) const
  // Prunning eigenvectors that is not showing in eigenIdxList. Return the user a prunned map.
{
  ATH_MSG_INFO("getCoefficientMap()");
  if(!m_initialised)
    const_cast<BTaggingEigenVectorRecompositionTool*>(this)->initialize();
  
  std::map<std::string, std::map<std::string, double>> fullMap;
  if(label.compare("B") == 0)
    fullMap = m_coefficientMapB;
  else if(label.compare("C") == 0)
    fullMap = m_coefficientMapC;
  else if(label.compare("T") == 0)
    fullMap = m_coefficientMapT;
  else if(label.compare("Light") == 0)
    fullMap = m_coefficientMapLight;
  else{
    ATH_MSG_ERROR("Label is illegal! Available label: B, C, T and Light.");    
    return fullMap;
  }
  
  std::vector<std::string> evNameList;
  for(int i : evIdxList){
    // Note: This eigenvector name convention has to be same with that in CalibrationDataEigenVariations. One way to avoid the naming convention is to change coefficient map structure into: map<int, map<string, double>>
    evNameList.push_back("Eigen_"+label+"_"+std::to_string(i));
  }

  std::map<std::string, std::map<std::string, double>> resultMap;
  for (std::map<std::string, std::map<std::string, double>>::iterator iter = fullMap.begin();
       iter != fullMap.end(); ++iter){
    if (evNameList.end() != std::find(evNameList.begin(), evNameList.end(), iter->first) ||
	evNameList.empty())
      resultMap[iter->first] = iter->second;
  }
  return resultMap;
}

std::vector<double> BTaggingEigenVectorRecompositionTool::getCoefficients(const std::string & label, const int& evIdx) const
{
  ATH_MSG_INFO("getCoefficients()");
  std::vector<double> coefficients; // dummy to be replaced
  std::map<std::string, std::map<std::string, double>> fullMap;
  std::vector<std::string> NPnameList;
  if(label.compare("B") == 0){
    fullMap = m_coefficientMapB;
    NPnameList = m_NPnameListB;
  }
  else if(label.compare("C") == 0){
    fullMap = m_coefficientMapC;
    NPnameList = m_NPnameListC;
  }
  else if(label.compare("T") == 0){
    fullMap = m_coefficientMapT;
    NPnameList = m_NPnameListT;
  }
  else if(label.compare("Light") == 0){
    fullMap = m_coefficientMapLight;
    NPnameList = m_NPnameListLight;
  }
  else{
    ATH_MSG_ERROR("Label is illegal! Available label: B, C, T and Light.");    
    return coefficients;
  }

  std::string evName = "Eigen_"+label+"_"+std::to_string(evIdx);
  if(fullMap.count(evName) <= 0){
    ATH_MSG_ERROR("Cannot find " << evName <<" in Eigen Vector coefficient map.");
    return coefficients;
  }
  
  std::map<std::string, double> oneEVmap = fullMap[evName];
  for(std::string NPname : NPnameList){
    coefficients.push_back(oneEVmap[NPname]);
  }
  return coefficients;
}


CP::SystematicSet BTaggingEigenVectorRecompositionTool::affectingSystematics() const {
  return m_btageffTool->affectingSystematics();
}

CP::SystematicCode BTaggingEigenVectorRecompositionTool::applySystematicVariation( const CP::SystematicSet & systConfig )
{
  for (auto syst : systConfig) {
    CP::SystematicSet myset;
    ATH_MSG_WARNING("applySystematicVariation was called for " << syst.name() << " but BTaggingEigenVectorRecompositionTool does not apply Systematic Variations");
    //the truth tagging tool provides results for all possible systematic variations in its results objects, the user does not need to call each one seperatly.
  }
  return CP::SystematicCode::Ok;
}

bool BTaggingEigenVectorRecompositionTool::isAffectedBySystematic( const CP::SystematicVariation & systematic ) const
{
  CP::SystematicSet sys = affectingSystematics();
  return sys.find(systematic) != sys.end();
}

CP::SystematicSet BTaggingEigenVectorRecompositionTool::recommendedSystematics() const {
  return affectingSystematics();
}
