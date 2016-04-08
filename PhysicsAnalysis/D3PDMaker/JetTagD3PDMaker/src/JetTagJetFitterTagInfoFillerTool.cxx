/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetTagJetFitterTagInfoFillerTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "JetTagInfo/JetFitterTagInfo.h"


namespace D3PD {
  
  

JetTagJetFitterTagInfoFillerTool::JetTagJetFitterTagInfoFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent),
      m_finder (*this, "JetFitterTagInfo")
{
  book().ignore();  // Avoid coverity warnings.
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetTagJetFitterTagInfoFillerTool::book()
{
  CHECK(addVariable ("nvtx",     m_nvtx,       "UNDOCUMENTED", -1));
  CHECK(addVariable ("nvtx1t",   m_nsingleTrk, "UNDOCUMENTED", -1));
  CHECK(addVariable ("ntrkAtVx", m_ntrkAtVx,   "UNDOCUMENTED", -1));
  CHECK(addVariable ("efrc",     m_eFrac,      "UNDOCUMENTED", -999));
  CHECK(addVariable ("mass",     m_mass,       "UNDOCUMENTED", -999));
  CHECK(addVariable ("sig3d",    m_sig3D,      "UNDOCUMENTED", -999));
  CHECK(addVariable ("deltaPhi", m_deltaPhi,   "UNDOCUMENTED", -999));
  CHECK(addVariable ("deltaEta", m_deltaEta,   "UNDOCUMENTED", -999));

  return StatusCode::SUCCESS;
}


StatusCode JetTagJetFitterTagInfoFillerTool::fill(const Jet& p)
{
  if (const Analysis::JetFitterTagInfo* infob =
      m_finder.getTagInfo<Analysis::JetFitterTagInfo> (p))
  {
    *m_nvtx = infob->nVTX();
    *m_nsingleTrk = infob->nSingleTracks();
    *m_ntrkAtVx = infob->nTracksAtVtx();
    *m_eFrac = infob->energyFraction();
    *m_mass = infob->mass();
    *m_sig3D = infob->significance3d();
    *m_deltaPhi = infob->deltaphi();
    *m_deltaEta = infob->deltaeta();
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
