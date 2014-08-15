/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTEGAMMAFEXBASETOOL_H
#define HLTEGAMMAFEXBASETOOL_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "xAODTrigger/EmTauRoI.h"

#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEvent/TrigEMClusterContainer.h"

#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"

#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/EMTrackMatch.h"

// Simple description to come
#include "TH1F.h"

#include "TrigHLTMonitoring/IHLTMonTool.h"

#include "TrigInDetEvent/TrigInDetTrack.h"

class TrigInDetTrackCollection;

namespace Rec {
  class TrackParticle;
}

class IExtrapolateToCaloTool;
class CaloDepthTool;

class HLTEgammaFEXBaseTool : public IHLTMonTool {

  public:
  
  // Constructor
  HLTEgammaFEXBaseTool(const std::string & type, const std::string & name, const IInterface* parent);
  virtual ~HLTEgammaFEXBaseTool();

  // To be over-written
/*
  StatusCode init();
  StatusCode book(bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun); // called by bookHistograms
  StatusCode fill(); // called by fillHistograms
  StatusCode proc(bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun); // called by procHistograms
*/

  protected:
  
  virtual void FillLVL1(const EmTau_ROI &itEMTau, const std::string &grp);
  virtual void FillLVL1(const xAOD::EmTauRoI &itEMTau, const std::string &grp);
  virtual void FillLVL2Calo(const TrigEMCluster *matchedL2, const std::string &grp);
  virtual void FillLVL2Calo(const xAOD::TrigEMCluster *matchedL2, const std::string &grp);
  virtual void FillLVL2ID(float &Pt, float &Eta, float &Phi, float &A0, float &Z0, const TrigEMCluster *matchedL2, const std::string &grp);
  virtual void FillLVL2ID(float &Pt, float &Eta, float &Phi, float &A0, float &Z0, const xAOD::TrigEMCluster *matchedL2, const std::string &grp);
  virtual void FillEFCalo(const CaloCluster *matchedEFCalo, float DetaEFCalo, float DphiEFCalo, const std::string &grp);
  virtual void FillEFCalo(const xAOD::CaloCluster *matchedEFCalo, float DetaEFCalo, float DphiEFCalo, const std::string &grp);
  virtual void FillEFTrack(const egamma *matchedEFTrk, std::map<std::string, std::vector<float> *> &vect);
  virtual void FillEFTrack(const xAOD::Electron *matchedEFTrk, std::map<std::string, std::vector<float> *> &vect);

  virtual bool runTrack(const DataHandle<TrigInDetTrackCollection> lvl2t_first, const DataHandle<TrigInDetTrackCollection> lvl2t_last, float l2eta, float l2phi, \
                        float &Pt, float &Eta, float &Phi, float &A0, float &Z0, TrigInDetTrack::AlgoId m);
  virtual void extrapolateToFirstSurface(const Rec::TrackParticle* m_track, double &caloeta, double &calophi);
  virtual void PrintTable(bool withcuts=true);
  virtual float delta(float a);
  virtual void FillEFIDHistos(std::map<std::string, std::vector<float> *> &vect, const std::string &grp);
  virtual void FillHisto(const std::string & name, std::map<std::string, std::vector<float> *> &vect, const std::string &grp);
  virtual void scaleError(float factor, TH1 *h);
  virtual void divide(TH1 *num, TH1 *den, TH1 *quo);

  IToolSvc* m_toolSvc;
  IExtrapolateToCaloTool* m_toCalo;
  CaloDepthTool* m_calodepth;
  bool m_doExtrapol;

  std::string m_EMClusterKey;

  float m_L2Rcore_cut;
  float m_L2Rstrip_cut;
  float m_L2Et_cut;
  float m_L2Ehad_cut;

  float m_L1EmClus;
  float m_L1EmIsol;
  float m_L1HadCore;
  float m_L1HadIsol;

  bool m_passThroughL1;

  float m_L2IDPt;
  float m_L2IDEP;
  float m_L2IDDeta;
  float m_L2IDDphi;

  float m_L;
  float m_nTotal;
  float m_nL1;
  float m_nL2;
  float m_nL2IDScan;
  float m_nL2IDSi;
  float m_nEFScanCalo;
  float m_nEFSiCalo;

  float m_nEFScanTrack;
  float m_nEFSiTrack;

  float m_EFCaloEt;
  float m_EFCaloDeta;
  float m_EFCaloDphi;

  float m_EFTrackeCalibFactor;
  float m_EFTrackDeta;
  float m_EFTrackDphi;
  float m_EFTrackEtaCut;
  float m_EFTrackLowEoPHighEta;
  float m_EFTrackHighEoPHighEta;
  float m_EFTrackLowEoPLowEta;
  float m_EFTrackHighEoPLowEta;

  float m_lowEtCut;
  float m_maxEtaCut;

};

#endif

