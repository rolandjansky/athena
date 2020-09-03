/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
 * CaloCellFastCopyTool_CppUnit.cxx
 *
 *  Created on: Oct 7, 2013
 *      Author:
 */


#undef NDEBUG


#include "../src/CaloCellFastCopyTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloConstCellContainer.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloHelpersTest.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/setupStoreGate.h"
#include <map>
#include <utility>

#include <vector>
#include <algorithm>
#include <iostream>
#include <cassert>



/** Class provides dummy algorithm
 *  to help construct CaloCellFastCopyTool to be tested.
 */

class DummyAlgorithm: public AthAlgorithm {
  public:
    DummyAlgorithm(const std::string& name, ISvcLocator* svcloc)
        : AthAlgorithm(name, svcloc) {
    }

    virtual StatusCode execute() {
      return StatusCode::SUCCESS;
    }

};

/** Class provides instance of CaloCell_ID
 *  and helps to construct CaloCell using cell hash.
 *  (code taken from: Calorimeter/CaloEvent/test/CaloCellContainer_test.cxx)
 *  The class designed using singleton pattern.
 */

class CaloHelper
{
public:
  CaloHelper() {}

  ~CaloHelper()
  {
    std::map<Identifier, CaloDetDescriptor*>::iterator it = m_ddmap.begin();
    for (; it != m_ddmap.end(); ++it) {
      if (it->second) delete it->second;
    }
  }

  const CaloCell_ID* GetCaloID()
  {
    return &m_helpers.caloID();
  }

  CaloCell* GetCell(IdentifierHash calo_hash)
  {
    Identifier cell_id = m_helpers.caloID().cell_id(calo_hash);
    Identifier reg_id = m_helpers.caloID().region_id (cell_id);
    CaloDetDescriptor* dd = m_ddmap[reg_id];
    if (!dd) {
      dd = new CaloDetDescriptor(reg_id,
                                 &m_helpers.tileID(),
                                 &m_helpers.caloID());
      m_ddmap[reg_id] = dd;
    }
    int sub_calo;
    IdentifierHash subcalo_hash = m_helpers.caloID().subcalo_cell_hash(calo_hash, sub_calo);
    CaloDetDescrElement* dde = new DummyDetDescrElement(subcalo_hash, dd->caloCellMin(), 0, dd);
    return new CaloCell(dde, 1, 1, 1, CaloGain::LARHIGHGAIN);
  }

private:

  CaloHelpersTest m_helpers;
  std::map<Identifier, CaloDetDescriptor*> m_ddmap;
};


/** Class to test the "CaloCellFastCopyTool.h" class
 *   StoreGate service required
 *
 */
class CaloCellFastCopyToolTest
{
public:
  
  CaloCellFastCopyToolTest (CaloHelper& helper)
    : m_evtStore("StoreGateSvc","")
    , m_detStore("DetectorStore","")
    , m_caloHelper(helper)
  {
  }


  ~CaloCellFastCopyToolTest()
  {
    if (m_alg) delete m_alg;
  }

  
  void setUp()
  {
    ISvcLocator* svcloc = Gaudi::svcLocator();
    assert(m_evtStore.retrieve());
    assert(m_detStore.retrieve());
    m_alg = new DummyAlgorithm("DummyAlgorithm", svcloc);
    m_alg->addRef();
    m_caloID = m_caloHelper.GetCaloID();

    if (!m_detStore->contains<CaloCell_ID>("CaloCell_ID"))
      assert(m_detStore->record(m_caloID, "CaloCell_ID"));

    m_tileGap3Hashes.emplace_back(186886); // TileGap3
    m_tileGap3Hashes.emplace_back(186887); // TileGap3
    m_allHashes = m_tileGap3Hashes;
    m_allHashes.emplace_back(186885); // TileGap1

    if (!m_evtStore->contains<CaloCellContainer>("AllCalo")) {
      CaloCellContainer* cellCont;
      cellCont = new CaloCellContainer(SG::OwnershipPolicy::OWN_ELEMENTS);
      assert(m_evtStore->record(cellCont, "AllCalo", false));

      std::vector<IdentifierHash>::const_iterator it = m_allHashes.begin();
      for (; it != m_allHashes.end(); ++it) {
        cellCont->push_back(m_caloHelper.GetCell(*it));
      }
    }

    if (!m_evtStore->contains<CaloCellContainer>("AllCaloFull")) {
      // Initialize full source container
      CaloCellContainer* srcCont;
      srcCont = new CaloCellContainer(SG::OwnershipPolicy::VIEW_ELEMENTS);
      assert(m_evtStore->record(srcCont, "AllCaloFull", false));

      IdentifierHash maxCellHash = m_caloID->calo_cell_hash_max();
      for (IdentifierHash cellHash = 0; cellHash < maxCellHash; cellHash += 1) {
        srcCont->push_back(m_caloHelper.GetCell(cellHash));
        if (m_caloID->calo_sample(cellHash) == CaloCell_ID::TileGap1) ++m_nTileGap1;
        if (m_caloID->calo_sample(cellHash) == CaloCell_ID::TileGap3) ++m_nTileGap3;
      }
    }
  }


  template <class CONT>
  StatusCode testProcess (CaloCellFastCopyTool* tool,
                          CONT* destCont)
  {
    EventContext ctx;
    ctx.setExtension (Atlas::ExtendedEventContext (m_evtStore->hiveProxyDict(), 0));
    return tool->process (destCont, ctx);
  }


  void testViewNotAvoidingDuplicatesFindCellIsNotFast()
  {
    testViewNotAvoidingDuplicatesIsFindCellFast("CopyToolTest[1]", false);
  }

  void testViewNotAvoidingDuplicatesFindCellIsFast()
  {
    testViewNotAvoidingDuplicatesIsFindCellFast("CopyToolTest[1]", true);
  }

  void testViewNotAvoidingDuplicatesIsFindCellFast(const std::string& toolName,
                                                   bool isFindCellFast)
  {
    const CaloCellContainer* srcCont(nullptr);
    assert(m_evtStore->retrieve(srcCont, "AllCalo"));

    // Get initialized tool to be tested.
    std::vector<std::string> includeSamplings(1, "TileGap3");
    bool avoidDuplicates(false);
    std::unique_ptr<CaloCellFastCopyTool> tool 
      (getInitializedTool(toolName,
                          includeSamplings,
                          avoidDuplicates ,
                          isFindCellFast));

    std::unique_ptr<CaloConstCellContainer> destCont =
      std::make_unique<CaloConstCellContainer>(SG::OwnershipPolicy::VIEW_ELEMENTS);

    // Test tool
    assert(testProcess (tool.get(), destCont.get()).isSuccess());

    // Test that all TileGap3 cells from source container has
    // been copied in destination container.
    assert( destCont->size() == m_tileGap3Hashes.size() );

    // Test that the pointers in both container is same.
    // because destination container has VIEW_ELEMENT own policy.
    CaloCellContainer::CellVector srcCv;
    srcCont->findCellVector(m_tileGap3Hashes, srcCv);
    CaloCellContainer::CellVector destCv;
    destCont->findCellVector(m_tileGap3Hashes, destCv);
    assert( destCv.size() == srcCv.size() );
    std::sort(srcCv.begin(), srcCv.end(), CaloCellContainer::orderWithCaloHash());
    std::sort(destCv.begin(), destCv.end(), CaloCellContainer::orderWithCaloHash());
    std::pair<CaloCellContainer::CellVector::iterator, CaloCellContainer::CellVector::iterator> misPair;
    misPair = std::mismatch(srcCv.begin(), srcCv.end(), destCv.begin());
    assert( misPair.first == srcCv.end() );

    // Test that only Tile subcalo is set in destination container;
    for (int i = 0; i < CaloCell_ID::NSUBCALO; ++i) {
      if (i == CaloCell_ID::TILE) assert( destCont->hasCalo((CaloCell_ID::SUBCALO)i) );
      else assert( !(destCont->hasCalo((CaloCell_ID::SUBCALO)i)) );
    }

    // Test next event (imitated);
    assert(testProcess (tool.get(), destCont.get()).isSuccess());
    // Not avoiding duplicates, therefore
    // size of destination container has been doubled
    assert( destCont->size() == (2 * m_tileGap3Hashes.size()) );
  }


  void testViewAvoidingDuplicatesFindCellIsNotFast()
  {
    testViewAvoidingDuplicatesIsFindCellFast("CopyToolTest[3]", false);
  }


  void testViewAvoidingDuplicatesFindCellIsFast()
  {
    testViewAvoidingDuplicatesIsFindCellFast("CopyToolTest[4]", true);
  }


  void testViewAvoidingDuplicatesIsFindCellFast(const std::string& toolName,
                                                bool isFindCellFast)
  {
    const CaloCellContainer* srcCont(nullptr);
    assert( m_evtStore->retrieve(srcCont, "AllCalo") );

    // Get initialized tool to be tested.
    std::vector<std::string> includeSamplings(1, "TileGap3");
    bool avoidDuplicates(true);
    std::unique_ptr<CaloCellFastCopyTool> tool
      (getInitializedTool(toolName,
                          includeSamplings,
                          avoidDuplicates,
                          isFindCellFast));

    // Initialize destination container
    std::unique_ptr<CaloConstCellContainer> destCont =
      std::make_unique<CaloConstCellContainer>(SG::OwnershipPolicy::VIEW_ELEMENTS);
    // Add one TileGap3 cell to destination in order to test
    // if tool is avoiding duplicates in destination container
    std::unique_ptr<CaloCell> cell (m_caloHelper.GetCell(m_tileGap3Hashes[0]));
    destCont->push_back(cell.get());

    // Test tool
    assert(testProcess (tool.get(), destCont.get()).isSuccess());

    // Test that all TileGap3 cells from source container
    // has been copied to destination container
    assert( destCont->size() == m_tileGap3Hashes.size() );

    // Test that all pointers to TileGap3 cells in both containers
    // is the same (destination container has VIWE_ELEMENTS own policy)
    // apart pointer to specially added cell before.
    CaloCellContainer::CellVector srcCv;
    srcCont->findCellVector(m_tileGap3Hashes, srcCv);
    CaloCellContainer::CellVector destCv;
    destCont->findCellVector(m_tileGap3Hashes, destCv);
    assert( destCv.size() == srcCv.size() );
    std::sort(srcCv.begin(), srcCv.end(), CaloCellContainer::orderWithCaloHash());
    std::sort(destCv.begin(), destCv.end(), CaloCellContainer::orderWithCaloHash());
    for (unsigned int i = 0; i < srcCv.size(); ++i) {
      if (destCv[i] != cell.get()) assert( destCv[i] == srcCv[i] );
    }

    // Test that only Tile sub calo is set in destination container
    for (int i = 0; i < CaloCell_ID::NSUBCALO; ++i) {
      if (i == CaloCell_ID::TILE) assert( destCont->hasCalo((CaloCell_ID::SUBCALO)i) );
      else assert( !(destCont->hasCalo((CaloCell_ID::SUBCALO)i)) );
    }

    // Test the next event (imitated)
    assert(testProcess (tool.get(), destCont.get()).isSuccess());
    // Test that size of destination container has not been changed,
    // because avoiding duplicates.
    assert( destCont->size() == m_tileGap3Hashes.size() );
  }


  void testViewNotAvoidingDuplicatesFindCellIsNotFastConst()
  {
    testViewNotAvoidingDuplicatesIsFindCellFastConst("CopyToolTest[13]", false);
  }

  void testViewNotAvoidingDuplicatesFindCellIsFastConst()
  {
    testViewNotAvoidingDuplicatesIsFindCellFastConst("CopyToolTest[14]", true);
  }

  void testViewNotAvoidingDuplicatesIsFindCellFastConst(const std::string& toolName,
                                                        bool isFindCellFast)
  {
    const CaloCellContainer* srcCont(nullptr);
    assert(m_evtStore->retrieve(srcCont, "AllCalo"));

    // Get initialized tool to be tested.
    std::vector<std::string> includeSamplings(1, "TileGap3");
    bool avoidDuplicates(false);
    std::unique_ptr<CaloCellFastCopyTool> tool 
      (getInitializedTool(toolName,
                          includeSamplings,
                          avoidDuplicates ,
                          isFindCellFast));

    std::unique_ptr<CaloCellContainer> destCont =
      std::make_unique<CaloCellContainer>(SG::OwnershipPolicy::VIEW_ELEMENTS);

    // Test tool
    assert(testProcess (tool.get(), destCont.get()).isFailure());
  }


  void testViewAvoidingDuplicatesFindCellIsNotFastConst()
  {
    testViewAvoidingDuplicatesIsFindCellFastConst("CopyToolTest[15]", false);
  }


  void testViewAvoidingDuplicatesFindCellIsFastConst()
  {
    testViewAvoidingDuplicatesIsFindCellFastConst("CopyToolTest[16]", true);
  }


  void testViewAvoidingDuplicatesIsFindCellFastConst(const std::string& toolName,
                                                     bool isFindCellFast)
  {
    const CaloCellContainer* srcCont(nullptr);
    assert(m_evtStore->retrieve(srcCont, "AllCalo"));

    // Get initialized tool to be tested.
    std::vector<std::string> includeSamplings(1, "TileGap3");
    bool avoidDuplicates(true);
    std::unique_ptr<CaloCellFastCopyTool> tool
      (getInitializedTool(toolName,
                          includeSamplings,
                          avoidDuplicates,
                          isFindCellFast));

    // Initialize destination container
    std::unique_ptr<CaloCellContainer> destCont =
      std::make_unique<CaloCellContainer>(SG::OwnershipPolicy::VIEW_ELEMENTS);
    // Test tool
    assert(testProcess (tool.get(), destCont.get()).isFailure());
  }


  void testCloneNotAvoidingDuplicatesFindCellIsNotFast()
  {
    testCloneNotAvoidingDuplicatesIsFindCellFast("CopyToolTest[5]", false);
  }


  void testCloneNotAvoidingDuplicatesFindCellIsFast()
  {
    testCloneNotAvoidingDuplicatesIsFindCellFast("CopyToolTest[6]", true);
  }

  
  void testCloneNotAvoidingDuplicatesIsFindCellFast(const std::string& toolName,
                                                    bool isFindCellFast)
  {
    const CaloCellContainer* srcCont(nullptr);
    assert(m_evtStore->retrieve(srcCont, "AllCalo"));

    // Get initialized tool to be tested.
    std::vector<std::string> includeSamplings(1, "TileGap3");
    bool avoidDuplicates(false);
    std::unique_ptr<CaloCellFastCopyTool> tool 
      (getInitializedTool(toolName,
                          includeSamplings,
                          avoidDuplicates,
                          isFindCellFast));

    // Initialize destination container
    std::unique_ptr<CaloCellContainer> destCont =
      std::make_unique<CaloCellContainer>(SG::OwnershipPolicy::OWN_ELEMENTS);

    // Test tool
    assert(testProcess (tool.get(), destCont.get()).isSuccess());

    // Test that all TileGap3 cells has been copied to destination container.
    assert( destCont->size() == m_tileGap3Hashes.size() );

    // Test that pointers to cells in both containers are different,
    // because the destination container has OWN_ELEMENTS own policy.
    CaloCellContainer::CellVector srcCv;
    srcCont->findCellVector(m_tileGap3Hashes, srcCv);
    CaloCellContainer::CellVector destCv;
    destCont->findCellVector(m_tileGap3Hashes, destCv);
    assert( destCv.size() == srcCv.size() );
    std::sort(srcCv.begin(), srcCv.end(), CaloCellContainer::orderWithCaloHash());
    std::sort(destCv.begin(), destCv.end(), CaloCellContainer::orderWithCaloHash());
    for (unsigned int i = 0; i < srcCv.size(); ++i) {
      assert( destCv[i] != srcCv[i] );
    }

    // Test that the cells have the same hashes in both containers.
    for (unsigned int i = 0; i < srcCv.size(); ++i) {
      assert( (destCv[i]->caloDDE()->calo_hash()) == (srcCv[i]->caloDDE()->calo_hash()) );
    }

    // Test that only Tile sub calo has been set in destination container.
    for (int i = 0; i < CaloCell_ID::NSUBCALO; ++i) {
      if (i == CaloCell_ID::TILE) assert( destCont->hasCalo((CaloCell_ID::SUBCALO)i) );
      else assert( !(destCont->hasCalo((CaloCell_ID::SUBCALO)i)) );
    }

    // Test the next event (imitated)
    assert(testProcess (tool.get(), destCont.get()).isSuccess());

    // Test that size of destination container has been doubled
    // because of not avoiding duplicates
    assert( destCont->size() == (2 * m_tileGap3Hashes.size()) );
  }


  void testCloneAvoidingDuplicatesFindCellIsNotFast()
  {
    testCloneAvoidingDuplicatesIsFindCellFast("CopyToolTest[7]", false);
  }


  void testCloneAvoidingDuplicatesFindCellIsFast()
  {
    testCloneAvoidingDuplicatesIsFindCellFast("CopyToolTest[8]", true);
  }

  
  void testCloneAvoidingDuplicatesIsFindCellFast(const std::string& toolName,
                                                 bool isFindCellFast)
  {
    const CaloCellContainer* srcCont(nullptr);
    assert(m_evtStore->retrieve(srcCont, "AllCalo"));

    // Get initialized tool to be tested.
    std::vector<std::string> includeSamplings(1, "TileGap3");
    bool avoidDuplicates(true);
    std::unique_ptr<CaloCellFastCopyTool> tool 
      (getInitializedTool(toolName,
                          includeSamplings,
                          avoidDuplicates,
                          isFindCellFast));
    
    // Initialize destination container
    std::unique_ptr<CaloCellContainer> destCont =
      std::make_unique<CaloCellContainer>(SG::OwnershipPolicy::OWN_ELEMENTS);
    // Add one TileGap3 cell to destination in order to test
    // if tool is avoiding duplicates in destination container
    CaloCell* cell = m_caloHelper.GetCell(m_tileGap3Hashes[0]);
    destCont->push_back(cell);

    // Test tool
    assert(testProcess (tool.get(), destCont.get()).isSuccess());

    // Test that all TileGap3 cells has been copied to destination container.
    assert( destCont->size() == m_tileGap3Hashes.size() );

    // Test that pointers to cells in both containers are different,
    // because the destination container has OWN_ELEMENTS own policy,
    // apart of pointer to cell specially added before.
    CaloCellContainer::CellVector srcCv;
    srcCont->findCellVector(m_tileGap3Hashes, srcCv);
    CaloCellContainer::CellVector destCv;
    destCont->findCellVector(m_tileGap3Hashes, destCv);
    assert( destCv.size() == srcCv.size() );
    std::sort(srcCv.begin(), srcCv.end(), CaloCellContainer::orderWithCaloHash());
    std::sort(destCv.begin(), destCv.end(), CaloCellContainer::orderWithCaloHash());
    for (unsigned int i = 0; i < srcCv.size(); ++i) {
      if (destCv[i] != cell) assert( destCv[i] != srcCv[i] );
    }

    // Test that the cells have the same hashes in both containers.
    for (unsigned int i = 0; i < srcCv.size(); ++i) {
      assert( (destCv[i]->caloDDE()->calo_hash()) == (srcCv[i]->caloDDE()->calo_hash()) );
    }

    // Test that only Tile sub calo has been set in destination container.
    for (int i = 0; i < CaloCell_ID::NSUBCALO; ++i) {
      if (i == CaloCell_ID::TILE) assert( destCont->hasCalo((CaloCell_ID::SUBCALO)i) );
      else assert( !(destCont->hasCalo((CaloCell_ID::SUBCALO)i)) );
    }

    // Test the next event (imitated)
    assert(testProcess (tool.get(), destCont.get()).isSuccess());

    // Test that size of destination container has not been changed,
    // because avoiding duplicates.
    assert( destCont->size() == m_tileGap3Hashes.size() );
  }


  void testCloneNotAvoidingDuplicatesFindCellIsNotFastConst()
  {
    testCloneNotAvoidingDuplicatesIsFindCellFastConst("CopyToolTest[9]", false);
  }


  void testCloneNotAvoidingDuplicatesFindCellIsFastConst()
  {
    testCloneNotAvoidingDuplicatesIsFindCellFastConst("CopyToolTest[10]", true);
  }

  
  void testCloneNotAvoidingDuplicatesIsFindCellFastConst(const std::string& toolName,
                                                         bool isFindCellFast)
  {
    const CaloCellContainer* srcCont(nullptr);
    assert(m_evtStore->retrieve(srcCont, "AllCalo"));

    // Get initialized tool to be tested.
    std::vector<std::string> includeSamplings(1, "TileGap3");
    bool avoidDuplicates(false);
    std::unique_ptr<CaloCellFastCopyTool> tool 
      (getInitializedTool(toolName,
                          includeSamplings,
                          avoidDuplicates,
                          isFindCellFast));

    // Initialize destination container
    std::unique_ptr<CaloConstCellContainer> destCont =
      std::make_unique<CaloConstCellContainer>(SG::OwnershipPolicy::OWN_ELEMENTS);

    // Test tool
    assert(testProcess (tool.get(), destCont.get()).isSuccess());

    // Test that all TileGap3 cells has been copied to destination container.
    assert( destCont->size() == m_tileGap3Hashes.size() );

    // Test that pointers to cells in both containers are different,
    // because the destination container has OWN_ELEMENTS own policy.
    CaloCellContainer::CellVector srcCv;
    srcCont->findCellVector(m_tileGap3Hashes, srcCv);
    CaloCellContainer::CellVector destCv;
    destCont->findCellVector(m_tileGap3Hashes, destCv);
    assert( destCv.size() == srcCv.size() );
    std::sort(srcCv.begin(), srcCv.end(), CaloCellContainer::orderWithCaloHash());
    std::sort(destCv.begin(), destCv.end(), CaloCellContainer::orderWithCaloHash());
    for (unsigned int i = 0; i < srcCv.size(); ++i) {
      assert( destCv[i] != srcCv[i] );
    }

    // Test that the cells have the same hashes in both containers.
    for (unsigned int i = 0; i < srcCv.size(); ++i) {
      assert( (destCv[i]->caloDDE()->calo_hash()) == (srcCv[i]->caloDDE()->calo_hash()) );
    }

    // Test that only Tile sub calo has been set in destination container.
    for (int i = 0; i < CaloCell_ID::NSUBCALO; ++i) {
      if (i == CaloCell_ID::TILE) assert( destCont->hasCalo((CaloCell_ID::SUBCALO)i) );
      else assert( !(destCont->hasCalo((CaloCell_ID::SUBCALO)i)) );
    }

    // Test the next event (imitated)
    assert(testProcess (tool.get(), destCont.get()).isSuccess());

    // Test that size of destination container has been doubled
    // because of not avoiding duplicates
    assert( destCont->size() == (2 * m_tileGap3Hashes.size()) );
  }


  void testCloneAvoidingDuplicatesFindCellIsNotFastConst()
  {
    testCloneAvoidingDuplicatesIsFindCellFastConst("CopyToolTest[11]", false);
  }


  void testCloneAvoidingDuplicatesFindCellIsFastConst()
  {
    testCloneAvoidingDuplicatesIsFindCellFastConst("CopyToolTest[12]", true);
  }

  
  void testCloneAvoidingDuplicatesIsFindCellFastConst(const std::string& toolName,
                                                      bool isFindCellFast)
  {
    const CaloCellContainer* srcCont(nullptr);
    assert(m_evtStore->retrieve(srcCont, "AllCalo"));

    // Get initialized tool to be tested.
    std::vector<std::string> includeSamplings(1, "TileGap3");
    bool avoidDuplicates(true);
    std::unique_ptr<CaloCellFastCopyTool> tool 
      (getInitializedTool(toolName,
                          includeSamplings,
                          avoidDuplicates,
                          isFindCellFast));
    
    // Initialize destination container
    std::unique_ptr<CaloConstCellContainer> destCont =
      std::make_unique<CaloConstCellContainer>(SG::OwnershipPolicy::OWN_ELEMENTS);
    // Add one TileGap3 cell to destination in order to test
    // if tool is avoiding duplicates in destination container
    CaloCell* cell = m_caloHelper.GetCell(m_tileGap3Hashes[0]);
    destCont->push_back(cell);

    // Test tool
    assert(testProcess (tool.get(), destCont.get()).isSuccess());

    // Test that all TileGap3 cells has been copied to destination container.
    assert( destCont->size() == m_tileGap3Hashes.size() );

    // Test that pointers to cells in both containers are different,
    // because the destination container has OWN_ELEMENTS own policy,
    // apart of pointer to cell specially added before.
    CaloCellContainer::CellVector srcCv;
    srcCont->findCellVector(m_tileGap3Hashes, srcCv);
    CaloCellContainer::CellVector destCv;
    destCont->findCellVector(m_tileGap3Hashes, destCv);
    assert( destCv.size() == srcCv.size() );
    std::sort(srcCv.begin(), srcCv.end(), CaloCellContainer::orderWithCaloHash());
    std::sort(destCv.begin(), destCv.end(), CaloCellContainer::orderWithCaloHash());
    for (unsigned int i = 0; i < srcCv.size(); ++i) {
      if (destCv[i] != cell) assert( destCv[i] != srcCv[i] );
    }

    // Test that the cells have the same hashes in both containers.
    for (unsigned int i = 0; i < srcCv.size(); ++i) {
      assert( (destCv[i]->caloDDE()->calo_hash()) == (srcCv[i]->caloDDE()->calo_hash()) );
    }

    // Test that only Tile sub calo has been set in destination container.
    for (int i = 0; i < CaloCell_ID::NSUBCALO; ++i) {
      if (i == CaloCell_ID::TILE) assert( destCont->hasCalo((CaloCell_ID::SUBCALO)i) );
      else assert( !(destCont->hasCalo((CaloCell_ID::SUBCALO)i)) );
    }

    // Test the next event (imitated)
    assert(testProcess (tool.get(), destCont.get()).isSuccess());

    // Test that size of destination container has not been changed,
    // because avoiding duplicates.
    assert( destCont->size() == m_tileGap3Hashes.size() );
  }


  void testOverFullContainerFindCellIsNotFast()
  {
    testOverFullContainerIsFindCellFast("CopyToolTest[9]", false);
  }


  void testOverFullContainerFindCellIsFast()
  {
    testOverFullContainerIsFindCellFast("CopyToolTest[10]", true);
  }


  void testOverFullContainerIsFindCellFast(const std::string& toolName,
                                           bool isFindCellFast)
  {
    // Initialize destination container
    std::unique_ptr<CaloConstCellContainer> destCont =
      std::make_unique<CaloConstCellContainer>(SG::OwnershipPolicy::VIEW_ELEMENTS);

    // Get initialized tool to be tested.
    std::vector<std::string> includeSamplings(1, "TileGap3");
    bool avoidDuplicates(false);
    std::unique_ptr<CaloCellFastCopyTool> tool 
      (getInitializedTool(toolName,
                          includeSamplings,
                          avoidDuplicates,
                          isFindCellFast,
                          "AllCaloFull"));

    // Test tool
    assert(testProcess (tool.get(), destCont.get()).isSuccess());

    // Test that all TileGap3 cells has been copied to destination container.
    assert( destCont->size() == m_nTileGap3 );

    // Test next event (imitated)
    assert(testProcess (tool.get(), destCont.get()).isSuccess());

    // Test that size of destination container has been doubled
    // because of not avoiding duplicates
    assert( destCont->size() == (2 * m_nTileGap3) );

    // Test with avoiding duplicates
    // -----------------------------

    // Initialize destination container
    destCont =
      std::make_unique<CaloConstCellContainer>(SG::OwnershipPolicy::VIEW_ELEMENTS);

    // Get initialized tool to be tested.
    includeSamplings.emplace_back("TileGap1");
    avoidDuplicates = true;
    tool = std::unique_ptr<CaloCellFastCopyTool>
      (getInitializedTool(toolName,
                          includeSamplings,
                          avoidDuplicates,
                          isFindCellFast,
                          "AllCaloFull"));

    // Test tool
    assert(testProcess (tool.get(), destCont.get()).isSuccess());

    // Test that all TileGap1 + TileGap3 cells has been copied to destination container.
    unsigned int nTileGap1AndTileGap3((m_nTileGap1 + m_nTileGap3));
    assert( destCont->size() == nTileGap1AndTileGap3 );

    // Test next event (imitated)
    assert(testProcess (tool.get(), destCont.get()).isSuccess());

    // Test that size of destination container has not been changed,
    // because avoiding duplicates.
    assert( destCont->size() == nTileGap1AndTileGap3 );
  }


private:

  CaloCellFastCopyTool*
  getInitializedTool(const std::string& name,
                     std::vector<std::string>& includeSamplings,
                     bool avoidDuplicates = false,
                     bool isFindCellFast = false,
                     const std::string& caloCellName = "AllCalo")
  {
    CaloCellFastCopyTool* tool = new CaloCellFastCopyTool("CaloCellFastCopyTool", name, m_alg);
    tool->addRef();
    assert(tool->setProperty("OutputLevel", "1").isSuccess());
    assert(tool->setProperty("AvoidDuplicates",avoidDuplicates).isSuccess());
    assert(tool->setProperty("IncludeSamplings", includeSamplings).isSuccess());
    assert(tool->setProperty("InputName", caloCellName).isSuccess());
    assert(tool->setProperty("IsFindCellFast", isFindCellFast).isSuccess());
    assert(tool->initialize().isSuccess());
    return tool;
  }


  ServiceHandle<StoreGateSvc> m_evtStore;
  ServiceHandle<StoreGateSvc> m_detStore;
  DummyAlgorithm* m_alg{nullptr};
  CaloHelper& m_caloHelper;
  const CaloCell_ID* m_caloID{nullptr};

  std::vector<IdentifierHash> m_tileGap3Hashes;
  std::vector<IdentifierHash> m_tileGap1Hashes;
  std::vector<IdentifierHash> m_allHashes;
  unsigned int m_nTileGap3{0};
  unsigned int m_nTileGap1{0};
};



void test1 (CaloHelper& helper)
{
  std::cout << "test1\n";

  CaloCellFastCopyToolTest tester (helper);
  tester.setUp();

  tester.testViewNotAvoidingDuplicatesFindCellIsNotFast();
  tester.testViewNotAvoidingDuplicatesFindCellIsFast();
  tester.testViewAvoidingDuplicatesFindCellIsNotFast();
  tester.testViewAvoidingDuplicatesFindCellIsFast();

  tester.testViewNotAvoidingDuplicatesFindCellIsNotFastConst();
  tester.testViewNotAvoidingDuplicatesFindCellIsFastConst();
  tester.testViewAvoidingDuplicatesFindCellIsNotFastConst();
  tester.testViewAvoidingDuplicatesFindCellIsFastConst();

  tester.testCloneNotAvoidingDuplicatesFindCellIsNotFast();
  tester.testCloneNotAvoidingDuplicatesFindCellIsFast();
  tester.testCloneAvoidingDuplicatesFindCellIsNotFast();
  tester.testCloneAvoidingDuplicatesFindCellIsFast();

  tester.testCloneNotAvoidingDuplicatesFindCellIsNotFastConst();
  tester.testCloneNotAvoidingDuplicatesFindCellIsFastConst();
  tester.testCloneAvoidingDuplicatesFindCellIsNotFastConst();
  tester.testCloneAvoidingDuplicatesFindCellIsFastConst();

  tester.testOverFullContainerFindCellIsNotFast();
  tester.testOverFullContainerFindCellIsFast();
}


int main (int /*argc*/, char** argv)
{
  Athena_test::setupStoreGate (argv[0]);

  CaloHelper helper;
  test1 (helper);

  return 0;
}
