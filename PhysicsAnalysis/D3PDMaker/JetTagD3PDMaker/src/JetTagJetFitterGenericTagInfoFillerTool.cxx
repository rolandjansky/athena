/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagJetFitterGenericTagInfoFillerTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "JetTagInfo/JetFitterGenericTagInfo.h"


namespace D3PD {
  
  

JetTagJetFitterGenericTagInfoFillerTool::JetTagJetFitterGenericTagInfoFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent),
      m_finder (*this, "JetFitterCharm")
{
  book().ignore();  // Avoid coverity warnings.
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetTagJetFitterGenericTagInfoFillerTool::book()
{

  CHECK(addVariable ("doublePropName",  m_namesDouble, "Names of the properties in JetFitterGenericTagInfo"));
  CHECK(addVariable ("doublePropValue", m_valuesDouble, "Values of the properties in JetFitterGenericTagInfo"));

  CHECK(addVariable ("intPropName",     m_namesInt, "Names of the properties in JetFitterGenericTagInfo"));
  CHECK(addVariable ("intPropValue",     m_valuesInt, "Values of the properties in JetFitterGenericTagInfo"));

  return StatusCode::SUCCESS;
}


StatusCode JetTagJetFitterGenericTagInfoFillerTool::fill(const Jet& p)
{
  if (const Analysis::JetFitterGenericTagInfo* infob =
      m_finder.getTagInfo<Analysis::JetFitterGenericTagInfo> (p))
  {

    Analysis::JetFitterGenericTagInfo::DoubleMap::const_iterator diter = infob->getDoublesBegin();
    Analysis::JetFitterGenericTagInfo::DoubleMap::const_iterator dend = infob->getDoublesEnd();

    Analysis::JetFitterGenericTagInfo::IntMap::const_iterator iiter = infob->getIntsBegin();
    Analysis::JetFitterGenericTagInfo::IntMap::const_iterator iend = infob->getIntsEnd();

    for(; diter!=dend; ++diter ){
      m_namesDouble->push_back(diter->first);
      m_valuesDouble->push_back(diter->second);
    }
    for(; iiter!=iend; ++iiter ){
      m_namesInt->push_back(iiter->first);
      m_valuesInt->push_back(iiter->second);
    }
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
