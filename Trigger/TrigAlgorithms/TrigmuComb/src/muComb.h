/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     muComb.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigmuComb
// 
// AUTHORS:   S. Giagu <stefano.giagu@cern.ch>
//            A, Di Mattia <dimattia@mail.cern.ch>
// 
// PURPOSE:  LVL2 Combined Muon FEX Algorithm
// ********************************************************************

#ifndef MUCOMB_H
#define MUCOMB_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "TrigInterfaces/FexAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
//#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTracking/TrackParticle.h"

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;  // legacy trigger code

/** Main LVL2 Algorithm. Sided by a xAOD::L2StandaloneMuon, match the muon spectrometer track with an ID track, and produces a xAOD::L2CombinedMuon. */
class muComb : public HLT::FexAlgo
{
 public:
  /** Constructor */
  muComb(const std::string&, ISvcLocator*);
  /** Destructor */
  ~muComb(){};
   
  /** hltExecute execute the combined muon FEX. */ 
  HLT::ErrorCode hltExecute(const HLT::TriggerElement*, HLT::TriggerElement*);
  /** hltInitialize. Called by the Steering. */
  HLT::ErrorCode hltInitialize();
  /** hltFinalize. Called by the Steering. */
  HLT::ErrorCode hltFinalize();
   
  /** recordFeature. Record on the event record the output xAOD::L2CombinedMuonCOntainer. */ 
  HLT::ErrorCode muCombSeed(HLT::TriggerElement*,xAOD::L2CombinedMuonContainer*);

 private:

  /** Pointer to Storegate. */
  StoreGateSvc*        m_pStoreGate;
    
  /** Pointer to the ROB data provider */
  //ROBDataProviderSvc*  m_pROBDataProvider;
      
  /** Handle to the G4 backExtrapolator tool */
  ToolHandle<Trk::IExtrapolator>  m_backExtrapolatorG4;  

  /** Handle to the Magnetic field service */
  MagField::IMagFieldSvc* m_MagFieldSvc; 

  /** Pointer to Timer Service */ 
  ITrigTimerSvc*  m_pTimerService;

  std::string m_paramSet;
    
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

  /** flag to switch between G4 and LUT based back-extrapolation */
  BooleanProperty m_useBackExtrapolatorG4;

  /** flag to switch on automatic configuration of algo. with magnetic field configuration */
  BooleanProperty m_useAthenaFieldService;
  /** flag to assume B_Toroid=0 anyway */
  BooleanProperty m_assumeToroidOff;
  /** flag to assume B_Solenoid=0 anyway */
  BooleanProperty m_assumeSolenoidOff;

  /** muComb matching strategy:
   *  0: auto select best option
   *  1: simplified R,(Pt) matching
   */
  Gaudi::CheckedProperty<int> m_AlgoStrategy;

  /** muComb charge assignment strategy:
   *  0: useMuFast
   *  1: use ID
   *  2: use resolution model
   */
  Gaudi::CheckedProperty<int> m_ChargeStrategy;

  /** ID Track collection used for matching */
  StringProperty m_ID_algo_to_use;

  /** Min Pt to select the ID track for matching */
  DoubleProperty m_PtMinTrk;
  /** Max abs(eta) to select the ID track for matching */
  DoubleProperty m_EtaMaxTrk;

  /** max deltaPt for simpified matching */
  DoubleProperty m_winPt;
  /** max deltaR for simplified matching */
  DoubleProperty m_winDR;

  /** Min Number of DOF to apply the chi2 cut on macthing based on LUT and G4 backextrapolators */
  IntegerProperty m_NdofMin;
  /** Use Absolute value of the Pt in chi2 calculation (i.e. not use muon charge) */
  BooleanProperty m_UseAbsPt;
  /** Number of sigmas for the Eta matching window LUT backextrapolator (Barrel) */
  DoubleProperty m_WinEta;
  /** Number of sigmas for the Phi matching window LUT backextrapolator (Barrel) */
  DoubleProperty m_WinPhi;
  /** Max Chi2 for the combined muon in LUT backextrapolator (Barrel) */
  DoubleProperty m_Chi2Max;
  /** Number of sigmas for the Eta matching window LUT backextrapolator (EndCaps) */
  DoubleProperty m_WinEta_EC;
  /** Number of sigmas for the Phi matching window LUT backextrapolator (EndCaps) */
  DoubleProperty m_WinPhi_EC;
  /** Max Chi2 for the combined muon in LUT backextrapolator (EndCaps) */
  DoubleProperty m_Chi2Max_EC;
  /** Number of sigmas for the Eta matching window LUT backextrapolator (Trigger Stations) */
  DoubleProperty m_WinEta_TS;
  /** Number of sigmas for the Phi matching window LUT backextrapolator (Trigger Stations) */
  DoubleProperty m_WinPhi_TS;
  /** Max Chi2 for the combined muon in LUT backextrapolator (Trigger Stations) */
  DoubleProperty m_Chi2Max_TS;
  /** Number of sigmas for the Eta matching window LUT backextrapolator (MS Feet region) */
  DoubleProperty m_WinEta_FE;
  /** Number of sigmas for the Phi matching window LUT backextrapolator (MS Feet region) */
  DoubleProperty m_WinPhi_FE;
  /** Max Chi2 for the combined muon in LUT backextrapolator (MS Feet region) */
  DoubleProperty m_Chi2Max_FE;
  /** Scale factor for the Eta matching window in LUT backextrapolator */
  DoubleProperty m_WeightEta;
  /** Scale factor for the Phi matching window in LUT backextrapolator */
  DoubleProperty m_WeightPhi;

  /** Number of sigmas for the Eta matching window LUT backextrapolator (Barrel) */
  DoubleProperty m_WinEta_g4;
  /** Number of sigmas for the Phi matching window LUT backextrapolator (Barrel) */
  DoubleProperty m_WinPhi_g4;
  /** Max Chi2 for the combined muon in LUT backextrapolator (Barrel) */
  DoubleProperty m_Chi2Max_g4;
  /** Number of sigmas for the Eta matching window LUT backextrapolator (EndCaps) */
  DoubleProperty m_WinEta_EC_g4;
  /** Number of sigmas for the Phi matching window LUT backextrapolator (EndCaps) */
  DoubleProperty m_WinPhi_EC_g4;
  /** Max Chi2 for the combined muon in LUT backextrapolator (EndCaps) */
  DoubleProperty m_Chi2Max_EC_g4;
  /** Scale factor for the Eta matching window in LUT backextrapolator */
  DoubleProperty m_WeightEta_g4;
  /** Scale factor for the Phi matching window in LUT backextrapolator */
  DoubleProperty m_WeightPhi_g4;
  /** Scale factor for the Chi2 1/pt resolutions (MS and ID) (Barrel) */
  DoubleProperty m_Chi2Weight_g4;

  /** configurable string to be used for regular test */
  StringProperty m_test_string;

  /** monitored quantities */
  float m_ptMS;
  float m_etaMS;
  float m_phiMS;
  float m_zetaMS;
  float m_ptID;
  float m_etaID;
  float m_phiID;
  float m_zetaID;
  float m_ptMC;
  float m_dEta;
  float m_dPhi;
  float m_dZeta;
  float m_dR;
  float m_ptFL;
  float m_etaFL;
  float m_phiFL;
  int m_efficiency;
  int m_StrategyMC;
  int m_ErrorFlagMC;
  int m_MatchFlagMC;

  std::vector<double> m_IDSCANRes_barrel;
  std::vector<double> m_IDSCANRes_endcap1;
  std::vector<double> m_IDSCANRes_endcap2;
  std::vector<double> m_IDSCANRes_endcap3;
  std::vector<double> m_IDSCANRes_endcap4;
};

#endif // MUCOMB_H
