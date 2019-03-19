/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file CaloIdentifier/src/CaloHelpersTest.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2018
 * @brief Helper to initialize ID helpers for unit tests.
 */


#include "CaloIdentifier/CaloHelpersTest.h"
#include "IdDictParser/IdDictParser.h"


CaloHelpersTest::CaloHelpersTest()
  : m_parser (std::make_unique<IdDictParser>())
{
  m_parser->register_external_entity("LArCalorimeter", "IdDictLArCalorimeter.xml");
  IdDictMgr& idd = m_parser->parse("IdDictParser/ATLAS_IDS.xml");
  m_em_idHelper.set_quiet (true);
  m_em_idHelper.set_do_neighbours(false);
  if (m_em_idHelper.initialize_from_dictionary(idd) != 0) {
    std::abort();
  }

  m_hec_idHelper.set_quiet (true);
  if (m_hec_idHelper.initialize_from_dictionary(idd) != 0) {
    std::abort();
  }

  m_fcal_idHelper.set_quiet (true);
  m_fcal_idHelper.set_do_neighbours(false);
  if (m_fcal_idHelper.initialize_from_dictionary(idd) != 0) {
    std::abort();
  }

  m_minifcal_idHelper.set_quiet (true);
  m_minifcal_idHelper.set_do_neighbours(false);
  if (m_minifcal_idHelper.initialize_from_dictionary(idd) != 0) {
    std::abort();
  }

  m_tile_idHelper.set_quiet (true);
  m_tile_idHelper.set_do_neighbours(false);
  if (m_tile_idHelper.initialize_from_dictionary(idd) != 0) {
    std::abort();
  }

  m_calo_idHelper = std::make_unique<CaloCell_ID> (&m_em_idHelper,
                                                   &m_hec_idHelper,
                                                   &m_fcal_idHelper,
                                                   &m_minifcal_idHelper,
                                                   &m_tile_idHelper);
  m_calo_idHelper->set_quiet (true);
  if (m_calo_idHelper->initialize_from_dictionary(idd) != 0) {
    std::abort();
  }
}


CaloHelpersTest::~CaloHelpersTest()
{
}


const LArEM_ID&        CaloHelpersTest::emID() const
{
  return m_em_idHelper;
}


const LArHEC_ID&       CaloHelpersTest::hecID() const
{
  return m_hec_idHelper;
}


const LArFCAL_ID&      CaloHelpersTest::fcalID() const
{
  return m_fcal_idHelper;
}


const LArMiniFCAL_ID&       CaloHelpersTest::minifcalID() const
{
  return m_minifcal_idHelper;
}


const TileID&         CaloHelpersTest::tileID() const
{
  return m_tile_idHelper;
}


const CaloCell_ID&     CaloHelpersTest::caloID() const
{
  return *m_calo_idHelper;
}
