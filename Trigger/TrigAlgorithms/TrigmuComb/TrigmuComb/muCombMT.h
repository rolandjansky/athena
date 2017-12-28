/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     muCombMT.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigmuComb
// 
// AUTHORS:   S. Giagu <stefano.giagu@cern.ch>
// 
// PURPOSE:  LVL2 Combined Muon FEX Algorithm (MT version)
// ********************************************************************

#ifndef MUCOMBMT_H
#define MUCOMBMT_H

// standard stuff
#include <vector>
#include <cmath> 

// general athena stuff
#include "GaudiKernel/IToolSvc.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "TrkExInterfaces/IExtrapolator.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"

//#include "TrigSteeringEvent/TrigRoiDescriptor.h"
//#include "TrigT1Interfaces/RecMuonRoI.h"

#include "AthenaMonitoring/GenericMonitoringTool.h"

/** Main LVL2 Algorithm. Sided by a xAOD::L2StandaloneMuon, match the muon spectrometer track with an ID track, and produces a xAOD::L2CombinedMuon. */
class muCombMT : public AthAlgorithm
{
 public:
  /** Constructor */
  muCombMT(const std::string&, ISvcLocator*);
  /** Destructor */
  ~muCombMT(){};
   
  /** initialize. Called by the Steering. */
  StatusCode initialize();
  /** finalize. Called by the Steering. */
  StatusCode finalize();
  /** execute execute the combined muon FEX. */ 
  StatusCode execute();
   
 private:

  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_TrackParticleContainerKey{ this,
      "TrackParticlesContainerName",            // property name
      "InDetTrigTrackingxAODCnv_Muon_FTF",      // default value of StoreGate key
      "input TrackParticle container name"};

  SG::ReadHandleKey<xAOD::L2StandAloneMuonContainer> m_muonCollKey{ this,
      "L2StandAloneMuonContainerName",          // property name
      "xAOD::L2StandAloneMuonContainer",        // default value of StoreGate key
      "input L2StandAloneMuon Container name"};


  SG::WriteHandleKey<xAOD::L2CombinedMuonContainer> m_outputCBmuonCollKey{ this,
      "L2CombinedMuonContainerName",           // property name
      "MuonL2CBInfo",                          // default value of StoreGate key
      "output CB Muon container name"};

  ToolHandle<GenericMonitoringTool> m_monTool;

  /** Handle to the G4 backExtrapolator tool */
  //ToolHandle<Trk::IExtrapolator>  m_backExtrapolatorG4;  
  PublicToolHandle<Trk::IExtrapolator> m_backExtrapolatorG4 {this,
     "AtlasExtrapolator",
     "Trk::Extrapolator/AtlasExtrapolator",
     "Tool to backextrapolate muon from MS to IP (G4 based)"};

  /** Handle to the Magnetic field service */
  //ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;       //!< helper tool to get the magnetic field
  MagField::IMagFieldSvc* m_MagFieldSvc; 


  int    drptMatch(double, double, double,
                   double, double, double, int, 
		   double&, double&, double&, double&, double&);

  int    drptMatch(const xAOD::L2StandAloneMuon* feature, 
                   double, double, double, int, 
		   double&, double&, double&, double&, double&);

  int    mfMatch(const xAOD::L2StandAloneMuon* feature, 
                 double, double, double, double,
		 double&, double&, double&, double&, double&, int&);

  int    g4Match(const xAOD::L2StandAloneMuon* feature,
                 double, double, double, double, double, double, double,
		 double&, double&, double&, double&, double&, int&);

 private:

  // Properties

  /** flag to switch between G4 and LUT based back-extrapolation */
   Gaudi::Property<bool> m_useBackExtrapolatorG4 {this, "UseBackExtrapolatorG4", true, "Use g4-based back-extraploator"};

  /** flag to switch on automatic configuration of algo. with magnetic field configuration */
   Gaudi::Property<bool> m_useAthenaFieldService {this, "UseAthenaFieldService", true, "Use AthenaFieldService to check magnetic fields"};
  /** flag to assume B_Toroid=0 anyway */
   Gaudi::Property<bool> m_assumeToroidOff       {this, "AssumeToroidOff",       false, "Force assumption toroid off"};
  /** flag to assume B_Solenoid=0 anyway */
   Gaudi::Property<bool> m_assumeSolenoidOff     {this, "AssumeSolenoidOff",     false, "Force assumption solenoid off"};


  /** muComb matching strategy:
   *  0: auto select best option
   *  1: simplified R,(Pt) matching
   */
   Gaudi::Property<int> m_AlgoStrategy {this, "MuCombStrategy", 0, "Algoritgm strategy [0=auto select best option,1=simple angular match]"};

  /** muComb charge assignment strategy:
   *  0: useMuFast
   *  1: use ID
   *  2: use resolution model
   */
   Gaudi::Property<int> m_ChargeStrategy {this, "ChargeStrategy", 0, "Charge assignement strategy [0=muFast,1=ID,2=resolution based]"};

  /** ID Track collection used for matching */
   Gaudi::Property<std::string> m_ID_algo_to_use {this, "IDalgo", "InDetTrigTrackingxAODCnv_Muon_FTF","ID trk xAOD collection to use"};

  /** Min Pt to select the ID track for matching */
   Gaudi::Property<double> m_PtMinTrk  {this, "MinPtTRK",     1.*CLHEP::GeV, "ID track minimum pT"}; //GeV/c
  /** Max abs(eta) to select the ID track for matching */
   Gaudi::Property<double> m_EtaMaxTrk {this, "MaxAbsEtaTRK", 2.5, "ID tracks max |eta|"};

   // Matching Parameters
   //Common
  /** Min Number of DOF to apply the chi2 cut on macthing based on LUT and G4 backextrapolators */
   Gaudi::Property<int>  m_NdofMin   {this, "NdofMin",     9999, "matching parameter: min ndof"};
  /** Use Absolute value of the Pt in chi2 calculation (i.e. not use muon charge) */
   Gaudi::Property<bool> m_UseAbsPt  {this, "UseAbsPt",    false, "matching parameter: use absolute pT"};

   // LUT based backextrapolator
  /** Number of sigmas for the Eta matching window LUT backextrapolator (Barrel) */
   Gaudi::Property<double>  m_WinEta         {this, "WinEtaSigma",          4.0, "matching parameter: LUT-based"};
  /** Number of sigmas for the Phi matching window LUT backextrapolator (Barrel) */
   Gaudi::Property<double>  m_WinPhi         {this, "WinPhiSigma",          4.0, "matching parameter: LUT-based"};
  /** Max Chi2 for the combined muon in LUT backextrapolator (Barrel) */
   Gaudi::Property<double>  m_Chi2Max        {this, "Chi2Max",              1.e33, "matching parameter: LUT-based"};
  /** Number of sigmas for the Eta matching window LUT backextrapolator (EndCaps) */
   Gaudi::Property<double>  m_WinEta_EC      {this, "WinEtaSigmaEndCaps",   4.0, "matching parameter: LUT-based"};
  /** Number of sigmas for the Phi matching window LUT backextrapolator (EndCaps) */
   Gaudi::Property<double>  m_WinPhi_EC      {this, "WinPhiSigmaEndCaps",   10., "matching parameter: LUT-based"};
  /** Max Chi2 for the combined muon in LUT backextrapolator (EndCaps) */
   Gaudi::Property<double>  m_Chi2Max_EC     {this, "Chi2MaxEndCaps",       1.e33, "matching parameter: LUT-based"};
  /** Number of sigmas for the Eta matching window LUT backextrapolator (Trigger Stations) */
   Gaudi::Property<double>  m_WinEta_TS      {this, "WinEtaSigmaTS",        5.0, "matching parameter: LUT-based"};
  /** Number of sigmas for the Phi matching window LUT backextrapolator (Trigger Stations) */
   Gaudi::Property<double>  m_WinPhi_TS      {this, "WinPhiSigmaTS",        5.0, "matching parameter: LUT-based"};
  /** Max Chi2 for the combined muon in LUT backextrapolator (Trigger Stations) */
   Gaudi::Property<double>  m_Chi2Max_TS     {this, "Chi2MaxTS",            1.e33, "matching parameter: LUT-based"};
  /** Number of sigmas for the Eta matching window LUT backextrapolator (MS Feet region) */
   Gaudi::Property<double>  m_WinEta_FE      {this, "WinEtaSigmaFE",        10.0, "matching parameter: LUT-based"};
  /** Number of sigmas for the Phi matching window LUT backextrapolator (MS Feet region) */
   Gaudi::Property<double>  m_WinPhi_FE      {this, "WinPhiSigmaFE",        10.0, "matching parameter: LUT-based"};
  /** Max Chi2 for the combined muon in LUT backextrapolator (MS Feet region) */
   Gaudi::Property<double>  m_Chi2Max_FE     {this, "Chi2MaxFE",            1.e33, "matching parameter: LUT-based"};
  /** Scale factor for the Eta matching window in LUT backextrapolator */
   Gaudi::Property<double>  m_WeightEta      {this, "WeightEta",            2.0, "matching parameter: LUT-based"};
  /** Scale factor for the Phi matching window in LUT backextrapolator */
   Gaudi::Property<double>  m_WeightPhi      {this, "WeightPhi",            2.0, "matching parameter: LUT-based"};
                                             
   // g4 based backextrapolator
  /** Number of sigmas for the Eta matching window LUT backextrapolator (Barrel) */
   Gaudi::Property<double>  m_WinEta_g4      {this, "WinEtaSigma_g4",       7.0, "matching parameter: G4-based"};
  /** Number of sigmas for the Phi matching window LUT backextrapolator (Barrel) */
   Gaudi::Property<double>  m_WinPhi_g4      {this, "WinPhiSigma_g4",       7.0, "matching parameter: G4-based"};
  /** Max Chi2 for the combined muon in LUT backextrapolator (Barrel) */
   Gaudi::Property<double>  m_Chi2Max_g4     {this, "Chi2Max_g4",           1.e33, "matching parameter: G4-based"};
  /** Number of sigmas for the Eta matching window LUT backextrapolator (EndCaps) */
   Gaudi::Property<double>  m_WinEta_EC_g4   {this, "WinEtaSigmaEndCaps_g4",7.0, "matching parameter: G4-based"};
  /** Number of sigmas for the Phi matching window LUT backextrapolator (EndCaps) */
   Gaudi::Property<double>  m_WinPhi_EC_g4   {this, "WinPhiSigmaEndCaps_g4",7.0, "matching parameter: G4-based"};
  /** Max Chi2 for the combined muon in LUT backextrapolator (EndCaps) */
   Gaudi::Property<double>  m_Chi2Max_EC_g4  {this, "Chi2MaxEndCaps_g4",    1.e33, "matching parameter: G4-based"};
  /** Scale factor for the Eta matching window in LUT backextrapolator */
   Gaudi::Property<double>  m_WeightEta_g4   {this, "WeightEta_g4",         2.0, "matching parameter: G4-based"};
  /** Scale factor for the Phi matching window in LUT backextrapolator */
   Gaudi::Property<double>  m_WeightPhi_g4   {this, "WeightPhi_g4",         2.0, "matching parameter: G4-based"};
  /** Scale factor for the Chi2 1/pt resolutions (MS and ID) (Barrel) */
   Gaudi::Property<double>  m_Chi2Weight_g4  {this, "Chi2Weight_g4",        2.0, "matching parameter: G4-based"};

   // Simplified DeltaR(/Pt) based match
  /** max deltaPt for simpified matching */
   Gaudi::Property<double> m_winPt {this, "WinPt",       -1.0*CLHEP::GeV, "matching parameter (simplifed): pT windows in geV (disabled if < 0)"};
  /** max deltaR for simplified matching */
   Gaudi::Property<double> m_winDR {this, "WinDelta",     0.2, "matching parameter (simplifed): strategy dependent"};

   // ID parametrized rsolution for matching
   Gaudi::Property<std::vector<double>> m_IDSCANRes_barrel  {this, "IDSCANBarrelRes", {0.017, 0.000000418}, "ID parametrized rsolution for matching (barrel)"};
   Gaudi::Property<std::vector<double>> m_IDSCANRes_endcap1 {this, "IDSCANEndcap1Res",{0.025, 0.0000002}, "ID parametrized rsolution for matching (ec1)"};
   Gaudi::Property<std::vector<double>> m_IDSCANRes_endcap2 {this, "IDSCANEndcap2Res",{0.030, 0.0000002}, "ID parametrized rsolution for matching (ec2)"};
   Gaudi::Property<std::vector<double>> m_IDSCANRes_endcap3 {this, "IDSCANEndcap3Res",{0.036, 0.0000004}, "ID parametrized rsolution for matching (ec3)"};
   Gaudi::Property<std::vector<double>> m_IDSCANRes_endcap4 {this, "IDSCANEndcap4Res",{0.046, 0.0000002}, "ID parametrized rsolution for matching (ec4)"};

};
DECLARE_ALGORITHM_FACTORY( muCombMT )
#endif // MUCOMB_H
