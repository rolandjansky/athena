/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::SiCombinatorialTrackFinder_xk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 12/04/2007 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include "SiCombinatorialTrackFinderTool_xk/SiCombinatorialTrackFinder_xk.h"

#include "EventInfo/TagInfo.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/ReadHandle.h"
#include "TrkExInterfaces/IPatternParametersPropagator.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkToolInterfaces/IPatternParametersUpdator.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkTrack/TrackInfo.h"

#include <iomanip>
#include <iostream>
#include <utility>

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiCombinatorialTrackFinder_xk::SiCombinatorialTrackFinder_xk
(const std::string& t, const std::string& n, const IInterface* p)
  : AthAlgTool(t, n, p)
{
  declareInterface<ISiCombinatorialTrackFinder>(this);
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiCombinatorialTrackFinder_xk::initialize()
{
  
  // Get RungeKutta propagator tool
  //
  if ( m_proptool.retrieve().isFailure() ) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_proptool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool " << m_proptool);
  }

  // Get updator tool
  //
  if ( m_updatortool.retrieve().isFailure() ) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_updatortool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool " << m_updatortool);
  }

  // Get RIO_OnTrack creator
  //
  if ( m_riocreator.retrieve().isFailure() ) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_riocreator);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool " << m_riocreator);
  }

  // Get tool for track-prd association
  //
  ATH_CHECK(m_assoTool.retrieve());

  if (m_usePIX) {  
    if ( m_pixelCondSummaryTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_pixelCondSummaryTool);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("Retrieved tool " << m_pixelCondSummaryTool);
    }
  }

  // Get SctConditionsSummaryTool
  //
  if (m_useSCT) {
    if ( m_sctCondSummaryTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_sctCondSummaryTool);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("Retrieved tool " << m_sctCondSummaryTool);
    }
  } else {
    m_sctCondSummaryTool.disable();
  }

  // get the key -- from StoreGate (DetectorStore)
  //
  std::vector< std::string > tagInfoKeys =  detStore()->keys<TagInfo> ();
  std::string tagInfoKey = "";

  if (tagInfoKeys.size()==0) {
    ATH_MSG_WARNING(" No TagInfo keys in DetectorStore ");
  } else {
    if (tagInfoKeys.size() > 1) {
      ATH_MSG_WARNING("More than one TagInfo key in the DetectorStore, using the first one ");
    }
    tagInfoKey = tagInfoKeys[0];
  }

  m_callbackString = tagInfoKey;

  const DataHandle<TagInfo> tagInfoH;
  
  // register the Callback
  //
  StatusCode sc = detStore()->regFcn(&InDet::SiCombinatorialTrackFinder_xk::mapDetectorElementsProduction,
				     this, tagInfoH, m_callbackString);

  if (sc==StatusCode::SUCCESS) {
    ATH_MSG_INFO("Registered callback for geometry " << name());
  } else {
    ATH_MSG_ERROR("Could not book callback for geometry " << name ());
    return StatusCode::FAILURE;
  }

  if ( !m_fieldServiceHandle.retrieve() ) {
    ATH_MSG_FATAL("Failed to retrieve " << m_fieldServiceHandle );
    return StatusCode::FAILURE;
  }    
  ATH_MSG_DEBUG("Retrieved " << m_fieldServiceHandle );

  // Set SiTools and conditions
  //
  m_tools.setTools(&*m_proptool, &*m_updatortool, &*m_riocreator, &*m_assoTool, &*m_fieldServiceHandle);
  m_tools.setTools(&*m_pixelCondSummaryTool, &*m_sctCondSummaryTool);

  // Setup callback for magnetic field
  //
  magneticFieldInit();

  // Set tool to trajectory
  //
  m_trajectory.setTools(&m_tools);

  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;

  ATH_CHECK( m_pixcontainerkey.initialize() );
  ATH_CHECK( m_sctcontainerkey.initialize() );
  ATH_CHECK( m_boundarySCTKey.initialize() );
  ATH_CHECK( m_SCTDetEleCollKey.initialize() );

  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiCombinatorialTrackFinder_xk::finalize()
{
  return AlgTool::finalize();
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream&  InDet::SiCombinatorialTrackFinder_xk::dump( MsgStream& out ) const
{
  out<<std::endl;
  if (m_nprint)  return dumpevent(out);
  return dumpconditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiCombinatorialTrackFinder_xk::dumpconditions( MsgStream& out ) const
{
  int n = 62-m_proptool.type().size();
  std::string s1;
  for (int i=0; i<n; ++i) s1.append(" ");
  s1.append("|");

  std::string fieldmode[9] ={"NoField"       , "ConstantField", "SolenoidalField",
			     "ToroidalField" , "Grid3DField"  , "RealisticField" ,
			     "UndefinedField", "AthenaField"  , "?????"         };

  int mode = m_fieldprop.magneticFieldMode(); 
  if (mode<0 || mode>8 ) mode = 8; 

  n     = 62-fieldmode[mode].size();
  std::string s3;
  for (int i=0; i<n; ++i) s3.append(" ");
  s3.append("|");

  n     = 62-m_updatortool.type().size();
  std::string s4;
  for (int i=0; i<n; ++i) s4.append(" ");
  s4.append("|");

  n     = 62-m_riocreator.type().size();
  std::string s5;
  for (int i=0; i<n; ++i) s5.append(" ");
  s5.append("|");

  n     = 62-m_pixcontainerkey.key().size();
  std::string s7;
  for (int i=0; i<n; ++i) s7.append(" ");
  s7.append("|");

  n     = 62-m_sctcontainerkey.key().size();
  std::string s8;
  for (int i=0; i<n; ++i) s8.append(" ");
  s8.append("|");

  n     = 62-m_assoTool.type().size();
  std::string s9;
  for (int i=0; i<n; ++i) s9.append(" ");
  s9.append("|");

  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;
  if (m_usePIX) {
    out<<"| Pixel clusters location | "<<m_pixcontainerkey.key() <<s7<<std::endl;
  }
  if (m_useSCT) {
    out<<"| SCT   clusters location | "<<m_sctcontainerkey.key() <<s8<<std::endl;
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

MsgStream& InDet::operator <<
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

  m_pix = m_usePIX && m_pixcontainerkey.initialize().isSuccess();
  m_sct = m_useSCT && m_sctcontainerkey.initialize().isSuccess();

  // Erase statistic information
  //
  m_inputseeds = 0;
  m_goodseeds  = 0;
  m_inittracks = 0;
  m_findtracks = 0;
  m_roadbug    = 0;

  // Set track info
  //
  m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSPSeededFinder);
  m_cosmicTrack = 0;
}

///////////////////////////////////////////////////////////////////
// Initiate track finding tool
///////////////////////////////////////////////////////////////////

void InDet::SiCombinatorialTrackFinder_xk::newEvent
(Trk::TrackInfo info, const TrackQualityCuts& Cuts)
{
  newEvent();
  m_trackinfo = info;
  
  // Get track qulaity cuts information
  //
  getTrackQualityCuts(Cuts);

  m_heavyion    = false;
  m_cosmicTrack =     0;

  if (info.patternRecoInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_Cosmic)) {
    m_cosmicTrack = 1;
  } else if (info.patternRecoInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_HeavyIon)) {
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
  if (m_outputlevel<=0) {
    m_nprint=1;
    ATH_MSG_DEBUG(*this);
  }
}

///////////////////////////////////////////////////////////////////
// Main method for track finding using space points
///////////////////////////////////////////////////////////////////

const std::list<Trk::Track*>&  InDet::SiCombinatorialTrackFinder_xk::getTracks
(const Trk::TrackParameters& Tp,
 const std::list<const Trk::SpacePoint*>& Sp,const std::list<Amg::Vector3D>& Gp,
 std::list<const InDetDD::SiDetectorElement*>& DE,const TrackQualityCuts& Cuts)
{
  m_tools.setBremNoise(false,false);
  m_tracks.erase(m_tracks.begin(), m_tracks.end());

  ++m_inputseeds;
  if (!m_pix && !m_sct) {
    return m_tracks;
  }

  // Get track qulaity cuts information
  //
  getTrackQualityCuts(Cuts);
  std::multimap<const Trk::PrepRawData*,const Trk::Track*> PT;
  if (!findTrack(Tp,Sp,Gp,DE,PT)) return m_tracks;
 
  m_trajectory.sortStep();

  // Trk::Track production
  //
  Trk::Track* t = convertToTrack();
  ++m_findtracks;
  m_tracks.push_back(t);

  if (!m_tools.multiTrack() || m_simpleTrack || Sp.size()<=2 || m_cosmicTrack || m_trajectory.pTfirst() < m_tools.pTmin()) return m_tracks;

  while ((t=convertToNextTrack())) {
    ++m_findtracks;
    m_tracks.push_back(t);
  }
  return m_tracks;
}

///////////////////////////////////////////////////////////////////
// Main method for track finding using space points
///////////////////////////////////////////////////////////////////

const std::list<Trk::Track*>& InDet::SiCombinatorialTrackFinder_xk::getTracks
(const Trk::TrackParameters& Tp,
 const std::list<const Trk::SpacePoint*>& Sp,const std::list<Amg::Vector3D>& Gp,
 std::list<const InDetDD::SiDetectorElement*>& DE,
 std::multimap<const Trk::PrepRawData*,const Trk::Track*>& PT)
{
  m_tools.setBremNoise(false, false);
  m_tracks.erase(m_tracks.begin(), m_tracks.end());

  ++m_inputseeds;
  if (!m_pix && !m_sct) {
    return m_tracks;
  }

  if (!findTrack(Tp,Sp,Gp,DE,PT)) return m_tracks;
  if (!m_trajectory.isNewTrack(PT)) return m_tracks;

  m_trajectory.sortStep();

  // Trk::Track production
  //
  Trk::Track* t = convertToTrack();
  if (t==nullptr) return m_tracks;

  ++m_findtracks;
  m_tracks.push_back(t);

  if (!m_tools.multiTrack() || m_simpleTrack || Sp.size()<=2 || m_cosmicTrack || m_trajectory.pTfirst() < m_tools.pTmin()) return m_tracks;

  while ((t=convertToNextTrack())) {
    ++m_findtracks;
    m_tracks.push_back(t);
  } 
  return m_tracks;
}

///////////////////////////////////////////////////////////////////
// Main method for track finding using space points and 
// using electron noise model
///////////////////////////////////////////////////////////////////

const std::list<Trk::Track*>&  InDet::SiCombinatorialTrackFinder_xk::getTracksWithBrem
(const Trk::TrackParameters& Tp,
 const std::list<const Trk::SpacePoint*>& Sp,
 const std::list<Amg::Vector3D>& Gp,
 std::list<const InDetDD::SiDetectorElement*>& DE,
 std::multimap<const Trk::PrepRawData*, const Trk::Track*>& PT,
 bool isCaloCompatible)
{
  // Old information
  //
  int mult = 0;
  if (m_tools.multiTrack()) mult = 1;
  double Xi2m = m_tools.xi2multi();

  m_tools.setBremNoise(false, true);
  m_tracks.erase(m_tracks.begin(), m_tracks.end());

  ++m_inputseeds;
  if (!m_pix && !m_sct) {
    return m_tracks;
  }

  bool Q = findTrack(Tp,Sp,Gp,DE,PT);
  if (Q) Q = m_trajectory.isNewTrack(PT);
  int na = 0;
  if (Q) {

    m_trajectory.sortStep();

    // Trk::Track production
    //
    Trk::TrackInfo oldinfo = m_trackinfo;
    if (isCaloCompatible) m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::TrackInCaloROI);

    m_tools.setMultiTracks(0   ,Xi2m);
    Trk::Track* t = convertToTrack();
    m_trackinfo = oldinfo;
    m_tools.setMultiTracks(mult,Xi2m);

    if (!t) return m_tracks;
    ++m_findtracks;
    m_tracks.push_back(t);
    na = m_trajectory.nclusters();
    if (na >=12 && !m_trajectory.nclustersNoAdd()) return m_tracks;
   
    if (m_trajectory.pTfirst() <  m_pTminBrem) return m_tracks;
  }
  if ((*Sp.begin())->clusterList().second) return m_tracks;
 
  // Repeat track finding using electron noise model
  //
  m_tools.setBremNoise(true,true);
  if (!findTrack(Tp, Sp, Gp, DE, PT)) return m_tracks;
  if (!m_trajectory.isNewTrack(PT)) return m_tracks;
  
  int nb = m_trajectory.nclusters();
  if (nb <= na ) return m_tracks;
  
  m_trajectory.sortStep();

  // Trk::Track production
  //
  Trk::TrackInfo oldinfo = m_trackinfo;
  m_trackinfo.setTrackProperties(Trk::TrackInfo::BremFit          );
  m_trackinfo.setTrackProperties(Trk::TrackInfo::BremFitSuccessful);
  if (isCaloCompatible) m_trackinfo.setPatternRecognitionInfo(Trk::TrackInfo::TrackInCaloROI);

  m_tools.setMultiTracks(0   ,Xi2m);
  Trk::Track* t = convertToTrack();
  m_trackinfo = oldinfo;
  m_tools.setMultiTracks(mult,Xi2m);

  if (t==nullptr) return m_tracks;

  ++m_findtracks;
  m_tracks.push_back(t);
  return m_tracks;
}

///////////////////////////////////////////////////////////////////
// Main method for track finding using space points
///////////////////////////////////////////////////////////////////

bool InDet::SiCombinatorialTrackFinder_xk::findTrack
(const Trk::TrackParameters& Tp,
 const std::list<const Trk::SpacePoint*>& Sp,const std::list<Amg::Vector3D>& Gp,
 std::list<const InDetDD::SiDetectorElement*>& DE,
 std::multimap<const Trk::PrepRawData*,const Trk::Track*>& PT)
{
  // List detector element links preparation
  //
  std::list<const InDet::SiDetElementBoundaryLink_xk*> DEL;
  detectorElementLinks(DE, DEL);

  SG::ReadHandle<InDet::SiClusterContainer> pixcontainer(m_pixcontainerkey);
  SG::ReadHandle<InDet::SiClusterContainer> sctcontainer(m_sctcontainerkey);

  // List cluster preparation
  //
  std::list<const InDet::SiCluster*> Cl;
  bool TWO = false;

  if (Sp.size() > 1) {
    if (!spacePointsToClusters(Sp,Cl)) {
      return false;
    }
    if (Sp.size()<=2) TWO = true;
  } else if (Gp.size() > 2) {
    if (!m_trajectory.globalPositionsToClusters(pixcontainer.ptr(),sctcontainer.ptr(),Gp,DEL,PT,Cl)) return false;
  } else {
    if (!m_trajectory.trackParametersToClusters(pixcontainer.ptr(),sctcontainer.ptr(),Tp,DEL,PT,Cl)) return false;
  }
  ++m_goodseeds;

  // Build initial trajectory
  //
  bool Qr;
  bool Q = m_trajectory.initialize(m_pix,m_sct,pixcontainer.ptr(),sctcontainer.ptr(),Tp,Cl,DEL,Qr);

  if (!Q && Sp.size() < 2 && Gp.size() > 3) {

    Cl.clear();
    if (!m_trajectory.trackParametersToClusters(pixcontainer.ptr(),sctcontainer.ptr(),Tp,DEL,PT,Cl)) return false;
    if (!m_trajectory.initialize   (m_pix,m_sct,pixcontainer.ptr(),sctcontainer.ptr(),Tp,Cl,DEL,Qr)) return false;
    Q = Qr = true;
  }

  if (!Qr) ++m_roadbug;
  if (!Q) return false;
  ++m_inittracks;
  bool pixseed = m_trajectory.isLastPixel();
  int itmax    = 30;
  if (m_simpleTrack) itmax = 10;
  if (m_heavyion) itmax = 50;

  // Track finding
  //
  if (pixseed) {      // Strategy for pixel seeds
    if (!m_trajectory.forwardExtension (false,itmax)) return false;
    if (!m_trajectory.backwardSmoother (false)      ) return false;
    if (!m_trajectory.backwardExtension(itmax)      ) return false;

    if (m_trajectory.difference() > 0) {
      if (!m_trajectory.forwardFilter()          ) return false;
      if (!m_trajectory.backwardSmoother (false) ) return false;
    } 
    int na = m_trajectory.nclustersNoAdd();
    if (m_trajectory.nclusters()+na < m_nclusmin || m_trajectory.ndf() < m_nwclusmin) return false;
  } else {      // Strategy for mixed seeds
    if (!m_trajectory.backwardSmoother(TWO)       ) return false;
    if (!m_trajectory.backwardExtension(itmax)    ) return false;
    if (!m_trajectory.forwardExtension(true,itmax)) return false;

    int na = m_trajectory.nclustersNoAdd();
    if (m_trajectory.nclusters()+na < m_nclusmin || m_trajectory.ndf() < m_nwclusmin) return false;
    if (!m_trajectory.backwardSmoother(false)    ) return false;

    na     = m_trajectory.nclustersNoAdd();
    if (m_trajectory.nclusters()+na < m_nclusmin || m_trajectory.ndf() < m_nwclusmin) return false;

    if (m_trajectory.difference() > 0) {
      if (!m_trajectory.forwardFilter()         ) return false;
      if (!m_trajectory.backwardSmoother (false)) return false;
    }
  } 

  if (m_trajectory.qualityOptimization()     <           (m_qualityCut*m_nclusmin)    ) return false;
  if (m_trajectory.pTfirst  () < m_pTmin     && m_trajectory.nclusters() < m_nclusmin ) return false;
  if (m_trajectory.nclusters() < m_nclusminb || m_trajectory.ndf      () < m_nwclusmin) return false;
  
  return true;
}

///////////////////////////////////////////////////////////////////
// Trk::Track production
///////////////////////////////////////////////////////////////////

Trk::Track* InDet::SiCombinatorialTrackFinder_xk::convertToTrack()
{
  if (!m_simpleTrack) {
    return new Trk::Track(m_trackinfo,
                          m_trajectory.convertToTrackStateOnSurface(m_cosmicTrack),
                          m_trajectory.convertToFitQuality());
  }

  Trk::TrackInfo info = m_trackinfo;
  info.setPatternRecognitionInfo(Trk::TrackInfo::SiSPSeededFinderSimple);
  return new Trk::Track(info,
                        m_trajectory.convertToSimpleTrackStateOnSurface(m_cosmicTrack),
                        m_trajectory.convertToFitQuality());
}

///////////////////////////////////////////////////////////////////
// Next Trk::Track production
///////////////////////////////////////////////////////////////////

Trk::Track* InDet::SiCombinatorialTrackFinder_xk::convertToNextTrack()
{
  DataVector<const Trk::TrackStateOnSurface>* tsos = m_trajectory.convertToNextTrackStateOnSurface();
  if (tsos==nullptr) return nullptr;
  return new Trk::Track(m_trackinfo,
                        tsos,
                        m_trajectory.convertToFitQuality());
}

///////////////////////////////////////////////////////////////////
// Callback function - get the magnetic field /
///////////////////////////////////////////////////////////////////

void InDet::SiCombinatorialTrackFinder_xk::magneticFieldInit() 
{
  // Build MagneticFieldProperties 
  //
  if      (m_fieldmode == "NoField"    ) m_fieldprop = Trk::MagneticFieldProperties(Trk::NoField  );
  else if (m_fieldmode == "MapSolenoid") m_fieldprop = Trk::MagneticFieldProperties(Trk::FastField);
  else                                   m_fieldprop = Trk::MagneticFieldProperties(Trk::FullField);
  m_tools.setTools(m_fieldprop);
}

///////////////////////////////////////////////////////////////////
// Map of detector elements production
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiCombinatorialTrackFinder_xk::mapDetectorElementsProduction 
(IOVSVC_CALLBACK_ARGS_P(I, keys))
{
  (void) I;

  StatusCode sc;

  // Get  Pixel Detector Manager
  //
  const InDetDD::PixelDetectorManager* pixmgr = 0;
  if (m_usePIX) {
    sc = detStore()->retrieve(pixmgr,m_pixm);
    if (sc.isFailure() || !pixmgr) {
      ATH_MSG_FATAL("Could not get PixelDetectorManager  !");
      return StatusCode::FAILURE;
    }
  }

  // check if the string is ESD for guaranteeing that misalignment has been introduced already
  //
  bool needsUpdate = false;

  for (const std::string k: keys) {
    if (k == m_callbackString) {
      needsUpdate = true;
      break;
    }
  } 
  if (!needsUpdate) return StatusCode::SUCCESS;

  m_boundaryPIX.erase(m_boundaryPIX.begin(), m_boundaryPIX.end());
  
  const PixelID* IDp = nullptr;

  if (m_usePIX && detStore()->retrieve(IDp, "PixelID").isFailure()) {
    ATH_MSG_FATAL("Could not get Pixel ID helper");
    return StatusCode::FAILURE;
  }
  
  if (!IDp) return StatusCode::FAILURE;

  InDetDD::SiDetectorElementCollection::const_iterator s,se;

  unsigned int npix = 0, minpixid = 1000000, maxpixid = 0;

  if (IDp) {
    std::map<IdentifierHash, const InDetDD::SiDetectorElement*> idd;

    // Loop over each wafer of pixels
    //
    s  =  pixmgr->getDetectorElementBegin();
    se =  pixmgr->getDetectorElementEnd  ();

    for (; s!=se; ++s) {
     
      IdentifierHash id = (*s)->identifyHash();
      ++npix;
      if (id > maxpixid) maxpixid = id;
      if (id < minpixid) minpixid = id;
      idd.insert(std::make_pair(id,(*s)));
    }
    if (idd.size()!= maxpixid+1 || minpixid!=0) return StatusCode::FAILURE;
    
    m_boundaryPIX.reserve(idd.size());
    std::map<IdentifierHash, const InDetDD::SiDetectorElement*>::iterator i;
    for (i = idd.begin(); i!=idd.end(); ++i) {
      InDet::SiDetElementBoundaryLink_xk dl((*i).second);
      m_boundaryPIX.push_back(dl);
    }
  }

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Convert space points to clusters
///////////////////////////////////////////////////////////////////

bool InDet::SiCombinatorialTrackFinder_xk::spacePointsToClusters
(const std::list<const Trk::SpacePoint*>& Sp, std::list<const InDet::SiCluster*>& Sc) const
{
  for (const Trk::SpacePoint* s: Sp) {
    const Trk::PrepRawData* p = s->clusterList().first;
    if (p) {
      const InDet::SiCluster* c = static_cast<const InDet::SiCluster*>(p);
      if (c) Sc.push_back(c);
    }
    p = s->clusterList().second;
    if (p) {
      const InDet::SiCluster* c = static_cast<const InDet::SiCluster*>(p);
      if (c) Sc.push_back(c);
    }
  }

  //  Detector elments test
  //
  std::list<const InDet::SiCluster*>::iterator c = Sc.begin(), cn, ce = Sc.end();
  
  for (; c!=ce; ++c) {

    const InDetDD::SiDetectorElement* de = (*c)->detectorElement();

    for (++(cn=c); cn!=ce; ++cn) {
      if (de == (*cn)->detectorElement()) return false;
    }

  }
  return true;
}
 
///////////////////////////////////////////////////////////////////
// Convert detector elements to detector element links
///////////////////////////////////////////////////////////////////

void InDet::SiCombinatorialTrackFinder_xk::detectorElementLinks
(std::list<const InDetDD::SiDetectorElement*>        & DE,
 std::list<const InDet::SiDetElementBoundaryLink_xk*>& DEL) const
{
  SG::ReadCondHandle<InDet::SiDetElementBoundaryLinks_xk> boundarySCTHandle(m_boundarySCTKey);
  const InDet::SiDetElementBoundaryLinks_xk* boundarySCT{*boundarySCTHandle};
  if (boundarySCT==nullptr) {
    ATH_MSG_FATAL(m_boundarySCTKey.fullKey() << " returns null pointer");
  }

  for (const InDetDD::SiDetectorElement* d: DE) {
    IdentifierHash id = d->identifyHash();
    if (d->isPixel() && id < m_boundaryPIX.size()) DEL.push_back(&m_boundaryPIX[id]);
    else if (d->isSCT() && boundarySCT && id < boundarySCT->size()) DEL.push_back(&(*boundarySCT)[id]);
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
  if (!Cuts.getIntCut   ("CosmicTrack"         ,m_cosmicTrack)) m_cosmicTrack   =    0;

  if (!Cuts.getIntCut   ("MinNumberOfClusters" ,m_nclusmin   )) m_nclusmin      =    7;
  m_nclusminb = m_nclusmin-1;
  if (m_nclusminb < 3) m_nclusminb = 3;

  if (!Cuts.getIntCut   ("MinNumberOfWClusters",m_nwclusmin  )) m_nwclusmin     =    7;

  if (!Cuts.getIntCut   ("MaxNumberOfHoles"    ,m_nholesmax  )) m_nholesmax     =    2;
  if (!Cuts.getIntCut   ("MaxHolesGae"         ,m_dholesmax  )) m_dholesmax     =    2;
  if (!m_cosmicTrack) {
    if (m_nholesmax > 2) m_nholesmax = 2;
    if (m_dholesmax > 2) m_dholesmax = 2;
  }
  if (m_dholesmax > m_nholesmax) m_dholesmax = m_nholesmax;

  int useasso;
  if (!Cuts.getIntCut   ("UseAssociationTool"  ,useasso      )) useasso         =    0;

  int simpletrack;
  if (!Cuts.getIntCut   ("SimpleTrack"         ,simpletrack  )) simpletrack     =    0;
  simpletrack ? m_simpleTrack = true : m_simpleTrack = false;

  int multitrack;
  if (!Cuts.getIntCut   ("doMultiTracksProd"   ,multitrack   )) multitrack      =    0;
 
  // Double cuts
  //
  if (!Cuts.getDoubleCut("pTmin"              ,m_pTmin      )) m_pTmin         = 500.;
  if (!Cuts.getDoubleCut("pTminBrem"          ,m_pTminBrem  )) m_pTminBrem     =1000.;

  if (!Cuts.getDoubleCut("MaxXi2forCluster"   ,m_xi2max     )) m_xi2max        =   9.;
  if (!Cuts.getDoubleCut("MaxXi2forOutlier"   ,m_xi2maxNoAdd)) m_xi2maxNoAdd   =  25.;
  if (!m_cosmicTrack) {
    if (m_xi2maxNoAdd > 25.) m_xi2maxNoAdd = 25.;
  }
  if (m_xi2maxNoAdd <= m_xi2max) m_xi2maxNoAdd = m_xi2max+5.;

  if (!Cuts.getDoubleCut("MaxXi2forSearch"    ,m_xi2maxlink )) m_xi2maxlink    = 100.;

  double xi2m;
  if (!Cuts.getDoubleCut("MaxXi2MultiTracks"  ,xi2m         )) xi2m            =   7.;
  if (!m_cosmicTrack) {
    if (xi2m > 7.) xi2m = 7.;
  }

  m_tools.setXi2pTmin(m_xi2max, m_xi2maxNoAdd, m_xi2maxlink, m_pTmin);
  m_tools.setHolesClusters(m_nholesmax, m_dholesmax, m_nclusmin);
  m_tools.setAssociation(useasso);
  m_tools.setMultiTracks(multitrack, xi2m);
  m_trajectory.setParameters();
}
