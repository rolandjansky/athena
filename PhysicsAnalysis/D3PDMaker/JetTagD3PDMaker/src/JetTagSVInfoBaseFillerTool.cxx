/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetTagSVInfoBaseFillerTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "JetTagInfo/SVInfoBase.h"


namespace D3PD {
  
  

JetTagSVInfoBaseFillerTool::JetTagSVInfoBaseFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent),
      m_finder (*this, "SV1")
{
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetTagSVInfoBaseFillerTool::book()
{
  /// add future variable here
  return StatusCode::SUCCESS;
}


StatusCode JetTagSVInfoBaseFillerTool::fill(const Jet& p)
{
  if (/*const Analysis::SVInfoBase* infob =*/
      m_finder.getTagInfo<Analysis::SVInfoBase> (p))
  {
  }

  return StatusCode::SUCCESS;
}


}// namespace D3PD
