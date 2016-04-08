/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetFixedWindowTrackTimeTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetCosmicsEventPhase/InDetFixedWindowTrackTimeTool.h"

#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"

#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"
#include "TRT_ConditionsData/RtRelation.h"
#include "TRT_ConditionsData/BasicRtRelation.h"

#include "InDetIdentifier/TRT_ID.h"

//================ Constructor =================================================

InDet::InDetFixedWindowTrackTimeTool::InDetFixedWindowTrackTimeTool( std::string const & t
								     , std::string const & n
								     , IInterface  const * p
								     ) : AthAlgTool(t,n,p)
								       , m_averageT0(0.)
								       , m_trtconddbsvc("TRT_CalDbSvc",n)
{
  declareInterface<IInDetCosmicsEventPhaseTool>(this);
  declareProperty("TRTCalDbSvc", m_trtconddbsvc);
  declareProperty("UseTRTCalibration",m_gett0=true);
  declareProperty("UseNewEP", m_useNewEP=true);
  declareProperty("GlobalOffset", m_globalOffset=10.);
  declareProperty("WindowCenter", m_windowCenter=-8.5);
  declareProperty("WindowSize", m_windowSize=7.);
}

//================ Destructor =================================================

InDet::InDetFixedWindowTrackTimeTool::~InDetFixedWindowTrackTimeTool()
{}


//================ Initialisation =================================================

StatusCode InDet::InDetFixedWindowTrackTimeTool::initialize()
{
  StatusCode sc = AlgTool::initialize();
  if (sc.isFailure()) return sc;

  sc= m_trtconddbsvc.retrieve();
  if(sc.isFailure())
    {
      ATH_MSG_FATAL( "Failed to retrieve TRT Calibration DB Service!" );
      return sc;
    }
  
  ATH_MSG_INFO( "initialize() successful in " << name() );

  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::InDetFixedWindowTrackTimeTool::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}

//============================================================================================

void InDet::InDetFixedWindowTrackTimeTool::beginRun() 
{
  m_averageT0 = 0.;

  if (!m_useNewEP) 
    {
      ATH_MSG_INFO( "InDetFixedWindowTrackTimeTool::beginRun, averageT0 = 0 (m_useNewEP=false) " ); 
      return;
    }

   TRT_ID const * TRTHelper;

    if (detStore()->retrieve(TRTHelper, "TRT_ID").isFailure()) {
     ATH_MSG_FATAL( "Could not get TRT ID helper" );
     return;
    }

    int countAverageT0 = 0; // should be equal to 32*2*1642  105088
    double rtShift = 0.; // shift from 0 of the RT relation

    for (std::vector<Identifier>::const_iterator it = TRTHelper->straw_layer_begin(); it != TRTHelper->straw_layer_end(); it++  )
      {
	int nStrawsInLayer = TRTHelper->straw_max(*it);
	for (int i=0; i<=nStrawsInLayer; i++)
	  {
	    Identifier id = TRTHelper->straw_id(*it, i);
	    if ( abs(TRTHelper->barrel_ec(id)) != 1 ) // average only for barrel straws
	      continue;
	    m_averageT0 += m_trtconddbsvc->getT0(id); // access straw T0 same as elsewhere, countAverageT0++;
	    countAverageT0++;
	    const TRTCond::RtRelation *rtRelation = m_trtconddbsvc->getRtRelation(id);
	    if (!rtRelation) 
	      {
		ATH_MSG_DEBUG( "rtRelation missing in InDetCosmicsEventPhase::initialize!" );
		continue; 
	      }
	    rtShift += rtRelation->drifttime(0.);
	  }
      }
   
    if(countAverageT0)//CID 13692
    m_averageT0 /= ( double(countAverageT0) );
    double averageT0 = m_averageT0;
    if(countAverageT0)//CID 13693
    rtShift /= ( double(countAverageT0) );

    m_averageT0 -= 20.; // shift so that the range is about the same as before
    m_averageT0 += rtShift;

    //print value for test
    ATH_MSG_DEBUG( "The number of straws seen is: " << countAverageT0 
		   << " and the values expected is:  " << 105088 );
    ATH_MSG_DEBUG( "The average T0 is: "          << averageT0 
		   << ", average RT(r=0) is "     << rtShift
		   << " and we are subtracting: " << m_averageT0 );
    ATH_MSG_INFO( "InDetFixedWindowTrackTimoeol::beginRun Using updated EP calculation (December 2009), subtracting: " << m_averageT0 
		  << " ns (average T0: " << averageT0 
		  << " ns, average RT(r=0): " << rtShift << " ns), GlobalOffset: " << m_globalOffset <<  " ns." );

  return;
}


double InDet::InDetFixedWindowTrackTimeTool::findPhase( Trk::Track const * track )
{
  ATH_MSG_DEBUG( "Finding phase..." );
  
  std::vector<float> data(4);
  data.push_back(0.); //0 0.1 -0.00087 0
  data.push_back(0.1);
  data.push_back(-0.00087);
  data.push_back(0.);
  
  TRTCond::RtRelation const *rtr = (m_gett0) ? 0
                                             : new TRTCond::BasicRtRelation(data);
  
  double timeresidualsum = 0;
  size_t ntrthits = 0;  

  std::vector<Trk::TrackStateOnSurface const *>::const_iterator trackStateItr = track->trackStateOnSurfaces()->begin();
  std::vector<Trk::TrackStateOnSurface const *>::const_iterator trackStateEnd = track->trackStateOnSurfaces()->end();
  for( ; trackStateItr!= trackStateEnd; ++trackStateItr )
    {
      Trk::MeasurementBase const * mesb = (*trackStateItr)->measurementOnTrack();
      if(  !mesb
	|| !(*trackStateItr)->type(Trk::TrackStateOnSurface::Measurement) 
	) 
	continue;
      
      InDet::TRT_DriftCircleOnTrack const * trtcirc = dynamic_cast<InDet::TRT_DriftCircleOnTrack const *>(mesb);
      
      if( !trtcirc )
	continue;
      
      InDet::TRT_DriftCircle const * rawhit = trtcirc->prepRawData();
      if( !rawhit )
	continue;

      if(  !rawhit->driftTimeValid() 
	|| rawhit->firstBinHigh() 
	)
	continue;
      
      Identifier const & ident = trtcirc->identify();
      double rawdrifttime = rawhit->rawDriftTime();
      
      double t0 = (m_gett0) ? m_trtconddbsvc->getT0(ident)
	                    : 0;
      ATH_MSG_DEBUG( "T0 : " << t0 );
      
      if (m_gett0)
	rtr = m_trtconddbsvc->getRtRelation(ident) ;
      
      Trk::TrackParameters const * tparp=((*trackStateItr)->trackParameters());
      if( !tparp )
	continue;

      double trkdistance  = tparp->parameters()[Trk::driftRadius];
      double trkdrifttime = rtr->drifttime( fabs(trkdistance) ) ;
      double timeresidual = rawdrifttime - t0 + m_averageT0 - trkdrifttime;
      
      ATH_MSG_DEBUG( "trkdistance=" << trkdistance 
		     << "  trkdrifttime=" << trkdrifttime 
		     << "  timeresidual=" << timeresidual
		     << " rawdrifttime=" << rawdrifttime );
      
      if( fabs(timeresidual-m_windowCenter) < m_windowSize
	  && fabs(trkdistance) < 2.8 )
	{
	  timeresidualsum += timeresidual ;
	  ++ntrthits ;
	}
    }
  if( !m_gett0 ) 
    delete rtr;
  
  ATH_MSG_DEBUG( "timeresidualsum = " << timeresidualsum ); 
  ATH_MSG_DEBUG( "ntrtrhits = " << ntrthits );

  if( ntrthits > 1 )
    return timeresidualsum/ntrthits + m_globalOffset;
  return 0.;
}


double InDet::InDetFixedWindowTrackTimeTool::findPhase( Trk::Segment const * segment)
{
  std::vector<float> data(4);
  data.push_back(0.); //0 0.1 -0.00087 0
  data.push_back(0.1);
  data.push_back(-0.00087);
  data.push_back(0.);
  
  TRTCond::RtRelation const *rtr = (m_gett0) ? 0
                                             : new TRTCond::BasicRtRelation(data);
  
  double sum_tr = 0.;
  double sum_goodhits = 0.;
  
  int nhits=segment->numberOfMeasurementBases();
  for( int i=0; i < nhits; ++i )
    {
      Trk::RIO_OnTrack const * rio = dynamic_cast<Trk::RIO_OnTrack const *>(segment->measurement(i));
      
      InDet::TRT_DriftCircleOnTrack const * trtcirc = dynamic_cast<InDet::TRT_DriftCircleOnTrack const *>(rio);
      if( !trtcirc ) 
	continue;
      
      InDet::TRT_DriftCircle const * rawhit = trtcirc->prepRawData();
      if( !rawhit )
	continue;
      
      if(!rawhit->lastBinHigh() && !rawhit->isNoise())
	{
	  Identifier const & ident = trtcirc->identify();
	  
	  if (m_gett0)
	    rtr = m_trtconddbsvc->getRtRelation(ident) ;
	  
	  double full_drifttime=rtr->drifttime(2.0);
	  
	  sum_tr+=rawhit->trailingEdge()*3.125-full_drifttime;
	  
	  ATH_MSG_VERBOSE( "Hit "   << sum_goodhits 
			   << " : " << rawhit->trailingEdge()*3.125 
			   << "  fulldrifttime=" << full_drifttime );
	  
	  sum_goodhits += 1;
	}
      else
	{
	  ATH_MSG_VERBOSE( "Hit has lastbin high" );
	}
    }
  
  if (!m_gett0) 
    delete rtr;
    
    
  if(sum_goodhits>1)
    return sum_tr/sum_goodhits + m_globalOffset;
  return 0;
}


double InDet::InDetFixedWindowTrackTimeTool::findPhaseFromTE(Trk::Track const * track)
{
  ATH_MSG_DEBUG( "Finding phase..." );
  
  double timeresidualsum = 0;
  size_t ntrthits = 0;  
  
  std::vector<Trk::TrackStateOnSurface const *>::const_iterator trackStateItr = track->trackStateOnSurfaces()->begin();
  std::vector<Trk::TrackStateOnSurface const *>::const_iterator trackStateEnd = track->trackStateOnSurfaces()->end();
  for( ; trackStateItr!= trackStateEnd; ++trackStateItr)
    {
      Trk::MeasurementBase const * mesb=(*trackStateItr)->measurementOnTrack();
      if(  !mesb 
	|| !(*trackStateItr)->type(Trk::TrackStateOnSurface::Measurement)
	) 
	continue;
      
      InDet::TRT_DriftCircleOnTrack const * trtcirc = dynamic_cast<InDet::TRT_DriftCircleOnTrack const *>(mesb);
      if( !trtcirc )
	continue;

      InDet::TRT_DriftCircle const * rawhit = trtcirc->prepRawData();
      if( !rawhit )
	continue;
      
      Identifier const & ident = trtcirc->identify();
      double rawtrailingedge = rawhit->trailingEdge()*3.125;
      
      double t0 = (m_gett0) ? m_trtconddbsvc->getT0(ident)
	                    : 0;
      
      ATH_MSG_DEBUG( "T0 : " << t0 );
      
      double timeresidual = rawtrailingedge - t0 + m_averageT0;
      
      ATH_MSG_DEBUG( "timeresidual=" << timeresidual );
      
      if(timeresidual < 2000)
	{
	  timeresidualsum += timeresidual ;
	  ++ntrthits ;
	}
    }
  
  if(ntrthits>1)
    return timeresidualsum/ntrthits + m_globalOffset;
  return 0.;
}


