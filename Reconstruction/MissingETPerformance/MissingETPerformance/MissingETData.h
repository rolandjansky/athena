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
  void setL1ROI( const LVL1_ROI *m ) { m_hlt_met_l1roi = m; }
  void setL2MET( const TrigMissingETContainer *m ) { m_hlt_met_l2 = m; }
  void setEFMET_FEBHeader( const TrigMissingETContainer *m ) { m_hlt_met_ef_FEB = m; }
  void setEFMET_noiseSupp( const TrigMissingETContainer *m ) { m_hlt_met_ef_noiseSupp = m; }
  void setEFMET_default( const TrigMissingETContainer *m ) { m_hlt_met_ef_default = m; }

  void setMETComposition(const MissingETComposition *m) {m_metComposition = m;}

  void setBase(const MissingEtCalo *m) {m_met_base = m;}
  void setRefFinal(const MissingET *m) {m_met_refFinal = m;}
  void setCorrTopo(const MissingEtCalo *m) {m_met_corrTopo = m;}             
  void setCalib(const MissingEtCalo *m) {m_met_calib = m;}                     
  void setCryoCone(const MissingET *m) {m_met_cryoCone = m;}             
  void setCellOut(const MissingET *m) {m_met_cellOut = m;}               
  void setCellOutEFlow(const MissingET *m) {m_met_cellOut_EFlow = m;}               
  void setRefEle(const MissingET *m) {m_met_refEle = m;}                 
  void setRefGamma(const MissingET *m) {m_met_refGamma = m;}             
  void setRefJet(const MissingET *m) {m_met_refJet = m;}                 
  void setSoftJets(const MissingET *m) {m_met_softJets = m;}                 
  void setMuonBoy(const MissingET *m) {m_met_MuonBoy = m;}               
  void setMuID(const MissingET *m) {m_met_MuID = m;}               
  void setRefMuon(const MissingET *m) {m_met_refMuon = m;}               
  void setRefMuon_Track(const MissingET *m) {m_met_refMuon_Track = m;}   
  void setRefTau(const MissingET *m) {m_met_refTau = m;}                 

  void setCryo(const MissingET *m) {m_METCryo = m;}
  void setDM_All(const MissingET *m) {m_METDM_All = m;}
  void setDM_Crack1(const MissingET *m) {m_METDM_Crack1 = m;}
  void setDM_Crack2(const MissingET *m) {m_METDM_Crack2 = m;}
  void setDM_Cryo(const MissingET *m) {m_METDM_Cryo = m;}
  void setFinal(const MissingET *m) {m_METFinal = m;}
  void setLocHadTopoObj(const MissingET *m) {m_METLocHadTopoObj = m;}
  void setTopoObj(const MissingET *m) {m_METTopoObj = m;}
  void setMuonBoy_Spectro(const MissingET *m) {m_METMuonBoy_Spectro = m;}
  void setMuonBoy_Track(const MissingET *m) {m_METMuonBoy_Track = m;}
  void setMuon(const MissingET *m) {m_METMuon = m;}
  void setMuonMuons(const MissingET *m) {m_METMuonMuons = m;}
  void setBase0(const MissingEtCalo *m) {m_METBase0 = m;}
  void setTopo(const MissingEtCalo *m) {m_METTopo = m;}
  void setLocHadTopo(const MissingEtCalo *m) {m_METLocHadTopo = m;}

  void setAllCaloCells(const CaloCellContainer *a) {m_allCalo = a;}
  void setCaloCellsExist(bool b) {m_caloCellsExist = b;}

  void setBCID(const int c) {m_bunch_crossing_id = c;}
  void setMU(const float d) {m_avgIntbXing = d;}

  //getters
  const LVL1_ROI               *getL1ROI() { return m_hlt_met_l1roi; }
  const TrigMissingETContainer *getL2MET() { return m_hlt_met_l2; }
  const TrigMissingETContainer *getEFMET_FEBHeader() { return m_hlt_met_ef_FEB; }
  const TrigMissingETContainer *getEFMET_noiseSupp() { return m_hlt_met_ef_noiseSupp; }
  const TrigMissingETContainer *getEFMET_default() { return m_hlt_met_ef_default; }

  const MissingEtCalo *base() {return m_met_base;}
  const MissingET *refFinal() {return m_met_refFinal;}
  const MissingEtCalo *corrTopo() {return m_met_corrTopo;}
  const MissingEtCalo *calib() {return m_met_calib;}
  const MissingET *cryoCone() {return m_met_cryoCone;}
  const MissingET *cellOut() {return m_met_cellOut;}
  const MissingET *cellOutEFlow() {return m_met_cellOut_EFlow;}
  const MissingET *refEle() {return m_met_refEle;}
  const MissingET *refGamma() {return m_met_refGamma;}
  const MissingET *refJet() {return m_met_refJet;}
  const MissingET *softJets() {return m_met_softJets;}
  const MissingET *MuonBoy() {return m_met_MuonBoy;}
  const MissingET *MuID() {return m_met_MuID;}
  const MissingET *refMuon() {return m_met_refMuon;}
  const MissingET *refMuon_Track() {return m_met_refMuon_Track;}
  const MissingET *refTau() {return m_met_refTau;}
  
  MissingET *truth_int() {return m_met_truth_int;}
  MissingET *truth_nonInt() {return m_met_truth_nonInt;}
  MissingET *truth_muons() {return m_met_truth_muons;}

  const MissingET *cryo() {return m_METCryo;}
  const MissingET *DM_All() {return m_METDM_All;}
  const MissingET *DM_Crack1() {return m_METDM_Crack1;}
  const MissingET *DM_Crack2() {return m_METDM_Crack2;}
  const MissingET *DM_Cryo() {return m_METDM_Cryo;}
  const MissingET *final() {return m_METFinal;}
  const MissingET *locHadTopoObj() {return m_METLocHadTopoObj;}
  const MissingET *topoObj() {return m_METTopoObj;}
  const MissingET *muonBoy_Spectro() {return m_METMuonBoy_Spectro;}
  const MissingET *muonBoy_Track() {return m_METMuonBoy_Track;}
  const MissingET *muon() {return m_METMuon;}
  const MissingET *muonMuons() {return m_METMuonMuons;}

  int BCID() {return m_bunch_crossing_id;}
  float MU() {return m_avgIntbXing;}

  const MissingETComposition *metComposition() {return m_metComposition;}

  // return MissingET* based on SG Key
  const MissingET* getMissingETbyName( const std::string& );

  const MissingEtCalo *base0() {return m_METBase0;}
  const MissingEtCalo *topo() {return m_METTopo;}
  const MissingEtCalo *locHadTopo() {return m_METLocHadTopo;}

  bool ignoreMissingContainers() {return m_ignoreMissingContainers;}

  const CaloCellContainer *allCaloCells() {return (m_caloCellsExist ? m_allCalo : 0);}
  bool caloCellsExist() {return m_caloCellsExist;}

  void fillEtaRings();

  const MissingET *eta_rings_all_samples(int ring) {return &m_eta_rings_all_samples[ring];}
  const MissingET *eta_rings_by_sample(int sample, int ring) {return &m_eta_rings_by_sample[sample][ring];}

  const MissingET *eta_rings_all_samples_nsigma(int ring) {return &m_eta_rings_all_samples_nsigma[ring];}
  const MissingET *eta_rings_by_sample_nsigma(int sample, int ring) {return &m_eta_rings_by_sample_nsigma[sample][ring];}

  double met_from_calo_cells_x() {return m_met_cells_x;}
  double met_from_calo_cells_y() {return m_met_cells_y;}
  double met_from_calo_cells_sigmacut_x() {return m_met_cells_x_nsigma;}
  double met_from_calo_cells_sigmacut_y() {return m_met_cells_y_nsigma;}
  double getNSigma() {return m_n_sigma;}

  double met_from_calo_cells_x_by_layer(int sample) {return m_met_cells_x_by_layer[sample];}
  double met_from_calo_cells_y_by_layer(int sample) {return m_met_cells_y_by_layer[sample];}
  double met_from_calo_cells_sigmacut_x_by_layer(int sample) {return m_met_cells_x_by_layer_nsigma[sample];}
  double met_from_calo_cells_sigmacut_y_by_layer(int sample) {return m_met_cells_y_by_layer_nsigma[sample];}

  //additional derived variables each get a function
  float sumet_MissingET_Base_EMfraction();

  // MB: reference to MET Goodies map for storing derived met goodness quantities
  static MET::Goodies& s_goodies;

 private:
  std::vector<MissingEtTruth::TruthIndex> m_truthIndices;

  //storegate keys
  std::string m_METTruthKey;
  std::string m_METCorrTopoKey;
  std::string m_METCalibKey;
  std::string m_METCryoConeKey;
  std::string m_METCellOutKey;
  std::string m_METCellOutEFlowKey;

  std::string m_HLTL1ROIKey;
  std::string m_HLTMETL2Key;
  std::string m_HLTMETEF_FEBHeaderKey;
  std::string m_HLTMETEF_noiseSuppKey;
  std::string m_HLTMETEF_defaultKey;

  std::string m_METBaseKey;
  std::string m_METRefFinalKey;
  std::string m_METRefEleKey;
  std::string m_METRefGammaKey;
  std::string m_METRefJetKey;
  std::string m_METSoftJetsKey;
  std::string m_METMuonBoyKey;
  std::string m_METMuIDKey;
  std::string m_METRefMuonKey;
  std::string m_METRefMuon_TrackKey;
  std::string m_METRefTauKey;

  std::string m_METCryoKey;            
  std::string m_METDM_AllKey;          
  std::string m_METDM_Crack1Key;       
  std::string m_METDM_Crack2Key;       
  std::string m_METDM_CryoKey;         
  std::string m_METFinalKey;           
  std::string m_METLocHadTopoObjKey;   
  std::string m_METTopoObjKey;         
  std::string m_METMuonBoy_SpectroKey; 
  std::string m_METMuonBoy_TrackKey;   
  std::string m_METMuonKey;            
  std::string m_METMuonMuonsKey;            
  std::string m_METBase0Key;           
  std::string m_METTopoKey;            
  std::string m_METLocHadTopoKey;

  std::string m_metCompositionKey;

  bool m_ignoreMissingContainers;

  std::string m_allCaloCellsKey;
  //whether to get calo cells
  bool m_getCaloCells;

  //Trigger MissingET objects
  const LVL1_ROI                *m_hlt_met_l1roi;
  LVL1_ROI                      *m_hlt_met_l1roi_zero;
  
  const TrigMissingETContainer  *m_hlt_met_l2;
  const TrigMissingETContainer  *m_hlt_met_ef_FEB;
  const TrigMissingETContainer  *m_hlt_met_ef_noiseSupp;
  const TrigMissingETContainer  *m_hlt_met_ef_default;
  TrigMissingETContainer        *m_hlt_met_cont_zero;
  
  
  //MissingET objects
  const MissingEtCalo *m_met_base;
  const MissingET *m_met_refFinal;
  const MissingEtCalo *m_met_corrTopo;
  const MissingEtCalo *m_met_calib;
  const MissingET *m_met_cryoCone;
  const MissingET *m_met_cellOut;
  const MissingET *m_met_cellOut_EFlow;
  const MissingET *m_met_refEle;
  const MissingET *m_met_refGamma;
  const MissingET *m_met_refJet;
  const MissingET *m_met_softJets;
  const MissingET *m_met_MuonBoy;
  const MissingET *m_met_MuID;
  const MissingET *m_met_refMuon;
  const MissingET *m_met_refMuon_Track;
  const MissingET *m_met_refTau;          

  const MissingET *m_METCryo;
  const MissingET *m_METDM_All;
  const MissingET *m_METDM_Crack1;
  const MissingET *m_METDM_Crack2;
  const MissingET *m_METDM_Cryo;
  const MissingET *m_METFinal;
  const MissingET *m_METLocHadTopoObj;
  const MissingET *m_METTopoObj;
  const MissingET *m_METMuonBoy_Spectro;
  const MissingET *m_METMuonBoy_Track;
  const MissingET *m_METMuon;
  const MissingET *m_METMuonMuons;
  const MissingEtCalo *m_METBase0;
  const MissingEtCalo *m_METTopo;
  const MissingEtCalo *m_METLocHadTopo;

  MissingET *m_met_truth_int;
  MissingET *m_met_truth_nonInt;
  MissingET *m_met_truth_muons;

  int m_bunch_crossing_id;

  float m_avgIntbXing;

  MissingEtCalo *m_met_zero;
  MissingETComposition *m_met_composition_zero;

  const CaloCellContainer *m_allCalo;
  bool m_caloCellsExist;

  bool m_useNoiseTool;
  ToolHandle<ICalorimeterNoiseTool> m_noiseTool;

  bool m_useBadChannelTool;
  ToolHandle<ICaloBadChanTool> m_badChannelTool;

  bool m_useBadChannelMasker;
  ToolHandle<ILArBadChannelMasker> m_badChannelMasker;

  double m_met_cells_x;
  double m_met_cells_y;
  double m_met_cells_x_nsigma;
  double m_met_cells_y_nsigma;
  double m_n_sigma;

  double m_met_cells_x_by_layer[24];
  double m_met_cells_y_by_layer[24];
  double m_met_cells_x_by_layer_nsigma[24];
  double m_met_cells_y_by_layer_nsigma[24];

  MissingET m_eta_rings_all_samples[100];
  MissingET m_eta_rings_by_sample[24][100];

  MissingET m_eta_rings_all_samples_nsigma[100];
  MissingET m_eta_rings_by_sample_nsigma[24][100];

  const MissingETComposition *m_metComposition;

};

#endif // MISSINGETDATA_H
