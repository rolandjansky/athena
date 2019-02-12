/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file CaloRec/test/CaloCellContainerFromClusterTool_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date June, 2018
 * @brief Unit test for CaloCellContainerFromClusterTool
 */


#undef NDEBUG


#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "CaloInterface/ICaloConstCellMakerTool.h"
#include "CaloIdentifier/CaloHelpersTest.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloUtils/CaloCellList.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloClusterCellLink.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloConstCellContainer.h"
#include "CaloEvent/CaloTester.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloRec/Blob2ToolConstants.h"
#include "StoreGate/setupStoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <set>
#include <iostream>
#include <cassert>


void create_cluster (xAOD::CaloClusterContainer& clusts,
                     const CaloCellContainer& cells,
                     float eta,
                     float phi,
                     std::set<IdentifierHash>& usedCells,
                     std::set<IdentifierHash>& otherCells)
{
  clusts.push_back (std::make_unique<xAOD::CaloCluster>());
  xAOD::CaloCluster& cl = *clusts.back();
  cl.setClusterSize (xAOD::CaloCluster::SW_55ele);
  cl.setEta (eta);
  cl.setPhi (phi);

  auto celllinks = std::make_unique<CaloClusterCellLink> (&cells);

  auto addCells = [cells, eta, phi] (CaloClusterCellLink* celllinks,
                                     std::set<IdentifierHash>& hashes,
                                     CaloSampling::CaloSample sam)
                  {
                    CaloCellList cell_list (&cells);
                    cell_list.select (eta, phi, 5*0.025, 5*0.025, sam);
                    for (const CaloCell* cell : cell_list) {
                      IdentifierHash idhash = cell->caloDDE()->calo_hash();
                      assert (cells[idhash] == cell);
                      if (celllinks) {
                        celllinks->addCell (idhash);
                      }
                      hashes.insert (idhash);
                    }
                  };
  addCells (celllinks.get(), usedCells, CaloSampling::EMB1);
  addCells (celllinks.get(), usedCells, CaloSampling::EMB2);
  addCells (celllinks.get(), usedCells, CaloSampling::EME1);
  addCells (celllinks.get(), usedCells, CaloSampling::EME2);
  addCells (celllinks.get(), usedCells, CaloSampling::EME3);

  addCells (nullptr, otherCells, CaloSampling::HEC0);
  addCells (nullptr, otherCells, CaloSampling::HEC1);
  addCells (nullptr, otherCells, CaloSampling::EMB3);

  std::set<IdentifierHash> ignoredCells;
  addCells (nullptr, ignoredCells, CaloSampling::HEC2);
  addCells (nullptr, ignoredCells, CaloSampling::HEC3);

  cl.addCellLink (std::move (celllinks));

  int counts[6] = {0};
  for (const CaloCell* cell : cl) {
    const CaloDetDescrElement* dde = cell->caloDDE();
    switch (dde->getSampling()) {
    case CaloSampling::EMB1:
      ++counts[0];
      break;
    case CaloSampling::EMB2:
      ++counts[1];
      break;
    case CaloSampling::EMB3:
      ++counts[2];
      break;
    case CaloSampling::EME1:
      ++counts[3];
      break;
    case CaloSampling::EME2:
      ++counts[4];
      break;
    case CaloSampling::EME3:
      ++counts[5];
      break;
    default:
      std::abort();
    }

    float deta = std::abs (dde->eta() - eta);
    float dphi = CaloPhiRange::diff (dde->phi(), phi);
    assert (deta <= 5*0.025);
    assert (dphi <= 5*0.025);
  }

  std::cout << "clust " << eta << " " << phi << " " << cl.size() << " cells";
  for (int i=0; i < 6; i++)
    std::cout << " " << counts[i];
  std::cout << "\n";
}


void make_cluster_lists (StoreGateSvc& sg,
                         const CaloCellContainer& cells,
                         std::set<IdentifierHash>& usedCells1,
                         std::set<IdentifierHash>& usedCells,
                         std::set<IdentifierHash>& otherCells)
{
  auto cl1 = std::make_unique<xAOD::CaloClusterContainer>();
  auto cl1aux = std::make_unique<xAOD::CaloClusterAuxContainer>();
  cl1->setStore (cl1aux.get());
  create_cluster (*cl1, cells,  0.3,  0.1,  usedCells1, otherCells);
  create_cluster (*cl1, cells,  0.32, 0.12, usedCells1, otherCells);
  create_cluster (*cl1, cells, -0.5,  1.4,  usedCells1, otherCells);

  usedCells = usedCells1;

  assert( sg.record (std::move (cl1), "Clusts1") );
  assert( sg.record (std::move (cl1aux), "Clusts1Aux.") );

  auto cl2 = std::make_unique<xAOD::CaloClusterContainer>();
  auto cl2aux = std::make_unique<xAOD::CaloClusterAuxContainer>();
  cl2->setStore (cl2aux.get());
  create_cluster (*cl2, cells,  1.8,  0.3,  usedCells, otherCells);
  create_cluster (*cl2, cells,  1.82, 0.32, usedCells, otherCells);
  create_cluster (*cl2, cells, -2.0,  1.4,  usedCells, otherCells);

  assert( sg.record (std::move (cl2), "Clusts2") );
  assert( sg.record (std::move (cl2aux), "Clusts2Aux.") );
}


void test1 (CaloTester& calotest, StoreGateSvc& sg, const EventContext& ctx)
{
  std::cout << "test1\n";

  std::unique_ptr<CaloCellContainer> cells = calotest.make_ccc();
  std::set<IdentifierHash> usedCells1;
  std::set<IdentifierHash> usedCells;
  std::set<IdentifierHash> otherCells;

  make_cluster_lists (sg, *cells, usedCells1, usedCells, otherCells);

  assert( sg.record (std::move (cells), "AllCalo") );

  // Only Clusts1.
  {
    ToolHandle<ICaloConstCellMakerTool> tool ("CaloCellContainerFromClusterTool/testtool1");
    assert( tool.retrieve().isSuccess() );

    CaloConstCellContainer ccc (SG::VIEW_ELEMENTS);
    assert( tool->process (&ccc, ctx).isSuccess() );
    assert (ccc.size() == usedCells1.size());
    for (const CaloCell* cell : ccc) {
      assert (usedCells1.count (cell->caloDDE()->calo_hash()) == 1);
    }
  }

  // Clusts1 + Clusts2
  {
    ToolHandle<ICaloConstCellMakerTool> tool ("CaloCellContainerFromClusterTool/testtool2");
    assert( tool.retrieve().isSuccess() );

    CaloConstCellContainer ccc (SG::VIEW_ELEMENTS);
    assert( tool->process (&ccc, ctx).isSuccess() );
    assert (ccc.size() == usedCells.size());
    for (const CaloCell* cell : ccc) {
      assert (usedCells.count (cell->caloDDE()->calo_hash()) == 1);
    }
  }

  // Clusts1 + Clusts2 + cells
  {
    ToolHandle<ICaloConstCellMakerTool> tool ("CaloCellContainerFromClusterTool/testtool3");
    assert( tool.retrieve().isSuccess() );

    CaloConstCellContainer ccc (SG::VIEW_ELEMENTS);
    assert( tool->process (&ccc, ctx).isSuccess() );
    assert (ccc.size() == usedCells.size() + otherCells.size());
    for (const CaloCell* cell : ccc) {
      assert (usedCells.count (cell->caloDDE()->calo_hash()) +
              otherCells.count (cell->caloDDE()->calo_hash()) == 1);
    }
  }
}


int main (int /*argc*/, char** argv)
{
  std::cout << "CaloCellContainerFromClusterTool_test\n";
  Athena_test::setupStoreGate (argv[0],
                               "CaloCellContainerFromClusterTool_test.txt");
  CaloTester calotest;
  calotest.record_mgr();
  //calotest.make_cells();

  ServiceHandle<StoreGateSvc> sg ("StoreGateSvc", "test");
  assert( sg.retrieve() );

  EventContext ctx;
  ctx.setExtension( Atlas::ExtendedEventContext (&*sg, 0) );
  Gaudi::Hive::setCurrentContext (ctx);

  test1 (calotest, *sg, ctx);
  return 0;
}



// Force a link dependency on libCaloRecLib; otherwise, the linker
// will remove the dependency.  And if we don't link against libCaloRecLib,
// then we can get ubsan errors related to ConstDataVector<CaloCellCollection>
// because in that case, the typeinfo for it won't get exported from
// the test binary.
void dum()
{
  Blob2ToolConstants ref ("", "", nullptr);
}
