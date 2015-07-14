/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PerigeeUnbiasedIPAtPVFillerTool.h"

#include "AthenaKernel/errorcheck.h"
//#include "TrkParameters/Perigee.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/PrimaryVertexSelector.h"

namespace D3PD {


PerigeeUnbiasedIPAtPVFillerTool::PerigeeUnbiasedIPAtPVFillerTool (const std::string& type,
				      const std::string& name,
				      const IInterface* parent)
  : Base (type, name, parent),
    m_trackToVertexIPEstimator("Trk::TrackToVertexIPEstimator")
{
  declareProperty ("VxKey", m_vxCandidate = "VxPrimaryCandidate");
  declareProperty ("TrackToVertexIPEstimator", m_trackToVertexIPEstimator);

  declareProperty ("ErrPrefix", m_errPrefix = "err_");
  declareProperty ("ErrPVPrefix", m_errPVPrefix = "errPV_");
  declareProperty ("BiasedName", m_biasedName = "_biased");
  declareProperty ("UnbiasedName", m_unbiasedName = "_unbiased");
  declareProperty ("DefaultValue", m_defaultValue = 0);
  declareProperty ("DoBiased",     m_doBiased = true);
  declareProperty ("DoUnbiased",   m_doUnbiased = true);

  book().ignore(); // Avoid coverity warnings.
}

StatusCode PerigeeUnbiasedIPAtPVFillerTool::initialize()
{
  CHECK( Base::initialize() );
  CHECK( m_trackToVertexIPEstimator.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode
PerigeeUnbiasedIPAtPVFillerTool::bookVars (Vars& vars,
                                           const std::string nameFrag)
{
  CHECK( addVariable ("d0" + nameFrag,
                      vars.m_d0, "", m_defaultValue) );
  CHECK( addVariable ("z0" + nameFrag,
                      vars.m_z0, "", m_defaultValue) );

  if (!m_errPrefix.empty()) {
    CHECK( addVariable (m_errPrefix + "d0" + nameFrag,
                        vars.m_err_d0, "", m_defaultValue) );
    CHECK( addVariable (m_errPrefix + "z0" + nameFrag,
                        vars.m_err_z0, "", m_defaultValue) );
  }

  if (!m_errPVPrefix.empty()) {
    CHECK( addVariable (m_errPVPrefix + "d0" + nameFrag,
                        vars.m_errPV_d0, "", m_defaultValue) );
    CHECK( addVariable (m_errPVPrefix + "z0" + nameFrag,
                        vars.m_errPV_z0, "", m_defaultValue) );
  }

  return StatusCode::SUCCESS;
}


StatusCode PerigeeUnbiasedIPAtPVFillerTool::book()
{
  if (m_doBiased)
    CHECK( bookVars (m_biased, m_biasedName) );

  if (m_doUnbiased)
    CHECK( bookVars (m_unbiased, m_unbiasedName) );

  return StatusCode::SUCCESS;
}


StatusCode
PerigeeUnbiasedIPAtPVFillerTool::fillVars (Vars& vars,
                                           const D3PD::PerigeePair& ppair,
                                           const Trk::VxCandidate* vx,
                                           bool unbiased)
{
  const Trk::ImpactParametersAndSigma* myIPandSigma =
    m_trackToVertexIPEstimator->estimate(ppair.oldtrack, ppair.newtrack,
                                         vx, unbiased);

  if(0==myIPandSigma) {
    //REPORT_MESSAGE ( MSG::DEBUG) << "trackToVertexIPEstimator at PV failed !";
  }
  else {
    *vars.m_d0 = myIPandSigma->IPd0;
    *vars.m_z0 = myIPandSigma->IPz0;
    if (!m_errPrefix.empty()) {
      *vars.m_err_d0 = myIPandSigma->sigmad0;
      *vars.m_err_z0 = myIPandSigma->sigmaz0;
    }
    if (!m_errPVPrefix.empty()) {
      *vars.m_errPV_d0 = myIPandSigma->PVsigmad0;
      *vars.m_errPV_z0 = myIPandSigma->PVsigmaz0;
    }
    delete myIPandSigma;
  }

  return StatusCode::SUCCESS;
}


StatusCode PerigeeUnbiasedIPAtPVFillerTool::fill (const Trk::Perigee& perigee)
{
  D3PD::PerigeePair ppair (&perigee, &perigee);
  return fill (ppair);
}


StatusCode
PerigeeUnbiasedIPAtPVFillerTool::fill (const D3PD::PerigeePair& ppair)
{

  const VxContainer* vxContainer = 0;
  StatusCode sc = evtStore()->retrieve(vxContainer, m_vxCandidate);
  if (sc.isFailure() || !vxContainer) {
    REPORT_MESSAGE (MSG::WARNING) << "Could not retrieve primary vertex container: " << m_vxCandidate;
    return StatusCode::SUCCESS;
  }

  const Trk::VxCandidate* vxI = PrimaryVertexSelector(*vxContainer);
  if (!vxI){
      REPORT_MESSAGE (MSG::WARNING) << "No primary vertices reconstructed";
      return StatusCode::SUCCESS;
  }

  if (m_doUnbiased)
    CHECK( fillVars (m_unbiased, ppair, vxI, true) );

  if (m_doBiased)
    CHECK( fillVars (m_biased, ppair, vxI, false) );

  return StatusCode::SUCCESS;
}

} // namespace D3PD
