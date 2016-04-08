/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrkSegmentConverter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSegment/Segment.h"
#include "TrkParameters/TrackParameters.h"
//#include "TrkParameters/AtaStraightLine.h"
#include "TrkSegmentConverter/TrkSegmentConverter.h"
#include "TrkEventPrimitives/FitQuality.h"
//#include "TrkParameters/MeasuredAtaStraightLine.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
//#include "TrkParameters/MeasuredPerigee.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkTrack/Track.h"

// Amg
#include "EventPrimitives/EventPrimitives.h"

//================ Constructor =================================================

Trk::TrkSegmentConverter::TrkSegmentConverter(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_useFitter(0),
  m_trackFitter("Trk::KalmanFitter/TrkKalmanFitter"),
  m_extrapolator ("Trk::Extrapolator/InDetExtrapolator")
{
  declareInterface<ISegmentConverterTool>(this);
  
  declareProperty("UseFitter",    m_useFitter = true);  
  declareProperty("TrackFitter",  m_trackFitter ); 
  declareProperty("Extrapolator", m_extrapolator); //Extrapolator tool
}

//================ Destructor =================================================

Trk::TrkSegmentConverter::~TrkSegmentConverter()
{}


//================ Initialisation =================================================

StatusCode Trk::TrkSegmentConverter::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;
  
  sc = m_extrapolator.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_extrapolator << endreq;
    return StatusCode::FAILURE;
  }else{
    msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endreq;
  }

  if(m_useFitter){
    sc = m_trackFitter.retrieve();
    if (sc.isFailure())
      {
	ATH_MSG_FATAL ("Could not find tool " << m_trackFitter<<". Exiting.");
	return sc;
      }
    else{
      ATH_MSG_INFO (" Got " << m_trackFitter << " as TrackFitter. ");
    }
  }

  ATH_MSG_INFO ("initialize() successful in " << name() );

  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Trk::TrkSegmentConverter::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}

//================ The conversion routine from the interface ======================

Trk::Track* Trk::TrkSegmentConverter::convert(const Trk::Segment& segment)
{
  Track* track=0;
  if(m_useFitter)
    track=convertWithFitter(segment);
  else
    track=convertBruteForce(segment);

  return track;
}

//================ Convert with a TrackFitter ======================================
Trk::Track* Trk::TrkSegmentConverter::convertWithFitter(const Trk::Segment& segment)
{
  Track* track=0;
  MeasurementSet myset;

  int nROTs=segment.numberOfMeasurementBases();

  if(nROTs>0){

    for(int i=0;i<nROTs;++i)
      myset.push_back(segment.measurement(i));
    
    ATH_MSG_DEBUG (" numberOfContainedRots: " << nROTs );
    
    Amg::Vector3D inputMatchingPos(segment.globalPosition());
 
    ATH_MSG_DEBUG (" created inputMatchingPos " );
    
    Amg::Vector3D inputMatchingMom(segment.globalPosition().x(),
				   segment.globalPosition().y(),
				   segment.globalPosition().z());
    
    ATH_MSG_DEBUG (" created inputMatchingMom ");
    
    const StraightLineSurface* testSf
      = dynamic_cast<const Trk::StraightLineSurface*>(&(segment.associatedSurface()));
    if(!testSf) { ATH_MSG_DEBUG (" Cannot dynamic cast segment surface to straight-line ! " ); return track;}
    
    ATH_MSG_DEBUG (" created testSf ");
    
    
    const AtaStraightLine* inputMatchingPar
      = new AtaStraightLine(inputMatchingPos,inputMatchingMom,
			    1., *testSf);
    
    ATH_MSG_DEBUG ( " created inputMatchingPar " );
    
    Amg::Vector3D startMomentum( 0., 0., 1.);
    track=m_trackFitter->fit(myset,
			     *inputMatchingPar,
			     false,
			     Trk::nonInteracting
			     );
    if(track)
      ATH_MSG_DEBUG (" Successfull fit of track. ");
  }

  return track;
}
//================ Convert using brute force =======================================
Trk::Track* Trk::TrkSegmentConverter::convertBruteForce(const Trk::Segment& tS)
{
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Transforming the TRT segment into a track..." << endreq;

  DataVector<const Trk::TrackStateOnSurface>* ntsos = new DataVector<const Trk::TrackStateOnSurface>;

  //Get the track segment fit quality. If not there drop segment
  const Trk::FitQuality* fq = tS.fitQuality() ? tS.fitQuality()->clone() : 0;
  //if(!fq) {if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Segment has no fit quality!Discard..." << endreq; return 0;}

  //Get the track segment information and build the initial track parameters
  const Amg::VectorX& p = tS.localParameters();
  AmgSymMatrix(5)* ep = new AmgSymMatrix(5);
  *ep = tS.localCovariance();
  const Trk::TrackParameters* segPar = tS.associatedSurface().createTrackParameters(p(0),p(1),p(2),p(3),p(4),ep);
  if(segPar){
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initial TRT Segment Parameters for refitting " << (*segPar) << endreq;
  }else{
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Could not get initial TRT segment parameters! " << endreq;
    return 0;
  }

  for(int it=0; it<int(tS.numberOfMeasurementBases()); it++){
    // on first measurement add parameters
    const Trk::TrackStateOnSurface* seg_tsos = 0;
    
    if ( dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(tS.measurement(it)) ) {
      Amg::Vector3D perigeePosition(0.,0.,0.);
      // --- create surface at perigee
      Trk::PerigeeSurface perigeeSurface(perigeePosition);
      const Trk::TrackParameters* parm = m_extrapolator->extrapolate(*segPar, perigeeSurface);
      const Trk::Perigee* perParm = dynamic_cast<const Trk::Perigee*>(parm); 
      if(!perParm) {if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Failed to build perigee parameters.Discard..." << endreq; delete ntsos; return 0;}
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
      typePattern.set(Trk::TrackStateOnSurface::Perigee);
      seg_tsos = new Trk::TrackStateOnSurface(0,perParm,0,0,typePattern);
    }
    if(it>=1){
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
      typePattern.set(Trk::TrackStateOnSurface::Measurement);
      if(it==1) {
	seg_tsos = new Trk::TrackStateOnSurface(tS.measurement(it)->clone(),segPar,0,0,typePattern);
      }else{
	seg_tsos = new Trk::TrackStateOnSurface(tS.measurement(it)->clone(),0,0,0,typePattern);
      }
    }
    ntsos->push_back(seg_tsos);
  }  

  ///Construct the new track
  Trk::TrackInfo info;
  info.setPatternRecognitionInfo(Trk::TrackInfo::TRTStandalone);
  
  Trk::Track* newTrack = new Trk::Track(info, ntsos, fq);

  return newTrack;
}
//============================================================================================

