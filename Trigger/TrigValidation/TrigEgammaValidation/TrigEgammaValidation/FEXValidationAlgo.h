/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FEXVALIDATIONALGO_H
#define FEXVALIDATIONALGO_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

#include "TH1F.h"
#include "TProfile2D.h"
#include "TNtuple.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"

#include "TrigCaloEvent/TrigEMClusterContainer.h"
#include "TrigCaloEvent/TrigEMCluster.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"

//#include "egammaEvent/egammaContainer.h"
//#include "egammaEvent/egamma.h"
//#include "egammaEvent/EMTrackMatch.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"

#include "TrigEgammaValidation/IValidationAlgo.h"

#include <string>
#include <map>
#include <vector>

class TrigInDetTrackCollection;

namespace Rec {
  class TrackParticle;
}

class IExtrapolateToCaloTool; //NDEH
class CaloDepthTool;//NDEH

class FEXValidationAlgo : public IValidationAlgo
{

 public:
  FEXValidationAlgo(const std::string & name, ISvcLocator * pSvcLocator, const std::string &myfile);
  ~FEXValidationAlgo();
 
  //StatusCode childInitialize();
  //StatusCode childExecute();
  //StatusCode childFinalize();

 protected:

  StatusCode prepare();
  bool runTrack(const DataHandle<TrigInDetTrackCollection> lvl2t_first, const DataHandle<TrigInDetTrackCollection> lvl2t_last, float l2eta, float l2phi,
                float &Pt, float &Eta, float &Phi, float &A0, float &Z0, TrigInDetTrack::AlgoId m);

  void extrapolateToFirstSurface(const Rec::TrackParticle *m_track, double &caloeta, double &calophi );
  float delta(float a);
  void scaleError(float factor, TH1 *h);

  void FillLVL1(LVL1_ROI::emtaus_type::const_iterator itEMTau);
  void FillLVL2Calo(TrigEMCluster *matchedL2);
  void FillLVL2ID(float Et, float Eta, float Phi, float A0, float Z0, TrigEMCluster *matchedL2, const std::string &ScanSi);
  void FillEFCalo(const CaloCluster *matchedEFCalo, float DetaEFCalo, float DphiEFCalo, const std::string &ScanSi);
  //void FillEFTrack(const egamma *matchedEFTrk, const std::string & ScanSi);
  void FillHisto(const std::string & name);
  void FillEFScanTrackHistos();
  void FillEFSiTrackHistos();

  void FillTable();

  // Extrapolation 
  IExtrapolateToCaloTool* m_toCalo;
  CaloDepthTool* m_calodepth;

  std::string m_rootDir;

  std::map<std::string, std::vector<float> *> vect;

  float m_L2Rcore_cut;
  float m_L2Rstrip_cut;
  float m_L2Et_cut;
  float m_L2Ehad_cut;

  float m_L1EmClus;
  float m_L1EmIsol;
  float m_L1HadCore;
  float m_L1HadIsol;

  float m_L2IDPt;
  float m_L2IDEP;
  float m_L2IDDeta;
  float m_L2IDDphi;

  float m_sL;

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
