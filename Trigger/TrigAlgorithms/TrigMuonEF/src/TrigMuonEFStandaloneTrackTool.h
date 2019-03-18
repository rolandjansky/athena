/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEF_TRIGMUONEFSTANDALONETRACKTOOL_H
#define TRIGMUONEF_TRIGMUONEFSTANDALONETRACKTOOL_H

#include <vector>

#include "SegmentCache.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkTrack/TrackCollection.h"

#include "TrigMuonToolInterfaces/ITrigMuonStandaloneTrackTool.h"
#include "TrigMuonToolInterfaces/TrigMuonEFMonVars.h"
#include "MuonRecToolInterfaces/IMuonTrackFinder.h"
#include "MuonRecToolInterfaces/IMuonCombiTrackMaker.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrigInterfaces/IMonitoredAlgo.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "IRegionSelector/RegSelEnums.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include <fstream>

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;  // legacy trigger code

#define DEBUG_ROI_VS_FULL false

/**
   @file TrigMuonEFStandaloneTrackTool.h
   @class TrigMuonEFStandaloneTrackTool

   @brief Tool to find segments for TrigMuonEF

   @author Robert Harrington <roberth@cern.ch>
   @author Martin Woudstra <martin.woudstra@cern.ch>
   @date 10/21/2011
 */

namespace HLT {
  class TriggerElement;
  class Algo;
}

namespace Muon {
  class IMooSegmentCombinationFinder;
  class IMuonPatternSegmentAssociationTool;
  class IMuonRdoToPrepDataTool;
  class IMuonTrackFinder;
  class MboyMooSegmentCombinationFinder;

}
namespace MuonCombined {
  class IMuonCandidateTool;
}

namespace Rec {
  class IMuidBackTracker;
}

namespace Trk {
  class ITrackSummaryTool;
  class ITrackParticleCreatorTool;
}

class ICscClusterBuilder;  
class TrigMuonEFInfoContainer;
class TrigTimer;
class StoreGateSvc;
class ActiveStoreSvc;
class IRegSelSvc;
class IIncidentSvc;

class MdtIdHelper;
class CscIdHelper;
class RpcIdHelper;
class TgcIdHelper;

class IRoiDescriptor;

class TrigMuonEFStandaloneTrackTool : public AthAlgTool, 
  public virtual ITrigMuonStandaloneTrackTool, 
  public virtual IIncidentListener
{
  
 public:
  
  TrigMuonEFStandaloneTrackTool(const std::string& type,
			      const std::string& name,
			      const IInterface* parent);
  
  virtual ~TrigMuonEFStandaloneTrackTool();
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
  /** Run segment finding only */
  virtual HLT::ErrorCode getSegments(const IRoiDescriptor* muonRoI,
				     TrigMuonEFMonVars& monvars,
				     std::vector<TrigTimer*>& timers);

  /** Run segment finding and spectrometer track building */
  virtual HLT::ErrorCode getSpectrometerTracks(const IRoiDescriptor* muonRoI,
					       TrigMuonEFMonVars& monVars,
					       std::vector<TrigTimer*>& timers);

  /** Run segment finding, spectrometer track building and extrapolation */
  virtual HLT::ErrorCode getExtrapolatedTracks(const IRoiDescriptor* muonRoI,
					       MuonCandidateCollection& candidateCollection,
					       TrackCollection& extrapolatedTracks,
					       TrigMuonEFMonVars& monvars,
					       std::vector<TrigTimer*>& timers);
  
  /** get list of hashIDs used in roi (used for internal caching in TrigMuSuperEF) **/
  virtual std::vector<std::vector<IdentifierHash> > getHashList(const IRoiDescriptor* muonRoI);

  /** return last created MuonPatternCombinationCollection. Caller is responsible for deletion of object.
      Call this function if you want to attach the object to the TriggerElement */
  virtual const MuonPatternCombinationCollection* patternCombisToAttach();

  /** return last created Trk::SegmentCollection. Caller is responsible for deletion of object.
      Call this function if you want to attach the object to the TriggerElement */
  virtual const Trk::SegmentCollection* segmentsToAttach();

  /** return last created TrackCollection with muon spectrometer tracks. Caller is responsible for deletion of object.
      Call this function if you want to attach the object to the TriggerElement */
  virtual const TrackCollection* spectrometerTracksToAttach();

  /** return last created TrackCollection with extrapolated tracks. Caller is responsible for deletion of object.
      Call this function if you want to attach the object to the TriggerElement */
  virtual const TrackCollection* extrapolatedTracksToAttach();

  /** return last created TrackParticleContainer with extrapolated tracks. Caller is responsible for deletion of object.
      Call this function if you want to attach the object to the TriggerElement */
  virtual const xAOD::TrackParticleContainer* trackParticleContainerToAttach();
  virtual const xAOD::TrackParticleAuxContainer* trackParticleAuxContainerToAttach();

  /** return last created MuonPatternCombinationCollection. Object will be deleted by tool at the end of the event.
      NB: You can not attach this object to the TriggerElement */
  virtual const MuonPatternCombinationCollection* patternCombis();

  /** return last created Trk::SegmentCollection. Object will be deleted by tool at the end of the event.
      NB: You can not attach this object to the TriggerElement */
  virtual const Trk::SegmentCollection* segments();

  /** return last created TrackCollection with muon spectrometer tracks. Object will be deleted by tool at the end of the event.
      NB: You can not attach this object to the TriggerElement */
  virtual const TrackCollection* spectrometerTracks();

  /** return last created TrackCollection with extrapolated tracks. Object will be deleted by tool at the end of the event.
      NB: You can not attach this object to the TriggerElement */
  virtual const TrackCollection* extrapolatedTracks();

  int segmentMonitoring(const std::vector<const Muon::MuonSegment*>& segmentVector,
			TrigMuonEFSegmentMonVars& monVars);
		         
  virtual void handle(const Incident &inc);

  virtual void recordPatterns();
  virtual void recordSpectrometerTracks();

  virtual void declareSegmentMonitoringVariables     (HLT::Algo* fexAlgo, TrigMuonEFMonVars& monVars);
  virtual void declareSpectrometerMonitoringVariables(HLT::Algo* fexAlgo, TrigMuonEFMonVars& monVars);
  virtual void declareExtrapolatedMonitoringVariables(HLT::Algo* fexAlgo, TrigMuonEFMonVars& monVars);

  virtual void setSegmentTimers     (HLT::Algo* fexAlgo, std::vector<TrigTimer*>& timers);
  virtual void setSpectrometerTimers(HLT::Algo* fexAlgo, std::vector<TrigTimer*>& timers);
  virtual void setExtrapolatedTimers(HLT::Algo* fexAlgo, std::vector<TrigTimer*>& timers);
  
  virtual const std::vector<uint32_t>& getMdtRobList( const IRoiDescriptor* muonRoI);
  virtual const std::vector<uint32_t>& getRpcRobList( const IRoiDescriptor* muonRoI);
  virtual const std::vector<uint32_t>& getTgcRobList( const IRoiDescriptor* muonRoI);
  virtual const std::vector<uint32_t>& getCscRobList( const IRoiDescriptor* muonRoI);
  virtual void clearRoiCache();

  virtual bool useMdtSeededDecoding() {return m_useMdtSeededDecoding;}
  virtual bool useRpcSeededDecoding() {return m_useRpcSeededDecoding;}
  virtual bool useTgcSeededDecoding() {return m_useTgcSeededDecoding;}
  virtual bool useCscSeededDecoding() {return m_useCscSeededDecoding;}
  
 private:

  void cleanSegmentCollections();


  /** Run segment finding only */
  HLT::ErrorCode getSegments(const IRoiDescriptor* muonRoI,
			     SegmentCache*& cache,
			     TrigMuonEFMonVars& monvars,
			     std::vector<TrigTimer*>& timers);
  
  /** Run segment finding and spectrometer track building */
  HLT::ErrorCode getSpectrometerTracks(const IRoiDescriptor* muonRoI,
				       SegmentCache*& cache,
				       TrigMuonEFMonVars& monVars,
				       std::vector<TrigTimer*>& timers);

  /** Run segment finding, spectrometer track building and extrapolation */
  HLT::ErrorCode getExtrapolatedTracks(const IRoiDescriptor* muonRoI,
				       MuonCandidateCollection& candidateCollection,
				       TrackCollection& extrapolatedTracks,
				       SegmentCache*& cache,
				       TrigMuonEFMonVars& monvars,
				       std::vector<TrigTimer*>& timers);
  

  /** Find segments in input ROI */
  HLT::ErrorCode findSegments(const IRoiDescriptor* muonRoI,
			      SegmentCache*& cache,
			      TrigMuonEFSegmentMonVars& monvars,
			      std::vector<TrigTimer*>& timers, unsigned int firstTimerIndex );

  /** Build tracks out of input segments */
  HLT::ErrorCode buildTracks(const Trk::SegmentCollection* segment_collection,
			     SegmentCache* cache,
			     TrigMuonEFMSMonVars& monVars,
			     std::vector<TrigTimer*>& timers, unsigned int firstTimerIndex );

  /** Extrapolate input track to IP */
  HLT::ErrorCode extrapolate(const xAOD::TrackParticleContainer* spectrometerTrackParticles,
			     MuonCandidateCollection& candidateCollection,			     
			     TrackCollection&extrapolatedTracks,
			     SegmentCache* cache,
			     TrigMuonEFSAMonVars& monvars,
			     std::vector<TrigTimer*>& timers, unsigned int firstTimerIndex );

#if DEBUG_ROI_VS_FULL
  void sanity_check(const std::vector<IdentifierHash>& input_hash_ids, const std::vector<IdentifierHash>& hash_ids_withData, std::ostream& outfile);
#endif
  
  void unpackTimers( std::vector<TrigTimer*>& timers, unsigned int firstIndex,
		     TrigTimer*& dataPrepTime,  TrigTimer*& algTime, TrigTimer*& dataOutputTime );

  // Region Selector
  ServiceHandle<IRegSelSvc> m_regionSelector; //<! pointer to RegionSelectionSvc

  // Pointers to sub-algoritms
  ToolHandle<Muon::IMooSegmentCombinationFinder>       m_segmentsFinderTool; //<! pointer to the segment finder
  ToolHandle<Muon::IMuonPatternSegmentAssociationTool> m_assocTool;          //<! pointer to association tool
  ToolHandle<Muon::IMuonTrackFinder>                   m_trackBuilderTool;   //<! pointer to the track finder in the 3rd chain implementation

  ToolHandle<Trk::ITrackSummaryTool >    m_trackSummaryTool;

  // handles to the RoI driven data access
  ToolHandle<Muon::IMuonRawDataProviderTool> m_mdtRawDataProvider;
  ToolHandle<Muon::IMuonRawDataProviderTool> m_rpcRawDataProvider;
  ToolHandle<Muon::IMuonRawDataProviderTool> m_tgcRawDataProvider;
  ToolHandle<Muon::IMuonRawDataProviderTool> m_cscRawDataProvider;
  ToolHandle<Muon::IMuonRdoToPrepDataTool> m_cscPrepDataProvider;
  ToolHandle<Muon::IMuonRdoToPrepDataTool> m_mdtPrepDataProvider;
  ToolHandle<Muon::IMuonRdoToPrepDataTool> m_rpcPrepDataProvider;
  ToolHandle<Muon::IMuonRdoToPrepDataTool> m_tgcPrepDataProvider;
  ToolHandle<ICscClusterBuilder>           m_cscClusterProvider;

  // Cache the ActiveStoreSvc ptr
  ActiveStoreSvc* p_ActiveStore;

  // Muon Id Helpers
  const CscIdHelper* m_cscIdHelper;
  const MdtIdHelper* m_mdtIdHelper;
  const RpcIdHelper* m_rpcIdHelper;
  const TgcIdHelper* m_tgcIdHelper;

  //Cache Rob Lists
  std::vector<uint32_t> m_MdtRobList;
  std::vector<uint32_t> m_RpcRobList;
  std::vector<uint32_t> m_TgcRobList;
  std::vector<uint32_t> m_CscRobList;

  bool m_HasMdtRobList;
  bool m_HasRpcRobList;
  bool m_HasTgcRobList;
  bool m_HasCscRobList;

  // true before first event done (used to initialize in first event)
  bool m_firstEvent;

  // job-opt properties

  // if true do caching
  bool m_doCache;
  
  // Flag to decide whether or not to run BS decoding
  Gaudi::Property< bool > m_decodeMdtBS { this, "DecodeMdtBS", true, "Flag to decide whether or not to run BS->RDO decoding for MTDs" };
  Gaudi::Property< bool > m_decodeRpcBS { this, "DecodeRpcBS", true, "Flag to decide whether or not to run BS->RDO decoding for RPCs" };
  Gaudi::Property< bool > m_decodeTgcBS { this, "DecodeTgcBS", true, "Flag to decide whether or not to run BS->RDO decoding for TGCs" };
  Gaudi::Property< bool > m_decodeCscBS { this, "DecodeCscBS", true, "Flag to decide whether or not to run BS->RDO decoding for CSCs" };

  bool m_useCscData;
  bool m_useRpcData;
  bool m_useTgcData;
  bool m_useTGCInPriorNextBC;
  unsigned int m_useMdtData;


  // decoding options
  bool m_useMdtSeededDecoding;
  bool m_useRpcSeededDecoding;
  bool m_useTgcSeededDecoding;
  bool m_useCscSeededDecoding;
  bool m_useTgcRobDecoding;
  bool m_useCscRobDecoding;
  
  //  timeout awareness
  bool m_doTimeOutChecks;

  // timeout guard (checks on number of hits)
  bool m_doTimeOutGuard;

  //Use L2 hits
  bool m_useL2Hits;

  // conditions to prevent events with excessive processing times
  int m_maxCscHits;
  int m_maxMdtHits;
  int m_maxRpcHits;
  int m_maxTgcHits;


  //Map to cache
  std::map<std::vector<std::vector<IdentifierHash> >, SegmentCache*> m_SFCacheSCMap;
  
  // list of IdentifierHash for PRD in RoI
  std::vector<std::vector<IdentifierHash> > m_hashlist;

  // features written to TriggerElement
  //  TrigMuonEFInfoContainer*                m_myMuonEFInfoCont;
  const MuonPatternCombinationCollection* m_patternCombiColl;
  const Trk::SegmentCollection*           m_segments;
  const TrackCollection*                  m_spectrometerTracks;
  const MuonPatternCombinationCollection* m_patternCombiCollInternal;
  const Trk::SegmentCollection*           m_segmentsInternal;
  const TrackCollection*                  m_spectrometerTracksInternal;
  TrackCollection*                        m_extrapolatedTracks;
  xAOD::TrackParticleContainer*           m_spectrometerTrackParticles;
  xAOD::TrackParticleAuxContainer*        m_spectrometerTrackParticlesAux;
  unsigned short int m_roi_num_seg;
  unsigned short int m_roi_num_mstrk;
  unsigned short int m_roi_num_satrk;

  unsigned int m_totalSegmentCalls;
  unsigned int m_cachedSegmentCalls;
  unsigned int m_totalSpectrometerCalls;
  unsigned int m_cachedSpectrometerCalls;
  unsigned int m_totalExtrapolatedCalls;
  unsigned int m_cachedExtrapolatedCalls;

  // tracks stored in cache for deletion at end of event
  std::vector<const TrackCollection*> m_spectrometerTracksCache;
  std::vector<TrackCollection*> m_extrapolatedTracksCache;
  std::vector<const MuonPatternCombinationCollection*> m_patternCombisCache;
  std::vector<const Trk::SegmentCollection*> m_segmentsCache;
  std::vector<Muon::MdtPrepDataCollection*> m_mdtcollCache;


  /// tool to create muon candidates
  ToolHandle<MuonCombined::IMuonCandidateTool> m_muonCandidateTool;

  /// tool to create track particles from Trk::Tracks
  ToolHandle<Trk::ITrackParticleCreatorTool> m_TrackToTrackParticleConvTool;

#if DEBUG_ROI_VS_FULL
  std::ofstream m_fileWithHashIds_rpc;
  std::ofstream m_fileWithHashIds_tgc;
  std::ofstream m_fileWithHashIds_mdt;
  std::ofstream m_fileWithHashIds_csc;
#endif

  SG::ReadHandleKey <Muon::RpcPrepDataContainer> m_rpcKey;
  SG::ReadHandleKey <Muon::TgcPrepDataContainer> m_tgcKey;
  SG::ReadHandleKey <Muon::TgcPrepDataContainer> m_tgcKeyNextBC;
  SG::ReadHandleKey <Muon::CscPrepDataContainer> m_cscKey;
  SG::ReadHandleKey <Muon::MdtPrepDataContainer> m_mdtKey;

  bool m_ignoreCSC;


};

#endif // TRIGMUONEF_TRIGMUONEFSTANDALONETRACKTOOL_H

