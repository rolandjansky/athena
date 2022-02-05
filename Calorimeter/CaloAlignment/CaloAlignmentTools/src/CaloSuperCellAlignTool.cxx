/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloDetDescr/src/CaloSuperCellAlignTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2012
 * @brief Propagate alignent changes to supercell geometry.
 */


#undef NDEBUG


#include "CaloSuperCellAlignTool.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "CaloDetDescrUtils/CaloSuperCellUtils.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaKernel/errorcheck.h"

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
  const CaloSuperCellDetDescrManager* scmgr = nullptr;
  const CaloDetDescrManager* mgr = nullptr;

  CHECK( detStore()->retrieve (scmgr, m_scMgrKey) );
  CHECK( detStore()->retrieve (mgr,   m_mgrKey) );

  // FIXME: This tool changes the content of the (const) CaloSuperCellDetDescrManager
  // recorded in the detector store.  Need to get rid of this for MT.
  // This should go away with the new scheme for dealing with alignments.
  CHECK( doUpdate (const_cast<CaloSuperCellDetDescrManager*>(scmgr), mgr) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Update supercell geometry from the offline geometry.
 * @param mgr The supercell geometry manager.
 * @param cellmgr The offline geometry manager.
 */
StatusCode CaloSuperCellAlignTool::doUpdate(CaloSuperCellDetDescrManager* mgr,
					    const CaloDetDescrManager* cellmgr)
{
  CHECK( updateElements    (mgr, cellmgr, m_scidTool.get()) );
  updateDescriptors(mgr, cellmgr, m_scidTool.get());
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

