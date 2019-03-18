/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/calocell_id_test_common.cxx
 * @author scott snyder
 * @date Aug 2012
 * @brief Code common between CaloCell_ID_test and CaloCell_SuperCell_ID test.
 *        This file is meant to be included in another file,
 *        not compiled separately.
 */


#include "IdDictParser/IdDictParser.h"
#include "CxxUtils/make_unique.h"
#include "boost/foreach.hpp"
#include <map>
#include <string>


#include "make_idhelper_common.cxx"


class CellCounter
{
public:
  CellCounter();
  void count(int bec, int samp);
  void report() const;


private:
  enum {
    N_BEC = 3,
    N_SAMP = 4
  };
  unsigned m_counts[N_BEC][N_SAMP];
};


CellCounter::CellCounter()
{
  for (unsigned i=0; i < N_BEC; i++)
    for (unsigned j=0; j < N_SAMP; j++)
      m_counts[i][j] = 0;
}


void CellCounter::count (int bec, int samp)
{
  bec = std::abs(bec) - 1;
  if (bec < 0 || bec >= N_BEC) std::abort();
  if (samp < 0 || samp >= N_SAMP) std::abort();
  ++m_counts[bec][samp];
}


void CellCounter::report() const
{
  unsigned tot[N_BEC] = {0};
  printf ("Sampling   Barrel   Outer wheel   Inner wheel\n");
  for (unsigned s=0; s < N_SAMP; s++) {
    printf ("%1u          %6d        %6d        %6d\n",
            s, m_counts[0][s], m_counts[1][s], m_counts[2][s]);
    for (unsigned b=0; b < N_BEC; b++)
      tot[b] += m_counts[b][s];
  }
  printf ("Total      %6u        %6u        %6u   Grand total: %6u\n",
          tot[0], tot[1], tot[2],
          tot[0]+tot[1]+tot[2]);
}


class TileCounter
{
public:
  TileCounter();
  void count(int section, int side);
  void report() const;


private:
  enum {
    N_SECTION = 4,
    N_SIDE = 2
  };
  unsigned m_counts[N_SECTION][N_SIDE];
};


TileCounter::TileCounter()
{
  for (unsigned i=0; i < N_SECTION; i++)
    for (unsigned j=0; j < N_SIDE; j++)
      m_counts[i][j] = 0;
}


void TileCounter::count (int section, int side)
{
  section -= 1;
  side = (side+1) / 2;
  if (section < 0 || section >= N_SECTION) std::abort();
  if (side < 0 || side >= N_SIDE) std::abort();
  ++m_counts[section][side];
}


void TileCounter::report() const
{
  unsigned tot[N_SECTION] = {0};
  printf ("Side    Barrel   Extended Barrel   Gap     Ancillary\n");
  for (unsigned s=0; s < N_SIDE; s++) {
    printf ("%1u       %6d   %6d            %6d  %6d   \n",
            s, m_counts[0][s], m_counts[1][s], m_counts[2][s], m_counts[3][s]);
    for (unsigned b=0; b < N_SECTION; b++)
      tot[b] += m_counts[b][s];
  }
  printf ("Total   %6u   %6u            %6u  %6u   Grand total: %6u\n",
          tot[0], tot[1], tot[2], tot[3],
          tot[0]+tot[1]+tot[2]+tot[3]);
}


class CellCounters
{
public:
  void count(const char* type, int bec, int samp);
  void report() const;

private:
  std::map<std::string, CellCounter> m_counters;
  TileCounter m_tilecounter;
};


void CellCounters::count (const char* type, int bec, int samp)
{
  if (std::string(type) == "TILE")
    m_tilecounter.count (bec, samp);
  else
    m_counters[type].count (bec, samp);
}


void CellCounters::report() const
{
  for (std::map<std::string, CellCounter>::const_iterator i= m_counters.begin();
       i != m_counters.end();
       ++i)
  {
    std::cout << "======\n";
    std::cout << i->first << "\n";
    i->second.report();
    std::cout << "\n";
  }
  std::cout << "======\n";
  std::cout << "Tile\n";
  m_tilecounter.report();
  std::cout << "\n";
  
}


#if 0
template <class T>
T* make_helper (IdDictMgr& idd, bool do_neighbours)
{
  T* h = new T;
  h->set_do_neighbours (do_neighbours);
  assert (h->initialize_from_dictionary (idd) == 0);
  return h;
}
#endif


template <class CALOCELL_ID_T,
          class LAREM_ID_T,
          class LARHEC_ID_T,
          class LARFCAL_ID_T,
          class TILE_ID_T>
std::unique_ptr<CALOCELL_ID_T> make_calo_id_t (IdDictParser& parser,
                                               bool do_neighbours = false)
{
  IdDictMgr& idd = parser.m_idd;

  // Set some default file names for neighbours (RDS 12/2009):
  if (!do_neighbours) {
    idd.add_metadata("FULLATLASNEIGHBORS",  "SuperCaloNeighbours-DC3-05-Comm-01.dat");  
    idd.add_metadata("FCAL2DNEIGHBORS",     "FCal2DNeighbors-DC3-05-Comm-01.txt");  
    idd.add_metadata("FCAL3DNEIGHBORSNEXT", "FCal3DNeighborsNext-DC3-05-Comm-01.txt");  
    idd.add_metadata("FCAL3DNEIGHBORSPREV", "FCal3DNeighborsPrev-DC3-05-Comm-01.txt");  
    idd.add_metadata("TILENEIGHBORS",       "TileNeighbour_reduced.txt");  
  }

  std::unique_ptr<LAREM_ID_T> em_id = make_helper<LAREM_ID_T> (parser,
                                                               do_neighbours,
                                                               false);
  std::unique_ptr<LARHEC_ID_T> hec_id = make_helper<LARHEC_ID_T> (parser,
                                                                  do_neighbours);
  std::unique_ptr<LARFCAL_ID_T> fcal_id = make_helper<LARFCAL_ID_T> (parser,
                                                                     do_neighbours);

#if 0
  IdDictParser* miniparser = new IdDictParser;
  miniparser->register_external_entity ("LArCalorimeter",
                                        "IdDictLArCalorimeter_sLHC-MiniFcal-00.xml");
  IdDictMgr& mini_idd = miniparser->parse ("IdDictParser/ATLAS_IDS.xml");
  mini_idd.add_metadata("FCAL2DNEIGHBORS",     "FCal2DNeighbors-DC3-05-Comm-01.txt");  
  mini_idd.add_metadata("FCAL3DNEIGHBORSNEXT", "FCal3DNeighborsNext-DC3-05-Comm-01.txt");  
  mini_idd.add_metadata("FCAL3DNEIGHBORSPREV", "FCal3DNeighborsPrev-DC3-05-Comm-01.txt");  
#endif
  // Adding in MiniFCAL doesn't really work, because MiniFCAL and FCAL
  // overlap ID assignments!
  std::unique_ptr<LArMiniFCAL_ID> minifcal_id = make_helper<LArMiniFCAL_ID> (parser,do_neighbours);

  std::unique_ptr<TILE_ID_T> tile_id = make_helper<TILE_ID_T> (parser, do_neighbours);

  auto calo_id = CxxUtils::make_unique<CALOCELL_ID_T> (em_id.release(),
                                                       hec_id.release(),
                                                       fcal_id.release(),
                                                       minifcal_id.release(),
                                                       tile_id.release());
  assert (calo_id->initialize_from_dictionary (idd) == 0);

  //assert (calo_id->em_idHelper() == em_id);
  //assert (calo_id->hec_idHelper() == hec_id);
  //assert (calo_id->fcal_idHelper() == fcal_id);
  //assert (calo_id->minifcal_idHelper() == minifcal_id);
  //assert (calo_id->tile_idHelper() == tile_id);

  return calo_id;
}


void region_test (const CaloCell_Base_ID& calo_id, const Identifier& ch_id)
{
  IdentifierHash hashId = calo_id.calo_cell_hash ( ch_id );

  assert (calo_id.sub_calo(ch_id) == calo_id.sub_calo(hashId));
  assert (calo_id.calo_sample(ch_id) == calo_id.calo_sample(hashId));
  assert (calo_id.is_em(ch_id) == calo_id.is_em(hashId));
  assert (calo_id.is_hec(ch_id) == calo_id.is_hec(hashId));
  assert (calo_id.is_fcal(ch_id) == calo_id.is_fcal(hashId));
  assert (calo_id.is_minifcal(ch_id) == calo_id.is_minifcal(hashId));
  assert (calo_id.is_tile(ch_id) == calo_id.is_tile(hashId));

  if (calo_id.is_em (ch_id)) {
    assert (calo_id.sub_calo (hashId) == CaloCell_Base_ID::LAREM);
    int bec  = calo_id.pos_neg(ch_id);
    switch (bec) {
    case 1:
    case -1:
      assert (calo_id.is_em_barrel (ch_id));
      assert (!calo_id.is_em_endcap (ch_id));
      assert (!calo_id.is_em_endcap_inner (ch_id));
      assert (!calo_id.is_em_endcap_outer (ch_id));
      break;
    case 2:
    case -2:
      assert (!calo_id.is_em_barrel (ch_id));
      assert (calo_id.is_em_endcap (ch_id));
      assert (!calo_id.is_em_endcap_inner (ch_id));
      assert (calo_id.is_em_endcap_outer (ch_id));
      break;
    case 3:
    case -3:
      assert (!calo_id.is_em_barrel (ch_id));
      assert (calo_id.is_em_endcap (ch_id));
      assert (calo_id.is_em_endcap_inner (ch_id));
      assert (!calo_id.is_em_endcap_outer (ch_id));
      break;
    default:
      abort();
    }
  }

  else if (calo_id.is_hec (ch_id)) {
    assert (calo_id.sub_calo (hashId) == CaloCell_Base_ID::LARHEC);
  }

  else if (calo_id.is_fcal (ch_id)) {
    assert (calo_id.sub_calo (hashId) == CaloCell_Base_ID::LARFCAL);
  }

  else if (calo_id.is_minifcal (ch_id)) {
    assert (calo_id.sub_calo (hashId) == CaloCell_Base_ID::LARMINIFCAL);
  }

  else if (calo_id.is_tile (ch_id)) {
    assert (calo_id.sub_calo (hashId) == CaloCell_Base_ID::TILE);
    switch (calo_id.section (ch_id)) {
    case 1:
      assert (calo_id.is_tile_barrel (ch_id));
      assert (!calo_id.is_tile_extbarrel (ch_id));
      assert (!calo_id.is_tile_gap (ch_id));
      break;

    case 2:
      assert (!calo_id.is_tile_barrel (ch_id));
      assert (calo_id.is_tile_extbarrel (ch_id));
      assert (!calo_id.is_tile_gap (ch_id));
      break;

    case 3:
      assert (!calo_id.is_tile_barrel (ch_id));
      assert (!calo_id.is_tile_extbarrel (ch_id));
      assert (calo_id.is_tile_gap (ch_id));
      break;

    case 4:
      assert (!calo_id.is_tile_barrel (ch_id));
      assert (!calo_id.is_tile_extbarrel (ch_id));
      assert (!calo_id.is_tile_gap (ch_id));
      break;


    default:
      std::abort();
    }

    switch (calo_id.side (ch_id)) {
    case 1:
      assert (calo_id.is_tile_positive (ch_id));
      assert (!calo_id.is_tile_negative (ch_id));
      break;

    case -1:
      assert (!calo_id.is_tile_positive (ch_id));
      assert (calo_id.is_tile_negative (ch_id));
      break;

    default:
      std::abort();
    }

    assert (calo_id.is_tile_gapscin(ch_id) ==
            (calo_id.sample (ch_id) == TileID::SAMP_E));
  }

  else
    std::abort();
}


void test_cells (const CaloCell_Base_ID& calo_id, bool supercell = false)
{
  std::cout << "test_cells\n";

  std::vector<Identifier>::const_iterator itId = calo_id.cell_begin();
  std::vector<Identifier>::const_iterator itIdEnd = calo_id.cell_end();
  CellCounters counts;
  assert ((itIdEnd - itId) == (int)calo_id.calo_cell_hash_max());

  std::vector<int> hash_counts (calo_id.calo_cell_hash_max());

  if (itId != itIdEnd)
    std::cout << calo_id.show_to_string (*itId) << "\n";

  int hashsum = 0;
  for(; itId!=itIdEnd; ++itId) {
    Identifier chId = *itId;
    region_test (calo_id, chId);

    int subcalo0 = -1;

    if (calo_id.is_em (chId)) {
      counts.count ("EM", calo_id.pos_neg(chId), calo_id.sampling(chId));
      subcalo0 = CaloCell_Base_ID::LAREM;
      assert (calo_id.is_supercell (chId) == supercell);
    }

    else if (calo_id.is_hec (chId)) {
      counts.count ("HEC", calo_id.pos_neg(chId), calo_id.sampling(chId));
      subcalo0 = CaloCell_Base_ID::LARHEC;
      assert (calo_id.is_supercell (chId) == supercell);
    }

    else if (calo_id.is_fcal (chId)) {
      counts.count ("FCAL", calo_id.pos_neg(chId), calo_id.sampling(chId));
      subcalo0 = CaloCell_Base_ID::LARFCAL;
      assert (calo_id.is_supercell (chId) == supercell);
    }

    else if (calo_id.is_minifcal (chId)) {
      counts.count ("MINIFCAL", calo_id.pos_neg(chId), calo_id.sampling(chId));
      subcalo0 = CaloCell_Base_ID::LARMINIFCAL;
      assert (calo_id.is_supercell (chId) == 0);
    }

    else if (calo_id.is_tile (chId)) {
      counts.count ("TILE", calo_id.section(chId), calo_id.side(chId));
      subcalo0 = CaloCell_Base_ID::TILE;
      assert (calo_id.is_supercell (chId) == supercell);
    }

    else
      std::abort();

    IdentifierHash hashId = calo_id.calo_cell_hash ( chId );
    int subcalo1 = calo_id.sub_calo(hashId);
    hashsum += hashId;

    assert (hashId < hash_counts.size());
    assert (hash_counts[hashId] == 0);
    hash_counts[hashId] = 1;

    assert (subcalo1 == subcalo0);

    IdentifierHash min=0, max=0;
    calo_id.calo_cell_hash_range(chId, min, max);

    IdentifierHash min2=0, max2=0;
    calo_id.calo_cell_hash_range(subcalo1, min2, max2);

    assert (min == min2);
    assert (max == max2);

    if (subcalo0 != -1) {
      int subcalo2=-1;
      IdentifierHash emHash = calo_id.subcalo_cell_hash(hashId, subcalo2) ;
      assert (subcalo2 == subcalo1);
      IdentifierHash caloCellHash = calo_id.calo_cell_hash(subcalo0, emHash) ;
      assert (hashId == caloCellHash);
    }

    Identifier chanId1 = calo_id.cell_id ( hashId );
    assert (chanId1 == chId);

    int subcalo3 = calo_id.sub_calo(chId);
    assert (subcalo3 == subcalo1);

    int bec    = 888888;
    int samp   = 888888;
    int reg    = 888888;
    int eta    = 888888;
    int phi    = 888888;
    if(calo_id.is_lar(chId)) {  
      bec    = calo_id.pos_neg(chId);
      samp   = calo_id.sampling(chId);
      reg    = calo_id.region(chId);
      eta    = calo_id.eta(chId);
      phi    = calo_id.phi(chId);
    }
    else  if(calo_id.is_tile(chId)) {
      bec    = calo_id.section(chId);
      samp   = calo_id.side(chId);
      reg    = calo_id.module(chId);
      eta    = calo_id.tower(chId);
      phi    = calo_id.sample(chId);
    }
    else
      std::abort();

    Identifier chanId3 = calo_id.cell_id (subcalo3, bec, samp, reg, eta, phi);
    assert (chanId3 == chId);

    Identifier regId = calo_id.region_id (chId);
    if(!calo_id.is_tile(chId)) {           
      Identifier chanId2 = calo_id.cell_id (regId, eta, phi);
      assert (chanId2 == chId);
    }

    int subcalo4=-1;
    IdentifierHash subHashId = calo_id.subcalo_cell_hash (chId, subcalo4) ;
    assert (subcalo4 == subcalo1);

    Identifier chanId4 = calo_id.cell_id ( subcalo3, subHashId );
    assert (chanId4 == chId);
  }

#if __cplusplus > 201100
  for (Identifier ch_id : calo_id.cell_range()) {
#else
  BOOST_FOREACH (Identifier ch_id, calo_id.cell_range()) {
#endif
    hashsum -= calo_id.calo_cell_hash (ch_id);
  }
  assert (hashsum == 0);

  for (int i = 0; i < CaloCell_Base_ID::NSUBCALO; i++) {
    IdentifierHash min=0, max=0;
    calo_id.calo_cell_hash_range (i, min, max);
    for (; min < max; min=min+1) {
      assert (min < hash_counts.size());
      assert (hash_counts[min] == 1);
      ++hash_counts[min];
    }
  }

  for (size_t i = 0; i < hash_counts.size(); i++)
    assert (hash_counts[i] == 2);

  counts.report();
}


void test_sample (const CaloCell_Base_ID& calo_id, bool supercell)
{
  std::cout << "test_sample\n";

  Identifier id = calo_id.cell_id (CaloCell_Base_ID::LAREM,
                                    1, 1, 0, 1, 1);
  assert (calo_id.calo_sample (id) == CaloCell_Base_ID::EMB1);
  assert (calo_id.calo_sample (calo_id.calo_cell_hash(id)) ==
          CaloCell_Base_ID::EMB1);

  id = calo_id.cell_id (CaloCell_Base_ID::LAREM,
                         2, 1, 0, 0, 1);
  assert (calo_id.calo_sample (id) == CaloCell_Base_ID::EME1);
  assert (calo_id.calo_sample (calo_id.calo_cell_hash(id)) ==
          CaloCell_Base_ID::EME1);

  id = calo_id.cell_id (CaloCell_Base_ID::LAREM,
                         3, 1, 0, 1, 1);
  assert (calo_id.calo_sample (id) == CaloCell_Base_ID::EME2);
  assert (calo_id.calo_sample (calo_id.calo_cell_hash(id)) ==
          CaloCell_Base_ID::EME2);

  id = calo_id.cell_id (CaloCell_Base_ID::LARHEC,
                         2, 0, 0, 1, 1);
  assert (calo_id.calo_sample (id) == CaloCell_Base_ID::HEC0);
  assert (calo_id.calo_sample (calo_id.calo_cell_hash(id)) ==
          CaloCell_Base_ID::HEC0);

#if 0
  id = calo_id.cell_id (CaloCell_Base_ID::LARMINIFCAL,
                         2, 0, 0, 1, 1);
  std::cout << "xxx2 " << calo_id.calo_sample (id) << "\n";
  //assert (calo_id.calo_sample (id) == CaloCell_Base_ID::HEC1);
  //assert (calo_id.calo_sample (calo_id.calo_cell_hash(id)) ==
  //        CaloCell_Base_ID::HEC1);
#endif

  id = calo_id.cell_id (CaloCell_Base_ID::LARFCAL,
                         2, 2, 0, 1, 0);
  assert (calo_id.calo_sample (id) == CaloCell_Base_ID::FCAL1);
  assert (calo_id.calo_sample (calo_id.calo_cell_hash(id)) ==
          CaloCell_Base_ID::FCAL1);

  if (supercell) {
    id = calo_id.cell_id (CaloCell_Base_ID::TILE,
                           1, 1, 0, 1, 0);
    assert (calo_id.calo_sample (id) == CaloCell_Base_ID::TileBar0);
    assert (calo_id.calo_sample (calo_id.calo_cell_hash(id)) ==
            CaloCell_Base_ID::TileBar0);

    id = calo_id.cell_id (CaloCell_Base_ID::TILE,
                           2, 1, 0, 12, 2);
    assert (calo_id.calo_sample (id) == CaloCell_Base_ID::TileExt2);
    assert (calo_id.calo_sample (calo_id.calo_cell_hash(id)) ==
            CaloCell_Base_ID::TileExt2);
  }
  else {
    id = calo_id.cell_id (CaloCell_Base_ID::TILE,
                           1, 1, 0, 1, 1);
    assert (calo_id.calo_sample (id) == CaloCell_Base_ID::TileBar1);
    assert (calo_id.calo_sample (calo_id.calo_cell_hash(id)) ==
            CaloCell_Base_ID::TileBar1);

    id = calo_id.cell_id (CaloCell_Base_ID::TILE,
                           2, 1, 0, 12, 1);
    assert (calo_id.calo_sample (id) == CaloCell_Base_ID::TileExt1);
    assert (calo_id.calo_sample (calo_id.calo_cell_hash(id)) ==
            CaloCell_Base_ID::TileExt1);

    id = calo_id.cell_id (CaloCell_Base_ID::TILE,
                           3, 1, 0, 9, 1);
    assert (calo_id.calo_sample (id) == CaloCell_Base_ID::TileGap1);
    assert (calo_id.calo_sample (calo_id.calo_cell_hash(id)) ==
            CaloCell_Base_ID::TileGap1);
  }
}


void test_subcalo (const CaloCell_Base_ID& calo_id)
{
  std::cout << "test_subcalo\n";

  assert (calo_id.GetSubCaloName ("LAREM") == CaloCell_Base_ID::LAREM);
  assert (calo_id.GetSubCaloName ("LARHEC") == CaloCell_Base_ID::LARHEC);
  assert (calo_id.GetSubCaloName ("LARMINIFCAL") == CaloCell_Base_ID::LARMINIFCAL);
  assert (calo_id.GetSubCaloName ("LARFCAL") == CaloCell_Base_ID::LARFCAL);
  assert (calo_id.GetSubCaloName ("TILE") == CaloCell_Base_ID::TILE);
  assert (calo_id.GetSubCaloName ("xxx") == CaloCell_Base_ID::NOT_VALID);

  for (int subCalo = 0; subCalo < CaloCell_Base_ID::NSUBCALO; subCalo++) { 
    std::vector<Identifier>::const_iterator itId = calo_id.cell_begin(subCalo);
    std::vector<Identifier>::const_iterator itIdEnd = calo_id.cell_end(subCalo);
    int nSubCalo = 0;
    int hashsum = 0;
    for(; itId!=itIdEnd;++itId){
      nSubCalo++;
      hashsum += calo_id.calo_cell_hash(*itId);
    }
    IdentifierHash min=0, max=0;
    calo_id.calo_cell_hash_range(subCalo,min,max);
    assert (nSubCalo == (int)(max-min));

    nSubCalo = 0;
#if __cplusplus > 201100
    for (Identifier ch_id : calo_id.cell_range(subCalo)) {
#else
    BOOST_FOREACH (Identifier ch_id, calo_id.cell_range(subCalo)) {
#endif
      hashsum -= calo_id.calo_cell_hash(ch_id);
    }
    assert (hashsum == 0);
  }

  ExpandedIdentifier id1;
  id1 << 5; // TILE
  assert (calo_id.is_tile (id1));
  ExpandedIdentifier id2;
  id2 << 4; // LAR
  assert (!calo_id.is_tile (id2));
}


void test_regions (const CaloCell_Base_ID& calo_id)
{
  std::cout << "test_regions\n";

  std::vector<Identifier>::const_iterator itId = calo_id.reg_begin();
  std::vector<Identifier>::const_iterator itIdEnd = calo_id.reg_end();

  std::vector<Identifier>::const_iterator itReg = calo_id.reg_end();
  std::vector<Identifier>::const_iterator itRegEnd = calo_id.reg_end();
  int last_subcalo = -1;

  assert ((itIdEnd - itId) == (int)calo_id.calo_region_hash_max());

  int hashsum = 0;
  int subhashsum = 0;
  for(; itId!=itIdEnd;++itId) {
    Identifier regId = *itId;
    IdentifierHash hashId = calo_id.calo_region_hash ( regId );
    hashsum += hashId;
#if 1
    printf ("Region %-18s eta: %8.6f %8.6f %1d %3d phi: %3.1f %8.6f %1d %3d\n",
            calo_id.show_to_string(regId).c_str(),
            calo_id.eta0(regId),
            calo_id.etaGranularity(regId),
            calo_id.eta_min(regId),
            calo_id.eta_max(regId),
            calo_id.phi0(regId),
            calo_id.phiGranularity(regId),
            calo_id.phi_min(regId),
            calo_id.phi_max(regId));
#endif
    Identifier regId1 = calo_id.region_id ( hashId );
    assert (regId1 == regId);

    int subcalo = calo_id.sub_calo(regId);

    IdentifierHash min=0, max=0;
    calo_id.calo_reg_hash_range(regId, min, max);
    IdentifierHash min2=0, max2=0;
    calo_id.calo_reg_hash_range(subcalo, min2, max2);
    assert (min == min2);
    assert (max == max2);

    int subcalo2=-1;
    IdentifierHash subHashId = calo_id.subcalo_region_hash(hashId, subcalo2);
    assert (subcalo2 == subcalo);

    IdentifierHash hashId2 =  calo_id.calo_region_hash (subcalo, subHashId) ;
    assert (hashId2 == hashId);

    if(calo_id.is_tile(regId)) {
      int section = calo_id.section(regId);
      int side = calo_id.side(regId);
      Identifier regId2 = calo_id.region_id (subcalo, section, side, 0);
      assert (regId2 == regId);
    }
    else {
      int bec  = calo_id.pos_neg(regId);
      int samp = calo_id.sampling(regId);
      int reg  = calo_id.region(regId);
      Identifier regId2 = calo_id.region_id (subcalo, bec, samp, reg);
      assert (regId2 == regId);
    }

    int subcalo3 = -1;
    IdentifierHash subHashId2 = calo_id.subcalo_region_hash (regId, subcalo3);
    assert (subcalo3 == subcalo);
    Identifier regId3 = calo_id.region_id ( subcalo, subHashId2 );
    assert (regId3 == regId);

    if (subcalo != last_subcalo) {
      if (last_subcalo != -1) {
#if __cplusplus > 201100
        for (Identifier reg_id : calo_id.reg_range (last_subcalo)) {
#else
        BOOST_FOREACH (Identifier reg_id, calo_id.reg_range (last_subcalo)) {
#endif
          subhashsum -= calo_id.calo_region_hash (reg_id);
        }
        assert (subhashsum == 0);
      }
      assert (itReg == itRegEnd);
      last_subcalo = subcalo;
      itReg = calo_id.reg_begin (subcalo);
      itRegEnd = calo_id.reg_end (subcalo);
    }
    subhashsum += hashId;
    assert (itReg != itRegEnd);
    assert (*itReg == regId);
    ++itReg;
  }

#if __cplusplus > 201100
  for (Identifier reg_id : calo_id.reg_range()) {
#else
  BOOST_FOREACH (Identifier reg_id, calo_id.reg_range()) {
#endif
    hashsum -= calo_id.calo_region_hash (reg_id);
  }
  assert (hashsum == 0);
}


void test_exceptions (const CaloCell_Base_ID& calo_id)
{
  std::cout << "test_exceptions\n";

  bool caught = false;
  try {
    /*Identifier wrongChanId =*/ calo_id.cell_id (0, 99, 0, 0, 0, 0, true);
  }
  catch(LArID_Exception & except){
    caught = true;
    std::cout << "Exception 1: " << (std::string)except << "\n";
  }
  assert (caught);
}


