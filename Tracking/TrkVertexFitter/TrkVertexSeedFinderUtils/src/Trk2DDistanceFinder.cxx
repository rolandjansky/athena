/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
    Trk2DDistanceFinder.cxx - Description in header file
*********************************************************************/

//#define SEEDNEWTONTRKDISTANCEFINDER_DEBUG


#include "TrkVertexSeedFinderUtils/Trk2DDistanceFinder.h"
#include "TrkVertexSeedFinderUtils/Trk2dDistanceSeeder.h"
#include "TrkVertexSeedFinderUtils/NewtonTrkDistanceFinder.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkTrack/Track.h"
#include <math.h>
#include "TrkParticleBase/TrackParticleBase.h"


namespace {
  inline double square(const double tosquare) {
    return std::pow(tosquare,2);
  }
  double dist(std::pair<const Amg::Vector3D,const Amg::Vector3D> pairofpos) {
    Amg::Vector3D diff(pairofpos.first-pairofpos.second);
    return std::sqrt(square(diff.x())+square(diff.y())+square(diff.z()));
  }

}

  
namespace Trk
{
    

  Trk2DDistanceFinder::Trk2DDistanceFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
    m_2ddistanceseeder("Trk::Trk2dDistanceSeeder"),
    m_numberOfMinimizationFailures(0)
  {   
    declareProperty("Trk2dDistanceSeeder",     m_2ddistanceseeder);
    declareInterface<ITrkDistanceFinder>(this);
  }

  Trk2DDistanceFinder::~Trk2DDistanceFinder() {}

  StatusCode Trk2DDistanceFinder::initialize() 
  { 

    //initialize number of failures to 0
    m_numberOfMinimizationFailures=0;
    
     StatusCode s = AlgTool::initialize();
    if (s.isFailure() )
    {
      msg(MSG::FATAL) << "AlgTool::initialize() initialize failed!" << endmsg;
      return StatusCode::FAILURE;
    }

    s = m_2ddistanceseeder.retrieve();
    if (s.isFailure())
      {
	msg(MSG::FATAL)<<"Could not find 2d distance seeder tool." << endmsg;
	return StatusCode::FAILURE;
      }
    msg(MSG::INFO) << "Initialize successful" << endmsg;
    return StatusCode::SUCCESS;
  }

  StatusCode Trk2DDistanceFinder::finalize() 
  {
    
    msg(MSG::INFO) << "Finalize successful. Number of failed minimizations: " << m_numberOfMinimizationFailures << ". Few per events is OK!" << endmsg;
    return StatusCode::SUCCESS;
  }


  /** method to do the calculation starting from two MeasuredPerigees*/
  /** return value is true if calculation is successfull */
  bool  Trk2DDistanceFinder::CalculateMinimumDistance(const Trk::Perigee & a, const Trk::Perigee & b) 
  {
    
    std::pair<PointOnTrack,PointOnTrack> minpoints; 

    try {
      minpoints=m_2ddistanceseeder->GetSeed(TwoTracks(a,b));
    } catch (...) {
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Problem with 2d analytic minimum distance finder" << endmsg;
      m_numberOfMinimizationFailures+=1;
      return false;
    }
    
    
#ifdef SEEDNEWTONTRKDISTANCEFINDER_DEBUG
//    m_log(MSG::DEBUG) << "Returned a_phi " << minpoints.first.getPhiPoint() << endmsg;
//    m_log(MSG::DEBUG) << "Returned b_phi " << minpoints.second.getPhiPoint() << endmsg;
    
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Returned a_phi " << minpoints.first.getPhiPoint() << endmsg;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Returned b_phi " << minpoints.second.getPhiPoint() << endmsg;
#endif
    
    return true;
    
  }
    
  /** method to do the calculation starting from two tracks */
  bool  Trk2DDistanceFinder::CalculateMinimumDistance(const  Trk::Track & a, const Trk::Track & b) {

    if (std::isnan(a.perigeeParameters()->parameters()[Trk::d0])||std::isnan(b.perigeeParameters()->parameters()[Trk::d0])) {
      msg(MSG::ERROR) << "Nan parameters in tracks. Cannot use them" << endmsg;
      return false;
    }
    
    return CalculateMinimumDistance(*(a.perigeeParameters()),*(b.perigeeParameters()));
    
  }

  /** method to do the calculation starting from two track particles */
  bool  Trk2DDistanceFinder::CalculateMinimumDistance(const  Trk::TrackParticleBase & a, const Trk::TrackParticleBase & b) {

    const Trk::TrackParameters& para=a.definingParameters();
    const Trk::TrackParameters& parb=b.definingParameters();

    const Trk::Perigee* parpera=dynamic_cast<const Trk::Perigee*>(&para);
    const Trk::Perigee* parperb=dynamic_cast<const Trk::Perigee*>(&parb);

    if (parpera==0||parperb==0) {
      msg(MSG::WARNING) << "Cannot cast to perigee. Neutral will be supported soon" << endmsg;
      return false;
    }

    if (std::isnan(parpera->parameters()[Trk::d0])||std::isnan(parperb->parameters()[Trk::d0])) {
      msg(MSG::ERROR) << "Nan parameters in tracks. Cannot use them" << endmsg;
      return false;
    }
    
    return CalculateMinimumDistance(*(parpera),*(parperb));
    
  }
  
  /**method to obtain the distance (call CalculateMinimumDistance before) **/
  double  Trk2DDistanceFinder::GetDistance() const {
    return dist(m_2ddistanceseeder->GetClosestPoints());//GetSeedPoint has to be implemented
  }
    
  /** method to obtain the points on the two tracks at minimum distance **/
  const std::pair<Amg::Vector3D,Amg::Vector3D>  Trk2DDistanceFinder::GetPoints() const {
    return m_2ddistanceseeder->GetClosestPoints();
  }

  
  
}
