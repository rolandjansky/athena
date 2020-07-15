/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkJetVxFitter/TrkDistanceFinderNeutralNeutral.h"

namespace {

}

namespace Trk {

TrkDistanceFinderNeutralNeutral::TrkDistanceFinderNeutralNeutral(const std::string& t, const std::string& n, const IInterface*  p) : 
  AthAlgTool(t,n,p)
{
  declareInterface<TrkDistanceFinderNeutralNeutral>(this);
  
}


StatusCode TrkDistanceFinderNeutralNeutral::initialize() 
  { 
    StatusCode s = AthAlgTool::initialize();
    ATH_MSG_INFO("Initialize successful");
    return s;
  }

StatusCode TrkDistanceFinderNeutralNeutral::finalize() 
{
  ATH_MSG_INFO("Finalize successful");
  return StatusCode::SUCCESS;
}


TrkDistanceFinderNeutralNeutral::~TrkDistanceFinderNeutralNeutral() = default;

std::pair<Amg::Vector3D,double>  
TrkDistanceFinderNeutralNeutral::getPointAndDistance(const Trk::NeutralTrack& neutralaxis,
                                                     const Trk::NeutralTrack& neutraltrk,
						     double & distanceOnAxis) const {

  const Trk::NeutralTrack& neutraltrk1=neutralaxis;
  const Trk::NeutralTrack& neutraltrk2=neutraltrk;

  Amg::Vector3D DeltaR0(neutraltrk1.position()-neutraltrk2.position());
  const Amg::Vector3D& Mom1(neutraltrk1.momentum());
  const Amg::Vector3D& Mom2(neutraltrk2.momentum());

  double p1p2=Mom1.dot(Mom2);
  double p1DR0=Mom1.dot(DeltaR0);
  double p2DR0=Mom2.dot(DeltaR0);
  double p1p1=Mom1.mag2();
  double p2p2=Mom2.mag2();
  
  double lambda1=(p1DR0*p2p2-p1p2*p2DR0)/(p1p2*p1p2-p1p1*p2p2);
  double lambda2=(p1DR0*p1p2-p1p1*p2DR0)/(p1p2*p1p2-p1p1*p2p2);

  Amg::Vector3D pos1=neutraltrk1.position()+lambda1*Mom1;
  Amg::Vector3D pos2=neutraltrk2.position()+lambda2*Mom2;

  ATH_MSG_VERBOSE ("x1fin " << pos1.x() << "y1fin " << pos1.y() << "z1fin " << pos1.z());
  ATH_MSG_VERBOSE ("x2fin " << pos2.x() << "y2fin " << pos2.y() << "z2fin " << pos2.z());

  double distance = (pos1-pos2).mag();

  distanceOnAxis = lambda1/sqrt(p1p1);

  ATH_MSG_VERBOSE ("orig distance " << DeltaR0.mag() << " final distance " << distance);
  ATH_MSG_VERBOSE ("fitted dist from 1 " << lambda1*sqrt(p1p1));
  ATH_MSG_VERBOSE ("fitted dist from 2 " << lambda2*sqrt(p2p2));

  return std::pair<Amg::Vector3D,double>(pos1,distance);//give back position on neutral track... (jet axis)
  
}
  
}
