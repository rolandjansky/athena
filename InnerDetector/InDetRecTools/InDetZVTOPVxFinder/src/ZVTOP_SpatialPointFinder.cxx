/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ZVTOP_SpatialPointFinder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetZVTOPVxFinder/ZVTOP_SpatialPointFinder.h"
#include "TrkParameters/TrackParameters.h"
#include "VxVertex/Vertex.h"
#include "Particle/TrackParticle.h"
#include "EventPrimitives/EventPrimitives.h"
#include "TrkParticleBase/TrackParticleBase.h"
//================ Constructor =================================================

InDet::ZVTOP_SpatialPointFinder::ZVTOP_SpatialPointFinder(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p)
{
  declareInterface<IZVTOP_SpatialPointFinder>(this);

  //  template for property declaration
  declareProperty("Chi2_cut_value", m_chi2);
}

//================ Destructor =================================================

InDet::ZVTOP_SpatialPointFinder::~ZVTOP_SpatialPointFinder()
{}


//================ Initialisation =================================================

StatusCode InDet::ZVTOP_SpatialPointFinder::initialize()
{
  
  StatusCode sc = AlgTool::initialize();

  if (sc.isFailure()) return sc;

  msg (MSG::INFO) << "initialize() successful in " << name() << endreq;
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::ZVTOP_SpatialPointFinder::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}
//============================================================================================
Trk::Vertex* InDet::ZVTOP_SpatialPointFinder::findSpatialPoint(const Trk::Track* trk_1, const Trk::Track* trk_2)
{
  const Trk::TrackParameters* perigee_1(dynamic_cast<const Trk::TrackParameters*>(trk_1->perigeeParameters()));
  const Trk::TrackParameters* perigee_2(dynamic_cast<const Trk::TrackParameters*>(trk_2->perigeeParameters()));
  if (!perigee_1 | !perigee_2) {
    if (msgLvl(MSG::DEBUG)) msg() << "Dynamic cast to MeasuredPerigee failed. Skipping this pair" << endreq;
    return 0;
  } else {
    Trk::Vertex * vertex = findSpatialPoint(perigee_1,perigee_2);
    return vertex;
  }//if measured perigee
}

//===============================================================================================
Trk::Vertex* InDet::ZVTOP_SpatialPointFinder::findSpatialPoint(const Trk::RecVertex vtx, const Trk::Track* trk_1)
{
  const Trk::TrackParameters *perigee_1(dynamic_cast<const Trk::TrackParameters*>(trk_1->perigeeParameters()));
  if (!perigee_1) {
    if (msgLvl(MSG::DEBUG)) msg() << "Dynamic cast to MeasuredPerigee failed. Skipping this pair" << endreq;
    return 0;
  } else {
    Trk::Vertex * vertex = findSpatialPoint(vtx,perigee_1);
    return vertex;
  }
}

//============================================================================================
Trk::Vertex* InDet::ZVTOP_SpatialPointFinder::findSpatialPoint(const Rec::TrackParticle* trk_1, const Rec::TrackParticle* trk_2)
{
  const Trk::TrackParameters* perigee_1(trk_1->measuredPerigee());
  const Trk::TrackParameters* perigee_2(trk_2->measuredPerigee());
  if (!perigee_1 | !perigee_2) {
    if (msgLvl(MSG::DEBUG)) msg() << "Dynamic cast to MeasuredPerigee failed. Skipping this pair" << endreq;
    return 0;
  } else {
		Trk::Vertex * vertex = findSpatialPoint(perigee_1,perigee_2);
		return vertex;
  }//if measured perigee
}

//============================================================================================
Trk::Vertex* InDet::ZVTOP_SpatialPointFinder::findSpatialPoint(const Trk::RecVertex vtx, const Rec::TrackParticle* trk_1)
{
  const Trk::TrackParameters* perigee_1(trk_1->measuredPerigee());
  if (!perigee_1) {
    if (msgLvl(MSG::DEBUG)) msg() << "Dynamic cast to MeasuredPerigee failed. Skipping this pair" << endreq;
    return 0;
  } else {
    Trk::Vertex * vertex = findSpatialPoint(vtx,perigee_1);
    return vertex;
  }
}
//============================================================================================
Trk::Vertex* InDet::ZVTOP_SpatialPointFinder::findSpatialPoint(const Trk::TrackParticleBase* trk_1, const Trk::TrackParticleBase* trk_2)
{
  const Trk::TrackParameters* perigee_1 = dynamic_cast<const Trk::TrackParameters*>(&trk_1->definingParameters());
  const Trk::TrackParameters* perigee_2 = dynamic_cast<const Trk::TrackParameters*>(&trk_2->definingParameters());
  if (!perigee_1 | !perigee_2) {
    if (msgLvl(MSG::DEBUG)) msg() << "Dynamic cast to MeasuredPerigee failed. Skipping this pair" << endreq;
    return 0;
  } else {
    Trk::Vertex * vertex = findSpatialPoint(perigee_1,perigee_2);
    return vertex;
  }//if measured perigee
}

//============================================================================================
Trk::Vertex* InDet::ZVTOP_SpatialPointFinder::findSpatialPoint(const Trk::RecVertex vtx, const Trk::TrackParticleBase* trk_1)
{
  const Trk::TrackParameters* perigee_1 = dynamic_cast<const Trk::TrackParameters*>(&trk_1->definingParameters());
  if (!perigee_1) {
    if (msgLvl(MSG::DEBUG)) msg() << "Dynamic cast to MeasuredPerigee failed. Skipping this pair" << endreq;
    return 0;
  } else {
    Trk::Vertex * vertex = findSpatialPoint(vtx,perigee_1);
    return vertex;
  }
}

//===============================================================================================


Trk::Vertex* InDet::ZVTOP_SpatialPointFinder::findSpatialPoint(const Trk::TrackParameters* param_1, const Trk::TrackParameters* param_2)
{
  Amg::Vector3D spatialPoint;
  const Trk::Perigee* perigee_1 =dynamic_cast<const Trk::Perigee*>(param_1);
  const Trk::Perigee* perigee_2 =dynamic_cast<const Trk::Perigee*>(param_2);

  double cot_theta_1 = 1./tan(perigee_1->parameters()[Trk::theta]);
  double sphi_1 = sin(perigee_1->parameters()[Trk::phi]);
  double cphi_1 = cos(perigee_1->parameters()[Trk::phi]);
  Amg::Vector3D locXpVec_1;
  locXpVec_1[0]= -perigee_1->parameters()[Trk::d0]*sphi_1;
  locXpVec_1[1]= perigee_1->parameters()[Trk::d0]*cphi_1;
  locXpVec_1[2]= perigee_1->parameters()[Trk::z0];
  
  double cot_theta_2 = 1./tan(perigee_2->parameters()[Trk::theta]);
  double sphi_2 = sin(perigee_2->parameters()[Trk::phi]);
  double cphi_2 = cos(perigee_2->parameters()[Trk::phi]);
  Amg::Vector3D locXpVec_2;
  locXpVec_2[0]= -perigee_2->parameters()[Trk::d0]*sphi_2;
  locXpVec_2[1]= perigee_2->parameters()[Trk::d0]*cphi_2;
  locXpVec_2[2]= perigee_2->parameters()[Trk::z0];
  
  AmgMatrix(3,3) DtWD_sum;
  Amg::Vector3D DtWDx_sum;
  double chi2(0.);
  AmgMatrix(3,3) DtWD_1;
  AmgMatrix(3,3) DtWD_2;
  AmgMatrix(2,2) cov_mat_1;
  AmgMatrix(2,2) cov_mat_2;
  cov_mat_1 =(* perigee_1->covariance()).block<2,2>(0,0);
  cov_mat_2 =(* perigee_2->covariance()).block<2,2>(0,0);

  // and the weightmatrix
  AmgMatrix(2,2) weight_mat_1 = cov_mat_1.inverse().eval();
  AmgMatrix(2,2) weight_mat_2 = cov_mat_2.inverse().eval();

  // calculate the D matrix
  AmgMatrix(2,3) D_1;
  AmgMatrix(2,3) D_2;
  D_1(0,0) = -sphi_1              ; D_1(0,1) = cphi_1              ; D_1(0,2) = 0.;
  D_1(1,0) = -cphi_1*cot_theta_1  ; D_1(1,1) = -sphi_1*cot_theta_1 ; D_1(1,2) = 1.;
  D_2(0,0) = -sphi_2              ; D_2(0,1) = cphi_2              ; D_2(0,2) = 0.;
  D_2(1,0) = -cphi_2*cot_theta_2  ; D_2(1,1) = -sphi_2*cot_theta_2 ; D_2(1,2) = 1.;
  // Calculate DtWD and DtWD*x and sum them
  DtWD_1 = D_1.transpose()*weight_mat_1*D_1;
  DtWD_2 = D_2.transpose()*weight_mat_2*D_2;
  DtWD_sum = DtWD_1 + DtWD_2;
  DtWDx_sum = DtWD_1*locXpVec_1 + DtWD_2*locXpVec_2;
  AmgMatrix(3,3) cov_spatialPoint = DtWD_sum.inverse();
  spatialPoint = cov_spatialPoint * DtWDx_sum;
  Amg::Vector3D XpVec_sp_1 = locXpVec_1 - spatialPoint;
  Amg::Vector3D XpVec_sp_2 = locXpVec_2 - spatialPoint;
  chi2 =  XpVec_sp_1.dot(DtWD_1*XpVec_sp_1) + XpVec_sp_2.dot(DtWD_2*XpVec_sp_2);
  if (chi2 <= m_chi2) 
    { 
      if (msgLvl(MSG::DEBUG)) msg() <<"found spatial point = ("<<spatialPoint[0]<<", "<<spatialPoint[1]<<", "<<spatialPoint[2]<<")"<< endreq;
      return new Trk::Vertex(spatialPoint);
    } else 
    {
      if (msgLvl(MSG::DEBUG)) msg() <<"found spatial point candidate doesn't pass chi2_cut" << endreq;
      return 0;
    }
     	
}


//============================================================================================

Trk::Vertex* InDet::ZVTOP_SpatialPointFinder::findSpatialPoint(const Trk::RecVertex vtx, const Trk::TrackParameters* param_1)
{
  Amg::Vector3D spatialPoint;
  const Trk::Perigee* perigee_1 =dynamic_cast<const Trk::Perigee*>(param_1);
  double cot_theta_1 = 1./tan(perigee_1->parameters()[Trk::theta]);
  double sphi_1 = sin(perigee_1->parameters()[Trk::phi]);
  double cphi_1 = cos(perigee_1->parameters()[Trk::phi]);
  Amg::Vector3D locXpVec_1;
  AmgMatrix(3,3) DtWD_1;
  AmgMatrix(3,3) vtx_weight;
  double chi2(0.);
  locXpVec_1[0]= -perigee_1->parameters()[Trk::d0]*sphi_1;
  locXpVec_1[1]= perigee_1->parameters()[Trk::d0]*cphi_1;
  locXpVec_1[2]= perigee_1->parameters()[Trk::z0];

  AmgMatrix(3,3) DtWD_sum;
  Amg::Vector3D DtWDx_sum;
  AmgMatrix(2,2) cov_mat_1 = (*perigee_1->covariance()).block<2,2>(0,0);
  // and the weightmatrix
  AmgMatrix(2,2) weight_mat_1 = cov_mat_1.inverse();
  AmgMatrix(2,3) D_1;
  D_1(0,0) = -sphi_1              ; D_1(0,1) = cphi_1              ; D_1(0,2) = 0.;
  D_1(1,0) = -cphi_1*cot_theta_1  ; D_1(1,1) = -sphi_1*cot_theta_1 ; D_1(1,2) = 1.;
  DtWD_1 = D_1.transpose()*weight_mat_1*D_1;
  vtx_weight = vtx.covariancePosition().inverse();
  DtWD_sum = DtWD_1 + vtx_weight;
  Amg::Vector3D vtx_pos; vtx_pos[0] = vtx.position().x(); vtx_pos[1] = vtx.position().y(); vtx_pos[2] = vtx.position().z();
  DtWDx_sum = DtWD_1*locXpVec_1 + vtx_weight*vtx_pos;
  AmgMatrix(3,3) cov_spatialPoint = DtWD_sum.inverse();
  spatialPoint = cov_spatialPoint * DtWDx_sum;
  Amg::Vector3D XpVec_sp_1 = locXpVec_1 - spatialPoint;
  Amg::Vector3D sp_vtx = spatialPoint - vtx_pos;
  chi2 =  XpVec_sp_1.dot(DtWD_1*XpVec_sp_1) + sp_vtx.dot(vtx_weight*sp_vtx);
  if (chi2 <= m_chi2) 
    { 
      if (msgLvl(MSG::DEBUG)) msg() <<"found spatial point = ("<<spatialPoint[0]<<", "<<spatialPoint[1]<<", "<<spatialPoint[2]<<")"<< endreq;
      return new Trk::Vertex(spatialPoint);
    } else 
    {
      if (msgLvl(MSG::DEBUG)) msg() <<"found spatial point candidate doesn't pass chi2_cut" << endreq;
      return 0;
    }

}

