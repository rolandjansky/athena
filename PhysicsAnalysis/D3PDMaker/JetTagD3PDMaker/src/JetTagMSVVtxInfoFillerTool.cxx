/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagMSVVtxInfoFillerTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "JetTagInfo/MSVVtxInfo.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"


namespace D3PD {
  
  

JetTagMSVVtxInfoFillerTool::JetTagMSVVtxInfoFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Analysis::MSVVtxInfo> (type, name, parent)
{
  book().ignore(); // Avoid warnings.
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetTagMSVVtxInfoFillerTool::book()
{

  CHECK(addVariable ("mass", m_mass) );
  CHECK(addVariable ("pt", m_pt) );
  CHECK(addVariable ("eta", m_eta) );
  CHECK(addVariable ("phi", m_phi) );
  CHECK(addVariable ("efrc", m_eFrac) );
  CHECK(addVariable ("x", m_x) );
  CHECK(addVariable ("y", m_y) );
  CHECK(addVariable ("z", m_z) );
  CHECK(addVariable ("err_x", m_err_x) );
  CHECK(addVariable ("err_y", m_err_y) );
  CHECK(addVariable ("err_z", m_err_z) );
  CHECK(addVariable ("cov_xy", m_cov_xy) );
  CHECK(addVariable ("cov_xz", m_cov_xz) );
  CHECK(addVariable ("cov_yz", m_cov_yz) );
  CHECK(addVariable ("chi2", m_chi2) );
  CHECK(addVariable ("ndof", m_ndof) );        
  CHECK(addVariable ("ntrk", m_ntrk) ); 
  CHECK(addVariable ("normDist", m_normDist) ); 
 
  return StatusCode::SUCCESS;
}


StatusCode JetTagMSVVtxInfoFillerTool::fill(const Analysis::MSVVtxInfo& info)
{

  const Trk::RecVertex& sv = info.getRecSvx();
  const AmgSymMatrix(3)& err = sv.covariancePosition();

  *m_mass = info.getMass();
  *m_pt = info.getPt();
  *m_eta = info.getEta();
  *m_phi = info.getPhi();
  *m_eFrac = info.getEnergyFraction();
  *m_x = sv.position().x();
  *m_y = sv.position().y();
  *m_z = sv.position().z();
  *m_err_x = Amg::error(err, Trk::x);
  *m_err_y = Amg::error(err, Trk::y);
  *m_err_z = Amg::error(err, Trk::z);
  *m_cov_xy = err(Trk::x,Trk::y);
  *m_cov_xz = err(Trk::x,Trk::z);
  *m_cov_yz = err(Trk::y,Trk::z);
  *m_chi2 = sv.fitQuality().chiSquared();
  *m_ndof = sv.fitQuality().numberDoF();        
  *m_ntrk = info.numTrackInfo();
  *m_normDist=info.getNormDist();
 
  return StatusCode::SUCCESS;
}


}// namespace D3PD
