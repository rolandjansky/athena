// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/test/hash_test.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2013
 * @brief Helper for running id helper tests.
 */


#ifndef CALOIDENTIFIER_TEST_HASH_TEST_H
#define CALOIDENTIFIER_TEST_HASH_TEST_H


#include "boost/foreach.hpp"


#if __cplusplus > 201100
# define TEST_LOOP(var, range) for(var : range)
#else
# define TEST_LOOP(var, range) BOOST_FOREACH(var, range)
#endif

#define HASH_TEST1(TYPE,RANGE_TYPE,EXTRA) do                               \
{                                                                          \
  IdContext context = idhelper.TYPE##_context();                           \
  int hashsum = 0;                                                         \
  std::vector<Identifier>::const_iterator itId = idhelper.RANGE_TYPE##_begin();  \
  std::vector<Identifier>::const_iterator itIdEnd = idhelper.RANGE_TYPE##_end(); \
  const std::vector<Identifier>& ids = idhelper.TYPE##_ids();              \
  std::vector<bool> hashvec(idhelper.TYPE##_hash_max());                   \
  std::cout << "  n" << #TYPE << " " << itIdEnd - itId << "\n";            \
                                                                           \
  for(; itId!=itIdEnd; ++itId) {                                           \
    Identifier id = *itId;                                                 \
    IdentifierHash hashId;                                                 \
    assert (idhelper.get_hash (id, hashId, &context) == 0);                \
    Identifier id2;                                                        \
    assert (idhelper.get_id (hashId, id2, &context) == 0);                 \
    assert (id == id2);                                                    \
    assert (hashId < hashvec.size());                                      \
    assert (!hashvec[hashId]);                                             \
    hashvec[hashId] = true;                                                \
    assert (ids[hashId] == id);                                            \
                                                                           \
    hashsum += hashId;                                                     \
    EXTRA                                                                  \
  }                                                                        \
                                                                           \
  TEST_LOOP(Identifier id, idhelper.RANGE_TYPE##_range()) {                \
    IdentifierHash hashId;                                                 \
    assert (idhelper.get_hash (id, hashId, &context) == 0);                \
    hashsum -= hashId;                                                     \
  }                                                                        \
  assert (hashsum == 0);                                                   \
} while(0)

#define HASH_TEST(TYPE) HASH_TEST1(TYPE,TYPE,{})

#define HASH_TEST_EXTRA(TYPE) \
  assert (id == idhelper.TYPE##_id (hashId)); \
  assert (hashId == idhelper.TYPE##_hash (id));



#endif // not CALOIDENTIFIER_TEST_HASH_TEST_H
