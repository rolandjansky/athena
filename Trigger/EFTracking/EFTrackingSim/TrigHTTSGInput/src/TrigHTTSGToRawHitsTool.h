/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigHTTSGInput_TrigHTTSGTORAWHITSTOOL_H
#define TrigHTTSGInput_TrigHTTSGTORAWHITSTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "BeamSpotConditionsData/BeamSpotData.h"


#include "HepPDT/ParticleDataTable.hh"
#include "HepPDT/ParticleData.hh"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "TrigHTTSGInput/ITrigHTTInputTool.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "TrigHTTSGInput/TrigHTTInputUtils.h"
#include "xAODEventInfo/EventInfo.h"
#include "GeneratorObjects/McEventCollection.h"

#include "InDetRawData/PixelRDO_Container.h"
#include "InDetRawData/SCT_RDO_Container.h"


class HTTEventInputHeader;
class HTTCluster;
class HTTTruthTrack;
class HTTOfflineTrack;

typedef unsigned int index_type;

/**
 * @brief Extract the raw hists from info in SG
 * 
 */
class TrigHTTSGToRawHitsTool : public extends<AthAlgTool, ITrigHTTInputTool> {

public:
  TrigHTTSGToRawHitsTool(const std::string&, const std::string&, const IInterface*);
  virtual ~TrigHTTSGToRawHitsTool() { ; }

  virtual StatusCode initialize() override;
  virtual StatusCode finalize()   override;
  virtual StatusCode readData(HTTEventInputHeader* header, const EventContext& eventContext)  override;


private:
  ToolHandle<Trk::ITruthToTrack> m_truthToTrack { this, "TruthToTrackTool", "Trk::TruthToTrack/InDetTruthToTrack" }; //!< tool to create track parameters from a gen particle
  ToolHandle<Trk::IExtrapolator> m_extrapolator { this, "Extrapolator", "Trk::Extrapolator/AtlasExtrapolator" };
  /////  ServiceHandle<IBeamCondSvc>    m_beamSpotSvc;
  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey { this, "EventInfo", "EventInfo" };
  SG::ReadHandleKey<InDet::SiClusterContainer> m_pixelClusterContainerKey { this, "pixelClustersName", "ITkPixelClusters" };
  SG::ReadHandleKey<InDet::SiClusterContainer> m_sctClusterContainerKey { this, "SCT_ClustersName", "SCT_Clusters" };

  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_offlineTracksKey  { this, "OfflineTracks", "InDetTrackParticles"};
  SG::ReadHandleKey<McEventCollection> m_mcCollectionKey  { this, "McTruth", "TruthEvent" };
  SG::ReadHandleKey<InDetSimDataCollection> m_pixelSDOKey  { this, "PixelSDO", "ITkPixelSDO_Map" };
  SG::ReadHandleKey<InDetSimDataCollection> m_stripSDOKey  { this, "StripSDO", "ITkStripSDO_Map" };
  SG::ReadHandleKey<PixelRDO_Container> m_pixelRDOKey  { this, "PixelRDO", "ITkPixelRDOs" };
  SG::ReadHandleKey<SCT_RDO_Container> m_stripRDOKey  { this, "StripRDO", "ITkStripRDOs" };

  
  


  Gaudi::Property<std::string>         m_tracksTruthName { this, "OfflineName", "InDetTrackParticles", "name of offline tracks collection" };

  Gaudi::Property<bool>        m_dumpHitsOnTracks { this, "dumpHitsOnTracks", false };
  Gaudi::Property<bool>        m_dumpTruthIntersections { this, "dumpTruthIntersections", false };
  Gaudi::Property<bool>        m_readOfflineClusters { this, "ReadOfflineClusters", true, "flag to enable the offline cluster save" };
  Gaudi::Property<bool>        m_readTruthTracks { this, "ReadTruthTracks", true, "flag to enable the truth tracking save" }; 
  Gaudi::Property<bool>        m_readOfflineTracks { this, "ReadOfflineTracks", true, "flag to enable the offline tracking save" };
  Gaudi::Property<bool>        m_UseNominalOrigin { this, "UseNominalOrigin", false, "if true truth values are always with respect to (0,0,0)" };
  Gaudi::Property<double>      m_maxEta { this, "maxEta", 3.3 };
  Gaudi::Property<double>      m_minPt { this, "minPt", .8*CLHEP::GeV };
  Gaudi::Property<int>    m_simBarcodeOffset{this, "SimBarcodeOffset",200000};

  //internal pointers
  AtlasDetectorID* m_idHelper = nullptr;
  const PixelID* m_pixelId = nullptr;
  const SCT_ID* m_sctId = nullptr;

  const InDetDD::SiDetectorManager* m_PIX_mgr = nullptr;
  const InDetDD::SiDetectorManager* m_SCT_mgr = nullptr;
  const HepPDT::ParticleDataTable* m_particleDataTable = nullptr;

  typedef std::map<Identifier, int> HitIndexMap;
  StatusCode readRawSilicon(HitIndexMap& hitIndexMap, HitIndexMap& pixelClusterIndexMap, const EventContext& eventContext); // dump raw silicon data to text file and populate hitIndexMap for rec. track processing
  StatusCode readTruthTracks(std::vector<HTTTruthTrack>& truth, const EventContext& eventContext);
  StatusCode readOfflineTracks(std::vector<HTTOfflineTrack>& Track, const EventContext& eventContext);
  StatusCode readOfflineClusters(std::vector<HTTCluster>& Clusters, const EventContext& eventContext);
  StatusCode readPixelSimulation(HitIndexMap& hitIndexMap, unsigned int& hitIndex, const EventContext& eventContext);
  StatusCode readStripSimulation(HitIndexMap& hitIndexMap, unsigned int& hitIndex, const EventContext& eventContext);
  StatusCode dumpPixelClusters(HitIndexMap& pixelClusterIndexMap, const EventContext& eventContext);

  // To get truth information from simulation
  void getTruthInformation(InDetSimDataCollection::const_iterator& iter, TrigHTTInputUtils::ParentBitmask& parentMask, HepMcParticleLink::ExtendedBarCode& bestExtcode, HepMC::ConstGenParticlePtr bestParent);
  HTTEventInputHeader*        m_eventHeader = nullptr;
};



#endif
