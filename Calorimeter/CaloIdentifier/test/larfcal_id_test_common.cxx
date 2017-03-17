/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/larfcal_id_test_common.cxx
 * @author scott snyder
 * @date May 2013
 * @brief Code common between LArFCAL_ID_test and LArFCAL_SuperCell_ID test.
 *        This file is meant to be included in another file,
 *        not compiled separately.
 */


#include "boost/foreach.hpp"
#include "make_idhelper_common.cxx"


// This because test differencing ignored lines containing `0x...'
std::string munghex (const std::string& h)
{
  if (h.size() > 2 && h[0] == '0' && h[1] == 'x')
    return "hex(" + h.substr (2, std::string::npos) + ")";
  return h;
}
void basic_print_id (const LArFCAL_Base_ID& idhelper, const Identifier& id)
{
  std::cout << idhelper.show_to_string (id) << " "
            << munghex(id.getString()) << "\n";
}


class ILArFCAL_ID_Test
{
public:
  virtual int get_lar_field_value() const = 0;
  virtual int get_lar_fcal_field_value() const = 0;
};


class CellCounter
{
public:
  CellCounter();
  void count(int mod);
  void report();


private:
  enum {
    N_MOD = 3
  };
  unsigned m_counts[N_MOD];
};


CellCounter::CellCounter()
{
  for (unsigned i=0; i < N_MOD; i++)
    m_counts[i] = 0;
}


void CellCounter::count (int mod)
{
  if (mod < 1 || mod > N_MOD) std::abort();
  ++m_counts[mod-1];
}


void CellCounter::report()
{
  int tot = 0;
  printf ("  Module counts: ");
  for (unsigned m=0; m < N_MOD; m++) {
    printf ("%u ", m_counts[m]);
    tot += m_counts[m];
  }
  printf ("; total: %d\n", tot);
}


void
test_connected (const LArFCAL_Base_ID& fcal_id, bool supercell = false)
{
  std::cout << "test_connected\n";

  // channels
  IdContext channelContext = fcal_id.channel_context();
  std::vector<Identifier>::const_iterator itId = fcal_id.fcal_begin();
  std::vector<Identifier>::const_iterator itIdEnd = fcal_id.fcal_end();
  std::vector<bool> hashvec(fcal_id.channel_hash_max());
  CellCounter counts;

  std::cout << "  nchan " << itIdEnd - itId << "\n";

  const std::vector<Identifier>& ch_ids = fcal_id.channel_ids();

  int hashsum = 0;
  for(; itId!=itIdEnd; ++itId) {
    Identifier ch_id = *itId;

    IdentifierHash hashId;
    assert (fcal_id.get_hash (ch_id, hashId, &channelContext) == 0);
    Identifier ch_id2;
    assert (fcal_id.get_id (hashId, ch_id2, &channelContext) == 0);
    assert (ch_id == ch_id2);
    assert (ch_id == fcal_id.channel_id (hashId));
    assert (hashId == fcal_id.channel_hash (ch_id));
    assert (hashId == fcal_id.channel_hash_binary_search (ch_id));
    assert (hashId < hashvec.size());
    assert (!hashvec[hashId]);
    hashvec[hashId] = true;
    assert (ch_ids[hashId] == ch_id);

    hashsum += hashId;

    int side = fcal_id.pos_neg(ch_id);
    int mod  = fcal_id.module(ch_id);
    int eta  = fcal_id.eta(ch_id);
    int phi  = fcal_id.phi(ch_id);

    assert (fcal_id.is_supercell(ch_id) == supercell);
      
    ch_id2 = fcal_id.channel_id (side, mod, eta, phi);
    assert (ch_id == ch_id2);

    Identifier mod_id = fcal_id.module_id (side, mod);
    ch_id2 = fcal_id.channel_id (mod_id, eta, phi);
    assert (ch_id == ch_id2);
    assert (mod_id == fcal_id.module_id (ch_id));

    counts.count (mod);

    ExpandedIdentifier exp_id;
    const ILArFCAL_ID_Test& fcal_id_test = dynamic_cast<const ILArFCAL_ID_Test&>(fcal_id);
    exp_id << fcal_id_test.get_lar_field_value()
      	   << fcal_id_test.get_lar_fcal_field_value()
	   << fcal_id.pos_neg(ch_id)
	   << fcal_id.module(ch_id)
           << fcal_id.eta(ch_id)
           << fcal_id.phi(ch_id)
           << (unsigned)fcal_id.is_supercell(ch_id);
    assert (fcal_id.channel_id (exp_id) == ch_id);
 }
  for (size_t i = 0; i < hashvec.size(); i++)
    assert (hashvec[i]);

#if __cplusplus > 201100
  for (Identifier ch_id : fcal_id.fcal_range()) {
#else
  BOOST_FOREACH (Identifier ch_id, fcal_id.fcal_range()) {
#endif
    hashsum -= fcal_id.channel_hash (ch_id);
  }
  assert (hashsum == 0);


  // modules
  itId = fcal_id.mod_begin();
  itIdEnd = fcal_id.mod_end();
  hashvec.clear();
  hashvec.resize (fcal_id.module_hash_max());
  IdContext module_context = fcal_id.module_context();
  
  std::cout << "  nreg " << itIdEnd - itId << "\n";

  for(; itId!=itIdEnd; ++itId) {
    Identifier mod_id = *itId;
    
    assert (fcal_id.is_supercell(mod_id) == supercell);

    IdentifierHash modHash = fcal_id.module_hash(mod_id);
    hashsum += modHash;
#if 1
    printf ("Module %-18s eta: %8.6f %8.6f %1d %3d phi: %3.1f %8.6f %1d %3d\n",
            fcal_id.show_to_string(mod_id).c_str(),
            fcal_id.eta0(modHash),
            fcal_id.etaGranularity(modHash),
            fcal_id.eta_min(mod_id),
            fcal_id.eta_max(mod_id),
            fcal_id.phi0(modHash),
            fcal_id.phiGranularity(modHash),
            fcal_id.phi_min(mod_id),
            fcal_id.phi_max(mod_id));
#endif
    Identifier mod_id2 = fcal_id.module_id(modHash);
    assert (mod_id == mod_id2);

    IdentifierHash modHash2;
    assert (fcal_id.get_hash (mod_id, modHash2, &module_context) == 0);

    assert (!hashvec[modHash]);
    hashvec[modHash] = true;

    ExpandedIdentifier exp_id;
    const ILArFCAL_ID_Test& fcal_id_test = dynamic_cast<const ILArFCAL_ID_Test&>(fcal_id);
    exp_id << fcal_id_test.get_lar_field_value()
      	   << fcal_id_test.get_lar_fcal_field_value()
	   << fcal_id.pos_neg(mod_id)
	   << fcal_id.module(mod_id);
    assert (fcal_id.module_id (exp_id) == mod_id);
  }
  for (size_t i = 0; i < hashvec.size(); i++)
    assert (hashvec[i]);

#if __cplusplus > 201100
  for (Identifier ch_id : fcal_id.mod_range()) {
#else
  BOOST_FOREACH (Identifier ch_id, fcal_id.mod_range()) {
#endif
    hashsum -= fcal_id.module_hash (ch_id);
  }
  assert (hashsum == 0);

  counts.report();
}




void test_exceptions (const LArFCAL_Base_ID& fcal_id,
                      bool /*supercell*/ = false)
{
  std::cout << "test_exeptions\n";

  bool caught = false;
  try {
    /*Identifier wrongRegionId =*/ fcal_id.module_id (0,99); 
  }
  catch(LArID_Exception & except){
    caught = true;
    std::cout << "Exception 1: " << (std::string)except << "\n";
  }
  assert (caught);

  caught = false;
  try {
    /*Identifier wrongChannelId =*/ fcal_id.channel_id (0,99,0,0); 
  }
  catch(LArID_Exception & except){
    caught = true;
    std::cout << "Exception 2: " << (std::string)except << "\n";
  }
  assert (caught);

  caught = false;
  try {
    Identifier goodRegionId = fcal_id.module_id (1,1); 
    /*Identifier wrongChannelId =*/ fcal_id.channel_id (goodRegionId,0,-99); 
  }
  catch(LArID_Exception & except){
    caught = true;
    std::cout << "Exception 3: " << (std::string)except << "\n";
  }
  assert (caught);
}


