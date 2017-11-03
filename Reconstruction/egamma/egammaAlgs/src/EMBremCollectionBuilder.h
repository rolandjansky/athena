/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_EMBREMCOLLECTIONBUILDER_H
#define EGAMMAALGS_EMBREMCOLLECTIONBUILDER_H
/**
   @class EMBremCollectionBuilder
         Algorithm which creates new brem-refitted tracks
*/

#include "egammaInterfaces/IegammaTrkRefitterTool.h"
#include "egammaInterfaces/IEMExtrapolationTools.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "TrkToolInterfaces/ITrackSlimmingTool.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "TrkTrack/TrackCollection.h"

#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODCaloEvent/CaloClusterContainer.h"


class CaloCluster;

class EMBremCollectionBuilder : public AthAlgorithm 
{
public:
  /** @brief Default constructor*/
  EMBremCollectionBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();
private:
  //------------------------------------------------------------------------
  //      methods
  //------------------------------------------------------------------------
  //
  /** @brief Refit of track */
  StatusCode refitTrack(const xAOD::TrackParticle* tmpTrkPart);
  /** @brief broad track selection */
  bool Select(const xAOD::CaloCluster*        cluster,
              bool                      trkTRT,
              const xAOD::TrackParticle* track) const;
  //------------------------------------------------------------------------
  //      configurable data members
  //------------------------------------------------------------------------       
  /** @brief The track refitter */
  ToolHandle<IegammaTrkRefitterTool>  m_trkRefitTool {this,
      "TrackRefitTool", "ElectronRefitterTool", "Track refitter tool"};

  /** @brief Tool to create track particle */
  ToolHandle< Trk::ITrackParticleCreatorTool > m_particleCreatorTool {this,
      "TrackParticleCreatorTool", 
      "TrackParticleCreatorTool", 
      "TrackParticle creator tool"};

  /** @brief Tool to slim tracks  */
  ToolHandle<Trk::ITrackSlimmingTool>  m_slimTool {this,
      "TrackSlimmingTool", "TrkTrackSlimmingTool", "Track slimming tool"};

  /** @brief Tool for Track summary  */
  ToolHandle<Trk::ITrackSummaryTool>   m_summaryTool {this,
      "TrackSummaryTool", "InDetTrackSummaryTool", "Track summary tool"};

  /** @brief Tool for extrapolation */
  ToolHandle<IEMExtrapolationTools> m_extrapolationTool {this,
      "ExtrapolationTool", "EMExtrapolationTools", "Extrapolation tool"};

  /** @brier Option to do truth*/
  Gaudi::Property<bool> m_doTruth {this, "DoTruth", false, "do truth"};

  /** @brief Names of input output collections */
  SG::ReadHandleKey<xAOD::CaloClusterContainer>  m_clusterContainerKey {this,
      "ClusterContainerName", "LArClusterEM", "Input calo cluster for seeding"};

  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticleContainerKey {this,
      "TrackParticleContainerName", "InDetTrackParticles", 
      "Input TrackParticles to refit"};

  SG::WriteHandleKey<xAOD::TrackParticleContainer> m_OutputTrkPartContainerKey {this,
      "OutputTrkPartContainerName", "GSFTrackParticles", 
      "Output refitted TrackParticles"};

  SG::WriteHandleKey<TrackCollection> m_OutputTrackContainerKey {this,
      "OutputTrackContainerName", "GSFTracks", "Output refitted Trk::Tracks"};
  
  /** @Cut on minimum silicon hits*/
  Gaudi::Property<int> m_MinNoSiHits {this, "minNoSiHits", 4, 
      "Minimum number of silicon hits on track before it is allowed to be refitted"};

  /** @brief broad cut on deltaEta*/
  Gaudi::Property<double> m_broadDeltaEta {this, "broadDeltaEta", 0.1,
      "Value of broad cut for delta eta, it is mult by 2"};

  /** @brief broad cut on deltaPhi*/
  Gaudi::Property<double> m_broadDeltaPhi {this, "broadDeltaPhi", 0.15,
      "Value of broad cut for delta phi, it is mult by 2"};

  /** @narrow windows*/
  Gaudi::Property<double> m_narrowDeltaEta {this, "narrowDeltaEta", 0.05,
      "Value of narrow cut for delta eta"};

  Gaudi::Property<double> m_narrowDeltaPhi {this, "narrowDeltaPhi", 0.05,
      "Value of narrow cut for delta phi"};

  Gaudi::Property<double> m_narrowDeltaPhiBrem {this, "narrowDeltaPhiBrem", 0.15,
      "Value of the narrow cut for delta phi in the brem direction"};

  Gaudi::Property<double> m_narrowRescale {this, "narrowDeltaPhiRescale", 0.05,
      "Value of the narrow cut for delta phi Rescale"};

  Gaudi::Property<double> m_narrowRescaleBrem {this, "narrowDeltaPhiRescaleBrem", 0.1,
      "Value of the narrow cut for delta phi Rescale Brem"};

  //collections
  TrackCollection*                    m_finalTracks;
  xAOD::TrackParticleContainer*       m_finalTrkPartContainer;
  //counters
  unsigned int m_AllClusters;
  unsigned int m_AllTracks;
  unsigned int m_AllTRTTracks;
  unsigned int m_AllSiTracks;
  unsigned int m_SelectedTracks;
  unsigned int m_SelectedTRTTracks;
  unsigned int m_SelectedSiTracks;
  unsigned int m_FailedFitTracks;
  unsigned int m_FailedSiliconRequirFit;
  unsigned int m_RefittedTracks;
  //------------------------------------------------------------------------

};
#endif // ANA1_H
