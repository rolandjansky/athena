/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::SiCombinatorialTrackFinder_xk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 12/04/2007 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <utility>

#include "SiCombinatorialTrackFinderTool_xk/SiCombinatorialTrackFinder_xk.h"

#include "TrkTrack/TrackInfo.h"
#include "EventInfo/TagInfo.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkToolInterfaces/IPatternParametersUpdator.h"
#include "TrkExInterfaces/IPatternParametersPropagator.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "TrkGeometry/MagneticFieldProperties.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiCombinatorialTrackFinder_xk::SiCombinatorialTrackFinder_xk
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p)                                           ,
    m_pixelCondSummarySvc("PixelConditionsSummarySvc",n        ),
    m_sctCondSummarySvc  ("SCT_ConditionsSummarySvc" ,n        ),
    m_fieldServiceHandle("AtlasFieldSvc",n)                     ,
    m_proptool   ("Trk::RungeKuttaPropagator/InDetPropagator"  ),
    m_updatortool("Trk::KalmanUpdator_xk/InDetPatternUpdator"  ),
    m_riocreator ("Trk::RIO_OnTrackCreator/RIO_OnTrackCreator" ),
    m_assoTool   ("InDet::InDetPRD_AssociationToolGangedPixels"),
    m_pixcontainer ("PixelClusters"),
    m_sctcontainer ("SCT_Clusters")
{
  m_usePIX      = true               ;
  m_useSCT      = true               ;
  m_simpleTrack = false              ;
  m_heavyion    = false              ;
  m_sctm        = "SCT"              ;
  m_pixm        = "Pixel"            ;
  m_fieldmode   = "MapSolenoid"      ;
//  m_pixelname   = "PixelClusters"    ;
//  m_sctname     = "SCT_Clusters"     ;
  m_inputseeds  = 0                  ;
  m_findtracks  = 0                  ;
  m_qualityCut  = 9.3                ;
  m_fieldService = 0                 ; 
  m_passThroughExtension = false     ;
  declareInterface<ISiCombinatorialTrackFinder>(this);

  declareProperty("SCTManagerLocation"   ,m_sctm               );
  declareProperty("PixManagerLocation"   ,m_pixm               );
  declareProperty("PropagatorTool"       ,m_proptool           );
  declareProperty("UpdatorTool"          ,m_updatortool        );
  declareProperty("RIOonTrackTool"       ,m_riocreator         );
  declareProperty("MagneticFieldMode"    ,m_fieldmode          );
  declareProperty("PixelClusterContainer",m_pixcontainer       );
  declareProperty("SCT_ClusterContainer" ,m_sctcontainer       ); 
  declareProperty("AssosiationTool"      ,m_assoTool           );
  declareProperty("usePixel"             ,m_usePIX             );
  declareProperty("useSCT"               ,m_useSCT             );
  declareProperty("PixelSummarySvc"      ,m_pixelCondSummarySvc);
  declareProperty("SctSummarySvc"        ,m_sctCondSummarySvc  );
  declareProperty("TrackQualityCut"      ,m_qualityCut         );
  declareProperty("MagFieldSvc"         , m_fieldServiceHandle );
  declareProperty("PassThroughExtension" ,m_passThroughExtension);
}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::SiCombinatorialTrackFinder_xk::~SiCombinatorialTrackFinder_xk()
{
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiCombinatorialTrackFinder_xk::initialize()
{
  
  // Get RungeKutta propagator tool
  //
  if ( m_proptool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_proptool << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_proptool << endreq;
  }

  // Get updator tool
  //
  if ( m_updatortool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_updatortool << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_updatortool << endreq;
  }

  // Get RIO_OnTrack creator
  //
  if ( m_riocreator.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_riocreator << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_riocreator << endreq;
  }
  Trk::IRIO_OnTrackCreator* riocreator = &(*m_riocreator);

  // Get tool for track-prd association
  //
  Trk::IPRD_AssociationTool* assoTool = 0;
  if( !m_assoTool.retrieve().isFailure()) assoTool = &(*m_assoTool);

  // Get PixelConditionsSummarySvc
  //
  IInDetConditionsSvc* pixcond = 0; 
  if(m_usePIX ) {  
    if ( m_pixelCondSummarySvc.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_pixelCondSummarySvc << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_pixelCondSummarySvc << endreq;
    }
    pixcond = &(*m_pixelCondSummarySvc); 
  }

  // Get SctConditionsSummarySvc
  //
  IInDetConditionsSvc* sctcond = 0; 
  if(m_useSCT ) {  
    if ( m_sctCondSummarySvc.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_sctCondSummarySvc << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_sctCondSummarySvc << endreq;
    }
    sctcond = &(*m_sctCondSummarySvc); 
  }

  // get the key -- from StoreGate (DetectorStore)
  //
  std::vector< std::string > tagInfoKeys =  detStore()->keys<TagInfo> ();
  std::string tagInfoKey = "";

  if(tagInfoKeys.size()==0)
    msg(MSG::WARNING) << " No TagInfo keys in DetectorStore "<< endreq;
   else {
     if(tagInfoKeys.size() > 1) {
       msg(MSG::WARNING) <<"More than one TagInfo key in the DetectorStore, using the first one "<< endreq;
     }
     tagInfoKey = tagInfoKeys[0];
   }

  m_callbackString = tagInfoKey;

  const DataHandle<TagInfo> tagInfoH;
  
  // register the Callback
  //
  StatusCode sc = detStore()->regFcn(&InDet::SiCombinatorialTrackFinder_xk::mapDetectorElementsProduction,
				     this,tagInfoH,m_callbackString);

  if(sc==StatusCode::SUCCESS) {
      msg(MSG::INFO) << "Registered callback for geometry " << name() << endreq;
   } else {
      msg(MSG::ERROR) << "Could not book callback for geometry " << name () << endreq;
      return StatusCode::FAILURE;
  }

  if( !m_fieldServiceHandle.retrieve() ){
    ATH_MSG_FATAL("Failed to retrieve " << m_fieldServiceHandle );
    return StatusCode::FAILURE;
  }    
  ATH_MSG_DEBUG("Retrieved " << m_fieldServiceHandle );
  m_fieldService = &*m_fieldServiceHandle;

  // Set SiTools and conditions
  //
  m_tools.setTools(&(*m_proptool),&(*m_updatortool),riocreator,assoTool,m_fieldService);
  m_tools.setTools(pixcond,sctcond);


  // Setup callback for magnetic field
  //
  magneticFieldInit();       

  // Set tool to trajectory
  //
  m_trajectory.setTools(&m_tools);

  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiCombinatorialTrackFinder_xk::finalize()
{
   StatusCode sc = AlgTool::finalize(); return sc;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream&  InDet::SiCombinatorialTrackFinder_xk::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)  { return dumpevent(out);      }
  else          { return dumpconditions(out); }
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiCombinatorialTrackFinder_xk::dumpconditions( MsgStream& out ) const
{
  int n = 62-m_proptool.type().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");

  std::string fieldmode[9] ={"NoField"       ,"ConstantField","SolenoidalField",
			     "ToroidalField" ,"Grid3DField"  ,"RealisticField" ,
			     "UndefinedField","AthenaField"  , "?????"         };

  int mode = m_fieldprop.magneticFieldMode(); 
  if(mode<0 || mode>8 ) mode = 8; 

  n     = 62-fieldmode[mode].size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");

  n     = 62-m_updatortool.type().size();
  std::string s4; for(int i=0; i<n; ++i) s4.append(" "); s4.append("|");

  n     = 62-m_riocreator.type().size();
  std::string s5; for(int i=0; i<n; ++i) s5.append(" "); s5.append("|");


  n     = 62-m_pixcontainer.name().size();
  std::string s7; for(int i=0; i<n; ++i) s7.append(" "); s7.append("|");

  n     = 62-m_sctcontainer.name().size();
  std::string s8; for(int i=0; i<n; ++i) s8.append(" "); s8.append("|");

  n     = 62-m_assoTool.type().size();
  std::string s9; for(int i=0; i<n; ++i) s9.append(" "); s9.append("|");

  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
       <<std::endl;
  if(m_usePIX) {
    out<<"| Pixel clusters location | "<<m_pixcontainer.name()      <<s7<<std::endl;
  }
  if(m_useSCT) {
    out<<"| SCT   clusters location | "<<m_sctcontainer.name()      <<s8<<std::endl;
  }
  out<<"| Tool for propagation    | "<<m_proptool   .type()<<s1<<std::endl;
  out<<"| Tool for updator        | "<<m_updatortool.type()<<s4<<std::endl;
  out<<"| Tool for rio  on track  | "<<m_riocreator .type()<<s5<<std::endl;
  out<<"| Tool for track-prd assos| "<<m_assoTool   .type()<<s9<<std::endl;
  out<<"| Magnetic field mode     | "<<fieldmode[mode]     <<s3<<std::endl;
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
      <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiCombinatorialTrackFinder_xk::dumpevent( MsgStream& out ) const
{
  out<<"|---------------------------------------------------------------------|"
       <<std::endl;
  out<<"| Min pT of track (MeV)   | "<<std::setw(12)<<std::setprecision(5)<<m_pTmin
     <<"                              |"<<std::endl;
  out<<"| Max Xi2 for cluster     | "<<std::setw(12)<<std::setprecision(5)<<m_xi2max
     <<"                              |"<<std::endl;
  out<<"| Max Xi2 for outlayer    | "<<std::setw(12)<<std::setprecision(5)<<m_xi2maxNoAdd
     <<"                              |"<<std::endl;
  out<<"| Max Xi2 for link        | "<<std::setw(12)<<std::setprecision(5)<<m_xi2maxlink
     <<"                              |"<<std::endl;
  out<<"| Min number of clusters  | "<<std::setw(12)<<m_nclusmin
     <<"                              |"<<std::endl;
  out<<"| Min number of wclusters | "<<std::setw(12)<<m_nwclusmin
     <<"                              |"<<std::endl;
  out<<"| Max number holes        | "<<std::setw(12)<<m_nholesmax
     <<"                              |"<<std::endl;
  out<<"| Max holes  gap          | "<<std::setw(12)<<m_dholesmax
     <<"                              |"<<std::endl;
  out<<"| Use association tool ?  | "<<std::setw(12)<<m_tools.useassoTool()
     <<"                              |"<<std::endl;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  out<<"| Number input     seeds  | "<<std::setw(12)<<m_inputseeds
     <<"                              |"<<std::endl;
  out<<"| Number accepted  seeds  | "<<std::setw(12)<<m_goodseeds
     <<"                              |"<<std::endl;
  out<<"| Number initial  tracks  | "<<std::setw(12)<<m_inittracks  
     <<"                              |"<<std::endl;
  out<<"| Number wrong DE  roads  | "<<std::setw(12)<<m_roadbug  
     <<"                              |"<<std::endl;
  out<<"| Number output   tracks  | "<<std::setw(12)<<m_findtracks  
     <<"                              |"<<std::endl;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::SiCombinatorialTrackFinder_xk::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::SiCombinatorialTrackFinder_xk& se)
{ 
  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::SiCombinatorialTrackFinder_xk& se)
{
  return se.dump(sl); 
}   

///////////////////////////////////////////////////////////////////
// Initiate track finding tool 
///////////////////////////////////////////////////////////////////

void InDet::SiCombinatorialTrackFinder_xk::newEvent()
{
//  m_pix          = false;
//  m_sct          = false;
//  m_pixcontainer = 0    ;
//  m_sctcontainer = 0    ; 

//  // Test is sct clusters collection for given event
//  //
//  if(m_usePIX) {
//    StatusCode sc = evtStore()->retrieve(m_pixcontainer,m_pixelname);
//    if(!sc.isFailure() && m_pixcontainer) m_pix = true;
//  }
//  // Test is sct clusters collection for given event
//  //
//  if(m_useSCT) {
//    StatusCode  sc = evtStore()->retrieve(m_sctcontainer,m_sctname);
//    if(!sc.isFailure() && m_sctcontainer) m_sct = true;
//  }

//Adam - Migration to ReadHandle
  m_pix = m_usePIX && m_pixcontainer.isValid();
  m_sct = m_useSCT && m_sctcontainer.isValid();


  // Erase statistic information
  //
  m_inputseeds = 0;
  m_goodseeds  = 0;
  m_inittracks = 0;
  m_findtracks = 0;
  m_roadbug    = 0;

  // Set track info
  //
  m_trackinfo.setPatternRecognitionInfo( Trk::TrackInfo::SiSPSeededFinder);
  m_cosmicTrack = 0; 
}

///////////////////////////////////////////////////////////////////
// Initiate track finding tool
///////////////////////////////////////////////////////////////////

void InDet::SiCombinatorialTrackFinder_xk::newEvent
(Trk::TrackInfo info,const TrackQualityCuts& Cuts)
{
  newEvent(); m_trackinfo = info;
  
  // Get track qulaity cuts information
  //
  getTrackQualityCuts(Cuts);

  m_heavyion    = false;
  m_cosmicTrack =     0;

  if     (info.patternRecoInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_Cosmic)) {
    m_cosmicTrack = 1;
  }
  else if(info.patternRecoInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_HeavyIon)) {
    m_heavyion = true;
  }
  m_tools.setHeavyIon(m_heavyion);
}

///////////////////////////////////////////////////////////////////
// Finalize track finding tool for given event
///////////////////////////////////////////////////////////////////

void InDet::SiCombinatorialTrackFinder_xk::endEvent()
{

  // Print event information 
  //
  if(m_outputlevel<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endreq;
  }
}

///////////////////////////////////////////////////////////////////
// Main method for track finding using space points
///////////////////////////////////////////////////////////////////

const std::list<Trk::Track*>&  InDet::SiCombinatorialTrackFinder_xk::getTracks
(const Trk::TrackParameters& Tp,
 const std::list<const Trk::SpacePoint*>& Sp,const std::list<Amg::Vector3D>& Gp,
 std::vector<const InDetDD::SiDetectorElement*>& DE,const TrackQualityCuts& Cuts)
{
  m_tools.setBremNoise(false,false);
  m_tracks.erase(m_tracks.begin(),m_tracks.end());

  ++m_inputseeds;
  if(!m_pix && !m_sct) return m_tracks;

  // Get track qulaity cuts information
  //
  getTrackQualityCuts(Cuts);
  std::multimap<const Trk::PrepRawData*,const Trk::Track*> PT;  
  if(!findTrack(Tp,Sp,Gp,DE,PT)) return m_tracks;
 
  m_trajectory.sortStep();

  // Test ordering of the detector elements
  /*
  if(!m_trajectory.goodOrder()) {
    std::cout<<"Wrond ordering detector elements on the trajectory= "<<std::endl;
    std::cout<< m_trajectory <<std::endl;
  }
  */  

  // Trk::Track production
  //
  Trk::Track* t = convertToTrack();  
  ++m_findtracks; m_tracks.push_back(t);

  if(!m_tools.multiTrack() || m_simpleTrack || Sp.size()<=2 || m_cosmicTrack || m_trajectory.pTfirst() < m_tools.pTmin()) return m_tracks;

  while((t=convertToNextTrack())) {++m_findtracks; m_tracks.push_back(t);} 
  return m_tracks;
}

///////////////////////////////////////////////////////////////////
// Main method for track finding using space points
///////////////////////////////////////////////////////////////////

const std::list<Trk::Track*>&  InDet::SiCombinatorialTrackFinder_xk::getTracks
(const Trk::TrackParameters& Tp,
 const std::list<const Trk::SpacePoint*>& Sp,const std::list<Amg::Vector3D>& Gp,
 std::vector<const InDetDD::SiDetectorElement*>& DE,
 std::multimap<const Trk::PrepRawData*,const Trk::Track*>& PT)
{
  m_tools.setBremNoise(false,false);
  m_tracks.erase(m_tracks.begin(),m_tracks.end());

  ++m_inputseeds;
  if(!m_pix && !m_sct) return m_tracks;

  if(!findTrack(Tp,Sp,Gp,DE,PT)     ) return m_tracks;
  if(!m_trajectory.isNewTrack(PT)) return m_tracks;

  m_trajectory.sortStep();

  // Test ordering of the detector elements
  /*
  if(!m_trajectory.goodOrder()) {
    std::cout<<"Wrond ordering detector elements on the trajectory= "<<std::endl;
    std::cout<< m_trajectory <<std::endl;
  }
  */  

  // Trk::Track production
  //
  Trk::Track* t = convertToTrack(); if(!t) return m_tracks;

  ++m_findtracks; m_tracks.push_back(t);

  if(!m_tools.multiTrack() || m_simpleTrack || Sp.size()<=2 || m_cosmicTrack || m_trajectory.pTfirst() < m_tools.pTmin()) return m_tracks;

  while((t=convertToNextTrack())) {++m_findtracks; m_tracks.push_back(t);} 
  return m_tracks;
}

///////////////////////////////////////////////////////////////////
// Main method for track finding using space points and 
// using electron noise model
///////////////////////////////////////////////////////////////////

const std::list<Trk::Track*>&  InDet::SiCombinatorialTrackFinder_xk::getTracksWithBrem
(const Trk::TrackParameters& Tp,
 const std::list<const Trk::SpacePoint*>& Sp,const std::list<Amg::Vector3D>& Gp,
 std::vector<const InDetDD::SiDetectorElement*>& DE,
 std::multimap<const Trk::PrepRawData*,const Trk::Track*>& PT,
 bool isCaloCompatible)
{
  // Old information
  //
  int mult    = 0; if(m_tools.multiTrack()) mult = 1; 
  double Xi2m = m_tools.xi2multi();

  m_tools.setBremNoise(false,true);
  m_tracks.erase(m_tracks.begin(),m_tracks.end());

  ++m_inputseeds;
  if(!m_pix && !m_sct) return m_tracks;

  bool  Q = findTrack(Tp,Sp,Gp,DE,PT); 
  if(Q) Q = m_trajectory.isNewTrack(PT); 
  int na  = 0;
  if(Q) {

    m_trajectory.sortStep();

    // Trk::Track production
    //
    Trk::TrackInfo oldinfo = m_trackinfo; 
    if(isCaloCompatible) m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::TrackInCaloROI);

    m_tools.setMultiTracks(0   ,Xi2m);
    Trk::Track* t = convertToTrack(); m_trackinfo = oldinfo;
    m_tools.setMultiTracks(mult,Xi2m);

    if(!t) return m_tracks;
    ++m_findtracks; m_tracks.push_back(t);
    na = m_trajectory.nclusters(); if(na >=12 && !m_trajectory.nclustersNoAdd()) return m_tracks;  
   
    if(m_trajectory.pTfirst() <  m_pTminBrem) return m_tracks;
  }
  if((*Sp.begin())->clusterList().second) return m_tracks;
 
  // Repeat track finding using electron noise model
  //
  m_tools.setBremNoise(true,true);
  if(!findTrack(Tp,Sp,Gp,DE,PT)  ) return m_tracks;
  if(!m_trajectory.isNewTrack(PT)) return m_tracks;
  
  int nb = m_trajectory.nclusters();
  if(nb <= na ) return m_tracks;
  
  m_trajectory.sortStep();

  // Trk::Track production
  //
  Trk::TrackInfo oldinfo = m_trackinfo; 
  m_trackinfo.setTrackProperties(Trk::TrackInfo::BremFit          );
  m_trackinfo.setTrackProperties(Trk::TrackInfo::BremFitSuccessful);       
  if(isCaloCompatible) m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::TrackInCaloROI);

  m_tools.setMultiTracks(0   ,Xi2m);
  Trk::Track* t = convertToTrack();  m_trackinfo = oldinfo; 
  m_tools.setMultiTracks(mult,Xi2m);

  if(!t) return m_tracks;

  ++m_findtracks; m_tracks.push_back(t);
  return m_tracks;
}

///////////////////////////////////////////////////////////////////
// Main method for track finding using space points
///////////////////////////////////////////////////////////////////

bool InDet::SiCombinatorialTrackFinder_xk::findTrack
(const Trk::TrackParameters& Tp,
 const std::list<const Trk::SpacePoint*>& Sp,const std::list<Amg::Vector3D>& Gp,
 std::vector<const InDetDD::SiDetectorElement*>& DE,
 std::multimap<const Trk::PrepRawData*,const Trk::Track*>& PT)
{
  // List detector element links preparation
  //
  std::vector<const InDet::SiDetElementBoundaryLink_xk*> DEL; 
  detectorElementLinks(DE,DEL);

  // List cluster preparation
  //
  std::vector<const InDet::SiCluster*> Cl; 
  bool TWO     = false;

  if     (Sp.size() > 1) {
    if(!spacePointsToClusters(Sp,Cl)) { return false; }
    if(Sp.size()<=2) { TWO = true; }
  }
  else if(Gp.size() > 2) {
    if(!m_trajectory.globalPositionsToClusters(m_pixcontainer.ptr(),m_sctcontainer.ptr(),Gp,DEL,PT,Cl)) return false;
  }
  else                   {
    if(!m_trajectory.trackParametersToClusters(m_pixcontainer.ptr(),m_sctcontainer.ptr(),Tp,DEL,PT,Cl)) return false;
  }
  ++m_goodseeds;

  // Build initial trajectory
  //
  bool Qr;
  bool Q = m_trajectory.initialize(m_pix,m_sct,m_pixcontainer.ptr(),m_sctcontainer.ptr(),Tp,Cl,DEL,Qr);

  if(!Q && Sp.size() < 2 && Gp.size() > 3) {

    Cl.clear();
    if(!m_trajectory.trackParametersToClusters(m_pixcontainer.ptr(),m_sctcontainer.ptr(),Tp,DEL,PT,Cl)) return false;
    if(!m_trajectory.initialize   (m_pix,m_sct,m_pixcontainer.ptr(),m_sctcontainer.ptr(),Tp,Cl,DEL,Qr)) return false;
    Q=Qr=true;
  }

  if(!Qr) { ++m_roadbug; }
  if(!Q)  { return false; }
  ++m_inittracks;
  bool pixseed = m_trajectory.isLastPixel();
  int itmax    = 30; if(m_simpleTrack) itmax = 10; if(m_heavyion) itmax = 50;

  // Track finding
  //
  if(m_passThroughExtension) return true;
  if(pixseed) {      // Strategy for pixel seeds
    
    if(!m_trajectory.forwardExtension (false,itmax)) return false;
    if(!m_trajectory.backwardSmoother (false)      ) return false;
    if(!m_trajectory.backwardExtension(itmax)      ) return false;

    if(m_trajectory.difference() > 0) {
      if(!m_trajectory.forwardFilter()          ) return false;
      if(!m_trajectory.backwardSmoother (false) ) return false;
    } 
    int na = m_trajectory.nclustersNoAdd();
    if(m_trajectory.nclusters()+na < m_nclusmin || m_trajectory.ndf() < m_nwclusmin) return false;
  }
  else        {      // Strategy for mixed seeds

    if(!m_trajectory.backwardSmoother(TWO)       ) return false;
    if(!m_trajectory.backwardExtension(itmax)    ) return false;
    if(!m_trajectory.forwardExtension(true,itmax)) return false;

    int na = m_trajectory.nclustersNoAdd();
    if(m_trajectory.nclusters()+na < m_nclusmin || m_trajectory.ndf() < m_nwclusmin) return false;
    if(!m_trajectory.backwardSmoother(false)    ) return false;

    na     = m_trajectory.nclustersNoAdd();
    if(m_trajectory.nclusters()+na < m_nclusmin || m_trajectory.ndf() < m_nwclusmin) return false;

    if(m_trajectory.difference() > 0) {
      if(!m_trajectory.forwardFilter()         ) return false;
      if(!m_trajectory.backwardSmoother (false)) return false;
    }
  } 

  if(m_trajectory.qualityOptimization()     <           (m_qualityCut*m_nclusmin)    ) return false;
  if(m_trajectory.pTfirst  () < m_pTmin     && m_trajectory.nclusters() < m_nclusmin ) return false;
  if(m_trajectory.nclusters() < m_nclusminb || m_trajectory.ndf      () < m_nwclusmin) return false;
  
  return true;
}

///////////////////////////////////////////////////////////////////
// Trk::Track production
///////////////////////////////////////////////////////////////////

Trk::Track* InDet::SiCombinatorialTrackFinder_xk::convertToTrack()
 {
  if(!m_simpleTrack) {

    return new Trk::Track
      (
       m_trackinfo,
       m_trajectory.convertToTrackStateOnSurface(m_cosmicTrack),
       m_trajectory.convertToFitQuality()
       );
  }
  else              {
    
    Trk::TrackInfo info = m_trackinfo; 
    info.setPatternRecognitionInfo(Trk::TrackInfo::SiSPSeededFinderSimple);
    return new Trk::Track
      (
       info,
       m_trajectory.convertToSimpleTrackStateOnSurface(m_cosmicTrack),
       m_trajectory.convertToFitQuality()
       );
  }
}

///////////////////////////////////////////////////////////////////
// Next Trk::Track production
///////////////////////////////////////////////////////////////////

Trk::Track* InDet::SiCombinatorialTrackFinder_xk::convertToNextTrack()
  {
    DataVector<const Trk::TrackStateOnSurface>* tsos = m_trajectory.convertToNextTrackStateOnSurface();
    if(!tsos) return 0;
    return new Trk::Track
      (m_trackinfo,tsos,m_trajectory.convertToFitQuality());
  }

///////////////////////////////////////////////////////////////////
// Callback function - get the magnetic field /
///////////////////////////////////////////////////////////////////

void InDet::SiCombinatorialTrackFinder_xk::magneticFieldInit() 
{
  // Build MagneticFieldProperties 
  //
  Trk::MagneticFieldProperties* pMF = 0;
  if     (m_fieldmode == "NoField"    ) pMF = new Trk::MagneticFieldProperties(Trk::NoField  );
  else if(m_fieldmode == "MapSolenoid") pMF = new Trk::MagneticFieldProperties(Trk::FastField);
  else                                  pMF = new Trk::MagneticFieldProperties(Trk::FullField);
  m_fieldprop = *pMF; delete pMF;
  m_tools.setTools(m_fieldprop);
}

///////////////////////////////////////////////////////////////////
// Map of detector elements production
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiCombinatorialTrackFinder_xk::mapDetectorElementsProduction 
(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  (void) I;

  StatusCode sc; 

  // Get  Pixel Detector Manager
  //
  const InDetDD::PixelDetectorManager* pixmgr = 0;
  if(m_usePIX) {
    sc = detStore()->retrieve(pixmgr,m_pixm);
    if (sc.isFailure() || !pixmgr) {
      msg(MSG::FATAL)<<"Could not get PixelDetectorManager  !"<<endreq; 
      return StatusCode::FAILURE;
    }
  }

  // Get  SCT Detector Manager
  //
  const InDetDD::SCT_DetectorManager* sctmgr = 0;
  if(m_useSCT) {
    sc = detStore()->retrieve(sctmgr,m_sctm);
    if (sc.isFailure() || !sctmgr) {
      msg(MSG::FATAL)<<"Could not get SCT_DetectorManager !"<<endreq; 
      return StatusCode::FAILURE;
    }
  }

  // check if the string is ESD for guaranteeing that misalignment has been introduced already
  //
  bool needsUpdate = false;

  for (std::list<std::string>::const_iterator k=keys.begin(); k!=keys.end(); ++k) {
    if ((*k) == m_callbackString) {needsUpdate = true; break;}
  } 
  if(!needsUpdate) return StatusCode::SUCCESS;

  m_boundaryPIX.erase(m_boundaryPIX.begin(),m_boundaryPIX.end()); 
  m_boundarySCT.erase(m_boundarySCT.begin(),m_boundarySCT.end()); 
  
  const PixelID* IDp = 0; 
  const SCT_ID*  IDs = 0; 

  if (m_usePIX && detStore()->retrieve(IDp, "PixelID").isFailure()) {
    msg(MSG::FATAL) << "Could not get Pixel ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  
  if (m_useSCT && detStore()->retrieve(IDs, "SCT_ID").isFailure()) {
    msg(MSG::FATAL) << "Could not get SCT ID helper" << endreq;
    return StatusCode::FAILURE;
  }

   if(!IDs && !IDp) return StatusCode::FAILURE;

  InDetDD::SiDetectorElementCollection::const_iterator s,se;

  unsigned int npix = 0, minpixid = 1000000, maxpixid = 0;
  unsigned int nsct = 0, minsctid = 1000000, maxsctid = 0;

  if(IDp) {

    std::map<IdentifierHash,const InDetDD::SiDetectorElement*> idd;

    // Loop over each wafer of pixels
    //
    s  =  pixmgr->getDetectorElementBegin();
    se =  pixmgr->getDetectorElementEnd  ();

    for (; s!=se; ++s) {
     
      IdentifierHash id = (*s)->identifyHash();
      ++npix;
      if(id > maxpixid) maxpixid = id;
      if(id < minpixid) minpixid = id;
      idd.insert(std::make_pair(id,(*s)));
    }
    if(idd.size()!= maxpixid+1 || minpixid!=0) return StatusCode::FAILURE;
    
    m_boundaryPIX.reserve(idd.size());
    std::map<IdentifierHash,const InDetDD::SiDetectorElement*>::iterator i;

    for(i = idd.begin(); i!=idd.end(); ++i) {

      InDet::SiDetElementBoundaryLink_xk dl((*i).second);
      m_boundaryPIX.push_back(dl);
    }
  }

  if(IDs) {

    std::map<IdentifierHash,const InDetDD::SiDetectorElement*> idd;

    // Loop over each wafer of sct
    //
    s  = sctmgr->getDetectorElementBegin();
    se = sctmgr->getDetectorElementEnd  ();

    for (; s!=se; ++s) {

      const IdentifierHash id = (*s)->identifyHash();
      ++nsct;
      if(id > maxsctid) maxsctid = id;
      if(id < minsctid) minsctid = id;
      idd.insert(std::make_pair(id,(*s)));
    }
    if(idd.size()!= maxsctid+1 || minsctid!=0) return StatusCode::FAILURE;
    
    m_boundarySCT.reserve(idd.size());
    std::map<IdentifierHash,const InDetDD::SiDetectorElement*>::iterator i;

    for(i = idd.begin(); i!=idd.end(); ++i) {

      InDet::SiDetElementBoundaryLink_xk dl((*i).second);
      m_boundarySCT.push_back(dl);
    }
  }
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Convert space points to clusters
///////////////////////////////////////////////////////////////////

bool InDet::SiCombinatorialTrackFinder_xk::spacePointsToClusters
(const std::list<const Trk::SpacePoint*>& Sp,std::vector<const InDet::SiCluster*>& Sc)
{
  std::list<const Trk::SpacePoint*>::const_iterator s=Sp.begin(),se=Sp.end();

  for(; s!=se; ++s) {

     const Trk::PrepRawData* p = (*s)->clusterList().first ; 
     
     if(p) {
       const InDet::SiCluster* c = static_cast<const InDet::SiCluster*>(p);
       if(c) Sc.push_back(c);
     }
     p = (*s)->clusterList().second; 
     if(p) {
       const InDet::SiCluster* c = static_cast<const InDet::SiCluster*>(p);
       if(c) Sc.push_back(c);
     }
  }

  //  Detector elments test
  //
  std::vector<const InDet::SiCluster*>::iterator c = Sc.begin(),cn,ce = Sc.end();
  
  for(; c!=ce; ++c) {

    const InDetDD::SiDetectorElement* de = (*c)->detectorElement(); 

    for(++(cn=c); cn!=ce; ++cn) {
      if(de == (*cn)->detectorElement()) return false;
    }

  }
  return true;
}
 
///////////////////////////////////////////////////////////////////
// Convert detector elements to detector element links
///////////////////////////////////////////////////////////////////

void InDet::SiCombinatorialTrackFinder_xk::detectorElementLinks
(std::vector<const InDetDD::SiDetectorElement*>        & DE,
 std::vector<const InDet::SiDetElementBoundaryLink_xk*>& DEL)
{
  std::vector<const InDetDD::SiDetectorElement*>::iterator d = DE.begin(),de = DE.end();
 
  for(; d!=de; ++d) {
 
    IdentifierHash id = (*d)->identifyHash();
    if     ((*d)->isPixel() && id < m_boundaryPIX.size()) DEL.push_back(&m_boundaryPIX[id]);
    else if((*d)->isSCT()   && id < m_boundarySCT.size()) DEL.push_back(&m_boundarySCT[id]);
  }
}

///////////////////////////////////////////////////////////////////
// Get track quality cuts
///////////////////////////////////////////////////////////////////

void  InDet::SiCombinatorialTrackFinder_xk::getTrackQualityCuts
(const TrackQualityCuts& Cuts)
{
  // Integer cuts
  //
  int useasso,simpletrack,multitrack;
  double xi2m;
  if(!Cuts.getIntCut   ("MinNumberOfClusters" ,m_nclusmin   )) {m_nclusmin      =    7;}
  if(!Cuts.getIntCut   ("MinNumberOfWClusters",m_nwclusmin  )) {m_nwclusmin     =    7;}
  if(!Cuts.getIntCut   ("MaxNumberOfHoles"    ,m_nholesmax  )) {m_nholesmax     =    2;}
  if(!Cuts.getIntCut   ("MaxHolesGae"         ,m_dholesmax  )) {m_dholesmax     =    2;}
  if(!Cuts.getIntCut   ("UseAssociationTool"  ,useasso      )) {useasso         =    0;} 
  if(!Cuts.getIntCut   ("CosmicTrack"         ,m_cosmicTrack)) {m_cosmicTrack   =    0;}
  if(!Cuts.getIntCut   ("SimpleTrack"         ,simpletrack  )) {simpletrack     =    0;} 
  if(!Cuts.getIntCut   ("doMultiTracksProd"   ,multitrack   )) {multitrack      =    0;}  
 
  // Double cuts
  //
  if(!Cuts.getDoubleCut("pTmin"              ,m_pTmin      )) {m_pTmin         = 500.;}
  if(!Cuts.getDoubleCut("pTminBrem"          ,m_pTminBrem  )) {m_pTminBrem     =1000.;}
  if(!Cuts.getDoubleCut("MaxXi2forCluster"   ,m_xi2max     )) {m_xi2max        =   9.;}
  if(!Cuts.getDoubleCut("MaxXi2forOutlier"   ,m_xi2maxNoAdd)) {m_xi2maxNoAdd   =  25.;}
  if(!Cuts.getDoubleCut("MaxXi2forSearch"    ,m_xi2maxlink )) {m_xi2maxlink    = 100.;}
  if(!Cuts.getDoubleCut("MaxXi2MultiTracks"  ,xi2m         )) {xi2m            =   7.;}

  simpletrack ? m_simpleTrack = true :  m_simpleTrack = false;

  if(!m_cosmicTrack) {

    if(   xi2m       > 7. ) xi2m          = 7.         ;
    if(m_nholesmax   > 2  ) m_nholesmax   = 2          ; 
    if(m_dholesmax   > 2  ) m_dholesmax   = 2          ;
    if(m_xi2maxNoAdd > 25.) m_xi2maxNoAdd = 25.        ;
  }

  if(m_xi2maxNoAdd  <= m_xi2max   ) m_xi2maxNoAdd = m_xi2max+5.;
  if(m_dholesmax    >  m_nholesmax) m_dholesmax   = m_nholesmax;
  m_nclusminb = m_nclusmin-1; if(m_nclusminb < 3 ) m_nclusminb = 3;

  m_tools.setXi2pTmin     (m_xi2max,m_xi2maxNoAdd,m_xi2maxlink,m_pTmin);
  m_tools.setHolesClusters(m_nholesmax,m_dholesmax,m_nclusmin);
  m_tools.setAssociation  (useasso);
  m_tools.setMultiTracks  (multitrack,xi2m);
  m_trajectory.setParameters();
}
