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

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "SiTrackMakerTool_xk/SiTrackMaker_xk.h"

#include "TrkCaloClusterROI/CaloClusterROI.h"
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiTrackMaker_xk::SiTrackMaker_xk
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p)                                        ,
    m_fieldServiceHandle("AtlasFieldSvc",n), 
    m_roadmaker   ("InDet::SiDetElementsRoadMaker_xk"    ),
    m_tracksfinder("InDet::SiCombinatorialTrackFinder_xk")
{
  m_fieldmode    = "MapSolenoid"      ;
  m_patternName  = "SiSPSeededFinder" ; 
  m_pix          = false              ;
  m_sct          = false              ;
  m_useassoTool  = false              ;
  m_cosmicTrack  = false              ;    
  m_simpleTrack  = false              ;
  m_multitracks  = false              ;
  m_useBremModel = false              ;
  m_useCaloSeeds = false              ;
  m_useSSSfilter = true               ; 
  m_xi2max       = 15.                ;
  m_xi2maxNoAdd  = 35.                ;
  m_xi2maxlink   = 200.               ;
  m_xi2multitracks = 3.               ;
  m_pTmin        = 500.               ;
  m_pTminBrem    = 1000.              ;
  m_distmax      = 20.                ;
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
  m_inputClusterContainerName = "InDetCaloClusterROIs";

  declareInterface<ISiTrackMaker>(this);

  declareProperty("RoadTool"                ,m_roadmaker   );
  declareProperty("CombinatorialTrackFinder",m_tracksfinder);
  declareProperty("Xi2max"                  ,m_xi2max      );
  declareProperty("Xi2maxNoAdd"             ,m_xi2maxNoAdd );
  declareProperty("Xi2maxlink"              ,m_xi2maxlink  );
  declareProperty("pTmin"                   ,m_pTmin       );
  declareProperty("pTminBrem"               ,m_pTminBrem   );
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
  declareProperty("phiWidth"                ,m_phiWidth    );
  declareProperty("etaWidth"                ,m_etaWidth    );
  declareProperty("InputClusterContainerName",m_inputClusterContainerName);
  declareProperty("MagFieldSvc"             , m_fieldServiceHandle);
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

  // TrackpatternRecoInfo preparation 
  //
  if     (m_patternName == "SiSpacePointsSeedMaker_Cosmic"     )  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_Cosmic     );
  }
  else if(m_patternName == "SiSpacePointsSeedMaker_HeavyIon"   )  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_HeavyIon   ); 
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
  else                                                            {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSPSeededFinder                  );
  } 

  if(m_cosmicTrack)  m_seedsfilter = 3;
  
  // Setup for magnetic field
  //
  magneticFieldInit();

  m_distmax = m_distmax*m_distmax;
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
  if(m_nprint)  return dumpevent(out); return dumpconditions(out);
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
  m_pix          = PIX;
  m_sct          = SCT;
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
    m_caloE.clear();

    const CaloClusterROI_Collection* calo = 0;    
    StatusCode sc = evtStore()->retrieve(calo,m_inputClusterContainerName);
  
    if(sc == StatusCode::SUCCESS && calo) {

      CaloClusterROI_Collection::const_iterator c = calo->begin(), ce = calo->end();

      for(; c!=ce; ++c) {
	double x = (*c)->globalPosition().x();
	double y = (*c)->globalPosition().y();
	double z = (*c)->globalPosition().z();
	m_caloF.push_back(atan2(y,x));
	m_caloE.push_back(atan2(1.,z/sqrt(x*x+y*y)));
      }
    }
  }
}

///////////////////////////////////////////////////////////////////
// Initiate track finding tool for new event
///////////////////////////////////////////////////////////////////

void InDet::SiTrackMaker_xk::newTrigEvent(bool PIX,bool SCT)
{
  m_pix          = PIX;
  m_sct          = SCT;
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
 
  // Print event information 
  //
  if (outputLevel()<=0) {
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
  m_tracks.erase(m_tracks.begin(),m_tracks.end());
  if(!m_pix && !m_sct) return m_tracks;
  
  bool good;  
  !m_seedsfilter ? good=true : m_seedsfilter==1 ? good=newClusters(Sp) : good=newSeed(Sp);  

  if(!good) return m_tracks;
  
  // Get initial parameters estimation
  // 
  const Trk::TrackParameters* Tp = getAtaPlane(Sp);
  if(!Tp) return m_tracks;
  ++m_goodseeds;

  // Get detector elements road
  //
  std::list<const InDetDD::SiDetectorElement*> DE;
  if(!m_cosmicTrack) m_roadmaker->detElementsRoad(*Tp,Trk::alongMomentum,   DE);
  else               m_roadmaker->detElementsRoad(*Tp,Trk::oppositeMomentum,DE);

  if(!m_pix || !m_sct) detectorElementsSelection(DE);

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
  else if(isCaloCompatible(*Tp)) {
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
  m_tracks.erase(m_tracks.begin(),m_tracks.end());
  if(!m_pix && !m_sct) return m_tracks;

  ++m_goodseeds;

  // Get detector elements road
  //
  std::list<const InDetDD::SiDetectorElement*> DE;
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
  else if(isCaloCompatible(Tp) ) {
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
(const std::list<const Trk::SpacePoint*>& SP)
{
  std::list<const Trk::SpacePoint*>::const_iterator is=SP.begin(),ise=SP.end(),is0,is1,is2;  
  if(is==ise) return 0;

  const Trk::PrepRawData*       cl  = (*is)->clusterList().first;
  if(!cl) return 0;
  const Trk::PlaneSurface*      pla = 
    dynamic_cast<const Trk::PlaneSurface*>(&cl->detectorElement()->surface());
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

  double p[5];
  p[0] = d[0]*Ax[0]+d[1]*Ax[1]+d[2]*Ax[2];
  p[1] = d[0]*Ay[0]+d[1]*Ay[1]+d[2]*Ay[2];

  if(m_fieldprop.magneticFieldMode() > 0) {

    double H[3],gP[3] ={x0,y0,z0}; m_fieldService->getFieldZR(gP,H);

    if(fabs(H[2])>.0001) {
      p[2] = atan2(b+a*A,a-b*A);
      p[3] = atan2(1.,T)       ;  
      double pTi =-C/(300.*H[2]); if(fabs(pTi)*m_pTmin > 1.1) return 0;
      p[4] = pTi/sqrt(1.+T*T)  ;
    }
    else {
      T    =  z2*sqrt(r2);
      p[2] = atan2(y2,x2);
      p[3] = atan2(1.,T) ;
      p[4] = 1./(m_pTmin*sqrt(1.+T*T)); 
    }
  }
  else {
      T    = z2*sqrt(r2) ;
      p[2] = atan2(y2,x2);
      p[3] = atan2(1.,T) ;
      p[4] = 1./(m_pTmin*sqrt(1.+T*T)); 
  }
  return pla->createTrackParameters(p[0],p[1],p[2],p[3],p[4],0); 
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

void InDet::SiTrackMaker_xk::detectorElementsSelection(std::list<const InDetDD::SiDetectorElement*>& DE)
{
  std::list<const InDetDD::SiDetectorElement*>::iterator d = DE.begin();

  while(d!=DE.end()) {

    if     ((*d)->isPixel()) {if(!m_pix) {DE.erase(d++); continue;}}
    else if(   !m_sct      ) {            DE.erase(d++); continue; }
    ++d;
  }

} 

///////////////////////////////////////////////////////////////////
// Callback function - get the magnetic field /
///////////////////////////////////////////////////////////////////

void  InDet::SiTrackMaker_xk::magneticFieldInit() 
{
  // Build MagneticFieldProperties 
  //
  Trk::MagneticFieldProperties* pMF = 0;
  if     (m_fieldmode == "NoField"    ) pMF = new Trk::MagneticFieldProperties(Trk::NoField  );
  else if(m_fieldmode == "MapSolenoid") pMF = new Trk::MagneticFieldProperties(Trk::FastField);
  else                                  pMF = new Trk::MagneticFieldProperties(Trk::FullField);
  m_fieldprop = *pMF; delete pMF;
}

///////////////////////////////////////////////////////////////////
// New clusters comparison with clusters associated with track
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::newClusters(const std::list<const Trk::SpacePoint*>& Sp)
{
  const Trk::PrepRawData* prd   [ 40];
  const Trk::Track*       trk[2][200];
  std::multimap<const Trk::PrepRawData*,const Trk::Track*>::const_iterator 
    t[40],te = m_clusterTrack.end();

  std::list<const Trk::SpacePoint*>::const_iterator s=Sp.begin(),se=Sp.end();
  int n = 0;
  for(; s!=se; ++s) {

     if((*s)->clusterList().first ) {
       prd[n] = (*s)->clusterList().first;
       t  [n] = m_clusterTrack.find(prd[n]); if(t[n]==te) return true; ++n;
     }
     if((*s)->clusterList().second) {
       prd[n] = (*s)->clusterList().second;
       t  [n] = m_clusterTrack.find(prd[n]); if(t[n]==te) return true; ++n;
     }
     if(n==40) break;
  }
  if(!n) return false;

  int m = 0;
  for(; t[0]!=te; ++t[0]) {
    if( (*t[0]).first != prd[0] ) break; trk[0][m++] = (*t[0]).second; 
    if(m==200) break;
  } 
  
  int in=0, ou=1;

  for(int i=1; i!=n; ++i) {

    int l = 0;
    for(; t[i]!=te; ++t[i]) {

      if( (*t[i]).first != prd[i] ) break;

      for(int j=0; j!=m; ++j) {
	if((*t[i]).second == trk[in][j]) {trk[ou][l++]= trk[in][j]; break;}
      }
    }
    if(l==0) return true;
    m=l; if(in==0) {in=1; ou=0;} else {in=0; ou=1;}
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// New clusters comparison with clusters associated with track
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::newSeed(const std::list<const Trk::SpacePoint*>& Sp)
{
  const Trk::PrepRawData* prd   [ 40];
  const Trk::Track*       trk[2][200];
  std::multimap<const Trk::PrepRawData*,const Trk::Track*>::const_iterator 
    tt,t[40],te = m_clusterTrack.end();

  std::list<const Trk::SpacePoint*>::const_iterator s=Sp.begin(),se=Sp.end();
  int n  = 0;
  int nc = 0;
  for(; s!=se; ++s) {

     if((*s)->clusterList().first ) {
       prd[n] = (*s)->clusterList().first;
       t  [n] = m_clusterTrack.find(prd[n]); if(t[n]!=te) ++nc; ++n;
     }
     if((*s)->clusterList().second) {
       prd[n] = (*s)->clusterList().second;
       t  [n] = m_clusterTrack.find(prd[n]); if(t[n]!=te) ++nc; ++n; 
     }
     if(n==40) break;
  }

  if(!nc || (!m_cosmicTrack && n==3)) return true; 
  if(n==nc) {

    int m = 0;
    for(tt=t[0]; tt!=te; ++tt) {
      if( (*tt).first != prd[0] ) break; trk[0][m++] = (*tt).second;
      if(m == 200) break;
    } 
    
    int in=0, ou=1, i=1;

    for(; i!=n; ++i) {

      int l = 0;
      for(tt=t[i]; t[i]!=te; ++tt) {
	if( (*tt).first != prd[i] ) break;
	for(int j=0; j!=m; ++j) {
	  if((*tt).second == trk[in][j]) {trk[ou][l++]= trk[in][j]; break;}
	}
      }
      if(l==0) break;
      m=l; if(in==0) {in=1; ou=0;} else {in=0; ou=1;}
    }
    if(i==n) return false;
  }

  if(n==6 && !m_useSSSfilter) return true;

  int h = 0;
  for(int i=0; i!=n; ++i) {

    for(tt=t[i]; t[i]!=te; ++tt) {
      if( (*tt).first != prd[i] ) break;
      if((*tt).second->measurementsOnTrack()->size() >= m_wrongcluster) {++h; break;}
    }
  }
  if(h) return false; return true;
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
  double dir0[3],dir1[3];

  p0[0] = s0->globalPosition().x(); 
  p0[1] = s0->globalPosition().y();
  p0[2] = s0->globalPosition().z();

  p1[0] = s1->globalPosition().x(); dir0[0] = p1[0]-p0[0];
  p1[1] = s1->globalPosition().y(); dir0[1] = p1[1]-p0[1];
  p1[2] = s1->globalPosition().z(); dir0[2] = p1[2]-p0[2];

  p2[0] = s2->globalPosition().x(); dir1[0] = p2[0]-p1[0];
  p2[1] = s2->globalPosition().y(); dir1[1] = p2[1]-p1[1];
  p2[2] = s2->globalPosition().z(); dir1[2] = p2[2]-p1[2];

  if(s0->clusterList().second) {

    double p[3] = {p0[0],p0[1],p0[2]}; globalPosition(s0,dir0     ,p0);
    if(!m_cosmicTrack) {
      double dx   = p0[0]-p[0];
      double dy   = p0[1]-p[1];
      double dz   = p0[2]-p[2];
      if(dx*dx+dy*dy+dz*dz > m_distmax) return false;
    }
  }
  if(s1->clusterList().second) {

    double p[3] = {p1[0],p1[1],p1[2]}; globalPosition(s1,dir0,dir1,p1);
    if(!m_cosmicTrack) {
      double dx   = p1[0]-p[0];
      double dy   = p1[1]-p[1];
      double dz   = p1[2]-p[2];
      if(dx*dx+dy*dy+dz*dz > m_distmax) return false;
    }
  }
  if(s2->clusterList().second) {

    double p[3] = {p2[0],p2[1],p2[2]}; globalPosition(s2,     dir1,p2);
    if(!m_cosmicTrack) {
      double dx   = p2[0]-p[0];
      double dy   = p2[1]-p[1];
      double dz   = p2[2]-p[2];
      if(dx*dx+dy*dy+dz*dz > m_distmax) return false;
    }
  }
  return true;
}

///////////////////////////////////////////////////////////////////
// Calculation global position for space points
///////////////////////////////////////////////////////////////////

void InDet::SiTrackMaker_xk::globalPosition
(const Trk::SpacePoint* sp, double* dir,double* p)
{
  const Trk::PrepRawData*  c0  = sp->clusterList().first;
  const Trk::PrepRawData*  c1  = sp->clusterList().second;

  const InDet::SiCluster* s0 = dynamic_cast<const InDet::SiCluster*>(c0);
  const InDet::SiCluster* s1 = dynamic_cast<const InDet::SiCluster*>(c1);
  if(!s0  || !s1 ) {p[0]=0.; p[1]=0.; p[2]=0.; return;}
  const InDetDD::SiDetectorElement* de0 = s0->detectorElement();
  const InDetDD::SiDetectorElement* de1 = s1->detectorElement();
  if(!de0 || !de1) {p[0]=0.; p[1]=0.; p[2]=0.; return;}

  Amg::Vector2D localPos = c0->localPosition();  
  std::pair<Amg::Vector3D,Amg::Vector3D> ends0
    (de0->endsOfStrip(InDetDD::SiLocalPosition(localPos.y(),localPos.x(),0))); 
 
  localPos = c1->localPosition();                
  std::pair< Amg::Vector3D,Amg::Vector3D> ends1
    (de1->endsOfStrip(InDetDD::SiLocalPosition(localPos.y(),localPos.x(),0))); 
 
  double r0[3] = {ends0.first .x()      ,ends0.first .y()      ,ends0.first .z()      };
  double r1[3] = {ends1.first .x()      ,ends1.first .y()      ,ends1.first .z()      };

  double a0[3] = {ends0.second.x()-r0[0],ends0.second.y()-r0[1],ends0.second.z()-r0[2]};
  double a1[3] = {ends1.second.x()-r1[0],ends1.second.y()-r1[1],ends1.second.z()-r1[2]};

  // u = a1 x a
  //
  double u[3]  = {a1[1]*dir[2]-a1[2]*dir[1],a1[2]*dir[0]-a1[0]*dir[2],a1[0]*dir[1]-a1[1]*dir[0]};
  double du    = a0[0]*u[0]+a0[1]*u[1]+a0[2]*u[2];

  if(du!=0.) {
    double step = ((r1[0]-r0[0])*u[0]+(r1[1]-r0[1])*u[1]+(r1[2]-r0[2])*u[2])/du;
    p[0] = r0[0]+step*a0[0]; 
    p[1] = r0[1]+step*a0[1]; 
    p[2] = r0[2]+step*a0[2];
    return;
  }
  p[0] = sp->globalPosition().x(); 
  p[1] = sp->globalPosition().y();
  p[2] = sp->globalPosition().z();
}

///////////////////////////////////////////////////////////////////
// Calculation global position for space points
///////////////////////////////////////////////////////////////////

void InDet::SiTrackMaker_xk::globalPosition
(const Trk::SpacePoint* sp, double* dir0,double* dir1,double* p)
{
  const Trk::PrepRawData*  c0  = sp->clusterList().first;
  const Trk::PrepRawData*  c1  = sp->clusterList().second;

  const InDet::SiCluster* s0 = dynamic_cast<const InDet::SiCluster*>(c0);
  const InDet::SiCluster* s1 = dynamic_cast<const InDet::SiCluster*>(c1);

  const InDetDD::SiDetectorElement* de0 = s0->detectorElement();
  const InDetDD::SiDetectorElement* de1 = s1->detectorElement();


  Amg::Vector2D localPos = c0->localPosition();  
  std::pair<Amg::Vector3D,Amg::Vector3D> ends0
    (de0->endsOfStrip(InDetDD::SiLocalPosition(localPos.y(),localPos.x(),0))); 
 
  localPos = c1->localPosition();  
  std::pair<Amg::Vector3D, Amg::Vector3D> ends1
    (de1->endsOfStrip(InDetDD::SiLocalPosition(localPos.y(),localPos.x(),0))); 
 
  double r0[3] = {ends0.first .x()      ,ends0.first .y()      ,ends0.first .z()      };
  double r1[3] = {ends1.first .x()      ,ends1.first .y()      ,ends1.first .z()      };

  double a0[3] = {ends0.second.x()-r0[0],ends0.second.y()-r0[1],ends0.second.z()-r0[2]};
  double a1[3] = {ends1.second.x()-r1[0],ends1.second.y()-r1[1],ends1.second.z()-r1[2]};

  // u = a1 x a
  //
  double u[3]  = {a1[1]*dir0[2]-a1[2]*dir0[1],a1[2]*dir0[0]-a1[0]*dir0[2],a1[0]*dir0[1]-a1[1]*dir0[0]};
  double v[3]  = {a1[1]*dir1[2]-a1[2]*dir1[1],a1[2]*dir1[0]-a1[0]*dir1[2],a1[0]*dir1[1]-a1[1]*dir1[0]};

  double du    = (a0[0]*u[0]+a0[1]*u[1]+a0[2]*u[2]);
  double dv    = (a0[0]*v[0]+a0[1]*v[1]+a0[2]*v[2]);

  if(du!=0. && dv!=0.) {
    double step0 = ((r1[0]-r0[0])*u[0]+(r1[1]-r0[1])*u[1]+(r1[2]-r0[2])*u[2])/du;
    double step1 = ((r1[0]-r0[0])*v[0]+(r1[1]-r0[1])*v[1]+(r1[2]-r0[2])*v[2])/dv;
    double step  = .5*(step0+step1);
    p[0] = r0[0]+step*a0[0]; 
    p[1] = r0[1]+step*a0[1]; 
    p[2] = r0[2]+step*a0[2];
    return;
  }
  p[0] = sp->globalPosition().x(); 
  p[1] = sp->globalPosition().y();
  p[2] = sp->globalPosition().z();
}

///////////////////////////////////////////////////////////////////
// Test is it track with calo seed compatible
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::isCaloCompatible(const Trk::TrackParameters& Tp)
{
  const double pi = M_PI, pi2 = 2.*M_PI;
  if(m_caloF.empty()) return false;   

  std::list<double>::iterator f = m_caloF.begin(), fe = m_caloF.end();
  std::list<double>::iterator e = m_caloE.begin();

  const AmgVector(5)& Vp = Tp.parameters(); 
  double F = Vp[2]; 
  double E = Vp[3]; 

  for(; f!=fe; ++f) {
    double df = fabs(F-(*f));
    if(df > pi        ) df = fabs(pi2-df);
    if(df < m_phiWidth) {
      double de = fabs(E-(*e));
      if(de < m_etaWidth) return true;
    }
    ++e;
  }
  return false;
}
