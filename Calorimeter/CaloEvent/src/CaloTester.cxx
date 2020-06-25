/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CaloEvent/src/CaloTester.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Helpers for creating calorimeter objects for unit tests.
 */


#include "CaloEvent/CaloTester.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/ServiceHandle.h"


/**
 * @brief Constructor.
 *        The constructor will create the ID helpers.
 *        A CaloDetDescrManager instance will also be created, but it will
 *        not be recorded in the detector store unless @c record_mgr
 *        is called.
 */
CaloTester::CaloTester()
  : m_mgr_up (std::make_unique<CaloDetDescrManager>())
{
  // Make a manager instance.
  m_mgr = m_mgr_up.get();
  m_mgr->set_helper (&m_helpers.caloID());
  m_mgr->initialize();
}


/**
 * @brief Record the CaloDetDescrManager in the detector store.
 *        This may create the detector store as a side effect.
 */
StatusCode CaloTester::record_mgr()
{
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "test");
  CHECK_WITH_CONTEXT( detStore.retrieve(), "CaloTester" );
  CHECK_WITH_CONTEXT( detStore->record (std::move (m_mgr_up), "CaloMgr"), "CaloTester" );
  return StatusCode::SUCCESS;
}


/**
 * @brief Return a region descriptor corresponding to a given cell.
 * @param hashid Cell hash id.
 *
 * The desciptor will be created if needed and registered with the manager.
 */
const CaloDetDescriptor* CaloTester::find_dd (int hashid)
{
  const CaloCell_Base_ID&     helper = m_helpers.caloID();
  
  Identifier id = helper.cell_id (hashid);
  Identifier reg_id;
  int subcalo = helper.sub_calo (id);
  if (subcalo == CaloCell_ID::TILE) {
    int section = helper.section (id);
    int side = helper.side (id);
    reg_id = helper.region_id (subcalo, section, side, 0);
  }
  else {
    int sampling = helper.sampling (id);
    int posneg = helper.pos_neg (id);
    int region = helper.region (id);
    reg_id = helper.region_id (subcalo, posneg, sampling, region);
  }

  const CaloDetDescriptor* dd = m_mgr->get_descriptor (reg_id);
  if (!dd) {
    auto ddp = std::make_unique<CaloDetDescriptor> (reg_id,
                                                    &m_helpers.tileID(),
                                                    &helper);

    if (subcalo != CaloCell_ID::TILE) {
      float eta0 = helper.eta0 (reg_id);
      float eta1 = eta0 + (helper.eta_max (reg_id) - helper.eta_min (reg_id)) * helper.etaGranularity (reg_id);

      if (ddp->calo_sign() >= 0) {
        ddp->setLArRegMin (eta0);
        ddp->setLArRegMax (eta1);
        ddp->setCaloEtaMin (eta0);
        ddp->setCaloEtaMax (eta1);
      }
      else {
        ddp->setLArRegMin (-eta1);
        ddp->setLArRegMax (-eta0);
        ddp->setCaloEtaMin (-eta1);
        ddp->setCaloEtaMax (-eta0);
      }

      ddp->setLArEtaMin (eta0);
      ddp->setLArPhiMin (0);
    }

    dd = ddp.get();
    m_mgr->add (std::move (ddp));
  }
  return dd;
}


/**
 * @brief Return a region descriptor corresponding to a given cell.
 * @param reg_id Cell identifier.
 *
 * The desciptor will be created if needed and registered with the manager.
 */
const CaloDetDescriptor* CaloTester::find_dd (const Identifier& cell_id)
{
  return find_dd (m_helpers.caloID().calo_cell_hash (cell_id));
}


/**
 * @brief Create a calorimeter cell.
 * @param hashid Cell hash id.
 */
std::unique_ptr<CaloCell> CaloTester::make_cell (int hashid)
{
  const CaloDetDescriptor* descr = find_dd (hashid);
  auto ddep = std::make_unique<DummyDetDescrElement> (hashid -
                                                      descr->caloCellMin(),
                                                      0,
                                                      0,
                                                      descr);
  CaloDetDescrElement* dde = ddep.get();

  int ieta_min = m_helpers.caloID().eta_min (descr->identify());
  int iphi_min = m_helpers.caloID().phi_min (descr->identify());
  int ieta = m_helpers.caloID().eta (dde->identify());
  int iphi = m_helpers.caloID().phi (dde->identify());
  float r = 0; // FIXME; not filled in
  float phi = descr->calo_phi_min() + (iphi - iphi_min) * descr->dphi();
  float eta;
  if (descr->calo_sign() > 0) {
    eta = descr->calo_eta_min() + (ieta - ieta_min) * descr->deta();
  }
  else {
    eta = descr->calo_eta_max() - (ieta - ieta_min) * descr->deta();
  }

  ddep->set_cylindric (eta, phi, r);
  ddep->set_cylindric_size (descr->deta(), descr->dphi(), 0);

  // mgr takes ownership of LAr ddes, but not tile...
  m_mgr->add (dde);
  if (m_helpers.caloID().is_tile (hashid)) {
    m_tileddes.push_back (std::move (ddep));
  }
  else {
    ddep.release();
  }

  return std::make_unique<CaloCell> (dde, 1, 1, 1, CaloGain::LARHIGHGAIN);
}


/**
 * @brief Create all calo cells and save in internal list.
 */
void CaloTester::make_cells()
{
  size_t hashmax = m_helpers.caloID().calo_cell_hash_max();
  m_cells.reserve (hashmax);
  for (size_t i = 0; i < hashmax; i++)
    m_cells.push_back (make_cell (i));
}


/**
 * @brief Return a vector of all cells.
 *        The cells remain owned by the Tester object.
 */
std::vector<CaloCell*> CaloTester::get_cells()
{
  if (m_cells.empty()) {
    make_cells();
  }
  std::vector<CaloCell*> cells;
  cells.reserve (m_cells.size());
  for (const std::unique_ptr<CaloCell>& cell : m_cells) {
    cells.push_back (cell.get());
  }
  return cells;
}


/**
 * @brief Return a vector of all cells as const pointers.
 *        The cells remain owned by the Tester object.
 */
std::vector<const CaloCell*> CaloTester::get_const_cells()
{
  if (m_cells.empty()) {
    make_cells();
  }
  std::vector<const CaloCell*> cells;
  cells.reserve (m_cells.size());
  for (const std::unique_ptr<CaloCell>& cell : m_cells) {
    cells.push_back (cell.get());
  }
  return cells;
}


/**
 * @brief Return a CaloCellContainer for all cells.
 *        This will be a view container;
 *        the cells remain owned by the Tester object.
 */
std::unique_ptr<CaloCellContainer> CaloTester::make_ccc()
{
  if (m_cells.empty()) {
    make_cells();
  }
  auto ccc = std::make_unique<CaloCellContainer> (SG::VIEW_ELEMENTS);
  for (std::unique_ptr<CaloCell>& cell : m_cells) {
    ccc->push_back (cell.get());
  }
  ccc->initializeLookUpTable();
  return ccc;
}


