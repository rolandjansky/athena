/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagSVInfoPlusFillerTool.h"
#include "JetEvent/Jet.h"
#include "JetTagInfo/SVInfoPlus.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {
  
  

JetTagSVInfoPlusFillerTool::JetTagSVInfoPlusFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent),
      m_finder (*this, "SVInfoPlus", true)
{
  declareProperty("AddNormDist", m_addNormDist=false);
  book().ignore(); // Avoid coverity warnings.
  m_norm_dist = nullptr;
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetTagSVInfoPlusFillerTool::book()
{

  CHECK(addVariable ("isValid", m_isValid) );
  CHECK(addVariable ("ntrkv",   m_ntrkVx,  "UNDOCUMENTED", -1) );
  CHECK(addVariable ("ntrkj",   m_ntrkJet, "UNDOCUMENTED", -1) );
  CHECK(addVariable ("n2t",     m_n2trkVx, "UNDOCUMENTED", -1) );
  CHECK(addVariable ("mass",    m_mass,    "UNDOCUMENTED", -1) );
  CHECK(addVariable ("efrc",    m_eFrac,   "UNDOCUMENTED", -1) );
  CHECK(addVariable ("x", m_x) );
  CHECK(addVariable ("y", m_y) );
  CHECK(addVariable ("z", m_z) );
  CHECK(addVariable ("err_x", m_err_x) );
  CHECK(addVariable ("err_y", m_err_y) );
  CHECK(addVariable ("err_z", m_err_z) );
  CHECK(addVariable ("cov_xy", m_cov_xy) );
  CHECK(addVariable ("cov_xz", m_cov_xz) );
  CHECK(addVariable ("cov_yz", m_cov_yz) );
  CHECK(addVariable ("chi2", m_chi2, "UNDOCUMENTED", -1) );
  if (m_addNormDist) CHECK(addVariable ("norm_dist", m_norm_dist) ); 
  CHECK(addVariable ("ndof", m_ndof, "UNDOCUMENTED", -1) );        
  CHECK(addVariable ("ntrk", m_ntrk, "UNDOCUMENTED", -1) ); /// duplicated if we associate to tracks but good for check


  return StatusCode::SUCCESS;
}


StatusCode JetTagSVInfoPlusFillerTool::fill(const Jet& p)
{
  if (const Analysis::SVInfoPlus* info =
      m_finder.getTagInfo<Analysis::SVInfoPlus> (p))
  {
    //// *m_isValid=infob->isValid(); /// is valid not used use vertex prop
    const Trk::RecVertex& sv = info->getRecSvx();
    const AmgSymMatrix(3)& err = sv.covariancePosition();
    *m_ntrkVx = info->getNGTrackInSvx();
    *m_ntrkJet = info->getNGTrackInJet();
    *m_n2trkVx = info->getN2T();
    *m_mass = info->getMass();
    *m_eFrac = info->getEnergyFraction();
    if (m_addNormDist) *m_norm_dist = info->getNormDist(); 
    *m_x = sv.position().x();
    *m_y = sv.position().y();
    *m_z = sv.position().z();
    if( (*m_ntrkVx)>0 ){
      *m_err_x = Amg::error(err, Trk::x);
      *m_err_y = Amg::error(err, Trk::y);
      *m_err_z = Amg::error(err, Trk::z);
      *m_cov_xy = err(Trk::x,Trk::y);
      *m_cov_xz = err(Trk::x,Trk::z);
      *m_cov_yz = err(Trk::y,Trk::z);
    }
    *m_chi2 = sv.fitQuality().chiSquared();
    *m_ndof = sv.fitQuality().numberDoF();        
    *m_ntrk = info->numTrackInfo();
  }

  *m_isValid = ( (*m_mass)>0 && (*m_n2trkVx)>0 );
 
  return StatusCode::SUCCESS;
}


}// namespace D3PD
