/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODBTaggingEfficiency/BTaggingEigenVectorRecompositionTool.h"


BTaggingEigenVectorRecompositionTool::BTaggingEigenVectorRecompositionTool( const std::string & name) : 
  asg::AsgTool( name ),
  m_btageffTool("")
{
  declareProperty("BTaggingEfficiencyTool", m_btageffTool);
}

BTaggingEigenVectorRecompositionTool::~BTaggingEigenVectorRecompositionTool() 
{

}


StatusCode BTaggingEigenVectorRecompositionTool::initialize()
{
  ATH_MSG_INFO("Hello BTaggingEigenVectorRecompositionTool user,,, initializing");
  ATH_CHECK(m_btageffTool.retrieve());

  if (m_btageffTool.empty()) {
      ATH_MSG_ERROR("Failed to retrieve BTaggingEfficiencyTool handle");
      return StatusCode::FAILURE;
  }
   
  return StatusCode::SUCCESS;

}


CP::CorrectionCode BTaggingEigenVectorRecompositionTool::getListOfOriginalNuisanceParameters( std::vector<std::string> & np_list) const
{
  np_list.push_back("aaaa");
  return CP::CorrectionCode::Ok;

  // In case of a problem 
  // return CP::CorrectionCode::Error;
  
}


CP::CorrectionCode BTaggingEigenVectorRecompositionTool::getListOfCoefficients() const
{
  ATH_MSG_INFO("Here you print the coefficients for now");
  return CP::CorrectionCode::Ok;
}
