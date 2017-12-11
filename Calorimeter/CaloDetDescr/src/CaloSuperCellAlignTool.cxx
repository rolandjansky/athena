/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloDetDescr/src/CaloSuperCellAlignTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2012
 * @brief Propagate alignent changes to supercell geometry.
 */


#undef NDEBUG


#include "CaloDetDescr/CaloSuperCellAlignTool.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaKernel/errorcheck.h"


namespace {


int descr_index (const CaloDetDescriptor* desc,
                 const CaloDetDescrManager_Base* mgr)
{
  if (desc->is_tile()) {
    int i= 0;
    for (const CaloDetDescriptor* d : mgr->tile_descriptors_range()) {
      if (d == desc) return mgr->calo_descriptors_size() + i;
      ++i;
    }
    return -1;
  }
  else
    return desc->calo_hash();
}


const CaloDetDescriptor* get_descriptor (Identifier reg_id,
                                         const CaloDetDescrManager_Base* mgr)
{
  const CaloCell_Base_ID* calo_helper = mgr->getCaloCell_ID();
  if (!calo_helper->is_tile (reg_id)) {
    assert (reg_id.is_valid());
    return mgr->get_descriptor (reg_id);
  }

  for (const CaloDetDescriptor* d : mgr->tile_descriptors_range()) {
    if (d->identify() == reg_id) return d;
  }
  return 0;
}


} // anonymous namespace


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
CaloSuperCellAlignTool::CaloSuperCellAlignTool (const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent)
  : base_class (type, name, parent),
    m_caloAlignTool ("CaloAlignTool"),
    m_scidTool ("CaloSuperCellIDTool")
{
  declareProperty ("CaloAlignTool", m_caloAlignTool,
                   "Offline geometry alignment tool.");
  declareProperty ("CaloSuperCellIDTool", m_scidTool,
                   "Offline / SuperCell ID mapping tool.");

  declareProperty ("SCMgrKey", m_scMgrKey = "CaloSuperCellMgr",
                   "SG key for the supercell geometry manager.");
  declareProperty ("MgrKey", m_mgrKey = "CaloMgr",
                   "SG key for the offline geometry manager.");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode CaloSuperCellAlignTool::initialize()
{
  CHECK( base_class::initialize() );
  CHECK( m_scidTool.retrieve() );

  if (!m_caloAlignTool.retrieve().isSuccess()) {
    REPORT_MESSAGE (MSG::WARNING)
      << "Cannot find " << m_caloAlignTool.typeAndName()
      << "; no supercell alignments will be propagated.";
    return StatusCode::SUCCESS;
  }

  CHECK( detStore()->regFcn (&IGeoAlignTool::align,
                             &*m_caloAlignTool,
                             &IGeoAlignTool::align,
                             static_cast<IGeoAlignTool*>(this)) );

  return StatusCode::SUCCESS;
}



/**
 * @brief Callback function triggered when alignment constants are updated
 * in StoreGate.  It is called after CaloAlignTool to propagate
 * geometry changes from the offline to supercell versions.
 */
StatusCode CaloSuperCellAlignTool::align(IOVSVC_CALLBACK_ARGS)
{
  // Get the managers.
  const CaloSuperCellDetDescrManager* scmgr = 0;
  const CaloDetDescrManager* mgr = 0;

  CHECK( detStore()->retrieve (scmgr, m_scMgrKey) );
  CHECK( detStore()->retrieve (mgr,   m_mgrKey) );

  CHECK( doUpdate ((CaloSuperCellDetDescrManager*)scmgr, mgr) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Update supercell geometry from the offline geometry.
 * @param mgr The supercell geometry manager.
 * @param cellmgr The offline geometry manager.
 */
StatusCode CaloSuperCellAlignTool::doUpdate (CaloSuperCellDetDescrManager* mgr,
                                             const CaloDetDescrManager* cellmgr)
{
  CHECK( updateElements    (mgr, cellmgr) );
  CHECK( updateDescriptors (mgr, cellmgr) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c queryInterface method.
 *
 * We need do implement this by hand because 
 * @c IGeoAlignTool doesn't use @c DeclareInterfaceID.
 */
StatusCode
CaloSuperCellAlignTool::queryInterface (const InterfaceID& riid, void** ppvIf)
{
  if ( riid == IGeoAlignTool::interfaceID() )  {
    *ppvIf = static_cast<IGeoAlignTool*> (this);
    addRef();
    return StatusCode::SUCCESS;
  }
  if ( riid == ICaloSuperCellAlignTool::interfaceID() )  {
    *ppvIf = static_cast<ICaloSuperCellAlignTool*> (this);
    addRef();
    return StatusCode::SUCCESS;
  }

  return AthAlgTool::queryInterface( riid, ppvIf );
}


/**
 * @brief Update the supercell element geometry.
 * @param mgr The supercell geometry manager.
 * @param cellmgr The offline geometry manager.
 *
 * The geometry information of the supercell elements is updated
 * based on the provided offline geometry.
 */
StatusCode
CaloSuperCellAlignTool::updateElements (CaloSuperCellDetDescrManager* mgr,
                                        const CaloDetDescrManager* cellmgr)
{
  // For each supercell, we make a list of the corresponding cells.
  // Then we pass that list to the supercell's @c update method.

  for (CaloDetDescrElement* elt : mgr->element_range()) {
    if (!elt) continue;
    CaloSuperCellDetectorElement* selt =
      dynamic_cast<CaloSuperCellDetectorElement*> (elt);
    if (!selt) continue;

    std::vector<Identifier> ids =
      m_scidTool->superCellToOfflineID (elt->identify());
    assert (!ids.empty());

    const CaloCell_Base_ID* idhelper      = mgr->getCaloCell_ID();
    const CaloCell_Base_ID* cell_idhelper = cellmgr->getCaloCell_ID();

    std::vector<const CaloDetDescrElement*> fromelts;
    fromelts.reserve (ids.size());
    for (Identifier id : ids) {
      // For tile tower sums, exclude D-layer cells
      // (they have a different size).
      if (cell_idhelper->sub_calo(id) == CaloCell_Base_ID::TILE &&
          cell_idhelper->sample(id) == 2 &&
          idhelper->sample(elt->identify()) != 2)
        continue;
      const CaloDetDescrElement* fromelt = cellmgr->get_element (id);
      assert (fromelt != 0);
      fromelts.push_back (fromelt);
    }
    CHECK( selt->update (fromelts) );
  }

  return StatusCode::SUCCESS;
}


namespace {

/**
 * @brief Helper: Convert to descriptor phi convention.
 *
 * Descriptors use a 0..2pi phi convention; this helper converts a standard
 * ATLAS phi (-pi..pi) to the descriptor convention.
 */
inline
double phi_for_descr (double phi)
{
  if (phi < 0)
    phi += 2*M_PI;
  return phi;
}
} // anonymous namespace


/**
 * @brief Update the supercell descriptor geometry.
 * @param mgr The supercell geometry manager.
 * @param cellmgr The offline geometry manager.
 *
 * The geometry information of the supercell descriptors is updated
 * based on the provided offline geometry.
 */
StatusCode
CaloSuperCellAlignTool::updateDescriptors (CaloSuperCellDetDescrManager* mgr,
                                           const CaloDetDescrManager* cellmgr)
{
  size_t maxdesc = mgr->calo_descriptors_size() + mgr->tile_descriptors_size();
  std::vector<DescrMinMax> descr_minmax (maxdesc);

  // Loop over cells and record range limits for each descriptor.
  for (CaloDetDescrElement* elt : mgr->element_range()) {
    if (!elt) continue;
    CaloDetDescriptor* desc = const_cast<CaloDetDescriptor*>(elt->descriptor());
    int ndx = descr_index (desc, mgr);
    assert (ndx >= 0 && ndx < (int)descr_minmax.size());
    DescrMinMax& mm = descr_minmax[ndx];

    int dz_den = desc->is_lar_hec() ? 1 : 2;

    mm.eta.add (std::abs(elt->eta_raw()), elt->deta()/2);
    mm.phi.add (phi_for_descr (elt->phi_raw()), elt->dphi()/2);
    mm.r.add (elt->r(), elt->dr()/2);
    mm.z.add (std::abs(elt->z_raw()), elt->dz()/dz_den);
  }

  // Loop over each descriptor and update.
  size_t i = 0;
  for (CaloDetDescriptor* desc : mgr->calo_descriptors_range()) {
    updateDescriptor (desc, descr_minmax[i], cellmgr);
    ++i;
  }
  for (CaloDetDescriptor* desc : mgr->tile_descriptors_range()) {
    updateDescriptor (desc, descr_minmax[i], cellmgr);
    ++i;
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Update a single descriptor.
 */
void
CaloSuperCellAlignTool::updateDescriptor
  (CaloDetDescriptor* desc,
   const DescrMinMax& mm,
   const CaloDetDescrManager* cellmgr)
{
  if (!desc) return;

  if (desc->calo_eta_max() == 0) {
    // Only do this the first time.
    desc->setCaloEtaMin (mm.eta.min);
    desc->setCaloEtaMax (mm.eta.max);
    desc->setCaloPhiMin (mm.phi.min);
    desc->setCaloPhiMax (mm.phi.max);
    desc->setCaloRMin (mm.r.min);
    desc->setCaloRMax (mm.r.max);
    desc->setCaloZMin (mm.z.min);
    desc->setCaloZMax (mm.z.max);

    std::vector<Identifier> cell_regions =
      m_scidTool->superCellToOfflineRegion (desc->identify());
    assert (!cell_regions.empty());

    const CaloDetDescriptor* celldesc =
      get_descriptor (cell_regions[0], cellmgr);

    if (celldesc) {
      if (desc->is_tile()) {
        desc->set_eta_phi_granularity (celldesc->n_eta(),
                                       celldesc->deta(),
                                       celldesc->n_phi(),
                                       celldesc->dphi());
      }

      std::vector<double> depths;
      desc->set_n_calo_depth (celldesc->n_calo_depth());
      celldesc->get_depth_in (depths);
      desc->set_depth_in (depths);
      celldesc->get_depth_out (depths);
      desc->set_depth_out (depths);

      desc->set_transform (celldesc->transform());
    }
  }

  // Do this each realignment.
  desc->setLArEtaMin (mm.eta.min);
  desc->setLArPhiMin (mm.phi.min);
  if (desc->calo_sign() > 0) {
    desc->setLArRegMin (mm.eta.min);
    desc->setLArRegMax (mm.eta.max);
  }
  else {
    desc->setLArRegMin (-mm.eta.max);
    desc->setLArRegMax (-mm.eta.min);
  }
}
