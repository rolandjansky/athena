/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauPriVtxFillerTool.cxx 577665 2014-01-09 23:18:59Z ssnyder $
/**
 * @file TauD3PDMaker/src/TauPriVtxFillerTool.cxx
 * @author KG <kong.guan.tan@cern.ch>
 * @date October, 2011
 * @brief Block filler tool for Tau Primary Vertex variables.
 */

#include "TauPriVtxFillerTool.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"
#include "AthenaKernel/errorcheck.h"
#include "VxVertex/VxCandidate.h"
#include "Particle/TrackParticle.h"

#include "StoreGate/StoreGateSvc.h"
//#include "TauDiscriminant/TauPVTool.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TauPriVtxFillerTool::TauPriVtxFillerTool (const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent)
  : BlockFillerTool<Analysis::TauJet> (type, name, parent)//,
    //    m_tauPVTool("TauPVTool_ApplyTJVA")
{
  book().ignore(); // Avoid coverity warnings.
}

StatusCode TauPriVtxFillerTool::initialize()
{
  // get StoreGate 
  StatusCode sc = service("StoreGateSvc", m_storeGate);

  if (sc == StatusCode::FAILURE) {
    REPORT_MESSAGE(MSG::ERROR) << "   could not access StoreGate ";
    return StatusCode::FAILURE;
  }
  /*
  // Get the tau PV tool
  if (m_tauPVTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_tauPVTool);
    return StatusCode::FAILURE;;
  } else {
    ATH_MSG_VERBOSE("Retrieved tool " << m_tauPVTool);
  }
  */
  return StatusCode::SUCCESS;
}


/**
 * @brief Book variables for this block.
 */
StatusCode TauPriVtxFillerTool::book()
{
  CHECK( addVariable ("privtx_x",  m_vtx_x )  );
  CHECK( addVariable ("privtx_y",  m_vtx_y )  );
  CHECK( addVariable ("privtx_z",  m_vtx_z )  );
  CHECK( addVariable ("privtx_xx", m_vtx_xx )  );
  CHECK( addVariable ("privtx_yy", m_vtx_yy )  );
  CHECK( addVariable ("privtx_zz", m_vtx_zz )  );
  CHECK( addVariable ("privtx_xy", m_vtx_xy )  );
  CHECK( addVariable ("privtx_yz", m_vtx_yz )  );
  CHECK( addVariable ("privtx_zx", m_vtx_zx )  );
  CHECK( addVariable ("privtx_chiSquared",  m_vtx_chiSquared )  );
  CHECK( addVariable ("privtx_numberDoF",  m_vtx_numberDoF )  );
  CHECK( addVariable ("privtx_jvf",  m_vtx_jvf )  );

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode TauPriVtxFillerTool::fill (const Analysis::TauJet& p)
{
  // get vertex from tau origin, protect against NULL pointers
  const Trk::RecVertex* myVtx = p.origin();
  if (myVtx!=0) {
    const Amg::Vector3D& pos = myVtx->position();
    const AmgSymMatrix(3)& cov = myVtx->covariancePosition();
    *m_vtx_x = pos.x();
    *m_vtx_y = pos.y();
    *m_vtx_z = pos.z();
    *m_vtx_xx = cov(Trk::x, Trk::x);
    *m_vtx_yy = cov(Trk::y, Trk::y);
    *m_vtx_zz = cov(Trk::z, Trk::z);
    *m_vtx_xy = cov(Trk::x, Trk::y);
    *m_vtx_yz = cov(Trk::y, Trk::z);
    *m_vtx_zx = cov(Trk::z, Trk::x);
    const Trk::FitQuality& fq = myVtx->fitQuality();
    *m_vtx_chiSquared = fq.chiSquared();
    *m_vtx_numberDoF = fq.numberDoF();
    //    *m_vtx_jvf = m_tauPVTool->getTJVF(&p);
  }
  
  return StatusCode::SUCCESS;
} // end fill method

} // namespace D3PD
