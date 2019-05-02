/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::SiTrackMaker_xk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include "SiTrackMakerTool_xk/SiTrackMaker_xk.h"

#include "InDetPrepRawData/SiClusterContainer.h"
#include "TrkCaloClusterROI/CaloClusterROI.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

#include <iomanip>
#include <ostream>

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiTrackMaker_xk::SiTrackMaker_xk
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p)
{
  declareInterface<ISiTrackMaker>(this);
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiTrackMaker_xk::initialize()
{
  // Get beam geometry
  //
  ATH_CHECK( m_beamSpotKey.initialize() );

  // Get magnetic field service
  //
  if (m_fieldmode != "NoField" ) {
    
    if ( !m_fieldServiceHandle.retrieve() ){
      ATH_MSG_FATAL("Failed to retrieve " << m_fieldServiceHandle );
      return StatusCode::FAILURE;
    }    
    ATH_MSG_DEBUG("Retrieved " << m_fieldServiceHandle );
  }
  if (m_fieldmode == "NoField") m_fieldModeEnum = Trk::NoField;
  else if (m_fieldmode == "MapSolenoid") m_fieldModeEnum = Trk::FastField;
  else m_fieldModeEnum = Trk::FullField;

  // Get detector elements road maker tool
  //
  if ( m_roadmaker.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << m_roadmaker );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "Retrieved tool " << m_roadmaker );
  }

  // Get combinatorial track finder tool
  //
  if ( m_tracksfinder.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << m_tracksfinder );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "Retrieved tool " << m_tracksfinder );
  }

  // Get seed to track conversion tool
  //
  if (m_seedsegmentsWrite) {
    if (m_seedtrack.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Failed to retrieve tool " << m_seedtrack );
      return StatusCode::FAILURE;

    } else {
      ATH_MSG_INFO( "Retrieved tool " << m_seedtrack );
    }
  } else {
    m_seedtrack.disable();
  }

  m_heavyion = false;

  // TrackpatternRecoInfo preparation 
  //
  if        (m_patternName == "SiSpacePointsSeedMaker_Cosmic"     )  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_Cosmic     );
  } else if (m_patternName == "SiSpacePointsSeedMaker_HeavyIon"   )  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_HeavyIon   );
    m_heavyion = true;
  } else if (m_patternName == "SiSpacePointsSeedMaker_LowMomentum")  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_LowMomentum);
  } else if (m_patternName == "SiSpacePointsSeedMaker_BeamGas"    )  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_BeamGas    );
  } else if (m_patternName == "SiSpacePointsSeedMaker_ForwardTracks"    )  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_ForwardTracks);
  } else if (m_patternName == "SiSpacePointsSeedMaker_LargeD0"    )  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_LargeD0    );
  } else if (m_patternName == "SiSpacePointsSeedMaker_SLHCConversionTracks")  {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_SLHCConversionTracks);
  } else {
    m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSPSeededFinder                  );
  } 

  if (m_cosmicTrack) m_seedsfilter = 3;

  ATH_CHECK(m_caloCluster.initialize(m_useBremModel && m_useCaloSeeds));
  ATH_CHECK(m_caloHad.initialize( !m_useSSSfilter && m_useHClusSeed) );
  
  if (m_pTmin < 20.) m_pTmin = 20.;
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiTrackMaker_xk::finalize()
{
  return AlgTool::finalize();
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream&  InDet::SiTrackMaker_xk::dump( MsgStream& out ) const
{
  out<<std::endl;
  if (m_nprint) return dumpevent(out);
  return dumpconditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiTrackMaker_xk::dumpconditions( MsgStream& out ) const
{

  int n      = 62-m_tracksfinder.type().size();
  std::string s4; for (int i=0; i<n; ++i) s4.append(" "); s4.append("|");

  std::string fieldmode[9] ={"NoField"       ,"ConstantField","SolenoidalField",
                             "ToroidalField" ,"Grid3DField"  ,"RealisticField" ,
                             "UndefinedField","AthenaField"  , "?????"         };

  Trk::MagneticFieldMode fieldModeEnum(m_fieldModeEnum);
  if (!m_fieldServiceHandle->solenoidOn()) fieldModeEnum = Trk::NoField;
  Trk::MagneticFieldProperties fieldprop(fieldModeEnum);
  int mode = fieldprop.magneticFieldMode();
  if (mode<0 || mode>8 ) mode = 8; 

  n     = 62-fieldmode[mode].size();
  std::string s3; for (int i=0; i<n; ++i) s3.append(" "); s3.append("|");

  n     = 62-m_roadmaker.type().size();
  std::string s6; for (int i=0; i<n; ++i) s6.append(" "); s6.append("|");

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
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
  if (beamSpotHandle.isValid()) { m_xybeam[0] = beamSpotHandle->beamPos()[0]; m_xybeam[1] = beamSpotHandle->beamPos()[1]; }
  else                          { m_xybeam[0] = 0.;                           m_xybeam[1] = 0.;                           }
  
  m_pix          = PIX && m_usePix;
  m_sct          = SCT && m_useSct;
  bool simpleTrack  = false;

  InDet::TrackQualityCuts trackquality = std::move(setTrackQualityCuts(simpleTrack));

  // New event for track finder tool
  //
  m_tracksfinder->newEvent(m_trackinfo, trackquality);

  // Erase cluster to track association
  //
  if (m_seedsfilter) m_clusterTrack.clear();

  // Erase statistic information
  //
  m_inputseeds = 0;
  m_goodseeds  = 0;
  m_findtracks = 0;

  // Retrieve 
  //
  if (m_useBremModel && m_useCaloSeeds) {

    m_caloF.clear();
    m_caloR.clear();
    m_caloZ.clear();

    if (!m_caloCluster.key().empty()) {
      SG::ReadHandle<CaloClusterROI_Collection> calo_cluster(m_caloCluster);
      if (calo_cluster.isValid()) {
        for (const Trk::CaloClusterROI *c : * calo_cluster) {
          m_caloF.push_back( c->globalPosition().phi ());
          m_caloR.push_back( c->globalPosition().perp());
          m_caloZ.push_back( c->globalPosition().z   ());
        }
      }
    }
  }

  if (!m_useSSSfilter && m_useHClusSeed) {

    m_hadF.clear();
    m_hadR.clear();
    m_hadZ.clear();

    if (!m_caloHad.key().empty()) {
      SG::ReadHandle<CaloClusterROI_Collection> calo_had(m_caloHad);
      if (calo_had.isValid()) {
        for (const Trk::CaloClusterROI *c : * calo_had) {
          m_hadF.push_back( c->globalPosition().phi ());
          m_hadR.push_back( c->globalPosition().perp());
          m_hadZ.push_back( c->globalPosition().z   ());
        }
      }
    }
  }
  if (m_seedsegmentsWrite) m_seedtrack->newEvent(m_trackinfo,m_patternName);
}

///////////////////////////////////////////////////////////////////
// Initiate track finding tool for new event
///////////////////////////////////////////////////////////////////

void InDet::SiTrackMaker_xk::newTrigEvent(bool PIX,bool SCT)
{
  m_pix          = PIX && m_usePix;
  m_sct          = SCT && m_useSct;
  bool simpleTrack  = true;

  InDet::TrackQualityCuts trackquality = std::move(setTrackQualityCuts(simpleTrack));

  // New event for track finder tool
  //
  m_tracksfinder->newEvent(m_trackinfo, trackquality);

  // Erase cluster to track association
  //
  if (m_seedsfilter) m_clusterTrack.clear(); 

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
  if (m_seedsegmentsWrite) m_seedtrack->endEvent();
 
  // Print event information 
  //
  if (msgLevel()<=0) {
    m_nprint=1;
    ATH_MSG_DEBUG(*this);
  }
}

///////////////////////////////////////////////////////////////////
// Main method for track finding using space points
///////////////////////////////////////////////////////////////////

std::list<Trk::Track*> InDet::SiTrackMaker_xk::getTracks
(const std::list<const Trk::SpacePoint*>& Sp)
{
  ++m_inputseeds;
  std::list<Trk::Track*> tracks;
  if (!m_pix && !m_sct) return tracks;
  
  bool good;
  !m_seedsfilter ? good=true : good=newSeed(Sp);  

  if (!good) return tracks;

  bool dbm = isDBMSeeds(*Sp.begin());
  m_dbm = dbm; // Cache dbm for another getTracks method.

  // Get initial parameters estimation
  //
  bool sss = false;
  std::array<double, 9> par;
  const Trk::TrackParameters* Tp = nullptr;
  if (dbm) Tp = getAtaPlaneDBM(Sp, par);
  else Tp = getAtaPlane(sss && m_useHClusSeed, Sp, par);
  m_par = par; // Cache par for another getTracks method.
  if (!Tp) return tracks;
  ++m_goodseeds;

  // Get detector elements road
  //
  std::list<const InDetDD::SiDetectorElement*> DE;
  if (!m_cosmicTrack) m_roadmaker->detElementsRoad(*Tp,Trk::alongMomentum,   DE);
  else                m_roadmaker->detElementsRoad(*Tp,Trk::oppositeMomentum,DE);

  if (!m_pix || !m_sct || m_dbm) detectorElementsSelection(DE, dbm);

  if ( static_cast<int>(DE.size())  <   m_nclusmin) {
    delete Tp;
    return tracks;
  }

  std::list<Amg::Vector3D> Gp;
  
  // Find possible list of tracks using space points space points information
  //
  if (!m_useBremModel) {
    tracks = m_tracksfinder->getTracks        (*Tp,Sp,Gp,DE,m_clusterTrack);
  } else if (!m_useCaloSeeds) {
    tracks = m_tracksfinder->getTracksWithBrem(*Tp,Sp,Gp,DE,m_clusterTrack,false);
  } else if (isCaloCompatible(par)) {
    tracks = m_tracksfinder->getTracksWithBrem(*Tp,Sp,Gp,DE,m_clusterTrack,true);
  } else {
    tracks = m_tracksfinder->getTracks        (*Tp,Sp,Gp,DE,m_clusterTrack);
  }
  
  if (m_seedsfilter) {
    std::list<Trk::Track*>::iterator t = tracks.begin();
    while (t!=tracks.end()) {
      if (!isNewTrack((*t), m_clusterTrack)) {
        delete (*t);
        tracks.erase(t++);
      } else {
        clusterTrackMap((*t++), m_clusterTrack);
      }
    }
  }
  m_findtracks+=tracks.size();

  // Call seed to track execution
  //
  if (m_seedsegmentsWrite) m_seedtrack->executeSiSPSeedSegments(Tp,tracks.size(),Sp);

  delete Tp;
  return tracks;
}

///////////////////////////////////////////////////////////////////
// Main method for track finding using space points
///////////////////////////////////////////////////////////////////

std::list<Trk::Track*> InDet::SiTrackMaker_xk::getTracks
(const Trk::TrackParameters& Tp,const std::list<Amg::Vector3D>& Gp)
{
  ++m_inputseeds;
  std::list<Trk::Track*> tracks;
  if (!m_pix && !m_sct) return tracks;

  ++m_goodseeds;

  // Get detector elements road
  //
  std::list<const InDetDD::SiDetectorElement*> DE;
  if (!m_cosmicTrack) m_roadmaker->detElementsRoad(Tp,Trk::alongMomentum,   DE);
  else                m_roadmaker->detElementsRoad(Tp,Trk::oppositeMomentum,DE);

  // In this method, isDBMSeeds method is not called and dbm is not determined.
  // However, dbm is required for detectorElementsSelection.
  // Cached value of dbm is used.
  bool dbm = m_dbm;
  if (!m_pix || !m_sct) detectorElementsSelection(DE, dbm);

  if (static_cast<int>(DE.size()) < m_nclusmin) return tracks; 

  // Find possible list of tracks with trigger track parameters or global positions
  //
  std::list<const Trk::SpacePoint*>  Sp;
  
  // In this method, getAtaPlane method is not called and par is not determined.
  // However, par is required for isCaloCompatible.
  // Cached value of par is used.
  std::array<double, 9> par = m_par;

  if (!m_useBremModel) {
    tracks = m_tracksfinder->getTracks        (Tp,Sp,Gp,DE,m_clusterTrack);
  } else if (!m_useCaloSeeds) {
    tracks = m_tracksfinder->getTracksWithBrem(Tp,Sp,Gp,DE,m_clusterTrack,false);
  } else if (isCaloCompatible(par)) {
    tracks = m_tracksfinder->getTracksWithBrem(Tp,Sp,Gp,DE,m_clusterTrack,true);
  } else {
    tracks = m_tracksfinder->getTracks        (Tp,Sp,Gp,DE,m_clusterTrack);
  }
  
  if (m_seedsfilter) {
    std::list<Trk::Track*>::iterator t = tracks.begin();
    while (t!=tracks.end()) {
      if (!isNewTrack((*t), m_clusterTrack)) {
        delete (*t);
        tracks.erase(t++);
      } else {
        clusterTrackMap((*t++), m_clusterTrack);
      }
    }
  }
  m_findtracks+=tracks.size();
  return tracks;
}

///////////////////////////////////////////////////////////////////
// Space point seed parameters extimation
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters* InDet::SiTrackMaker_xk::getAtaPlane
(bool sss,const std::list<const Trk::SpacePoint*>& SP, std::array<double, 9>& par) const
{
  std::list<const Trk::SpacePoint*>::const_iterator is=SP.begin(),ise=SP.end(),is0,is1,is2;  
  if (is==ise) return nullptr;

  const Trk::PrepRawData* cl  = (*is)->clusterList().first;
  if (!cl) return nullptr;
  const Trk::PlaneSurface* pla = 
    static_cast<const Trk::PlaneSurface*>(&cl->detectorElement()->surface());
  if (!pla) return nullptr;

  is0 = is;
  if (++is==ise) return nullptr;
  is1 = is;
  if (++is==ise) return nullptr;
  is2 = is ;
  double p0[3],p1[3],p2[3]; 
  if (!globalPositions((*is0),(*is1),(*is2),p0,p1,p2)) return nullptr;

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
  
  double   d[3] = {x0-D[0],y0-D[1],z0-D[2]};

  par[0] = d[0]*Ax[0]+d[1]*Ax[1]+d[2]*Ax[2];
  par[1] = d[0]*Ay[0]+d[1]*Ay[1]+d[2]*Ay[2];

  Trk::MagneticFieldMode fieldModeEnum(m_fieldModeEnum);
  if (!m_fieldServiceHandle->solenoidOn()) fieldModeEnum = Trk::NoField;
  Trk::MagneticFieldProperties fieldprop(fieldModeEnum);
  if (fieldprop.magneticFieldMode() > 0) {

    double H[3],gP[3] ={x0,y0,z0}; m_fieldServiceHandle->getFieldZR(gP,H);

    if (fabs(H[2])>.0001) {
      par[2] = atan2(b+a*A,a-b*A);
      par[3] = atan2(1.,T)       ;  
      par[5] = -C/(300.*H[2])    ;
    } else {
      T    =  z2*sqrt(r2)  ;
      par[2] = atan2(y2,x2);
      par[3] = atan2(1.,T) ;
      par[5] = 1./m_pTmin  ;
    }
  } else {
    T    = z2*sqrt(r2)   ;
    par[2] = atan2(y2,x2);
    par[3] = atan2(1.,T) ;
    par[5] = 1./m_pTmin  ;
  }
  
  if (fabs(par[5])*m_pTmin > 1.1) return nullptr;
  par[4] = par[5]/sqrt(1.+T*T);
  par[6] = x0                              ;
  par[7] = y0                              ;
  par[8] = z0                              ;

  if (sss && !isHadCaloCompatible(par)) return nullptr;

  return pla->createTrackParameters(par[0],par[1],par[2],par[3],par[4],0); 
}

///////////////////////////////////////////////////////////////////
// Space point seed parameters extimation for DBM seed
///////////////////////////////////////////////////////////////////

const Trk::TrackParameters* InDet::SiTrackMaker_xk::getAtaPlaneDBM
(const std::list<const Trk::SpacePoint*>& SP, std::array<double, 9>& par) const
{
  std::list<const Trk::SpacePoint*>::const_iterator is=SP.begin(),ise=SP.end(),is0,is1,is2;  
  if (is==ise) return nullptr;

  const Trk::PrepRawData* cl = (*is)->clusterList().first;
  if (!cl) return nullptr;
  const Trk::PlaneSurface* pla = 
    static_cast<const Trk::PlaneSurface*>(&cl->detectorElement()->surface());
  if (!pla) return nullptr;

  is0 = is;
  if (++is==ise) return nullptr;
  is1 = is;
  if (++is==ise) return nullptr;
  is2 = is;
  double p0[3],p1[3],p2[3]; 
  if (!globalPositions((*is0),(*is1),(*is2),p0,p1,p2)) return nullptr;

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

  par[0] = d[0]*Ax[0]+d[1]*Ax[1]+d[2]*Ax[2];
  par[1] = d[0]*Ay[0]+d[1]*Ay[1]+d[2]*Ay[2];

  Trk::MagneticFieldMode fieldModeEnum(m_fieldModeEnum);
  if (!m_fieldServiceHandle->solenoidOn()) fieldModeEnum = Trk::NoField;
  Trk::MagneticFieldProperties fieldprop(fieldModeEnum);
  if (fieldprop.magneticFieldMode() > 0) {
    double H[3],gP[3] ={p0[0],p0[1],p0[2]};
    m_fieldServiceHandle->getFieldZR(gP,H);

    if (fabs(H[2])>.0001) {
      par[2] = atan2(b+a*A,a-b*A);
      par[3] = atan2(1.,T)       ;  
      par[5] = -C/(300.*H[2])    ;
    } else {
      T    =  z2*sqrt(r2)  ;
      par[2] = atan2(y2,x2);
      par[3] = atan2(1.,T) ;
      par[5] = 1./1000.    ;
    }
  } else {
    T    = z2*sqrt(r2)   ;
    par[2] = atan2(y2,x2);
    par[3] = atan2(1.,T) ;
    par[5] = 1./1000     ;
  }

  if (fabs(par[5])*20. > 1.1) return nullptr;
  par[4] = par[5]/sqrt(1.+T*T);
  par[6] = p0[0]                           ;
  par[7] = p0[1]                           ;
  par[8] = p0[2]                           ;
  return pla->createTrackParameters(par[0],par[1],par[2],par[3],par[4],0); 
}

///////////////////////////////////////////////////////////////////
// Set track quality cuts
///////////////////////////////////////////////////////////////////

InDet::TrackQualityCuts InDet::SiTrackMaker_xk::setTrackQualityCuts(bool simpleTrack)
{
  InDet::TrackQualityCuts trackquality;
  // Integer cuts
  //
  trackquality.setIntCut("MinNumberOfClusters" ,m_nclusmin   );
  trackquality.setIntCut("MinNumberOfWClusters",m_nwclusmin  );
  trackquality.setIntCut("MaxNumberOfHoles"    ,m_nholesmax  ); 
  trackquality.setIntCut("MaxHolesGae"         ,m_dholesmax  ); 

  if (m_useassoTool) trackquality.setIntCut("UseAssociationTool",1);
  else               trackquality.setIntCut("UseAssociationTool",0);
  if (m_cosmicTrack) trackquality.setIntCut("CosmicTrack"       ,1); 
  else               trackquality.setIntCut("CosmicTrack"       ,0);
  if (simpleTrack)   trackquality.setIntCut("SimpleTrack"       ,1);
  else               trackquality.setIntCut("SimpleTrack"       ,0);
  if (m_multitracks) trackquality.setIntCut("doMultiTracksProd" ,1);
  else               trackquality.setIntCut("doMultiTracksProd" ,0);

  // Double cuts
  //
  trackquality.setDoubleCut("pTmin"              ,m_pTmin      );
  trackquality.setDoubleCut("pTminBrem"          ,m_pTminBrem  );
  trackquality.setDoubleCut("MaxXi2forCluster"   ,m_xi2max     );
  trackquality.setDoubleCut("MaxXi2forOutlier"   ,m_xi2maxNoAdd);
  trackquality.setDoubleCut("MaxXi2forSearch"    ,m_xi2maxlink );
  trackquality.setDoubleCut("MaxXi2MultiTracks"  ,m_xi2multitracks);

  return trackquality;
}

///////////////////////////////////////////////////////////////////
// Detector elements selection
///////////////////////////////////////////////////////////////////

void InDet::SiTrackMaker_xk::detectorElementsSelection(std::list<const InDetDD::SiDetectorElement*>& DE, bool dbm) const
{
  std::list<const InDetDD::SiDetectorElement*>::iterator d = DE.begin();
  if (!dbm) {
    while (d!=DE.end()) {
      if ((*d)->isPixel()) {
        if (!m_pix) {
          d = DE.erase(d);
          continue;
        }
      } else if (!m_sct) {
        d = DE.erase(d);
        continue;
      }
      ++d;
    }
  } else {
    while (d!=DE.end()) {
      if (!(*d)->isDBM() ) {
        if ((*d)->isSCT() || (*d)->isEndcap()) {
          d = DE.erase(d);
          continue;
        }
        const Amg::Transform3D& T = (*d)->surface().transform(); 
        if (T(0,3)*T(0,3)+T(1,3)*T(1,3) > (43.*43) ) {
          d = DE.erase(d);
          continue;
        }
      }
      ++d;
    }
  }
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
  for (s = Sp.begin(); s!=se; ++s) {

    const Trk::PrepRawData* p = (*s)->clusterList().first; 
   
    for (pt = m_clusterTrack.find(p); pt!=pte; ++pt) {
      if ((*pt).first!=p) break;
      trackseed.insert((*pt).second);
    }
    ++n;

    p = (*s)->clusterList().second;
    if (!p) continue;

    for (pt = m_clusterTrack.find(p); pt!=pte; ++pt) {
      if ((*pt).first!=p) break;
      trackseed.insert((*pt).second);
    }
    ++n;

  }
  if (trackseed.empty()) return true;

  if (m_ITKGeomtry && n!=3 && n!=6) return false;
 
  std::multiset<const Trk::Track*>::iterator t = trackseed.begin(), te = trackseed.end();

  const Trk::Track* tr  = (*t)                             ;
  unsigned int      nsm = tr->measurementsOnTrack()->size();
  unsigned int      nsm3= 0                                ;
  int               nt  = 1                                ;
  int               t3  = 0                                ;

  for (++t; t!=te; ++t) {
    if ((*t) == tr) {
      ++nt;
      continue;
    }
    if (nt  == n) {
      ++t3;
      unsigned int ns =  tr->measurementsOnTrack()->size();
      if (ns > nsm3) nsm3 = ns;
    }
    tr = (*t); nt = 1;
    unsigned int ns = tr->measurementsOnTrack()->size();
    if (ns > nsm) nsm = ns;
  }
  if (nt == n) {
    ++t3;
    unsigned int ns =  tr->measurementsOnTrack()->size();
    if (ns > nsm3) nsm3 = ns;
  }

  if (m_heavyion) {
    if (n==3 || t3 <=0) return true;
    return false;
  }

  if ( (m_ITKGeomtry && t3 > 0) || nsm3 > 13 || t3 > 2) return false;

  if ( !m_cosmicTrack && n==3 && m_sct && (*Sp.begin())->r() > 43. ) return true;
  if (t3 > 0) return false;
  return true;
}

///////////////////////////////////////////////////////////////////
// Clusters-track multimap production
///////////////////////////////////////////////////////////////////

void  InDet::SiTrackMaker_xk::clusterTrackMap(Trk::Track* Tr,
                                              std::multimap<const Trk::PrepRawData*, const Trk::Track*>& clusterTrack) const
{
  DataVector<const Trk::MeasurementBase>::const_iterator 
    m  = Tr->measurementsOnTrack()->begin(), 
    me = Tr->measurementsOnTrack()->end  ();

  for (; m!=me; ++m) {
    const Trk::PrepRawData* prd = static_cast<const Trk::RIO_OnTrack*>(*m)->prepRawData();
    if (prd) clusterTrack.insert(std::make_pair(prd,Tr));
  }
}
 
///////////////////////////////////////////////////////////////////
// Test is it new track
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::isNewTrack(Trk::Track* Tr,
                                        const std::multimap<const Trk::PrepRawData*, const Trk::Track*>& clusterTrack) const
{
  const Trk::PrepRawData* prd   [100];
  std::multimap<const Trk::PrepRawData*,const Trk::Track*>::const_iterator 
    ti,t[100],te = m_clusterTrack.end();

  int n = 0;

  DataVector<const Trk::MeasurementBase>::const_iterator 
    m  = Tr->measurementsOnTrack()->begin(), 
    me = Tr->measurementsOnTrack()->end  ();

  for (; m!=me; ++m) {
  
    const Trk::PrepRawData* pr = static_cast<const Trk::RIO_OnTrack*>(*m)->prepRawData();
    if (pr) {
      prd[n] =pr;
      t  [n] = clusterTrack.find(prd[n]);
      if (t[n]==te) return true;
      ++n;
    }
  }

  if (!n) return true;
  int nclt = n;
  
  for (int i=0; i!=n; ++i) {
    int nclmax = 0;
    for (ti=t[i]; ti!=te; ++ti) {
      if ( (*ti).first != prd[i] ) break;
      int ncl = (*ti).second->measurementsOnTrack()->size();
      if (ncl > nclmax) nclmax = ncl;
    }   
    if (nclt > nclmax) return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// Calculation global position for space points
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::globalPositions
(const Trk::SpacePoint* s0,const Trk::SpacePoint* s1,const Trk::SpacePoint* s2,
 double* p0,double* p1,double* p2) const
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
 
  if (!s0->clusterList().second && !s1->clusterList().second && !s2->clusterList().second) return true;

  double dir0[3],dir1[3],dir2[3]; 

  globalDirections(p0,p1,p2,dir0,dir1,dir2);

  if (s0->clusterList().second && !globalPosition(s0,dir0,p0)) return false;
  if (s1->clusterList().second && !globalPosition(s1,dir1,p1)) return false;
  if (s2->clusterList().second && !globalPosition(s2,dir2,p2)) return false;
  
  return true;
}

///////////////////////////////////////////////////////////////////
// Calculation global position for space points
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::globalPosition
(const Trk::SpacePoint* sp, double* dir,double* p) const
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

  if (du==0. || dv==0.) return false;

  double s0 = (dr[0]*u[0]+dr[1]*u[1]+dr[2]*u[2])/du;
  double s1 =-(dr[0]*v[0]+dr[1]*v[1]+dr[2]*v[2])/dv;

  if (s0 < -d0 || s0 > 1.+d0 ||  s1 < -d0 || s1 > 1.+d0) return false;

  p[0] = e0.first.x()+s0*a0[0]; 
  p[1] = e0.first.y()+s0*a0[1]; 
  p[2] = e0.first.z()+s0*a0[2];

  return true;
}

///////////////////////////////////////////////////////////////////
// Test is it track with calo seed compatible
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::isCaloCompatible(const std::array<double, 9>& par) const
{
  const double pi = M_PI, pi2 = 2.*M_PI;

  if (m_caloF.empty()) return false;   

  std::list<double>::const_iterator f = m_caloF.begin(), fe = m_caloF.end();
  std::list<double>::const_iterator r = m_caloR.begin();
  std::list<double>::const_iterator z = m_caloZ.begin();

  double F = par[2]                           ;
  double E = -log(tan(.5*par[3]))             ;
  double R = sqrt(par[6]*par[6]+par[7]*par[7]);
  double Z = par[8]                           ;                           
  
  for (; f!=fe; ++f) {
    double df = fabs(F-(*f));
    if (df > pi) df = fabs(pi2-df);
    if (df < m_phiWidth) {
      double dR = (*r)-R;
      double dZ = (*z)-Z;
      if (fabs(E-atanh(dZ/sqrt(dR*dR+dZ*dZ)) ) < m_etaWidth) return true;
    }
    ++r; ++z;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// Test track is compatible withi had calo seed
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::isHadCaloCompatible(const std::array<double, 9>& par) const
{
  const double pi = M_PI, pi2 = 2.*M_PI;

  if (m_hadF.empty() || fabs(par[5])*m_pTminSSS > 1.) return false;   

  std::list<double>::const_iterator f = m_hadF.begin(), fe = m_hadF.end();
  std::list<double>::const_iterator r = m_hadR.begin();
  std::list<double>::const_iterator z = m_hadZ.begin();

  double F = par[2]                           ;
  double E = -log(tan(.5*par[3]))             ;
  double R = sqrt(par[6]*par[6]+par[7]*par[7]);
  double Z = par[8]                           ;                           
  
  for (; f!=fe; ++f) {
    double df = fabs(F-(*f));
    if (df > pi) df = fabs(pi2-df);
    if (df < m_phiWidth) {
      double dR = (*r)-R;
      double dZ = (*z)-Z;
      if (fabs(E-atanh(dZ/sqrt(dR*dR+dZ*dZ))) < m_etaWidth) return true;
    }
    ++r;
    ++z;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// Test is it DBM seed
///////////////////////////////////////////////////////////////////

bool InDet::SiTrackMaker_xk::isDBMSeeds(const Trk::SpacePoint* s) const
{
  const InDetDD::SiDetectorElement* de= 
    static_cast<const InDetDD::SiDetectorElement*>(s->clusterList().first->detectorElement());
  return de && de->isDBM();
}

///////////////////////////////////////////////////////////////////
// Calculation global direction for positions of space points
///////////////////////////////////////////////////////////////////

void InDet::SiTrackMaker_xk::globalDirections
(double* p0,double* p1,double* p2,double* d0,double* d1,double* d2) const
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
