/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class TRT_TrackSegmentsMaker_ATLxk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for TRT_DriftCircleOnTrack object production
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include "TRT_TrackSegmentsTool_xk/TRT_TrackSegmentsMaker_ATLxk.h"

#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "GeoModelInterfaces/IGeoModelTool.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "TRT_ReadoutGeometry/TRT_Numerology.h"
#include "InDetRecToolInterfaces/ITRT_TrackExtensionTool.h"
#include "TrkExInterfaces/IPropagator.h"
#include "StoreGate/ReadHandle.h"
///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_TrackSegmentsMaker_ATLxk::TRT_TrackSegmentsMaker_ATLxk
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
  m_propTool     ("Trk::RungeKuttaPropagator"                  )
{
  m_fieldmode   =      "MapSolenoid" ;
  m_pTmin       =                500.;
  m_sharedfrac  =                0.3 ;
  m_nPhi        =                500 ;
  m_nMom        =                 70 ;
  m_clustersCut =                 10 ;
  m_removeNoise =                true;
  m_build       =               false;
  m_gupdate     =               false;

  declareInterface<ITRT_TrackSegmentsMaker>(this);

  declareProperty("PropagatorTool"         ,m_propTool     );
  declareProperty("MagneticFieldMode"      ,m_fieldmode    );
  declareProperty("NumberAzimuthalChannel" ,m_nPhi         ); 
  declareProperty("NumberMomentumChannel"  ,m_nMom         ); 
  declareProperty("MinNumberDriftCircles"  ,m_clustersCut  );
  declareProperty("RemoveNoiseDriftCircles",m_removeNoise  );
  declareProperty("pTmin"                  ,m_pTmin        );
  declareProperty("sharedFrac"             ,m_sharedfrac   );

}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////
InDet::TRT_TrackSegmentsMaker_ATLxk::~TRT_TrackSegmentsMaker_ATLxk()
{ 
}



///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackSegmentsMaker_ATLxk::initialize()
{
  StatusCode sc = AlgTool::initialize(); 

  // Get tTools servise
  //
  IToolSvc* toolSvc;
  if ((sc=service("ToolSvc", toolSvc)).isFailure())  {
    msg(MSG::FATAL)<<"Toll service not found !"<<endmsg; return sc;
  }

  // Initiate magnetic field properties
  //
  magneticFieldInit();

  // Initialize ReadHandle
  //
  ATH_CHECK(m_trtname.initialize());
  

  // Get propagator tool
  //
  if( m_propTool.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_propTool <<endmsg;
    return StatusCode::FAILURE;
  } else {
     msg(MSG::INFO) << "Retrieved tool " << m_propTool << endmsg;
  }

  // Get tool for track extension to TRT
  //
  if( m_extensionTool.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_extensionTool <<endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_extensionTool << endmsg;
  }

  // PRD-to-track association (optional)
  ATH_CHECK( m_prdToTrackMap.initialize( !m_prdToTrackMap.key().empty()));
  ATH_CHECK( m_condDataKey.initialize());

  // TRT
  if (detStore()->retrieve(m_trtid, "TRT_ID").isFailure()) {
    msg(MSG::FATAL) << "Could not get TRT ID helper" << endmsg;
    return StatusCode::FAILURE;
  }

  //Define set of private variables
  m_Ts          = m_nPhi*m_nMom                       ;
  m_Psi         = 2./float(m_nMom-1)                  ;
  m_Psi128      = m_Psi*128.                          ;
  m_Ns128       = m_nPhi*128                          ;
  m_A           = float(m_nPhi)/(2.*M_PI)             ;
  m_Ai          = 1./m_A                              ;
  m_histsize    = (m_nPhi*m_nMom*26/4)                ;
  
  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackSegmentsMaker_ATLxk::finalize()
{
   StatusCode sc = AlgTool::finalize(); return sc;
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new event 
///////////////////////////////////////////////////////////////////

std::unique_ptr<InDet::ITRT_TrackSegmentsMaker::IEventData>
InDet::TRT_TrackSegmentsMaker_ATLxk::newEvent(const EventContext &ctx) const
{

  const float pi2 = 2.*M_PI;

  const TRT_TrackSegmentsToolCondData_xk &condData = *getConditionsData();

  // Get drift circles collection
  //
  SG::ReadHandle<InDet::TRT_DriftCircleContainer> trtcontainer(m_trtname, ctx);
  if(not trtcontainer.isValid()) {
    std::stringstream msg;
    msg << name() << " Missing TRT_DriftCircleContainer " << m_trtname.key();
    throw std::runtime_error(msg.str());
  }

  SG::ReadHandle<Trk::PRDtoTrackMap>  prd_to_track_map;
  const Trk::PRDtoTrackMap *prd_to_track_map_cptr = nullptr;
  if (!m_prdToTrackMap.key().empty()) {
    prd_to_track_map=SG::ReadHandle<Trk::PRDtoTrackMap>(m_prdToTrackMap, ctx);
    if (!prd_to_track_map.isValid()) {
      ATH_MSG_ERROR("Failed to read PRD to track association map: " << m_prdToTrackMap.key());
    }
    prd_to_track_map_cptr = prd_to_track_map.cptr();
  }


  std::unique_ptr<TRT_TrackSegmentsMaker_ATLxk::EventData>
     event_data = std::make_unique<TRT_TrackSegmentsMaker_ATLxk::EventData>(trtcontainer.cptr(), condData.m_cirsize);

  // Initiate extension tool
  //
  event_data->m_extEventData = m_extensionTool->newEvent(ctx);

  InDet::TRT_DriftCircleContainer::const_iterator
    w = trtcontainer->begin(),we = trtcontainer->end();
  int    n = 0;
  if(w!=we) {

  eraseHistogramm(*event_data);

  for(; w!=we; ++w) {

    if(n >= condData.m_cirsize) break;

    Identifier ID = (*w)->identify();
    int be = m_trtid->barrel_ec     (ID);
    int lw = m_trtid->layer_or_wheel(ID);
    int sl = m_trtid->straw_layer   (ID);
    
    int b; be < 0 ?  b = be+2 : b = be+1;
    int l  = condData.m_flayers[b][lw]+sl;
    unsigned int sb  = condData.m_begin[b][l];
    unsigned int se  = condData.m_end  [b][l];
    unsigned int ad  = 1000*b+l;

    // Loop through all clusters from given detector element
    //
    InDet::TRT_DriftCircleCollection::const_iterator 
      c  = (*w)->begin(), ce = (*w)->end();
    for(; c!=ce; ++c) {

      if(prd_to_track_map_cptr &&  prd_to_track_map_cptr->isUsed(*(*c))) continue;
      if(m_removeNoise && (*c)->isNoise()           ) continue;

      if(n >= condData.m_cirsize) break;

      int               ns = m_trtid->straw((*c)->identify());
      const Amg::Vector3D& sc = (*c)->detectorElement()->strawCenter(ns);
      float             Fs = atan2(sc.y(),sc.x()); if(Fs<0.) Fs+=pi2;
      event_data->m_circles[n].set((*c),Fs,ad);
      
      // Loop through all dz/dr for given cluster 
      //
      for(unsigned int s=sb; s<=se; ++s) fillHistogramm (Fs,s,*event_data);
      ++n;
    }
  }
  }
  event_data->m_clusters = n;
  return std::unique_ptr<InDet::ITRT_TrackSegmentsMaker::IEventData>(event_data.release());
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

std::unique_ptr<InDet::ITRT_TrackSegmentsMaker::IEventData>
InDet::TRT_TrackSegmentsMaker_ATLxk::newRegion
(const EventContext& ctx, const std::vector<IdentifierHash>& vTRT) const
{
  const float pi2 = 2.*M_PI;

  const TRT_TrackSegmentsToolCondData_xk &condData = *getConditionsData();

  // Get drift cilrcles collection
  //
  SG::ReadHandle<InDet::TRT_DriftCircleContainer> trtcontainer(m_trtname, ctx);
  if(not trtcontainer.isValid() && msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG)<<"Could not get TRT_DriftCircleContainer"<<endmsg;
  }

  SG::ReadHandle<Trk::PRDtoTrackMap>  prd_to_track_map;
  const Trk::PRDtoTrackMap *prd_to_track_map_cptr = nullptr;
  if (!m_prdToTrackMap.key().empty()) {
    prd_to_track_map=SG::ReadHandle<Trk::PRDtoTrackMap>(m_prdToTrackMap, ctx);
    if (!prd_to_track_map.isValid()) {
      ATH_MSG_ERROR("Failed to read PRD to track association map: " << m_prdToTrackMap.key());
    }
    prd_to_track_map_cptr = prd_to_track_map.cptr();
  }

  std::unique_ptr<TRT_TrackSegmentsMaker_ATLxk::EventData>
     event_data = std::make_unique<TRT_TrackSegmentsMaker_ATLxk::EventData>(trtcontainer.cptr(), condData.m_cirsize);

  if(trtcontainer.isValid()) {
  // Initiate extension tool
  //
  event_data->m_extEventData = m_extensionTool->newEvent(ctx);

  eraseHistogramm(*event_data);

  std::vector<IdentifierHash>::const_iterator d=vTRT.begin(),de=vTRT.end();
  int n = 0;
  for(; d!=de; ++d) {

    auto w = trtcontainer->indexFindPtr((*d));

    if(w!=nullptr) {

      Identifier ID = w->identify();
      int be = m_trtid->barrel_ec     (ID);
      int lw = m_trtid->layer_or_wheel(ID);
      int sl = m_trtid->straw_layer   (ID);
      
      int b; be < 0 ?  b = be+2 : b = be+1;
      int l  = condData.m_flayers[b][lw]+sl;
      unsigned int sb  = condData.m_begin[b][l];
      unsigned int se  = condData.m_end  [b][l];
      unsigned int ad  = 1000*b+l;

      InDet::TRT_DriftCircleCollection::const_iterator 
	c  = w->begin(), ce = w->end();

      for(; c!=ce; ++c) {

	if(prd_to_track_map_cptr &&  prd_to_track_map_cptr->isUsed(*(*c))) continue;
	if(m_removeNoise && (*c)->isNoise()           ) continue;
	
	if(n >= condData.m_cirsize) break;
	
	int               ns = m_trtid->straw((*c)->identify());
	const Amg::Vector3D& sc = (*c)->detectorElement()->strawCenter(ns);
	float             Fs = atan2(sc.y(),sc.x()); if(Fs<0.) Fs+=pi2;
	event_data->m_circles[n].set((*c),Fs,ad);
	
	// Loop through all dz/dr for given cluster 
	//
	for(unsigned int s=sb; s<=se; ++s) fillHistogramm (Fs,s,*event_data);
	++n;
      }
    }
  }
  event_data->m_clusters = n;
  }
  return std::unique_ptr<InDet::ITRT_TrackSegmentsMaker::IEventData>( event_data.release() );
}

///////////////////////////////////////////////////////////////////
// Inform tool about end of event or region investigation
///////////////////////////////////////////////////////////////////

void InDet::TRT_TrackSegmentsMaker_ATLxk::endEvent (InDet::ITRT_TrackSegmentsMaker::IEventData &virt_event_data) const
{
   if (msgLvl(MSG::DEBUG)) {
      TRT_TrackSegmentsMaker_ATLxk::EventData &
         event_data = TRT_TrackSegmentsMaker_ATLxk::EventData::getPrivateEventData(virt_event_data);
      dumpEvent(msg(MSG::DEBUG),event_data);
      dumpConditions(msg(MSG::DEBUG));
      msg(MSG::DEBUG) << endmsg;
  }
}

///////////////////////////////////////////////////////////////////
// Methods for seeds production without vertex constraint
///////////////////////////////////////////////////////////////////
void InDet::TRT_TrackSegmentsMaker_ATLxk::find(const EventContext &ctx,
                                               InDet::ITRT_TrackSegmentsMaker::IEventData &virt_event_data) const
{
   TRT_TrackSegmentsMaker_ATLxk::EventData &
      event_data = TRT_TrackSegmentsMaker_ATLxk::EventData::getPrivateEventData(virt_event_data);

  event_data.m_sizebin_iterator = event_data.m_sizebin.rbegin();

  if(event_data.m_clusters<m_clustersCut) return;

  const TRT_TrackSegmentsToolCondData_xk &condData = *getConditionsData();

  event_data.m_clusterSegment.clear();
  event_data.m_qualitySegment.clear();

  unsigned int mc = event_data.m_clusters;

  for(unsigned int n=0; n!=mc; ++n) {
    
    unsigned int b  = event_data.m_circles[n].buffer();
    unsigned int l  = event_data.m_circles[n].layer ();
    unsigned int sb = condData.m_begin[b][l];
    unsigned int se = condData.m_end  [b][l];
    
    // Loop through all dz/dr for given cluster 
    //
    unsigned char max    = 0;
    unsigned int  maxbin = 0;

    for(unsigned int s=sb; s<=se; ++s) {
      analyseHistogramm(max,maxbin,event_data.m_circles[n].phi(),s,event_data);
    }

    if(int(max) > m_clustersCut) {
      event_data.m_bincluster.insert(std::make_pair(localMaximum(maxbin,event_data),n));
    }
  }
  
  std::multimap<unsigned int,unsigned int>::iterator
    bc,bce =event_data.m_bincluster.end();

  unsigned int nbins = 0       ;
  unsigned int fbin  = 99999999; 
  for(bc = event_data.m_bincluster.begin(); bc!=bce; ++bc) {

    if((*bc).first==fbin) ++nbins;
    else     {
      if(fbin!=99999999 && nbins>=5) event_data.m_sizebin.insert(std::make_pair(nbins,fbin));
      fbin=(*bc).first; nbins = 1;
    }
  }
  if(fbin!=99999999 && nbins>=5) event_data.m_sizebin.insert(std::make_pair(nbins,fbin));
  event_data.m_sizebin_iterator = event_data.m_sizebin.rbegin();

  // @TODO add to event data
  SG::ReadHandle<Trk::PRDtoTrackMap>  prd_to_track_map;
  const Trk::PRDtoTrackMap  *prd_to_track_map_cptr = nullptr;
  if (!m_prdToTrackMap.key().empty()) {
    prd_to_track_map=SG::ReadHandle<Trk::PRDtoTrackMap>(m_prdToTrackMap, ctx);
    if (!prd_to_track_map.isValid()) {
      ATH_MSG_ERROR("Failed to read PRD to track association map: " << m_prdToTrackMap.key());
    }
    prd_to_track_map_cptr = prd_to_track_map.cptr();
  }

  // Local reconstruction and track segments production
  //
  while(event_data.m_sizebin_iterator!=event_data.m_sizebin.rend()) {

    unsigned int bin =(*event_data.m_sizebin_iterator++).second;
    findLocaly(ctx, bin,prd_to_track_map_cptr, event_data);
  }

  // Final segments preparation
  //
  segmentsPreparation(event_data);

  event_data.m_segiterator = event_data.m_segments.begin();
}

///////////////////////////////////////////////////////////////////
// Pseudo iterator
///////////////////////////////////////////////////////////////////

Trk::TrackSegment* InDet::TRT_TrackSegmentsMaker_ATLxk::next(InDet::ITRT_TrackSegmentsMaker::IEventData &virt_event_data) const
{
   TRT_TrackSegmentsMaker_ATLxk::EventData &
      event_data = TRT_TrackSegmentsMaker_ATLxk::EventData::getPrivateEventData(virt_event_data);

  if(event_data.m_segiterator!=event_data.m_segments.end()) return (*event_data.m_segiterator++);
  return 0;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackSegmentsMaker_ATLxk::dump( MsgStream& out ) const
{
  out<<std::endl;
  return dumpConditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackSegmentsMaker_ATLxk::dumpConditions( MsgStream& out ) const
{

  const TRT_TrackSegmentsToolCondData_xk &condData = *getConditionsData();

  std::string fieldmode[9] ={"NoField"       ,"ConstantField","SolenoidalField",
			     "ToroidalField" ,"Grid3DField"  ,"RealisticField" ,
			     "UndefinedField","AthenaField"  , "?????"         };

  int mode = m_fieldprop.magneticFieldMode(); 
  if(mode<0 || mode>8 ) mode = 8; 

  int n  = 62-fieldmode[mode].size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");

  n     = 62-m_trtname.key().size();
  std::string s4; for(int i=0; i<n; ++i) s4.append(" "); s4.append("|");

  n     = 62-m_propTool.type().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");

  n     = 62-m_extensionTool.type().size();
  std::string s7; for(int i=0; i<n; ++i) s7.append(" "); s7.append("|");


  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
       <<std::endl;
  out<<"| Tool for propagation    | "<<m_propTool     .type()<<s1<<std::endl;
  out<<"| Tool tracks extension   | "<<m_extensionTool.type()<<s7<<std::endl;    
  out<<"| Magnetic field mode     | "<<fieldmode[mode]       <<s3<<std::endl;
  out<<"| TRT container           | "<<m_trtname.key().size()             <<s4<<std::endl;
  out<<"| Min. number straws      | "
     <<std::setw(12)<<m_clustersCut
     <<"                                                  |"<<std::endl;
  out<<"| Number neg. bar. layers | "
     <<std::setw(12)<<condData.m_nlayers[1]
     <<"                                                  |"<<std::endl;
  out<<"| Number pos. bar. layers | "
     <<std::setw(12)<<condData.m_nlayers[2]
     <<"                                                  |"<<std::endl;
  out<<"| Number neg. end. layers | "
     <<std::setw(12)<<condData.m_nlayers[0]
     <<"                                                  |"<<std::endl;
  out<<"| Number pos. end. layers | "
     <<std::setw(12)<<condData.m_nlayers[3]
     <<"                                                  |"<<std::endl;
  out<<"| Number neg. bar. straws | "
     <<std::setw(12)<<condData.m_nstraws[1]
     <<"                                                  |"<<std::endl;
  out<<"| Number pos. bar. straws | "
     <<std::setw(12)<<condData.m_nstraws[2]
     <<"                                                  |"<<std::endl;
  out<<"| Number neg. end. straws | "
     <<std::setw(12)<<condData.m_nstraws[0]
     <<"                                                  |"<<std::endl;
  out<<"| Number pos. end. straws | "
     <<std::setw(12)<<condData.m_nstraws[3]
     <<"                                                  |"<<std::endl;
  out<<"| Number azimut. channels | "
     <<std::setw(12)<<m_nPhi
     <<"                                                  |"<<std::endl;
  out<<"| Number moment. channels | "
     <<std::setw(12)<<m_nMom
     <<"                                                  |"<<std::endl;
  out<<"| Number histog. channels | "
     <<std::setw(12)<<m_histsize
     <<"                                                  |"<<std::endl;
  out<<"| Number cluster links    | "
     <<std::setw(12)<<condData.m_cirsize
     <<"                                                  |"<<std::endl;
  out<<"| Use PRD-to-track assoc.?| "
     <<std::setw(12)<< (!m_prdToTrackMap.key().empty() ? "yes" : "no ")
     <<"                                                  |"<<std::endl;
  out<<"| Remove noise   ?        | "
     <<std::setw(12)<<m_removeNoise
     <<"                                                  |"<<std::endl;     
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackSegmentsMaker_ATLxk::dumpEvent( MsgStream& out,
                                                           InDet::ITRT_TrackSegmentsMaker::IEventData &virt_event_data) const
{
   TRT_TrackSegmentsMaker_ATLxk::EventData &
      event_data = TRT_TrackSegmentsMaker_ATLxk::EventData::getPrivateEventData(virt_event_data);
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;
  out<<"| Number drift circles    | "
     <<std::setw(12)<<event_data.m_clusters
     <<"                                                  |"<<std::endl;
  out<<"| Number local calls      | "
     <<std::setw(12)<<event_data.m_nlocal
     <<"                                                  |"<<std::endl;
  out<<"| Number found segments   | "
     <<std::setw(12)<<event_data.m_nsegments
     <<"                                                  |"<<std::endl;
  out<<"| Number save  segments   | "
     <<std::setw(12)<<event_data.m_segments.size()
     <<"                                                  |"<<std::endl;
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::TRT_TrackSegmentsMaker_ATLxk::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Erase histogramm
///////////////////////////////////////////////////////////////////

void InDet::TRT_TrackSegmentsMaker_ATLxk::eraseHistogramm(TRT_TrackSegmentsMaker_ATLxk::EventData &event_data) const
{
  for(int i=0; i!=m_histsize; i+=10) {

    event_data.m_U.H4[i  ]=0;

    event_data.m_U.H4[i+1]=0;
    event_data.m_U.H4[i+2]=0;
    event_data.m_U.H4[i+3]=0;
    event_data.m_U.H4[i+4]=0;
    event_data.m_U.H4[i+5]=0;
    event_data.m_U.H4[i+6]=0;
    event_data.m_U.H4[i+7]=0;
    event_data.m_U.H4[i+8]=0;
    event_data.m_U.H4[i+9]=0;
  }
}

///////////////////////////////////////////////////////////////////
// Fill histogramm
///////////////////////////////////////////////////////////////////

void InDet::TRT_TrackSegmentsMaker_ATLxk::fillHistogramm 
(float Fs,int s, TRT_TrackSegmentsMaker_ATLxk::EventData &event_data) const
{
  const TRT_TrackSegmentsToolCondData_xk &condData = *getConditionsData();

  int s0 = condData.m_ndzdr[s]*m_Ts;
  int f  = int((Fs*m_A-condData.m_slope[s])*128.); if(f<0) f+=m_Ns128;
  int sf = condData.m_islope[s];

  // Loop through all momentum slopes
  //
  for(int i=s0; i!=s0+m_Ts; i+=m_nPhi) {
    int k =(f>>7); f+=sf; k<m_nPhi ? ++event_data.m_U.H[k+i] : ++event_data.m_U.H[k+i-m_nPhi];
  }
}


///////////////////////////////////////////////////////////////////
// Analyse histogramm
///////////////////////////////////////////////////////////////////
 
void InDet::TRT_TrackSegmentsMaker_ATLxk::analyseHistogramm 
(unsigned char& max,unsigned int& maxbin,float Fs,int s, TRT_TrackSegmentsMaker_ATLxk::EventData &event_data) const
{
  const TRT_TrackSegmentsToolCondData_xk &condData = *getConditionsData();

  int s0     = condData.m_ndzdr[s]*m_Ts;
  int f      = int((Fs*m_A-condData.m_slope[s])*128.); if(f<0) f+=m_Ns128;
  int sf     = condData.m_islope[s];

  // Loop through all momentum slopes
  //
  for(int i=s0; i!=s0+m_Ts; i+=m_nPhi) {
    int k =(f>>7); f+=sf; if(k>=m_nPhi) k-=m_nPhi;
    if(event_data.m_U.H[k+i] > max) max = event_data.m_U.H[maxbin = k+i];
  }
}

///////////////////////////////////////////////////////////////////
// TRT seeds production
///////////////////////////////////////////////////////////////////

void InDet::TRT_TrackSegmentsMaker_ATLxk::findLocaly(const EventContext &ctx,
                                                     unsigned int bin,
                                                     const Trk::PRDtoTrackMap *prd_to_track_map,
                                                     TRT_TrackSegmentsMaker_ATLxk::EventData &event_data) const
{
  const TRT_TrackSegmentsToolCondData_xk &condData = *getConditionsData();

  const double pi=M_PI, pi2 = 2.*M_PI;
   
  std::multimap<const InDet::TRT_DriftCircle*,Trk::TrackSegment*>::const_iterator
    cse = event_data.m_clusterSegment.end();

  std::multimap<unsigned int,unsigned int>::iterator 
    bc                          ,
    bcb = event_data.m_bincluster.find(bin),
    bce = event_data.m_bincluster.end()    ;
  int nfree = 0;
  for(bc=bcb; bc!=bce; ++bc) {

    if((*bc).first!=bin) break;
    if(event_data.m_clusterSegment.find(event_data.m_circles[(*bc).second].circle())==cse) ++nfree;
  }
  if(nfree<5) return;
  unsigned int ndzdr = bin/m_Ts;
  unsigned int b     = bin-ndzdr*m_Ts;
  unsigned int m     = b/m_nPhi;
  float        c0    = (1.-float(m)*m_Psi)*m_Ai;
  
  bce=bc;
  float fm    = 0.   ;
  float Fo    = 0.   ;
  bool  first = false;
  for(bc=bcb; bc!=bce; ++bc) {

    unsigned int n = (*bc).second;
    
    unsigned int b  = event_data.m_circles[n].buffer();
    unsigned int l  = event_data.m_circles[n].layer ();
    unsigned int s  = condData.m_begin[b][l];
    unsigned int se = condData.m_end  [b][l];
    
    for(; s<= se; ++s) {if(condData.m_ndzdr[s]==ndzdr) break;}
    if(s>se) continue;
    float F  = event_data.m_circles[n].phi()-condData.m_slope[s]*c0;

    if(!first) {
      Fo = F; first = true;
    }
    else       {
      float df = F-Fo;
      if     (df > pi) df-=pi2; 
      else if(df <-pi) df+=pi2; 
      fm+=df;
    }
  }
  fm = Fo+fm/float(nfree);

  if     (fm > pi) fm = fmod(fm+pi,pi2)-pi;
  else if(fm <-pi) fm = fmod(fm-pi,pi2)+pi;

  double pT = m_pTmin/(double(m)*m_Psi-1.);

  double pin = 1./(pT*sqrt((1.+condData.m_dzdr[ndzdr]*condData.m_dzdr[ndzdr])));

  Amg::Vector3D PSV(0.,0.,0.); Trk::PerigeeSurface PS(PSV);
  const Trk::TrackParameters* Tp = PS.createTrackParameters(0.,0.,fm,atan2(1.,condData.m_dzdr[ndzdr]),pin,0);
    ++event_data.m_nlocal;

  Trk::TrackSegment* seg = m_extensionTool->findSegment(ctx, *Tp, *(event_data.m_extEventData) );
  delete Tp;
  if(!seg) return;

  // Momentum cut
  //
  double T  = seg->localParameters().get(Trk::theta );
  double iP = seg->localParameters().get(Trk::qOverP);

  // ME: let's not use a soft cut here
  if(sin(T) < 0.9*m_pTmin*fabs(iP)) {delete seg; return;}


  ++event_data.m_nsegments;

  bool isbarrel=false;
  const Trk::MeasurementBase *lastmeas=seg->containedMeasurements().back();
  if (std::abs(lastmeas->globalPosition().z())<750.) isbarrel=true;
  // Number drift circles test
  //
  unsigned int size = isbarrel ? seg->numberOfMeasurementBases()-1 : seg->numberOfMeasurementBases()-2;

  if(int(size) < m_clustersCut) {delete seg; return;}

  std::vector<const Trk::MeasurementBase*>::const_iterator
    s = seg->containedMeasurements().begin(), se = seg->containedMeasurements().end();

  if(prd_to_track_map) {
    
    // Test number unused drift circles 
    //
    int nu = 0, ntot = 0;
    for(++s; s!=se; ++s) {

      const Trk::RIO_OnTrack*  rio = dynamic_cast<const Trk::RIO_OnTrack*>(*s);
      if(rio) {
	++ntot;
	if (prd_to_track_map->isUsed(*rio->prepRawData())) ++nu;
      }
    }
    // ME: use fraction cut
    if( nu > int(ntot*m_sharedfrac))  {delete seg; return;}
  }

  // Save information about qiality and segment
  //
  int    quality = size;
  double Xi2     = seg->fitQuality()->chiSquared()/double(seg->fitQuality()->numberDoF());  

  if(Xi2 > .5) {
    if     (Xi2 > 6)  quality-=9;
    else if(Xi2 > 5.) quality-=7;
    else if(Xi2 > 3.) quality-=5;
    else if(Xi2 > 2.) quality-=3;
    else if(Xi2 > 1.) quality-=1;
  }
 
  event_data.m_qualitySegment.insert(std::make_pair(quality,seg));

  s = seg->containedMeasurements().begin();

  // Save information about TRT clusters and segments
  //
  for(++s; s!=se; ++s) {
    const Trk::RIO_OnTrack*  rio = dynamic_cast<const Trk::RIO_OnTrack*>(*s);
    if (!rio) continue;
    const InDet::TRT_DriftCircle* dc = ((const InDet::TRT_DriftCircleOnTrack*)(*s))->prepRawData();
    if(dc) event_data.m_clusterSegment.insert(std::make_pair(dc,seg));
  }
}

///////////////////////////////////////////////////////////////////
// Remove fake TRT segments
///////////////////////////////////////////////////////////////////

void InDet::TRT_TrackSegmentsMaker_ATLxk::segmentsPreparation(TRT_TrackSegmentsMaker_ATLxk::EventData &event_data) const
{
  std::multimap<int,Trk::TrackSegment*>::reverse_iterator 
    qs = event_data.m_qualitySegment.rbegin();

  for(; qs!=event_data.m_qualitySegment.rend(); ++qs) {

    int nfree = 0;

    std::vector<const Trk::MeasurementBase*>::const_iterator
      s  =(*qs).second->containedMeasurements().begin(), 
      se =(*qs).second->containedMeasurements().end  ();

    for(++s; s!=se; ++s) {
      const Trk::RIO_OnTrack*  rio = dynamic_cast<const Trk::RIO_OnTrack*>(*s);
    if (!rio) continue;

      const InDet::TRT_DriftCircle* dc = ((const InDet::TRT_DriftCircleOnTrack*)(*s))->prepRawData();
      if(dc && event_data.m_clusterSegment.erase(dc)) ++nfree;
    }
    if(nfree >= 7) event_data.m_segments.push_back((*qs).second);
    else           delete               (*qs).second;
  }
}

///////////////////////////////////////////////////////////////////
// Local maximum
///////////////////////////////////////////////////////////////////

unsigned int InDet::TRT_TrackSegmentsMaker_ATLxk::localMaximum
(unsigned int bin, TRT_TrackSegmentsMaker_ATLxk::EventData &event_data) const
{
  int           b    = bin-(bin/m_Ts)*m_Ts  ;
  int           m    = b/m_nPhi             ;
  int           f    = b-m*m_nPhi           ;
  unsigned int  maxb = bin                  ;
  unsigned char max  = event_data.m_U.H[bin];

  int a1 = bin-1; if(f==   0    ) a1+=m_nPhi; if(event_data.m_U.H[a1]>max) {max=event_data.m_U.H[a1]; maxb=a1;}
  int a2 = bin+1; if(f==m_nPhi-1) a2-=m_nPhi; if(event_data.m_U.H[a2]>max) {max=event_data.m_U.H[a2]; maxb=a2;}

  if ( m < m_nMom-1) {

    int a = bin+m_nPhi; if(event_data.m_U.H[a]>max) {max=event_data.m_U.H[a]; maxb=a;}
    a     = a1 +m_nPhi; if(event_data.m_U.H[a]>max) {max=event_data.m_U.H[a]; maxb=a;}
    a     = a2 +m_nPhi; if(event_data.m_U.H[a]>max) {max=event_data.m_U.H[a]; maxb=a;}

  }
  if ( m  >  0     ) {

    int a = bin-m_nPhi; if(event_data.m_U.H[a]>max) {max=event_data.m_U.H[a]; maxb=a;}
    a     = a1 -m_nPhi; if(event_data.m_U.H[a]>max) {max=event_data.m_U.H[a]; maxb=a;}
    a     = a2 -m_nPhi; if(event_data.m_U.H[a]>max) {max=event_data.m_U.H[a]; maxb=a;}
  }
  return maxb;
}

void InDet::TRT_TrackSegmentsMaker_ATLxk::magneticFieldInit()
{
  // Build MagneticFieldProperties 
  //
  if     (m_fieldmode == "NoField"    ) m_fieldprop = Trk::MagneticFieldProperties(Trk::NoField  );
  else if(m_fieldmode == "MapSolenoid") m_fieldprop = Trk::MagneticFieldProperties(Trk::FastField);
  else                                  m_fieldprop = Trk::MagneticFieldProperties(Trk::FullField);
}
