/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Wrapper algorithm to make a smart "OR" of TrigMuonEF and TrigMuGirl
// Authors: Martin Woudstra, Joel Klinger, Mark Owen

#ifndef TRIGMUSUPEREF_TRIGMUSUPEREF_H
#define TRIGMUSUPEREF_TRIGMUSUPEREF_H

#include <string>
#include <vector>
#include <map>

#include "TrigInterfaces/Algo.h"
#include "TrigInterfaces/FexAlgo.h"

#include "TrigNavigation/NavigationCore.icc"

#include "GaudiKernel/System.h"

#include "TrigConfHLTData/HLTTriggerElement.h"

#include "TrigMuonToolInterfaces/TrigMuonEFMonVars.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedTrigCaloTagExtensionTool.h"

// EDM includes
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/SlowMuonContainer.h"
#include "Particle/TrackParticleContainer.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidateToTagMap.h"
#include "TrkSegment/SegmentCollection.h"

#include "InternalCache.h"

class IRoiDescriptor;
class TrigRoiDescriptor;
class TrigTimer;


class ITrigMuonStandaloneTrackTool;

namespace Trk {
  class ITrackSummaryTool;
  class ITrackParticleCreatorTool;
}

namespace MuonCombined {
  class IMuonCombinedTool;
  class IMuonCreatorTool;
  class IMuonCombinedInDetExtensionTool;
  class IMuonCombinedTrigCaloTagExtensionTool;
  class IMuonSegmentTagTool;
}

namespace Muon {
  class IMuonSystemExtensionTool;
}


class TrigMuSuperEF: public virtual HLT::FexAlgo {
 public:

  TrigMuSuperEF (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrigMuSuperEF();

  virtual HLT::ErrorCode hltInitialize();
  virtual HLT::ErrorCode hltExecute(const HLT::TriggerElement*, HLT::TriggerElement*);
  virtual HLT::ErrorCode hltEndEvent();
  virtual HLT::ErrorCode hltFinalize();

  using HLT::FexAlgo::prepareRobRequests;
  virtual HLT::ErrorCode prepareRobRequests(const HLT::TriggerElement* input );


 private:

  struct PassCounters {
  PassCounters() : total(0), pass(0) {};
    unsigned int total;
    unsigned int pass;
  };
  

  void clearRoiCache();

  // run the standard chain of MS (possible stopping at that point), then CB
  HLT::ErrorCode runStandardChain(const HLT::TriggerElement*, HLT::TriggerElement*, std::unique_ptr<xAOD::MuonContainer>& muonContainerOwn);

  HLT::ErrorCode runStandAlone(const IRoiDescriptor*, HLT::TriggerElement*, MuonCandidateCollection*, std::unique_ptr<xAOD::MuonContainer>& muonContainerOwn);
  
  // run in combiner only mode
  HLT::ErrorCode runCombinerOnly(const HLT::TriggerElement* inputTE, HLT::TriggerElement* TEout, std::unique_ptr<xAOD::MuonContainer>& muonContainerOwn);

  // run in caloTag mode
  HLT::ErrorCode runCaloTagOnly(const HLT::TriggerElement* inputTE, HLT::TriggerElement* TEout, std::unique_ptr<xAOD::MuonContainer>& muonContainerOwn);

  // run the MS+CB reconstruction of TrigMuonEF
  HLT::ErrorCode runMSCBReconstruction(const IRoiDescriptor* muonRoI,
				       HLT::TriggerElement* TEout, 
				       MuonCandidateCollection& muonCandidates,
				       InDetCandidateCollection& inDetCandidates, std::unique_ptr<xAOD::MuonContainer>& muonContainerOwn);

  // run the MS-only reconstruction
  HLT::ErrorCode runMSReconstruction(const IRoiDescriptor* muonRoI, HLT::TriggerElement* TEout, MuonCandidateCollection& muonCandidates, std::unique_ptr<xAOD::MuonContainer>& muonContainerOwn);

  HLT::ErrorCode runSegmentTaggedOnly(const HLT::TriggerElement* inputTE, HLT::TriggerElement* TEout, std::unique_ptr<xAOD::MuonContainer> & muonContainer);

  HLT::ErrorCode runMSSegmentTaggedReconstruction(HLT::TriggerElement* TEout, 
						  MuonCandidateCollection& muonCandidates,
						  InDetCandidateCollection& inDetCandidates, std::unique_ptr<xAOD::MuonContainer>& muonContainerOwn);

  /// Function to get ID track particle links from trigger element
  HLT::ErrorCode getIDTrackParticleLinks(const HLT::TriggerElement* te, ElementLinkVector<xAOD::TrackParticleContainer>& elv_xaodidtrks) ;
  HLT::ErrorCode getIDTrackParticleLinksL2(const HLT::TriggerElement* te, ElementLinkVector<xAOD::TrackParticleContainer>& elv_xaodidtrks) ;

  /// Function to build combined tracks

  HLT::ErrorCode buildCombinedTracks(const MuonCandidateCollection* muonCandidates,
				     InDetCandidateCollection& inDetCandidates,
				     TrigMuonEFCBMonVars& monVars,
				     std::vector<TrigTimer*>& timers);

  /// Function to segment tagged candidates tracks
  HLT::ErrorCode buildSegmentTaggedTracks(InDetCandidateCollection& inDetCandidates);

  // function to build xAOD muons
  HLT::ErrorCode buildMuons(const MuonCandidateCollection* muonCandidates,
			    InDetCandidateCollection* inDetCandidates,
			    xAOD::TrackParticleContainer* combinedTrackParticles,
			    xAOD::TrackParticleContainer* extrapolatedTrackParticles);
  
  // attach all the different output collections to the TE
  HLT::ErrorCode attachOutput(HLT::TriggerElement* TEout,
			      xAOD::TrackParticleContainer* combinedTrackParticles,
			      xAOD::TrackParticleContainer* extrapolatedTrackParticles, std::unique_ptr<xAOD::MuonContainer> muonContainerOwn);

  HLT::ErrorCode attachMuonSegmentCombinationCollection(HLT::TriggerElement* TEout);
  HLT::ErrorCode attachSegments(HLT::TriggerElement* TEout);
  HLT::ErrorCode attachMSTracks(HLT::TriggerElement* TEout);
  HLT::ErrorCode attachTrackParticleContainer(HLT::TriggerElement* TEout);
  /// Utility function to attached xAOD track particle container to the trigger element
  void attachTrackParticleContainer( HLT::TriggerElement* TEout, const xAOD::TrackParticleContainer* trackParticleCont, const std::string& name);
  void setCombinedTimers(HLT::Algo* fexAlgo, std::vector<TrigTimer*>& timers);

  const IRoiDescriptor* getRoiDescriptor(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);

  TrigRoiDescriptor* createSingleTrigRoiDescriptor( double eta, double phi, double etaWidth, double phiWidth, int roiNum );

  void buildInDetCandidates(const ElementLinkVector<xAOD::TrackParticleContainer>&, InDetCandidateCollection*);
  bool buildL2InDetCandidates(const ElementLinkVector<xAOD::TrackParticleContainer>&, InDetCandidateCollection*, const xAOD::L2CombinedMuonContainer*);
  void runMuGirl(const ElementLinkVector<xAOD::TrackParticleContainer>&, InDetCandidateCollection*);

  bool retrieveFromCache(std::map<std::vector<std::vector<IdentifierHash> >, InternalCache*>::iterator itmap, MuonCandidateCollection* muonCandidates, xAOD::TrackParticleContainer* combTrackParticleCont, xAOD::TrackParticleContainer* saTrackParticleCont);

  void retrieveTrackContainersFromCache(InternalCache* cache, xAOD::TrackParticleContainer* combTrackParticleCont, xAOD::TrackParticleContainer* saTrackParticleCont);

  HLT::ErrorCode rebuildCache(const IRoiDescriptor*, HLT::TriggerElement*, MuonCandidateCollection* muonCandidates, InDetCandidateCollection* inDetCandidates, xAOD::TrackParticleContainer* combTrackParticleCont, xAOD::TrackParticleContainer* saTrackParticleCont, std::unique_ptr<xAOD::MuonContainer>& muonContainerOwn, const ElementLinkVector<xAOD::TrackParticleContainer>& elv_idtrks );

  HLT::ErrorCode runOutsideInOnly(const IRoiDescriptor*, HLT::TriggerElement*, MuonCandidateCollection* muonCandidates, InDetCandidateCollection* inDetCandidates, std::unique_ptr<xAOD::MuonContainer>& muonContainerOwn, const ElementLinkVector<xAOD::TrackParticleContainer>& elv_idtrks);
  void printCounterStats( const PassCounters& counter, const std::string& source, int precision = 3 ) const;
  void printTrigMuGirlStats() const;
  void printTrigMuonEFStats() const;
  void printTotalStats() const;
  void printTEStats() const;

  // function to declare combined muon monitoring variables
  void declareCombinedMonitoringVariables(TrigMuonEFCBMonVars& monVars);

  // function to declare caloTag muon monitoring variables
  void declareCaloTagMonitoringVariables(TrigMuonCaloTagMonVars& monVars);

  void fillMonitoringVars( );
  void fillIDMonitoringVars( const ElementLinkVector<xAOD::TrackParticleContainer>& elv_idtrks );
  void fillCTMonitoringVars( const xAOD::TrackParticleContainer& idTrks );
  void fillCBMonitoringVars();

  void ResetTimers(std::vector<TrigTimer*>& timers);

  ToolHandle<ITrigMuonStandaloneTrackTool> m_TrigMuonEF_saTrackTool;

  const IRoiDescriptor* m_roi; // to cache actually used RoI
  TrigRoiDescriptor* m_Roi_StillToBeAttached;
  TrigTimer* m_totalTime;

  //Maps to muon tags
  std::vector<MuonCombined::InDetCandidateToTagMap*> m_tagMaps;

  // Output xAOD muons
  xAOD::MuonContainer* m_muonContainer;
  //xAOD::SlowMuonContainer* m_slowMuonContainer;

  // Output tagged TrackParticles
  xAOD::TrackParticleContainer* m_ctTrackParticleContainer;

  // Output combined TrackCollection
  TrackCollection* m_combTrkTrackColl;

  //Output extrapolated TrackCollection
  TrackCollection* m_extrTrkTrackColl;

  /// holder for containers that we create and may need to be deleted at end of event
  std::vector< TrackCollection* > m_tracksCache;

  //collection for segments created by MuGirl and MuonStau
  Trk::SegmentCollection* m_muGirlTrkSegColl;

  // holder for segment containers that may need to be deleted
  std::vector<Trk::SegmentCollection*> m_segsCache;

  //Trigger pass/fail counts
  PassCounters m_counter_TrigMuonEF;
  PassCounters m_counter_TrigMuGirl;
  PassCounters m_counter_CaloTag;
  PassCounters m_counter;
  std::map<unsigned int,PassCounters> m_counter_perTE;  // use TE id as key

  //// Monitoring Histograms

  TrigMuonEFCBMonVars m_monVars;

  TrigMuonEFMonVars m_TMEF_monVars;
  TrigMuGirlMonVars   m_TrigMuGirl_monVars;
  TrigMuonCaloTagMonVars m_TrigCaloTag_monVars;

  bool m_doInsideOut; // run TrigMuGirl or not
  bool m_doOutsideIn; // run TrigMuonEF or not
  bool m_insideOutFirst; // run TrigMuGirl first (true) or run TrigMuonEF first (false)
  bool m_forceBoth;
  bool m_singleContainer;
  bool m_standaloneOnly; // run only the MS reconstruction
  bool m_combinerOnly; // run only the combiner - MS reco must have been run previously
  bool m_caloTagOnly;
  bool m_segmentTagOnly;

  bool m_fullScan;
  bool m_recordSegments;
  bool m_recordPatterns;
  bool m_recordTracks;

  float  m_dEtaRoI;
  float  m_dPhiRoI;

  std::string m_idTrackParticlesName;

  std::string m_saTrackParticleContName; /// Name of the xAOD track particle container for extrapolated tracks
  std::string m_msTrackParticleContName; /// Name of the xAOD track particle container for muon spectrometer tracks
  std::string m_cbTrackParticleContName; /// Name of the xAOD track particle container for combined tracks
  std::string m_ctTrackParticleContName; /// Name of the xAOD track particle container for caloTag tracks

  std::string m_muonContName; // Name of the xAOD muon container

  bool m_debug; // handy flag for conditional DEBUG output
  bool m_verbose; // handy flag for conditional VERBOSE output

  // Pointers to tools
  ToolHandle<Trk::ITrackSummaryTool > m_trackSummaryTool;
  ToolHandle<MuonCombined::IMuonCombinedTool> m_muonCombinedTool;
  ToolHandle<MuonCombined::IMuonCombinedInDetExtensionTool> m_muGirlTool;
  ToolHandle<MuonCombined::IMuonCombinedTrigCaloTagExtensionTool> m_caloTagTool;
  ToolHandle<MuonCombined::IMuonSegmentTagTool> m_muonSegmentTagTool;

  /// tool to create track particles from Trk::Tracks
  ToolHandle<Trk::ITrackParticleCreatorTool> m_TrackToTrackParticleConvTool;

  /// tool for creating xAOD muons
  ToolHandle<MuonCombined::IMuonCreatorTool> m_muonCreatorTool;
  ToolHandle<MuonCombined::IMuonCreatorTool> m_stauCreatorTool;

  /// tool for extending ID tracks to the muon system
  ToolHandle <Muon::IMuonSystemExtensionTool> m_muonSystemExtensionTool;

  //Timers
  std::vector<TrigTimer*> m_TMEF_SATimers;
  std::vector<TrigTimer*> m_TMEF_STTimers;
  std::vector<TrigTimer*> m_TMEF_CBTimers;
  std::vector<TrigTimer*> m_TrigMuGirl_Timers;

  bool m_doMuonFeature;
  bool m_useL2Info;
  bool m_doCache;

  //Map to cache
  std::map<std::vector<std::vector<IdentifierHash> >, InternalCache*> m_CacheMap;
  std::map<std::vector<std::vector<IdentifierHash> >, InternalCache*> m_CacheMapTMEFonly;
  
  // list of IdentifierHash for PRD in RoI
  std::vector<std::vector<IdentifierHash> > m_hashlist;

};

#endif // TRIGMUSUPEREF_TRIGMUSUPEREF_H
