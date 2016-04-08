/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetTagIPInfoBaseFillerTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "JetTagInfo/IPInfoBase.h"


namespace D3PD {
  
  

JetTagIPInfoBaseFillerTool::JetTagIPInfoBaseFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent),
      m_finder (*this, "IP2D")
{
  book().ignore();  // Avoid coverity warnings.
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetTagIPInfoBaseFillerTool::book()
{

  CHECK(addVariable ("ntrk",    m_ntrk, "UNDOCUMENTED", -1));
  
  return StatusCode::SUCCESS;
}


StatusCode JetTagIPInfoBaseFillerTool::fill(const Jet& p)
{
  if (const Analysis::IPInfoBase* infob =
      m_finder.getTagInfo<Analysis::IPInfoBase> (p))
  {
    *m_ntrk=infob->nbTracks();
  }

  return StatusCode::SUCCESS;
}


}// namespace D3PD
