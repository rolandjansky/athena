/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/larem_id_test_common.cxx
 * @author Walter Hopkins (based on scott snyder's example)
 * @date Oct 2014
 * @brief Code common between GTower_ID_test and JTower_ID test.
 *        This file is meant to be included in another file,
 *        not compiled separately.
 */


#include "boost/foreach.hpp"


// This because test differencing ignored lines containing `0x...'
std::string munghex (const std::string& h)
{
  if (h.size() > 2 && h[0] == '0' && h[1] == 'x')
    return "hex(" + h.substr (2, std::string::npos) + ")";
  return h;
}
void basic_print_id (const JGTowerBase_ID& idhelper, const Identifier& id)
{
  std::cout << idhelper.show_to_string (id) << " "
            << munghex(id.getString()) << "\n";
}


class GTower_ID_Test
  : public JGTowerBase_ID
{
  // public:
  //   using JGTowerBase_ID::lar_field_value;
  //   using JGTowerBase_ID::lar_em_field_value;
};


class CellCounter
{
public:
  CellCounter();
  void count(int reg);
  void report();


private:
  enum {
    N_REG = 10
  };
  unsigned m_counts[N_REG];
};

CellCounter::CellCounter()
{
  for (unsigned i=0; i < N_REG; i++)
    m_counts[i] = 0;
}

void CellCounter::count (int reg)
{
  if (reg < 0 || reg >= N_REG) std::abort();
  ++m_counts[reg];
}

void CellCounter::report()
{
  int towerSum = 0;
  for(int regI=0; regI<N_REG; ++regI){
    printf ("Region %2d: %6u\n", regI, m_counts[regI]);
    towerSum+=m_counts[regI];
  }
  printf ("Total: %6d\n", towerSum);
}



void test_towers (const JGTowerBase_ID& tower_id)
{
  std::cout << "test_towers\n";

  // towers
  IdContext towerContext = tower_id.tower_context();
  std::vector<Identifier>::const_iterator itId = tower_id.tower_begin();
  std::vector<Identifier>::const_iterator itIdEnd = tower_id.tower_end();
  std::vector<bool> hashvec(tower_id.tower_hash_max());
  CellCounter counts;

  std::cout << "  nTowers " << itIdEnd - itId << "\n";

  int hashsum = 0;
  for(; itId!=itIdEnd; ++itId) {
    Identifier t_id = *itId;
  
    IdentifierHash hashId;
    assert (tower_id.get_hash (t_id, hashId, &towerContext) == 0);
    Identifier t_id2;
    assert (tower_id.get_id (hashId, t_id2, &towerContext) == 0);
    assert (t_id == t_id2);
    assert (t_id == tower_id.tower_id (hashId));
    assert (hashId == tower_id.tower_hash (t_id));
    assert (hashId == tower_id.tower_hash_binary_search (t_id));
    assert (hashId < hashvec.size());
    assert (!hashvec[hashId]);
    hashvec[hashId] = true;

    hashsum += hashId;

    int pos_neg = tower_id.pos_neg(t_id);
    int sampling = tower_id.sampling(t_id);
    int reg  = tower_id.region(t_id);
    int eta  = tower_id.eta(t_id);
    int phi  = tower_id.phi(t_id);

    t_id2 = tower_id.tower_id (pos_neg, sampling, reg, eta, phi);
    assert (t_id == t_id2);
    counts.count(reg);
  }
  for (size_t i = 0; i < hashvec.size(); i++)
    assert (hashvec[i]);

#if __cplusplus > 201100
  for (Identifier t_id : tower_id.tower_range()) {
#else
    BOOST_FOREACH (Identifier t_id, tower_id.tower_range()) {
#endif
      hashsum -= tower_id.tower_hash (t_id);
    }
    assert (hashsum == 0);

      counts.report();
  }


  void test_exceptions (const JGTowerBase_ID& tower_id, int detSide)
{
  std::cout << "test_exceptions\n";
  bool caught = false;
  try {
    /*Identifier wrongRegionId =*/ tower_id.region_id (0,0,99); 
  }
  catch(CaloID_Exception & except){
    caught = true;
    std::cout << "Exception 1: " << (std::string)except << "\n";
  }
  assert (caught);

  caught = false;
  try {
    /*Identifier wrongChannelId =*/ tower_id.tower_id (0,0,0,0,99); 
  }
  catch(CaloID_Exception & except){
    caught = true;
    std::cout << "Exception 2: " << (std::string)except << "\n";
  }
  assert (caught);

  caught = false;
  try {
    Identifier goodRegionId = tower_id.region_id (detSide,0, 0); 
    /*Identifier wrongChannelId =*/ tower_id.tower_id (goodRegionId,0,-99); 
  }
  catch(CaloID_Exception & except){
    caught = true;
    std::cout << "Exception 3: " << (std::string)except << "\n";
  }
  assert (caught);
}


// neighbors
void test4 (const JGTowerBase_ID& idhelper)
{
  std::cout << "test4\n";

  IdentifierHash prevPhiHash;
  IdentifierHash nextPhiHash;
  IdentifierHash prevEtaHash;
  IdentifierHash nextEtaHash;
  IdentifierHash curHash;
  IdentifierHash tmpHash;

  Identifier curId;

  // towers
  IdContext towerContext = idhelper.tower_context();
  std::vector<Identifier>::const_iterator itId = idhelper.tower_begin();
  std::vector<Identifier>::const_iterator itIdEnd = idhelper.tower_end();
 
  for(; itId!=itIdEnd; ++itId) {
    Identifier t_id = *itId;
  
    assert (idhelper.get_hash (t_id, curHash, &towerContext) == 0);

    // Check that previous phi works
    assert (idhelper.get_prev_in_phi(curHash, prevPhiHash) == 0);
    assert (idhelper.get_next_in_phi(prevPhiHash, tmpHash) == 0);
    Identifier t_id2;
    assert (idhelper.get_id (tmpHash, t_id2, &towerContext) == 0);
    assert (t_id == t_id2);

    // Check next phi
    assert (idhelper.get_next_in_phi(curHash, nextPhiHash) == 0);
    assert (idhelper.get_prev_in_phi(nextPhiHash, tmpHash) == 0);
    assert (idhelper.get_id (tmpHash, t_id2, &towerContext) == 0);
    assert (t_id == t_id2);

// Check that previous eta works
    if (!idhelper.get_prev_in_eta(curHash, prevEtaHash)){
      assert (idhelper.get_next_in_eta(prevEtaHash, tmpHash) == 0);
      assert (idhelper.get_id (tmpHash, t_id2, &towerContext) == 0);
      assert (t_id == t_id2);
    }
    else{
      std::cout << "No previous tower in eta found for hash: " << curHash << std::endl;
    }

    // Check next eta
    if (!idhelper.get_next_in_eta(curHash, nextEtaHash)){;
    assert (idhelper.get_prev_in_eta(nextEtaHash, tmpHash) == 0);
    assert (idhelper.get_id (tmpHash, t_id2, &towerContext) == 0);
    assert (t_id == t_id2);
    }
else{
  std::cout << "No next tower in eta found for hash: " << curHash << std::endl;
    }
  }
}

 void testRegionInfo (const JGTowerBase_ID& idhelper)
 {
   IdContext regionContext = idhelper.region_context();

   BOOST_FOREACH (Identifier r_id, idhelper.reg_range()) {
  
     //IdentifierHash hashId;
     // assert (idhelper.get_hash (r_id, hashId, &regionContext) == 0);
       std::cout << "(deta,dphi) and (eta0,phi0) is: (" << idhelper.etaGranularity(r_id) 
		 << "," << idhelper.phiGranularity(r_id) << "), (" 
		 << idhelper.eta0(r_id) << "," << idhelper.phi0(r_id) 
		 << ")" << std::endl;
   }
 }
