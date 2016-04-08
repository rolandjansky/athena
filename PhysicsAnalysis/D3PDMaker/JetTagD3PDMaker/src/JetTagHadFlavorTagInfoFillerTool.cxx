/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetTagHadFlavorTagInfoFillerTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "JetTagInfo/HadFlavorTagInfo.h"


namespace D3PD {
  
  

JetTagHadFlavorTagInfoFillerTool::JetTagHadFlavorTagInfoFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent),
      m_finder (*this, "GbbNN")
{
  book().ignore();  // Avoid coverity warnings.
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetTagHadFlavorTagInfoFillerTool::book()
{
  CHECK(addVariable ("nMatchingTracks", m_nMatchingTracks, "UNDOCUMENTED", -1));
  CHECK(addVariable ("trkJetWidth",     m_trkJetWidth,     "UNDOCUMENTED", -1));
  CHECK(addVariable ("trkJetDRKtaxes",  m_trkJetDRKtaxes,  "UNDOCUMENTED", -1));

  return StatusCode::SUCCESS;
}


StatusCode JetTagHadFlavorTagInfoFillerTool::fill(const Jet& p)
{
  if (const Analysis::HadFlavorTagInfo* infob =
      m_finder.getTagInfo<Analysis::HadFlavorTagInfo> (p))
  {
    *m_nMatchingTracks =infob->nMatchingTracks();
    *m_trkJetWidth =infob->trkJetWidth();
    *m_trkJetDRKtaxes =infob->trkJetDRKtaxes();
  }

  return StatusCode::SUCCESS;
}


}// namespace D3PD
