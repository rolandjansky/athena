/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTTRACKSIMWRAP_DUMPSP
#define FASTTRACKSIMWRAP_DUMPSP 

// FASTTRACKSIMWRAP
// ================================================================
// This algorithm is used to extract all needed information from
// Athena for use in the standalone FTK simulation. Eventually an
// Athena-based FTK simulation will exist and this code will
// disappear.
// ================================================================
// 2009-05-20 Antonio Boveia (boveia@hep.uchicago.edu)
//            access SDOs and dump GEANT truth for each silicon
//            channel. perform matching of offline reconstructed
//            tracks to truth using this info. substantial rewrite 
//            and cleanup.
// 2007-12-10 Giulio Usai <giulio.usai@cern.ch>
//            Monica Dunford <mdunford@hep.uchicago.edu>
//            original version.



#include <string>
#include <map>
#include <bitset>
#include <fstream>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/shared_ptr.hpp>

//#include "GaudiKernel/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
//#include "GaudiKernel/StatusCode.h"
//#include "GaudiKernel/ToolHandle.h"
//#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/ServiceHandle.h"
//#include "GaudiKernel/IJobOptionsSvc.h"

#include "GeneratorObjects/HepMcParticleLink.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkTrackSummaryTool/TrackSummaryTool.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"

class AtlasDetectorID;
class StoreGateSvc;
class ITruthParameters;
class TruthSelector; 
class PixelID; 
class SCT_ID;
class EventID;

namespace InDetDD {
  class PixelDetectorManager;
}
namespace HepPDT { 
  class ParticleDataTable; 
}
namespace HepMC {
  class GenParticle;
}

class
DumpSp : public AthAlgorithm
{   

public:
  
  typedef std::map<unsigned int,HepMcParticleLink::ExtendedBarCode> RecoToTruthTrackMap;
  typedef std::map<HepMcParticleLink::ExtendedBarCode,unsigned int> TruthToRecoTrackMap;
  typedef std::multimap<HepMcParticleLink::ExtendedBarCode,float> TruthToRecoProbMap;
  typedef std::map<Identifier,int> HitIndexMap;
  
public:
  
  DumpSp( const std::string& name , ISvcLocator* pSvcLocator );
  virtual ~DumpSp() {}
  
  StatusCode    initialize(void);    
  StatusCode    execute(void);
  StatusCode    finalize(void);

private:
  
  RecoToTruthTrackMap m_rttTrackMap; // mapping from reconstructed track to mc barcode
  TruthToRecoTrackMap m_ttrTrackMap; // mapping from mc barcode to reconstructed track (index into m_tracksName TrackCollection)
  TruthToRecoProbMap m_ttrProbMap;  // mapping from mc barcode to matching fraction for corresponding rttTrackMap entry

  typedef enum { TAU_PARENT_BIT , B_PARENT_BIT , PION_PARENT_BIT , PION_IMMEDIATE_PARENT_BIT , NBITS } Bits;
  typedef std::bitset<NBITS> ParentBitmask;

  void build_matching_maps(); // build geant truth to reconstruction maps for this event
  const ParentBitmask construct_truth_bitmap( const HepMC::GenParticle* p ) const;
  void dump_spacepoints() const; // dump clustered spacepoints to text file
  void dump_raw_silicon( HitIndexMap& hitIndexMap, HitIndexMap& pixelClusterIndexMap ) const; // dump raw silicon data to text file and populate hitIndexMap for rec. track processing
  void dump_tracks( const HitIndexMap& hitIndexMap, const HitIndexMap& pixelClusterIndexMap ) const; // dump reconstructed tracks in new truth matching format
  void dump_truth() const; // dump old track-truth matching format to text file
  void dump_beamspot() const; // dump beamspot to wrapper output
  void dump_bad_modules() const; // dump list of bad modules to the output
  void dump_MBTS( ) const; // dump MBTS data for data/MC occupancy studies
  void dump_vertex( ) const; // dump offline vertex info
  void dump_lumibcid( const EventID* evid ) const; // dump lumiblock Number and bcid

private:
  // tools
  // ================================================================

  
  boost::shared_ptr<AtlasDetectorID> m_idHelper;
  StoreGateSvc*  m_storeGate;
  StoreGateSvc*  m_detStore;
  StoreGateSvc*  m_evtStore;

  const PixelID*   m_pixelId;
  const SCT_ID*    m_sctId;

  const InDetDD::PixelDetectorManager*     m_PIX_mgr;

  const InDet::SiClusterContainer*  m_pixelContainer;
  const InDet::SiClusterContainer*  m_sctContainer;

  const HepPDT::ParticleDataTable*  m_particleDataTable;

  ToolHandle<Trk::ITruthToTrack>            m_truthToTrack; //!< tool to create track parameters from a gen particle
  ToolHandle<Trk::ITrackSummaryTool>        m_trkSummaryTool;

  //#ifdef HAVE_VERSION_15
  ToolHandle<Trig::TrigDecisionTool>        m_trigDecTool; // tool to retrieve trigger decision
  ServiceHandle<IInDetConditionsSvc>        m_pixelCondSummarySvc; // tool to retrieve pixel conditions db
  ToolHandle<IInDetConditionsTool>          m_sctCondSummaryTool{this, "SctSummaryTool",
      "SCT_ConditionsSummaryTool/InDetSCT_ConditionsSummaryTool", "Tool to retrieve SCT Conditions Summary"}; // tool to retrieve SCT conditions db
  ToolHandle<ISiLorentzAngleTool>           m_pixelLorentzAngleTool{this, "PixelLorentzAngleTool", "SiLorentzAngleTool/PixelLorentzAngleTool", "Tool to retreive Lorentz angle of Pixel"};
  ToolHandle<ISiLorentzAngleTool>           m_sctLorentzAngleTool{this, "SCTLorentzAngleTool", "SiLorentzAngleTool/SCTLorentzAngleTool", "Tool to retreive Lorentz angle of Pixel"};
  //#else
    // ToolHandle<Trk::TruthToTrack>             m_truthToTrack; //!< tool to create track parameters from a gen particle
  //#endif
  // ToolHandle<Trk::ITrackHoleSearchTool>     m_holeSearchTool;

  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  // job configuration 
  
  std::string    m_pixelClustersName;
  std::string    m_sctClustersName;
  std::string    m_pixelSpacePointsName;
  std::string    m_sctSpacePointsName;
  std::string    m_overlapSpacePointsName;
  std::string    m_tracksName;
  std::string    m_tracksTruthName;
  double         m_maxEta;
  double         m_minPt;

  std::string    m_spacePointsName;
  std::string    m_outFileName;
  std::string    m_outFileNameRawHits;

  bool           m_doTrigger;
  bool           m_doData;
  bool           m_doVertex;
  bool           m_doLumiBCID;
  bool           m_doBadMod;

  bool           m_dumpHitsOnTracks;
  bool           m_dumpSpacePoints;
  bool           m_dumpTruthIntersections;
  bool           m_dumpMBTSInfo;
  bool           m_useOfflineTrackSelectorTool;
  bool           m_outputBeamSpotToWrapper;
  bool           m_useSimpleCuts;

  bool           m_doRDODebug;


  // output
  // ================================================================

  mutable boost::shared_ptr<boost::iostreams::filtering_ostream> m_ofl;
  mutable boost::shared_ptr<boost::iostreams::filtering_ostream> m_oflraw;

};

#endif 
