/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/tile_id_test_common.cxx
 * @author scott snyder
 * @date Aug 2012
 * @brief Code common between TileID_test and Tile_SuperCell_ID test.
 *        This file is meant to be included in another file,
 *        not compiled separately.
 */


#include "hash_test.h"
#include "make_idhelper_common.cxx"


// This because test differencing ignored lines containing `0x...'
std::string munghex (const std::string& h)
{
  if (h.size() > 2 && h[0] == '0' && h[1] == 'x')
    return "hex(" + h.substr (2, std::string::npos) + ")";
  return h;
}
void basic_print_id (const Tile_Base_ID& idhelper, const Identifier& id)
{
  std::cout << idhelper.show_to_string (id) << " "
            << munghex(id.getString()) << "\n";
}


class ITile_ID_Test
{
public:
  virtual int get_tile_field_value() const = 0;
};


class CellCounter
{
public:
  CellCounter();
  void count(int sect, int samp);
  void report();


private:
  enum {
    N_REG = 4,
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


void CellCounter::count (int sect, int samp)
{
  sect -= 1;
  if (sect >= N_REG) sect = N_REG-1;
  if (samp < 0 || samp >= N_SAMP) std::abort();
  ++m_counts[sect][samp];
}


void CellCounter::report()
{
  unsigned tot[N_REG] = {0};
  printf ("Sampling  Barrel     Ext Barrel     Gap      Other\n");
  for (unsigned s=0; s < N_SAMP; s++) {
    printf ("%2u          %4d        %4d       %4d       %4d\n",
            s,
            m_counts[0][s], m_counts[1][s], m_counts[2][s],
            m_counts[3][s]);
    for (unsigned b=0; b < N_REG; b++)
      tot[b] += m_counts[b][s];
  }
  printf ("Total       %4u        %4u       %4u       %4u   Grand total: %4u\n",
          tot[0], tot[1], tot[2], tot[3],
          tot[0]+tot[1]+tot[2]+tot[3]);
}


void region_check (const Tile_Base_ID& idhelper, Identifier id)
{
  assert (idhelper.is_tile (id));
  int sect = idhelper.section(id);
  if (sect == Tile_Base_ID::BARREL) {
    assert ( idhelper.is_tile_barrel(id));
    assert (!idhelper.is_tile_extbarrel(id));
    assert (!idhelper.is_tile_gap(id));
    assert (!idhelper.is_tile_aux(id));
  }
  else if (sect == Tile_Base_ID::EXTBAR) {
    assert (!idhelper.is_tile_barrel(id));
    assert ( idhelper.is_tile_extbarrel(id));
    assert (!idhelper.is_tile_gap(id));
    assert (!idhelper.is_tile_aux(id));
  }
  else if (sect == Tile_Base_ID::GAPDET) {
    assert (!idhelper.is_tile_barrel(id));
    assert (!idhelper.is_tile_extbarrel(id));
    assert ( idhelper.is_tile_gap(id));
    assert (!idhelper.is_tile_aux(id));
  }
  else {
    assert (!idhelper.is_tile_barrel(id));
    assert (!idhelper.is_tile_extbarrel(id));
    assert (!idhelper.is_tile_gap(id));
    assert ( idhelper.is_tile_aux(id));
  }

  int side = idhelper.side(id);
  if (side == Tile_Base_ID::NEGATIVE) {
    assert ( idhelper.is_negative(id));
    assert (!idhelper.is_positive(id));
    assert (!idhelper.is_twosides(id));
  }
  else if (side == Tile_Base_ID::POSITIVE) {
    assert (!idhelper.is_negative(id));
    assert ( idhelper.is_positive(id));
    assert (!idhelper.is_twosides(id));
  }
  else if (side == Tile_Base_ID::TWOSIDES) {
    assert (!idhelper.is_negative(id));
    assert (!idhelper.is_positive(id));
    assert ( idhelper.is_twosides(id));
  }

  int adc = idhelper.adc(id);
  if (adc == Tile_Base_ID::LOWGAIN) {
    assert ( idhelper.is_low_gain(id));
    assert (!idhelper.is_high_gain(id));
  }
  else if (adc == Tile_Base_ID::HIGHGAIN) {
    assert (!idhelper.is_low_gain(id));
    assert ( idhelper.is_high_gain(id));
  }

  int samp = idhelper.sample (id);
  if (samp == Tile_Base_ID::SAMP_E)
    assert (idhelper.is_tile_gapscin(id));
  else
    assert (!idhelper.is_tile_gapscin(id));

  assert (idhelper.system(id) == 5); // tile

  if (sect == Tile_Base_ID::AUXDET)
    assert (idhelper.sample(id) == Tile_Base_ID::SAMP_X);
  else
    assert (idhelper.sample(id) == idhelper.sampling(id));
}


void test_connected (const Tile_Base_ID& idhelper, bool supercell)
{
  std::cout << "test_connected\n";

  HASH_TEST(cell);
  HASH_TEST(module);
  HASH_TEST(region);
  HASH_TEST(tower);
  HASH_TEST(pmt);
  HASH_TEST(adc);

  CellCounter counts;
  TEST_LOOP(Identifier id, idhelper.cell_range()) {
    IdContext context = idhelper.cell_context();
    IdentifierHash hashId;
    assert (idhelper.get_hash (id, hashId, &context) == 0);
    assert (id == idhelper.cell_id (hashId));
    assert (hashId == idhelper.cell_hash (id));
    assert (idhelper.is_supercell(id) == supercell);

    int sect = idhelper.section(id);
    int side = idhelper.side(id);
    int mod  = idhelper.module(id);
    int tow  = idhelper.tower(id);
    int samp = idhelper.sample(id);

    Identifier id2 = idhelper.cell_id (sect, side, mod, tow, samp);
    assert (id == id2);

    Identifier reg_id = idhelper.region_id (id);
    Identifier id3;
    assert (idhelper.cell_id (reg_id, mod, tow, samp, id3));
    assert (id == id3);
    assert (!idhelper.cell_id (reg_id, 100, tow, samp, id3));
    assert (!idhelper.cell_id (reg_id, mod, 100, samp, id3));
    assert (!idhelper.cell_id (reg_id, mod, tow,    4, id3));

    counts.count (sect, samp);

    ExpandedIdentifier exp_id;
    const ITile_ID_Test& idhelper_test = dynamic_cast<const ITile_ID_Test&>(idhelper);
    exp_id << idhelper_test.get_tile_field_value()
           << (idhelper.section(id) + (supercell ? Tile_Base_ID::SUPERCELL_MASK : 0))
	   << idhelper.side(id)
	   << idhelper.module(id)
	   << idhelper.tower(id)
           << idhelper.sample(id);
    assert (idhelper.cell_id (exp_id) == id);
  }
  counts.report();


  TEST_LOOP(Identifier id, idhelper.module_range()) {
    int sect = idhelper.section(id);
    int side = idhelper.side(id);
    int mod  = idhelper.module(id);
    assert (idhelper.module_id (sect, side, mod) == id);
    assert (idhelper.is_supercell(id) == supercell);

    Identifier reg_id = idhelper.region_id (id);
    Identifier id2;
    assert (idhelper.module_id (reg_id, mod, id2));
    assert (id == id2);
    assert (!idhelper.module_id (reg_id, 100, id2));

    ExpandedIdentifier exp_id;
    const ITile_ID_Test& idhelper_test = dynamic_cast<const ITile_ID_Test&>(idhelper);
    exp_id << idhelper_test.get_tile_field_value()
           << (idhelper.section(id) + (supercell ? Tile_Base_ID::SUPERCELL_MASK : 0))
	   << idhelper.side(id)
	   << idhelper.module(id);
    assert (idhelper.module_id (exp_id) == id);
  }

  TEST_LOOP(Identifier id, idhelper.tower_range()) {
    int sect = idhelper.section(id);
    int side = idhelper.side(id);
    int mod  = idhelper.module(id);
    int tow  = idhelper.tower(id);
    assert (idhelper.tower_id (sect, side, mod, tow) == id);
    assert (idhelper.is_supercell(id) == supercell);

    Identifier reg_id = idhelper.region_id (id);
    Identifier id2;
    assert (idhelper.tower_id (reg_id, mod, tow, id2));
    assert (id == id2);
    assert (!idhelper.tower_id (reg_id, 100, tow, id2));
    assert (!idhelper.tower_id (reg_id, mod, 100, id2));

    ExpandedIdentifier exp_id;
    const ITile_ID_Test& idhelper_test = dynamic_cast<const ITile_ID_Test&>(idhelper);
    exp_id << idhelper_test.get_tile_field_value()
           << (idhelper.section(id) + (supercell ? Tile_Base_ID::SUPERCELL_MASK : 0))
	   << idhelper.side(id)
	   << idhelper.module(id)
	   << idhelper.tower(id);
    assert (idhelper.tower_id (exp_id) == id);
  }

  TEST_LOOP(Identifier id, idhelper.pmt_range()) {
    Identifier cell_id = idhelper.cell_id (id);
    int pmt = idhelper.pmt (id);

    Identifier id2;
    assert (idhelper.pmt_id (cell_id, pmt, id2));
    assert (id == id2);
    assert (!idhelper.pmt_id (cell_id, 2, id2));

    ExpandedIdentifier exp_id;
    const ITile_ID_Test& idhelper_test = dynamic_cast<const ITile_ID_Test&>(idhelper);
    exp_id << idhelper_test.get_tile_field_value()
           << (idhelper.section(id) + (supercell ? Tile_Base_ID::SUPERCELL_MASK : 0))
	   << idhelper.side(id)
	   << idhelper.module(id)
	   << idhelper.tower(id)
           << idhelper.sample(id)
           << idhelper.pmt(id);
    assert (idhelper.pmt_id (exp_id) == id);
  }

  TEST_LOOP(Identifier id, idhelper.adc_range()) {
    int sect = idhelper.section(id);
    int side = idhelper.side(id);
    int mod  = idhelper.module(id);
    int tow  = idhelper.tower(id);
    int samp = idhelper.sample(id);
    int pmt  = idhelper.pmt(id);
    int adc  = idhelper.adc(id);
    assert (idhelper.is_supercell(id) == supercell);

    Identifier module_id = idhelper.module_id (sect, side, mod);
    assert (module_id == idhelper.module_id (id));
    assert (idhelper.is_supercell(module_id) == supercell);

    Identifier tower_id = idhelper.tower_id (sect, side, mod, tow);
    assert (tower_id == idhelper.tower_id (id));
    assert (idhelper.is_supercell(tower_id) == supercell);

    Identifier cell_id = idhelper.cell_id (sect, side, mod, tow, samp);
    assert (cell_id == idhelper.cell_id (id));
    assert (idhelper.is_supercell(cell_id) == supercell);

    Identifier pmt_id = idhelper.pmt_id (sect, side, mod, tow, samp, pmt);
    assert (pmt_id == idhelper.pmt_id (id));
    assert (pmt_id == idhelper.pmt_id (cell_id, pmt));
    assert (idhelper.is_supercell(pmt_id) == supercell);

    assert (id == idhelper.adc_id (sect, side, mod, tow, samp, pmt, adc));
    assert (id == idhelper.adc_id (pmt_id, adc));
    assert (id == idhelper.adc_id (cell_id, pmt, adc));

    Identifier reg_id = idhelper.region_id (sect, side);
    assert (reg_id == idhelper.region_id (id));
    assert (idhelper.is_supercell(reg_id) == supercell);

    Identifier id2;
    assert (idhelper.adc_id (pmt_id, adc, id2));
    assert (id == id2);
    assert (!idhelper.adc_id (pmt_id, 2, id2));
    id2 = Identifier();
    assert (idhelper.adc_id (cell_id, pmt, adc, id2));
    assert (id == id2);
    assert (!idhelper.adc_id (cell_id, pmt, 2, id2));
    assert (!idhelper.adc_id (cell_id, 2, adc, id2));

    region_check (idhelper, id);

    ExpandedIdentifier exp_id;
    const ITile_ID_Test& idhelper_test = dynamic_cast<const ITile_ID_Test&>(idhelper);
    exp_id << idhelper_test.get_tile_field_value()
           << (idhelper.section(id) + (supercell ? Tile_Base_ID::SUPERCELL_MASK : 0))
	   << idhelper.side(id)
	   << idhelper.module(id)
	   << idhelper.tower(id)
           << idhelper.sample(id)
           << idhelper.pmt(id)
           << idhelper.adc(id);
    assert (idhelper.adc_id (exp_id) == id);
  }

  TEST_LOOP(Identifier id, idhelper.region_range()) {
    IdContext context = idhelper.region_context();
    IdentifierHash hashId;
    assert (idhelper.get_hash (id, hashId, &context) == 0);
    assert (id == idhelper.region_id (hashId));
    assert ((int)hashId == idhelper.region (id));
    assert (idhelper.is_supercell(id) == supercell);

    int sect = idhelper.section(id);
    int side = idhelper.side(id);
    assert (id == idhelper.region_id (sect, side));
    assert ((int)hashId == idhelper.region (sect, side));

    printf ("Region %-18s eta: %2d %3d %2d %3d phi: %3d\n",
            idhelper.show_to_string(id).c_str(),
            idhelper.eta_min(id),
            idhelper.eta_max(id),
            idhelper.region_eta_min(id),
            idhelper.region_eta_max(id),
            idhelper.phi_max(id));

    ExpandedIdentifier exp_id;
    const ITile_ID_Test& idhelper_test = dynamic_cast<const ITile_ID_Test&>(idhelper);
    exp_id << idhelper_test.get_tile_field_value()
           << (idhelper.section(id) + (supercell ? Tile_Base_ID::SUPERCELL_MASK : 0))
	   << idhelper.side(id);
    assert (idhelper.region_id (exp_id) == id);
  }
}


void test_exceptions (const Tile_Base_ID& idhelper)
{
  std::cout << "test_exceptions\n";

  bool caught = false;
  try {
    idhelper.region_id (255, 1); 
  }
  catch(TileID_Exception & except){
    caught = true;
    std::cout << "Exception 1: " << (std::string)except << "\n";
  }
  assert (caught);

  caught = false;
  try {
    idhelper.module_id (1, 1, 99);
  }
  catch(TileID_Exception & except){
    caught = true;
    std::cout << "Exception 2: " << (std::string)except << "\n";
  }
  assert (caught);

  caught = false;
  try {
    idhelper.tower_id (1, 1, 0, 99);
  }
  catch(TileID_Exception & except){
    caught = true;
    std::cout << "Exception 3: " << (std::string)except << "\n";
  }
  assert (caught);

  caught = false;
  try {
    idhelper.cell_id (1, 1, 0, 0, 99);
  }
  catch(TileID_Exception & except){
    caught = true;
    std::cout << "Exception 4: " << (std::string)except << "\n";
  }
  assert (caught);

  Identifier cell_id = idhelper.cell_id (1, 1, 0, 0, 0);
  caught = false;
  try {
    idhelper.pmt_id (cell_id, 99);
  }
  catch(TileID_Exception & except){
    caught = true;
    std::cout << "Exception 5: " << (std::string)except << "\n";
  }
  assert (caught);
  
  caught = false;
  try {
    idhelper.pmt_id (1, 1, 0, 0, 0, 99);
  }
  catch(TileID_Exception & except){
    caught = true;
    std::cout << "Exception 6: " << (std::string)except << "\n";
  }
  assert (caught);
  
  caught = false;
  try {
    idhelper.adc_id (cell_id, 0, 99);
  }
  catch(TileID_Exception & except){
    caught = true;
    std::cout << "Exception 7: " << (std::string)except << "\n";
  }
  assert (caught);
  
  Identifier pmt_id = idhelper.pmt_id (1, 1, 0, 0, 0, 0);
  caught = false;
  try {
    idhelper.adc_id (pmt_id, 99);
  }
  catch(TileID_Exception & except){
    caught = true;
    std::cout << "Exception 8: " << (std::string)except << "\n";
  }
  assert (caught);

  caught = false;
  try {
    idhelper.adc_id (1, 1, 0, 0, 0, 0, 99);
  }
  catch(TileID_Exception & except){
    caught = true;
    std::cout << "Exception 9: " << (std::string)except << "\n";
  }
  assert (caught);

  caught = false;
  try {
    idhelper.cell_id (999999);
  }
  catch(TileID_Exception & except){
    caught = true;
    std::cout << "Exception 10: " << (std::string)except << "\n";
  }
  assert (caught);

  caught = false;
  try {
    idhelper.cell_hash (Identifier());
  }
  catch(TileID_Exception & except){
    caught = true;
    std::cout << "Exception 11: " << (std::string)except << "\n";
  }
  assert (caught);
}


void test_to_string (const Tile_Base_ID& idhelper, bool supercell)
{
  std::cout << "test_to_string\n";
  Identifier id;
  if (supercell)
    id = idhelper.adc_id (1, 1, 0, 4, 2, 0, 0);
  else
    id = idhelper.adc_id (1, 1, 0, 0, 1, 1, 1);

  IdContext contexts[8] = {
    idhelper.adc_context(), // 0
    idhelper.region_context(), // 1
    idhelper.region_context(), // 2
    idhelper.module_context(), // 3
    idhelper.tower_context(),  // 4
    idhelper.cell_context(),   // 5
    idhelper.pmt_context(),    // 6
    idhelper.adc_context(),    // 7
  };
  for (int i=2; i < 8; i++) {
    IdentifierHash hashId;
    assert (idhelper.get_hash (id, hashId, &contexts[i]) == 0);

    std::cout << i << " " << idhelper.to_string (id, i) << "\n";
    std::cout << i << " " << idhelper.to_string (hashId, i) << "\n";
  }
}
