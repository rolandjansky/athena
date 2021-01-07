/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataCommon/src/xAODTestReadCLinks.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief Read and dump CLinks/CLinksAOD objects.
 */


#include "xAODTestReadCLinks.h"
#include "StoreGate/ReadHandle.h"
#include <sstream>


namespace DMTest {


typedef ElementLink<CVec> EL;


/** 
 * @brief Format an ElementLink to a string.
 */
std::string formEL (const EL& el)
{
  std::ostringstream ss;
  ss << "(" << el.dataID() << ":";
  if (static_cast<int>(el.index()) == -1) {
    ss << "inv";
  }
  else {
    ss << el.index();
  }
  ss << ")";
  return ss.str();
}


/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestReadCLinks::initialize()
{
  ATH_CHECK( m_clinksKey.initialize (SG::AllowEmpty) );
  ATH_CHECK( m_clinksContainerKey.initialize (SG::AllowEmpty) );
  ATH_CHECK( m_clinksAODKey.initialize (SG::AllowEmpty) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestReadCLinks::execute (const EventContext& ctx) const
{
  if (!m_clinksKey.empty()) {
    SG::ReadHandle<CLinks> clinks (m_clinksKey, ctx);
    ATH_MSG_INFO( m_clinksKey.key() );
    ATH_CHECK( dumpCLinks (*clinks) );
  }

  if (!m_clinksContainerKey.empty()) {
    SG::ReadHandle<CLinksContainer> clinkscont (m_clinksContainerKey, ctx);
    ATH_MSG_INFO( m_clinksContainerKey.key() );
    for (const CLinks* clinks : *clinkscont) {
      ATH_CHECK( dumpCLinks (*clinks) );
    }
  }

  if (!m_clinksAODKey.empty()) {
    SG::ReadHandle<CLinksAOD> clinksaod (m_clinksAODKey, ctx);
    ATH_MSG_INFO( m_clinksAODKey.key() );

    std::ostringstream ss1;
    ss1 << "  ";
    for (const EL& el : clinksaod->vel()) {
      ss1 << formEL (el) << " ";
    }
    ATH_MSG_INFO (ss1.str());

    std::ostringstream ss2;
    ss2 << "  ";
    for (const EL el : clinksaod->elv()) {
      ss2 << formEL (el) << " ";
    }
    ATH_MSG_INFO (ss2.str());
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Dump a CLinks object.
 */
StatusCode xAODTestReadCLinks::dumpCLinks (const CLinks& clinks) const
{
  std::ostringstream ss;
  ss << "  link: " << formEL (clinks.link()) << " links: ";
  for (const EL& el : clinks.links()) {
    ss << formEL (el) << " ";
  }
  ATH_MSG_INFO( ss.str() );
  return StatusCode::SUCCESS;
}


} // namespace DMTest
