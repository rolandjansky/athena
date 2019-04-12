// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** Tool to measure the intrinsic single hit efficiency in the SCT
    This tool is part of the SCT_Monitoring package. Its aim is to
    measure the intrinsic single hit efficiency in the SCT using
    the holes-on-tracks method.
    @author SCT Monitoring group hn-atlas-sct-monitoring@cern.ch
*/

#ifndef SCTEFFICIENCYTOOL_H
#define SCTEFFICIENCYTOOL_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "SCT_MonitoringNumbers.h"

#include "CommissionEvent/ComTime.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkTrack/Track.h" 
#include "TrkTrack/TrackCollection.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

//ROOT
#include "TString.h"

//STL
#include <string>
#include <array>

class IChronoStatSvc;
class Identifier;
class ISCT_ConfigurationConditionsTool;
class PixelID;
class SCT_ID;
class TRT_ID;

class TH1D;
class TH1F;
class TH2I;
class TH1I;
class TProfile2D;
class TProfile;
class TGraphErrors;

/**
   @class SCTHitEffMonTool
   The tool itself
*/
class SCTHitEffMonTool : public ManagedMonitorToolBase  {
  
 public:
  /** Constructor */
  SCTHitEffMonTool (const std::string& type, const std::string& name, const IInterface* parent);
  /** Destructor */
  virtual ~SCTHitEffMonTool() = default;

  virtual StatusCode initialize() override final;

  /** Histogram booking method */
  virtual StatusCode bookHistograms() override final;
  virtual StatusCode bookHistogramsRecurrent() override final;

  /** Histogram filling method */
  virtual StatusCode fillHistograms() override final;
  virtual StatusCode procHistograms() override final;

 private:
  typedef std::array<TProfile*, SCT_Monitoring::N_REGIONS> TProfArray;
  typedef std::array<TProfile2D*, SCT_Monitoring::N_REGIONS> TProf2DArray;
  typedef std::array<TH1F*, SCT_Monitoring::N_REGIONS> TH1FArray;
  typedef std::array<TH2F*, SCT_Monitoring::N_REGIONS> TH2FArray;
  typedef std::array<std::array<TH2F*, SCT_Monitoring::N_ENDCAPS>, SCT_Monitoring::N_REGIONS> TH2FArrayLayer;

  /** Method to cut on track or hit variables and automatize DEBUG statements */
  StatusCode failCut(bool value, std::string name) const;

  /** Method to compute incident angle of track to wafer */
  StatusCode findAnglesToWaferSurface(const Amg::Vector3D& mom, const Identifier id,
                                       const InDetDD::SiDetectorElementCollection* elements,
                                       double& theta, double& phi) const;

  /** Method to find the chip just before a given hit */
  int previousChip(double xl, int side, bool swap) const;

  /** Computes residual of a hit to a track */
  double getResidual(const Identifier& surfaceID,
                      const Trk::TrackParameters* trkParam,
                      const InDet::SCT_ClusterContainer* p_sctclcontainer) const;

  /** Single histogram booking method */
  template < class T > StatusCode bookEffHisto(T*& histo, MonGroup& MG, 
                                                TString name, TString title,
                                                int nbin, double x1, double x2) const;

  template < class T > StatusCode bookEffHisto(T*& histo, MonGroup& MG, 
                                                TString name, TString title,
                                                int nbinx, double x1, double x2,
                                                int nbiny, double y1, double y2) const;

  template < class T > StatusCode bookEffHisto(T*& histo, MonGroup& MG, 
                                                TString name, TString title,
                                                int nbinx, double* xbins,
                                                int nbiny, double* ybins) const;


  /**Convert a layer/disk number (0-21) to a bec index (0,1,2) according to position of that layer
   * Numbering is counter-intuitive, would expect C then B then A; in fact the original ordering was A, C, B
   * I have re-ordered this!!!! so now its C,B,A
   **/
  SCT_Monitoring::BecIndex layerIndex2becIndex(const int index) const;
  ///Convert a layer/disk number (0-21) to a layer number (0-8 for endcaps, 0-3 for barrel)
  int layerIndex2layer(const int index) const;
  int becIdxLayer2Index(const int becIdx, const int layer) const;

  std::string m_path;
  const std::map<Identifier, unsigned int>* m_badChips{nullptr};

  SG::ReadHandleKey<InDet::SCT_ClusterContainer> m_sctContainerName{this, "SCT_ClusterContainer", "SCT_Clusters"};
  SG::ReadHandleKey<TrackCollection> m_TrackName{this, "TrackName", "CombinedInDetTracks"};
  SG::ReadHandleKey<ComTime> m_comTimeName{this, "ComTimeKey", "TRT_Phase"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  ServiceHandle<IChronoStatSvc> m_chrono{this, "ChronoStatSvc", "ChronoStatSvc"};
  ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle{this, "MagFieldSvc", "AtlasFieldSvc"};
  ToolHandle<Trig::IBunchCrossingTool> m_bunchCrossingTool{this, "BunchCrossingTool", "Trig::BunchCrossingTool/BunchCrossingTool"};

  ToolHandle<Trk::IResidualPullCalculator> m_residualPullCalculator{this, "ResPullCalc", "Trk::ResidualPullCalculator/ResidualPullCalculator"};
  ToolHandle<Trk::IRIO_OnTrackCreator> m_rotcreator{this, "ROTCreator", "InDet::SCT_ClusterOnTrackTool/SCT_ClusterOnTrackTool"};
  ToolHandle<Trk::ITrackHoleSearchTool> m_holeSearchTool{this, "HoleSearch", "InDet::InDetTrackHoleSearchTool"};
  ToolHandle<ISCT_ConfigurationConditionsTool> m_configConditions{this, "ConfigConditions",
      "SCT_ConfigurationConditionsTool/InDetSCT_ConfigurationConditionsTool", "Tool to retrieve SCT Configuration Tool"};

  const PixelID* m_pixelId{nullptr};
  const SCT_ID* m_sctId{nullptr};
  const TRT_ID* m_trtId{nullptr};

  IntegerProperty m_DetectorMode{this, "DetectorMode", 3, "Barrel = 1, endcap =2, both =3"};
  IntegerProperty m_RunningMode{this, "RunningMode", 2};

  // If all 3 of these -1, defaults depend on cosmic/not
  IntegerProperty m_minSCTHits{this, "MinSCTHits", -1};
  IntegerProperty m_minTRTHits{this, "MinTRTHits", -1};
  IntegerProperty m_minOtherHits{this, "MinOtherHits", 6};

  IntegerProperty m_minPixelHits{this, "MinPixelHits", -1};

  FloatProperty m_maxPhiAngle{this, "MaxPhiAngle", 40., "Degrees, 100 implies no cut."};
  FloatProperty m_maxChi2{this, "MaxChi2", 3.};
  FloatProperty m_maxD0{this, "Maxd0", 10., "mm of D0"};
  FloatProperty m_minPt{this, "MinPt", 1000., "minimu pt in MeV/c"};
  FloatProperty m_effdistcut{this, "effDistanceCut", 2.};
  FloatProperty m_maxZ0sinTheta{this, "MaxZ0sinTheta", 0.};
  UnsignedIntegerProperty m_maxTracks{this, "MaxTracks", 500};

  BooleanProperty m_insideOutOnly{this, "InsideOutOnly", false};
  BooleanProperty m_usemasks{this, "UseMasks", false};
  BooleanProperty m_detailed{this, "Detailed", false};
  BooleanProperty m_superDetailed{this, "SuperDetailed", false};
  BooleanProperty m_usedatabase{this, "LookAtDatabase", false};
  BooleanProperty m_isCosmic{this, "IsCosmic", false};
  BooleanProperty m_isSim{this, "IsSim", false};
  BooleanProperty m_useTRTPhase{this, "UseTRTPhase", false};
  BooleanProperty m_useSCTorTRT{this, "UseSCTorTRT", false};
  BooleanProperty m_requireEnclosingHits{this, "RequireEnclosingHits", false};
  BooleanProperty m_requireOtherFace{this, "RequireOtherFace", false};
  BooleanProperty m_requireGuardRing{this, "RequireGuardRing", false, "should be returned to true"};
  BooleanProperty m_vetoBadChips{this, "VetoBadChips", true};
  BooleanProperty m_chronotime{this, "ChronoTime", false};
  BooleanProperty m_useIDGlobal{this, "useIDGlobal", false};

  std::array < std::array < TProfile2D*, 2 >, SCT_Monitoring::N_LAYERS_TOTAL > m_effMap;
  std::array < std::array < TProfile2D*, 2 >, SCT_Monitoring::N_LAYERS_TOTAL > m_effMapFirstBCID;
  std::array < std::array < TProfile2D*, 2 >, SCT_Monitoring::N_LAYERS_TOTAL > m_ineffMap;
  std::array < std::array < TProfile*, 2 >, SCT_Monitoring::N_LAYERS_TOTAL > m_effLumiBlock;
  std::array < TProfile2D*, SCT_Monitoring::N_LAYERS_TOTAL > m_accMap;
  std::array < TProfile2D*, SCT_Monitoring::N_LAYERS_TOTAL > m_accPhysMap;
  std::array < std::array < TProfile2D*, SCT_Monitoring::N_ENDCAPSx2 >, SCT_Monitoring::N_REGIONS > m_layerResidualHistos;

  TProfile* m_Eff_Total{nullptr};
  TProfile* m_Eff_TotalBCID{nullptr};
  TProfile* m_Eff_hashCodeHisto{nullptr};
  TProfile* m_Eff_LumiBlockHisto_Total{nullptr};
  TH1F* m_effdistribution{nullptr};

  TProfile2D* m_effHashLumiB{nullptr};

  TProfArray m_Eff_summaryHisto{nullptr};
  TProfArray m_Eff_summaryHistoFirstBCID{nullptr};
  TProfArray m_Eff_summaryHisto_old{nullptr};
  TProfArray m_holesPerTrackHisto{nullptr};
  TProfArray m_holesDistPerTrackHisto{nullptr};
  TProfArray m_Unas_summaryHisto{nullptr};
  TProfArray m_Eff_etaHisto{nullptr};
  TProfArray m_Eff_etaPhiCutHisto{nullptr};
  TProfArray m_Eff_ptiHisto{nullptr};
  TProfArray m_Eff_ptHisto{nullptr};
  TProfArray m_Unas_ptiHisto{nullptr};
  TProfArray m_Eff_phiHisto{nullptr};
  TProfArray m_Eff_phiEtaCutHisto{nullptr};
  TProfArray m_Eff_phiHistoFinal{nullptr};
  TProfArray m_Unas_phiHisto{nullptr};
  TProfArray m_Eff_d0Histo{nullptr};
  TProfArray m_Eff_d0PrecHisto{nullptr};
  TProfArray m_Eff_z0Histo{nullptr};
  TProfArray m_Eff_xlocHisto, m_Eff_ylocHistos{nullptr};
  TProfArray m_Unas_xlocHisto, m_Unas_ylocHisto{nullptr};
  TProfArray m_Eff_nSCTHisto{nullptr};
  TProfArray m_Eff_nTRTHisto{nullptr};
  TProfArray m_Eff_nOtherHisto{nullptr};
  TProfArray m_Eff_otherFaceHisto{nullptr};
  TProfArray m_Unas_otherFaceHisto{nullptr};
  TProfArray m_Eff_timecorHisto{nullptr};
  TProfArray m_probEnclosedHisto{nullptr};
  TProfArray m_Eff_SelectionHisto{nullptr};
  TProfArray m_Eff_EventHisto{nullptr};
  TProfArray m_Eff_LumiBlockHisto{nullptr};
  TProfArray m_Eff_chi2Histo{nullptr};
  TProfArray m_Eff_chi2HistoFinal{nullptr};
  TProfArray m_chi2vPhiHisto{nullptr};
  TProfArray m_badModPerSide{nullptr};
  TProfArray m_Eff_summaryIncBadMod{nullptr};
  TProfArray m_Eff_nTrk{nullptr}; 
  TProfArray m_Eff_nGoodTrk{nullptr}; 

  TProf2DArray m_inEffStrip{};
  TProf2DArray m_inEffChip{};

  TH1FArray m_EventHisto{};
  TH1FArray m_SelectionHisto{};
  TH1FArray m_ResidualHisto{};
  TH1FArray m_ResidualUnasHisto{};
  TH1FArray m_ResidualMissHisto{};
  TH1FArray m_timecorHisto{};
  TH1FArray m_nOtherHisto{};
  TH1FArray m_etaTkUsedHisto{};
  TH1FArray m_phiTkUsedHisto{};
  TH1FArray m_ptiTkUsedHisto{};
  TH1FArray m_ptTkUsedHisto{};
  TH1FArray m_d0TkUsedHisto{};
  TH1FArray m_d0PrecTkUsedHisto{};
  TH1FArray m_nTrkUsedHisto{};
  TH1FArray m_z0TkUsedHisto{};
  TH1FArray m_phiLocalHisto, m_phiLocalCutHisto{};
  TH1FArray m_chi2Histo{};
  TH1FArray m_localHitXHisto, m_localHitYHistos{}; 

  TH1F* m_mNHitHisto{nullptr};
  TH1F* m_barrelNHitHisto{nullptr};
  TH1F* m_pNHitHisto{nullptr};
  TH1F* m_SCTNHitHisto{nullptr};
  TH1F* m_trtNHitHisto{nullptr};
  TH1F* m_pixelNHitHisto{nullptr};
  TH1F* m_PtTkHisto{nullptr};
  TH1F* m_etaTkHisto{nullptr};
  TH1F* m_d0TkHisto{nullptr};
  TH1F* m_d0PrecTkHisto{nullptr};
  TH1F* m_nTrkHisto{nullptr};
  TH1F* m_nTrkParsHisto{nullptr};
  TH1F* m_nTrkGoodHisto{nullptr};
  TH1F* m_LumiBlock{nullptr};
  TH1F* m_z0TkHisto{nullptr};
  TH1F* m_hashCodeHisto{nullptr};

  TH2I* m_badModFineMap{nullptr};

  TH2FArray m_localHitHisto{};
  TH2FArray m_localMissHisto{};
  TH2FArray m_localUnasHisto{};
  TH2FArray m_localHitGHisto{};
  TH2FArray m_TwoSidesResiduals{};
  TH2FArray m_TwoSidesResidualsIneff{};
  TH2FArray m_chi2ResidualHisto{};

  TH2FArrayLayer m_xlResidualHisto{};
  TH2FArrayLayer m_xlResidualE0Histo{};
  TH2FArrayLayer m_xlResidualE1Histo{};
  TH2FArrayLayer m_xlResidualUnasHisto{};

  TGraphErrors* m_badModMap{nullptr};
  TGraphErrors* m_badChipMap{nullptr};
};

#endif //SCTEFFICIENCYTOOL_H
