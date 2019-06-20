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
  : base_class(t, n, p)
{
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiCombinatorialTrackFinder_xk::initialize ATLAS_NOT_THREAD_SAFE ()
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
  if (not m_assoTool.empty()) {
    ATH_CHECK(m_assoTool.retrieve());
  } else {
    m_assoTool.disable();
  }

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
  std::vector< std::string > tagInfoKeys = detStore()->keys<TagInfo>();
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
  if (m_usePIX) {
    StatusCode sc = detStore()->regFcn(&InDet::SiCombinatorialTrackFinder_xk::mapDetectorElementsProduction,
                                       this, tagInfoH, m_callbackString);
    if (sc==StatusCode::SUCCESS) {
      ATH_MSG_INFO("Registered callback for geometry " << name());
    } else {
      ATH_MSG_ERROR("Could not book callback for geometry " << name ());
      return StatusCode::FAILURE;
    }
  }

  if ( !m_fieldServiceHandle.retrieve() ) {
    ATH_MSG_FATAL("Failed to retrieve " << m_fieldServiceHandle );
    return StatusCode::FAILURE;
  }    
  ATH_MSG_DEBUG("Retrieved " << m_fieldServiceHandle );

  // Setup callback for magnetic field
  //
  magneticFieldInit();

  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;

  if (m_usePIX) {
    ATH_CHECK( m_pixcontainerkey.initialize() );
  }
  if (m_useSCT) {
    ATH_CHECK( m_sctcontainerkey.initialize() );
    ATH_CHECK( m_boundarySCTKey.initialize() );
    ATH_CHECK( m_SCTDetEleCollKey.initialize() );
  }

  return StatusCode::SUCCESS;
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

MsgStream&  InDet::SiCombinatorialTrackFinder_xk::dump(SiCombinatorialTrackFinderData_xk& data, MsgStream& out) const
{
  if (not data.isInitialized()) initializeCombinatorialData(data);

  out<<std::endl;
  if (data.nprint()) return dumpevent(data, out);
  return dumpconditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiCombinatorialTrackFinder_xk::dumpconditions(MsgStream& out) const
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

  std::string s9;
  if (not m_assoTool.empty()) {
    n     = 62-m_assoTool.type().size();
    for (int i=0; i<n; ++i) s9.append(" ");
    s9.append("|");
  }

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
  if (not m_assoTool.empty()) {
    out<<"| Tool for track-prd assos| "<<m_assoTool   .type()<<s9<<std::endl;
  }
  out<<"| Magnetic field mode     | "<<fieldmode[mode]     <<s3<<std::endl;
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiCombinatorialTrackFinder_xk::dumpevent(SiCombinatorialTrackFinderData_xk& data, MsgStream& out) const
{
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  out<<"| Min pT of track (MeV)   | "<<std::setw(12)<<std::setprecision(5)<<data.pTmin()
     <<"                              |"<<std::endl;
  out<<"| Max Xi2 for cluster     | "<<std::setw(12)<<std::setprecision(5)<<data.xi2max()
     <<"                              |"<<std::endl;
  out<<"| Max Xi2 for outlayer    | "<<std::setw(12)<<std::setprecision(5)<<data.xi2maxNoAdd()
     <<"                              |"<<std::endl;
  out<<"| Max Xi2 for link        | "<<std::setw(12)<<std::setprecision(5)<<data.xi2maxlink()
     <<"                              |"<<std::endl;
  out<<"| Min number of clusters  | "<<std::setw(12)<<data.nclusmin()
     <<"                              |"<<std::endl;
  out<<"| Min number of wclusters | "<<std::setw(12)<<data.nwclusmin()
     <<"                              |"<<std::endl;
  out<<"| Max number holes        | "<<std::setw(12)<<data.nholesmax()
     <<"                              |"<<std::endl;
  out<<"| Max holes  gap          | "<<std::setw(12)<<data.dholesmax()
     <<"                              |"<<std::endl;
  out<<"| Use association tool ?  | "<<std::setw(12)<<data.tools().useassoTool()
     <<"                              |"<<std::endl;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  out<<"| Number input     seeds  | "<<std::setw(12)<<data.inputseeds()
     <<"                              |"<<std::endl;
  out<<"| Number accepted  seeds  | "<<std::setw(12)<<data.goodseeds()
     <<"                              |"<<std::endl;
  out<<"| Number initial  tracks  | "<<std::setw(12)<<data.inittracks()
     <<"                              |"<<std::endl;
  out<<"| Number wrong DE  roads  | "<<std::setw(12)<<data.roadbug()
     <<"                              |"<<std::endl;
  out<<"| Number output   tracks  | "<<std::setw(12)<<data.findtracks()  
     <<"                              |"<<std::endl;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Initiate track finding tool 
///////////////////////////////////////////////////////////////////

void InDet::SiCombinatorialTrackFinder_xk::newEvent(SiCombinatorialTrackFinderData_xk& data) const
{
  if (not data.isInitialized()) initializeCombinatorialData(data);

  // Erase statistic information
  //
  data.inputseeds() = 0;
  data.goodseeds()  = 0;
  data.inittracks() = 0;
  data.findtracks() = 0;
  data.roadbug()    = 0;

  // Set track info
  //
  data.trackinfo().setPatternRecognitionInfo(Trk::TrackInfo::SiSPSeededFinder);
  data.cosmicTrack() = 0;
}

///////////////////////////////////////////////////////////////////
// Initiate track finding tool
///////////////////////////////////////////////////////////////////

void InDet::SiCombinatorialTrackFinder_xk::newEvent
(SiCombinatorialTrackFinderData_xk& data, Trk::TrackInfo info, const TrackQualityCuts& Cuts) const
{
  if (not data.isInitialized()) initializeCombinatorialData(data);

  newEvent(data);
  data.trackinfo() = info;
  
  // Get track qulaity cuts information
  //
  getTrackQualityCuts(data, Cuts);

  data.heavyIon() = false;
  data.cosmicTrack() = 0;

  if (info.patternRecoInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_Cosmic)) {
    data.cosmicTrack() = 1;
  } else if (info.patternRecoInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_HeavyIon)) {
    data.heavyIon() = true;
  }
  data.tools().setHeavyIon(data.heavyIon());
}

///////////////////////////////////////////////////////////////////
// Finalize track finding tool for given event
///////////////////////////////////////////////////////////////////

void InDet::SiCombinatorialTrackFinder_xk::endEvent(SiCombinatorialTrackFinderData_xk& data) const
{
  if (not data.isInitialized()) initializeCombinatorialData(data);

  // Print event information 
  //
  if (m_outputlevel<=0) {
    data.nprint() = 1;
    dump(data, msg(MSG::DEBUG));
  }
}

///////////////////////////////////////////////////////////////////
// Main method for track finding using space points
///////////////////////////////////////////////////////////////////

const std::list<Trk::Track*>&  InDet::SiCombinatorialTrackFinder_xk::getTracks
(SiCombinatorialTrackFinderData_xk& data,
 const Trk::TrackParameters& Tp,
 const std::list<const Trk::SpacePoint*>& Sp,
 const std::list<Amg::Vector3D>& Gp,
 std::list<const InDetDD::SiDetectorElement*>& DE,
 const TrackQualityCuts& Cuts) const
{
  if (not data.isInitialized()) initializeCombinatorialData(data);

  data.tools().setBremNoise(false, false);
  data.tracks().erase(data.tracks().begin(), data.tracks().end());

  ++data.inputseeds();
  if (!m_usePIX && !m_useSCT) {
    return data.tracks();
  }

  // Get track qulaity cuts information
  //
  getTrackQualityCuts(data, Cuts);
  std::multimap<const Trk::PrepRawData*, const Trk::Track*> PT;
  if (!findTrack(data, Tp, Sp, Gp, DE, PT)) return data.tracks();
 
  data.trajectory().sortStep();

  // Trk::Track production
  //
  Trk::Track* t = convertToTrack(data);
  ++data.findtracks();
  data.tracks().push_back(t);

  if (!data.tools().multiTrack() || data.simpleTrack() || Sp.size()<=2 || data.cosmicTrack() || data.trajectory().pTfirst() < data.tools().pTmin()) return data.tracks();

  while ((t=convertToNextTrack(data))) {
    ++data.findtracks();
    data.tracks().push_back(t);
  }
  return data.tracks();
}

///////////////////////////////////////////////////////////////////
// Main method for track finding using space points
///////////////////////////////////////////////////////////////////

const std::list<Trk::Track*>& InDet::SiCombinatorialTrackFinder_xk::getTracks
(SiCombinatorialTrackFinderData_xk& data,
 const Trk::TrackParameters& Tp,
 const std::list<const Trk::SpacePoint*>& Sp,
 const std::list<Amg::Vector3D>& Gp,
 std::list<const InDetDD::SiDetectorElement*>& DE,
 std::multimap<const Trk::PrepRawData*, const Trk::Track*>& PT) const
{
  if (not data.isInitialized()) initializeCombinatorialData(data);

  data.tools().setBremNoise(false, false);
  data.tracks().erase(data.tracks().begin(), data.tracks().end());

  ++data.inputseeds();
  if (!m_usePIX && !m_useSCT) {
    return data.tracks();
  }

  if (!findTrack(data, Tp, Sp, Gp, DE, PT)) return data.tracks();
  if (!data.trajectory().isNewTrack(PT)) return data.tracks();

  data.trajectory().sortStep();

  // Trk::Track production
  //
  Trk::Track* t = convertToTrack(data);
  if (t==nullptr) return data.tracks();

  ++data.findtracks();
  data.tracks().push_back(t);

  if (!data.tools().multiTrack() || data.simpleTrack() || Sp.size()<=2 || data.cosmicTrack() || data.trajectory().pTfirst() < data.tools().pTmin()) return data.tracks();

  while ((t=convertToNextTrack(data))) {
    ++data.findtracks();
    data.tracks().push_back(t);
  } 
  return data.tracks();
}

///////////////////////////////////////////////////////////////////
// Main method for track finding using space points and 
// using electron noise model
///////////////////////////////////////////////////////////////////

const std::list<Trk::Track*>&  InDet::SiCombinatorialTrackFinder_xk::getTracksWithBrem
(SiCombinatorialTrackFinderData_xk& data,
 const Trk::TrackParameters& Tp,
 const std::list<const Trk::SpacePoint*>& Sp,
 const std::list<Amg::Vector3D>& Gp,
 std::list<const InDetDD::SiDetectorElement*>& DE,
 std::multimap<const Trk::PrepRawData*, const Trk::Track*>& PT,
 bool isCaloCompatible) const
{
  if (not data.isInitialized()) initializeCombinatorialData(data);

  // Old information
  //
  int mult = 0;
  if (data.tools().multiTrack()) mult = 1;
  double Xi2m = data.tools().xi2multi();

  data.tools().setBremNoise(false, true);
  data.tracks().erase(data.tracks().begin(), data.tracks().end());

  ++data.inputseeds();
  if (!m_usePIX && !m_useSCT) {
    return data.tracks();
  }

  bool Q = findTrack(data, Tp, Sp, Gp, DE, PT);
  if (Q) Q = data.trajectory().isNewTrack(PT);
  int na = 0;
  if (Q) {
    data.trajectory().sortStep();

    // Trk::Track production
    //
    Trk::TrackInfo oldinfo = data.trackinfo();
    if (isCaloCompatible) data.trackinfo().setPatternRecognitionInfo(Trk::TrackInfo::TrackInCaloROI);

    data.tools().setMultiTracks(0, Xi2m);
    Trk::Track* t = convertToTrack(data);
    data.trackinfo() = oldinfo;
    data.tools().setMultiTracks(mult,Xi2m);

    if (!t) return data.tracks();
    ++data.findtracks();
    data.tracks().push_back(t);
    na = data.trajectory().nclusters();
    if (na >=12 && !data.trajectory().nclustersNoAdd()) return data.tracks();
   
    if (data.trajectory().pTfirst() < data.pTminBrem()) return data.tracks();
  }
  if ((*Sp.begin())->clusterList().second) return data.tracks();
 
  // Repeat track finding using electron noise model
  //
  data.tools().setBremNoise(true,true);
  if (!findTrack(data, Tp, Sp, Gp, DE, PT)) return data.tracks();
  if (!data.trajectory().isNewTrack(PT)) return data.tracks();
  
  int nb = data.trajectory().nclusters();
  if (nb <= na) return data.tracks();
  
  data.trajectory().sortStep();

  // Trk::Track production
  //
  Trk::TrackInfo oldinfo = data.trackinfo();
  data.trackinfo().setTrackProperties(Trk::TrackInfo::BremFit          );
  data.trackinfo().setTrackProperties(Trk::TrackInfo::BremFitSuccessful);
  if (isCaloCompatible) data.trackinfo().setPatternRecognitionInfo(Trk::TrackInfo::TrackInCaloROI);

  data.tools().setMultiTracks(0, Xi2m);
  Trk::Track* t = convertToTrack(data);
  data.trackinfo() = oldinfo;
  data.tools().setMultiTracks(mult, Xi2m);

  if (t==nullptr) return data.tracks();

  ++data.findtracks();
  data.tracks().push_back(t);
  return data.tracks();
}

///////////////////////////////////////////////////////////////////
// Main method for track finding using space points
///////////////////////////////////////////////////////////////////

bool InDet::SiCombinatorialTrackFinder_xk::findTrack
(SiCombinatorialTrackFinderData_xk& data,
 const Trk::TrackParameters& Tp,
 const std::list<const Trk::SpacePoint*>& Sp,const std::list<Amg::Vector3D>& Gp,
 std::list<const InDetDD::SiDetectorElement*>& DE,
 std::multimap<const Trk::PrepRawData*,const Trk::Track*>& PT) const
{
  if (not data.isInitialized()) initializeCombinatorialData(data);

  // List detector element links preparation
  //
  std::list<const InDet::SiDetElementBoundaryLink_xk*> DEL;
  detectorElementLinks(DE, DEL);

  const InDet::SiClusterContainer* p_pixcontainer = nullptr;
  if (m_usePIX) {
    SG::ReadHandle<InDet::PixelClusterContainer> pixcontainer(m_pixcontainerkey);
    p_pixcontainer = reinterpret_cast<const InDet::SiClusterContainer*>(pixcontainer.ptr());
  }
  const InDet::SiClusterContainer* p_sctcontainer = nullptr;
  if (m_useSCT) {
    SG::ReadHandle<InDet::SCT_ClusterContainer> sctcontainer(m_sctcontainerkey);
    p_sctcontainer = reinterpret_cast<const InDet::SiClusterContainer*>(sctcontainer.ptr());
  }

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
    if (!data.trajectory().globalPositionsToClusters(p_pixcontainer, p_sctcontainer, Gp, DEL, PT, Cl)) return false;
  } else {
    if (!data.trajectory().trackParametersToClusters(p_pixcontainer, p_sctcontainer, Tp, DEL, PT, Cl)) return false;
  }
  ++data.goodseeds();

  // Build initial trajectory
  //
  bool Qr;
  bool Q = data.trajectory().initialize(m_usePIX, m_useSCT, p_pixcontainer, p_sctcontainer, Tp, Cl, DEL, Qr);

  if (!Q && Sp.size() < 2 && Gp.size() > 3) {

    Cl.clear();
    if (!data.trajectory().trackParametersToClusters(p_pixcontainer, p_sctcontainer, Tp, DEL, PT, Cl)) return false;
    if (!data.trajectory().initialize(m_usePIX, m_useSCT, p_pixcontainer, p_sctcontainer, Tp, Cl, DEL, Qr)) return false;
    Q = Qr = true;
  }

  if (!Qr) ++data.roadbug();
  if (!Q) return false;
  ++data.inittracks();
  bool pixseed = data.trajectory().isLastPixel();
  int itmax    = 30;
  if (data.simpleTrack()) itmax = 10;
  if (data.heavyIon()) itmax = 50;

  // Track finding
  //
  if (pixseed) {      // Strategy for pixel seeds
    if (!data.trajectory().forwardExtension (false,itmax)) return false;
    if (!data.trajectory().backwardSmoother (false)      ) return false;
    if (!data.trajectory().backwardExtension(itmax)      ) return false;

    if (data.trajectory().difference() > 0) {
      if (!data.trajectory().forwardFilter()          ) return false;
      if (!data.trajectory().backwardSmoother (false) ) return false;
    } 
    int na = data.trajectory().nclustersNoAdd();
    if (data.trajectory().nclusters()+na < data.nclusmin() || data.trajectory().ndf() < data.nwclusmin()) return false;
  } else {      // Strategy for mixed seeds
    if (!data.trajectory().backwardSmoother(TWO)       ) return false;
    if (!data.trajectory().backwardExtension(itmax)    ) return false;
    if (!data.trajectory().forwardExtension(true,itmax)) return false;

    int na = data.trajectory().nclustersNoAdd();
    if (data.trajectory().nclusters()+na < data.nclusmin() || data.trajectory().ndf() < data.nwclusmin()) return false;
    if (!data.trajectory().backwardSmoother(false)    ) return false;

    na     = data.trajectory().nclustersNoAdd();
    if (data.trajectory().nclusters()+na < data.nclusmin() || data.trajectory().ndf() < data.nwclusmin()) return false;

    if (data.trajectory().difference() > 0) {
      if (!data.trajectory().forwardFilter()         ) return false;
      if (!data.trajectory().backwardSmoother (false)) return false;
    }
  } 

  if (data.trajectory().qualityOptimization()     <           (m_qualityCut*data.nclusmin())    ) return false;
  if (data.trajectory().pTfirst  () < data.pTmin()     && data.trajectory().nclusters() < data.nclusmin() ) return false;
  if (data.trajectory().nclusters() < data.nclusminb() || data.trajectory().ndf      () < data.nwclusmin()) return false;
  
  return true;
}

///////////////////////////////////////////////////////////////////
// Trk::Track production
///////////////////////////////////////////////////////////////////

Trk::Track* InDet::SiCombinatorialTrackFinder_xk::convertToTrack(SiCombinatorialTrackFinderData_xk& data) const
{
  if (!data.simpleTrack()) {
    return new Trk::Track(data.trackinfo(),
                          data.trajectory().convertToTrackStateOnSurface(data.cosmicTrack()),
                          data.trajectory().convertToFitQuality());
  }

  Trk::TrackInfo info = data.trackinfo();
  info.setPatternRecognitionInfo(Trk::TrackInfo::SiSPSeededFinderSimple);
  return new Trk::Track(info,
                        data.trajectory().convertToSimpleTrackStateOnSurface(data.cosmicTrack()),
                        data.trajectory().convertToFitQuality());
}

///////////////////////////////////////////////////////////////////
// Next Trk::Track production
///////////////////////////////////////////////////////////////////

Trk::Track* InDet::SiCombinatorialTrackFinder_xk::convertToNextTrack(SiCombinatorialTrackFinderData_xk& data) const
{
  DataVector<const Trk::TrackStateOnSurface>* tsos{data.trajectory().convertToNextTrackStateOnSurface()};
  if (tsos==nullptr) return nullptr;
  return new Trk::Track(data.trackinfo(),
                        tsos,
                        data.trajectory().convertToFitQuality());
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
}

///////////////////////////////////////////////////////////////////
// Map of detector elements production
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiCombinatorialTrackFinder_xk::mapDetectorElementsProduction 
(IOVSVC_CALLBACK_ARGS_P(/*I*/, keys))
{
  // Get  Pixel Detector Manager
  //
  const InDetDD::PixelDetectorManager* pixmgr = nullptr;
  StatusCode sc = detStore()->retrieve(pixmgr, m_pixm);
  if (sc.isFailure() || !pixmgr) {
    ATH_MSG_FATAL("Could not get PixelDetectorManager  !");
      return StatusCode::FAILURE;
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

  if (detStore()->retrieve(IDp, "PixelID").isFailure()) {
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
  const InDet::SiDetElementBoundaryLinks_xk* boundarySCT{nullptr};
  if (m_useSCT) {
    SG::ReadCondHandle<InDet::SiDetElementBoundaryLinks_xk> boundarySCTHandle(m_boundarySCTKey);
    boundarySCT = *boundarySCTHandle;
    if (boundarySCT==nullptr) {
      ATH_MSG_FATAL(m_boundarySCTKey.fullKey() << " returns null pointer");
    }
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
(SiCombinatorialTrackFinderData_xk& data, const TrackQualityCuts& Cuts) const
{
  // Integer cuts
  //
  if (!Cuts.getIntCut   ("CosmicTrack"         ,data.cosmicTrack())) data.cosmicTrack()   =    0;

  if (!Cuts.getIntCut   ("MinNumberOfClusters" ,data.nclusmin()   )) data.nclusmin()      =    7;
  data.nclusminb() = data.nclusmin()-1;
  if (data.nclusminb() < 3) data.nclusminb() = 3;

  if (!Cuts.getIntCut   ("MinNumberOfWClusters",data.nwclusmin()  )) data.nwclusmin()     =    7;

  if (!Cuts.getIntCut   ("MaxNumberOfHoles"    ,data.nholesmax()  )) data.nholesmax()     =    2;
  if (!Cuts.getIntCut   ("MaxHolesGae"         ,data.dholesmax()  )) data.dholesmax()     =    2;
  if (!data.cosmicTrack()) {
    if (data.nholesmax() > 2) data.nholesmax() = 2;
    if (data.dholesmax() > 2) data.dholesmax() = 2;
  }
  if (data.dholesmax() > data.nholesmax()) data.dholesmax() = data.nholesmax();

  int useasso;
  if (!Cuts.getIntCut   ("UseAssociationTool"  ,useasso      )) useasso         =    0;

  int simpletrack;
  if (!Cuts.getIntCut   ("SimpleTrack"         ,simpletrack  )) simpletrack     =    0;
  simpletrack ? data.simpleTrack() = true : data.simpleTrack() = false;

  int multitrack;
  if (!Cuts.getIntCut   ("doMultiTracksProd"   ,multitrack   )) multitrack      =    0;
 
  // Double cuts
  //
  if (!Cuts.getDoubleCut("pTmin"              ,data.pTmin()      )) data.pTmin()         = 500.;
  if (!Cuts.getDoubleCut("pTminBrem"          ,data.pTminBrem()  )) data.pTminBrem()     =1000.;

  if (!Cuts.getDoubleCut("MaxXi2forCluster"   ,data.xi2max()     )) data.xi2max()        =   9.;
  if (!Cuts.getDoubleCut("MaxXi2forOutlier"   ,data.xi2maxNoAdd())) data.xi2maxNoAdd()   =  25.;
  if (!data.cosmicTrack()) {
    if (data.xi2maxNoAdd() > 25.) data.xi2maxNoAdd() = 25.;
  }
  if (data.xi2maxNoAdd() <= data.xi2max()) data.xi2maxNoAdd() = data.xi2max()+5.;

  if (!Cuts.getDoubleCut("MaxXi2forSearch"    ,data.xi2maxlink() )) data.xi2maxlink()    = 100.;

  double xi2m;
  if (!Cuts.getDoubleCut("MaxXi2MultiTracks"  ,xi2m         )) xi2m            =   7.;
  if (!data.cosmicTrack()) {
    if (xi2m > 7.) xi2m = 7.;
  }

  data.tools().setXi2pTmin(data.xi2max(), data.xi2maxNoAdd(), data.xi2maxlink(), data.pTmin());
  data.tools().setHolesClusters(data.nholesmax(), data.dholesmax(), data.nclusmin());
  data.tools().setAssociation(useasso);
  data.tools().setMultiTracks(multitrack, xi2m);
  data.trajectory().setParameters();
}

void InDet::SiCombinatorialTrackFinder_xk::initializeCombinatorialData(SiCombinatorialTrackFinderData_xk& data) const {
  data.setTools(&*m_proptool,
                &*m_updatortool,
                &*m_riocreator,
                ((not m_assoTool.empty()) ? &*m_assoTool : nullptr),
                &*m_fieldServiceHandle,
                (m_usePIX ? &*m_pixelCondSummaryTool : nullptr),
                (m_useSCT ? &*m_sctCondSummaryTool : nullptr),
                &m_fieldprop);
}
