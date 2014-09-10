/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CovarianceFillerTool.cxx 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/CovarianceFillerTool.xcx
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Block filler tool for covariance matrix.
 */


#include "CovarianceFillerTool.h"
//#include "TrkEventPrimitives/ErrorMatrix.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "AthenaKernel/errorcheck.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include <sstream>

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
CovarianceFillerTool::CovarianceFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : BlockFillerTool<Types<AmgSymMatrix(5),AmgSymMatrix(3),Amg::MatrixX>> (type, name, parent)
{
  declareProperty ("Error", m_writeError=true);
  declareProperty ("DiagCovariance", m_writeDiagCovariance=false);
  declareProperty ("OffDiagCovariance", m_writeOffDiagCovariance=false);
  declareProperty ("IsPosition", m_isPosition=false);
  declareProperty ("IsTrackPerigee", m_isTrackPerigee=false);
  declareProperty ("NParameters", m_nParameters=0);
}


/**
 * @brief Book variables for this block.
 */
StatusCode CovarianceFillerTool::book()
{

  if( m_isPosition && m_isTrackPerigee ) {
    REPORT_MESSAGE (MSG::WARNING) << "CovarianceFillerTool is configured as a position and a track perigee error matrix filler"
				  << " ===> Filling a generic covariance matrix";
    m_isPosition = false;
    m_isTrackPerigee = false;
  }

  if( m_isPosition ){
    m_nParameters = 3;
  }else if( m_isTrackPerigee ){
    m_nParameters = 5;
  }else{
    //REPORT_MESSAGE (MSG::DEBUG) << "CovarianceFillerTool is filling a generic covariance matrix";
  }

  if(m_nParameters<=0){
    REPORT_MESSAGE (MSG::WARNING) << "CovarianceFillerTool is configured to fill a generic covariance matrix of size <= 0";
    return StatusCode::SUCCESS;
  }

  if( m_writeError )
    m_err.resize (m_nParameters);

  int ncov = 0;
  if( m_writeDiagCovariance)
    ncov = m_nParameters;
  if( m_writeOffDiagCovariance)
    ncov += m_nParameters * (m_nParameters - 1) / 2;
  m_cov.resize (ncov);
  
  if( m_isPosition ){
    if( m_writeError ){
      CHECK( addVariable ("err_x", m_err[0]) );
      CHECK( addVariable ("err_y", m_err[1]) );
      CHECK( addVariable ("err_z", m_err[2]) );
    }
    size_t icov = 0;
    if( m_writeDiagCovariance ){
      CHECK( addVariable ("cov_x",  m_cov[icov++]) );
      CHECK( addVariable ("cov_y",  m_cov[icov++]) );
      CHECK( addVariable ("cov_z",  m_cov[icov++]) );
    }
    if( m_writeOffDiagCovariance ){
      CHECK( addVariable ("cov_xy", m_cov[icov++]) );
      CHECK( addVariable ("cov_xz", m_cov[icov++]) );
      CHECK( addVariable ("cov_yz", m_cov[icov++]) );
    }
  }else if( m_isTrackPerigee ){
    if( m_writeError ) {
      CHECK( addVariable ("err_d0",    m_err[0]) );
      CHECK( addVariable ("err_z0",    m_err[1]) );
      CHECK( addVariable ("err_phi",   m_err[2]) );
      CHECK( addVariable ("err_theta", m_err[3]) );
      CHECK( addVariable ("err_qoverp",m_err[4]) );
    }

    size_t icov = 0;
    if( m_writeDiagCovariance ){
      CHECK( addVariable ("cov_d0",         m_cov[icov++])  );
      CHECK( addVariable ("cov_z0",         m_cov[icov++])  );
      CHECK( addVariable ("cov_phi",        m_cov[icov++])  );
      CHECK( addVariable ("cov_theta",      m_cov[icov++]) );
      CHECK( addVariable ("cov_qoverp",     m_cov[icov++]) );
    }
    if( m_writeOffDiagCovariance ){
      CHECK( addVariable ("cov_d0_z0",      m_cov[icov++])  );
      CHECK( addVariable ("cov_d0_phi",     m_cov[icov++])  );
      CHECK( addVariable ("cov_d0_theta",   m_cov[icov++])  );
      CHECK( addVariable ("cov_d0_qoverp",  m_cov[icov++])  );
      CHECK( addVariable ("cov_z0_phi",     m_cov[icov++])  );
      CHECK( addVariable ("cov_z0_theta",   m_cov[icov++]) );
      CHECK( addVariable ("cov_z0_qoverp",  m_cov[icov++]) );
      CHECK( addVariable ("cov_phi_theta",  m_cov[icov++]) );
      CHECK( addVariable ("cov_phi_qoverp", m_cov[icov++]) );
      CHECK( addVariable ("cov_theta_qoverp",m_cov[icov++]) );
    }
  }else{
    std::ostringstream str;
    if( m_writeError ) {
      for(int i=0; i<m_nParameters; i++){
	str.str("");
	str << "err_par" << i;
	CHECK( addVariable (str.str().c_str(), m_err[i]) );
      }
    }
    size_t icov = 0;
    if( m_writeDiagCovariance) {
      for(int i=0; i<m_nParameters; i++){
        str.str("");
        str << "cov_par" << i << "_par" << i;
        CHECK( addVariable (str.str().c_str(), m_cov[icov++]) );
      }
    }
    if( m_writeOffDiagCovariance) {
      for(int i=0; i<m_nParameters; i++){
	for(int j=i+1; j<m_nParameters; j++){
          str.str("");
          str << "cov_par" << i << "_par" << j;
          CHECK( addVariable (str.str().c_str(), m_cov[icov++]) );
        }
      }
    }
    assert (icov == m_cov.size());
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param m The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
  StatusCode CovarianceFillerTool::fill (const AmgSymMatrix(5) &m)
{

  if(m_nParameters<=0){
    return StatusCode::SUCCESS;
  }

  if( m_isTrackPerigee ) {
    if (m.rows() > 0) {
      if( m_writeError ) {
        *m_err[0] = Amg::error(m,Trk::d0);
        *m_err[1] = Amg::error(m,Trk::z0);
        *m_err[2] = Amg::error(m,Trk::phi0);
        *m_err[3] = Amg::error(m,Trk::theta);
        *m_err[4] = Amg::error(m,Trk::qOverP);	
      }
      size_t icov = 0;
      if( m_writeDiagCovariance ) {
        *m_cov[icov++] = m(Trk::d0,Trk::d0);
        *m_cov[icov++] = m(Trk::z0,Trk::z0);
        *m_cov[icov++] = m(Trk::phi0,Trk::phi0);
        *m_cov[icov++] = m(Trk::theta,Trk::theta);
        *m_cov[icov++] = m(Trk::qOverP,Trk::qOverP);
      }
      if( m_writeOffDiagCovariance ) {
        *m_cov[icov++] = m(Trk::d0,Trk::z0);
        *m_cov[icov++] = m(Trk::d0,Trk::phi0);
        *m_cov[icov++] = m(Trk::d0,Trk::theta);
        *m_cov[icov++] = m(Trk::d0,Trk::qOverP);
        *m_cov[icov++] = m(Trk::z0,Trk::phi0);
        *m_cov[icov++] = m(Trk::z0,Trk::theta);
        *m_cov[icov++] = m(Trk::z0,Trk::qOverP);
        *m_cov[icov++] = m(Trk::phi0,Trk::theta);
        *m_cov[icov++] = m(Trk::phi0,Trk::qOverP);
        *m_cov[icov++] = m(Trk::theta,Trk::qOverP);
      }
      assert (icov == m_cov.size());
    }
  }
  else{
    if( m_writeError ) {
      for(int i=0; i<m_nParameters; i++){
	*m_err[i] = Amg::error(m,(Trk::ParamDefs)i);
      }
    }

    size_t icov = 0;
    if( m_writeDiagCovariance ) {
      for(int i=0; i<m_nParameters; i++)
        *m_cov[icov++] = m((Trk::ParamDefs)i,(Trk::ParamDefs)i);
    }
    if( m_writeOffDiagCovariance ) {
      for(int i=0; i<m_nParameters; i++) {
	for(int j=i+1; j<m_nParameters; j++)
          *m_cov[icov++] = m((Trk::ParamDefs)i,(Trk::ParamDefs)j);
      }
    }
    assert (icov == m_cov.size());
  }

  return StatusCode::SUCCESS;
}

StatusCode CovarianceFillerTool::fill (const AmgSymMatrix(3) &m)
{

  if(m_nParameters<=0){
    return StatusCode::SUCCESS;
  }

  if( m_isPosition ){
    if( m_writeError ) {
      *m_err[0] = sqrt(m(Trk::x,Trk::x));
      *m_err[1] = sqrt(m(Trk::y,Trk::y));
      *m_err[2] = sqrt(m(Trk::z,Trk::z));
    }

    size_t icov = 0;
    if( m_writeDiagCovariance ) {
      *m_cov[icov++] = m(Trk::x,Trk::x);
      *m_cov[icov++] = m(Trk::y,Trk::y);
      *m_cov[icov++] = m(Trk::z,Trk::z);
    }
    if( m_writeOffDiagCovariance ) {
      *m_cov[icov++] = m(Trk::x,Trk::y);
      *m_cov[icov++] = m(Trk::x,Trk::z);
      *m_cov[icov++] = m(Trk::y,Trk::z);
    }
    assert (icov == m_cov.size());
  }else{
    if( m_writeError ) {
      for(int i=0; i<m_nParameters; i++){
	*m_err[i] = sqrt(m((Trk::ParamDefs)i,(Trk::ParamDefs)i));
      }
    }

    size_t icov = 0;
    if( m_writeDiagCovariance ) {
      for(int i=0; i<m_nParameters; i++)
        *m_cov[icov++] = m((Trk::ParamDefs)i,(Trk::ParamDefs)i);
    }
    if( m_writeOffDiagCovariance ) {
      for(int i=0; i<m_nParameters; i++) {
	for(int j=i+1; j<m_nParameters; j++)
          *m_cov[icov++] = m((Trk::ParamDefs)i,(Trk::ParamDefs)j);
      }
    }
    assert (icov == m_cov.size());
  }

  return StatusCode::SUCCESS;
}

  StatusCode CovarianceFillerTool::fill (const Amg::MatrixX &m)
{

  if(m_nParameters<=0){
    return StatusCode::SUCCESS;
  }

  if( m_isTrackPerigee ) {
    if (m.rows() > 0) {
      if( m_writeError ) {
        *m_err[0] = Amg::error(m,Trk::d0);
        *m_err[1] = Amg::error(m,Trk::z0);
        *m_err[2] = Amg::error(m,Trk::phi0);
        *m_err[3] = Amg::error(m,Trk::theta);
        *m_err[4] = Amg::error(m,Trk::qOverP);	
      }
      size_t icov = 0;
      if( m_writeDiagCovariance ) {
        *m_cov[icov++] = m(Trk::d0,Trk::d0);
        *m_cov[icov++] = m(Trk::z0,Trk::z0);
        *m_cov[icov++] = m(Trk::phi0,Trk::phi0);
        *m_cov[icov++] = m(Trk::theta,Trk::theta);
        *m_cov[icov++] = m(Trk::qOverP,Trk::qOverP);
      }
      if( m_writeOffDiagCovariance ) {
        *m_cov[icov++] = m(Trk::d0,Trk::z0);
        *m_cov[icov++] = m(Trk::d0,Trk::phi0);
        *m_cov[icov++] = m(Trk::d0,Trk::theta);
        *m_cov[icov++] = m(Trk::d0,Trk::qOverP);
        *m_cov[icov++] = m(Trk::z0,Trk::phi0);
        *m_cov[icov++] = m(Trk::z0,Trk::theta);
        *m_cov[icov++] = m(Trk::z0,Trk::qOverP);
        *m_cov[icov++] = m(Trk::phi0,Trk::theta);
        *m_cov[icov++] = m(Trk::phi0,Trk::qOverP);
        *m_cov[icov++] = m(Trk::theta,Trk::qOverP);
      }
      assert (icov == m_cov.size());
    }
  } else if( m_isPosition ){
    if( m_writeError ) {
      *m_err[0] = Amg::error(m,Trk::x);
      *m_err[1] = Amg::error(m,Trk::y);
      *m_err[2] = Amg::error(m,Trk::z);
    }

    size_t icov = 0;
    if( m_writeDiagCovariance ) {
      *m_cov[icov++] = m(Trk::x,Trk::x);
      *m_cov[icov++] = m(Trk::y,Trk::y);
      *m_cov[icov++] = m(Trk::z,Trk::z);
    }
    if( m_writeOffDiagCovariance ) {
      *m_cov[icov++] = m(Trk::x,Trk::y);
      *m_cov[icov++] = m(Trk::x,Trk::z);
      *m_cov[icov++] = m(Trk::y,Trk::z);
    }
    assert (icov == m_cov.size());
  }else{
    if( m_writeError ) {
      for(int i=0; i<m_nParameters; i++){
	*m_err[i] = Amg::error(m,(Trk::ParamDefs)i);
      }
    }

    size_t icov = 0;
    if( m_writeDiagCovariance ) {
      for(int i=0; i<m_nParameters; i++)
        *m_cov[icov++] = m((Trk::ParamDefs)i,(Trk::ParamDefs)i);
    }
    if( m_writeOffDiagCovariance ) {
      for(int i=0; i<m_nParameters; i++) {
	for(int j=i+1; j<m_nParameters; j++)
          *m_cov[icov++] = m((Trk::ParamDefs)i,(Trk::ParamDefs)j);
      }
    }
    assert (icov == m_cov.size());
  }

  return StatusCode::SUCCESS;
}

} // namespace D3PD
