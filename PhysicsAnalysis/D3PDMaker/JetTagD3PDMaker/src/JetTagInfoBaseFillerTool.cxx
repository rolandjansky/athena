/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetTagInfoBaseFillerTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "JetTagInfo/BaseTagInfo.h"


namespace D3PD {
  
  

JetTagInfoBaseFillerTool::JetTagInfoBaseFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent),
      m_finder (*this, "")
{
  declareProperty("AddIsValid", m_addValid=true);
  declareProperty("AddPC", m_addPc=false);
  declareProperty("AddPTau", m_addPtau=false);
  declareProperty("AddWeight", m_addWeight=false);

  book().ignore();  // Avoid coverity warnings.
  m_pc = nullptr;
  m_ptau = nullptr;
  m_weight = nullptr;
  m_isValid = nullptr;
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetTagInfoBaseFillerTool::book()
{

  CHECK(addVariable ("pu",      m_pu, "UNDOCUMENTED", -999));
  CHECK(addVariable ("pb",      m_pb, "UNDOCUMENTED", -999));
  if(m_addPc)CHECK(addVariable ("pc",         m_pc,     "UNDOCUMENTED", -999));
  if(m_addPtau)CHECK(addVariable ("ptau",         m_ptau,     "UNDOCUMENTED", -999));
  if(m_addWeight)CHECK(addVariable ("weight", m_weight, "UNDOCUMENTED", -999));
  if(m_addValid)CHECK(addVariable ("isValid", m_isValid ));
  
  return StatusCode::SUCCESS;
}


StatusCode JetTagInfoBaseFillerTool::fill(const Jet& p)
{
  if (const JetTagInfoBase* infob = m_finder.getTagInfoBase (p))
  {
    if(infob->tagLikelihood().size()){
      *m_pb=infob->tagLikelihood()[0];
    }
    if(infob->tagLikelihood().size() > 1){
      *m_pu=infob->tagLikelihood()[1];
    }
   if(m_addPc && infob->tagLikelihood().size() > 2){
     *m_pc=infob->tagLikelihood()[2];
   }
   if(m_addPtau && infob->tagLikelihood().size() > 3){
     *m_ptau=infob->tagLikelihood()[3];
   }

   if(m_addValid) *m_isValid=infob->isValid();
   if(m_addWeight) *m_weight=infob->weight();
  }

  return StatusCode::SUCCESS;
}


}// namespace D3PD
