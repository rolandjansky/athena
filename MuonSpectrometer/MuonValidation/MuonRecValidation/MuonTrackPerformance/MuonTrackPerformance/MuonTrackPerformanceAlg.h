/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKPERFORMANCEALG_H
#define MUONTRACKPERFORMANCEALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODEventInfo/EventInfo.h"

#include "MuonRecToolInterfaces/IMuonTrackTruthTool.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/MuonTrackSummary.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTruthData/TruthTrajectory.h"

#include "MuonIdHelpers/MuonStationIndex.h"

#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "TrkTrack/TrackCollection.h"
#include "StoreGate/ReadHandleKey.h"
#include "GeneratorObjects/McEventCollection.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <set>

class MsgStream;
class TFile;
class TruthTrajectory;

namespace HepMC{
  class GenParticle;
}

namespace Trk {
  class Track;
  class TrackSummary;
  class ITrackSummaryHelperTool;  
}

namespace Muon {
  class MuonIdHelperTool;
  class MuonEDMPrinterTool;
  class MuonEDMHelperTool;
  class MuonSegmentCombination;
  class MuonSegment;
}


class MuonTrackPerformanceAlg : public AthAlgorithm
{
public:
  struct ChamberData {
    Identifier chId;
    std::set<Identifier> hits;
  };

  struct TrackData{
    TrackData() : truthTrack(0),motherPdg(-1),chi2Ndof(0.),productionVertex(0),momentumAtProduction(0),truthTrajectory(0),trackPars(0),trackSummary(0) {}

    ~TrackData() {
      delete trackPars;
      delete trackSummary;
      delete truthTrack;
      delete truthTrajectory;
      delete productionVertex;
      delete momentumAtProduction;
    }

    TrackData( const TrackData& data ){
      motherPdg = data.motherPdg;
      chi2Ndof = data.chi2Ndof;
      trackPars = data.trackPars ? new Trk::Perigee(*data.trackPars) : 0; 
      trackSummary = data.trackSummary ? new Trk::TrackSummary(*data.trackSummary) : 0; 
      truthTrack = data.truthTrack ? new TrackRecord(*data.truthTrack) : 0; 
      productionVertex = data.productionVertex ? new Amg::Vector3D(*data.productionVertex) : 0;
      momentumAtProduction = data.momentumAtProduction ? new Amg::Vector3D(*data.momentumAtProduction) : 0;
    }


    TrackRecord*       truthTrack;
    int motherPdg;
    double chi2Ndof;
    Amg::Vector3D*  productionVertex;
    Amg::Vector3D*  momentumAtProduction;
    TruthTrajectory*   truthTrajectory;
    Trk::Perigee*      trackPars;
    Trk::TrackSummary* trackSummary;

    std::vector<ChamberData> missingChambers;
    std::vector<ChamberData> wrongChambers;
    
    std::set<Muon::MuonStationIndex::StIndex> layers;
    std::set<Muon::MuonStationIndex::StIndex> missingLayers;
    std::set<Muon::MuonStationIndex::StIndex> missingCscLayers;
    std::set<Muon::MuonStationIndex::StIndex> wrongLayers;

    std::set<Muon::MuonStationIndex::StIndex> layersTrigger;
    std::set<Muon::MuonStationIndex::StIndex> missingLayersTrigger;
    std::set<Muon::MuonStationIndex::StIndex> wrongLayersTrigger;

    bool isEndcapSLTrack() const {
      if( layers.size() == 1 && layers.count(Muon::MuonStationIndex::EM) ) return true;
      if( layers.size() == 2 && layers.count(Muon::MuonStationIndex::EM) && layers.count(Muon::MuonStationIndex::EO) ) return true;
      return false;
    }

    bool hasMissingChambers() const {
      return !missingChambers.empty();
    }

    bool hasWrongChambers() const {
      return !wrongChambers.empty();
    }

    bool hasMissingLayers() const {
      return !missingLayers.empty();
    }

    bool hasWrongLayers() const {
      return !wrongLayers.empty();
    }

    bool hasMissingLayersTrigger() const {
      return !missingLayersTrigger.empty();
    }

    bool hasWrongLayersTrigger() const {
      return  !wrongLayersTrigger.empty();
    }
    
    bool isMissingInner() const {
      if( missingLayers.count(Muon::MuonStationIndex::EI) || missingLayers.count(Muon::MuonStationIndex::BI) ) return true;
      return false;
    }

    bool hasWrongInner() const {
      if( wrongLayers.count(Muon::MuonStationIndex::EI) || wrongLayers.count(Muon::MuonStationIndex::BI) ) return true;
      return false;
    }
    
    bool allOk() const {
      if( !missingChambers.empty() || !wrongChambers.empty() ) return false;
      if( !missingLayers.empty()   || !wrongLayers.empty()   ) return false;
      if( !missingLayersTrigger.empty()   || !wrongLayersTrigger.empty()   ) return false;
      return true;
    }

  private:
    // no copying
    TrackData& operator=(const TrackData&) { return *this; }
  };


  struct EventData{
    unsigned int eventNumber;
    unsigned int eventPosition;
    std::vector<TrackData*> missingTruthTracks;
    std::vector<TrackData*> missingTruthTracksOneStation;

    std::vector<TrackData*> missingStationMomLoss;

    std::vector<TrackData*> missingStationLayer;
    std::vector<TrackData*> missingCscStationLayer;
    std::vector<TrackData*> missingStation;

    std::vector<TrackData*> wrongStationLayer;
    std::vector<TrackData*> wrongStation;

    std::vector<TrackData*> missingStationLayerTrigger;
    std::vector<TrackData*> wrongStationLayerTrigger;

    std::vector<TrackData*> fakeTracks;
    std::vector<TrackData*> fakeTracksLowPt;
    std::vector<TrackData*> fakeTracksSL;

    bool goodEvent() const {
      return missingTruthTracks.empty() && missingTruthTracksOneStation.empty() && missingStationMomLoss.empty() && 
	missingStationLayer.empty() && missingStation.empty() &&
	wrongStationLayer.empty() && wrongStation.empty() && fakeTracks.empty() && fakeTracksLowPt.empty() && fakeTracksSL.empty();
    }
  };


  MuonTrackPerformanceAlg(const std::string& name, ISvcLocator* pSvcLocator);

public:
  virtual ~MuonTrackPerformanceAlg();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:
  void extractEtaPhiCounts( const std::set<Identifier>& ids, int& neta, int& nphi, int& netaCh, int& nphiCh ) const;

  bool handleTracks();
  bool handleTrackTruth( const TrackCollection& trackCollection );

  bool handleSegmentCombi( const Muon::MuonSegmentCombination& combi );
  bool handleSegmentTruth( const std::vector<const Muon::MuonSegment*>& segments );


  void doSummary( const TrackCollection& tracks ) const;
  void doSummary( const Muon::IMuonTrackTruthTool::TruthTree& truthTracks ) const;

  std::string print( const Muon::IMuonTrackTruthTool::TruthTreeEntry& trackTruth ) const;

  std::string  eventInfo() const;
  int eventNumber() const;

  void printMissingChambers() const;
  void printMissingTracks() const;
  void printFakeTracks() const;

  /** counts number of eta (first number) and phi (second number in pair) hits in the set of hit IDs in the given chamber */
  std::pair<int,int> countHitsInChamber( const Identifier& chId, const std::set<Identifier>& hitIds ) const;

  /**  insert chamber information into ChamberData if hits counts pass cuts, returns true if anything was inserted */
  bool insertChamber( const Identifier& chId, const std::set<Identifier>& hits, 
		      int minEtaHits, int minPhiHits, ChamberData& chamberData ) const;
  
  /** insert set of chambers into chamber data if hits counts pass cuts, returns true if anything was inserted */
  bool insertTechnology( const std::set<Identifier>& chIds, const std::set<Identifier>& hits, 
			 int minEtaHits, int minPhiHits, std::vector<ChamberData>& chamberData ) const;

  /** insert station layers into layer set if they are not in exclusion list */
  bool insertStationLayers( const std::set<Identifier>& chIds, const std::set<Muon::MuonStationIndex::StIndex>& exclusionList, 
			    std::set<Muon::MuonStationIndex::StIndex>& layers ) const;

  /** insert chamber data into layers if not in exclusion list */
  bool insertStationLayers( const std::vector<ChamberData>& chambers, const std::set<Muon::MuonStationIndex::StIndex>& exclusionList, 
			    std::set<Muon::MuonStationIndex::StIndex>& layers, bool usePrecision ) const;

  /** evaluate track/truth overlap and create the corresponding overlap description object */
  TrackData* evaluateTrackTruthOverlap( const Muon::MuonTrackTruth& truthTrack ) const;

  /** create track data object for truth track */
  TrackData* createTrackData( const Muon::IMuonTrackTruthTool::TruthTreeEntry& trackTruth ) const;

  void addTrackToTrackData( const Trk::Track& track, TrackData& trackData ) const;

  /** print TrackData to a string */
  std::string print( const TrackData& trackData ) const;

  std::string print( const EventData& event, const std::vector<TrackData*>& tracks, std::string message ) const;
  std::string print( const EventData& event, const std::vector<const Trk::Track*>& tracks, std::string message ) const;

  /** print all the track counters to a string to provide a summary */
  std::string printTrackCounters( bool doSecondaries = true ) const; 

  void clearTracks( std::vector<const Trk::Track*> tracks );
  void clearTracks( std::vector<TrackData*> tracks );
  void clearEvent( EventData& event );


  const HepMC::GenParticle* getMother( const TruthTrajectory& traj ) const;
  const HepMC::GenParticle* getInitialState( const TruthTrajectory& traj ) const;

  bool goodTruthTrack( const Muon::IMuonTrackTruthTool::TruthTreeEntry& entry ) const;
  bool isSecondary( const Muon::MuonTrackTruth& truthTrack ) const;
  bool isSecondary( const Muon::IMuonTrackTruthTool::TruthTreeEntry& entry ) const;
  bool isSecondary( const TruthTrajectory& truthTrajectory ) const;

  bool selectPdg( int pdg ) const { return m_selectedPdgs.count(pdg); }

  SG::ReadHandleKey<TrackCollection> m_trackKey{this,"TrackInputLocation","MuonSpectrometerTracks","input tracks"};            //!< Location of the track input container
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this,"EventInfoKey","EventInfo","EventInfo key"};
  const xAOD::EventInfo*    m_eventInfo;                //!< pointer to the event info

  //truth readHandles
  SG::ReadHandleKey<McEventCollection> m_mcEventColl{this,"McEventCollectionKey","TruthEvent","McEventCollection"};
  SG::ReadHandleKeyArray<MuonSimDataCollection> m_muonSimData{this,"MuonSimDataNames",{ "MDT_SDO", "RPC_SDO", "TGC_SDO", "sTGC_SDO", "MM_SDO" },"Muon SDO maps"};
  SG::ReadHandleKey<CscSimDataCollection> m_cscSimData{this,"CSC_SDO_Container","CSC_SDO","CSC SDO"};
  SG::ReadHandleKey<TrackRecordCollection> m_trackRecord{this,"TrackRecord","MuonEntryLayerFilter","Track Record Collection"};

  // member set by Joboptions 
  int  m_doSummary;
  int  m_doHitResiduals;
  int  m_doTrackDebug;
  bool m_doTruth;
  bool m_doHistos;
  double m_momentumCutSim;
  double m_momentumCut;
  bool m_usePtCut;
  bool m_isCombined;
  bool m_doSegments;
  bool m_writeToFile;
  bool m_doNSW;

  ToolHandle<Muon::MuonIdHelperTool> m_idHelperTool;
  ToolHandle<Muon::MuonEDMPrinterTool> m_printer;
  ToolHandle<Muon::MuonEDMHelperTool> m_helper;
  ToolHandle<Muon::IMuonTrackTruthTool> m_truthTool;
  ToolHandle<Trk::ITrackSummaryHelperTool>    m_summaryHelperTool;
  MsgStream* m_log;
  bool m_debug;
  bool m_verbose;
  int m_doEventListMissed;
  int m_doEventListIncomplete;
  int m_doEventListFake;

  int m_minMdtHits;
  int m_minCscEtaHits;
  int m_minCscPhiHits;
  int m_minRpcEtaHits;
  int m_minRpcPhiHits;
  int m_minTgcEtaHits;
  int m_minTgcPhiHits;
  int m_minsTgcEtaHits;
  int m_minsTgcPhiHits;
  int m_minMMEtaHits;

  unsigned int m_nevents;
  unsigned int m_ntracks;

  unsigned int m_nmatchedTracks;
  unsigned int m_nmatchedTracksSecondary;
  unsigned int m_nmatchedFakeTracks;

  unsigned int m_nfakeTracks;
  unsigned int m_nfakeTracksHighPt;
  unsigned int m_nfakeTracksLowPt;
  unsigned int m_nfakeTracksSL;

  unsigned int m_nmissedTracks;
  unsigned int m_nmissedTracksOneStation;

  unsigned int m_nmissedTracksSecondary;
  unsigned int m_nmissedTracksOneStationSecondary;

  unsigned int m_nmissingStationMomLoss;
  unsigned int m_nmissingStationLayer;
  unsigned int m_nmissingCscStationLayer;
  unsigned int m_nmissingStationLayerTrigger;
  unsigned int m_nmissingStation;

  unsigned int m_nwrongStationLayer;
  unsigned int m_nwrongStationLayerTrigger;
  unsigned int m_nwrongStation;

  unsigned int m_ntruthTracks;
  unsigned int m_ntruthTracksSecondary;
  


  struct TrackSummary {
    std::string trackPars;
    std::string chambers;
    std::vector<std::string> missingChambers;
  };

  struct EventSummary {
    std::string eventNumber;
    std::vector<TrackSummary> trackSummary;
  };

  void printSummary( const std::vector<EventSummary>& summary, std::string typeName ) const;

  std::vector<EventData>    m_badEvents;

  /** name of external file to write statistics */
  std::string m_fileName;

  /** output file*/
  std::ofstream  m_fileOutput;  

  IntegerArrayProperty m_pdgsToBeConsidered;
  std::set<int> m_selectedPdgs; // set storing particle PDG's considered for matching

};
  


#endif //MUONTRACKPERFORMANCEALG_H
