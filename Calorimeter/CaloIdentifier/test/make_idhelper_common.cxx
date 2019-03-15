/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/make_idhelper_common.cxx
 * @author scott snyder
 * @date Apr 2016
 * @brief Common code to make an idhelper.
 *        This file is meant to be included in another file,
 *        not compiled separately.
 */


#include "IdDictParser/IdDictParser.h"
#include "CxxUtils/make_unique.h"
#include <memory>


const std::string idDictType = "LArCalorimeter";
const std::string idDictXmlFile = "IdDictLArCalorimeter_DC3-05-Comm-01.xml";

std::unique_ptr<IdDictParser> make_parser(const std::string& type = idDictType,
                                          const std::string& xmlFile = idDictXmlFile)
{
  auto parser = std::make_unique<IdDictParser>();
  parser->register_external_entity (type, xmlFile);
  parser->parse ("IdDictParser/ATLAS_IDS.xml");
  return parser;
}


template <class T>
std::unique_ptr<T> make_helper (const IdDictParser& parser,
                                bool do_neighbours = false,
                                bool do_checks = true)
{
  auto idhelper = CxxUtils::make_unique<T>();
  idhelper->set_do_neighbours (do_neighbours);
  assert (idhelper->initialize_from_dictionary (parser.m_idd) == 0);

  assert (!idhelper->do_checks());
  if (do_checks) {
    idhelper->set_do_checks (true);
    assert (idhelper->do_checks());
  }

  return idhelper;
}


