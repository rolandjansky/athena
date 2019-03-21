/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::TRT_TrackExtensionTool_xk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include <list>
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "InDetRecToolInterfaces/ITRT_DetElementsRoadMaker.h" 
#include "TRT_TrackExtensionTool_xk/TRT_TrackExtensionTool_xk.h"
#include "InDetRecToolInterfaces/ITrtDriftCircleCutTool.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetPrepRawData/SCT_Cluster.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_TrackExtensionTool_xk::TRT_TrackExtensionTool_xk
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
    m_fieldServiceHandle("AtlasFieldSvc",n), 
    m_roadtool  ("InDet::TRT_DetElementsRoadMaker_xk"                                                ),
    m_proptool    ("Trk::RungeKuttaPropagator"                                                       ),
    m_updatortool ("Trk::KalmanUpdator_xk"                                                           ),
    m_selectortool("InDet::InDetTrtDriftCircleCutTool"                                               ),
    m_riontrackD("InDet::TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackTool"                      ),
    m_riontrackN("InDet::TRT_DriftCircleOnTrackNoDriftTimeTool/TRT_DriftCircleOnTrackNoDriftTimeTool")
{
  m_fieldmode       = "MapSolenoid"      ;
  m_trtname         = "TRT_DriftCircles" ;
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
  m_measurement.reserve(200)             ;

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
  declareProperty("TRT_ClustersContainer"  ,m_trtname         ); 
  declareProperty("MagneticFieldMode"      ,m_fieldmode       );
  declareProperty("MinNumberSCTclusters"   ,m_minNumberSCT    );
  declareProperty("MinNumberPIXclusters"   ,m_minNumberPIX    );
  declareProperty("MagFieldSvc"        , m_fieldServiceHandle);
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
    msg(MSG::FATAL)<<"Toll service not found !"<<endreq;
    return StatusCode::FAILURE;
  }

  // Get magnetic field service
  //
  if(m_fieldmode != "NoField" ) {
    
    if( !m_fieldServiceHandle.retrieve() ){
      ATH_MSG_FATAL("Failed to retrieve " << m_fieldServiceHandle );
      return StatusCode::FAILURE;
    }    
    ATH_MSG_DEBUG("Retrieved " << m_fieldServiceHandle );
    m_fieldService = &*m_fieldServiceHandle;
  }

  // Get RIO_OnTrack creator with drift time information
  //
  if(m_riontrackD.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_riontrackD <<endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_riontrackD << endreq;
  }

  // Get RIO_OnTrack creator without drift time information
  //
  if(m_riontrackN.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_riontrackN <<endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_riontrackN << endreq;
  }

  // Get detector elements road maker tool
  //
  if(m_roadtool.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_roadtool <<endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_roadtool << endreq;
  }

  // Get propagator tool
  //
  if(m_proptool.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_proptool <<endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_proptool << endreq;
  }

  // Get updator tool
  //
  if(m_updatortool.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_updatortool <<endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_updatortool << endreq;
  }

  // Get segment selector tool
  //
  if(m_selectortool.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_selectortool <<endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_selectortool << endreq;
  }

  // Get  TRT Detector Manager
  /*
  const InDetDD::TRT_DetectorManager* trtmanager;
  sc = detStore()->retrieve(trtmanager,m_trtmanager);
  if (sc.isFailure()) {
    msg(MSG::FATAL)<<"Could not get TRT_DetectorManager"<<endreq; return sc;
  }
  */

  const TRT_ID * trtid;
  // TRT
  if (detStore()->retrieve(trtid,"TRT_ID").isFailure()) {
    msg(MSG::FATAL) << "Could not get TRT ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  m_trajectory.set
    (trtid,&(*m_proptool),&(*m_updatortool),(&(*m_riontrackD)),(&(*m_riontrackN)),
     m_roadwidth,m_zVertexWidth,m_impact,m_scale_error);

  // Setup callback for magnetic field
  //
  std::string folder( "/EXT/DCS/MAGNETS/SENSORDATA" );
  if (detStore()->contains<CondAttrListCollection>(folder)){
    const DataHandle<CondAttrListCollection> currentHandle;
    sc = detStore()->regFcn(&InDet::TRT_TrackExtensionTool_xk::magneticFieldInit,this,currentHandle,folder);
    
    if(sc==StatusCode::SUCCESS) {
      msg(MSG::INFO) << "Registered callback from MagneticFieldSvc for " << name() << endreq;
    } else {
      msg(MSG::ERROR) << "Could not book callback from MagneticFieldSvc for " << name () << endreq;
      return StatusCode::FAILURE;
    }
  } else {
    magneticFieldInit();
    ATH_MSG_INFO("Folder " << folder << " not present, magnetic field callback not set up. Not a problem if AtlasFieldSvc.useDCS=False");
  }


  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
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
  if(m_nprint)
    return dumpEvent(out);
  else
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
  n     = 64-m_trtname.size();
  std::string s7; for(int i=0; i<n; ++i) s7.append(" "); s7.append("|");
  n     = 64-m_riontrackD.type().size();
  std::string s8; for(int i=0; i<n; ++i) s8.append(" "); s8.append("|");
  n     = 64-m_riontrackN.type().size();
  std::string s9; for(int i=0; i<n; ++i) s9.append(" "); s9.append("|");

  out<<"|----------------------------------------------------------------------"
     <<"---------------------|"
     <<std::endl;
  out<<"| TRT container           | "<<m_trtname           <<s7 <<std::endl;
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

void InDet::TRT_TrackExtensionTool_xk::newEvent()
{
  m_trtcontainer = 0;
  StatusCode sc = evtStore()->retrieve(m_trtcontainer,m_trtname); 
  if(sc.isFailure() && m_outputlevel<=0) {
    msg(MSG::DEBUG)<<"Could not get TRT_DriftCircleContainer"<<endreq;
  }
}

///////////////////////////////////////////////////////////////////
// Main methods for track extension to TRT
///////////////////////////////////////////////////////////////////

std::vector<const Trk::MeasurementBase*>& 
InDet::TRT_TrackExtensionTool_xk::extendTrack(const Trk::Track& Tr)
{ 
  m_measurement.clear();

  if(!m_trtcontainer) return m_measurement;

  const DataVector<const Trk::TrackStateOnSurface>* 
    tsos = Tr.trackStateOnSurfaces();

  const Trk::TrackParameters* 
    par = (*(tsos->rbegin()))->trackParameters(); if(!par ) return m_measurement;
  const Trk::TrackParameters* 
    parb = (*(tsos->begin()))->trackParameters();

  
  if(parb && par!=parb) {

    const Amg::Vector3D& g1 = par ->position();
    const Amg::Vector3D& g2 = parb->position();
    if((g2.x()*g2.x()+g2.y()*g2.y()) > (g1.x()*g1.x()+g1.y()*g1.y())) par=parb;
  }
  return extendTrack(*par);
}

///////////////////////////////////////////////////////////////////
// Main methods for track extension to TRT for pixles+sct tracks
///////////////////////////////////////////////////////////////////

std::vector<const Trk::MeasurementBase*>& 
InDet::TRT_TrackExtensionTool_xk::extendTrack(const Trk::TrackParameters& par)
{ 
  m_measurement.clear();
  if(isGoodExtension(par)) m_trajectory.convert(m_measurement);
  return m_measurement;
}

///////////////////////////////////////////////////////////////////
// Main methods for segment finding in TRT for TRT seeds
///////////////////////////////////////////////////////////////////

Trk::TrackSegment* 
InDet::TRT_TrackExtensionTool_xk::findSegment(const Trk::TrackParameters& par)
{
  int nCut = m_minNumberDCs;
  if(m_parameterization) {nCut = m_selectortool->minNumberDCs(&par); if(nCut<m_minNumberDCs) nCut=m_minNumberDCs;}

  // TRT detector elements road builder
  //
  std::list<const InDetDD::TRT_BaseElement*> DE;
  m_roadtool->detElementsRoad(par,Trk::alongMomentum,DE);

  if(int(DE.size())< nCut) return 0;
  
  // Array pointers to surface preparation
  //
  std::list<const Trk::Surface*> surfaces;
  int roadsize = 0;
  std::list<const InDetDD::TRT_BaseElement*>::iterator d,de=DE.end();
  for(d=DE.begin(); d!=de; ++d) {
    surfaces.push_back(&(*d)->surface()); if(++roadsize==399) break;
  }

  // Global position on surfaces production
  // 
  Trk::PatternTrackParameters Tp; if(!Tp.production(&par)) return 0;
  std::list< std::pair<Amg::Vector3D,double> > gpos;
  m_proptool->globalPositions(Tp,surfaces,gpos,m_fieldprop);

  // Initiate trajectory
  //
  m_trajectory.initiateForTRTSeed(gpos,DE,m_trtcontainer,Tp);
  
  if(m_trajectory.naElements() < nCut) return 0;
  double maxslope = m_maxslope;
  m_maxslope      = .0002     ; 

  if(m_trajectory.isFirstElementBarrel()) {

    if(m_usedriftrad)  m_trajectory.trackFindingWithDriftTime   (m_maxslope);
    else               m_trajectory.trackFindingWithoutDriftTime(m_maxslope);
    if(!m_trajectory.searchStartStop()) {m_maxslope = maxslope; return 0;}
  }
  else   {

    m_trajectory.trackFindingWithoutDriftTime(m_maxslope);
    if(!m_trajectory.searchStartStop()) { m_maxslope = maxslope; return 0;}
    m_trajectory.radiusCorrection();
    if(m_usedriftrad) m_trajectory.trackFindingWithDriftTimeBL   (m_maxslope);
    else              m_trajectory.trackFindingWithoutDriftTimeBL(m_maxslope);
    if(!m_trajectory.searchStartStop()) { m_maxslope = maxslope; return 0;}
  }

  // Track associate with clusters and holes
  //
  m_trajectory.buildTrajectoryForTRTSeed(m_usedriftrad);

  m_maxslope = maxslope; 

  // Trajectory quality test
  // 
  int nc = m_trajectory.nclusters();
  int nh = m_trajectory.nholes   ();
  if( nc < nCut  || (1000*nc) < (700*(nc+nh)) ) return 0;

  if     (m_segmentFindMode==0) {
    if(!m_trajectory.trackParametersEstimationForPerigeeWithVertexConstraint()) 
      return 0;
  }
  else if(m_segmentFindMode==1) {
    if(!m_trajectory.trackParametersEstimationForFirstPointWithVertexConstraint()) 
      return 0;
  }
  else if(m_segmentFindMode==2) {
    if(!m_trajectory.trackParametersEstimationForFirstPoint()) 
      return 0;
  }
  else                          {
    if(!m_trajectory.fitter()) 
      return 0;
  }

  if(m_trajectory.nclusters() < nCut) return 0;

  // Trk::TrackSegment production
  //
  return m_trajectory.convert();
}

///////////////////////////////////////////////////////////////////
// Callback function - get the magnetic field /
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackExtensionTool_xk::magneticFieldInit(IOVSVC_CALLBACK_ARGS) 
{
  // Build MagneticFieldProperties 
  //
  if(!m_fieldService->solenoidOn()) m_fieldmode ="NoField"; magneticFieldInit();
  return StatusCode::SUCCESS;
}

void InDet::TRT_TrackExtensionTool_xk::magneticFieldInit() 
{
  // Build MagneticFieldProperties 
  //

  Trk::MagneticFieldProperties* pMF = 0;
  if     (m_fieldmode == "NoField"    ) pMF = new Trk::MagneticFieldProperties(Trk::NoField  );
  else if(m_fieldmode == "MapSolenoid") pMF = new Trk::MagneticFieldProperties(Trk::FastField);
  else                                  pMF = new Trk::MagneticFieldProperties(Trk::FullField);
  m_fieldprop = *pMF; delete pMF;
  m_trajectory.set(m_fieldprop,m_fieldService);
}

///////////////////////////////////////////////////////////////////
// Test possiblity extend track to TRT for pixles+sct tracks
///////////////////////////////////////////////////////////////////

bool InDet::TRT_TrackExtensionTool_xk::isGoodExtension(const Trk::TrackParameters& par)
{ 
  // TRT detector elements road builder
  //
  std::list<const InDetDD::TRT_BaseElement*> DE;
  m_roadtool->detElementsRoad(par,Trk::alongMomentum,DE);
  if(int(DE.size()) < m_minNumberDCs) return false;
  
  // Array pointers to surface preparation
  //
  std::list<const Trk::Surface*> surfaces;
  int roadsize = 0;
  std::list<const InDetDD::TRT_BaseElement*>::iterator d,de=DE.end();
  for(d=DE.begin(); d!=de; ++d) {
    surfaces.push_back(&(*d)->surface()); if(++roadsize==399) break;
  }

  // Global position on surfaces production
  // 
  Trk::PatternTrackParameters Tp; if(!Tp.production(&par)) return false;
  std::list< std::pair<Amg::Vector3D,double> > gpos;
  m_proptool->globalPositions(Tp,surfaces,gpos,m_fieldprop);

  // Initiate trajectory
  //
  m_trajectory.initiateForPrecisionSeed(gpos,DE,m_trtcontainer,Tp);
  if(m_trajectory.naElements() < m_minNumberDCs) return false;

  // Track finding
  //
  if(m_usedriftrad)  m_trajectory.trackFindingWithDriftTime   (m_maxslope);
  else               m_trajectory.trackFindingWithoutDriftTime(m_maxslope);

  // Track associate with clusters and holes
  //
  m_trajectory.buildTrajectoryForPrecisionSeed(m_usedriftrad);

  // Final test quality
  //
  if( m_trajectory.nclusters() < m_minNumberDCs) return false;
  return true;
}

///////////////////////////////////////////////////////////////////
// Methods for track extension to TRT for pixles+sct tracks 
// and new track production
///////////////////////////////////////////////////////////////////

Trk::Track* InDet::TRT_TrackExtensionTool_xk::newTrack(const Trk::Track& Tr)
{ 
  if(!m_trtcontainer) return 0;

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
  if(isGoodExtension(*pe)) return m_trajectory.convert(Tr);
  return 0;
}

///////////////////////////////////////////////////////////////////
// Number of SCT clusters test for start extension
///////////////////////////////////////////////////////////////////

bool InDet::TRT_TrackExtensionTool_xk::numberPIXandSCTclustersCut(const Trk::Track& Tr)
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
