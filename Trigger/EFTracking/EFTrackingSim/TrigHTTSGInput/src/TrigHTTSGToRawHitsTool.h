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
#include "TrigHTTSGInput/ITrigHTTInputTool.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "TrigHTTSGInput/TrigHTTInputUtils.h"
#include "xAODEventInfo/EventInfo.h"


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
  virtual StatusCode readData(HTTEventInputHeader* header)  override;


private:
  ToolHandle<Trk::ITruthToTrack> m_truthToTrack { this, "TruthToTrackTool", "Trk::TruthToTrack/InDetTruthToTrack" }; //!< tool to create track parameters from a gen particle
  ToolHandle<Trk::IExtrapolator> m_extrapolator { this, "Extrapolator", "Trk::Extrapolator/AtlasExtrapolator" };
  /////  ServiceHandle<IBeamCondSvc>    m_beamSpotSvc;
  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey { this, "EventInfo", "EventInfo" };
  SG::ReadHandleKey<InDet::SiClusterContainer> m_pixelClusterContainerKey { this, "pixelClustersName", "PixelClusters" };
  SG::ReadHandleKey<InDet::SiClusterContainer> m_sctClusterContainerKey { this, "SCT_ClustersName", "SCT_Clusters" };

  Gaudi::Property<std::string>         m_tracksTruthName { this, "OfflineName", "InDetTrackParticles", "name of offline tracks collection" };
  Gaudi::Property<std::string>         m_offlineName { this, "OfflineName", "InDetTrackParticles", "name of offline tracks collection" };

  Gaudi::Property<bool>        m_dumpHitsOnTracks { this, "dumpHitsOnTracks", false };
  Gaudi::Property<bool>        m_dumpTruthIntersections { this, "dumpTruthIntersections", false };
  Gaudi::Property<bool>        m_readOfflineClusters { this, "ReadOfflineClusters", true, "flag to enable the offline cluster save" };
  Gaudi::Property<bool>        m_readTruthTracks { this, "ReadTruthTracks", true, "flag to enable the truth tracking save" }; 
  Gaudi::Property<bool>        m_readOfflineTracks { this, "ReadOfflineTracks", true, "flag to enable the offline tracking save" };
  Gaudi::Property<bool>        m_UseNominalOrigin { this, "UseNominalOrigin", false, "if true truth values are always with respect to (0,0,0)" };
  Gaudi::Property<double>      m_maxEta { this, "maxEta", 3.3 };
  Gaudi::Property<double>      m_minPt { this, "minPt", .8*CLHEP::GeV };


  //internal pointers
  AtlasDetectorID* m_idHelper;
  const PixelID* m_pixelId;
  const SCT_ID* m_sctId;

  const InDetDD::SiDetectorManager* m_PIX_mgr;
  const InDetDD::SiDetectorManager* m_SCT_mgr;
  const HepPDT::ParticleDataTable* m_particleDataTable;

  typedef std::map<Identifier, int> HitIndexMap;
  StatusCode read_raw_silicon(HitIndexMap& hitIndexMap, HitIndexMap& pixelClusterIndexMap); // dump raw silicon data to text file and populate hitIndexMap for rec. track processing
  StatusCode read_truth_tracks(std::vector<HTTTruthTrack>& truth);
  StatusCode read_offline_tracks(std::vector<HTTOfflineTrack>& Track);
  StatusCode read_offline_clusters(std::vector<HTTCluster>& Clusters);
  StatusCode ReadPixelSimulation(HitIndexMap& hitIndexMap, unsigned int& hitIndex);
  StatusCode ReadStripSimulation(HitIndexMap& hitIndexMap, unsigned int& hitIndex);
  StatusCode DumpPixelClusters(HitIndexMap& pixelClusterIndexMap);

  // To get truth information from simulation
  void GetTruthInformation(InDetSimDataCollection::const_iterator& iter, TrigHTTInputUtils::ParentBitmask& parentMask, HepMcParticleLink::ExtendedBarCode& bestExtcode, const HepMC::GenParticle* bestParent);
  HTTEventInputHeader*        m_eventHeader = nullptr;
};



#endif
