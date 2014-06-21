/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauCellsDetailsFillerTool.cxx
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @date December, 2009
 * @brief Block filler tool for tau track details.
 */

#include "TauSecVtxFillerTool.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"
#include "AthenaKernel/errorcheck.h"
#include "VxVertex/VxCandidate.h"
#include "Particle/TrackParticle.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TauSecVtxFillerTool::TauSecVtxFillerTool (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent)
  : BlockFillerTool<Analysis::TauJet> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode TauSecVtxFillerTool::book()
{
    CHECK( addVariable ("secvtx_x",  m_vtx_x )  );
    CHECK( addVariable ("secvtx_y",  m_vtx_y )  );
    CHECK( addVariable ("secvtx_z",  m_vtx_z )  );
    CHECK( addVariable ("secvtx_xx", m_vtx_xx )  );
    CHECK( addVariable ("secvtx_yy", m_vtx_yy )  );
    CHECK( addVariable ("secvtx_zz", m_vtx_zz )  );
    CHECK( addVariable ("secvtx_xy", m_vtx_xy )  );
    CHECK( addVariable ("secvtx_yz", m_vtx_yz )  );
    CHECK( addVariable ("secvtx_zx", m_vtx_zx )  );
    CHECK( addVariable ("secvtx_chiSquared",  m_vtx_chiSquared )  );
    CHECK( addVariable ("secvtx_numberDoF",  m_vtx_numberDoF )  );


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
StatusCode TauSecVtxFillerTool::fill (const Analysis::TauJet& p)
{
  // get vertex from tau common details, protect against NULL pointers
  const Analysis::TauCommonDetails*  p_taudetails(0);
  p_taudetails = p.details<const Analysis::TauCommonDetails>();
  if (p_taudetails==0){
    REPORT_MESSAGE (MSG::ERROR) << "TauJet has no associated CommonDetails!!";
    return StatusCode::SUCCESS;
  }
  const Trk::RecVertex* myVtx = p_taudetails->secVtx();
  if (myVtx!=0){
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
  }
  
  return StatusCode::SUCCESS;
} // end fill method

} // namespace D3PD
