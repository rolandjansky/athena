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


StatusCode BTaggingEigenVectorRecompositionTool::initialize()
{
  ATH_MSG_INFO("Hello BTaggingEigenVectorRecompositionTool user... initializing");
  ATH_CHECK(m_btageffTool.retrieve());

  if (m_btageffTool.empty()) {
      ATH_MSG_ERROR("Failed to retrieve BTaggingEfficiencyTool handle");
      return StatusCode::FAILURE;
  }
   
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

std::vector<std::string> BTaggingEigenVectorRecompositionTool::getListOfOriginalNuisanceParameters() const
{
  ATH_MSG_INFO("getListOfOriginalNuisanceParameters()");
  std::vector<std::string> vec; // dummy to be replaced
  return vec;
}


std::vector<float> BTaggingEigenVectorRecompositionTool::getCoefficients(const std::string & ev_name) const
{
  ATH_MSG_INFO("getCoefficients(" << ev_name << ")");
  std::vector<float> vec; // dummy to be replaced
  return vec;
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
