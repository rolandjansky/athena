/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/LArMiniFCAL_ID_test.cxx
 * @author scott snyder
 * @date May 2013
 * @brief Unit test for LArMiniFCAL_ID.
 */

#undef NDEBUG

#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "IdDictParser/IdDictParser.h"
#include "CxxUtils/make_unique.h"
#include <iostream>


#include "hash_test.h"
#include "make_idhelper_common.cxx"


class ILArMiniFCAL_ID_Test
{
public:
  virtual int get_lar_field_value() const = 0;
  virtual int get_lar_fcal_field_value() const = 0;
};


class LArMiniFCAL_ID_Test
  : public LArMiniFCAL_ID, public ILArMiniFCAL_ID_Test
{
public:
  virtual int get_lar_field_value() const override { return lar_field_value(); }
  virtual int get_lar_fcal_field_value() const override { return lar_fcal_field_value(); }
};


// This because test differencing ignored lines containing `0x...'
std::string munghex (const std::string& h)
{
  if (h.size() > 2 && h[0] == '0' && h[1] == 'x')
    return "hex(" + h.substr (2, std::string::npos) + ")";
  return h;
}
void basic_print_id (const LArMiniFCAL_ID& idhelper, const Identifier& id)
{
  std::cout << idhelper.show_to_string (id) << " "
            << munghex(id.getString()) << "\n";
}


void test_basic (const LArMiniFCAL_ID& idhelper)
{
  std::cout << "test_basic\n";

  basic_print_id (idhelper, idhelper.channel_id (2, 0, 0, 0, 0));
  basic_print_id (idhelper, idhelper.channel_id (2, 0, 1, 0, 0));
  basic_print_id (idhelper, idhelper.channel_id (2, 0, 2, 0, 0));

  basic_print_id (idhelper, idhelper.channel_id (-2, 0, 0, 0, 0));
  basic_print_id (idhelper, idhelper.channel_id (-2, 0, 1, 0, 0));
  basic_print_id (idhelper, idhelper.channel_id (-2, 0, 2, 0, 0));
}


class CellCounter
{
public:
  CellCounter();
  void count(int samp);
  void report();


private:
  enum {
    N_SAMP = 4
  };
  unsigned m_counts[N_SAMP];
};


CellCounter::CellCounter()
{
  for (unsigned j=0; j < N_SAMP; j++)
    m_counts[j] = 0;
}


void CellCounter::count (int samp)
{
  if (samp < 0 || samp >= N_SAMP) std::abort();
  ++m_counts[samp];
}


void CellCounter::report()
{
  unsigned tot = 0;
  printf ("Sampling\n");
  for (unsigned s=0; s < N_SAMP; s++) {
    printf ("%1u          %6u\n", s, m_counts[s]);
    tot += m_counts[s];
  }
  printf ("Total      %6u\n", tot);
}


void test_connected (const LArMiniFCAL_ID& idhelper)
{
  std::cout << "test_connected\n";

  HASH_TEST1(channel,minifcal,HASH_TEST_EXTRA(channel));
  HASH_TEST1(module,mod,HASH_TEST_EXTRA(module));

  CellCounter counts;
  TEST_LOOP(Identifier id, idhelper.minifcal_range()) {
    IdentifierHash hashId = idhelper.channel_hash (id);
    assert (hashId == idhelper.channel_hash_binary_search (id));

    assert (idhelper.is_supercell(id) == false);

    int side  = idhelper.pos_neg (id);
    int mod   = idhelper.module (id);
    int depth = idhelper.depth (id);
    int eta   = idhelper.eta (id);
    int phi   = idhelper.phi (id);

    assert (id == idhelper.channel_id (side, mod, depth, eta, phi));

    Identifier mod_id = idhelper.module_id (side, mod);
    assert (id == idhelper.channel_id (mod_id, depth, eta, phi));

    assert (mod_id == idhelper.module_id (id));

    counts.count (depth);

    ExpandedIdentifier exp_id;
    const ILArMiniFCAL_ID_Test& idhelper_test = dynamic_cast<const ILArMiniFCAL_ID_Test&>(idhelper);
    exp_id << idhelper_test.get_lar_field_value()
      	   << idhelper_test.get_lar_fcal_field_value()
	   << idhelper.pos_neg(id)
	   << idhelper.module(id)
	   << idhelper.depth(id)
           << idhelper.eta(id)
           << idhelper.phi(id);
    assert (idhelper.channel_id (exp_id) == id);
  }
  counts.report();

  TEST_LOOP(Identifier id, idhelper.mod_range()) {
    IdentifierHash hashId = idhelper.module_hash(id);
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

    int side  = idhelper.pos_neg (id);
    int mod   = idhelper.module (id);
    assert (id == idhelper.module_id (side, mod));

    ExpandedIdentifier exp_id;
    const ILArMiniFCAL_ID_Test& idhelper_test = dynamic_cast<const ILArMiniFCAL_ID_Test&>(idhelper);
    exp_id << idhelper_test.get_lar_field_value()
      	   << idhelper_test.get_lar_fcal_field_value()
	   << idhelper.pos_neg(id)
	   << idhelper.module(id)
	   << idhelper.depth(id);
    assert (idhelper.module_id (exp_id) == id);
  }
}




void test_exceptions (const LArMiniFCAL_ID& idhelper)
{
  std::cout << "test_exceptions\n";

  bool caught = false;
  try {
    /*Identifier wrongRegionId =*/ idhelper.module_id (99,0); 
  }
  catch(LArID_Exception & except){
    caught = true;
    std::cout << "Exception 1: " << (std::string)except << "\n";
  }
  assert (caught);

  caught = false;
  try {
    /*Identifier wrongChannelId =*/ idhelper.channel_id (2,0,0,0,999);
  }
  catch(LArID_Exception & except){
    caught = true;
    std::cout << "Exception 2: " << (std::string)except << "\n";
  }
  assert (caught);
}


void test_neighbors (const LArMiniFCAL_ID& idhelper)
{
  std::cout << "test_neighbors\n";

  std::vector<IdentifierHash> neighbourList;
  IdentifierHash hash_min = 999999 ;
  IdentifierHash hash_max = 0 ;
  for (unsigned int iCell = 0 ; iCell < idhelper.channel_hash_max(); ++iCell){
    idhelper.get_neighbours(iCell, LArNeighbours::all3D, neighbourList);

    std::vector<IdentifierHash>::iterator first=neighbourList.begin();
    std::vector<IdentifierHash>::iterator last=neighbourList.end();
    for (;last!=first; first++){
      
      IdentifierHash neighbourHash=(*first);
      if(neighbourHash < hash_min ) {
        hash_min = neighbourHash;
      }
      if(neighbourHash > hash_max) {
        hash_max = neighbourHash;
      }
      
    }
  }

  // Not working for now.
  //assert (hash_min == 0);
  //assert (hash_max == idhelper.channel_hash_max()-1);
}


int main()
{
  std::string xmlFile = "IdDictLArCalorimeter_sLHC-MiniFcal-00.xml";
  std::unique_ptr<IdDictParser> parser = make_parser (idDictType, xmlFile);
  IdDictMgr& idd = parser->m_idd;
  idd.add_metadata("FCAL2DNEIGHBORS",     "FCal2DNeighbors-DC3-05-Comm-01.txt");  
  idd.add_metadata("FCAL3DNEIGHBORSNEXT", "FCal3DNeighborsNext-DC3-05-Comm-01.txt");  
  idd.add_metadata("FCAL3DNEIGHBORSPREV", "FCal3DNeighborsPrev-DC3-05-Comm-01.txt");  

  std::unique_ptr<LArMiniFCAL_ID> idhelper = make_helper<LArMiniFCAL_ID_Test>(*parser);
  std::unique_ptr<LArMiniFCAL_ID> idhelper_n = make_helper<LArMiniFCAL_ID_Test>(*parser,true);
  try {
    test_basic (*idhelper);
    test_connected (*idhelper);
    test_exceptions (*idhelper);
    test_neighbors (*idhelper_n);
  }
  catch(LArID_Exception & except){
    std::cout << "Unexpected exception: " << (std::string) except << std::endl ;
  }
  return 0;
}
