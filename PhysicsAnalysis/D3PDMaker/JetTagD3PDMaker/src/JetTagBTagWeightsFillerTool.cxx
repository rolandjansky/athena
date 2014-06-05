/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagBTagWeightsFillerTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {
  

JetTagBTagWeightsFillerTool::JetTagBTagWeightsFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent)
{
  declareProperty ("TaggerNames", m_taggerNames);
}
  
StatusCode JetTagBTagWeightsFillerTool::book()
{
  // Eliminate duplicates.
  std::sort (m_taggerNames.begin(), m_taggerNames.end());
  std::vector<std::string>::iterator end = 
    std::unique (m_taggerNames.begin(), m_taggerNames.end());
  m_taggerNames.erase (end, m_taggerNames.end());

  m_weights.resize(m_taggerNames.size()+1);
 
  CHECK(addVariable ("Comb", m_weights[0] ));

  for(unsigned int i=1; i<m_taggerNames.size()+1; ++i){
    CHECK(addVariable (m_taggerNames[i-1], m_weights[i] ));
  }

  return StatusCode::SUCCESS;
}


StatusCode JetTagBTagWeightsFillerTool::fill(const Jet& p)
{

  clearData();

  *m_weights[0] = p.getFlavourTagWeight();

  for(unsigned int i=1; i<m_taggerNames.size()+1; ++i){
    *m_weights[i] = p.getFlavourTagWeight(m_taggerNames[i-1]);
  }

 
  return StatusCode::SUCCESS;
}

void JetTagBTagWeightsFillerTool::clearData(){

  for(unsigned int i=0; i<m_weights.size(); ++i){
    *m_weights[i] = -999.;
  }


}


} // namespace D3PD
