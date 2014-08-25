/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETDATA_H
#define MISSINGETDATA_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtTruth.h"
#include "MissingETEvent/MissingEtCalo.h"
#include "MissingETEvent/MissingETComposition.h"

#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigMissingEtEvent/TrigMissingETContainer.h"
#include "TrigMissingEtEvent/TrigMissingEtComponent.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"
#include "CaloConditions/ICaloBadChanTool.h"
#include "LArRecConditions/ILArBadChannelMasker.h"

#include "MissingETGoodness/Goodies.h"

static const InterfaceID IID_MissingETData("MissingETData", 1, 0);

class MissingETData : public AthAlgTool {
public:
  MissingETData(const std::string& type, const std::string& name,
                   const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_MissingETData; };

  MissingETData();
  ~MissingETData();

  /** Overriding initialize and finalize */
  virtual StatusCode initialize();
  /** finalization */
  virtual StatusCode finalize();

  StatusCode retrieveContainers();
  StatusCode retrieveLevel1Containers(); //D3PD container retrieval 
  StatusCode retrieveLevel2Containers(); //D3PD container retrieval 
  StatusCode retrieveLevel3Containers(); //D3PD container retrieval 
 

  //setters
  void setL1ROI( const LVL1_ROI *m ) { _hlt_met_l1roi = m; }
  void setL2MET( const TrigMissingETContainer *m ) { _hlt_met_l2 = m; }
  void setEFMET_FEBHeader( const TrigMissingETContainer *m ) { _hlt_met_ef_FEB = m; }
  void setEFMET_noiseSupp( const TrigMissingETContainer *m ) { _hlt_met_ef_noiseSupp = m; }
  void setEFMET_default( const TrigMissingETContainer *m ) { _hlt_met_ef_default = m; }

  void setMETComposition(const MissingETComposition *m) {_metComposition = m;}

  void setBase(const MissingEtCalo *m) {_met_base = m;}
  void setRefFinal(const MissingET *m) {_met_refFinal = m;}
  void setCorrTopo(const MissingEtCalo *m) {_met_corrTopo = m;}             
  void setCalib(const MissingEtCalo *m) {_met_calib = m;}                     
  void setCryoCone(const MissingET *m) {_met_cryoCone = m;}             
  void setCellOut(const MissingET *m) {_met_cellOut = m;}               
  void setCellOutEFlow(const MissingET *m) {_met_cellOut_EFlow = m;}               
  void setRefEle(const MissingET *m) {_met_refEle = m;}                 
  void setRefGamma(const MissingET *m) {_met_refGamma = m;}             
  void setRefJet(const MissingET *m) {_met_refJet = m;}                 
  void setSoftJets(const MissingET *m) {_met_softJets = m;}                 
  void setMuonBoy(const MissingET *m) {_met_MuonBoy = m;}               
  void setMuID(const MissingET *m) {_met_MuID = m;}               
  void setRefMuon(const MissingET *m) {_met_refMuon = m;}               
  void setRefMuon_Track(const MissingET *m) {_met_refMuon_Track = m;}   
  void setRefTau(const MissingET *m) {_met_refTau = m;}                 

  void setCryo(const MissingET *m) {_METCryo = m;}
  void setDM_All(const MissingET *m) {_METDM_All = m;}
  void setDM_Crack1(const MissingET *m) {_METDM_Crack1 = m;}
  void setDM_Crack2(const MissingET *m) {_METDM_Crack2 = m;}
  void setDM_Cryo(const MissingET *m) {_METDM_Cryo = m;}
  void setFinal(const MissingET *m) {_METFinal = m;}
  void setLocHadTopoObj(const MissingET *m) {_METLocHadTopoObj = m;}
  void setTopoObj(const MissingET *m) {_METTopoObj = m;}
  void setMuonBoy_Spectro(const MissingET *m) {_METMuonBoy_Spectro = m;}
  void setMuonBoy_Track(const MissingET *m) {_METMuonBoy_Track = m;}
  void setMuon(const MissingET *m) {_METMuon = m;}
  void setMuonMuons(const MissingET *m) {_METMuonMuons = m;}
  void setBase0(const MissingEtCalo *m) {_METBase0 = m;}
  void setTopo(const MissingEtCalo *m) {_METTopo = m;}
  void setLocHadTopo(const MissingEtCalo *m) {_METLocHadTopo = m;}

  void setAllCaloCells(const CaloCellContainer *a) {_allCalo = a;}
  void setCaloCellsExist(bool b) {_caloCellsExist = b;}

  void setBCID(const int c) {_bunch_crossing_id = c;}
  void setMU(const float d) {_avgIntbXing = d;}

  //getters
  const LVL1_ROI               *getL1ROI() { return _hlt_met_l1roi; }
  const TrigMissingETContainer *getL2MET() { return _hlt_met_l2; }
  const TrigMissingETContainer *getEFMET_FEBHeader() { return _hlt_met_ef_FEB; }
  const TrigMissingETContainer *getEFMET_noiseSupp() { return _hlt_met_ef_noiseSupp; }
  const TrigMissingETContainer *getEFMET_default() { return _hlt_met_ef_default; }

  const MissingEtCalo *base() {return _met_base;}
  const MissingET *refFinal() {return _met_refFinal;}
  const MissingEtCalo *corrTopo() {return _met_corrTopo;}
  const MissingEtCalo *calib() {return _met_calib;}
  const MissingET *cryoCone() {return _met_cryoCone;}
  const MissingET *cellOut() {return _met_cellOut;}
  const MissingET *cellOutEFlow() {return _met_cellOut_EFlow;}
  const MissingET *refEle() {return _met_refEle;}
  const MissingET *refGamma() {return _met_refGamma;}
  const MissingET *refJet() {return _met_refJet;}
  const MissingET *softJets() {return _met_softJets;}
  const MissingET *MuonBoy() {return _met_MuonBoy;}
  const MissingET *MuID() {return _met_MuID;}
  const MissingET *refMuon() {return _met_refMuon;}
  const MissingET *refMuon_Track() {return _met_refMuon_Track;}
  const MissingET *refTau() {return _met_refTau;}
  
  MissingET *truth_int() {return _met_truth_int;}
  MissingET *truth_nonInt() {return _met_truth_nonInt;}
  MissingET *truth_muons() {return _met_truth_muons;}

  const MissingET *cryo() {return _METCryo;}
  const MissingET *DM_All() {return _METDM_All;}
  const MissingET *DM_Crack1() {return _METDM_Crack1;}
  const MissingET *DM_Crack2() {return _METDM_Crack2;}
  const MissingET *DM_Cryo() {return _METDM_Cryo;}
  const MissingET *final() {return _METFinal;}
  const MissingET *locHadTopoObj() {return _METLocHadTopoObj;}
  const MissingET *topoObj() {return _METTopoObj;}
  const MissingET *muonBoy_Spectro() {return _METMuonBoy_Spectro;}
  const MissingET *muonBoy_Track() {return _METMuonBoy_Track;}
  const MissingET *muon() {return _METMuon;}
  const MissingET *muonMuons() {return _METMuonMuons;}

  int BCID() {return _bunch_crossing_id;}
  float MU() {return _avgIntbXing;}

  const MissingETComposition *metComposition() {return _metComposition;}

  // return MissingET* based on SG Key
  const MissingET* getMissingETbyName( const std::string& );

  const MissingEtCalo *base0() {return _METBase0;}
  const MissingEtCalo *topo() {return _METTopo;}
  const MissingEtCalo *locHadTopo() {return _METLocHadTopo;}

  bool ignoreMissingContainers() {return _ignoreMissingContainers;}

  const CaloCellContainer *allCaloCells() {return (_caloCellsExist ? _allCalo : 0);}
  bool caloCellsExist() {return _caloCellsExist;}

  void fillEtaRings();

  const MissingET *eta_rings_all_samples(int ring) {return &_eta_rings_all_samples[ring];}
  const MissingET *eta_rings_by_sample(int sample, int ring) {return &_eta_rings_by_sample[sample][ring];}

  const MissingET *eta_rings_all_samples_nsigma(int ring) {return &_eta_rings_all_samples_nsigma[ring];}
  const MissingET *eta_rings_by_sample_nsigma(int sample, int ring) {return &_eta_rings_by_sample_nsigma[sample][ring];}

  double met_from_calo_cells_x() {return met_cells_x;}
  double met_from_calo_cells_y() {return met_cells_y;}
  double met_from_calo_cells_sigmacut_x() {return met_cells_x_nsigma;}
  double met_from_calo_cells_sigmacut_y() {return met_cells_y_nsigma;}
  double getNSigma() {return n_sigma;}

  double met_from_calo_cells_x_by_layer(int sample) {return met_cells_x_by_layer[sample];}
  double met_from_calo_cells_y_by_layer(int sample) {return met_cells_y_by_layer[sample];}
  double met_from_calo_cells_sigmacut_x_by_layer(int sample) {return met_cells_x_by_layer_nsigma[sample];}
  double met_from_calo_cells_sigmacut_y_by_layer(int sample) {return met_cells_y_by_layer_nsigma[sample];}

  //additional derived variables each get a function
  float sumet_MissingET_Base_EMfraction();

  // MB: reference to MET Goodies map for storing derived met goodness quantities
  static MET::Goodies& goodies;

 private:
  std::vector<MissingEtTruth::TruthIndex> m_truthIndices;

  //storegate keys
  std::string _METTruthKey;
  std::string _METCorrTopoKey;
  std::string _METCalibKey;
  std::string _METCryoConeKey;
  std::string _METCellOutKey;
  std::string _METCellOutEFlowKey;

  std::string _HLTL1ROIKey;
  std::string _HLTMETL2Key;
  std::string _HLTMETEF_FEBHeaderKey;
  std::string _HLTMETEF_noiseSuppKey;
  std::string _HLTMETEF_defaultKey;

  std::string _METBaseKey;
  std::string _METRefFinalKey;
  std::string _METRefEleKey;
  std::string _METRefGammaKey;
  std::string _METRefJetKey;
  std::string _METSoftJetsKey;
  std::string _METMuonBoyKey;
  std::string _METMuIDKey;
  std::string _METRefMuonKey;
  std::string _METRefMuon_TrackKey;
  std::string _METRefTauKey;

  std::string _METCryoKey;            
  std::string _METDM_AllKey;          
  std::string _METDM_Crack1Key;       
  std::string _METDM_Crack2Key;       
  std::string _METDM_CryoKey;         
  std::string _METFinalKey;           
  std::string _METLocHadTopoObjKey;   
  std::string _METTopoObjKey;         
  std::string _METMuonBoy_SpectroKey; 
  std::string _METMuonBoy_TrackKey;   
  std::string _METMuonKey;            
  std::string _METMuonMuonsKey;            
  std::string _METBase0Key;           
  std::string _METTopoKey;            
  std::string _METLocHadTopoKey;      

  std::string _metCompositionKey;

  bool _ignoreMissingContainers;

  std::string _allCaloCellsKey;
  //whether to get calo cells
  bool _getCaloCells;

  //Trigger MissingET objects
  const LVL1_ROI                *_hlt_met_l1roi;
  LVL1_ROI                      *_hlt_met_l1roi_zero;
  
  const TrigMissingETContainer  *_hlt_met_l2;
  const TrigMissingETContainer  *_hlt_met_ef_FEB;
  const TrigMissingETContainer  *_hlt_met_ef_noiseSupp;
  const TrigMissingETContainer  *_hlt_met_ef_default;
  TrigMissingETContainer        *_hlt_met_cont_zero;
  
  
  //MissingET objects
  const MissingEtCalo *_met_base;
  const MissingET *_met_refFinal;
  const MissingEtCalo *_met_corrTopo;
  const MissingEtCalo *_met_calib;
  const MissingET *_met_cryoCone;
  const MissingET *_met_cellOut;
  const MissingET *_met_cellOut_EFlow;
  const MissingET *_met_refEle;
  const MissingET *_met_refGamma;
  const MissingET *_met_refJet;
  const MissingET *_met_softJets;
  const MissingET *_met_MuonBoy;
  const MissingET *_met_MuID;
  const MissingET *_met_refMuon;
  const MissingET *_met_refMuon_Track;
  const MissingET *_met_refTau;          

  const MissingET *_METCryo;
  const MissingET *_METDM_All;
  const MissingET *_METDM_Crack1;
  const MissingET *_METDM_Crack2;
  const MissingET *_METDM_Cryo;
  const MissingET *_METFinal;
  const MissingET *_METLocHadTopoObj;
  const MissingET *_METTopoObj;
  const MissingET *_METMuonBoy_Spectro;
  const MissingET *_METMuonBoy_Track;
  const MissingET *_METMuon;
  const MissingET *_METMuonMuons;
  const MissingEtCalo *_METBase0;
  const MissingEtCalo *_METTopo;
  const MissingEtCalo *_METLocHadTopo;

  MissingET *_met_truth_int;
  MissingET *_met_truth_nonInt;
  MissingET *_met_truth_muons;

  int _bunch_crossing_id;

  float _avgIntbXing;

  MissingEtCalo *_met_zero;
  MissingETComposition *_met_composition_zero;

  const CaloCellContainer *_allCalo;
  bool _caloCellsExist;

  bool m_useNoiseTool;
  ToolHandle<ICalorimeterNoiseTool> m_noiseTool;

  bool m_useBadChannelTool;
  ToolHandle<ICaloBadChanTool> m_badChannelTool;

  bool m_useBadChannelMasker;
  ToolHandle<ILArBadChannelMasker> m_badChannelMasker;

  double met_cells_x;
  double met_cells_y;
  double met_cells_x_nsigma;
  double met_cells_y_nsigma;
  double n_sigma;

  double met_cells_x_by_layer[24];
  double met_cells_y_by_layer[24];
  double met_cells_x_by_layer_nsigma[24];
  double met_cells_y_by_layer_nsigma[24];

  MissingET _eta_rings_all_samples[100];
  MissingET _eta_rings_by_sample[24][100];

  MissingET _eta_rings_all_samples_nsigma[100];
  MissingET _eta_rings_by_sample_nsigma[24][100];

  const MissingETComposition *_metComposition;

};

#endif // MISSINGETDATA_H
