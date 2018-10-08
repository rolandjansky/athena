/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/larhec_id_test_common.cxx
 * @author scott snyder
 * @date May 2013
 * @brief Code common between LArHEC_ID_test and LArHEC_SuperCell_ID test.
 *        This file is meant to be included in another file,
 *        not compiled separately.
 */


#include "boost/foreach.hpp"
#include "hash_test.h"
#include "make_idhelper_common.cxx"


// This because test differencing ignored lines containing `0x...'
std::string munghex (const std::string& h)
{
  if (h.size() > 2 && h[0] == '0' && h[1] == 'x')
    return "hex(" + h.substr (2, std::string::npos) + ")";
  return h;
}
void basic_print_id (const LArHEC_Base_ID& idhelper, const Identifier& id)
{
  std::cout << idhelper.show_to_string (id) << " "
            << munghex(id.getString()) << "\n";
}


class ILArHEC_ID_Test
{
public:
  virtual int get_lar_field_value() const = 0;
  virtual int get_lar_hec_field_value() const = 0;
};


class CellCounter
{
public:
  CellCounter();
  void count(int bec, int samp);
  void report();


private:
  enum {
    N_REG = 2,
    N_SAMP = 4
  };
  unsigned m_counts[N_REG][N_SAMP];
};


CellCounter::CellCounter()
{
  for (unsigned i=0; i < N_REG; i++)
    for (unsigned j=0; j < N_SAMP; j++)
      m_counts[i][j] = 0;
}


void CellCounter::count (int reg, int samp)
{
  if (reg < 0 || reg >= N_REG) std::abort();
  if (samp < 0 || samp >= N_SAMP) std::abort();
  ++m_counts[reg][samp];
}


void CellCounter::report()
{
  unsigned tot[N_REG] = {0};
  printf ("Sampling   Outer         Inner\n");
  for (unsigned s=0; s < N_SAMP; s++) {
    printf ("%1u          %6d        %6d\n",
            s, m_counts[0][s], m_counts[1][s]);
    for (unsigned b=0; b < N_REG; b++)
      tot[b] += m_counts[b][s];
  }
  printf ("Total      %6u        %6u   Grand total: %6u\n",
          tot[0], tot[1],
          tot[0]+tot[1]);
}


void test_connected (const LArHEC_Base_ID& idhelper, bool supercell)
{
  std::cout << "test_connected\n";

  HASH_TEST1(channel,hec,HASH_TEST_EXTRA(channel));
  HASH_TEST1(region,reg,HASH_TEST_EXTRA(region));

  CellCounter counts;
  TEST_LOOP (Identifier id, idhelper.hec_range()) {
    assert (idhelper.is_lar_hec (id));
    IdentifierHash hashId = idhelper.channel_hash (id);
    assert (hashId == idhelper.channel_hash_binary_search (id));

    int side = idhelper.pos_neg(id);
    int samp = idhelper.sampling(id);
    int reg  = idhelper.region(id);
    int eta  = idhelper.eta(id);
    int phi  = idhelper.phi(id);

    assert (idhelper.is_supercell(id) == supercell);
      
    Identifier id2 = idhelper.channel_id (side, samp, reg, eta, phi);
    assert (id == id2);

    Identifier reg_id = idhelper.region_id (side, samp, reg);
    id2 = idhelper.channel_id (reg_id, eta, phi);
    assert (id == id2);
    assert (reg_id == idhelper.region_id (id));

    counts.count (reg, samp);

    ExpandedIdentifier exp_id;
    const ILArHEC_ID_Test& idhelper_test = dynamic_cast<const ILArHEC_ID_Test&>(idhelper);
    exp_id << idhelper_test.get_lar_field_value()
      	   << idhelper_test.get_lar_hec_field_value()
	   << idhelper.pos_neg(id)
	   << idhelper.sampling(id)
	   << idhelper.region(id)
           << idhelper.eta(id)
           << idhelper.phi(id)
           << (unsigned)idhelper.is_supercell(id);
    assert (idhelper.channel_id (exp_id) == id);
  }
  counts.report();

  TEST_LOOP (Identifier id, idhelper.reg_range()) {
    assert (idhelper.is_supercell(id) == supercell);
    IdentifierHash hashId = idhelper.region_hash (id);

    printf ("Region %-18s eta: %8.6f %8.6f %1d %3d phi: %3.1f %8.6f %1d %3d\n",
            idhelper.show_to_string(id).c_str(),
            idhelper.eta0(hashId),
            idhelper.etaGranularity(hashId),
            idhelper.eta_min(id),
            idhelper.eta_max(id),
            idhelper.phi0(hashId),
            idhelper.phiGranularity(hashId),
            idhelper.phi_min(id),
            idhelper.phi_max(id));

    int side = idhelper.pos_neg(id);
    int samp = idhelper.sampling(id);
    int reg  = idhelper.region(id);
    Identifier id2 = idhelper.region_id (side, samp, reg);
    assert (id == id2);

    ExpandedIdentifier exp_id;
    const ILArHEC_ID_Test& idhelper_test = dynamic_cast<const ILArHEC_ID_Test&>(idhelper);
    exp_id << idhelper_test.get_lar_field_value()
      	   << idhelper_test.get_lar_hec_field_value()
	   << idhelper.pos_neg(id)
	   << idhelper.sampling(id)
	   << idhelper.region(id);
    assert (idhelper.region_id (exp_id) == id);
  }
}



void test_exceptions (const LArHEC_Base_ID& idhelper)
{
  std::cout << "test_exceptions\n";

  bool caught = false;
  try {
    /*Identifier wrongRegionId =*/ idhelper.region_id (0,99,0); 
  }
  catch(LArID_Exception & except){
    caught = true;
    std::cout << "Exception 1: " << (std::string)except << "\n";
  }
  assert (caught);

  caught = false;
  try {
    /*Identifier wrongChannelId =*/ idhelper.channel_id (0,99,0,0,0); 
  }
  catch(LArID_Exception & except){
    caught = true;
    std::cout << "Exception 2: " << (std::string)except << "\n";
  }
  assert (caught);

  caught = false;
  try {
    Identifier goodRegionId = idhelper.region_id (2,0,0); 
    /*Identifier wrongChannelId =*/ idhelper.channel_id (goodRegionId,0,-99); 
  }
  catch(LArID_Exception & except){
    caught = true;
    std::cout << "Exception 3: " << (std::string)except << "\n";
  }
  assert (caught);
}


// hec-specific addressing.
void test_hec (const LArHEC_Base_ID& idhelper)
{
  std::cout << "test_hec\n";

  assert (idhelper.channel_id (2, 0, 0, 5, 1) ==
          idhelper.channel_id (2, 0,    5, 1));
  assert (idhelper.channel_id (2, 0, 1, 3, 1) ==
          idhelper.channel_id (2, 0,   13, 1));

  assert (idhelper.channel_id (2, 0, 0, 5, 1) ==
          idhelper.channel_id (2, 0, 0, 0, 5, 1));
  assert (idhelper.channel_id (2, 0, 0, 5, 10) ==
          idhelper.channel_id (2, 0, 5, 0, 5, 0));
  assert (idhelper.channel_id (2, 0, 0, 5, 11) ==
          idhelper.channel_id (2, 0, 5, 0, 5, 1));
}
