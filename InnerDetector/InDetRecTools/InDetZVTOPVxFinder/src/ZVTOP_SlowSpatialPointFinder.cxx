/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ZVTOP_SpatialPointFinder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetZVTOPVxFinder/ZVTOP_SlowSpatialPointFinder.h"
#include "TrkParameters/TrackParameters.h"
#include "VxVertex/Vertex.h"
#include "Particle/TrackParticle.h"
#include "EventPrimitives/EventPrimitives.h"
#include "VxVertex/LinearizedTrack.h"
#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"

//================ Constructor =================================================

InDet::ZVTOP_SlowSpatialPointFinder::ZVTOP_SlowSpatialPointFinder(const std::string& t, const std::string& n, const IInterface*  p )
  :
  AthAlgTool(t,n,p)
{
  declareInterface<IZVTOP_SpatialPointFinder>(this);
  //  template for property declaration
  declareProperty("Chi2_cut_value", m_chi2);
  declareProperty ( "LinearizedTrackFactory", m_linFactory );
}

//================ Destructor =================================================

InDet::ZVTOP_SlowSpatialPointFinder::~ZVTOP_SlowSpatialPointFinder()
{}


//================ Initialisation =================================================

StatusCode InDet::ZVTOP_SlowSpatialPointFinder::initialize()
{
  
  StatusCode sc = AlgTool::initialize();

  if (sc.isFailure()) return sc;
  // get tool svc
  if(m_linFactory.retrieve().isFailure())
    {
      msg (MSG::ERROR) <<"Could not find ToolSvc."<<endreq;
      return sc;
    } else msg (MSG::INFO) << "Retrieved tool " << m_linFactory << endreq;
  
  msg (MSG::INFO) << "initialize() successful in " << name() << endreq;
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::ZVTOP_SlowSpatialPointFinder::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}
//============================================================================================
Trk::Vertex* InDet::ZVTOP_SlowSpatialPointFinder::findSpatialPoint(const Trk::Track* trk_1, const Trk::Track* trk_2)
{
  const Trk::TrackParameters* perigee_1(dynamic_cast<const Trk::TrackParameters*>(trk_1->perigeeParameters()));
  const Trk::TrackParameters* perigee_2(dynamic_cast<const Trk::TrackParameters*>(trk_2->perigeeParameters()));
  if (!perigee_1 | !perigee_2) {
    if (msgLvl(MSG::VERBOSE)) msg() << "Dynamic cast to MeasuredPerigee failed. Skipping this pair" << endreq;
    return 0;
  } else {
		Trk::Vertex* vertex = findSpatialPoint(perigee_1, perigee_2);
		return vertex;
  }//if measured perigee
}

//============================================================================================

Trk::Vertex* InDet::ZVTOP_SlowSpatialPointFinder::findSpatialPoint(const Trk::RecVertex vtx, const Trk::Track* trk_1)
{
  const Trk::TrackParameters *perigee_1(dynamic_cast<const Trk::TrackParameters*>(trk_1->perigeeParameters()));
  if (!perigee_1) {
    if (msgLvl(MSG::VERBOSE)) msg() << "Dynamic cast to MeasuredPerigee failed. Skipping this pair" << endreq;
    return 0;
  } else {
    //we need Trk::Vertex
    Trk::Vertex* vertex = findSpatialPoint(vtx, perigee_1);
    return vertex;
  }
}

//============================================================================================
Trk::Vertex* InDet::ZVTOP_SlowSpatialPointFinder::findSpatialPoint(const Rec::TrackParticle* trk_1, const Rec::TrackParticle* trk_2)
{
  const Trk::TrackParameters* perigee_1(trk_1->measuredPerigee());
  const Trk::TrackParameters* perigee_2(trk_2->measuredPerigee());
  if (!perigee_1 | !perigee_2) {
    if (msgLvl(MSG::VERBOSE)) msg() << "Dynamic cast to MeasuredPerigee failed. Skipping this pair" << endreq;
    return 0;
  } else {
    //we need Trk::Vertex
    Trk::Vertex* vertex = findSpatialPoint(perigee_1, perigee_2);
    return vertex;
  }//if measured perigee
}

//============================================================================================
Trk::Vertex* InDet::ZVTOP_SlowSpatialPointFinder::findSpatialPoint(const Trk::RecVertex vtx, const Rec::TrackParticle* trk_1)
{
  const Trk::TrackParameters* perigee_1(trk_1->measuredPerigee());
  if (!perigee_1) {
    if (msgLvl(MSG::VERBOSE)) msg() << "Dynamic cast to MeasuredPerigee failed. Skipping this pair" << endreq;
    return 0;
  } else {
    Trk::Vertex* vertex = findSpatialPoint(vtx, perigee_1);
    return vertex;
  }
}

//============================================================================================
Trk::Vertex* InDet::ZVTOP_SlowSpatialPointFinder::findSpatialPoint(const Trk::TrackParticleBase* trk_1, const Trk::TrackParticleBase* trk_2)
{
  const Trk::TrackParameters* perigee_1 = dynamic_cast<const Trk::TrackParameters*>(&(trk_1)->definingParameters());
  const Trk::TrackParameters* perigee_2 = dynamic_cast<const Trk::TrackParameters*>(&(trk_2)->definingParameters());
  if (!perigee_1 | !perigee_2) {
    if (msgLvl(MSG::VERBOSE)) msg() << "Dynamic cast to MeasuredPerigee failed. Skipping this pair" << endreq;
    return 0;
  } else {
    //we need Trk::Vertex
    Trk::Vertex* vertex = findSpatialPoint(perigee_1, perigee_2);
    return vertex;
  }//if measured perigee
}
//============================================================================================
Trk::Vertex* InDet::ZVTOP_SlowSpatialPointFinder::findSpatialPoint(const Trk::RecVertex vtx, const Trk::TrackParticleBase* trk_1)
{
  const Trk::TrackParameters* perigee_1 = dynamic_cast<const Trk::TrackParameters*>(&(trk_1)->definingParameters());
  if (!perigee_1) {
    if (msgLvl(MSG::VERBOSE)) msg() << "Dynamic cast to MeasuredPerigee failed. Skipping this pair" << endreq;
    return 0;
  } else {
    Trk::Vertex* vertex = findSpatialPoint(vtx, perigee_1);
    return vertex;
  }
}

//=============================================================================================

Trk::Vertex* InDet::ZVTOP_SlowSpatialPointFinder::findSpatialPoint(const Trk::TrackParameters* perigee_1, const Trk::TrackParameters* perigee_2)
{
  Amg::Vector3D spatialPoint;
  //we need Trk::Vertex
  double chi2(0.);
  const Amg::Vector3D lin_point(0.,0.,0.); // we start our linearization always at ATLAS origin!
  Trk::Vertex linPoint(lin_point);
  for (unsigned int i = 0; i < 3; i++){
    Trk::LinearizedTrack* linTrack1 = m_linFactory->linearizedTrack (perigee_1, linPoint.position() );
    Trk::LinearizedTrack* linTrack2 = m_linFactory->linearizedTrack (perigee_2, linPoint.position() );
    if (linTrack1 && linTrack2)
      {
	Amg::Vector3D locXpVec_1 = linTrack1->expectedPositionAtPCA();
	locXpVec_1[0] = locXpVec_1[0] - linPoint.position()[0];
	locXpVec_1[1] = locXpVec_1[1] - linPoint.position()[1];
	locXpVec_1[2] = locXpVec_1[2] - linPoint.position()[2];
	
	Amg::Vector3D locXpVec_2 = linTrack2->expectedPositionAtPCA();
	locXpVec_2[0] = locXpVec_2[0] - linPoint.position()[0];
	locXpVec_2[1] = locXpVec_2[1] - linPoint.position()[1];
	locXpVec_2[2] = locXpVec_2[2] - linPoint.position()[2];
	
	AmgMatrix(2,2) billoirCovMat_1 = linTrack1->expectedCovarianceAtPCA().block<2,2>(0,0);
	AmgMatrix(2,2) billoirCovMat_2 = linTrack2->expectedCovarianceAtPCA().block<2,2>(0,0);
	AmgMatrix(2,2) billoirWeightMat_1 = billoirCovMat_1.inverse().eval();
	AmgMatrix(2,2) billoirWeightMat_2 = billoirCovMat_2.inverse().eval();
	AmgMatrix(3,3) DtWD_sum;
	Amg::Vector3D DtWDx_sum;
	AmgMatrix(3,3) DtWD_1;
	AmgMatrix(3,3) DtWD_2;
	// D matrix for d0 and z0
	AmgMatrix(2,3) D_1 = linTrack1->positionJacobian().block<2,3>(0,0);
	AmgMatrix(2,3) D_2 = linTrack2->positionJacobian().block<2,3>(0,0);
	// Calculate DtWD and DtWD*x and sum them
	DtWD_1 = D_1.transpose()*billoirWeightMat_1*D_1;
	DtWD_2 = D_2.transpose()*billoirWeightMat_2*D_2;
	DtWD_sum = DtWD_1 + DtWD_2;
	DtWDx_sum = DtWD_1 * locXpVec_1 + DtWD_2* locXpVec_2;
	AmgMatrix(3,3) cov_spatialPoint = DtWD_sum.inverse();
	spatialPoint = cov_spatialPoint * DtWDx_sum;
	Amg::Vector3D XpVec_sp_1 = locXpVec_1 - spatialPoint;
	Amg::Vector3D XpVec_sp_2 = locXpVec_2 - spatialPoint;
	chi2 =  XpVec_sp_1.dot(DtWD_1*XpVec_sp_1) + XpVec_sp_2.dot(DtWD_2*XpVec_sp_2);
	spatialPoint[0] += linPoint.position()[0];
	spatialPoint[1] += linPoint.position()[1];
	spatialPoint[2] += linPoint.position()[2];
	if (msgLvl(MSG::VERBOSE)) msg() <<"found spatial point = ("<<spatialPoint[0]<<", "<<spatialPoint[1]<<", "<<spatialPoint[2]<<")"<< endreq;
	if (msgLvl(MSG::VERBOSE)) msg() <<"chi2 = "<<chi2<<endreq;
	linPoint = Trk::Vertex(spatialPoint);
      }	
    delete linTrack1; linTrack1=0; delete linTrack2; linTrack2=0;
  }// two iterations
  if (chi2 <= m_chi2) return new Trk::Vertex(spatialPoint);
  else {
    if (msgLvl(MSG::VERBOSE)) msg() <<"found spatial point candidate doesn't pass chi2_cut" << endreq;
    return 0;
  }
}

//========================================================================================================
Trk::Vertex* InDet::ZVTOP_SlowSpatialPointFinder::findSpatialPoint(const Trk::RecVertex vtx, const Trk::TrackParameters* perigee_1)
{
  Amg::Vector3D spatialPoint;
  //we need Trk::Vertex
  double chi2(0.);
  const Amg::Vector3D lin_point(0.,0.,0.); // we start our linearization always at ATLAS origin!
  Trk::Vertex linPoint(lin_point);
  for (unsigned int i = 0; i < 3; i++){
    Trk::LinearizedTrack* linTrack1 = m_linFactory->linearizedTrack (perigee_1, linPoint.position() );
    if (linTrack1)
      {
	Amg::Vector3D locXpVec_1(0.,0.,0.);
	AmgMatrix(3,3) DtWD_1;
	AmgMatrix(3,3) vtx_weight;
	locXpVec_1[0]= locXpVec_1[0] - linPoint.position()[0];
	locXpVec_1[1]= locXpVec_1[1] - linPoint.position()[1];
	locXpVec_1[2]= locXpVec_1[2] - linPoint.position()[2];

	AmgMatrix(3,3) DtWD_sum;
	Amg::Vector3D DtWDx_sum;
	AmgMatrix(2,2) billoirCovMat_1 = linTrack1->expectedCovarianceAtPCA().block<2,2>(0,0);
	AmgMatrix(2,2) billoirWeightMat_1 = billoirCovMat_1.inverse().eval();
	// D matrix for d0 and z0
	AmgMatrix(2,3) D_1 = linTrack1->positionJacobian().block<2,3>(0,0);
	// Calculate DtWD and DtWD*x and sum them
	DtWD_1 = D_1.transpose()*billoirWeightMat_1*D_1 ;
	vtx_weight = vtx.covariancePosition().inverse();
	DtWD_sum = DtWD_1 + vtx_weight;
	Amg::Vector3D vtx_pos(3,0); vtx_pos[0] = vtx.position().x(); vtx_pos[1] = vtx.position().y(); vtx_pos[2] = vtx.position().z();
	DtWDx_sum =  DtWD_1 * locXpVec_1 + vtx_weight*vtx_pos;
	AmgMatrix(3,3) cov_spatialPoint = DtWD_sum.inverse();
	spatialPoint = cov_spatialPoint * DtWDx_sum;
	Amg::Vector3D XpVec_sp_1 = locXpVec_1 - spatialPoint;
	Amg::Vector3D sp_vtx = spatialPoint - vtx_pos;
	chi2 =  XpVec_sp_1.dot(DtWD_1*XpVec_sp_1) + sp_vtx.dot(vtx_weight*sp_vtx);
	spatialPoint[0] += linPoint.position()[0];
	spatialPoint[1] += linPoint.position()[1];
	spatialPoint[2] += linPoint.position()[2];
	if (msgLvl(MSG::VERBOSE)) msg() <<"found spatial point = ("<<spatialPoint[0]<<", "<<spatialPoint[1]<<", "<<spatialPoint[2]<<")"<< endreq;
	if (msgLvl(MSG::VERBOSE)) msg() <<"chi2 = "<<chi2<<endreq;
	linPoint = Trk::Vertex (spatialPoint);
      }
    delete linTrack1; linTrack1=0; 
  }
  if (chi2 <= m_chi2) return new Trk::Vertex(spatialPoint);
  else {
    if (msgLvl(MSG::VERBOSE)) msg() <<"found spatial point candidate doesn't pass chi2_cut" << endreq;
    return 0;
  }
}
