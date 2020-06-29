/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODBTaggingEfficiency/BTaggingEigenVectorRecompositionTool.h"


BTaggingEigenVectorRecompositionTool::BTaggingEigenVectorRecompositionTool( const std::string & name) : 
  asg::AsgTool( name )
{
  declareProperty("BTaggingEfficiencyTool", m_btageffTool);
}

BTaggingEigenVectorRecompositionTool::~BTaggingEigenVectorRecompositionTool() 
{

}

// Initialize BtaggingEfficiencyTool handle and retrieve coefficient map for
// all flavours. Also initialize vectors which contains all original sources
// uncertainties' names. One vector for each flavour.
StatusCode BTaggingEigenVectorRecompositionTool::initialize()
{
  ATH_MSG_INFO("Hello BTaggingEigenVectorRecompositionTool user... initializing");
  ATH_CHECK(m_btageffTool.retrieve());

  if (m_btageffTool.empty()) {
      ATH_MSG_ERROR("Failed to retrieve BTaggingEfficiencyTool handle");
      return StatusCode::FAILURE;
  }
  CP::CorrectionCode code1 = m_btageffTool->getEigenRecompositionCoefficientMap("B", m_coefficientMapB);
  CP::CorrectionCode code2 = m_btageffTool->getEigenRecompositionCoefficientMap("C", m_coefficientMapC);
  CP::CorrectionCode code3 = m_btageffTool->getEigenRecompositionCoefficientMap("T", m_coefficientMapT);
  CP::CorrectionCode code4 = m_btageffTool->getEigenRecompositionCoefficientMap("Light", m_coefficientMapLight);

  if(code1 != CP::CorrectionCode::Ok) {
      ATH_MSG_ERROR("Failed to retrieve coefficient map of B");
      return StatusCode::FAILURE;
  }
  if(code2 != CP::CorrectionCode::Ok) {
      ATH_MSG_ERROR("Failed to retrieve coefficient map of C");
      return StatusCode::FAILURE;
  }
  if(code3 != CP::CorrectionCode::Ok) {
      ATH_MSG_ERROR("Failed to retrieve coefficient map of T");
      return StatusCode::FAILURE;
  }
  if(code4 != CP::CorrectionCode::Ok) {
      ATH_MSG_ERROR("Failed to retrieve coefficient map of Light");
      return StatusCode::FAILURE;
  }
  
  std::map<std::string, std::map<std::string, float>>::iterator outter;
  
  outter = m_coefficientMapB.begin();
  for (std::map<std::string, float>::iterator inner=outter->second.begin();
       inner!=outter->second.end(); ++inner){
    m_NPnameListB.push_back(inner->first);
  }
  outter = m_coefficientMapC.begin();
  for (std::map<std::string, float>::iterator inner=outter->second.begin();
       inner!=outter->second.end(); ++inner){
    m_NPnameListC.push_back(inner->first);
  }
  outter = m_coefficientMapT.begin();
  for (std::map<std::string, float>::iterator inner=outter->second.begin();
       inner!=outter->second.end(); ++inner){
    m_NPnameListT.push_back(inner->first);
  }
  outter = m_coefficientMapLight.begin();
  for (std::map<std::string, float>::iterator inner=outter->second.begin();
       inner!=outter->second.end(); ++inner){
    m_NPnameListLight.push_back(inner->first);
  }

  return StatusCode::SUCCESS;

}

// Return a vector which contains a list of original vector uncertainties names.
// vector list is for the chosen flavour label. The order of the names is the same
// as the coefficient values given by getCoefficients()
std::vector<std::string> BTaggingEigenVectorRecompositionTool::getListOfOriginalNuisanceParameters(const std::string & label) const
{
  ATH_MSG_INFO("getListOfOriginalNuisanceParameters()");
  
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

// Produce a coefficient map contains only eigenvectors that is showing in
// eigenIdxList and return it to user. If given empty evIdxList, the function
// returns a full map. Produced map is for the chosen flavour label.
std::map<std::string, std::map<std::string, float>> BTaggingEigenVectorRecompositionTool::getCoefficientMap(const std::string & label, const std::vector<unsigned int> evIdxList) const
{
  ATH_MSG_INFO("getCoefficientMap()");
  
  std::map<std::string, std::map<std::string, float>> fullMap;
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
  for(unsigned int i : evIdxList){
    // Note: This eigenvector name convention has to be same with that in CalibrationDataEigenVariations. One way to avoid the naming convention is to change coefficient map structure into: map<unsigned int, map<string, float>>
    evNameList.push_back("Eigen_"+label+"_"+std::to_string(i));
  }

  std::map<std::string, std::map<std::string, float>> resultMap;
  for (std::map<std::string, std::map<std::string, float>>::iterator iter = fullMap.begin();
       iter != fullMap.end(); ++iter){
    if (evNameList.end() != std::find(evNameList.begin(), evNameList.end(), iter->first) ||
	evNameList.empty())
      resultMap[iter->first] = iter->second;
  }
  return resultMap;
}

// Returns a vector contains the coefficients value of the chosen label
// and the chosen eigenvector. The order of the value is the same as
// the order of original uncertainty names given by
// getListOfOriginalNuisanceParameters()
std::vector<float> BTaggingEigenVectorRecompositionTool::getCoefficients(const std::string & label, const unsigned int evIdx) const
{
  ATH_MSG_INFO("getCoefficients()");
  std::vector<float> coefficients; // dummy to be replaced
  std::map<std::string, std::map<std::string, float>> fullMap;
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
  
  std::map<std::string, float> oneEVmap = fullMap[evName];
  for(std::string NPname : NPnameList){
    coefficients.push_back(oneEVmap[NPname]);
  }
  return coefficients;
}

// Return number of eigenvectors used for the chosen label.
int BTaggingEigenVectorRecompositionTool::getNumEigenVectors(const std::string & label)const{
  std::map<std::string, std::map<std::string, float>> fullMap;
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
    return -1;
  }
  return fullMap.size();
}

// this returns a list of eigen vector systematics supported by the btaggingEfficiency tool handle
CP::SystematicSet BTaggingEigenVectorRecompositionTool::affectingSystematics() const {
  return m_btageffTool->affectingSystematics();
}

// it indicates which systematic shifts are to be applied for all future calls
// no systematics for now, proxy for later
CP::SystematicCode BTaggingEigenVectorRecompositionTool::applySystematicVariation( const CP::SystematicSet & systConfig )
{
  for (auto syst : systConfig) {
    CP::SystematicSet myset;
    ATH_MSG_WARNING("applySystematicVariation was called for " << syst.name() << " but BTaggingEigenVectorRecompositionTool does not apply Systematic Variations");
    //the truth tagging tool provides results for all possible systematic variations in its results objects, the user does not need to call each one seperatly.
  }
  return CP::SystematicCode::Ok;
}

// returns true if the argument systematic is supported by the
// btaggingEfficiency tool handle
bool BTaggingEigenVectorRecompositionTool::isAffectedBySystematic( const CP::SystematicVariation & systematic ) const
{
  CP::SystematicSet sys = affectingSystematics();
  return sys.find(systematic) != sys.end();
}

// subset of Eigenvector systeamtics that are recommended by the
// btaggingEfficiency tool handle
CP::SystematicSet BTaggingEigenVectorRecompositionTool::recommendedSystematics() const {
  return affectingSystematics();
}
