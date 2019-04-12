// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include "SCT_MonitoringNumbers.h"

#include "Identifier/Identifier.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

#include "GaudiKernel/ToolHandle.h"

#include <map>
#include <string>
#include <vector>

// Forward declarations
class IInterface;
class TH1F;
class TH2F;
class TH1I;
class TH2I;
class TH1F_LW;
class TH2F_LW;
class TH2I_LW;
class TProfile;
class TProfile2D;
class TProfile_LW;
class StatusCode;
class SCT_ID;
class ISCT_ConfigurationConditionsTool;

class SCTHitsNoiseMonTool : public ManagedMonitorToolBase {
 public:
  SCTHitsNoiseMonTool(const std::string& type, const std::string& name,const IInterface* parent); 
  ~SCTHitsNoiseMonTool() = default;
  virtual StatusCode initialize() final;
  /**    @name Book, fill & check (reimplemented from baseclass) */
  //@{
  ///Book is called at the beginning
  virtual StatusCode bookHistograms() override;
  virtual StatusCode bookHistogramsRecurrent() override;
  ///fill is called on each event loop
  virtual StatusCode fillHistograms() override;
  ///checkHists not currently used
  virtual StatusCode checkHists(bool fromFinalize) override;
  ///procHistograms is called at the end
  virtual StatusCode procHistograms() override;
  //@} 
  
 private:
  typedef TProfile2D* Prof2_t;
  typedef TProfile_LW* Prof_t;
  typedef TH1I* H1I_t;
  typedef TH2I_LW* H2I_t;
  typedef TH1F_LW* H1_t;
  typedef TH2F_LW* H2_t;
  typedef std::vector<Prof2_t> VecProf2_t;
  typedef std::vector<H1I_t> VecH1I_t;
  typedef std::vector<H2I_t> VecH2I_t;
  typedef std::vector<H1_t> VecH1_t;
  typedef std::vector<H2_t> VecH2_t;

  enum Thresholds {thresh100, thresh1000, thresh10000, nThreshes};
  static const double s_thresholds[nThreshes];
  static const std::string s_thresholdNames[nThreshes];

  static constexpr int s_commonSize{std::max(SCT_Monitoring::N_BARRELSx2, SCT_Monitoring::N_DISKSx2)};

  /// stream stem for the ROOT histograms
  std::string m_stream{"/stat"};

  int m_current_lb{0};
  int m_last_reset_lb{0};
  //Count number of events
  int m_numberOfEvents{0};
  //Count number of events in the selected stream
  int m_numberOfEventsTrigger{0};
  //Count number of events since last reset
  int m_numberOfEventsRecent{0};
  //CAM adds skip events counter
  int m_skipEvents{0};

  int m_noisyM[nThreshes][SCT_Monitoring::NBINS_LBs+1]{};
  int m_occ_lb[SCT_Monitoring::N_REGIONS+1][SCT_Monitoring::NBINS_LBs+1]{};
  int m_events_lb{0};
  int m_noisyMTrigger[nThreshes][SCT_Monitoring::NBINS_LBs+1]{};
  int m_occTrigger_lb[SCT_Monitoring::N_REGIONS+1][SCT_Monitoring::NBINS_LBs+1]{};
  int m_eventsTrigger_lb{0};
  
  int m_noisyMWithHO[nThreshes][SCT_Monitoring::NBINS_LBs+1]{};
  int m_hitocc_lb[SCT_Monitoring::N_REGIONS+1][SCT_Monitoring::NBINS_LBs+1]{};
  int m_noisyMWithHOTrigger[nThreshes][SCT_Monitoring::NBINS_LBs+1]{};
  int m_hitoccTrigger_lb[SCT_Monitoring::N_REGIONS+1][SCT_Monitoring::NBINS_LBs+1]{};

  std::vector<int> m_nSP_buf{};
  int m_nSP_pos{0};
  std::vector<int> m_nHits_buf{};
  int m_nHits_pos{0};
  std::vector<int> m_nmaxHits_buf{};
  std::vector<Identifier> m_nmaxModule_buf{};
  std::vector<int> m_nminHits_buf{};
  std::vector<Identifier> m_nminModule_buf{};

  std::vector<Identifier> m_RDOsOnTracks;

  ///additional maps for track NO to compare with SP NO calc
  std::map<Identifier, int> m_RDOs;

  std::map<Identifier, float> m_occSumUnbiased;
  std::map<Identifier, float> m_occSumUnbiasedTrigger;
  std::map<Identifier, float> m_occSumUnbiasedRecent;

  std::map<Identifier, float> m_occSumUnbiased_lb[SCT_Monitoring::N_REGIONS+1];
  std::map<Identifier, float> m_occSumUnbiasedTrigger_lb[SCT_Monitoring::N_REGIONS+1];
   
  std::map<Identifier, float> m_hitoccSumUnbiased{};
  std::map<Identifier, float> m_hitoccSumUnbiasedTrigger{};
  std::map<Identifier, float> m_hitoccSumUnbiasedRecent{};

  std::map<Identifier, float> m_hitoccSumUnbiased_lb[SCT_Monitoring::N_REGIONS+1]{};
  std::map<Identifier, float> m_hitoccSumUnbiasedTrigger_lb[SCT_Monitoring::N_REGIONS+1]{};

  /// Name of the L1 Type to use for filling the extra NO histograms
  StringProperty m_NOTriggerItem{this, "NOTrigger", "L1_RD0_EMPTY"};
  BooleanProperty m_doTrigger{this, "doTrigger", false};
  /// Switch on or off the hitmaps histograms
  BooleanProperty m_boolhitmaps{this, "doHitmapHistos", false};
  /// Set bin width to increase exponentially (fixed width on a Log-X plot) 
  BooleanProperty m_doLogXNoise{this, "doLogXNoise", true};
  /// Switch on or off the endcaps
  BooleanProperty m_doPositiveEndcap{this, "doPositiveEndcap", true};
  BooleanProperty m_doNegativeEndcap{this, "doNegativeEndcap", true};
  ///Select the noise algorithm
  BooleanProperty m_doTrackBasedNoise{this, "doTrackBasedNoise", false};
  BooleanProperty m_doSpacePointBasedNoise{this, "doSpacePointBasedNoise", true};
  /// Add time-bin filtering to space point NO algorithm
  BooleanProperty m_doTimeBinFilteringForNoise{this, "doTimeBinFilteringForNoise", true};
  /// Switch on or off the hits histos as a function of TriggerType and L1ID for TX scan
  BooleanProperty m_booltxscan{this, "doTXScan", false};
  /// CheckHists() frequency
  IntegerProperty m_checkrate{this, "CheckRate", 1000};
  IntegerProperty m_checkrecent{this, "CheckRecent", 30};
  /// ChechHists() frequency
  IntegerProperty m_evtsbins{this, "EvtsBins", 5000};
  //Things for doing time bin distributions for track hits
  BooleanProperty m_doTrackHits{this, "doTrackHits", true};
  UnsignedIntegerProperty m_maxTracks{this, "MaxTracks", 1000};
  ///Determines whether a local summary is written
  IntegerProperty m_localSummary{this, "localSummary", 0};

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

  /// ---------------------------------------
  //@name Histograms related members
  //@{
  /// Vector of pointers to hitmaps histograms
  VecH1_t m_phitmapHistoVector;
  
  /// Vector of pointers to histogram of SCT modules hits; 1 histo per layer and side
  VecH2_t m_phitsHistoVector[SCT_Monitoring::N_REGIONS];
  VecH2_t m_phitsHistoVectorRecent[SCT_Monitoring::N_REGIONS];
  VecProf2_t m_pnoiseoccupancymapHistoVector[SCT_Monitoring::N_REGIONS];
  VecProf2_t m_pnoiseoccupancymapHistoVectorRecent[SCT_Monitoring::N_REGIONS];
  VecProf2_t m_pnoiseoccupancymapHistoVectorTrigger[SCT_Monitoring::N_REGIONS];
  
  TH1F* m_nSP{nullptr};
  TH1F* m_nHits{nullptr};
  TH1F* m_nmaxHits{nullptr};
  TH1F* m_nminHits{nullptr};

  //Histograms with hits per luminosity block
  H1_t m_numHitsPerLumiBlock[SCT_Monitoring::N_REGIONS]{};

  //Histograms with SPs per luminsity block
  H1_t m_numSPPerLumiBlock[SCT_Monitoring::N_REGIONS]{};

  H2_t m_rioMap{nullptr};
  //@}
  //Histograms with NO distribution
  TH1F* m_NO[SCT_Monitoring::N_REGIONS+1]{};
  TH1F* m_NOTrigger[SCT_Monitoring::N_REGIONS+1]{};

  //---- results required no triggers
  // # of hits vs LBs
  Prof_t m_NallHits_vsLB[SCT_Monitoring::N_REGIONS]{};
  Prof_t m_NSPHits_vsLB[SCT_Monitoring::N_REGIONS]{};
  // NO with hits subtracted by SP
  Prof_t m_NO_vsLB[SCT_Monitoring::N_REGIONS+1]{};
  Prof_t m_NoisyModules_vsLB[nThreshes]{};
  
  //---- results required trigger
  // # of hits vs LBs
  Prof_t m_NallHitsTrigger_vsLB[SCT_Monitoring::N_REGIONS]{};
  Prof_t m_NSPHitsTrigger_vsLB[SCT_Monitoring::N_REGIONS]{};
  // NO with hits subtracted by SP
  Prof_t m_NOTrigger_vsLB[SCT_Monitoring::N_REGIONS+1]{};
  Prof_t m_NoisyModulesTrigger_vsLB[nThreshes]{};

  //Online code- template vector
  VecH1_t m_pstripmapTemplateVector{};
  H1_t m_MaxOccupancyStripHist{nullptr};
  H1_t m_MinOccupancyStripHist{nullptr};
  /// Pointers to histogram of SCT cluster width.
  H1_t m_clusize{nullptr};
  H1_t m_clusizeRecent{nullptr};

  /// Pointers to histogram of hits as a function of TriggerType.
  H1_t m_hitsvstrigger{nullptr};

  /// Pointers to histogram of hits as a function of L1ID.
  H1_t m_hitsvsL1ID{nullptr};

  /// Pointers to vector of histograms of SCT cluster width; 1 histo per layer and side.
  VecH1_t m_clusizeHistoVector[SCT_Monitoring::N_REGIONS]{};
  VecH1_t m_clusizeHistoVectorRecent[SCT_Monitoring::N_REGIONS]{};

  /// Pointer to 1D histogram of Number of SCT Clusters per Event
  TH1F* m_ncluHisto{nullptr};

  /// Vector of pointers to 1D histogram of Number of SCT Clusters per Event; 1 histo per layer and side
  VecH1_t m_ncluHistoVector[SCT_Monitoring::N_REGIONS]{};

  //Pointer to histogram of time bin coincidences
  H2I_t m_coincidenceHist{nullptr};

  VecH2_t m_ptrackhitsHistoVector[SCT_Monitoring::N_REGIONS];
  VecH2_t m_ptrackhitsHistoVectorRecent[SCT_Monitoring::N_REGIONS];
  VecH1_t m_tbinHistoVector[SCT_Monitoring::N_REGIONS]{};
  VecH1_t m_tbinHistoVectorRecent[SCT_Monitoring::N_REGIONS]{};
  H1_t m_tbinHisto[SCT_Monitoring::N_REGIONS]{};
  H1_t m_tbinHistoRecent[SCT_Monitoring::N_REGIONS]{};
  Prof2_t m_tbinfrac[SCT_Monitoring::N_REGIONS][s_commonSize]{};
  Prof2_t m_clusizedist[SCT_Monitoring::N_REGIONS][s_commonSize]{};
  Prof_t m_tbinfracall{nullptr};
  Prof_t m_tbinfracVsLB[SCT_Monitoring::N_REGIONS]{};

  VecProf2_t m_phitoccupancymapHistoVector[SCT_Monitoring::N_REGIONS]{};
  VecProf2_t m_phitoccupancymapHistoVectorRecent[SCT_Monitoring::N_REGIONS]{};
  VecProf2_t m_phitoccupancymapHistoVectorTrigger[SCT_Monitoring::N_REGIONS]{};

  //Histograms with HO distribution
  TH1F* m_HO[SCT_Monitoring::N_REGIONS+1]{};
  TH1F* m_HOTrigger[SCT_Monitoring::N_REGIONS+1]{};

  //---- results required no triggers
  // # of hits vs LBs
  Prof_t m_HallHits_vsLB[SCT_Monitoring::N_REGIONS]{};
  Prof_t m_HSPHits_vsLB[SCT_Monitoring::N_REGIONS]{};
  // HO with hits subtracted by SP
  Prof_t m_HO_vsLB[SCT_Monitoring::N_REGIONS+1]{};
  Prof_t m_NoisyModulesWithHO_vsLB[nThreshes]{};

  //---- results required trigger
  // # of hits vs LBs
  Prof_t m_HallHitsTrigger_vsLB[SCT_Monitoring::N_REGIONS]{};
  Prof_t m_HSPHitsTrigger_vsLB[SCT_Monitoring::N_REGIONS]{};
  // HO with hits
  Prof_t m_HOTrigger_vsLB[SCT_Monitoring::N_REGIONS+1]{};
  Prof_t m_NoisyModulesWithHOTrigger_vsLB[nThreshes]{};

  StatusCode makeVectorOfTrackRDOIdentifiers();
 
  //@name Histograms related methods
  //@{
  
  StatusCode bookGeneralTrackHits(const unsigned int systemIndex);
  StatusCode bookGeneralTrackTimeHistos(const unsigned int systemIndex);

  // Book Hitmaps Histograms
  StatusCode book1DHitHists();

  StatusCode bookHitMaps();
  StatusCode bookSPvsEventNumber();
  StatusCode makeSPvsEventNumber();
  // Book noise map histograms
  StatusCode bookNoiseMaps();
  StatusCode bookNoiseDistributions();

  // Book noise occupancy map histograms
  StatusCode bookGeneralNoiseOccupancyMaps(const unsigned int systemIndex);
  //Book hit occupancy map histograms
  StatusCode bookGeneralHitOccupancyMaps(const unsigned int systemIndex);

  // Book Hits Histograms
  StatusCode bookGeneralHits(const unsigned int systemIndex);

  StatusCode bookClusterSize();
  StatusCode bookGeneralCluSize(const unsigned int systemIndex);
  //@}

  //Do checking of noise maps
  StatusCode checkNoiseMaps(bool final);
  StatusCode initializeNoiseMaps();
  StatusCode resetNoiseMapHists();
  StatusCode resetNoiseMapsRecent();
  StatusCode resetHitMapHists();
  StatusCode generalHistsandNoise();

  StatusCode resetVecProf2(VecProf2_t hists) const;
  StatusCode resetVecH2(VecH2_t hists) const;
  StatusCode resetVecH1(VecH1_t hists) const;

  H1_t h1Factory(const std::string& name, const std::string& title, MonGroup& registry, VecH1_t& storageVector, const float lo, const float hi, const unsigned int nbins) const;
  H1_t h1Factory(const std::string& name, const std::string& title, MonGroup& registry, const float lo, const float hi, const unsigned int nbins) const;

  TH1F* th1Factory(const std::string& name, const std::string& title, MonGroup& registry, std::vector<TH1F*>& storageVector, const float lo, const float hi, const unsigned int nbins) const;
  TH1F* th1Factory(const std::string& name, const std::string& title, MonGroup& registry, const float lo, const float hi, const unsigned int nbins) const;

  H2_t h2Factory(const std::string& name, const std::string& title, const SCT_Monitoring::Bec bec, MonGroup& registry, VecH2_t& storageVector) const;

  H2I_t h2IFactory(const std::string& name, const std::string& title, MonGroup& registry, int nbinx, float xlo, float xhi, int nbiny, float ylo, float yhi) const;

  Prof2_t prof2Factory(const std::string& name, const std::string& title, const SCT_Monitoring::Bec bec, MonGroup& registry, VecProf2_t& storageVector) const;
  Prof2_t prof2DFactory(const std::string& name, const std::string& title, MonGroup& registry, int nbinx, float xlo, float xhi, int nbiny, float ylo, float yhi) const;

  Prof_t profFactory(const std::string& name, const std::string& title, MonGroup& registry, int nbin, float lo, float hi) const;
  Prof_t profFactory(const std::string& name, const std::string& title, MonGroup& registry) const;

  ///Format the position as a string
  std::string positionString(const Identifier& plane) const;
};

#endif // SCTHITSNOISEMONTOOL_H
