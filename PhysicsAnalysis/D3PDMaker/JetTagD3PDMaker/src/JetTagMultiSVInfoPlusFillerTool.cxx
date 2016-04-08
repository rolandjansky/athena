/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagMultiSVInfoPlusFillerTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "JetTagInfo/MultiSVInfoPlus.h"


namespace D3PD {
  
  

JetTagMultiSVInfoPlusFillerTool::JetTagMultiSVInfoPlusFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent),
      m_finder (*this, "MultiSVInfoPlus", true)
{
  book().ignore(); // Avoid coverity warnings.
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetTagMultiSVInfoPlusFillerTool::book()
{

  CHECK(addVariable ("isValid", m_isValid) );
  CHECK(addVariable ("ntrkv",   m_ntrkVx,  "UNDOCUMENTED", -1) );
  CHECK(addVariable ("ntrkj",   m_ntrkJet, "UNDOCUMENTED", -1) );
  CHECK(addVariable ("n2t",     m_n2trkVx, "UNDOCUMENTED", -1) );
  CHECK(addVariable ("nVtx",    m_nVtx,    "UNDOCUMENTED", -1) );
  CHECK(addVariable ("normWeightedDist", m_normWeightedDist) );
 

  return StatusCode::SUCCESS;
}


StatusCode JetTagMultiSVInfoPlusFillerTool::fill(const Jet& p)
{
  if (const Analysis::MultiSVInfoPlus* info =
      m_finder.getTagInfo<Analysis::MultiSVInfoPlus> (p))
  {
    *m_isValid=info->isValid(); 
    *m_ntrkVx = info->getNGTrackInSvx();
    *m_ntrkJet = info->getNGTrackInJet();
    *m_n2trkVx = info->getN2T();
    *m_nVtx = info->numVtxInfo();
    *m_normWeightedDist = info->getNormDist();
  }
 
  return StatusCode::SUCCESS;
}


}// namespace D3PD
