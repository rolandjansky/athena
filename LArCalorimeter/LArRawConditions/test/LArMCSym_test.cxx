/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LArRawConditions/test/LArMCSym_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2020
 * @brief Unit test for LArMCSym.
 */

#undef NDEBUG
#include "LArRawConditions/LArMCSym.h"
#include "CaloIdentifier/CaloHelpersTest.h"
#include "LArIdentifier/LArOnlineID.h"
#include "IdDictParser/IdDictParser.h"
#include <vector>
#include <algorithm>
#include <unordered_set>


class LArOnlineIDTest
{
public:
  LArOnlineIDTest();
  ~LArOnlineIDTest();

  const LArOnlineID&  onlineID() const { return m_helper; }


private:
  std::unique_ptr<IdDictParser> m_parser;
  LArOnlineID                   m_helper;
};


LArOnlineIDTest::LArOnlineIDTest()
  : m_parser (std::make_unique<IdDictParser>())
{
  m_parser->register_external_entity("LArCalorimeter", "IdDictLArCalorimeter_DC3-05.xml");
  IdDictMgr& idd = m_parser->parse("IdDictParser/ATLAS_IDS.xml");
  m_helper.set_quiet (true);
  if (m_helper.initialize_from_dictionary(idd) != 0) {
    std::abort();
  }
}


LArOnlineIDTest::~LArOnlineIDTest()
{
}


void test1 (const CaloHelpersTest& caloHelpers,
            const LArOnlineID& onlineID)
{
  std::cout << "test1\n";

  // Mappings here are completely bogus ... just for testing
  // (would have to read the DB to get proper mappings).

  const unsigned onlHashMax = onlineID.channelHashMax();
  std::vector<HWIdentifier> onlHashtoSymOnl(onlHashMax);
  IdentifierHash hwid_max_hash = 0;
  std::unordered_set<HWIdentifier> symIds;
  for (HWIdentifier hwid : onlineID.channel_range()) {
    if (!onlineID.isEMBchannel (hwid)) continue;
    IdentifierHash hwid_hash = onlineID.channel_Hash (hwid);
    HWIdentifier hwid_sym = onlineID.channel_Id (onlineID.feedthrough_Id(hwid),
                                                 1,
                                                 onlineID.channel(hwid));
    onlHashtoSymOnl[hwid_hash] = hwid_sym;
    hwid_max_hash = std::max (hwid_max_hash, hwid_hash);
    symIds.insert (hwid_sym);
  }

  onlHashtoSymOnl.resize (hwid_max_hash+1);

  std::vector<HWIdentifier> oflHashtoSymOnl = onlHashtoSymOnl;
  std::reverse (oflHashtoSymOnl.begin(), oflHashtoSymOnl.end());

  std::vector<HWIdentifier> symVec (symIds.begin(), symIds.end());
  std::sort (symVec.begin(), symVec.end());

  const CaloCell_ID& calo_id = caloHelpers.caloID();

  LArMCSym mcsym (&onlineID, &calo_id,
                  std::vector<HWIdentifier> (oflHashtoSymOnl),
                  std::vector<HWIdentifier> (onlHashtoSymOnl),
                  std::vector<HWIdentifier> (symVec));

  assert (mcsym.symIds() == symVec);

  for (size_t i = 0; i <= hwid_max_hash; ++i) {
    HWIdentifier hwid = onlineID.channel_Id(i);
    assert (mcsym.ZPhiSymOnl(i) == onlHashtoSymOnl[i]);
    assert (mcsym.ZPhiSymOnl(hwid) == onlHashtoSymOnl[i]);
    if (!onlHashtoSymOnl[i].is_valid()) {
      assert (mcsym.ZPhiSymOnlIndex(i) == LArMCSym::NO_INDEX);
      assert (mcsym.ZPhiSymOnlIndex(hwid) == LArMCSym::NO_INDEX);
    }
    else {
      size_t idx = mcsym.ZPhiSymOnlIndex(i);
      assert (idx != LArMCSym::NO_INDEX);
      assert (mcsym.ZPhiSymOnlIndex(hwid) == idx);
      assert (symVec[idx] == onlHashtoSymOnl[i]);
    }
  }

  for (size_t i = 0; i <= hwid_max_hash; ++i) {
    Identifier id = calo_id.cell_id (i);
    if (!id.is_valid()) continue;

    assert (mcsym.ZPhiSymOfl(i) == oflHashtoSymOnl[i]);
    assert (mcsym.ZPhiSymOfl(id) == oflHashtoSymOnl[i]);
    if (!oflHashtoSymOnl[i].is_valid()) {
      assert (mcsym.ZPhiSymOflIndex(i) == LArMCSym::NO_INDEX);
      assert (mcsym.ZPhiSymOflIndex(id) == LArMCSym::NO_INDEX);
    }
    else {
      size_t idx = mcsym.ZPhiSymOflIndex(i);
      assert (idx != LArMCSym::NO_INDEX);
      assert (mcsym.ZPhiSymOflIndex(id) == idx);
      assert (symVec[idx] == oflHashtoSymOnl[i]);
    }
  }
}


int main()
{
  std::cout << "LArRawConditions/LArMCSym_test\n";

  CaloHelpersTest helpers;
  LArOnlineIDTest larhelpers;

  test1 (helpers, larhelpers.onlineID());
  return 0;
}

