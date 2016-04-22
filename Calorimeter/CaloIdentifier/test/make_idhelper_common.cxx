/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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


std::string idDictType = "LArCalorimeter";
std::string idDictXmlFile = "IdDictLArCalorimeter_DC3-05-Comm-01.xml";

IdDictMgr& getDictMgr()
{
  static IdDictMgr* mgr = nullptr;
  if (!mgr) {
    static IdDictParser parser;
    parser.register_external_entity (idDictType,
                                     idDictXmlFile);
    mgr = &parser.parse ("IdDictParser/ATLAS_IDS.xml");
  }
  return *mgr;
}


template <class T>
std::unique_ptr<T> make_helper (bool do_neighbours = false,
                                bool do_checks = true)
{
  auto idhelper = CxxUtils::make_unique<T>();
  idhelper->set_do_neighbours (do_neighbours);
  assert (idhelper->initialize_from_dictionary (getDictMgr()) == 0);

  assert (!idhelper->do_checks());
  if (do_checks) {
    idhelper->set_do_checks (true);
    assert (idhelper->do_checks());
  }

  return idhelper;
}


