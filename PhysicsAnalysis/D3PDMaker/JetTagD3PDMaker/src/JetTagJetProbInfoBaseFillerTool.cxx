/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetTagJetProbInfoBaseFillerTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "JetTagInfo/JetProbInfoBase.h"


namespace D3PD {
  
  

JetTagJetProbInfoBaseFillerTool::JetTagJetProbInfoBaseFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent),
      m_finder (*this, "JetProb")
{
  book().ignore();  // Avoid coverity warnings.
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetTagJetProbInfoBaseFillerTool::book()
{

  CHECK(addVariable ("ntrk",    m_ntrk, "UNDOCUMENTED", -1 ));
  
  return StatusCode::SUCCESS;
}


StatusCode JetTagJetProbInfoBaseFillerTool::fill(const Jet& p)
{
  if (const Analysis::JetProbInfoBase* infob =
      m_finder.getTagInfo<Analysis::JetProbInfoBase> (p))
  {
    *m_ntrk = infob->nbTracks();
  }

  return StatusCode::SUCCESS;
}


}// namespace D3PD
