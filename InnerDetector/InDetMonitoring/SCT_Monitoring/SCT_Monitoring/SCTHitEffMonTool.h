/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** Tool to measure the intrinsic single hit efficiency in the SCT
    This tool is part of the SCT_Monitoring package. Its aim is to
    measure the intrinsic single hit efficiency in the SCT using
    the holes-on-tracks method.
    @author SCT Monitoring group hn-atlas-sct-monitoring@cern.ch
 */

#ifndef SCTEFFICIENCYTOOL_H
#define SCTEFFICIENCYTOOL_H
//STL
#include <string>
//#include <boost/array.hpp>
#include <array>

//Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

//Athena
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

//cannot fwd declare due to typedefs
#include "TrkTrack/Track.h" 
#include "TrkTrack/TrackCollection.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"

#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

//SCT
#include "SCT_ConditionsServices/ISCT_FlaggedConditionSvc.h"
#include "SCT_Monitoring/SCT_MonitoringNumbers.h"
#include "TString.h"

//
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "StoreGate/ReadHandleKey.h"
#include "CommissionEvent/ComTime.h"
#include "xAODEventInfo/EventInfo.h"

class Identifier;
class PixelID;
class SCT_ID;
class TRT_ID;
class ISCT_ConfigurationConditionsSvc;
class IInterface;
class TH1D;
class TH1F;
class TH2I;
class TH1I;
class TProfile2D;
class TProfile;
class TGraphErrors;

class IChronoStatSvc;
class StatusCode;

//using namespace SCT_Monitoring;
//using boost::array;


/**
   @class SCTHitEffMonTool
   The tool itself
*/
class SCTHitEffMonTool : public ManagedMonitorToolBase  {
  
 public:
  typedef Trk::Track Track;
  /** Constructor */
  SCTHitEffMonTool (const std::string &type, const std::string &name, const IInterface* parent);
  /** Destructor */
  ~SCTHitEffMonTool (); 

  /** Histogram booking method */
  //  virtual StatusCode bookHistograms (bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun ); // hidetoshi 14.01.22
  virtual StatusCode bookHistograms ();                                                               // hidetoshi 14.01.22
  virtual StatusCode bookHistogramsRecurrent ();                                                      // hidetoshi 14.01.22

  /** Histogram filling method */
  virtual StatusCode fillHistograms ();
  //  virtual StatusCode procHistograms (bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun );  // hidetoshi 14.01.22
  virtual StatusCode procHistograms ();                                                                      // hidetoshi 14.01.22

private:

  StatusCode initialize();

  const InDetDD::SCT_DetectorManager * m_mgr;

  /** Method to cut on track or hit variables and automatize DEBUG statements */
  StatusCode failCut (Bool_t value, std::string name);

  /** Method to compute incident angle of track to wafer */
  StatusCode findAnglesToWaferSurface (const Amg::Vector3D &mom, Identifier id,
				       Double_t &theta, Double_t &phi);

  /** Method to find the chip just before a given hit */
  Int_t previousChip (Double_t xl, Int_t side, bool swap);

  /** Computes residual of a hit to a track */
  Double_t getResidual (const Identifier& surfaceID,
			const Trk::TrackParameters * trkParam,
			const InDet::SCT_ClusterContainer* p_sctclcontainer);

  /** Single histogram booking method */
  template < class T > StatusCode bookEffHisto (T*& histo, MonGroup & MG, 
						TString name, TString title, 
						Int_t nbin, Double_t x1, Double_t x2);

  template < class T > StatusCode bookEffHisto (T*& histo, MonGroup & MG, 
						TString name, TString title,
						Int_t nbinx, Double_t x1, Double_t x2,
						Int_t nbiny, Double_t y1, Double_t y2);

  template < class T > StatusCode bookEffHisto (T*& histo, MonGroup & MG, 
						TString name, TString title,
						Int_t nbinx, Double_t * xbins,
						Int_t nbiny, Double_t * ybins);

  const SCT_ID*   m_pSCTHelper;
  const InDetDD::SCT_DetectorManager*   m_pManager;
  SG::ReadHandle<TrackCollection> m_TrackName;
  IChronoStatSvc * m_chrono;
  const TrackCollection * m_tracks; 
  Trk::TrackSummaryTool* m_TrackSum;

  const std::map < Identifier, unsigned int > * m_badChips;
  ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle;
  ToolHandle<Trig::IBunchCrossingTool> m_bunchCrossingTool;


  Int_t m_DetectorMode;
  Int_t m_RunningMode;
  Int_t m_minPixelHits;
  Int_t m_minTRTHits;
  Int_t m_minSCTHits;
  Int_t m_minOtherHits; 

  Float_t m_maxPhiAngle;
  Float_t m_maxChi2;
  Float_t m_maxD0;
  Float_t m_minPt;
  Float_t m_effdistcut;
  Float_t m_maxZ0sinTheta;
  UInt_t m_maxTracks;

  std::string m_path;
  SG::ReadHandleKey<InDet::SCT_ClusterContainer> m_sctContainerName;


  Bool_t m_insideOutOnly;
  Bool_t m_usemasks;
  Bool_t m_detailed, m_superDetailed; 
  Bool_t m_usedatabase;
  Bool_t m_isCosmic; 
  Bool_t m_isSim;
  Bool_t m_useTRTPhase; 
  Bool_t m_useSCTorTRT; 
  Bool_t m_requireEnclosingHits;
  Bool_t m_requireOtherFace; 
  Bool_t m_requireGuardRing;
  Bool_t m_vetoBadChips;
  Bool_t m_chronotime;
  Bool_t m_useIDGlobal;

  ToolHandle < Trk::IResidualPullCalculator >   m_residualPullCalculator;
  ToolHandle < Trk::IRIO_OnTrackCreator > m_rotcreator; 
  ToolHandle < Trk::ITrackHoleSearchTool >  m_holeSearchTool;  

  ServiceHandle < ISCT_FlaggedConditionSvc > m_flaggedConditionSvc;
  ServiceHandle < ISCT_ConfigurationConditionsSvc > m_configConditions;


  typedef std::array < TProfile*, SCT_Monitoring::N_REGIONS > TProfArray;
  typedef std::array < TH1F*, SCT_Monitoring::N_REGIONS > TH1FArray;
  typedef std::array < TH2F*, SCT_Monitoring::N_REGIONS > TH2FArray;
  typedef std::array < std::array < TH2F*, SCT_Monitoring::N_ENDCAPS >, SCT_Monitoring::N_REGIONS > TH2FArrayLayer;

  std::array < std::array < TProfile2D *, 2 >, SCT_Monitoring::N_LAYERS_TOTAL > m_effMap;
  std::array < std::array < TProfile2D *, 2 >, SCT_Monitoring::N_LAYERS_TOTAL > m_ineffMap;
  std::array < std::array < TProfile *, 2 >, SCT_Monitoring::N_LAYERS_TOTAL > m_effLumiBlock;
  std::array < TProfile2D *, SCT_Monitoring::N_LAYERS_TOTAL > m_accMap;
  std::array < TProfile2D *, SCT_Monitoring::N_LAYERS_TOTAL > m_accPhysMap;
  std::array < std::array < TProfile2D *, SCT_Monitoring::N_ENDCAPSx2 >, SCT_Monitoring::N_REGIONS > m_layerResidualHistos;

  TProfile * m_Eff_Total;
  TProfile * m_Eff_TotalBCID;
  TProfile * m_Eff_hashCodeHisto;
	TProfile * m_Eff_LumiBlockHisto_Total;
  TH1F* m_effdistribution;

  TProfile2D * m_effHashLumiB;

  TProfArray m_Eff_summaryHisto;
  TProfArray m_Eff_summaryHistoFirstBCID;
  TProfArray m_Eff_summaryHisto_old;
  TProfArray m_holesPerTrackHisto;
  TProfArray m_holesDistPerTrackHisto;
  TProfArray m_Unas_summaryHisto;
  TProfArray m_Eff_etaHisto;
  TProfArray m_Eff_etaPhiCutHisto;
  TProfArray m_Eff_ptiHisto;
  TProfArray m_Eff_ptHisto;
  TProfArray m_Unas_ptiHisto;
  TProfArray m_Eff_phiHisto;
  TProfArray m_Eff_phiEtaCutHisto;
  TProfArray m_Eff_phiHistoFinal;
  TProfArray m_Unas_phiHisto;
  TProfArray m_Eff_d0Histo;
  TProfArray m_Eff_d0PrecHisto;
  TProfArray m_Eff_z0Histo;
  TProfArray m_Eff_xlocHisto, m_Eff_ylocHistos;
  TProfArray m_Unas_xlocHisto, m_Unas_ylocHisto;
  TProfArray m_Eff_nSCTHisto;
  TProfArray m_Eff_nTRTHisto;
  TProfArray m_Eff_nOtherHisto;
  TProfArray m_Eff_otherFaceHisto;
  TProfArray m_Unas_otherFaceHisto;
  TProfArray m_Eff_timecorHisto;
  TProfArray m_probEnclosedHisto;
  TProfArray m_Eff_SelectionHisto;
  TProfArray m_Eff_EventHisto;
  TProfArray m_Eff_LumiBlockHisto;
  TProfArray m_Eff_chi2Histo;
  TProfArray m_Eff_chi2HistoFinal;
  TProfArray m_chi2vPhiHisto;
  TProfArray m_badModPerSide;
  TProfArray m_Eff_summaryIncBadMod;
  TProfArray m_Eff_nTrk; 
  TProfArray m_Eff_nGoodTrk; 

  std::array < TProfile2D *, SCT_Monitoring::N_REGIONS > m_inEffStrip;
  std::array < TProfile2D *, SCT_Monitoring::N_REGIONS > m_inEffChip;

  TH1FArray m_EventHisto;
  TH1FArray m_SelectionHisto;
  TH1FArray m_ResidualHisto;
  TH1FArray m_ResidualUnasHisto;
  TH1FArray m_ResidualMissHisto;
  TH1FArray m_timecorHisto;
  TH1FArray m_nOtherHisto;
  TH1FArray m_etaTkUsedHisto;
  TH1FArray m_phiTkUsedHisto;
  TH1FArray m_ptiTkUsedHisto;
  TH1FArray m_ptTkUsedHisto;
  TH1FArray m_d0TkUsedHisto;
  TH1FArray m_d0PrecTkUsedHisto;
  TH1FArray m_nTrkUsedHisto;
  TH1FArray m_z0TkUsedHisto;
  TH1FArray m_phiLocalHisto, m_phiLocalCutHisto;
  TH1FArray m_chi2Histo;
  TH1FArray m_localHitXHisto, m_localHitYHistos; 

  TH1F * m_mNHitHisto, *m_barrelNHitHisto,  *m_pNHitHisto;
  TH1F * m_SCTNHitHisto, *m_trtNHitHisto,  *m_pixelNHitHisto;
  TH1F * m_PtTkHisto;
  TH1F * m_etaTkHisto;
  TH1F * m_d0TkHisto;
  TH1F * m_d0PrecTkHisto;
  TH1F * m_nTrkHisto, * m_nTrkParsHisto, * m_nTrkGoodHisto;
  TH1F * m_LumiBlock;
  TH1F * m_z0TkHisto;
  TH1F * m_hashCodeHisto;

  TH2I * m_badModFineMap;

  TH2FArray m_localHitHisto, m_localMissHisto, m_localUnasHisto;
  TH2FArray m_localHitGHisto;
  TH2FArray m_TwoSidesResiduals;
  TH2FArray m_TwoSidesResidualsIneff;
  TH2FArray m_chi2ResidualHisto;

  TH2FArrayLayer m_xlResidualHisto;
  TH2FArrayLayer m_xlResidualE0Histo;
  TH2FArrayLayer m_xlResidualE1Histo;
  TH2FArrayLayer m_xlResidualUnasHisto;

  TGraphErrors * m_badModMap;
  TGraphErrors * m_badChipMap;

  Int_t m_countEvent;
  const PixelID * m_pixelId;
  const SCT_ID * m_sctId;
  const TRT_ID * m_trtId;

  SG::ReadHandleKey<ComTime> m_comTimeName;
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey;

  /**Convert a layer/disk number (0-21) to a bec index (0,1,2) according to position of that layer
   * Numbering is counter-intuitive, would expect C then B then A; in fact the original ordering was A, C, B
   * I have re-ordered this!!!! so now its C,B,A
   **/ 
  SCT_Monitoring::BecIndex layerIndex2becIndex(const Int_t index){
    if (index< 0 || index>21) return SCT_Monitoring::INVALID_INDEX;
    if (index< 9) return SCT_Monitoring::ENDCAP_C_INDEX;
    if (index< 13) return SCT_Monitoring::BARREL_INDEX;
    if (index< 22) return SCT_Monitoring::ENDCAP_A_INDEX;
    return SCT_Monitoring::INVALID_INDEX;
  }
  ///Convert a layer/disk number (0-21) to a layer number (0-8 for endcaps, 0-3 for barrel)
  Int_t layerIndex2layer(const Int_t index){
    if (index < 0 || index > 21) return SCT_Monitoring::INVALID_INDEX;
    if (index < 9) return index;
    if (index < 13) return index-9;
    if (index < 22) return index-13;
    return SCT_Monitoring::INVALID_INDEX;
  }
  
  Int_t becIdxLayer2Index(const Int_t becIdx, const Int_t layer){
    switch( becIdx ) {
    case SCT_Monitoring::ENDCAP_C_INDEX:
      return layer;
    case SCT_Monitoring::BARREL_INDEX:
      return layer + SCT_Monitoring::N_DISKS;
    case SCT_Monitoring::ENDCAP_A_INDEX: 
      return layer + SCT_Monitoring::N_DISKS + SCT_Monitoring::N_BARRELS;
    default:
      return -1;
    }
  }

};

#endif //SCTEFFICIENCYTOOL_H

