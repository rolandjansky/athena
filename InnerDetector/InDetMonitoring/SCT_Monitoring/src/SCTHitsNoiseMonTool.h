// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**    @file SCTHitsNoiseMonTool.h
 *   Class declaration for SCTHitsMonTool
 *   
 *                          
 *                          
 *    @author Sofia Chouridou based on code from Luca Fiorini, Shaun Roe, Manuel Diaz Gomez
 *    and Maria Jose Casta.
 *  
 *   
 * 
 *   
 */

#ifndef SCTHITSNOISEMONTOOL_H
#define SCTHITSNOISEMONTOOL_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "SCT_Monitoring/SCT_MonitoringNumbers.h"

#include "Identifier/Identifier.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkTrack/TrackCollection.h"

#include "GaudiKernel/ToolHandle.h"

#include <array>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

// Forward declarations
class ISCT_ConfigurationConditionsTool;
class SCT_ID;

class TH1F_LW;
class TH2F_LW;
class TProfile_LW;

class TH1F;
class TProfile2D;

class SCTHitsNoiseMonTool : public ManagedMonitorToolBase {
 public:
  SCTHitsNoiseMonTool(const std::string& type, const std::string& name,const IInterface* parent); 
  ~SCTHitsNoiseMonTool() = default;
  virtual StatusCode initialize() override final;
  /**    @name Book, fill & check (reimplemented from baseclass) */
  //@{
  ///Book is called at the beginning
  virtual StatusCode bookHistograms() override final;
  virtual StatusCode bookHistogramsRecurrent() override final;
  ///fill is called on each event loop
  virtual StatusCode fillHistograms() override final;
  ///checkHists not currently used
  virtual StatusCode checkHists(bool fromFinalize) override final;
  ///procHistograms is called at the end
  virtual StatusCode procHistograms() override final;
  //@} 
  
 private:
  static constexpr int s_commonSize{std::max(SCT_Monitoring::N_BARRELSx2, SCT_Monitoring::N_DISKSx2)};

  /// Name of the L1 Type to use for filling the extra NO histograms
  StringProperty m_NOTriggerItem{this, "NOTrigger", "L1_RD0_EMPTY"};
  BooleanProperty m_doTrigger{this, "doTrigger", false};
  /// Switch on or off the hitmaps histograms
  BooleanProperty m_boolhitmaps{this, "doHitmapHistos", false};
  /// Switch on or off the endcaps
  BooleanProperty m_doPositiveEndcap{this, "doPositiveEndcap", true};
  BooleanProperty m_doNegativeEndcap{this, "doNegativeEndcap", true};
  ///Select the noise algorithm
  BooleanProperty m_doSpacePointBasedNoise{this, "doSpacePointBasedNoise", true};
  /// Add time-bin filtering to space point NO algorithm
  BooleanProperty m_doTimeBinFilteringForNoise{this, "doTimeBinFilteringForNoise", true};
  /// CheckHists() frequency
  IntegerProperty m_checkrate{this, "CheckRate", 1000};
  IntegerProperty m_checkrecent{this, "CheckRecent", 30};
  //Things for doing time bin distributions for track hits
  BooleanProperty m_doTrackHits{this, "doTrackHits", true};
  UnsignedIntegerProperty m_maxTracks{this, "MaxTracks", 1000};

  /// Name of the Track collection to use
  SG::ReadHandleKey<TrackCollection> m_tracksName{this, "tracksName", "CombinedInDetTracks"};
  SG::ReadHandleKey<InDet::SCT_ClusterContainer> m_clusContainerKey{this, "ClusterKey", "SCT_Clusters"};
  //CAM adds map for SP NO
  SG::ReadHandleKey<SpacePointContainer> m_SCTSPContainerName{this, "SpacePointKey", "SCT_SpacePoints"};
  /// Data object name: for the SCT this is "SCT_RDOs"
  SG::ReadHandleKey<SCT_RDO_Container> m_dataObjectName{this, "RDOKey", "SCT_RDOs"};
  // For P->T converter of SCT_Clusters
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  //@name Tool methods
  //@{
  ToolHandle<ISCT_ConfigurationConditionsTool> m_ConfigurationTool{this, "conditionsTool",
      "SCT_ConfigurationConditionsTool/InDetSCT_ConfigurationConditionsTool", "Tool to retrieve SCT Configuration Tool"};
  //@}

  //@nameService members
  //@{  
  ///SCT Helper class
  const SCT_ID* m_pSCTHelper{nullptr};
  //@}

  ///// Not used in checkNoiseMaps /////

  //Count number of events
  int m_numberOfEvents{0}; // Used in bookHistograms, bookHistogramsRecurrent, fillHistograms

  int m_last_reset_lb{0}; // Used in fillHistograms

  std::array<std::unordered_set<Identifier>, SCT_Monitoring::N_WAFERS> m_RDOsOnTracks; // Used in generalHistsandNoise, makeVectorOfTrackRDOIdentifiers

  ///// Read in checkNoiseMaps /////

  //Count number of events since last reset
  int m_numberOfEventsRecent{0}; // Used in fillHistograms, checkNoiseMaps

  std::map<Identifier, float> m_occSumUnbiasedRecent; // Used in generalHistsandNoise, checkNoiseMaps, initializeNoiseMaps, resetNoiseMapsRecent

  std::map<Identifier, float> m_hitoccSumUnbiasedRecent{}; // Used in checkNoiseMaps, initializeNoiseMaps, resetNoiseMapsRecent, generalHistsandNoise

  /// ---------------------------------------
  //@name Histograms related members
  //@{

  ///// Not used in checkNoiseMaps and not reset at all /////

  /// Vector of pointers to hitmaps histograms
  std::vector<TH1F_LW*> m_phitmapHistoVector; // Used in generalHistsandNoise, book1DHitHists
  
  /// Vector of pointers to histogram of SCT modules hits; 1 histo per layer and side
  std::vector<TH2F_LW*> m_phitsHistoVector[SCT_Monitoring::N_REGIONS]; // Used in bookGeneralHits, generalHistsandNoise

  /// Pointers to histogram of SCT cluster width.
  TH1F_LW* m_clusize{nullptr}; // Used in generalHistsandNoise, bookClusterSize

  /// Pointer to 1D histogram of Number of SCT Clusters per Event
  TH1F* m_ncluHisto{nullptr}; // Used in bookHistograms, bookHistogramsRecurrent, generalHistsandNoise

  //---- results required no triggers
  // # of hits vs LBs
  TProfile_LW* m_HallHits_vsLB[SCT_Monitoring::N_REGIONS]{}; // Used in generalHistsandNoise, bookGeneralHitOccupancyMaps
  TProfile_LW* m_HSPHits_vsLB[SCT_Monitoring::N_REGIONS]{}; // Used in generalHistsandNoise, bookGeneralHitOccupancyMaps

  //---- results required trigger
  // # of hits vs LBs
  TProfile_LW* m_HallHitsTrigger_vsLB[SCT_Monitoring::N_REGIONS]{}; // Used in generalHistsandNoise, bookGeneralHitOccupancyMaps
  TProfile_LW* m_HSPHitsTrigger_vsLB[SCT_Monitoring::N_REGIONS]{}; // Used in generalHistsandNoise, bookGeneralHitOccupancyMaps

  /// Vector of pointers to 1D histogram of Number of SCT Clusters per Event; 1 histo per layer and side
  std::vector<TH1F_LW*> m_ncluHistoVector[SCT_Monitoring::N_REGIONS]{}; // Used in bookGeneralHits, generalHistsandNoise

  TProfile_LW* m_tbinfracall{nullptr}; // Used in bookGeneralTrackTimeHistos, generalHistsandNoise

  //---- results required no triggers
  // NO with hits subtracted by SP
  TProfile_LW* m_NO_vsLB[SCT_Monitoring::N_REGIONS_INC_GENERAL]{}; // Used in bookGeneralNoiseOccupancyMaps, bookNoiseDistributions, generalHistsandNoise
  // HO with hits
  TProfile_LW* m_HO_vsLB[SCT_Monitoring::N_REGIONS_INC_GENERAL]{}; // Used in bookNoiseDistributions, bookGeneralHitOccupancyMaps, generalHistsandNoise
  
  //---- results required trigger
  // NO with hits subtracted by SP
  TProfile_LW* m_NOTrigger_vsLB[SCT_Monitoring::N_REGIONS_INC_GENERAL]{}; // Used in bookGeneralNoiseOccupancyMaps, bookNoiseDistributions, generalHistsandNoise
  // HO with hits
  TProfile_LW* m_HOTrigger_vsLB[SCT_Monitoring::N_REGIONS_INC_GENERAL]{}; // Used in bookGeneralHitOccupancyMaps, bookNoiseDistributions, generalHistsandNoise

  ///// Not used in checkNoiseMaps and are reset at some point /////

  /// Vector of pointers to histogram of SCT modules hits; 1 histo per layer and side
  std::vector<TH2F_LW*> m_phitsHistoVectorRecent[SCT_Monitoring::N_REGIONS]; // Used in bookGeneralHits, generalHistsandNoise, resetHitMapHists

  /// Pointers to histogram of SCT cluster width.
  TH1F_LW* m_clusizeRecent{nullptr}; // Used in generalHistsandNoise, bookClusterSize, resetHitMapHists

  std::vector<TH2F_LW*> m_ptrackhitsHistoVector[SCT_Monitoring::N_REGIONS]; // Used in bookGeneralTrackHitso, generalHistsandNoise, resetHitMapHists
  std::vector<TH2F_LW*> m_ptrackhitsHistoVectorRecent[SCT_Monitoring::N_REGIONS]; // Used in bookGeneralTrackHitso, generalHistsandNoise, resetHitMapHists

  /// Vector of pointers to histogram of SCT modules hits; 1 histo per layer and side
  std::vector<TProfile2D*> m_phitoccupancymapHistoVector[SCT_Monitoring::N_REGIONS]{}; // Used in bookGeneralHitOccupancyMaps, generalHistsandNoise, resetNoiseMapHists
  std::vector<TProfile2D*> m_pnoiseoccupancymapHistoVectorTrigger[SCT_Monitoring::N_REGIONS]; // Used in bookGeneralNoiseOccupancyMaps, generalHistsandNoise, resetNoiseMapHists

  ///// Updated in checkNoiseMaps /////

  /// Vector of pointers to histogram of SCT modules hits; 1 histo per layer and side
  std::vector<TProfile2D*> m_pnoiseoccupancymapHistoVectorRecent[SCT_Monitoring::N_REGIONS]; // Used in bookGeneralNoiseOccupancyMaps, checkNoiseMaps, resetNoiseMapHists

  std::vector<TProfile2D*> m_phitoccupancymapHistoVectorRecent[SCT_Monitoring::N_REGIONS]{}; // Used in bookGeneralHitOccupancyMaps, checkNoiseMaps, resetNoiseMapHists

  //@}

  //@name Histograms related methods
  //@{

  // Used in bookHistograms and bookHistogramsRecurrent
  StatusCode bookGeneralTrackHits(const unsigned int systemIndex);
  StatusCode bookGeneralTrackTimeHistos();
  StatusCode book1DHitHists(); // Book Hitmaps Histograms
  StatusCode bookNoiseDistributions(); // Book noise map histograms
  StatusCode bookGeneralNoiseOccupancyMaps(const unsigned int systemIndex); // Book noise occupancy map histograms
  StatusCode bookGeneralHitOccupancyMaps(const unsigned int systemIndex); // Book hit occupancy map histograms
  StatusCode bookGeneralHits(const unsigned int systemIndex); // Book Hits Histograms
  StatusCode bookClusterSize();
  StatusCode initializeNoiseMaps();

  // Used in fillHistograms
  StatusCode makeVectorOfTrackRDOIdentifiers();
  StatusCode resetNoiseMapsRecent();
  StatusCode resetHitMapHists();
  StatusCode generalHistsandNoise(const EventContext& ctx);

  // Used in fillHistograms and procHistograms
  StatusCode checkNoiseMaps(); // Do checking of noise maps

  // Used in book1DHitHists, bookGeneralHits, bookGeneralCluSize, bookGeneralTrackTimeHistos
  TH1F_LW* h1Factory(const std::string& name, const std::string& title, MonGroup& registry, std::vector<TH1F_LW*>& storageVector, const float lo, const float hi, const unsigned int nbins) const;
  // Used in bookGeneralHits, bookClusterSize, bookGeneralTrackTimeHistos
  TH1F_LW* h1Factory(const std::string& name, const std::string& title, MonGroup& registry, const float lo, const float hi, const unsigned int nbins) const;

  // Used in bookGeneralHits, bookGeneralTrackHits
  TH2F_LW* h2Factory(const std::string& name, const std::string& title, const SCT_Monitoring::Bec bec, MonGroup& registry, std::vector<TH2F_LW*>& storageVector) const;

  // Used in bookGeneralNoiseOccupancyMaps, bookGeneralHitOccupancyMaps
  TProfile2D* prof2Factory(const std::string& name, const std::string& title, const SCT_Monitoring::Bec bec, MonGroup& registry, std::vector<TProfile2D*>& storageVector) const;
  // Used in bookClusterSize, bookGeneralTrackTimeHistos
  TProfile2D* prof2DFactory(const std::string& name, const std::string& title, MonGroup& registry, int nbinx, float xlo, float xhi, int nbiny, float ylo, float yhi) const;

  // Used in bookGeneralTrackTimeHistos
  TProfile_LW* profFactory(const std::string& name, const std::string& title, MonGroup& registry) const;

  // Used in book1DHitHists
  std::string positionString(const Identifier& plane) const; /// Format the position as a string

  // Used in resetNoiseMapHists
  StatusCode resetVecProf2(std::vector<TProfile2D*>& hists) const;

  // Used in resetHitMapHists
  StatusCode resetVecH2(std::vector<TH2F_LW*>& hists) const;

  // Used in checkNoiseMaps
  StatusCode resetNoiseMapHists();

  //@}
};

#endif // SCTHITSNOISEMONTOOL_H
