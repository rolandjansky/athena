/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::TRT_TrackExtensionTool_xk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////


#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "InDetRecToolInterfaces/ITRT_DetElementsRoadMaker.h"
#include "TRT_TrackExtensionTool_xk/TRT_TrackExtensionTool_xk.h"
#include "InDetRecToolInterfaces/ITrtDriftCircleCutTool.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include <list>
#include <iostream>

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_TrackExtensionTool_xk::TRT_TrackExtensionTool_xk
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
    m_roadtool  ("InDet::TRT_DetElementsRoadMaker_xk"                                                ),
    m_proptool    ("Trk::RungeKuttaPropagator"                                                       ),
    m_updatortool ("Trk::KalmanUpdator_xk"                                                           ),
    m_selectortool("InDet::InDetTrtDriftCircleCutTool"                                               ),
    m_riontrackD("InDet::TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackTool"                      ),
    m_riontrackN("InDet::TRT_DriftCircleOnTrackNoDriftTimeTool/TRT_DriftCircleOnTrackNoDriftTimeTool")
{
  m_fieldmode       = "MapSolenoid"      ;
  m_trtmanager      = "TRT"              ;
  m_minNumberDCs    = 9                  ;
  m_minNumberSCT    = 5                  ;
  m_minNumberPIX    = 2                  ;
  m_roadwidth       = 10.                ;
  m_maxslope        = .00005             ;
  m_zVertexWidth    = 150.               ;
  m_impact          = 50.                ;
  m_segmentFindMode = 3                  ;
  m_usedriftrad     = true               ;
  m_parameterization= true               ;
  m_scale_error     = 2.                 ;
  declareInterface<ITRT_TrackExtensionTool>(this);
  declareProperty("RoadTool"               ,m_roadtool        );
  declareProperty("PropagatorTool"         ,m_proptool        );
  declareProperty("UpdatorTool"            ,m_updatortool     );
  declareProperty("DriftCircleCutTool"     ,m_selectortool    );
  declareProperty("RIOonTrackToolYesDr"    ,m_riontrackD      );
  declareProperty("RIOonTrackToolNoDr"     ,m_riontrackN      );
  declareProperty("TrtManagerLocation"     ,m_trtmanager      );
  declareProperty("RoadWidth"              ,m_roadwidth       );
  declareProperty("ZVertexHalfWidth"       ,m_zVertexWidth    );
  declareProperty("maxImpactParameter"     ,m_impact          );
  declareProperty("Maxslope"               ,m_maxslope        );
  declareProperty("MinNumberDriftCircles"  ,m_minNumberDCs    );
  declareProperty("UseParameterization"    ,m_parameterization);
  declareProperty("UseDriftRadius"         ,m_usedriftrad     );
  declareProperty("ScaleHitUncertainty"    ,m_scale_error     );
  declareProperty("SegmentFindMode"        ,m_segmentFindMode );
  declareProperty("MagneticFieldMode"      ,m_fieldmode       );
  declareProperty("MinNumberSCTclusters"   ,m_minNumberSCT    );
  declareProperty("MinNumberPIXclusters"   ,m_minNumberPIX    );
  declareProperty("minTRTSegmentpT"        ,m_minTRTSegmentpT=300. );
}

///////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////

InDet::TRT_TrackExtensionTool_xk::~TRT_TrackExtensionTool_xk()
{
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackExtensionTool_xk::initialize()
{
  StatusCode sc = AlgTool::initialize();

  // Get tTools servise
  //
  IToolSvc* toolSvc;
  if ((sc=service("ToolSvc", toolSvc)).isFailure())  {
    msg(MSG::FATAL)<<"Toll service not found !"<<endmsg;
    return StatusCode::FAILURE;
  }

  // Get magnetic field service
  //
  if(m_fieldmode != "NoField" ) {
    ATH_CHECK( m_fieldCondObjInputKey.initialize());
  }

  if     (m_fieldmode == "NoField"    ) m_fieldprop = Trk::MagneticFieldProperties(Trk::NoField  );
  else if(m_fieldmode == "MapSolenoid") m_fieldprop = Trk::MagneticFieldProperties(Trk::FastField);
  else                                  m_fieldprop = Trk::MagneticFieldProperties(Trk::FullField);

  // Get RIO_OnTrack creator with drift time information
  //
  if(m_riontrackD.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_riontrackD <<endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_riontrackD << endmsg;
  }

  // Get RIO_OnTrack creator without drift time information
  //
  if(m_riontrackN.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_riontrackN <<endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_riontrackN << endmsg;
  }

  // Get detector elements road maker tool
  //
  if(m_roadtool.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_roadtool <<endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_roadtool << endmsg;
  }

  // Get propagator tool
  //
  if(m_proptool.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_proptool <<endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_proptool << endmsg;
  }

  // Get updator tool
  //
  if(m_updatortool.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_updatortool <<endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_updatortool << endmsg;
  }

  // Get segment selector tool
  //
  if(m_selectortool.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_selectortool <<endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_selectortool << endmsg;
  }



  // TRT
  if (detStore()->retrieve(m_trtid,"TRT_ID").isFailure()) {
    msg(MSG::FATAL) << "Could not get TRT ID helper" << endmsg;
    return StatusCode::FAILURE;
  }


  //Initialize container
  ATH_CHECK(m_trtname.initialize());

  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize-
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackExtensionTool_xk::finalize()
{
   StatusCode sc = AlgTool::finalize(); return sc;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackExtensionTool_xk::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)  return dumpEvent(out);
  return dumpConditions(out);
}


///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackExtensionTool_xk::dumpConditions( MsgStream& out ) const
{
  int n = 64-m_proptool.type().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");
  n     = 64-m_updatortool.type().size();
  std::string s10; for(int i=0; i<n; ++i) s10.append(" "); s10.append("|");

  std::string fieldmode[9] ={"NoField"       ,"ConstantField","SolenoidalField",
			     "ToroidalField" ,"Grid3DField"  ,"RealisticField" ,
			     "UndefinedField","AthenaField"  , "?????"         };

  int mode = m_fieldprop.magneticFieldMode();
  if(mode<0 || mode>8 ) mode = 8;

  n     = 64-fieldmode[mode].size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  n     = 64-m_roadtool.type().size();
  std::string s6; for(int i=0; i<n; ++i) s6.append(" "); s6.append("|");
  n     = 64-m_trtname.key().size();
  std::string s7; for(int i=0; i<n; ++i) s7.append(" "); s7.append("|");
  n     = 64-m_riontrackD.type().size();
  std::string s8; for(int i=0; i<n; ++i) s8.append(" "); s8.append("|");
  n     = 64-m_riontrackN.type().size();
  std::string s9; for(int i=0; i<n; ++i) s9.append(" "); s9.append("|");

  out<<"|----------------------------------------------------------------------"
     <<"---------------------|"
     <<std::endl;
  out<<"| TRT container           | "<<m_trtname.key()           <<s7 <<std::endl;
  out<<"| Tool for propagation    | "<<m_proptool    .type()<<s1 <<std::endl;
  out<<"| Tool for updator        | "<<m_updatortool .type()<<s10<<std::endl;
  out<<"| Tool for rio  on trackD | "<<m_riontrackD.type()<<s8 <<std::endl;
  out<<"| Tool for rio  on trackN | "<<m_riontrackN.type()<<s9 <<std::endl;
  out<<"| Tool for road builder   | "<<m_roadtool  .type()<<s6 <<std::endl;
  out<<"| Magnetic field mode     | "<<fieldmode[mode]<<s3<<std::endl;
  out<<"| Segments find  mode     | "
     <<std::setw(12)<<m_segmentFindMode
     <<"                                                    |"<<std::endl;
  out<<"| TRT road half width (mm)| "
     <<std::setw(12)<<std::setprecision(5)<<m_roadwidth
     <<"                                                    |"<<std::endl;
  out<<"| Min number DriftCircles | "
     <<std::setw(12)<<m_minNumberDCs
     <<"                                                    |"<<std::endl;
  out<<"| Use drift time  ?       | "
     <<std::setw(12)<<m_usedriftrad
     <<"                                                    |"<<std::endl;
 out<<"| Z vertex half width      | "
     <<std::setw(12)<<m_zVertexWidth
     <<"                                                    |"<<std::endl;
 out<<"| Scalefactor hit error    | "
     <<std::setw(12)<<m_scale_error
     <<"                                                    |"<<std::endl;
 out<<"| Max impact parameter     | "
     <<std::setw(12)<<m_impact
     <<"                                                    |"<<std::endl;
  out<<"|----------------------------------------------------------------------"
     <<"---------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the ostream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackExtensionTool_xk::dumpEvent( MsgStream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::TRT_TrackExtensionTool_xk::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    <<
  (MsgStream& sl,const InDet::TRT_TrackExtensionTool_xk& se)
{
  return se.dump(sl);
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator <<
  (std::ostream& sl,const InDet::TRT_TrackExtensionTool_xk& se)
{
  return se.dump(sl);
}

///////////////////////////////////////////////////////////////////
// Track extension initiation
///////////////////////////////////////////////////////////////////

std::unique_ptr<InDet::ITRT_TrackExtensionTool::IEventData>
InDet::TRT_TrackExtensionTool_xk::newEvent(const EventContext& ctx) const
{
  SG::ReadHandle<TRT_DriftCircleContainer> trtcontainer(m_trtname, ctx);

  if((not trtcontainer.isValid()) && m_outputlevel<=0) {
    std::stringstream msg;
    msg << "Missing TRT_DriftCircleContainer " << m_trtname.key();
    throw std::runtime_error( msg.str() );
  }

  // Get AtlasFieldCache
  MagField::AtlasFieldCache fieldCache;

  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
  if (fieldCondObj == nullptr) {
      ATH_MSG_ERROR("InDet::TRT_TrackExtensionTool_xk::findSegment: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
      return 0;
  }
  fieldCondObj->getInitializedCache (fieldCache);

  Trk::MagneticFieldProperties     fieldprop =  ( fieldCache.solenoidOn()
                                                  ? m_fieldprop
                                                  : Trk::MagneticFieldProperties(Trk::NoField  ));

  std::unique_ptr<EventData> event_data(new EventData(trtcontainer.cptr(), m_maxslope));
  event_data->m_trajectory.set(fieldprop, fieldCondObj);
  event_data->m_trajectory.set (m_trtid,
                                &(*m_proptool),
                                &(*m_updatortool),
                                &(*m_riontrackD),
                                &(*m_riontrackN),
                                m_roadwidth,
                                m_zVertexWidth,
                                m_impact,
                                m_scale_error,
                                m_minTRTSegmentpT);
  event_data->m_measurement.reserve(200);

  return std::unique_ptr<InDet::ITRT_TrackExtensionTool::IEventData>(event_data.release());
}

///////////////////////////////////////////////////////////////////
// Main methods for track extension to TRT
///////////////////////////////////////////////////////////////////

std::vector<const Trk::MeasurementBase*>&
InDet::TRT_TrackExtensionTool_xk::extendTrack(const EventContext& ctx,
                                              const Trk::Track& Tr,
                                              InDet::ITRT_TrackExtensionTool::IEventData &virt_event_data) const
{
  InDet::TRT_TrackExtensionTool_xk::EventData &
     event_data=InDet::TRT_TrackExtensionTool_xk::EventData::getPrivateEventData(virt_event_data);

  event_data.m_measurement.clear();

  const DataVector<const Trk::TrackStateOnSurface>*
    tsos = Tr.trackStateOnSurfaces();

  const Trk::TrackParameters*
    par = (*(tsos->rbegin()))->trackParameters(); if(!par ) return event_data.m_measurement;
  const Trk::TrackParameters*
    parb = (*(tsos->begin()))->trackParameters();


  if(parb && par!=parb) {

    const Amg::Vector3D& g1 = par ->position();
    const Amg::Vector3D& g2 = parb->position();
    if((g2.x()*g2.x()+g2.y()*g2.y()) > (g1.x()*g1.x()+g1.y()*g1.y())) par=parb;
  }
  return extendTrackFromParameters(ctx, *par, event_data);
}

///////////////////////////////////////////////////////////////////
// Main methods for track extension to TRT for pixles+sct tracks
///////////////////////////////////////////////////////////////////

std::vector<const Trk::MeasurementBase*>&
InDet::TRT_TrackExtensionTool_xk::extendTrack(const EventContext& ctx,
                                              const Trk::TrackParameters& par,
                                              InDet::ITRT_TrackExtensionTool::IEventData &virt_event_data) const
{
  InDet::TRT_TrackExtensionTool_xk::EventData &
     event_data=InDet::TRT_TrackExtensionTool_xk::EventData::getPrivateEventData(virt_event_data);
  return extendTrackFromParameters(ctx, par, event_data);
}


std::vector<const Trk::MeasurementBase*>&
InDet::TRT_TrackExtensionTool_xk::extendTrackFromParameters(const EventContext& ctx,
                                                            const Trk::TrackParameters& par,
                                                            InDet::TRT_TrackExtensionTool_xk::EventData &event_data) const
{
  event_data.m_measurement.clear();
  if(isGoodExtension(ctx, par,event_data)) event_data.m_trajectory.convert(event_data.m_measurement);
  return event_data.m_measurement;
}

///////////////////////////////////////////////////////////////////
// Main methods for segment finding in TRT for TRT seeds
///////////////////////////////////////////////////////////////////

Trk::TrackSegment*
InDet::TRT_TrackExtensionTool_xk::findSegment(const EventContext& ctx,
                                              const Trk::TrackParameters& par,
                                              InDet::ITRT_TrackExtensionTool::IEventData &virt_event_data) const
{
  InDet::TRT_TrackExtensionTool_xk::EventData &
     event_data=InDet::TRT_TrackExtensionTool_xk::EventData::getPrivateEventData(virt_event_data);

  int nCut = m_minNumberDCs;
  if(m_parameterization) {nCut = m_selectortool->minNumberDCs(&par); if(nCut<m_minNumberDCs) nCut=m_minNumberDCs;}


  // Get AtlasFieldCache
  MagField::AtlasFieldCache fieldCache;

  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
  if (fieldCondObj == nullptr) {
      ATH_MSG_ERROR("InDet::TRT_TrackExtensionTool_xk::findSegment: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
      return 0;
  }
  fieldCondObj->getInitializedCache (fieldCache);


  // TRT detector elements road builder
  //
  std::vector<const InDetDD::TRT_BaseElement*> DE;
  m_roadtool->detElementsRoad(ctx, fieldCache, par, Trk::alongMomentum,DE);

  if(int(DE.size())< nCut) return 0;

  // Array pointers to surface preparation
  //
  std::list<const Trk::Surface*> surfaces;
  int roadsize = 0;
  std::vector<const InDetDD::TRT_BaseElement*>::iterator d,de=DE.end();
  for(d=DE.begin(); d!=de; ++d) {
    surfaces.push_back(&(*d)->surface()); if(++roadsize==399) break;
  }

  // Global position on surfaces production
  //
  Trk::PatternTrackParameters Tp; if(!Tp.production(&par)) return 0;
  std::list< std::pair<Amg::Vector3D,double> > gpos;
  m_proptool->globalPositions(ctx, Tp, surfaces, gpos, m_fieldprop);

  // Initiate trajectory
  //

  event_data.m_trajectory.initiateForTRTSeed(gpos,DE,event_data.m_trtcontainer,Tp);

  if(event_data.m_trajectory.naElements() < nCut) return 0;
  event_data.m_maxslope      = .0002     ;

  if(event_data.m_trajectory.isFirstElementBarrel()) {

    if(m_usedriftrad)  event_data.m_trajectory.trackFindingWithDriftTime   (event_data.m_maxslope);
    else               event_data.m_trajectory.trackFindingWithoutDriftTime(event_data.m_maxslope);
    if(!event_data.m_trajectory.searchStartStop()) {return 0;}
  }
  else   {

    event_data.m_trajectory.trackFindingWithoutDriftTime(event_data.m_maxslope);
    if(!event_data.m_trajectory.searchStartStop()) { return 0;}
    event_data.m_trajectory.radiusCorrection();
    if(m_usedriftrad) event_data.m_trajectory.trackFindingWithDriftTimeBL   (event_data.m_maxslope);
    else              event_data.m_trajectory.trackFindingWithoutDriftTimeBL(event_data.m_maxslope);
    if(!event_data.m_trajectory.searchStartStop()) { return 0;}
  }

  // Track associate with clusters and holes
  //
  event_data.m_trajectory.buildTrajectoryForTRTSeed(m_usedriftrad);

  event_data.m_maxslope = m_maxslope;

  // Trajectory quality test
  //
  int nc = event_data.m_trajectory.nclusters();
  int nh = event_data.m_trajectory.nholes   ();
  if( nc < nCut  || (1000*nc) < (700*(nc+nh)) ) return 0;

  if     (m_segmentFindMode==0) {
    if(!event_data.m_trajectory.trackParametersEstimationForPerigeeWithVertexConstraint())
      return 0;
  }
  else if(m_segmentFindMode==1) {
    if(!event_data.m_trajectory.trackParametersEstimationForFirstPointWithVertexConstraint())
      return 0;
  }
  else if(m_segmentFindMode==2) {
    if(!event_data.m_trajectory.trackParametersEstimationForFirstPoint())
      return 0;
  }
  else                          {
    if(!event_data.m_trajectory.fitter())
      return 0;
  }

  if(event_data.m_trajectory.nclusters() < nCut) return 0;

  // Trk::TrackSegment production
  //
  return event_data.m_trajectory.convert();
}

///////////////////////////////////////////////////////////////////
// Test possiblity extend track to TRT for pixles+sct tracks
///////////////////////////////////////////////////////////////////

bool InDet::TRT_TrackExtensionTool_xk::isGoodExtension(const EventContext& ctx,
                                                       const Trk::TrackParameters& par,
                                                       InDet::TRT_TrackExtensionTool_xk::EventData &event_data) const
{

  // Get AtlasFieldCache
  MagField::AtlasFieldCache fieldCache;

  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
  if (fieldCondObj == nullptr) {
      ATH_MSG_ERROR("InDet::TRT_TrackExtensionTool_xk::findSegment: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
      return 0;
  }
  fieldCondObj->getInitializedCache (fieldCache);


  // TRT detector elements road builder
  //
  std::vector<const InDetDD::TRT_BaseElement*> DE;
  m_roadtool->detElementsRoad(ctx, fieldCache, par,Trk::alongMomentum,DE);
  if(int(DE.size()) < m_minNumberDCs) return false;

  // Array pointers to surface preparation
  //
  std::list<const Trk::Surface*> surfaces;
  int roadsize = 0;
  std::vector<const InDetDD::TRT_BaseElement*>::iterator d,de=DE.end();
  for(d=DE.begin(); d!=de; ++d) {
    surfaces.push_back(&(*d)->surface()); if(++roadsize==399) break;
  }

  // Global position on surfaces production
  //
  Trk::PatternTrackParameters Tp; if(!Tp.production(&par)) return false;
  std::list< std::pair<Amg::Vector3D,double> > gpos;
  m_proptool->globalPositions(ctx, Tp,surfaces,gpos,m_fieldprop);

  // Initiate trajectory
  //

  event_data.m_trajectory.initiateForPrecisionSeed(gpos,DE,event_data.m_trtcontainer,Tp);
  if(event_data.m_trajectory.naElements() < m_minNumberDCs) return false;

  // Track finding
  //
  if(m_usedriftrad)  event_data.m_trajectory.trackFindingWithDriftTime   (event_data.m_maxslope);
  else               event_data.m_trajectory.trackFindingWithoutDriftTime(event_data.m_maxslope);

  // Track associate with clusters and holes
  //
  event_data.m_trajectory.buildTrajectoryForPrecisionSeed(m_usedriftrad);

  // Final test quality
  //
  if( event_data.m_trajectory.nclusters() < m_minNumberDCs) return false;
  return true;
}

///////////////////////////////////////////////////////////////////
// Methods for track extension to TRT for pixles+sct tracks
// and new track production
///////////////////////////////////////////////////////////////////

Trk::Track* InDet::TRT_TrackExtensionTool_xk::newTrack(const EventContext& ctx,
                                                       const Trk::Track& Tr,
                                                       InDet::ITRT_TrackExtensionTool::IEventData &virt_event_data) const
{
  InDet::TRT_TrackExtensionTool_xk::EventData &
     event_data=InDet::TRT_TrackExtensionTool_xk::EventData::getPrivateEventData(virt_event_data);

  const DataVector<const Trk::TrackStateOnSurface>*
    tsos = Tr.trackStateOnSurfaces();

  // Test conditions to start track extension to TRT
  //
  const Trk::TrackParameters*
    pe  = (*(tsos->rbegin()))->trackParameters(); if(!pe) return 0; if(!pe->covariance()) return 0;
  const Trk::TrackParameters*
    pb  = (*(tsos->begin ()))->trackParameters(); if(!pb) return 0; if(!pb->covariance()) return 0;

  // Number PIX and SCT clusters cuts
  //
  if(!numberPIXandSCTclustersCut(Tr)) return 0;

  // Test possibility extend track and new track production
  //
  if(isGoodExtension(ctx, *pe,event_data)) return event_data.m_trajectory.convert(Tr);
  return 0;
}

///////////////////////////////////////////////////////////////////
// Number of SCT clusters test for start extension
///////////////////////////////////////////////////////////////////

bool InDet::TRT_TrackExtensionTool_xk::numberPIXandSCTclustersCut(const Trk::Track& Tr) const
{
  if(m_minNumberSCT <=0 && m_minNumberPIX <=0) return true;

  const DataVector<const Trk::TrackStateOnSurface>*
    tsos = Tr.trackStateOnSurfaces();

  DataVector<const Trk::TrackStateOnSurface>::const_iterator
    s = tsos->begin(), se = tsos->end();

  int npix = 0;
  int nsct = 0;
  for(; s!=se; ++s) {

    if((*s)->type(Trk::TrackStateOnSurface::Measurement)) {
      const Trk::MeasurementBase* mb = (*s)->measurementOnTrack();
      if(!mb) continue;
      const Trk::RIO_OnTrack*     ri = dynamic_cast<const Trk::RIO_OnTrack*>(mb);
      if(!ri) continue;
      const Trk::PrepRawData*     rd = ri->prepRawData();
      if(!rd) continue;
      const InDet::SiCluster*     si = dynamic_cast<const InDet::SiCluster*>(rd);
      if(si) {
	if(dynamic_cast<const InDet::SCT_Cluster*>(si)) ++nsct;
	else                                            ++npix;
      }
      else return false;
    }
  }
  if(npix >= m_minNumberPIX && nsct >= m_minNumberSCT) return true;
  return false;
}
