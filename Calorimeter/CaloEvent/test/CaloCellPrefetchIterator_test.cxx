/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  CaloCellPrefetchIterator_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug 2008
 * @brief Component test for CaloCellPrefetchIterator
 */

#undef NDEBUG

#include "CaloEvent/CaloCellPrefetchIterator.h"
#include "CaloEvent/CaloCell.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "CaloIdentifier/TileID.h"
#include "IdDictParser/IdDictParser.h"
#include "Navigation/NavigationToken.h"
#include "GaudiKernel/Bootstrap.h"
#include "TestTools/initGaudi.h"
#include "TestTools/random.h"
#include <map>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <cassert>

CaloCell_ID* make_helper (TileID* tile_id)
{
  LArEM_ID*   em_id   = new LArEM_ID;
  LArHEC_ID*  hec_id  = new LArHEC_ID;
  LArFCAL_ID* fcal_id = new LArFCAL_ID;
  LArMiniFCAL_ID* minifcal_id = new LArMiniFCAL_ID;

  IdDictParser* parser = new IdDictParser;
  parser->register_external_entity ("LArCalorimeter",
                                    "IdDictLArCalorimeter.xml");
  IdDictMgr& idd = parser->parse ("IdDictParser/ATLAS_IDS.xml");
  em_id->set_do_neighbours (false);
  assert (em_id->initialize_from_dictionary (idd) == 0);
  assert (hec_id->initialize_from_dictionary (idd) == 0);
  fcal_id->set_do_neighbours (false);
  assert (fcal_id->initialize_from_dictionary (idd) == 0);
  minifcal_id->set_do_neighbours (false);
  assert (minifcal_id->initialize_from_dictionary (idd) == 0);
  tile_id->set_do_neighbours (false);
  assert (tile_id->initialize_from_dictionary (idd) == 0);

  CaloCell_ID* calo_helper = new CaloCell_ID (em_id,
                                              hec_id,
                                              fcal_id,
                                              minifcal_id,
                                              tile_id);
  assert (calo_helper->initialize_from_dictionary (idd) == 0);
  return calo_helper;
}


using DDMap_t = std::unordered_map<Identifier, std::unique_ptr<CaloDetDescriptor> >;
CaloDetDescriptor* find_dd (int hashid,
                            const CaloCell_ID* helper,
                            const TileID* tile_helper,
                            DDMap_t& ddmap)
{
  Identifier id = helper->cell_id (hashid);
  Identifier reg_id;
  int subcalo = helper->sub_calo (id);
  if (subcalo == CaloCell_ID::TILE) {
    int section = helper->section (id);
    int side = helper->side (id);
    reg_id = helper->region_id (subcalo, section, side, 0);
  }
  else {
    int sampling = helper->sampling (id);
    int posneg = helper->pos_neg (id);
    int region = helper->region (id);
    reg_id = helper->region_id (subcalo, posneg, sampling, region);
  }
  DDMap_t::mapped_type& ptr = ddmap[reg_id];
  if (!ptr) {
    ptr = std::make_unique<CaloDetDescriptor> (reg_id, tile_helper, helper);
  }
  return ptr.get();
}


CaloCell* make_cell (int hashid,
                     const CaloCell_ID* helper,
                     const TileID* tile_helper,
                     DDMap_t& ddmap)
{
  CaloDetDescriptor* descr = find_dd (hashid, helper, tile_helper, ddmap);
  CaloDetDescrElement* dde = new DummyDetDescrElement (hashid -
                                                        descr->caloCellMin(),
                                                       0,
                                                       0,
                                                       descr);
  return new CaloCell (dde, 1, 1, 1, CaloGain::LARHIGHGAIN);
}


std::vector<const CaloCell*>
make_cells (CaloCell_ID* helper, TileID* tile_helper, DDMap_t& ddmap)
{
  size_t hashmax = helper->calo_cell_hash_max();
  std::vector<const CaloCell*> v;
  v.reserve (hashmax);
  for (size_t i = 0; i < hashmax; i++)
    v.push_back (make_cell (i, helper, tile_helper, ddmap));
  return v;
}


//***************************************************************************

int which_type(int) { return 1; }
int which_type(double) { return 2; }
int which_type(NavigationDefaults::DefaultWeight) { return 3; }

// No weights
template <class T>
void check1 (CaloCellPrefetchIterator<T>& it, int n,
             const std::vector<const CaloCell*>& vec)
{
  for (int i=0; i < n; i++) {
    bool res = it.next();
    assert (res);
    assert (*it == vec[i]);
    assert (which_type (it.getParameter()) == 1);
    assert (it.getParameter() == 1);
  }
  assert (!it.next());
}
template <class T>
void check1 (T beg, T end, const std::vector<const CaloCell*>& vec)
{
  CaloCellPrefetchIterator<T> it (beg, end);
  check1 (it, end - beg, vec);
}
template <class T>
void check1 (const T& cont, const std::vector<const CaloCell*>& vec)
{
  CaloCellPrefetchIterator<typename T::const_iterator> it (cont);
  check1 (it, cont.size(), vec);
}


// vector of cells.
void test1 (const std::vector<const CaloCell*>& cells)
{
  std::cout << "test1\n";

  check1 (cells, cells);
  check1 (cells.begin(), cells.begin()+100, cells);
  check1 (cells.begin(), cells.begin()+2, cells);
  check1 (cells.begin(), cells.begin()+1, cells);
  check1 (cells.begin(), cells.begin()+0, cells);
}


// nav
template <class T>
void check2 (CaloCellPrefetchIterator<T>& it,
             int n, const std::vector<const CaloCell*>& vec,
             const std::vector<double>& wt = std::vector<double>())
{
  std::map<const CaloCell*, double> map1;
  for (int i=0; i < n; i++) {
    bool res = it.next();
    assert (res);
    map1[*it] = it.getParameter();
    if (wt.empty())
      assert (which_type (it.getParameter()) == 3);
    else
      assert (which_type (it.getParameter()) == 2);
  }
  assert (!it.next());

  std::map<const CaloCell*, double> map2;
  for (int i=0; i < n; i++) {
    double xwt = 1;
    if (!wt.empty())
      xwt = wt[i];
    map2[vec[i]] = xwt;
  }

  assert (map1.size() == map2.size());
#if 0
  std::map<const CaloCell*, double>::iterator it1 = map1.begin();
  std::map<const CaloCell*, double>::iterator it2 = map2.begin();
  for (; it1 != map1.end(); ++it1, ++it2) {
    std::cout << it1->first << " " << it2->first << " "
              << it1->second << " " << it2->second << "\n";
  }
#endif

  assert (map1 == map2);
}
template <class T>
void check2 (T beg, T end, int n, const std::vector<const CaloCell*>& vec,
             const std::vector<double>& wt = std::vector<double>())
{
  CaloCellPrefetchIterator<T> it (beg, end);
  check2 (it, n, vec, wt);
}
template <class T>
void check2 (const T& cont, int n, const std::vector<const CaloCell*>& vec,
             const std::vector<double>& wt = std::vector<double>())
{
  CaloCellPrefetchIterator<typename T::const_iterator> it (cont);
  check2 (it, n, vec, wt);
}

void check2a (NavigationToken<CaloCell>& cont, int n)
{
  std::vector<const CaloCell*> cells;
  NavigationToken<CaloCell>::const_iterator it = cont.begin();
  for (int i=0; i < n; i++) {
    cells.push_back (*it);
    ++it;
  }

  if (n == static_cast<int> (cont.size()))
    check2 (cont, n, cells);
  else
    check2 (cont.begin(), it, n, cells);
}


// navigation with default weights.
void test2 (const std::vector<const CaloCell*>& cells)
{
  std::cout << "test2\n";
  NavigationToken<CaloCell> token;
  for (size_t i=0; i<cells.size(); i++)
    token.setObject (cells[i]);
  check2 (token.begin(), token.end(), cells.size(), cells);
  NavigationToken<CaloCell>::const_iterator it = token.begin();
  check2 (token.begin(), it, 0, cells);
  check2a (token, 1);
  check2a (token, 2);
  check2a (token, 10);
  check2a (token, 100);
}


void check3a (NavigationToken<CaloCell,double>& cont, int n)
{
  std::vector<const CaloCell*> cells;
  std::vector<double> wt;
  NavigationToken<CaloCell,double>::const_iterator it = cont.begin();
  for (int i=0; i < n; i++) {
    cells.push_back (*it);
    wt.push_back (it.getParameter());
    ++it;
  }

  if (n == static_cast<int> (cont.size()))
    check2 (cont, n, cells, wt);
  else
    check2 (cont.begin(), it, n, cells, wt);
}


// navigation with weights.
void test3 (const std::vector<const CaloCell*>& cells)
{
  std::cout << "test3\n";
  NavigationToken<CaloCell, double> token;
  uint32_t seed = 1;
  for (size_t i=0; i<cells.size(); i++)
    token.setObject (cells[i], Athena_test::randf_seed(seed, 2));
  check3a (token, 0);
  check3a (token, 1);
  check3a (token, 2);
  check3a (token, 10);
  check3a (token, 100);
  check3a (token, token.size());
}


int main()
{
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("CaloEvent/CaloCellContainer_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }  

  TileID* tile_helper = new TileID;
  CaloCell_ID* helper = make_helper (tile_helper);
  DDMap_t ddmap;
  std::vector<const CaloCell*> cells = make_cells (helper, tile_helper, ddmap);

  test1 (cells);
  test2 (cells);
  test3 (cells);
}

