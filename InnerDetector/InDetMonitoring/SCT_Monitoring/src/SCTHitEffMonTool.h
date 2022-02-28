// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "SCT_Monitoring/SCT_MonitoringNumbers.h"

#include "CommissionEvent/ComTime.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "LumiBlockData/BunchCrossingCondData.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkTrack/Track.h" 
#include "TrkTrack/TrackCollection.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"

//STL
#include <array>
#include <string>

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
  StatusCode failCut(bool value, const std::string & name) const;

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
  SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCondObjInputKey{this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};
  SG::ReadCondHandleKey<BunchCrossingCondData> m_bunchCrossingKey{this, "BunchCrossingKey", "BunchCrossingData", "Key BunchCrossing CDO" }; 

  ToolHandle<Trk::IResidualPullCalculator> m_residualPullCalculator{this, "ResPullCalc", "Trk::ResidualPullCalculator/ResidualPullCalculator"};
  ToolHandle<Trk::IRIO_OnTrackCreator> m_rotcreator{this, "ROTCreator", "InDet::SCT_ClusterOnTrackTool/SCT_ClusterOnTrackTool"};
  ToolHandle<Trk::ITrackHoleSearchTool> m_holeSearchTool{this, "HoleSearch", "InDet::InDetTrackHoleSearchTool"};
  ToolHandle<ISCT_ConfigurationConditionsTool> m_configConditions{this, "ConfigConditions",
      "SCT_ConfigurationConditionsTool/InDetSCT_ConfigurationConditionsTool", "Tool to retrieve SCT Configuration Tool"};

  const PixelID* m_pixelId{nullptr};
  const SCT_ID* m_sctId{nullptr};
  const TRT_ID* m_trtId{nullptr};

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
  BooleanProperty m_isCosmic{this, "IsCosmic", false};
  BooleanProperty m_useTRTPhase{this, "UseTRTPhase", false};
  BooleanProperty m_useSCTorTRT{this, "UseSCTorTRT", false};
  BooleanProperty m_requireEnclosingHits{this, "RequireEnclosingHits", false};
  BooleanProperty m_requireOtherFace{this, "RequireOtherFace", false};
  BooleanProperty m_requireGuardRing{this, "RequireGuardRing", false, "should be returned to true"};
  BooleanProperty m_vetoBadChips{this, "VetoBadChips", true};
  BooleanProperty m_useIDGlobal{this, "useIDGlobal", false};

  std::array < std::array < TProfile2D*, 2 >, SCT_Monitoring::N_LAYERS_TOTAL > m_effMap{};
  std::array < std::array < TProfile2D*, 2 >, SCT_Monitoring::N_LAYERS_TOTAL > m_effMapFirstBCID{};
  std::array < std::array < TProfile2D*, 2 >, SCT_Monitoring::N_LAYERS_TOTAL > m_ineffMap{};
  std::array < std::array < TProfile*, 2 >, SCT_Monitoring::N_LAYERS_TOTAL > m_effLumiBlock{};

  TProfile* m_Eff_Total{nullptr};
  TProfile* m_Eff_TotalBCID{nullptr};
  TProfile* m_Eff_hashCodeHisto{nullptr};
  TProfile* m_Eff_LumiBlockHisto_Total{nullptr};
  TH1F* m_effdistribution{nullptr};

  TProfArray m_Eff_summaryHisto{nullptr};
  TProfArray m_Eff_summaryHistoFirstBCID{nullptr};
  TProfArray m_Eff_LumiBlockHisto{nullptr};
};

#endif //SCTEFFICIENCYTOOL_H
