/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  CaloIdentifier/test/CaloIDHelper_test.cxx
 * @author scott snyder
 * @date May 2013
 * @brief Unit test for CaloIDHelper
 */

#undef NDEBUG


#include "CaloIdentifier/CaloIDHelper.h"
#include "IdDictParser/IdDictParser.h"
#include <cassert>
#include <iostream>


class CaloIDHelperTest
  : public CaloIDHelper
{
public:
  CaloIDHelperTest (const std::string& name) : CaloIDHelper(name) {}

  virtual int  get_expanded_id  (const Identifier& id,
                                 ExpandedIdentifier& exp_id,
                                 const IdContext* /*context*/) const
  {
    exp_id.clear();
    int i = (id.get_identifier32().get_compact() - 100) / 10;
    int barrel_ec = 1;
    if (i >= 5) {
      barrel_ec = -1;
      i -= 5;
    }
    int sampling = i;
    int region = 0;
    if (sampling == 4) {
      sampling = 1;
      region = 1;
    }
    exp_id << lar_field_value()
           << lar_em_field_value()
           << barrel_ec
           << sampling
           << region;
    return 0;
  }

  using CaloIDHelper::dict;
  using CaloIDHelper::fill_vec_of_dict_regions;
  using CaloIDHelper::initialize_base_from_dictionary;

  Identifier to_id (const ExpandedIdentifier& exp_id) const
  {
    unsigned ndx = exp_id[0]-10;
    return Identifier (ndx*10 + 100);
  }
  Identifier to_id2 (const ExpandedIdentifier& exp_id) const
  {
    unsigned ndx = exp_id[0]-10;
    if (ndx == 9) ndx = 0;
    return Identifier (ndx*10 + 100);
  }

  int init_channels (const std::set<Identifier>& ids,
                     size_type end_index,
                     const MultiRange* full_range = nullptr)
  {
    return channels().init (name() + ".channels",
                            ids, end_index, m_msgSvc, full_range);
  }

  int init_regions (const std::set<Identifier>& ids,
                    size_type end_index,
                    const MultiRange* full_range = nullptr)
  {
    return regions().init (name() + ".regions",
                           ids, end_index, m_msgSvc, full_range);
  }
};


void test_hashgroup()
{
  std::cout << "test_hashgroup\n";

  std::set<Identifier> ids;
  for (unsigned int i = 100; i < 200; i += 10)
    ids.insert (Identifier (i));
  CaloIDHelper::HashGroup hg;
  assert (hg.init ("hg", ids, 3, nullptr) == 0);
  assert (hg.hash_max() == 10);

  std::vector<Identifier> vids (ids.begin(), ids.end());
  assert (hg.ids() == vids);

  unsigned int tot = 0;
  for (Identifier id : ids)
    tot += id.get_compact();

  unsigned int tot2 = 0;
  for (Identifier id : hg.range())
    tot2 += id.get_compact();
  assert (tot == tot2);

  tot2 = 0;
  for (CaloIDHelper::id_iterator it = hg.begin(); it != hg.end(); ++it)
    tot2 += it->get_compact();
  assert (tot == tot2);

  IdentifierHash hash = 0;
  assert (hg.get_hash (vids[3], hash) == 0);
  assert (hg.get_hash (Identifier(2000), hash) == 1);
  assert (hg.hash (vids[4]) == 4);
  assert (! hg.hash (Identifier(2000)).is_valid() );

  assert (hg.id (3) == vids[3]);

  assert (hg.end_index() == 3);

  IdContext context = hg.context();
  assert (context.begin_index() == 0);
  assert (context.end_index() == 3);

  Range r;
  r.add (10, 19);
  std::cout << r.cardinality() <<"\n";
  assert (r.cardinality() == 10);
  MultiRange mr;
  mr.add (r);
  assert (mr.cardinality() == 10);

  CaloIDHelper::HashGroup hg2;
  assert (hg2.init ("hg2", ids, 3, nullptr, &mr) == 0);

  CaloIDHelperTest helper ("helper");
  CaloIDHelper::HashGroup hg3;
  assert (hg3.init (helper, "", mr, &CaloIDHelperTest::to_id, 3) == 0);
  assert (hg3.ids() == vids);

  CaloIDHelper::HashGroup hg4;
  assert (hg4.init (helper, "", mr, &CaloIDHelperTest::to_id2, 3) != 0);
}


void test_helper()
{
  std::cout << "test_helper\n";

  CaloIDHelperTest helper ("helper");
  assert (helper.name() == "helper");
  assert (helper.msgSvc() == nullptr);


  IdDictParser parser;
  parser.register_external_entity ("LArCalorimeter",
                                   "IdDictLArCalorimeter_DC3-05-Comm-01.xml");
  IdDictMgr& idd = parser.parse ("IdDictParser/ATLAS_IDS.xml");

  assert (helper.initialize_base_from_dictionary(idd, "LArCalorimeterx") == 1);
  assert (helper.initialize_base_from_dictionary(idd, "LArCalorimeter") == 0);
  assert (helper.dict() != nullptr);

  std::set<Identifier> chan_ids;
  for (unsigned int i = 1000; i < 1200; i += 10)
    chan_ids.insert (Identifier (i));

  std::set<Identifier> reg_ids;
  for (unsigned int i = 100; i < 200; i += 10)
    reg_ids.insert (Identifier (i));

  helper.init_channels (chan_ids, 3);
  helper.init_regions (reg_ids, 2);

  const CaloIDHelper& chelper = helper;
  assert (chelper.channels().hash_max() == 20);
  assert (chelper.regions().hash_max() == 10);
  assert (helper.channel_hash_max() == 20);
  assert (helper.region_hash_max() == 10);

  IdContext reg_context = helper.region_context();
  assert (reg_context.begin_index() == 0);
  assert (reg_context.end_index() == 2);
  IdContext chan_context = helper.channel_context();
  assert (chan_context.begin_index() == 0);
  assert (chan_context.end_index() == 3);

  std::vector<Identifier> vchan_ids (chan_ids.begin(), chan_ids.end());
  std::vector<Identifier> vreg_ids (reg_ids.begin(),   reg_ids.end());
  assert (helper.channel_id(13) == vchan_ids[13]);
  assert (helper.region_id(4) == vreg_ids[4]);
  assert (helper.channel_hash (vchan_ids[14]) == 14);
  assert (helper.region_hash (vreg_ids[6]) == 6);

  Identifier id2;
  assert (helper.get_id (15, id2, &chan_context) == 0);
  assert (id2 == vchan_ids[15]);
  assert (helper.get_id (7, id2, &reg_context) == 0);
  assert (id2 == vreg_ids[7]);

  assert (helper.get_id (99, id2, &chan_context) == 1);
  assert (helper.get_id (99, id2, &reg_context) == 1);
  IdContext xcontext;
  assert (helper.get_id (0, id2, &xcontext) == 1);

  IdentifierHash hashid;
  assert (helper.get_hash (vchan_ids[5], hashid, &chan_context) == 0);
  assert (hashid == 5);
  assert (helper.get_hash (vreg_ids[7], hashid, &reg_context) == 0);
  assert (hashid == 7);

  assert (helper.get_hash (Identifier(9999), hashid, &chan_context) == 1);
  assert (helper.get_hash (Identifier(9999), hashid, &reg_context) == 1);
  assert (helper.get_hash (vchan_ids[5], hashid, &xcontext) == 1);

  assert (helper.fill_vec_of_dict_regions ("lar_em") == 0);

  for (Identifier id : chelper.regions().range()) {
    IdentifierHash hashid = helper.region_hash (id);
    std::cout << id.get_identifier32().get_compact() << " "
              << helper.eta0(hashid) << " "
              << helper.phi0(hashid) << " "
              << helper.etaGranularity(hashid) << " "
              << helper.phiGranularity(hashid) << "\n";
    assert (helper.dictRegions()[hashid]->m_eta0 == helper.eta0(hashid));
  }
}


int main()
{
  test_hashgroup();
  test_helper();
  return 0;
}
