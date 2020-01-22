/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataCommon/src/xAODTestWriteCLinks.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief Create CLinks/CLinksAOD objects.
 */


#include "xAODTestWriteCLinks.h"
#include "DataModelTestDataCommon/CLinksAuxInfo.h"
#include "DataModelTestDataCommon/CLinksAuxContainer.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"


namespace DMTest {


typedef ElementLink<CVec> EL;


/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode xAODTestWriteCLinks::initialize()
{
  ATH_CHECK( m_cvecKey.initialize() );
  ATH_CHECK( m_clinksKey.initialize (SG::AllowEmpty) );
  ATH_CHECK( m_clinksContainerKey.initialize (SG::AllowEmpty) );
  ATH_CHECK( m_clinksAODKey.initialize (SG::AllowEmpty) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode xAODTestWriteCLinks::execute (const EventContext& ctx) const
{
  SG::ReadHandle<CVec> cvec (m_cvecKey, ctx);

  if (!m_clinksKey.empty()) {
    auto clinks = std::make_unique<CLinks>();
    auto clinksaux = std::make_unique<CLinksAuxInfo>();
    clinks->setStore (clinksaux.get());
    ATH_CHECK( fillCLinks (m_cvecKey.key(), *cvec, 0, *clinks) );
    SG::WriteHandle<CLinks> clinksH (m_clinksKey, ctx);
    ATH_CHECK( clinksH.record (std::move (clinks), std::move (clinksaux)) );
  }

  if (!m_clinksContainerKey.empty()) {
    auto clinkscont = std::make_unique<CLinksContainer>();
    auto clinkscontaux = std::make_unique<CLinksAuxContainer>();
    clinkscont->setStore (clinkscontaux.get());
    for (size_t i = 0; i < 5; i++) {
      clinkscont->push_back (std::make_unique<CLinks>());
      ATH_CHECK( fillCLinks (m_cvecKey.key(), *cvec, i, *clinkscont->back()) );
    }
    SG::WriteHandle<CLinksContainer> clinksContH (m_clinksContainerKey, ctx);
    ATH_CHECK( clinksContH.record (std::move (clinkscont),
                                   std::move (clinkscontaux)) );
  }

  if (!m_clinksAODKey.empty()) {
    auto clinksaod = std::make_unique<CLinksAOD>();
    ATH_CHECK( fillCLinksAOD (m_cvecKey.key(), *cvec, *clinksaod) );
    SG::WriteHandle<CLinksAOD> clinksAODH (m_clinksAODKey, ctx);
    ATH_CHECK( clinksAODH.record (std::move (clinksaod)) );
  }

  return StatusCode::SUCCESS;
}


StatusCode xAODTestWriteCLinks::fillCLinks (const std::string& key,
                                            const CVec& cvec,
                                            size_t ndx,
                                            CLinks& clinks) const
{
  if (!cvec.empty()) {
    clinks.setLink (EL (key, (ndx % cvec.size())));

    std::vector<EL> links;
    size_t pos = ndx;
    assert (pos < cvec.size());
    do {
      links.emplace_back (key, pos);
      pos += (ndx+1);
      pos %= cvec.size();
    } while (pos != ndx);
    clinks.setLinks (std::move (links));
  }

  return StatusCode::SUCCESS;
}
  
                                            
StatusCode xAODTestWriteCLinks::fillCLinksAOD (const std::string& key,
                                               const CVec& cvec,
                                               CLinksAOD& clinks) const
{
  std::vector<EL> vel;
  ElementLinkVector<CVec> elv;
  for (size_t i = 0; i < cvec.size(); i++) {
    vel.emplace_back (key, i);
    elv.push_back (EL (key, cvec.size()-i-1));
  }
  clinks.setVel (vel);
  clinks.setElv (elv);
  return StatusCode::SUCCESS;
}


} // namespace DMTest
