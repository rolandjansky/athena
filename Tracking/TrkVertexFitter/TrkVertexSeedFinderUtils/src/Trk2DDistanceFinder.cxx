/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
    Trk2DDistanceFinder.cxx - Description in header file
*********************************************************************/

//#define SEEDNEWTONTRKDISTANCEFINDER_DEBUG


#include "TrkVertexSeedFinderUtils/Trk2DDistanceFinder.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkTrack/Track.h"
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"
#include "TrkVertexSeedFinderUtils/Trk2dDistanceSeeder.h"
#include <cmath>


namespace Trk
{
    

  Trk2DDistanceFinder::Trk2DDistanceFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    base_class(t,n,p),
    m_2ddistanceseeder("Trk::Trk2dDistanceSeeder"),
    m_numberOfMinimizationFailures(0)
  {   
    declareProperty("Trk2dDistanceSeeder",     m_2ddistanceseeder);
  }

  Trk2DDistanceFinder::~Trk2DDistanceFinder() = default;

  StatusCode Trk2DDistanceFinder::initialize() 
  { 

    //initialize number of failures to 0
    m_numberOfMinimizationFailures=0;
    
    ATH_CHECK( AlgTool::initialize() );
    ATH_CHECK( m_2ddistanceseeder.retrieve() );
    ATH_MSG_DEBUG( "Initialize successful" );
    return StatusCode::SUCCESS;
  }

  StatusCode Trk2DDistanceFinder::finalize() 
  {
    ATH_MSG_DEBUG( "Finalize successful. Number of failed minimizations: " << m_numberOfMinimizationFailures << ". Few per events is OK!" );
    return StatusCode::SUCCESS;
  }


  /** method to do the calculation starting from two MeasuredPerigees*/
  /** return value is true if calculation is successfull */
  std::optional<ITrkDistanceFinder::TwoPoints>
  Trk2DDistanceFinder::CalculateMinimumDistance(const Trk::Perigee & a,
                                                const Trk::Perigee & b) const
  {
    std::pair<PointOnTrack,PointOnTrack> minpoints;
    Trk::TwoPoints points;

    try {
      minpoints=m_2ddistanceseeder->GetSeed(TwoTracks(a,b), &points);
    } catch (...) {
      ATH_MSG_WARNING( "Problem with 2d analytic minimum distance finder" );
      m_numberOfMinimizationFailures+=1;
      return std::nullopt;
    }
    
    
#ifdef SEEDNEWTONTRKDISTANCEFINDER_DEBUG
    ATH_MSG_DEBUG( "Returned a_phi " << minpoints.first.getPhiPoint() );
    ATH_MSG_DEBUG( "Returned b_phi " << minpoints.second.getPhiPoint() );
#endif
    
    return points;
    
  }
    
  /** method to do the calculation starting from two tracks */
  std::optional<ITrkDistanceFinder::TwoPoints>
  Trk2DDistanceFinder::CalculateMinimumDistance(const  Trk::Track & a,
                                                const Trk::Track & b) const
  {
    if (std::isnan(a.perigeeParameters()->parameters()[Trk::d0])||std::isnan(b.perigeeParameters()->parameters()[Trk::d0])) {
      ATH_MSG_ERROR( "Nan parameters in tracks. Cannot use them" );
      return std::nullopt;
    }
    
    return CalculateMinimumDistance(*(a.perigeeParameters()),*(b.perigeeParameters()));
  }


  /** method to do the calculation starting from two track particles */
  std::optional<ITrkDistanceFinder::TwoPoints>
  Trk2DDistanceFinder::CalculateMinimumDistance(const  Trk::TrackParticleBase & a,
                                                const Trk::TrackParticleBase & b) const
  {
    const Trk::TrackParameters& para=a.definingParameters();
    const Trk::TrackParameters& parb=b.definingParameters();

    const Trk::Perigee* parpera=dynamic_cast<const Trk::Perigee*>(&para);
    const Trk::Perigee* parperb=dynamic_cast<const Trk::Perigee*>(&parb);

    if (parpera==nullptr||parperb==nullptr) {
      ATH_MSG_WARNING("Cannot cast to perigee. Neutral will be supported soon" );
      return std::nullopt;
    }

    if (std::isnan(parpera->parameters()[Trk::d0])||std::isnan(parperb->parameters()[Trk::d0])) {
      ATH_MSG_ERROR( "Nan parameters in tracks. Cannot use them" );
      return std::nullopt;
    }
    
    return CalculateMinimumDistance(*(parpera),*(parperb));
    
  }
  

} // namespace Trk

