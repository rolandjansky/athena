// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file EventCommonD3PDMaker/src/SkimDecisionVectorFillerTool.cxx
 * @author jgoodson <jgoodson@cern.ch>
 * @date Oct, 2010
 * @brief Block filler tool for SkimDecisions
 */


#include "SkimDecisionVectorFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "EventBookkeeperMetaData/SkimDecisionCollection.h"


namespace D3PD {

SkimDecisionVectorFillerTool::SkimDecisionVectorFillerTool
  (const std::string& type, 
   const std::string& name, 
   const IInterface* parent )
    : BlockFillerTool<SkimDecisionCollection>( type, name, parent )
{
  declareProperty( "SkimDecisionNames", m_skimDecisionMatchName, 
                   "Name of skim decision" );
  declareProperty( "DoAllSkimDecisions", m_doAllSkimDecisions = true, 
                   "Whether to get every decision in the collection" );

  book().ignore(); // Avoid coverity warnings.
}

 
StatusCode SkimDecisionVectorFillerTool::book()
{
  CHECK(addVariable("_name", m_skimDecisionName));
  CHECK(addVariable("_accepted", m_skimPass));
  
  return StatusCode::SUCCESS;
}
  

StatusCode SkimDecisionVectorFillerTool::fill(const SkimDecisionCollection& x )
{
  std::vector<std::string> tempName;
  std::vector<int> tempStatus;

  for (const SkimDecision* s : x){

    if(m_doAllSkimDecisions){
      tempStatus.push_back(s->isAccepted() == true ? 1:0);
      tempName.push_back(s->getName());
    }//if do all
    else {
      for(size_t i = 0; i < m_skimDecisionMatchName.size(); ++i){
        if(s->getName() == m_skimDecisionMatchName.at(i)){
          tempStatus.push_back(s->isAccepted() == true ? 1:0);
          tempName.push_back(s->getName());
        }//matching
      }//end of loop
    }//else if
  }//boost for each
     
  *m_skimDecisionName = tempName;
  *m_skimPass = tempStatus;
     
  return StatusCode::SUCCESS;
}


} // namespace D3PD

