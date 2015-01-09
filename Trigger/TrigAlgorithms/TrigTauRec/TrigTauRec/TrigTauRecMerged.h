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
#include "tauRec/TauToolBase.h"

#include "LumiBlockComps/ILuminosityTool.h" 
#include "InDetBeamSpotService/IBeamCondSvc.h"

namespace HLT {
  class TriggerElement;
}

class ILumiBlockMuTool;

class TrigTauRecMerged: public HLT::FexAlgo {

 public:

  /**  type definitions for internal tool storage */
  typedef std::string                          tool_key;
  typedef TauToolBase                          tool_type;
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
  ToolHandleArray<TauToolBase>  m_tools;

  /** internal tool store */
  ToolHandleArray<TauToolBase>  m_endtools;

  /** Luminosity Tool */
  ToolHandle<ILuminosityTool>  m_lumiTool; 
  ToolHandle<ILumiBlockMuTool> m_lumiBlockMuTool;

  /** Beam spot service */
  ServiceHandle<IBeamCondSvc>  m_beamSpotSvc;
  
  /** only build taus with eta_seed < m_maxeta */
  float m_maxeta;

  /** only build taus with pt_seed > m_minpt */
  float m_minpt;

  /** only build taus with track seed < 0.2 from roi center */
  float m_trkcone;

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

  /** Monitoring: PSSFractionw **/
  float m_PSSFraction;
  /** Monitoring: EMPOverTrkSysP **/
  float m_EMPOverTrkSysP;
  /** Monitoring: ChPiEMEOverCaloEME **/
  float m_ChPiEMEOverCaloEME;

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
  /** author */
  std::vector<unsigned char> m_author;

  /** deltaZ0 core Trks*/
  std::vector<float> m_deltaZ0coreTrks;

  /** deltaZ0 wide Trks*/
  std::vector<float> m_deltaZ0wideTrks;

};
#endif
