/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrackSelectorTool/InDetConversionTrackSelectorTool.h"
// forward declare
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "VxVertex/Vertex.h"
#include "TrkTrack/Track.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "TrkParticleBase/TrackParticleBase.h"
// normal includes
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "VxVertex/RecVertex.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODTracking/Vertex.h"


namespace InDet
{

 InDetConversionTrackSelectorTool::InDetConversionTrackSelectorTool(const std::string& t, const std::string& n, const IInterface*  p)
 :AthAlgTool(t,n,p),
  m_trkSumTool("Trk::TrackSummaryTool"),
  m_extrapolator("Trk::Extrapolator"),
  m_iBeamCondSvc ("BeamCondSvc",n),
  m_maxSiD0   (35.),
  m_maxTrtD0 (100.),
  m_maxSiZ0  (200.),
  m_maxTrtZ0 (1200),
  m_minPt    (500.),
  m_trRatio1  (0.5),
  m_trRatio2  (0.1),
  m_trRatio3 (0.05),
  m_trRatioTRT(0.1),
  m_trRatioV0  (1.),
  m_sD0_Si     (2.),
  m_sD0_Trt   (0.5),
  m_sZ0_Trt    (3.),
  m_isConv(true)
 {
   declareInterface<ITrackSelectorTool>(this);
   declareProperty("TrackSummaryTool",     m_trkSumTool);
   declareProperty("Extrapolator",         m_extrapolator);
   declareProperty("maxSiD0",              m_maxSiD0);
   declareProperty("maxTrtD0",             m_maxTrtD0);
   declareProperty("maxSiZ0",              m_maxSiZ0);
   declareProperty("maxTrtZ0",             m_maxTrtZ0);
   declareProperty("minPt",                m_minPt);
   declareProperty("RatioCut1",            m_trRatio1);
   declareProperty("RatioCut2",            m_trRatio2);
   declareProperty("RatioCut3",            m_trRatio3);
   declareProperty("RatioTRT",             m_trRatioTRT);
   declareProperty("RatioV0",              m_trRatioV0);
   declareProperty("significanceD0_Si",    m_sD0_Si);
   declareProperty("significanceD0_Trt",   m_sD0_Trt);
   declareProperty("significanceZ0_Trt",   m_sZ0_Trt);
   declareProperty("IsConversion",         m_isConv);
   declareProperty("BeamPositionSvc",      m_iBeamCondSvc);
   declareProperty("PIDonlyForXe",         m_PIDonlyForXe = false,
     "Only check TRT PID if all hits are Xe hits");
 }

 InDetConversionTrackSelectorTool::~InDetConversionTrackSelectorTool()
 {}
  
 StatusCode  InDetConversionTrackSelectorTool::initialize()
 {
   StatusCode sc = AthAlgTool::initialize();
   if(sc.isFailure()){
     msg(MSG::ERROR)<<" Unable to initialize the AlgTool"<<endreq;
     return StatusCode::FAILURE;
   }

   /* Get the track summary tool from ToolSvc */
   if ( m_trkSumTool.retrieve().isFailure() ) {
     msg(MSG::ERROR) << "Failed to retrieve tool " << m_trkSumTool << endreq;
     return StatusCode::FAILURE;
   } else {
     msg(MSG::INFO) << "Retrieved tool " << m_trkSumTool << endreq;
   }

   /* Get the extrapolator tool from ToolSvc */
   if ( m_extrapolator.retrieve().isFailure() ) {
     msg(MSG::ERROR) << "Failed to retrieve tool " << m_extrapolator << endreq;
     return StatusCode::FAILURE;
   } else {
     msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endreq;
   }

   /* Get BeamCondSvc */
   sc = m_iBeamCondSvc.retrieve();
   if (sc.isFailure()) {
     msg(MSG::INFO) << "Could not find BeamCondSvc. Will use (0,0,0) if no vertex is given and extrapolation is needed." << endreq;
   }

   return StatusCode::SUCCESS;
 }
    
 StatusCode InDetConversionTrackSelectorTool::finalize()
 {
  msg(MSG::INFO)  << "Finalize successful" << endreq;
  return StatusCode::SUCCESS;
 }
    
 bool InDetConversionTrackSelectorTool::decision(const Trk::Track& track,const Trk::Vertex* vx) const
 {  
   bool pass = false;

   const Trk::Perigee* perigee=dynamic_cast<const Trk::Perigee*>(track.perigeeParameters());
   const Trk::Vertex* myVertex=vx;    
   //in case no Vertex is provided by the user, beam position will be used if available
   if (myVertex==0) {
     if (!m_iBeamCondSvc.empty()) {
       myVertex=new Trk::RecVertex(m_iBeamCondSvc->beamVtx());
     } else {
       msg(MSG::WARNING) << " Cannot get beamSpot center from iBeamCondSvc. Using (0,0,0)... " << endreq;
       myVertex=new Trk::Vertex(Amg::Vector3D(0,0,0));
     }
   }
    
   Trk::PerigeeSurface perigeeSurface(myVertex->position());
   const Trk::TrackParameters *firstmeaspar=0;
   for (unsigned int i=0;i<track.trackParameters()->size();i++){
     if ( (*track.trackParameters())[i]->covariance() && !dynamic_cast<const Trk::Perigee*>((*track.trackParameters())[i])) {
       firstmeaspar=(*track.trackParameters())[i];
       break;
     }
   }
   if (!firstmeaspar) {
     //assumes perigeeParameters exist...
     //no track selection if firstmeas + perigee does not exist !
     firstmeaspar=track.perigeeParameters();
     if (!firstmeaspar){
       msg(MSG::WARNING) << " First measurment on track is missing. Using perigee Parameters, but they are missing: 0 pointer! Track selection failed " << endreq;
       //clean up vertex
       if (myVertex!=vx) {
	 delete myVertex;
	 myVertex=0;
       }
       return false;
     }
   }

   const Trk::TrackParameters * extrapolatedParameters= m_extrapolator->extrapolate(*firstmeaspar,perigeeSurface,Trk::anyDirection,true,track.info().particleHypothesis() );
//   const Trk::TrackParameters * extrapolatedParameters= firstmeaspar ? m_extrapolator->extrapolate(*firstmeaspar,perigeeSurface,Trk::anyDirection,true,track.info().particleHypothesis() ) : 0;
   perigee = extrapolatedParameters ? dynamic_cast<const Trk::Perigee*>(extrapolatedParameters) : 0; 
   if (perigee==0 || !perigee->covariance() ) {
     msg(MSG::WARNING) << "Track Selector failed to extrapolate track to the vertex: " << myVertex->position() << endreq;
     if (extrapolatedParameters!=0) {
       msg(MSG::WARNING) << "The return object of the extrapolator was not a perigee even if a perigeeSurface was used!" << endreq;
       delete extrapolatedParameters;
       if (myVertex!=vx) delete myVertex;
       return false;
     }
     return false;
   }

   double qOverP = perigee->parameters()[Trk::qOverP];
   double pt = fabs(1/qOverP)*sin(perigee->parameters()[Trk::theta]);
   double d0 = perigee->parameters()[Trk::d0];
   double z0 = perigee->parameters()[Trk::z0];
   const Trk::TrackSummary* tSum = m_trkSumTool->createSummaryNoHoleSearch(track);
   if(tSum){
     double ratioTrk = 1.0;
     int nclus  = tSum->get(Trk::numberOfPixelHits) + tSum->get(Trk::numberOfSCTHits);
     int nTrtHits       = tSum->get(Trk::numberOfTRTHits);
     int nTrtOutliers   = tSum->get(Trk::numberOfTRTOutliers);
     int ntrt           = nTrtHits + nTrtOutliers;
     int nTrtXenonHits  = tSum->get(Trk::numberOfTRTXenonHits);

     if(m_isConv) {

       if(ntrt > 0 && (!m_PIDonlyForXe || nTrtXenonHits==ntrt) ) // only check TRT PID if m_PIDonlyForXe is false or all TRT hits are Xenon hits
	 ratioTrk = tSum->getPID(Trk::eProbabilityHT);

       if ( pt >= m_minPt ) {
	 if ( (nclus==0 && fabs(d0)<=m_maxTrtD0) || (nclus>0 && fabs(d0)<=m_maxSiD0) ) {
	   if ( (nclus==0 && fabs(z0)<=m_maxTrtZ0) || (nclus>0 && fabs(z0)<=m_maxSiZ0) ) {
	     if (nclus>0) {
	       if((ntrt<=15 && ratioTrk>=m_trRatio1) ||
		  (ntrt>15 && ntrt<=25 && ratioTrk>=m_trRatio2) ||
		  (ntrt>25 && ratioTrk>=m_trRatio3)) pass = true;
	     } else if (ratioTrk>=m_trRatioTRT) pass = true;  
	   }
	 }
       }
     } else {
       //The cuts below are necessary for the V0 track selection
       const AmgSymMatrix(5)& err = *perigee->covariance();
       double sd0sq = err(0,0);
       double sd0 = (sd0sq>0.)?sqrt(sd0sq):0.;
       double sz0sq = err(1,1);
       double sz0 = (sz0sq>0.)?sqrt(sz0sq):0.;
       if(nclus == 0){
	 if(fabs(d0)>=m_sD0_Trt*sd0 && fabs(d0)<=m_maxTrtD0 && fabs(z0)<=m_sZ0_Trt*sz0 && pt>=m_minPt) pass = true;
       }else{
	 if(fabs(d0)>=m_sD0_Si*sd0 && fabs(z0)<=m_maxSiZ0 && pt>=m_minPt) pass = true;
       }
       
       ratioTrk = 1.0;
       if(ntrt > 0 && (!m_PIDonlyForXe || nTrtXenonHits==ntrt) ) // only check TRT PID if m_PIDonlyForXe is false or all TRT hits are Xenon hits
	 ratioTrk = tSum->getPID(Trk::eProbabilityHT);

       if(ratioTrk>m_trRatioV0) pass = false;
     }

     delete tSum;
   } else pass = false;

   if (myVertex!=vx) delete myVertex;
   if (perigee!=track.perigeeParameters()) delete perigee;
    
   return pass;
 }

 bool InDetConversionTrackSelectorTool::decision(const Trk::TrackParticleBase& track,const Trk::Vertex* vx) const
 {
   bool pass = false;

   const Trk::TrackParameters* definintParameters=&(track.definingParameters());
   const Trk::Perigee* perigee=dynamic_cast<const Trk::Perigee*>(definintParameters);
   const Trk::Vertex* myVertex=vx;
   //in case no Vertex is provided by the user, beam position will be used if available
   if (myVertex==0) {
     if (!m_iBeamCondSvc.empty()) {
       myVertex=new Trk::RecVertex(m_iBeamCondSvc->beamVtx());
     } else {
       msg(MSG::WARNING) << " Cannot get beamSpot center from iBeamCondSvc. Using (0,0,0)... " << endreq;
       myVertex=new Trk::Vertex(Amg::Vector3D(0,0,0));
     }
   }

   Trk::PerigeeSurface perigeeSurface(myVertex->position());
   const Trk::TrackParameters *firstmeaspar=0;
   for (unsigned int i=0;i<track.trackParameters().size();i++){
     if ( (track.trackParameters())[i]->covariance() && !dynamic_cast<const Trk::Perigee*>((track.trackParameters())[i])) {
       firstmeaspar=(track.trackParameters())[i];
       break;
     }
   }

   if (!firstmeaspar) {
     //using perigee instead of firstmeasurement, since first measurement was not found...
     firstmeaspar=&(track.definingParameters());
     if (!firstmeaspar){
       msg(MSG::DEBUG) << " Track Paraemters at first measurement not found. Perigee not found. Cannot do TrackSelection..." << endreq;
       if (myVertex!=vx) {
	 delete myVertex;
	 myVertex=0;
       }
       return false;
     }
   }

   const Trk::TrackParameters * extrapolatedParameters= m_extrapolator->extrapolate(*firstmeaspar,perigeeSurface,Trk::anyDirection,true,Trk::pion );
   //const Trk::TrackParameters * extrapolatedParameters= firstmeaspar ? m_extrapolator->extrapolate(*firstmeaspar,perigeeSurface,Trk::anyDirection,true,Trk::pion ) : 0;
   perigee = extrapolatedParameters ? dynamic_cast<const Trk::Perigee*>(extrapolatedParameters) : 0; 
   if (perigee==0 || !perigee->covariance()) {
     msg(MSG::WARNING) << "Track Selector failed to extrapolate track to the vertex: " << myVertex->position() << endreq;
     if (extrapolatedParameters!=0) {
       msg(MSG::WARNING) << "The return object of the extrapolator was not a perigee even if a perigeeSurface was used!" << endreq;
       delete extrapolatedParameters;
       if (myVertex!=vx) delete myVertex;
       return false;
     }
     return false;
   }
    
   double qOverP = perigee->parameters()[Trk::qOverP];
   double pt = fabs(1/qOverP)*sin(perigee->parameters()[Trk::theta]);
   double d0 = perigee->parameters()[Trk::d0];
   double z0 = perigee->parameters()[Trk::z0];
   const Trk::TrackSummary* tSum = track.trackSummary();
   if(tSum){
     double ratioTrk = 1.0;
     int nclus  = tSum->get(Trk::numberOfPixelHits) + tSum->get(Trk::numberOfSCTHits);
     int nTrtHits       = tSum->get(Trk::numberOfTRTHits);
     int nTrtOutliers   = tSum->get(Trk::numberOfTRTOutliers);
     int ntrt           = nTrtHits + nTrtOutliers;
     int nTrtXenonHits  = tSum->get(Trk::numberOfTRTXenonHits);

     if(m_isConv){

       if(ntrt > 0 && (!m_PIDonlyForXe || nTrtXenonHits==ntrt) ) // only check TRT PID if m_PIDonlyForXe is false or all TRT hits are Xenon hits
	 ratioTrk = tSum->getPID(Trk::eProbabilityHT);

       if ( pt >= m_minPt ) {
	 if ( (nclus==0 && fabs(d0)<=m_maxTrtD0) || (nclus>0 && fabs(d0)<=m_maxSiD0) ) {
	   if ( (nclus==0 && fabs(z0)<=m_maxTrtZ0) || (nclus>0 && fabs(z0)<=m_maxSiZ0) ) {
	     if (nclus > 0) {
	       if((ntrt<=15 && ratioTrk>=m_trRatio1) ||
		  (ntrt>15 && ntrt<=25 && ratioTrk>=m_trRatio2) ||
		  (ntrt>25 && ratioTrk>=m_trRatio3)) pass = true;
	     } else if (ratioTrk>=m_trRatioTRT) pass = true;  
	   }
	 }
       }
     } else {
       //The cuts below are necessary for the V0 track selection
       const AmgSymMatrix(5)& err = *perigee->covariance();
       double sd0sq = err(0,0);
       double sd0 = (sd0sq>0.)?sqrt(sd0sq):0.;
       double sz0sq = err(1,1);
       double sz0 = (sz0sq>0.)?sqrt(sz0sq):0.;
       if(nclus == 0){
	 if(fabs(d0)>=m_sD0_Trt*sd0 && fabs(d0)<= m_maxTrtD0 && fabs(z0)<=m_sZ0_Trt*sz0 && pt>=m_minPt) pass = true;
       }else{
	 if(fabs(d0)>=m_sD0_Si*sd0 && fabs(z0)<=m_maxSiZ0 && pt>=m_minPt) pass = true;
       }

       ratioTrk = 1.0;
       if(ntrt > 0 && (!m_PIDonlyForXe || nTrtXenonHits==ntrt) ) // only check TRT PID if m_PIDonlyForXe is false or all TRT hits are Xenon hits
	 ratioTrk = tSum->getPID(Trk::eProbabilityHT);
       if(ratioTrk>m_trRatioV0) pass = false;
     }
   } else pass = false;

   if (myVertex!=vx) delete myVertex;
   if (perigee!=&(track.definingParameters())) delete perigee;
    
   return pass;
 }
 
 
   // ---------------------------------------------------------------------
  bool InDetConversionTrackSelectorTool::decision(const xAOD::TrackParticle& tp,const xAOD::Vertex* vertex) const 
  {
    
    
    
    
    bool pass = false;

    const Trk::Perigee& perigee=tp.perigeeParameters();
    
    //in case no Vertex is provided by the user, beam position will be used if available

    Trk::PerigeeSurface perigeeSurface( vertex ? vertex->position() : (!m_iBeamCondSvc.empty() ? m_iBeamCondSvc->beamVtx().position() : Amg::Vector3D(0,0,0) ) );
  
    const Trk::TrackParameters* extrapolatedParameters= m_extrapolator->extrapolate(perigee,perigeeSurface,Trk::anyDirection,false,Trk::pion );
    if (extrapolatedParameters==0) {
      ATH_MSG_WARNING( "Extrapolation to the vertex failed: " << perigeeSurface << std::endl << perigee );
      return false;
    }
    
    double qOverP = perigee.parameters()[Trk::qOverP];
    double pt = fabs(1/qOverP)*sin(perigee.parameters()[Trk::theta]);
    double d0 = extrapolatedParameters->parameters()[Trk::d0];
    double z0 = extrapolatedParameters->parameters()[Trk::z0];

    double ratioTrk = 1.0;
    int nclus  = getCount(tp,xAOD::numberOfPixelHits) + getCount(tp,xAOD::numberOfSCTHits);
    int nTrtHits       = getCount(tp,xAOD::numberOfTRTHits);
    int nTrtOutliers   = getCount(tp,xAOD::numberOfTRTOutliers);
    int ntrt           = nTrtHits + nTrtOutliers;
    int nTrtXenonHits  = getCount(tp,xAOD::numberOfTRTXenonHits);

    if(m_isConv){
     float temp(0);
     if(ntrt > 0 && (!m_PIDonlyForXe || nTrtXenonHits==ntrt) ){ // only check TRT PID if m_PIDonlyForXe is false or all TRT hits are Xenon hits
      ratioTrk = tp.summaryValue(temp,xAOD::eProbabilityHT)  ?  temp: 0 ;
     }
 
     if ( pt >= m_minPt ) {
       if ( (nclus==0 && fabs(d0)<=m_maxTrtD0) || (nclus>0 && fabs(d0)<=m_maxSiD0) ) {
         if ( (nclus==0 && fabs(z0)<=m_maxTrtZ0) || (nclus>0 && fabs(z0)<=m_maxSiZ0) ) {
           if (nclus > 0) {
             if((ntrt<=15 && ratioTrk>=m_trRatio1) ||
          (ntrt>15 && ntrt<=25 && ratioTrk>=m_trRatio2) ||
          (ntrt>25 && ratioTrk>=m_trRatio3)) pass = true;
           } else if (ratioTrk>=m_trRatioTRT) pass = true;  
         }
       }
     }
    } else {
     //The cuts below are necessary for the V0 track selection
     const AmgSymMatrix(5)& err = *perigee.covariance();
     double sd0sq = err(0,0);
     double sd0 = (sd0sq>0.)?sqrt(sd0sq):0.;
     double sz0sq = err(1,1);
     double sz0 = (sz0sq>0.)?sqrt(sz0sq):0.;
     if(nclus == 0){
       if(fabs(d0)>=m_sD0_Trt*sd0 && fabs(d0)<= m_maxTrtD0 && fabs(z0)<=m_sZ0_Trt*sz0 && pt>=m_minPt) pass = true;
     }else{
       if(fabs(d0)>=m_sD0_Si*sd0 && fabs(z0)<=m_maxSiZ0 && pt>=m_minPt) pass = true;
     }

     ratioTrk = 1.0;
     float temp(0);
     if(ntrt > 0 && (!m_PIDonlyForXe || nTrtXenonHits==ntrt) ) // only check TRT PID if m_PIDonlyForXe is false or all TRT hits are Xenon hits
      ratioTrk = tp.summaryValue(temp,xAOD::eProbabilityHT)  ?  temp: 0 ;
     if(ratioTrk>m_trRatioV0) pass = false;
    }

        
    delete extrapolatedParameters;
    return pass;
  } 
}//end of namespace definitions
