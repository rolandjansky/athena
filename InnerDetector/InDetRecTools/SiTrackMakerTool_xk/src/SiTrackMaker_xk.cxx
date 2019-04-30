/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::SiTrackMaker_xk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <utility>

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "SiTrackMakerTool_xk/SiTrackMaker_xk.h"

#include "TrkCaloClusterROI/CaloClusterROI.h"
//#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiTrackMaker_xk::SiTrackMaker_xk
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p)                                        ,
    m_fieldServiceHandle("AtlasFieldSvc",n), 
    m_roadmaker   ("InDet::SiDetElementsRoadMaker_xk"    ),
    m_tracksfinder("InDet::SiCombinatorialTrackFinder_xk"),
    m_seedtrack   ("InDet::SeedToTrackConversionTool"    ),
    m_caloCluster("InDetCaloClusterROIs"),
    m_caloHad("InDetHadCaloClusterROIs")
{
  m_fieldmode    = "MapSolenoid"      ;
  m_patternName  = "SiSPSeededFinder" ; 
  m_pix          = false              ;
  m_sct          = false              ;
  m_usePix       = true               ;
  m_useSct       = true               ;
  m_useassoTool  = false              ;
  m_cosmicTrack  = false              ;    
  m_simpleTrack  = false              ;
  m_multitracks  = false              ;
  m_useBremModel = false              ;
  m_useCaloSeeds = false              ;
  m_useHClusSeed = false              ;
  m_useSSSfilter = true               ;
  m_ITKGeomtry   = false              ;
  m_heavyion     = false              ;
  m_xi2max       = 15.                ;
  m_xi2maxNoAdd  = 35.                ;
  m_xi2maxlink   = 200.               ;
  m_xi2multitracks = 3.               ;
  m_pTmin        = 500.               ;
  m_pTminBrem    = 1000.              ;
  m_pTminSSS     = 1000.              ;
  m_distmax      = 5.                 ;
  m_nholesmax    = 2                  ;
  m_dholesmax    = 2                  ;
  m_nclusmin     = 6                  ;
  m_nwclusmin    = 6                  ;              
  m_wrongcluster = 10                 ; 
  m_inputseeds   = 0                  ;
  m_goodseeds    = 0                  ;
  m_findtracks   = 0                  ;
  m_seedsfilter  = 2                  ;
  m_phiWidth     = .3                 ;
  m_etaWidth     = .3                 ;
  m_seedsegmentsWrite = false;
//  m_inputClusterContainerName    = "InDetCaloClusterROIs"   ;
//  m_inputHadClusterContainerName = "InDetHadCaloClusterROIs";
  m_beamconditions               = "BeamCondSvc"            ;

  declareInterface<ISiTrackMaker>(this);

  declareProperty("RoadTool"                ,m_roadmaker   );
  declareProperty("CombinatorialTrackFinder",m_tracksfinder);
  declareProperty("SeedToTrackConversion"   ,m_seedtrack   );
  declareProperty("Xi2max"                  ,m_xi2max      );
  declareProperty("Xi2maxNoAdd"             ,m_xi2maxNoAdd );
  declareProperty("Xi2maxlink"              ,m_xi2maxlink  );
  declareProperty("pTmin"                   ,m_pTmin       );
  declareProperty("pTminBrem"               ,m_pTminBrem   );
  declareProperty("pTminSSS"                ,m_pTminSSS    );
  declareProperty("MaxDistanceForSCTsp"     ,m_distmax     );
  declareProperty("nHolesMax"               ,m_nholesmax   );
  declareProperty("nHolesGapMax"            ,m_dholesmax   );
  declareProperty("nClustersMin"            ,m_nclusmin    );
  declareProperty("nWeightedClustersMin"    ,m_nwclusmin   );
  declareProperty("MagneticFieldMode"       ,m_fieldmode   );
  declareProperty("SeedsFilterLevel"        ,m_seedsfilter );
  declareProperty("GoodSeedClusterCount"    ,m_wrongcluster);
  declareProperty("UseAssociationTool"      ,m_useassoTool );
  declareProperty("CosmicTrack"             ,m_cosmicTrack );
  declareProperty("TrackPatternRecoInfo"    ,m_patternName );
  declareProperty("doMultiTracksProd"       ,m_multitracks );
  declareProperty("Xi2maxMultiTracks"       ,m_xi2multitracks);
  declareProperty("useBremModel"            ,m_useBremModel); 
  declareProperty("useSSSseedsFilter"       ,m_useSSSfilter); 
  declareProperty("doCaloSeededBrem"        ,m_useCaloSeeds);
  declareProperty("doHadCaloSeedSSS"        ,m_useHClusSeed);
  declareProperty("phiWidth"                ,m_phiWidth    );
  declareProperty("etaWidth"                ,m_etaWidth    );
  declareProperty("InputClusterContainerName"   , m_caloCluster   );
  declareProperty("InputHadClusterContainerName", m_caloHad );
  declareProperty("MagFieldSvc"             , m_fieldServiceHandle);
  declareProperty("useSCT"                  , m_useSct);
  declareProperty("usePixel"                , m_usePix);
  declareProperty("ITKGeometry"             , m_ITKGeomtry);
  declareProperty("SeedSegmentsWrite"       , m_seedsegmentsWrite);
}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::SiTrackMaker_xk::~SiTrackMaker_xk()
{
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiTrackMaker_xk::initialize()
{
  StatusCode sc = AlgTool::initialize(); 

  // Get beam geometry
  //
  m_beam = 0;
  if(m_beamconditions!="") {
    sc = service(m_beamconditions,m_beam);
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

  // Get detector elements road maker tool
  //
  if ( m_roadmaker.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_roadmaker << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_roadmaker << endreq;
  }

  // Get combinatorial track finder tool
  //
  if ( m_tracksfinder.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_tracksfinder << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_tracksfinder << endreq;
  }

  // Get seed to track conversion tool
  //
  if(m_seedsegmentsWrite) {

    if(m_seedtrack.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_seedtrack << endreq;
      return StatusCode::FAILURE;

    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_seedtrack << endreq;
    }
  }

  m_heavyion = false;

  // TrackpatternRecoInfo preparation 
  //
  if     (m_patternName == "SiSpacePointsSeedMaker_Cosmic"     )  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_Cosmic     );
  }
  else if(m_patternName == "SiSpacePointsSeedMaker_HeavyIon"   )  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_HeavyIon   ); m_heavyion = true;
  }
  else if(m_patternName == "SiSpacePointsSeedMaker_LowMomentum")  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_LowMomentum);
  }
  else if(m_patternName == "SiSpacePointsSeedMaker_BeamGas"    )  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_BeamGas    );
  }     
  else if(m_patternName == "SiSpacePointsSeedMaker_ForwardTracks"    )  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_ForwardTracks    );
  }
  else if(m_patternName == "SiSpacePointsSeedMaker_LargeD0"    )  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_LargeD0    );
  } 
  else if(m_patternName == "SiSpacePointsSeedMaker_SLHCConversionTracks")  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_SLHCConversionTracks);
  }
  else                                                            {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSPSeededFinder                  );
  } 

  if(m_cosmicTrack)  m_seedsfilter = 3;

  // Setup for magnetic field
  //
  std::string folder( "/EXT/DCS/MAGNETS/SENSORDATA" );
  const DataHandle<CondAttrListCollection> currentHandle;
  if (m_fieldmode != "NoField" && detStore()->contains<CondAttrListCollection>(folder)) {
    
    StatusCode  sc = detStore()->regFcn(&InDet::SiTrackMaker_xk::magneticFieldInit,this,currentHandle,folder);
    
    if(sc==StatusCode::SUCCESS) {
      msg(MSG::INFO) << "Registered callback from MagneticFieldSvc for " << name() << endreq;
    } else {
      msg(MSG::ERROR) << "Could not book callback from MagneticFieldSvc for " << name () << endreq;
      return StatusCode::FAILURE;
    }
  } 
  else {
    magneticFieldInit();
    ATH_MSG_INFO("Folder " << folder << " not present, magnetic field callback not set up. Not a problem if AtlasFieldSvc.useDCS=False");
  }
  
  if(m_pTmin < 20.) m_pTmin = 20.;
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiTrackMaker_xk::finalize()
{
   StatusCode sc = AlgTool::finalize(); return sc;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream&  InDet::SiTrackMaker_xk::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)
    return dumpevent(out);
  else
    return dumpconditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiTrackMaker_xk::dumpconditions( MsgStream& out ) const
{

  int n      = 62-m_tracksfinder.type().size();
  std::string s4; for(int i=0; i<n; ++i) s4.append(" "); s4.append("|");

  std::string fieldmode[9] ={"NoField"       ,"ConstantField","SolenoidalField",
			     "ToroidalField" ,"Grid3DField"  ,"RealisticField" ,
			     "UndefinedField","AthenaField"  , "?????"         };

  int mode = m_fieldprop.magneticFieldMode(); 
  if(mode<0 || mode>8 ) mode = 8; 

  n     = 62-fieldmode[mode].size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");

  n     = 62-m_roadmaker.type().size();
  std::string s6; for(int i=0; i<n; ++i) s6.append(" "); s6.append("|");

  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
       <<std::endl;
  out<<"| Tool for road builder   | "<<m_roadmaker  .type() <<s6<<std::endl;
  out<<"} Tool for track finding  | "<<m_tracksfinder.type()<<s4<<std::endl;
  out<<"| Use association tool ?  | "
     <<std::setw(12)<<m_useassoTool 
     <<"                                                  |"<<std::endl;
  out<<"| Magnetic field mode     | "<<fieldmode[mode]      <<s3<<std::endl;
  out<<"| Seeds filter level      | "<<std::setw(12)<<m_seedsfilter
     <<"                                                  |"<<std::endl;
  out<<"| Track length wrong cl.  | "<<std::setw(12)<<m_wrongcluster
     <<"                                                  |"<<std::endl;
  out<<"| Min pT of track (MeV)   | "<<std::setw(12)<<std::setprecision(5)<<m_pTmin
     <<"                                                  |"<<std::endl;
  out<<"| Max Xi2 for cluster     | "<<std::setw(12)<<std::setprecision(5)<<m_xi2max
     <<"                                                  |"<<std::endl;
  out<<"| Max Xi2 for outlayer    | "<<std::setw(12)<<std::setprecision(5)<<m_xi2maxNoAdd
     <<"                                                  |"<<std::endl;
  out<<"| Max Xi2 for link        | "<<std::setw(12)<<std::setprecision(5)<<m_xi2maxlink
     <<"                                                  |"<<std::endl;
  out<<"| Min number of clusters  | "<<std::setw(12)<<m_nclusmin
     <<"                                                  |"<<std::endl;
  out<<"| Min number of wclusters | "<<std::setw(12)<<m_nwclusmin
     <<"                                                  |"<<std::endl;
  out<<"| Max number holes        | "<<std::setw(12)<<m_nholesmax
     <<"                                                  |"<<std::endl;
  out<<"| Max holes  gap          | "<<std::setw(12)<<m_dholesmax
     <<"                                                  |"<<std::endl;
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
      <<std::endl;
  return out;
}


///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiTrackMaker_xk::dumpevent( MsgStream& out ) const
{
  out<<"|---------------------------------------------------------------------|"
       <<std::endl;
  out<<"| Number input seeds      | "<<std::setw(12)<<m_inputseeds
     <<"                              |"<<std::endl;
  out<<"| Number good  seeds      | "<<std::setw(12)<<m_goodseeds 
     <<"                              |"<<std::endl;
  out<<"| Number output tracks    | "<<std::setw(12)<<m_findtracks  
     <<"                              |"<<std::endl;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::SiTrackMaker_xk::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::SiTrackMaker_xk& se)
{ 
  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::SiTrackMaker_xk& se)
{
  return se.dump(sl); 
}   

///////////////////////////////////////////////////////////////////
// Initiate track finding tool for new event
///////////////////////////////////////////////////////////////////

void InDet::SiTrackMaker_xk::newEvent(bool PIX,bool SCT)
{
  if(m_beam) {Amg::Vector3D cb = m_beam->beamPos(); m_xybeam[0] = cb[0]; m_xybeam[1] = cb[1];}
  else       {                                      m_xybeam[0] =    0.; m_xybeam[1] =    0.;}
  
  m_pix          = PIX && m_usePix;
  m_sct          = SCT && m_useSct;
  m_simpleTrack  = false;

  setTrackQualityCuts();

  // New event for track finder tool
  //
  m_tracksfinder->newEvent(m_trackinfo,m_trackquality);

  // Erase cluster to track association
  //
  if(m_seedsfilter) m_clusterTrack.clear();

  // Erase statistic information
  //
  m_inputseeds = 0;
  m_goodseeds  = 0;
  m_findtracks = 0;

  // Retrieve 
  //
  if(m_useBremModel && m_useCaloSeeds) {

    m_caloF.clear();
    m_caloR.clear();
    m_caloZ.clear();

//    const CaloClusterROI_Collection* calo = 0;    
//    StatusCode sc = evtStore()->retrieve(calo,m_inputClusterContainerName);
 
    if(m_caloCluster.isValid()) {

      CaloClusterROI_Collection::const_iterator c = m_caloCluster->begin(), ce = m_caloCluster->end();

      for(; c!=ce; ++c) {
        m_caloF.push_back( (*c)->globalPosition().phi ());
        m_caloR.push_back( (*c)->globalPosition().perp());
	m_caloZ.push_back( (*c)->globalPosition().z   ());
     }
    }
  }

  if(!m_useSSSfilter && m_useHClusSeed) {

    m_hadF.clear();
    m_hadR.clear();
    m_hadZ.clear();

//    const CaloClusterROI_Collection* calo = 0;    
//    StatusCode sc = evtStore()->retrieve(calo,m_inputHadClusterContainerName);
  
    if(m_caloHad.isValid()) {

      CaloClusterROI_Collection::const_iterator c = m_caloHad->begin(), ce = m_caloHad->end();

      for(; c!=ce; ++c) {
        m_hadF.push_back( (*c)->globalPosition().phi ());
        m_hadR.push_back( (*c)->globalPosition().perp());
	m_hadZ.push_back( (*c)->globalPosition().z   ());
     }
    }
  }
  if(m_seedsegmentsWrite) m_seedtrack->newEvent(m_trackinfo,m_patternName);
}

///////////////////////////////////////////////////////////////////
// Initiate track finding tool for new event
///////////////////////////////////////////////////////////////////

void InDet::SiTrackMaker_xk::newTrigEvent(bool PIX,bool SCT)
{
  m_pix          = PIX && m_usePix;
  m_sct          = SCT && m_useSct;
  m_simpleTrack  = true;

  setTrackQualityCuts();

  // New event for track finder tool
  //
  m_tracksfinder->newEvent(m_trackinfo,m_trackquality);

  // Erase cluster to track association
  //
  if(m_seedsfilter) m_clusterTrack.clear(); 

  // Erase statistic information
  //
  m_inputseeds = 0;
  m_goodseeds  = 0;
  m_findtracks = 0;
}

///////////////////////////////////////////////////////////////////
// Finalize track finding tool for given event
///////////////////////////////////////////////////////////////////

void InDet::SiTrackMaker_xk::endEvent()
{

  // End event for track finder tool
  //
  m_tracksfinder->endEvent();

  //correction to exclude memory fragmentation
  m_clusterTrack.clear();

  // end event for seed to track tool
  if(m_seedsegmentsWrite) m_seedtrack->endEvent();
 
  // Print event information 
  //
  if (msgLevel()<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endreq;
  }
}

///////////////////////////////////////////////////////////////////
// Main method for track finding using space points
///////////////////////////////////////////////////////////////////

const std::list<Trk::Track*>& InDet::SiTrackMaker_xk::getTracks
(const std::list<const Trk::SpacePoint*>& Sp)
{
  ++m_inputseeds;
  m_tracks.clear();
  if(!m_pix && !m_sct) return m_tracks;
  
  bool good;  m_sss = false; 
  !m_seedsfilter ? good=true : good=newSeed(Sp);  

  if(!good) return m_tracks;

  m_dbm = isDBMSeeds(*Sp.begin());

  // Get initial parameters estimation
  // 
  const Trk::TrackParameters* Tp = getAtaPlane(m_sss && m_useHClusSeed,Sp);
  if(!Tp) return m_tracks;
  ++m_goodseeds;

  // Get detector elements road
  //
  std::vector<const InDetDD::SiDetectorElement*> DE;
  if(!m_cosmicTrack) m_roadmaker->detElementsRoad(*Tp,Trk::alongMomentum,   DE);
  else               m_roadmaker->detElementsRoad(*Tp,Trk::oppositeMomentum,DE);

  if(!m_pix || !m_sct || m_dbm) detectorElementsSelection(DE);

  if( int(DE.size())  <   m_nclusmin) {delete Tp; return m_tracks;} 

  std::list<Amg::Vector3D> Gp;
  
  // Find possible list of tracks using space points space points information
  //
  if     (!m_useBremModel      ) {
    m_tracks = m_tracksfinder->getTracks        (*Tp,Sp,Gp,DE,m_clusterTrack);
  }
  else if(!m_useCaloSeeds      ) {
    m_tracks = m_tracksfinder->getTracksWithBrem(*Tp,Sp,Gp,DE,m_clusterTrack,false);
  }
  else if(  isCaloCompatible() ) {
    m_tracks = m_tracksfinder->getTracksWithBrem(*Tp,Sp,Gp,DE,m_clusterTrack,true);
  }
  else                           {
    m_tracks = m_tracksfinder->getTracks        (*Tp,Sp,Gp,DE,m_clusterTrack);
  }
  
  if(m_seedsfilter) {

    std::list<Trk::Track*>::iterator t = m_tracks.begin();
    while(t!=m_tracks.end()) {

      if(!isNewTrack((*t))) {delete (*t); m_tracks.erase(t++);}
      else                  {clusterTrackMap((*t++));         }
    }
  }
  m_findtracks+=m_tracks.size();

  // Call seed to track execution
  //
  if(m_seedsegmentsWrite) m_seedtrack->executeSiSPSeedSegments(Tp,m_tracks.size(),Sp);

  delete Tp;
  return m_tracks;
}

///////////////////////////////////////////////////////////////////
// Main method for track finding using space points
///////////////////////////////////////////////////////////////////

const std::list<Trk::Track*>& InDet::SiTrackMaker_xk::getTracks
(const Trk::TrackParameters& Tp,const std::list<Amg::Vector3D>& Gp)
{
  ++m_inputseeds;
  m_tracks.clear();
  if(!m_pix && !m_sct) return m_tracks;

  ++m_goodseeds;

  // Get detector elements road
  //
  std::vector<const InDetDD::SiDetectorElement*> DE;
  if(!m_cosmicTrack) m_roadmaker->detElementsRoad(Tp,Trk::alongMomentum,   DE);
  else               m_roadmaker->detElementsRoad(Tp,Trk::oppositeMomentum,DE);

  if(!m_pix || !m_sct) detectorElementsSelection(DE);

  if( int(DE.size())  <   m_nclusmin) return m_tracks; 

  // Find possible list of tracks with trigger track parameters or global positions
  //
  std::list<const Trk::SpacePoint*>  Sp;
  
  if     (!m_useBremModel      ) {
    m_tracks = m_tracksfinder->getTracks        (Tp,Sp,Gp,DE,m_clusterTrack);
  }
  else if(!m_useCaloSeeds      ) {
    m_tracks = m_tracksfinder->getTracksWithBrem(Tp,Sp,Gp,DE,m_clusterTrack,false);
  }
  else if(  isCaloCompatible() ) {
    m_tracks = m_tracksfinder->getTracksWithBrem(Tp,Sp,Gp,DE,m_clusterTrack,true);
  }
  else                           {
    m_tracks = m_tracksfinder->getTracks        (Tp,Sp,Gp,DE,m_clusterTrack);
  }
  
  if(m_seedsfilter) {

    std::list<Trk::Track*>::iterator t = m_tracks.begin();
    while(t!=m_tracks.end()) {

      if(!isNewTrack((*t))) {delete (*t); m_tracks.erase(t++);}
      else                  {clusterTrackMap((*t++));         }
    }
  }
  m_findtracks+=m_tracks.size();
  return m_tracks;
}

///////////////////////////////////////////////////////////////////
// Space point seed parameters extimation
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters* InDet::SiTrackMaker_xk::getAtaPlane
(bool sss,const std::list<const Trk::SpacePoint*>& SP)
{
  if(m_dbm) return getAtaPlaneDBM(SP);

  std::list<const Trk::SpacePoint*>::const_iterator is=SP.begin(),ise=SP.end(),is0,is1,is2;  
  if(is==ise) return 0;

  const Trk::PrepRawData*       cl  = (*is)->clusterList().first;
  if(!cl) return 0;
  const Trk::PlaneSurface*      pla = 
    static_cast<const Trk::PlaneSurface*>(&cl->detectorElement()->surface());
  if(!pla) return 0;

  is0 = is; if(++is==ise) return 0; 
  is1 = is; if(++is==ise) return 0;
  is2 = is ;
  double p0[3],p1[3],p2[3]; 
  if(!globalPositions((*is0),(*is1),(*is2),p0,p1,p2)) return 0;

  double x0 = p0[0]   ;
  double y0 = p0[1]   ;
  double z0 = p0[2]   ;
  double x1 = p1[0]-x0;
  double y1 = p1[1]-y0;
  double x2 = p2[0]-x0;
  double y2 = p2[1]-y0;
  double z2 = p2[2]-z0; 

  double u1 = 1./sqrt(x1*x1+y1*y1)       ;
  double rn = x2*x2+y2*y2                ;
  double r2 = 1./rn                      ;
  double a  = x1*u1                      ;
  double b  = y1*u1                      ;
  double u2 = (a*x2+b*y2)*r2             ;
  double v2 = (a*y2-b*x2)*r2             ;
  double A  = v2/(u2-u1)                 ;
  double B  = 2.*(v2-A*u2)               ;
  double C  = B/sqrt(1.+A*A)             ;
  double T  = z2*sqrt(r2)/(1.+.04*C*C*rn);
  
  const Amg::Transform3D& Tp = pla->transform();

  double Ax[3] = {Tp(0,0),Tp(1,0),Tp(2,0)}; 
  double Ay[3] = {Tp(0,1),Tp(1,1),Tp(2,1)}; 
  double D [3] = {Tp(0,3),Tp(1,3),Tp(2,3)}; 
  
  double   d[3] = {x0-D[0],y0-D[1],z0-D[2]}            ;

  m_p[0] = d[0]*Ax[0]+d[1]*Ax[1]+d[2]*Ax[2];
  m_p[1] = d[0]*Ay[0]+d[1]*Ay[1]+d[2]*Ay[2];

  if(m_fieldprop.magneticFieldMode() > 0) {

    double H[3],gP[3] ={x0,y0,z0}; m_fieldService->getFieldZR(gP,H);

    if(fabs(H[2])>.0001) {
      m_p[2] = atan2(b+a*A,a-b*A);
      m_p[3] = atan2(1.,T)       ;  
      m_p[5] = -C/(300.*H[2])    ;
    }
    else {
      T    =  z2*sqrt(r2)  ;
      m_p[2] = atan2(y2,x2);
      m_p[3] = atan2(1.,T) ;
      m_p[5] = 1./m_pTmin  ;
    }
  }
  else {
      T    = z2*sqrt(r2)   ;
      m_p[2] = atan2(y2,x2);
      m_p[3] = atan2(1.,T) ;
      m_p[5] = 1./m_pTmin  ;
  }
  
  if(fabs(m_p[5])*m_pTmin > 1.1) return 0;
  m_p[4] = m_p[5]/sqrt(1.+T*T);
  m_p[6] = x0                              ;
  m_p[7] = y0                              ;
  m_p[8] = z0                              ;

  if(sss && !isHadCaloCompatible()) return 0;

  return pla->createTrackParameters(m_p[0],m_p[1],m_p[2],m_p[3],m_p[4],0); 
}

///////////////////////////////////////////////////////////////////
// Space point seed parameters extimation for DBM seed
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters* InDet::SiTrackMaker_xk::getAtaPlaneDBM
(const std::list<const Trk::SpacePoint*>& SP)
{
  std::list<const Trk::SpacePoint*>::const_iterator is=SP.begin(),ise=SP.end(),is0,is1,is2;  
  if(is==ise) return 0;

  const Trk::PrepRawData*       cl  = (*is)->clusterList().first;
  if(!cl) return 0;
  const Trk::PlaneSurface*      pla = 
    static_cast<const Trk::PlaneSurface*>(&cl->detectorElement()->surface());
  if(!pla) return 0;

  is0 = is; if(++is==ise) return 0; 
  is1 = is; if(++is==ise) return 0;
  is2 = is ;
  double p0[3],p1[3],p2[3]; 
  if(!globalPositions((*is0),(*is1),(*is2),p0,p1,p2)) return 0;

  double x0 = m_xybeam[0]-p0[0];
  double y0 = m_xybeam[1]-p0[1];
  double x2 = p2[0]      -p0[0];
  double y2 = p2[1]      -p0[1];
  double z2 = p2[2]      -p0[2];

  double u1 = -1./sqrt(x0*x0+y0*y0);
  double rn = x2*x2+y2*y2          ;
  double r2 = 1./rn                ;
  double a  =  x0*u1               ;
  double b  =  y0*u1               ;
  double u2 = (a*x2+b*y2)*r2       ;
  double v2 = (a*y2-b*x2)*r2       ;
  double A  = v2/(u2-u1)           ;
  double B  = 2.*(v2-A*u2)         ;
  double C  = B/sqrt(1.+A*A)       ;
  double T  = z2*sqrt(r2)          ;
  
  const Amg::Transform3D& Tp = pla->transform();

  double Ax[3] = {Tp(0,0),Tp(1,0),Tp(2,0)}; 
  double Ay[3] = {Tp(0,1),Tp(1,1),Tp(2,1)}; 
  double D [3] = {Tp(0,3),Tp(1,3),Tp(2,3)}; 
  
  double   d[3] = {p0[0]-D[0],p0[1]-D[1],p0[2]-D[2]};

  m_p[0] = d[0]*Ax[0]+d[1]*Ax[1]+d[2]*Ax[2];
  m_p[1] = d[0]*Ay[0]+d[1]*Ay[1]+d[2]*Ay[2];

  if(m_fieldprop.magneticFieldMode() > 0) {

    double H[3],gP[3] ={p0[0],p0[1],p0[2]}; m_fieldService->getFieldZR(gP,H);

    if(fabs(H[2])>.0001) {
      m_p[2] = atan2(b+a*A,a-b*A);
      m_p[3] = atan2(1.,T)       ;  
      m_p[5] = -C/(300.*H[2])    ;
    }
    else {
      T    =  z2*sqrt(r2)  ;
      m_p[2] = atan2(y2,x2);
      m_p[3] = atan2(1.,T) ;
      m_p[5] = 1./1000.    ;
    }
  }
  else {
      T    = z2*sqrt(r2)   ;
      m_p[2] = atan2(y2,x2);
      m_p[3] = atan2(1.,T) ;
      m_p[5] = 1./1000     ;
  }

  if(fabs(m_p[5])*20. > 1.1) return 0;
  m_p[4] = m_p[5]/sqrt(1.+T*T);
  m_p[6] = p0[0]                           ;
  m_p[7] = p0[1]                           ;
  m_p[8] = p0[2]                           ;
  return pla->createTrackParameters(m_p[0],m_p[1],m_p[2],m_p[3],m_p[4],0); 
}

///////////////////////////////////////////////////////////////////
// Set track quality cuts
///////////////////////////////////////////////////////////////////

void  InDet::SiTrackMaker_xk::setTrackQualityCuts()
{
  // Integer cuts
  //
  m_trackquality.setIntCut   ("MinNumberOfClusters" ,m_nclusmin   );
  m_trackquality.setIntCut   ("MinNumberOfWClusters",m_nwclusmin  );
  m_trackquality.setIntCut   ("MaxNumberOfHoles"    ,m_nholesmax  ); 
  m_trackquality.setIntCut   ("MaxHolesGae"         ,m_dholesmax  ); 

  if( m_useassoTool ) m_trackquality.setIntCut   ("UseAssociationTool",1);
  else                m_trackquality.setIntCut   ("UseAssociationTool",0);
  if( m_cosmicTrack ) m_trackquality.setIntCut   ("CosmicTrack"       ,1); 
  else                m_trackquality.setIntCut   ("CosmicTrack"       ,0);
  if(m_simpleTrack  ) m_trackquality.setIntCut   ("SimpleTrack"       ,1);
  else                m_trackquality.setIntCut   ("SimpleTrack"       ,0);
  if(m_multitracks  ) m_trackquality.setIntCut   ("doMultiTracksProd" ,1);
  else                m_trackquality.setIntCut   ("doMultiTracksProd" ,0);

  // Double cuts
  //
  m_trackquality.setDoubleCut("pTmin"              ,m_pTmin      );
  m_trackquality.setDoubleCut("pTminBrem"          ,m_pTminBrem  );
  m_trackquality.setDoubleCut("MaxXi2forCluster"   ,m_xi2max     );
  m_trackquality.setDoubleCut("MaxXi2forOutlier"   ,m_xi2maxNoAdd);
  m_trackquality.setDoubleCut("MaxXi2forSearch"    ,m_xi2maxlink );
  m_trackquality.setDoubleCut("MaxXi2MultiTracks"  ,m_xi2multitracks);
}

///////////////////////////////////////////////////////////////////
// Detector elements selection
///////////////////////////////////////////////////////////////////

void InDet::SiTrackMaker_xk::detectorElementsSelection(std::vector<const InDetDD::SiDetectorElement*>& DE)
{
  std::vector<const InDetDD::SiDetectorElement*>::iterator d = DE.begin();
  if(!m_dbm) {

    while(d!=DE.end()) {

      if     ((*d)->isPixel()) {if(!m_pix) {DE.erase(d++); continue;}}
      else if(   !m_sct      ) {            DE.erase(d++); continue; }
      ++d;
    }
  }
  else      {
    while(d!=DE.end()) {

      if(!(*d)->isDBM() ) {

	if((*d)->isSCT() || (*d)->isEndcap())       {DE.erase(d++); continue;}
	const Amg::Transform3D& T  = (*d)->surface().transform();	
	if(T(0,3)*T(0,3)+T(1,3)*T(1,3) > (43.*43) ) {DE.erase(d++); continue;}

      }
      ++d;
    }
  }
} 

///////////////////////////////////////////////////////////////////
// Callback function - get the magnetic field /
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiTrackMaker_xk::magneticFieldInit(IOVSVC_CALLBACK_ARGS) 
{
  // Build MagneticFieldProperties 
  //
  if(!m_fieldService->solenoidOn()) m_fieldmode ="NoField"; magneticFieldInit();
  return StatusCode::SUCCESS;
}

void InDet::SiTrackMaker_xk::magneticFieldInit() 
{
  Trk::MagneticFieldProperties* pMF = 0;
  if     (m_fieldmode == "NoField"    ) pMF = new Trk::MagneticFieldProperties(Trk::NoField  );
  else if(m_fieldmode == "MapSolenoid") pMF = new Trk::MagneticFieldProperties(Trk::FastField);
  else                                  pMF = new Trk::MagneticFieldProperties(Trk::FullField);
  m_fieldprop = *pMF; delete pMF;
}

///////////////////////////////////////////////////////////////////
// New clusters comparison with clusters associated with track
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::newSeed(const std::list<const Trk::SpacePoint*>& Sp)
{
  std::multiset<const Trk::Track*> trackseed;
  std::multimap<const Trk::PrepRawData*,const Trk::Track*>::const_iterator pt,pte = m_clusterTrack.end();
  std::list<const Trk::SpacePoint*>::const_iterator s=Sp.begin(),se=Sp.end();

  int n = 0;
  for(s = Sp.begin(); s!=se; ++s) {

    const Trk::PrepRawData* p = (*s)->clusterList().first; 
   
    for(pt = m_clusterTrack.find(p); pt!=pte; ++pt) {if((*pt).first!=p) break; trackseed.insert((*pt).second);} ++n;

    p = (*s)->clusterList().second; if(!p) continue;

    for(pt = m_clusterTrack.find(p); pt!=pte; ++pt) {if((*pt).first!=p) break; trackseed.insert((*pt).second);} ++n;

  }
  if(trackseed.empty()) return true;

  if(m_ITKGeomtry && n!=3 && n!=6) return false;
 
  std::multiset<const Trk::Track*>::iterator t = trackseed.begin(), te = trackseed.end();

  const Trk::Track* tr  = (*t)                             ;
  unsigned int      nsm = tr->measurementsOnTrack()->size();
  unsigned int      nsm3= 0                                ;
  int               nt  = 1                                ;
  int               t3  = 0                                ;

  for(++t; t!=te; ++t) {

    if((*t) == tr) {++nt; continue;}
    if (nt  == n ) {++t3; unsigned int ns =  tr->measurementsOnTrack()->size(); if(ns > nsm3) nsm3 = ns;} 
    tr = (*t); nt = 1;
    unsigned int ns = tr->measurementsOnTrack()->size(); if(ns > nsm) nsm = ns;
  }
  if(nt == n) {++t3; unsigned int ns =  tr->measurementsOnTrack()->size(); if(ns > nsm3) nsm3 = ns;}

  if(m_heavyion) {if(n==3 || t3 <=0) return true; return false;}

  if( (m_ITKGeomtry && t3 > 0) || nsm3 > 13 || t3 > 2) return false;

  if( !m_cosmicTrack && n==3 && m_sct && (*Sp.begin())->r() > 43. ) return true;
  if(t3 > 0) return false;
  return true;
}

///////////////////////////////////////////////////////////////////
// Clusters-track multimap production
///////////////////////////////////////////////////////////////////

void  InDet::SiTrackMaker_xk::clusterTrackMap(Trk::Track* Tr)
{
  DataVector<const Trk::MeasurementBase>::const_iterator 
    m  = Tr->measurementsOnTrack()->begin(), 
    me = Tr->measurementsOnTrack()->end  ();

  for(; m!=me; ++m) {
    const Trk::PrepRawData* prd = ((const Trk::RIO_OnTrack*)(*m))->prepRawData();
    if(prd) m_clusterTrack.insert(std::make_pair(prd,Tr));
  }
}
 
///////////////////////////////////////////////////////////////////
// Test is it new track
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::isNewTrack(Trk::Track* Tr)
{
  const Trk::PrepRawData* prd   [100];
  std::multimap<const Trk::PrepRawData*,const Trk::Track*>::const_iterator 
    ti,t[100],te = m_clusterTrack.end();

  int     n   = 0    ;

  DataVector<const Trk::MeasurementBase>::const_iterator 
    m  = Tr->measurementsOnTrack()->begin(), 
    me = Tr->measurementsOnTrack()->end  ();

  for(; m!=me; ++m) {
  
    const Trk::PrepRawData* pr = ((const Trk::RIO_OnTrack*)(*m))->prepRawData();
    if(pr) {
      prd[n] =pr;
      t  [n] = m_clusterTrack.find(prd[n]); if(t[n]==te) return true; ++n;
    }
  }

  if(!n) return true;
  int nclt = n;
  
  for(int i=0; i!=n; ++i) {

    int nclmax = 0;

    for(ti=t[i]; ti!=te; ++ti) {

      if( (*ti).first != prd[i] ) break;
      int ncl = (*ti).second->measurementsOnTrack()->size();
      if(ncl > nclmax) nclmax = ncl;
    }   
    if(nclt > nclmax) return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// Calculation global position for space points
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::globalPositions
(const Trk::SpacePoint* s0,const Trk::SpacePoint* s1,const Trk::SpacePoint* s2,
 double* p0,double* p1,double* p2)
{

  p0[0] = s0->globalPosition().x(); 
  p0[1] = s0->globalPosition().y();
  p0[2] = s0->globalPosition().z();

  p1[0] = s1->globalPosition().x(); 
  p1[1] = s1->globalPosition().y(); 
  p1[2] = s1->globalPosition().z(); 

  p2[0] = s2->globalPosition().x(); 
  p2[1] = s2->globalPosition().y(); 
  p2[2] = s2->globalPosition().z(); 
 
  if(!s0->clusterList().second && !s1->clusterList().second && !s2->clusterList().second) return true;

  double dir0[3],dir1[3],dir2[3]; 

  globalDirections(p0,p1,p2,dir0,dir1,dir2);

  if(s0->clusterList().second && !globalPosition(s0,dir0,p0)) return false;
  if(s1->clusterList().second && !globalPosition(s1,dir1,p1)) return false;
  if(s2->clusterList().second && !globalPosition(s2,dir2,p2)) return false;
  
  return true;
}

///////////////////////////////////////////////////////////////////
// Calculation global position for space points
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::globalPosition
(const Trk::SpacePoint* sp, double* dir,double* p)
{
  const Trk::PrepRawData*  c0  = sp->clusterList().first;
  const Trk::PrepRawData*  c1  = sp->clusterList().second;
 
  const InDetDD::SiDetectorElement* de0 = static_cast<const InDet::SiCluster*>(c0)->detectorElement(); 
  const InDetDD::SiDetectorElement* de1 = static_cast<const InDet::SiCluster*>(c1)->detectorElement(); 

  Amg::Vector2D localPos = c0->localPosition();  
  std::pair<Amg::Vector3D,Amg::Vector3D> e0
    (de0->endsOfStrip(InDetDD::SiLocalPosition(localPos.y(),localPos.x(),0.))); 
 
  localPos = c1->localPosition();                
  std::pair<Amg::Vector3D,Amg::Vector3D> e1
    (de1->endsOfStrip(InDetDD::SiLocalPosition(localPos.y(),localPos.x(),0.))); 
 
  double a0[3] = {e0.second.x()-e0.first.x(), e0.second.y()-e0.first.y(), e0.second.z()-e0.first.z()};
  double a1[3] = {e1.second.x()-e1.first.x(), e1.second.y()-e1.first.y(), e1.second.z()-e1.first.z()};
  double dr[3] = {e1.first .x()-e0.first.x(), e1.first .y()-e0.first.y(), e1.first .z()-e0.first.z()};
  
  double d0    = m_distmax/sqrt(a0[0]*a0[0]+a0[1]*a0[1]+a0[2]*a0[2]);

  // u = a1 x dir and  v = a0 x dir
  //
  double u[3]  = {a1[1]*dir[2]-a1[2]*dir[1],a1[2]*dir[0]-a1[0]*dir[2],a1[0]*dir[1]-a1[1]*dir[0]};
  double v[3]  = {a0[1]*dir[2]-a0[2]*dir[1],a0[2]*dir[0]-a0[0]*dir[2],a0[0]*dir[1]-a0[1]*dir[0]};

  double du    = a0[0]*u[0]+a0[1]*u[1]+a0[2]*u[2];
  double dv    = a1[0]*v[0]+a1[1]*v[1]+a1[2]*v[2];

  if(du==0. || dv==0.) return false;

  double s0 = (dr[0]*u[0]+dr[1]*u[1]+dr[2]*u[2])/du;
  double s1 =-(dr[0]*v[0]+dr[1]*v[1]+dr[2]*v[2])/dv;

  if(s0 < -d0 || s0 > 1.+d0 ||  s1 < -d0 || s1 > 1.+d0) return false;

  p[0] = e0.first.x()+s0*a0[0]; 
  p[1] = e0.first.y()+s0*a0[1]; 
  p[2] = e0.first.z()+s0*a0[2];

  return true;
}

///////////////////////////////////////////////////////////////////
// Test is it track with calo seed compatible
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::isCaloCompatible()
{
  const double pi = M_PI, pi2 = 2.*M_PI;

  if(m_caloF.empty()) return false;   

  std::list<double>::iterator f = m_caloF.begin(), fe = m_caloF.end();
  std::list<double>::iterator r = m_caloR.begin();
  std::list<double>::iterator z = m_caloZ.begin();

  double F = m_p[2]                           ;
  double E = -log(tan(.5*m_p[3]))             ;
  double R = sqrt(m_p[6]*m_p[6]+m_p[7]*m_p[7]);
  double Z = m_p[8]                           ;                           
  
  for(; f!=fe; ++f) {
    double df = fabs(F-(*f));
    if(df > pi        ) df = fabs(pi2-df);
    if(df < m_phiWidth) {
      
      double dR = (*r)-R;
      double dZ = (*z)-Z;
      if(fabs(E-atanh(dZ/sqrt(dR*dR+dZ*dZ)) ) < m_etaWidth) return true;
    }
    ++r; ++z;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// Test track is compatible withi had calo seed
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::isHadCaloCompatible()
{
  const double pi = M_PI, pi2 = 2.*M_PI;

  if(m_hadF.empty() || fabs(m_p[5])*m_pTminSSS > 1.) return false;   

  std::list<double>::iterator f = m_hadF.begin(), fe = m_hadF.end();
  std::list<double>::iterator r = m_hadR.begin();
  std::list<double>::iterator z = m_hadZ.begin();

  double F = m_p[2]                           ;
  double E = -log(tan(.5*m_p[3]))             ;
  double R = sqrt(m_p[6]*m_p[6]+m_p[7]*m_p[7]);
  double Z = m_p[8]                           ;                           
  
  for(; f!=fe; ++f) {
    double df = fabs(F-(*f));
    if(df > pi        ) df = fabs(pi2-df);
    if(df < m_phiWidth) {
      
      double dR = (*r)-R;
      double dZ = (*z)-Z;
      if(fabs(E-atanh(dZ/sqrt(dR*dR+dZ*dZ)) ) < m_etaWidth) return true;
    }
    ++r; ++z;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// Test is it DBM seed
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::isDBMSeeds(const Trk::SpacePoint* s)
{
  const InDetDD::SiDetectorElement* de= 
    static_cast<const InDetDD::SiDetectorElement*>(s->clusterList().first->detectorElement());
  return de && de->isDBM();
}

///////////////////////////////////////////////////////////////////
// Calculation global direction for positions of space points
///////////////////////////////////////////////////////////////////

void InDet::SiTrackMaker_xk::globalDirections
(double* p0,double* p1,double* p2,double* d0,double* d1,double* d2)
{
  double x01 = p1[0]-p0[0]      ;
  double y01 = p1[1]-p0[1]      ;
  double x02 = p2[0]-p0[0]      ;
  double y02 = p2[1]-p0[1]      ;

  double d01 = x01*x01+y01*y01  ; 
  double x1  = sqrt(d01)        ;
  double u01 = 1./x1            ;
  double a   = x01*u01          ; 
  double b   = y01*u01          ;
  double x2  = a*x02+b*y02      ;
  double y2  = a*y02-b*x02      ;
  double d02 = x2*x2+y2*y2      ;
  double u02 = x2/d02           ;
  double v02 = y2/d02           ;

  double A0  =  v02 /(u02-u01)  ;
  double B0  = 2.*(v02-A0*u02)  ;

  double C2  = (1.-B0*y2)       ;
  double S2  = (A0+B0*x2)       ;

  double T   = (p2[2]-p0[2])/sqrt(d02);
  double Se  = 1./sqrt(1.+T*T)        ;
  double Ce  = Se*T                   ;
  Se        /= sqrt(1.+A0*A0)         ;
  double Sa  = Se*a                   ;
  double Sb  = Se*b                   ;

  d0[0] = Sa   -Sb*A0; d0[1]= Sa*A0+Sb   ; d0[2]=Ce;  
  d1[0] = Sa   +Sb*A0; d1[1]= Sb   -Sa*A0; d1[2]=Ce;  
  d2[0] = Sa*C2-Sb*S2; d2[1]= Sa*S2+Sb*C2; d2[2]=Ce;  
}

