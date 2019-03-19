/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
/********************************************************************
 *
 * NAME:     TrigTauRecMerged.h
 * PACKAGE:  Trigger/TrigAlgorithms/TrigTauRecMerged
 *
 * AUTHOR:   R. Soluk (based on tauBuilder)
 * CREATED:  Nov 21 2005
 * MODIFIED: Dec 14 2006 S.Xella
 *           Mar 02 2001 ccuenca
 *********************************************************************/

#ifndef TRIGTAURECMERGED_H
#define TRIGTAURECMERGED_H

#include <vector>
#include "TrigInterfaces/FexAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "tauRecTools/ITauToolBase.h"
//#include "tauRecTools/TauJetRNNEvaluator.h"

#include "LumiBlockComps/ILuminosityTool.h" 
#include "BeamSpotConditionsData/BeamSpotData.h"


namespace HLT {
  class TriggerElement;
}

class ILumiBlockMuTool;

class TrigTauRecMerged: public HLT::FexAlgo {

 public:

  /**  type definitions for internal tool storage */
  typedef std::string                          tool_key;
  typedef ITauToolBase                         tool_type;
  typedef std::vector<tool_type*>              tool_store;
  typedef tool_store::const_iterator           tool_iterator;
  typedef std::map<tool_key,unsigned int>      tool_stats;
  typedef tool_stats::const_iterator           tool_stats_iterator;
  typedef std::string                          tool_containers;

  /**  constructor */
  TrigTauRecMerged(const std::string& name, ISvcLocator* pSvcLocator);

  /** destructor */
  ~TrigTauRecMerged();

  // Gaudi algorithm hooks
  /** HLT method to initialize */
  HLT::ErrorCode  hltInitialize();

  /** HLT method to finalize */
  HLT::ErrorCode hltFinalize();

  /** HLT method to execute FEX algo on a given TE.
   input is last TE from EF ID, output is TE for EF tauRec hypo execution */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);

 private:

  TauEventData m_tauEventData;

  void setEmptyTauTrack( xAOD::TauJet* &tauJet,
			 xAOD::TauTrackContainer* &tauTrackCont);

  enum TAUEFCALOMON{
    NoROIDescr=0,
    NoCellCont=1,
    EmptyCellCont=2,
    NoClustCont=3,
    NoClustKey=4,
    EmptyClustCont=5,
    NoJetAttach=6,
    NoHLTtauAttach=7,
    NoHLTtauDetAttach=8,
    NoHLTtauXdetAttach=9
  };

  enum TAUEFTRKMON{
    NoTrkCont=0,
    NoVtxCont=1
  };


  /** output container name */
  std::string m_outputName;

  /** internal tool store */
  ToolHandleArray<ITauToolBase>  m_tools;

  /** internal tool store */
  ToolHandleArray<ITauToolBase>  m_endtools;

  /** Luminosity Tool */
  ToolHandle<ILuminosityTool>  m_lumiTool; 
  ToolHandle<ILumiBlockMuTool> m_lumiBlockMuTool;

  /** Beam spot service */
  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
  
  /** only build taus with eta_seed < m_maxeta */
  float m_maxeta;

  /** only build taus with pt_seed > m_minpt */
  float m_minpt;

  /** only build taus with track seed < 0.2 from roi center */
  float m_trkcone;

  /** Store beam type  */
  std::string m_beamType;

  /** vector of Timers */
  std::vector<TrigTimer* > m_mytimers;

  /** Monitoring : nCells obtained */
  int  m_nCells;
  /** Monitoring : nTracks obtained */
  int  m_nTracks;

  /** Monitoring : Eta(tau)-Eta(RoI) reconstructed */
  float  m_dEta;
  /** Monitoring : Phi(tau)-Phi(RoI) reconstructed */
  float  m_dPhi;
  /** Monitoring : emRadius */
  float  m_emRadius;
  /** Monitoring : hadRadius */
  float  m_hadRadius;
  /** EM Et et Em scale*/
  float m_EtEm;
  /** Had Et at EM scale */
  float m_EtHad;
  /** Et at EM scale */
  float m_Et;
   /** Calibrated Et  */
  float m_EtFinal;
  /** EM fraction of tau energy */
  float m_EMFrac;
  /** Isolation fraction */
  float m_IsoFrac;

  /** Monitoring: seedCalo_centFrac **/
  float m_centFrac;
  /** Monitoring: seedCalo_nWideTrk **/
  int m_nWideTrk;
  /** Monitoring: ipSigLeadTrk **/
  float m_ipSigLeadTrk;
  /** Monitoring: trFlightPathSig **/
  float m_trFlightPathSig;
  /** Monitoring: massTrkSys **/
  float m_massTrkSys;
  /** Monitoring: seedCalo_dRmax **/
  float m_dRmax;
 
  /** Number of tracks used for tau ID */
  int m_numTrack;
  
  /** Track average distance */
  float m_trkAvgDist;
  /** Et over lead track Pt */
  float m_etovPtLead;
  /** number of tau candidates */
  int m_Ncand;

  /** Monitoring: actual interaction per bunch crossing **/
  double m_ActualInteractions;
  /** Monitoring: average interaction per bunch crossing **/
  double m_AvgInteractions;

  /** Monitoring: beamspot position */
  float m_beamspot_x,m_beamspot_y,m_beamspot_z;


  /** Monitoring: PSSFractionw **/
  float m_PSSFraction;
  /** Monitoring: EMPOverTrkSysP **/
  float m_EMPOverTrkSysP;
  /** Monitoring: ChPiEMEOverCaloEME **/
  float m_ChPiEMEOverCaloEME;

  /** Monitoring: innerTrkAvgDist **/
  float m_innerTrkAvgDist;

  /** Monitoring: SumPtTrkFrac **/
  float m_SumPtTrkFrac;

  /** Eta of L1 ROI */
  float m_EtaL1;
  /** Phi of L1 ROI */
  float m_PhiL1;

  /** Eta of EF ROI */
  float m_EtaEF;
  /** Phi of EF ROI */
  float m_PhiEF;

  /** calo errors */
  std::vector<unsigned char> m_calo_errors;
  /** track errors */
  std::vector<unsigned char> m_track_errors;

  //  RNN ID monitoring
  // retrieved from tool handle, if tool exists
  // needs migration of RNN tools to master
  //TauJetRNNEvaluator* m_rnn_evaluator;

  float m_RNN_scalar_ptRatioEflowApprox; 
  float m_RNN_scalar_mEflowApprox; 
  float m_RNN_scalar_pt_jetseed_log;

  int m_RNN_Nclusters;
  std::vector<double> m_RNN_cluster_et_log;
  std::vector<double> m_RNN_cluster_dEta;
  std::vector<double> m_RNN_cluster_dPhi;
  std::vector<double> m_RNN_cluster_CENTER_LAMBDA;
  std::vector<double> m_RNN_cluster_SECOND_LAMBDA;
  std::vector<double> m_RNN_cluster_SECOND_R;

  int m_RNN_Ntracks;
  std::vector<double> m_RNN_track_pt_log;
  std::vector<double> m_RNN_track_dEta;
  std::vector<double> m_RNN_track_dPhi;
  std::vector<double> m_RNN_track_d0_abs_log;
  std::vector<double> m_RNN_track_z0sinThetaTJVA_abs_log;
  std::vector<double> m_RNN_track_nInnermostPixelHits;
  std::vector<double> m_RNN_track_nPixelHits;
  std::vector<double> m_RNN_track_nSCTHits;

  float m_RNNJetScore;
  float m_RNNJetScoreSigTrans;

};
#endif
