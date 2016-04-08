/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetTagGbbNNInfoFillerTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "JetTagInfo/GbbNNTagInfo.h"


namespace D3PD {
  
  

JetTagGbbNNInfoFillerTool::JetTagGbbNNInfoFillerTool
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
StatusCode JetTagGbbNNInfoFillerTool::book()
{
  CHECK(addVariable ("nMatchingTracks", m_nMatchingTracks, "UNDOCUMENTED", -1));
  CHECK(addVariable ("trkJetWidth",     m_trkJetWidth,     "UNDOCUMENTED", -1));
  CHECK(addVariable ("trkJetMaxDeltaR", m_trkJetMaxDeltaR, "UNDOCUMENTED", -1));

  return StatusCode::SUCCESS;
}


StatusCode JetTagGbbNNInfoFillerTool::fill(const Jet& p)
{
  if (const Analysis::GbbNNTagInfo* infob =
      m_finder.getTagInfo<Analysis::GbbNNTagInfo> (p))
  {
    *m_nMatchingTracks =infob->nMatchingTracks();
    *m_trkJetWidth =infob->trkJetWidth();
    *m_trkJetMaxDeltaR =infob->trkJetMaxDeltaR();
  }

  return StatusCode::SUCCESS;
}


}// namespace D3PD
