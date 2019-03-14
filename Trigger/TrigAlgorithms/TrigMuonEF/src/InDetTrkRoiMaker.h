/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEF_INDETTRKROIMAKER_H
#define TRIGMUONEF_INDETTRKROIMAKER_H

// ************************************************
//
// NAME:     InDetTrkRoiMaker.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigMuonEF
//
// AUTHORS:   Alex Oh, Martin Woudstra. Inspired by code from Mark Owen.
//
// ************************************************

#include "TrigInterfaces/Algo.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInterfaces/FexAlgo.h"

class InDetTrkRoiMaker : public HLT::FexAlgo {

 public:

  
  /// Constructor
  InDetTrkRoiMaker(const std::string & name, ISvcLocator* pSvcLocator);

  /// Destructor
  ~InDetTrkRoiMaker();

  /// Initialize
  HLT::ErrorCode hltInitialize();

  /// Execute for each event
  HLT::ErrorCode hltExecute(const HLT::TriggerElement*, HLT::TriggerElement*);
  
  /// Finalize
  HLT::ErrorCode hltFinalize();

 protected:

  //
  // nested types
  //
  enum EFSMode { kFullScan, // Make a full-scan RoI
                 kSeededScan, // Make SuperRoI filled with RoI's based on input track seeds, within |eta| < etaMax
                 kHybridScan, // use seeded mode for |eta| <= etaMax, and full-scan mode for |eta| > etaMax
                 kNumberOfModes }; // for book keeping

  //
  // static data
  //
  static constexpr const char* s_scanModeNames[kNumberOfModes]{ "FullScan", "SeededScan", "HybridScan" };
  
  //
  // static functions
  //
  static EFSMode modeNameToEnum( const std::string& modeName );

  static TrigInDetTrack::AlgoId seedsNameToEnum( const std::string& seedsName ); 

  static std::string getScanModeHelp();

  //
  // member functions
  //
  TrigRoiDescriptor* createSingleTrigRoiDescriptor( double eta, double phi, double etaWidth, double phiWidth, int roiNum );

  //
  // member data
  //
  
  /** Identifier for the ID algorithm to use. Set from m_ID_algo_to_use Property */
  TrigInDetTrack::AlgoId m_seedsAlgoId;

  /** Full-scan mode for internal use. Value set from Property m_fullScanModeProperty */
  EFSMode m_fullScanMode;

  //
  // JobOptions Properties
  //

  std::string m_fullScanModeProperty;

  std::string m_ID_algo_to_use;

  std::string m_outputRoiLabel;
  
  /** Full eta range of the whole detector. Will be used as upper eta limit in Hybrid mode for the end-caps. */
  double m_fullEtaRange; 
  
  /** In SeededMode and Hybrid mode: only use track seeds with  |eta| <= m_seedsEtaMax.
      In Hybrid mode: In addition, do full scan for |eta| >  m_seedsEtaMax. */
  double m_EtaMaxTrk;

  /** In SeededMode and HybridMode: only use tracks seeds with pT > m_seedsPtMin (GeV/c) */
  double m_PtMinTrk;

  /** Max abs(Zeta) to select the ID track for matching */
  double m_ZMaxTrk;

  /** Max chi2 to select the ID track for matching */
  double m_Chi2MaxTrk;

  /** Min number of PIX space points to select the ID track for matching */
  int m_NPIXhitMinTrk;

  /** Min number of SCT space points to select the ID track for matching */
  int m_NSCThitMinTrk;

  //
  // Monitoring variables
  //
  int m_nTrksIn;
  int m_nRoiOut;
  double m_nRoiToTrkRatio;
  std::vector<double> m_ptTrkIn;
  std::vector<double> m_etaTrkIn;
  std::vector<double> m_phiTrkIn;
  std::vector<double> m_ptRoiOut;
  std::vector<double> m_etaRoiOut;
  std::vector<double> m_phiRoiOut;
  std::vector<double> m_dEtaTrkSel;
  std::vector<double> m_dPhiTrkSel;
  std::vector<double> m_dRTrkSel;
  std::vector<double> m_logdRTrkSel;


};//class InDetTrkRoiMaker


#endif // TRIGMUONEF_INDETTRKROIMAKER_H
