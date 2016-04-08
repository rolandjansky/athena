/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonErrorScalingTools/MuonRIO_OnTrackErrorScalingTool.h"  
#include "MuonRecToolInterfaces/IMuonErrorScaleDbTool.h"  

#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"

#include <cmath>

//________________________________________________________________________
Muon::MuonRIO_OnTrackErrorScalingTool::MuonRIO_OnTrackErrorScalingTool(const std::string& t, 
								       const std::string& n, const IInterface* p)
  : AthAlgTool(t,n,p)
  , m_errorScaleDbTool("Muon::MuonErrorScaleDbTool/MuonErrorScaleDbTool")  
  , m_log(msgSvc(),n)    
  , m_do_mdt(false)
  , m_do_tgc(false)
  , m_do_rpc(false)
  , m_do_csc(false)
{
  
  declareInterface<Trk::IRIO_OnTrackErrorScalingTool>(this);

  declareProperty("ErrorScaleDbTool", m_errorScaleDbTool);

}

//________________________________________________________________________
Muon::MuonRIO_OnTrackErrorScalingTool::~MuonRIO_OnTrackErrorScalingTool()
{

}

//________________________________________________________________________
StatusCode Muon::MuonRIO_OnTrackErrorScalingTool::initialize()
{
  
  m_log.setLevel(outputLevel());
  
  if (m_errorScaleDbTool.retrieve().isSuccess()) 
    ATH_MSG_INFO("Retrieved "<<m_errorScaleDbTool);
  else {
    ATH_MSG_FATAL("Could not get "<<m_errorScaleDbTool);
    return StatusCode::FAILURE;
  }

  // --- check scaling factors
  m_do_mdt = m_errorScaleDbTool->doMdt();
  m_do_tgc = m_errorScaleDbTool->doTgc();
  m_do_rpc = m_errorScaleDbTool->doRpc();
  m_do_csc = m_errorScaleDbTool->doCsc();
  
  m_log << MSG::INFO << "initialize() successful in " << name() << endreq;
  return StatusCode::SUCCESS;  
}

//________________________________________________________________________
MsgStream& Muon::MuonRIO_OnTrackErrorScalingTool::dump( MsgStream& out ) const
{
  out << m_errorScaleDbTool;

  return out;
}

//________________________________________________________________________
StatusCode Muon::MuonRIO_OnTrackErrorScalingTool::finalize()
{
  msg(MSG::INFO)  << "finalize() successful in " << name() << endreq;
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
void Muon::MuonRIO_OnTrackErrorScalingTool::scale2by2(Amg::MatrixX& cov,
						      const std::vector<double>& phiTrafo,
						      const std::vector<double>& etaTrafo) const 
{
  // careful not to change character of error
  double corr = (fabs(cov(0,1)) > 1.0e-20)             ?
    cov(0,1)/(cov(0,0)*cov(1,1)) :
    0.0;
  cov(0,0) *= phiTrafo[0]*phiTrafo[0];
  cov(0,0) += phiTrafo[1]*phiTrafo[1];
  cov(1,1) *= etaTrafo[0]*etaTrafo[0];
  cov(1,1) += etaTrafo[1]*etaTrafo[1];
  if (corr !=0.0) cov(0,1) = corr*cov(0,0)*cov(1,1);
}

//________________________________________________________________________
Amg::MatrixX*
Muon::MuonRIO_OnTrackErrorScalingTool::createScaledMdtCovariance(const Amg::MatrixX& inputCov, 
								 bool is_endcap) const
{
  Amg::MatrixX* newCov = new Amg::MatrixX(inputCov);
  double a = (is_endcap) ? m_scaling_mdt_endcap[0] : m_scaling_mdt_barrel[0];
  double b = (is_endcap) ? m_scaling_mdt_endcap[1] : m_scaling_mdt_barrel[1];
  (*newCov)(0,0) *= a*a;
  (*newCov)(0,0) += b*b;
  return newCov;
}

//________________________________________________________________________
Amg::MatrixX*
Muon::MuonRIO_OnTrackErrorScalingTool::createScaledTgcCovariance(const Amg::MatrixX& inputCov,
								 const Trk::ParamDefs measuredCoord) const // exists only as end-cap thing
{
  Amg::MatrixX* newCov = new Amg::MatrixX(inputCov);
  double a,b = 0.0;
  if (measuredCoord == Trk::distPhi) {
    a = m_scaling_tgcPhi[0];
    b = m_scaling_tgcPhi[1];
  } else if (measuredCoord == Trk::distEta) {
    a = m_scaling_tgcEta[0];
    b = m_scaling_tgcEta[1];
  } else {
    delete newCov;
    msg(MSG::WARNING) << "Wrong TGC measurement coordinate definition given to RIO_OnTrackErrorScalingTool." << endreq;
    msg(MSG::WARNING) << "Error scaling cancelled, cov=NULL." << endreq;
    msg(MSG::INFO) << "Allowed definitions: Trk::distPhi, Trk::distEta." << endreq;
    return 0;
  }
  (*newCov)(0,0) *= a*a;
  (*newCov)(0,0) += b*b;
  return newCov;
}

//________________________________________________________________________
Amg::MatrixX*
Muon::MuonRIO_OnTrackErrorScalingTool::createScaledRpcCovariance(const Amg::MatrixX& inputCov,
								 const Trk::ParamDefs measuredCoord ) const // exists only as barrel thing
{
  Amg::MatrixX* newCov = new Amg::MatrixX(inputCov);
  double a,b = 0.0;
  if (measuredCoord == Trk::distPhi) {
    a = m_scaling_rpcPhi[0];
    b = m_scaling_rpcPhi[1];
  } else if (measuredCoord == Trk::distEta) {
    a = m_scaling_rpcEta[0];
    b = m_scaling_rpcEta[1];
  } else {
    delete newCov;
    msg(MSG::WARNING) << "Wrong RPC measurement coordinate definition given to RIO_OnTrackErrorScalingTool." << endreq;
    msg(MSG::WARNING) << "Error scaling cancelled, cov=NULL." << endreq;
    msg(MSG::INFO) << "Allowed definitions: Trk::distPhi, Trk::distEta." << endreq;
    return 0;
  }
  (*newCov)(0,0) *= a*a;
  (*newCov)(0,0) += b*b;
  return newCov;
}

//________________________________________________________________________
Amg::MatrixX*
Muon::MuonRIO_OnTrackErrorScalingTool::createScaledCscCovariance(const Amg::MatrixX& inputCov,
								 const Trk::ParamDefs measuredCoord) const // is a far-fwd technology
{
  Amg::MatrixX* newCov = new Amg::MatrixX(inputCov);
  double a,b = 0.0;
  if (measuredCoord == Trk::distPhi) {
    a = m_scaling_cscPhi[0];
    b = m_scaling_cscPhi[1];
  } else if (measuredCoord == Trk::distEta) {
    a = m_scaling_cscEta[0];
    b = m_scaling_cscEta[1];
  } else {
    delete newCov;
    msg(MSG::WARNING) << "Wrong CSC measurement coordinate definition given to RIO_OnTrackErrorScalingTool." << endreq;
    msg(MSG::WARNING) << "Error scaling cancelled, cov=NULL." << endreq;
    msg(MSG::INFO) << "Allowed definitions: Trk::distPhi, Trk::distEta." << endreq;
    return 0;
  }
  (*newCov)(0,0) *= a*a;
  (*newCov)(0,0) += b*b;
  return newCov;
}

//________________________________________________________________________
Amg::MatrixX*
Muon::MuonRIO_OnTrackErrorScalingTool::createScaledMdtCovariance(const Amg::MatrixX& inputCov, 
								 Identifier ) const
{

  Amg::MatrixX* newCov = new Amg::MatrixX(inputCov);
  double a = 1.;
  double b = 0.;
  (*newCov)(0,0) *= a*a;
  (*newCov)(0,0) += b*b;
  return newCov;
}
