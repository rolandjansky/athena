/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
    SeedNewtonTrkDistanceFinder.cxx - Description in header file
*********************************************************************/

//#define SEEDNEWTONTRKDISTANCEFINDER_DEBUG


#include "TrkVertexSeedFinderUtils/SeedNewtonTrkDistanceFinder.h"
#include "TrkVertexSeedFinderUtils/Trk2dDistanceSeeder.h"
#include "TrkVertexSeedFinderUtils/NewtonTrkDistanceFinder.h"
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkTrack/Track.h"
#include <math.h>
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParameters/TrackParameters.h"


namespace {
#if 0
  inline const Amg::Vector3D operator + ( const Amg::Vector3D & first, const Amg::Vector3D & second) {
    return Amg::Vector3D( first.x()+second.x(),first.y()+second.y(),first.z()+second.z());
  }
#endif
  inline const Amg::Vector3D operator - ( const Amg::Vector3D & first, const Amg::Vector3D & second) {
    return Amg::Vector3D( first.x()-second.x(),first.y()-second.y(),first.z()-second.z());
  }
#if 0
  inline const Amg::Vector3D operator / ( const Amg::Vector3D & first, const double second) {
    return Amg::Vector3D( first.x()/second,first.y()/second,first.z()/second);
  }
#endif
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
    

  SeedNewtonTrkDistanceFinder::SeedNewtonTrkDistanceFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
    m_2ddistanceseeder("Trk::Trk2dDistanceSeeder"),
    m_distancefinder("Trk::NewtonTrkDistanceFinder"),
    m_numberOfMinimizationFailures(0)
  {   
    declareProperty("Trk2dDistanceSeeder",     m_2ddistanceseeder);
    declareProperty("TrkDistanceFinderImplementation",     m_distancefinder);
    declareInterface<ITrkDistanceFinder>(this);
  }

  SeedNewtonTrkDistanceFinder::~SeedNewtonTrkDistanceFinder() {}

  StatusCode SeedNewtonTrkDistanceFinder::initialize() 
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
    s = m_distancefinder.retrieve();
    if (s.isFailure())
      {
	msg(MSG::FATAL)<<"Could not find newton distance finder implementation tool." << endmsg;
	return StatusCode::FAILURE;
      }
    msg(MSG::INFO) << "Initialize successful" << endmsg;
    return StatusCode::SUCCESS;
  }

  StatusCode SeedNewtonTrkDistanceFinder::finalize() 
  {
    
    msg(MSG::INFO) << "Finalize successful. Number of failed minimizations: " << m_numberOfMinimizationFailures << ". Few per events is OK!" << endmsg;
    return StatusCode::SUCCESS;
  }


  /** method to do the calculation starting from two MeasuredPerigees*/
  /** return value is true if calculation is successfull */
  bool  SeedNewtonTrkDistanceFinder::CalculateMinimumDistance(const Trk::Perigee & a, const Trk::Perigee & b) 
  {
    
//defragmenting the meory: local variable instead of private data member    
    std::pair<PointOnTrack,PointOnTrack> minpoints; 
   
    //try first to get the minimum directly with the Newton method
    try {
//      m_minpoints=m_distancefinder->GetClosestPoints(a,b); 
      minpoints=m_distancefinder->GetClosestPoints(a,b); 
    } catch (Error::NewtonProblem e) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Problem with Newton finder: " << e.p << endmsg;
      try {
//	m_minpoints=m_2ddistanceseeder->GetSeed(TwoTracks(a,b));
//	m_minpoints=m_distancefinder->GetClosestPoints(m_minpoints);
        minpoints=m_2ddistanceseeder->GetSeed(TwoTracks(a,b));
        minpoints=m_distancefinder->GetClosestPoints(minpoints);
 
      } catch (Error::NewtonProblem e) {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Problem with Newton finder, even after 2d seeder: no minimum between tracks found" << e.p << endmsg;
        m_numberOfMinimizationFailures+=1;
	return false;
      } catch (...) {
        m_numberOfMinimizationFailures+=1;
	return false;
      }
    }
    
#ifdef SEEDNEWTONTRKDISTANCEFINDER_DEBUG
//    m_log(MSG::DEBUG) << "Returned a_phi " << m_minpoints.first.getPhiPoint() << endmsg;
//    m_log(MSG::DEBUG) << "Returned b_phi " << m_minpoints.second.getPhiPoint() << endmsg;

 if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Returned a_phi " << minpoints.first.getPhiPoint() << endmsg;
 if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Returned b_phi " << minpoints.second.getPhiPoint() << endmsg;
#endif

    return true;
    

  }
    
  /** method to do the calculation starting from two tracks */
  bool  SeedNewtonTrkDistanceFinder::CalculateMinimumDistance(const  Trk::Track & a, const Trk::Track & b) {

    if (std::isnan(a.perigeeParameters()->parameters()[Trk::d0])||std::isnan(b.perigeeParameters()->parameters()[Trk::d0])) {
      msg(MSG::ERROR) << "Nan parameters in tracks. Cannot use them" << endmsg;
      return false;
    }
    
    return CalculateMinimumDistance(*(a.perigeeParameters()),*(b.perigeeParameters()));
    
  }

  /** method to do the calculation starting from two track particles */
  bool  SeedNewtonTrkDistanceFinder::CalculateMinimumDistance(const  Trk::TrackParticleBase & a, const Trk::TrackParticleBase & b) {

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
  double  SeedNewtonTrkDistanceFinder::GetDistance() const {
    return dist(m_distancefinder->GetClosestPoints());//GetSeedPoint has to be implemented
  }
    
  /** method to obtain the points on the two tracks at minimum distance **/
  const std::pair<Amg::Vector3D,Amg::Vector3D>  SeedNewtonTrkDistanceFinder::GetPoints() const {
    return m_distancefinder->GetClosestPoints();
  }

  
  
}
