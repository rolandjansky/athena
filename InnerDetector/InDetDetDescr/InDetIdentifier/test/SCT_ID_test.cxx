/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file InDetIdentifier/test/SCT_ID_test.cxx
 * @author Shaun Roe
 * @date March 2020
 * @brief Some tests for SCT_ID 
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_SCT_ID

#include "IdDictParser/IdDictParser.h"  
#include "InDetIdentifier/SCT_ID.h"
#include <string>

#include <boost/test/unit_test.hpp>

#include <boost/test/tools/output_test_stream.hpp>
#include <iostream>
#include <algorithm>

struct cout_redirect {
    cout_redirect( std::streambuf * new_buffer ) 
        : m_old( std::cout.rdbuf( new_buffer ) )
    { }

    ~cout_redirect( ) {
        std::cout.rdbuf( m_old );
    }

private:
    std::streambuf * m_old;
};

//
//would prefer to use a local file in the package
static const std::string sctDictFilename{"InDetIdDictFiles/IdDictInnerDetector_IBL3D25-03.xml"};
static const std::string rangeError{"ERROR SCT_ID::wafer_id_checks  result is NOT ok. ID, range 2/2/10/3/3/1/02/2/0/0/0:31/-6:-1/0:1 | 2/2/0/1/0:39/-6:-1/0:1 | 2/2/0/2/0:47/-6:-1/0:1 | 2/2/0/3/0:55/-6:-1/0:1 | 2/2/0/0/0:31/1:6/0:1 | 2/2/0/1/0:39/1:6/0:1 | 2/2/0/2/0:47/1:6/0:1 | 2/2/0/3/0:55/1:6/0:1 | 2/2/-2,2/0:8/0:51/0/0:1 | 2/2/-2,2/0/0:39/1/0:1 | 2/2/-2,2/1:5/0:39/1:2/0:1 | 2/2/-2,2/6:7/0:39/1/0:1\n"};

BOOST_AUTO_TEST_SUITE(SCT_ID_Test)
  BOOST_AUTO_TEST_CASE(IdentifierMethods){
    IdDictParser parser;
    parser.register_external_entity("InnerDetector", sctDictFilename);
    IdDictMgr& idd = parser.parse ("IdDictParser/ATLAS_IDS.xml");
    SCT_ID sctId;
    BOOST_TEST(sctId.initialize_from_dictionary (idd) == 0);
    //check valid module identifier
    const auto barrelIdentifier = sctId.module_id(0,3,3,-1);
    BOOST_TEST_MESSAGE("Module (0,3,3,-1) : "<<barrelIdentifier);
    //empirically, this is 0xa61a80000000000
    Identifier::value_type v = 0xa61a80000000000;
    BOOST_TEST(barrelIdentifier == Identifier(v));
    //the following case doesn't generate any error, even with 'checks on'
    const auto nonsenseIdentifier = sctId.module_id(0,3,3,0);//eta=0 modules don't exist in the barrel
    BOOST_TEST_MESSAGE("Module (0,3,3,0) : "<<nonsenseIdentifier<<", which is nonsense.");//but this doesn't prevent it generating a number
    // These tests fail an assertion in debug builds.
    // Even with assertions disabled, they involve undefined behavior --- the
    // assertion protects against out-of-bounds array access.
#if 0
    boost::test_tools::output_test_stream output;
    BOOST_TEST_MESSAGE("Check nonsense identifiers without and with checks turned on");
    auto nonsenseIdentifier2 = Identifier{};
    auto nonsenseIdentifier3 = Identifier{};
    { 
      //note that the nonsense identifiers are still numbers which might be used
      cout_redirect guard( output.rdbuf( ) ); //catch the cout output for comparison
      nonsenseIdentifier2 = sctId.module_id(10,3,3,1);//no checks, no message
      sctId.set_do_checks(true);
      nonsenseIdentifier3 = sctId.module_id(10,3,3,1); //just generates a message
      sctId.set_do_checks(false);
    }
    //put messages outside the cout capture block
    BOOST_TEST_MESSAGE("These invalid identifiers were returned : "<<nonsenseIdentifier2<<", "<<nonsenseIdentifier3<<", so be careful!");
    BOOST_CHECK( output.is_equal( rangeError ) );
#endif
    //check wafer Identifier for side 0
    const auto barrelWaferId0 = sctId.wafer_id(0,3,3,-1,0);
    //which is just the same as the module Id...
    BOOST_TEST(barrelWaferId0 == Identifier(v));
    //whereas "side 1" ...
    Identifier::value_type s1 = 0xa61ac0000000000;
    const auto barrelWaferId1 = sctId.wafer_id(0,3,3,-1,1);
    BOOST_TEST(barrelWaferId1 == Identifier(s1));
    //strip 0 of side 0...
    const auto barrelStrip0id = sctId.strip_id(0,3,3,-1,0,0);
    //has the same id as the module...
    BOOST_TEST(barrelStrip0id == Identifier(v));
    //but strip 1 does not...
    Identifier::value_type strip1 = 0xa61a80100000000;
    const auto barrelStrip1id = sctId.strip_id(0,3,3,-1,0,1);
    BOOST_TEST(barrelStrip1id == Identifier(strip1));
    //alternatively can specify the wafer Id and a strip number
    BOOST_TEST(sctId.strip_id(barrelWaferId0, 1) == barrelStrip1id);
    //..and I'm not going to test the 'row' alternative
    //int base_bit(void) const;
    BOOST_TEST(sctId.base_bit() == 32);
    //Identifier::diff_type calc_offset(const Identifier& base,const Identifier& target) const;
    const auto barrelStrip99id = sctId.strip_id(0,3,3,-1,0,99);
    auto offset = sctId.calc_offset(barrelIdentifier,barrelStrip99id );
    BOOST_TEST(offset == 99);
    //Identifier strip_id_offset(const Identifier& base,Identifier::diff_type offset) const;
    BOOST_TEST(sctId.strip_id_offset(barrelIdentifier, 99) == barrelStrip99id);
    //expanded identifier methods
    //void get_expanded_id(const Identifier& id,ExpandedIdentifier& exp_id, const IdContext* context = 0) const;
    ExpandedIdentifier expId;
    sctId.get_expanded_id(barrelIdentifier, expId);//who uses this?
    BOOST_TEST(std::string(expId) == "2/2/0/3/3/-1/0/0");
    // Identifier strip_id(const ExpandedIdentifier& strip_id) const;
    BOOST_TEST(sctId.strip_id(expId) == barrelIdentifier);
    //Iterator usage
    const int nWafersUsingIterator = std::accumulate(sctId.wafer_begin(), sctId.wafer_end(), 0,[](int s, Identifier ){return s+1;});
    BOOST_TEST(nWafersUsingIterator == 8176);
    //expanded iterator usage
    const int nStripsUsingIterator = std::accumulate(sctId.strip_begin(), sctId.strip_end(), 0,[](int s, ExpandedIdentifier ){return s+1;});
    BOOST_TEST(nStripsUsingIterator == 8176 * 6 * 128);
    //complete test, more-or-less equivalent to "test wafer packing" in original code
    auto context = sctId.wafer_context();
    bool allIdentifiersAndExpandedIdentifiersAreEquivalent{true};
    enum indices{INDET, SCT, BARREL_EC, LAYER_DISK, PHI, ETA, SIDE, STRIP};
    for(auto pWaferId = sctId.wafer_begin(); pWaferId != sctId.wafer_end(); ++pWaferId){
      Identifier id = (*pWaferId);
      ExpandedIdentifier expId;
      sctId.get_expanded_id(id, expId, &context);
      Identifier returnedId = sctId.wafer_id(expId[BARREL_EC],
                                   expId[LAYER_DISK],
                                   expId[PHI],
                                   expId[ETA],
                                   expId[SIDE]);
      allIdentifiersAndExpandedIdentifiersAreEquivalent &=(returnedId == id);
    }
    BOOST_TEST(allIdentifiersAndExpandedIdentifiersAreEquivalent);
  }
  
  BOOST_AUTO_TEST_CASE(HashMethods){
    IdDictParser parser;
    parser.register_external_entity("InnerDetector", sctDictFilename);
    IdDictMgr& idd = parser.parse ("IdDictParser/ATLAS_IDS.xml");
    SCT_ID sctId;
    sctId.initialize_from_dictionary (idd);
    static constexpr size_t nWafers(8176);
    static constexpr size_t nStrips{nWafers * 128 * 6};
    BOOST_TEST(sctId.wafer_hash_max() == nWafers);
    BOOST_TEST(sctId.strip_hash_max() == nStrips);
    Identifier::value_type s1 = 0xa61a80000000000;
    Identifier id(s1);
    BOOST_TEST(sctId.wafer_hash(id) == 4938);
    IdentifierHash hashVal;
    BOOST_TEST(sctId.get_hash(id, hashVal) == 1);//i.e. intentionally doesn't work
    BOOST_TEST(hashVal == 0);//ditto
    BOOST_TEST(sctId.get_id(4938, id) == 1);//not sure why this one doesn't work, though
    //..and the returned id in this case would be 0xffffffffffffffff (not tested)
  }
  
  BOOST_AUTO_TEST_CASE(GeographicalMethods){
    IdDictParser parser;
    parser.register_external_entity("InnerDetector", sctDictFilename);
    IdDictMgr& idd = parser.parse ("IdDictParser/ATLAS_IDS.xml");
    SCT_ID sctId;
    sctId.initialize_from_dictionary (idd);
    //
    Identifier::value_type v = 0xa61a80000000000;
    const auto barrelIdentifier = Identifier(v);
    //  bool is_barrel(const Identifier& id) const;
    BOOST_TEST(sctId.is_barrel(barrelIdentifier));
    //int barrel_ec(const Identifier& id) const;
    BOOST_TEST(sctId.barrel_ec(barrelIdentifier) == 0);
    //int layer_disk(const Identifier& id) const;
    BOOST_TEST(sctId.layer_disk(barrelIdentifier) == 3);
    //int phi_module(const Identifier& id) const;
    BOOST_TEST(sctId.phi_module(barrelIdentifier) == 3);
    //int eta_module(const Identifier& id) const;
    BOOST_TEST(sctId.eta_module(barrelIdentifier) == -1);
    //int side(const Identifier& id) const;
    BOOST_TEST(sctId.side(barrelIdentifier) == 0);
    //int row(const Identifier& id) const; <- untested
    //int strip(const Identifier& id) const;
    BOOST_TEST(sctId.strip(barrelIdentifier) == 0);
    //max/min values
    //int layer_disk_max(const Identifier& id) const;
    BOOST_TEST(sctId.layer_disk_max(barrelIdentifier) == 3);
    //int phi_module_max(const Identifier& id) const;
    BOOST_TEST(sctId.phi_module_max(barrelIdentifier) == 55);
    //int eta_module_max(const Identifier& id) const;
    BOOST_TEST(sctId.eta_module_max(barrelIdentifier) == -1);
    //int eta_module_min(const Identifier& id) const;
    BOOST_TEST(sctId.eta_module_min(barrelIdentifier) == -6);
    //int strip_max(const Identifier& id) const;
    BOOST_TEST(sctId.strip_max(barrelIdentifier) == 767);
    //neighbours
    IdentifierHash returnedHash;
    IdentifierHash module_0_3_3_m1Hash(4938);
    //int get_prev_in_phi(const IdentifierHash& id, IdentifierHash& prev) const;
    Identifier prevPhiId = sctId.module_id(0,3,2,-1);
    auto prevPhiHash = sctId.wafer_hash(prevPhiId);
    BOOST_TEST(sctId.get_prev_in_phi(module_0_3_3_m1Hash, returnedHash) == 0);
    BOOST_TEST(returnedHash == prevPhiHash);
    //int get_next_in_phi(const IdentifierHash& id, IdentifierHash& next) const;
    Identifier nextPhiId = sctId.module_id(0,3,4,-1);
    auto nextPhiHash = sctId.wafer_hash(nextPhiId);
    BOOST_TEST(sctId.get_next_in_phi(module_0_3_3_m1Hash, returnedHash) == 0);
    BOOST_TEST(returnedHash == nextPhiHash);
    //int get_prev_in_eta(const IdentifierHash& id, IdentifierHash& prev) const;
    Identifier prevEtaId = sctId.module_id(0,3,3,-2);
    auto prevEtaHash = sctId.wafer_hash(prevEtaId);
    BOOST_TEST(sctId.get_prev_in_eta(module_0_3_3_m1Hash, returnedHash) == 0);
    BOOST_TEST(returnedHash == prevEtaHash);
    //int get_next_in_eta(const IdentifierHash& id, IdentifierHash& next) const;
    Identifier nextEtaId = sctId.module_id(0,3,3,1);//barrel eta skips zero
    auto nextEtaHash = sctId.wafer_hash(nextEtaId);
    BOOST_TEST(sctId.get_next_in_eta(module_0_3_3_m1Hash, returnedHash) == 0);
    BOOST_TEST(returnedHash == nextEtaHash);
    //int get_other_side(const IdentifierHash& id, IdentifierHash& other) const;
    Identifier oppositeId = sctId.wafer_id(0,3,3,-1,1);//use wafer_id
    IdentifierHash oppositeHash = sctId.wafer_hash(oppositeId);
    BOOST_TEST(sctId.get_other_side(module_0_3_3_m1Hash, returnedHash) == 0);
    BOOST_TEST(returnedHash == oppositeHash);
    //what happens if there's no neighbour in the given direction:
    Identifier endModuleId = sctId.module_id(0,3,2,6);
    auto endHash = sctId.wafer_hash(endModuleId);
    returnedHash = 100;
    BOOST_TEST(sctId.get_next_in_eta(endHash, returnedHash) == 1);
    BOOST_TEST(returnedHash == 100);//hash is unchanged]
    //new methods introduced March 2020
    //IdentifierHash get_prev_in_phi(const IdentifierHash& id) const;
    BOOST_TEST(sctId.get_prev_in_phi(module_0_3_3_m1Hash) == prevPhiHash);
    //IdentifierHash get_next_in_phi(const IdentifierHash& id) const;
    BOOST_TEST(sctId.get_next_in_phi(module_0_3_3_m1Hash) == nextPhiHash);
    //IdentifierHash get_prev_in_eta(const IdentifierHash& id) const;
    BOOST_TEST(sctId.get_prev_in_eta(module_0_3_3_m1Hash) == prevEtaHash);
    //IdentifierHash get_next_in_eta(const IdentifierHash& id) const;
    BOOST_TEST(sctId.get_next_in_eta(module_0_3_3_m1Hash) == nextEtaHash);
    //IdentifierHash get_other_side(const IdentifierHash& id) const;
    BOOST_TEST(sctId.get_other_side(module_0_3_3_m1Hash) == oppositeHash);
    //no module in that direction?
    BOOST_TEST(sctId.get_next_in_eta(endHash).is_valid() == false);
    //bool is_phi_module_max(const Identifier& id) const;
    BOOST_TEST(sctId.is_phi_module_max(barrelIdentifier) == false);
    Identifier maxPhiId = sctId.module_id(0,3,55,-1);
    BOOST_TEST(sctId.is_phi_module_max(maxPhiId));
    //bool is_eta_module_min(const Identifier& id) const;
    Identifier minEtaId = sctId.module_id(0,3,3,-6);
    BOOST_TEST(sctId.is_eta_module_min(minEtaId));
    //bool is_eta_module_max(const Identifier& id) const;
    Identifier maxEtaId = sctId.module_id(0,3,3,-1);
    BOOST_TEST(sctId.is_eta_module_max(maxEtaId));
    //test new methods for neighbours
    //expected return values
    const std::array<IdentifierHash, 5> neighboursByEta{oppositeHash, prevEtaHash, nextEtaHash, prevPhiHash, nextPhiHash};
    const std::array<IdentifierHash, 5> neighboursByPhi{oppositeHash, prevPhiHash, nextPhiHash, prevEtaHash, nextEtaHash};
    //
    BOOST_TEST(sctId.neighbours_by_eta(module_0_3_3_m1Hash) == neighboursByEta, boost::test_tools::per_element());
    BOOST_TEST(sctId.neighbours_by_phi(module_0_3_3_m1Hash) == neighboursByPhi, boost::test_tools::per_element());
    //
    const std::array<std::function< IdentifierHash(const IdentifierHash & )>, 5> & neighbourCalls = sctId.neighbour_calls_by_eta();
    std::array<IdentifierHash, 5> returnedNeighbours;
    std::transform(neighbourCalls.begin(), neighbourCalls.end(), returnedNeighbours.begin(),[module_0_3_3_m1Hash](auto fn){return fn(module_0_3_3_m1Hash);});
    BOOST_TEST_MESSAGE("Test using neighbour_calls_by_eta");
    BOOST_TEST(neighboursByEta == returnedNeighbours, boost::test_tools::per_element());
  }

  
 
  
BOOST_AUTO_TEST_SUITE_END()
