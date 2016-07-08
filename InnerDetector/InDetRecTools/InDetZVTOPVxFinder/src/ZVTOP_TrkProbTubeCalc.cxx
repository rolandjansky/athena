/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ZVTOP_TrkProbTubeCalc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetZVTOPVxFinder/ZVTOP_TrkProbTubeCalc.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "VxVertex/Vertex.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "EventPrimitives/EventPrimitives.h"
#include "TrkExInterfaces/IExtrapolator.h"

//================ Constructor =================================================

InDet::ZVTOP_TrkProbTubeCalc::ZVTOP_TrkProbTubeCalc(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p)
{
  declareInterface<IZVTOP_TrkProbTubeCalc>(this);
  //  template for property declaration
  declareProperty("Extrapolator",		m_extrapolator);
}

//================ Destructor =================================================

InDet::ZVTOP_TrkProbTubeCalc::~ZVTOP_TrkProbTubeCalc()
{}


//================ Initialisation =================================================

StatusCode InDet::ZVTOP_TrkProbTubeCalc::initialize()
{
  
  StatusCode sc = AlgTool::initialize();

  if (sc.isFailure()) return sc;
  
  /* Get the right extrapolator tool from ToolSvc */
  if ( m_extrapolator.retrieve().isFailure() ) {
    msg (MSG::FATAL) << "Failed to retrieve tool " << m_extrapolator << endreq;
    return StatusCode::FAILURE;
  } else {
    msg (MSG::INFO) << "Retrieved tool " << m_extrapolator << endreq;
  }
  msg (MSG::INFO) << "initialize() successful in " << name() << endreq;
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::ZVTOP_TrkProbTubeCalc::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}

//============================================================================================
double InDet::ZVTOP_TrkProbTubeCalc::calcProbTube(const Trk::Track& trk, Trk::Vertex& vec)
{
  double probTube = 0.;
  //perigee surface 
  const Trk::Perigee* trkPer(dynamic_cast<const Trk::Perigee*>(trk.perigeeParameters()));
  probTube = calcProbTube(trkPer, vec);
  return probTube;
}

//============================================================================================
double InDet::ZVTOP_TrkProbTubeCalc::calcProbTube(const Trk::RecVertex& beam_spot, Trk::Vertex& vec)
{
  double probTube = 0.;
  Amg::Vector3D diff; 
  diff[0]= beam_spot.position()[0] - vec.position()[0];
  diff[1]= beam_spot.position()[1] - vec.position()[1];
  diff[2]= beam_spot.position()[2] - vec.position()[2];
  AmgMatrix(3,3) beam_spot_weight = beam_spot.covariancePosition().inverse();
  probTube = exp(-0.5*diff.transpose()*beam_spot_weight*diff);
  return probTube;
}
//============================================================================================
double InDet::ZVTOP_TrkProbTubeCalc::calcProbTube(const Rec::TrackParticle& trk, Trk::Vertex& vec)
{
  double probTube = 0.;
  const Trk::Perigee* trkPer(trk.measuredPerigee());
  probTube = calcProbTube(trkPer,vec);
  return probTube;
}

double InDet::ZVTOP_TrkProbTubeCalc::calcProbTube(const Trk::TrackParticleBase& trk, Trk::Vertex& vec)
{
  double probTube = 0.;
  //perigee surface 
  const Trk::Perigee* trkPer = dynamic_cast<const Trk::Perigee*>(&(trk.definingParameters()));
  probTube = calcProbTube(trkPer,vec);
  return probTube;
}

double InDet::ZVTOP_TrkProbTubeCalc::calcProbTube(const Trk::Perigee* trkPer, Trk::Vertex& vec){

  double probTube = 0.;
  Amg::Vector3D lp =vec.position();
  Trk::PerigeeSurface perigeeSurface(lp);
  double f_phi0 = trkPer->parameters()[Trk::phi0];
  double f_theta = trkPer->parameters()[Trk::theta];
  double f_qOverP = trkPer->parameters()[Trk::qOverP];
  Trk::Perigee * extrapolatedPerigee(const_cast<Trk::Perigee*>(dynamic_cast<const Trk::Perigee*>(m_extrapolator->extrapolateDirectly(*trkPer, perigeeSurface))));
  if (extrapolatedPerigee) {
    AmgVector(5) diff;
    diff[0]= extrapolatedPerigee->parameters()[Trk::d0];
    diff[1]= extrapolatedPerigee->parameters()[Trk::z0];
    diff[2]= extrapolatedPerigee->parameters()[Trk::phi0] - f_phi0;
    diff[3]= extrapolatedPerigee->parameters()[Trk::theta] - f_theta;
    diff[4]= extrapolatedPerigee->parameters()[Trk::qOverP] - f_qOverP;
    if (extrapolatedPerigee->covariance() != 0) {
      AmgMatrix(5,5) exp_perigee_weight = (*extrapolatedPerigee->covariance()).inverse();
      probTube = exp(-0.5*diff.transpose()*exp_perigee_weight*diff);
    } else {
      if (msgLvl(MSG::DEBUG)) msg()  << "extrapolateted perigee has NO information on the covariance matrix" << endreq;
    }
  } else {
    if (msgLvl(MSG::DEBUG)) msg() << "Perigee was not extrapolated!" << endreq;
  }
  delete extrapolatedPerigee;
  return probTube;
}
