/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETDATA_H
#define MISSINGETDATA_H

#include "D3PD.h"

class MissingET {
 public:
  MissingET() {_isValid = false;}
  ~MissingET() {}

  Float_t etx()   {return (_isValid ? *_etx   : 0);}
  Float_t ety()   {return (_isValid ? *_ety   : 0);}
  Float_t sumet() {return (_isValid ? *_sumet : 0);}
  Float_t et()    {return (_isValid ? *_et    : 0);}
  Float_t phi()   {return (_isValid ? *_phi   : 0);}

  Float_t etx_eta45()   {return (_isValid ? (etx_CentralReg()+etx_EndcapReg()+etx_ForwardReg())       : 0);}
  Float_t ety_eta45()   {return (_isValid ? (ety_CentralReg()+ety_EndcapReg()+ety_ForwardReg())       : 0);}
  Float_t sumet_eta45() {return (_isValid ? (sumet_CentralReg()+sumet_EndcapReg()+sumet_ForwardReg()) : 0);}
  Float_t et_eta45()    {return (_isValid ? sqrt(pow(etx_eta45(),2)+pow(ety_eta45(),2))                  : 0);}
  Float_t phi_eta45()   {return (_isValid ? atan2(ety_eta45(),etx_eta45())                               : 0);}

  Float_t etx_CentralReg()     {return (_isValid ? *_etx_CentralReg     : 0);}
  Float_t ety_CentralReg()     {return (_isValid ? *_ety_CentralReg     : 0);}
  Float_t sumet_CentralReg()   {return (_isValid ? *_sumet_CentralReg   : 0);}
  Float_t et_CentralReg()      {return (_isValid ? sqrt(pow(etx_CentralReg(),2)+pow(ety_CentralReg(),2))                  : 0);}
  Float_t phi_CentralReg()     {return (_isValid ? *_phi_CentralReg     : 0);}

  Float_t etx_EndcapReg()      {return (_isValid ? *_etx_EndcapRegion   : 0);}
  Float_t ety_EndcapReg()      {return (_isValid ? *_ety_EndcapRegion   : 0);}
  Float_t sumet_EndcapReg()    {return (_isValid ? *_sumet_EndcapRegion : 0);}
  Float_t et_EndcapReg()      {return (_isValid ? sqrt(pow(etx_EndcapReg(),2)+pow(ety_EndcapReg(),2))                  : 0);}
  Float_t phi_EndcapReg()      {return (_isValid ? *_phi_EndcapRegion   : 0);}

  Float_t etx_ForwardReg()     {return (_isValid ? *_etx_ForwardReg     : 0);}
  Float_t ety_ForwardReg()     {return (_isValid ? *_ety_ForwardReg     : 0);}
  Float_t sumet_ForwardReg()   {return (_isValid ? *_sumet_ForwardReg   : 0);}
  Float_t et_ForwardReg()      {return (_isValid ? sqrt(pow(etx_ForwardReg(),2)+pow(ety_ForwardReg(),2))                  : 0);}
  Float_t phi_ForwardReg()     {return (_isValid ? *_phi_ForwardReg     : 0);}

  Float_t nCell()              {return (_isValid ? (*_nCell_PEMB+*_nCell_EMB+*_nCell_FCAL+
						    *_nCell_PEME+*_nCell_EME+*_nCell_TILE+*_nCell_HEC)         : 0);}

  Float_t etx_FCAL()           {return (_isValid ? *_etx_FCAL           : 0);}
  Float_t ety_FCAL()           {return (_isValid ? *_ety_FCAL           : 0);}
  Float_t sumet_FCAL()         {return (_isValid ? *_sumet_FCAL         : 0);}
  Float_t phi_FCAL()           {return (_isValid ? *_phi_FCAL           : 0);}
  Float_t nCell_FCAL()         {return (_isValid ? *_nCell_FCAL         : 0);}

  Float_t etx_PEMB()           {return (_isValid ? *_etx_PEMB           : 0);}
  Float_t ety_PEMB()           {return (_isValid ? *_ety_PEMB	        : 0);}
  Float_t sumet_PEMB()         {return (_isValid ? *_sumet_PEMB	        : 0);}
  Float_t phi_PEMB()           {return (_isValid ? *_phi_PEMB	        : 0);}
  Float_t nCell_PEMB()         {return (_isValid ? *_nCell_PEMB         : 0);}

  Float_t etx_EMB()            {return (_isValid ? *_etx_EMB	        : 0);}
  Float_t ety_EMB()            {return (_isValid ? *_ety_EMB	        : 0);}
  Float_t sumet_EMB()          {return (_isValid ? *_sumet_EMB	        : 0);}
  Float_t phi_EMB()            {return (_isValid ? *_phi_EMB	        : 0);}
  Float_t nCell_EMB()          {return (_isValid ? *_nCell_EMB          : 0);}

  Float_t etx_EME()            {return (_isValid ? *_etx_EME	        : 0);}
  Float_t ety_EME()            {return (_isValid ? *_ety_EME	        : 0);}
  Float_t sumet_EME()          {return (_isValid ? *_sumet_EME          : 0);}
  Float_t phi_EME()            {return (_isValid ? *_phi_EME	        : 0);}
  Float_t nCell_EME()          {return (_isValid ? *_nCell_EME          : 0);}

  Float_t etx_PEMEC()          {return (_isValid ? *_etx_PEME	        : 0);}
  Float_t ety_PEMEC()          {return (_isValid ? *_ety_PEME	        : 0);}
  Float_t sumet_PEMEC()        {return (_isValid ? *_sumet_PEME	        : 0);}
  Float_t phi_PEMEC()          {return (_isValid ? *_phi_PEME	        : 0);}
  Float_t nCell_PEMEC()        {return (_isValid ? *_nCell_PEME         : 0);}

  Float_t etx_TILE()           {return (_isValid ? *_etx_TILE	        : 0);}
  Float_t ety_TILE()           {return (_isValid ? *_ety_TILE	        : 0);}
  Float_t sumet_TILE()         {return (_isValid ? *_sumet_TILE	        : 0);}
  Float_t phi_TILE()           {return (_isValid ? *_phi_TILE	        : 0);}
  Float_t nCell_TILE()         {return (_isValid ? *_nCell_TILE         : 0);}
  
  Float_t etx_HEC()            {return (_isValid ? *_etx_HEC            : 0);}
  Float_t ety_HEC()            {return (_isValid ? *_ety_HEC	        : 0);}
  Float_t sumet_HEC()          {return (_isValid ? *_sumet_HEC	        : 0);}
  Float_t phi_HEC()            {return (_isValid ? *_phi_HEC	        : 0);}
  Float_t nCell_HEC()          {return (_isValid ? *_nCell_HEC          : 0);}

  bool isValid() {return _isValid;}

 private:
  Float_t *_etx;
  Float_t *_ety;
  Float_t *_sumet;
  Float_t *_et;
  Float_t *_phi;
  bool _isValid;

  Float_t *_etx_CentralReg;
  Float_t *_ety_CentralReg;
  Float_t *_sumet_CentralReg;
  Float_t *_phi_CentralReg;
  Float_t *_etx_EndcapRegion;
  Float_t *_ety_EndcapRegion;
  Float_t *_sumet_EndcapRegion;
  Float_t *_phi_EndcapRegion;
  Float_t *_etx_ForwardReg;
  Float_t *_ety_ForwardReg;
  Float_t *_sumet_ForwardReg;
  Float_t *_phi_ForwardReg;

  Float_t *_etx_FCAL;
  Float_t *_ety_FCAL;
  Float_t *_sumet_FCAL;
  Float_t *_phi_FCAL;
  Float_t *_nCell_FCAL;

  Float_t *_etx_PEMB ;
  Float_t *_ety_PEMB;
  Float_t *_sumet_PEMB;
  Float_t *_phi_PEMB;
  Float_t *_nCell_PEMB;

  Float_t *_etx_EMB;
  Float_t *_ety_EMB;
  Float_t *_sumet_EMB;
  Float_t *_phi_EMB;
  Float_t *_nCell_EMB;

  Float_t *_etx_EME;
  Float_t *_ety_EME;
  Float_t *_sumet_EME;
  Float_t *_phi_EME;
  Float_t *_nCell_EME;

  Float_t *_etx_PEME;
  Float_t *_ety_PEME;
  Float_t *_sumet_PEME;
  Float_t *_phi_PEME;
  Float_t *_nCell_PEME;

  Float_t *_etx_TILE;
  Float_t *_ety_TILE;
  Float_t *_sumet_TILE;
  Float_t *_phi_TILE;
  Float_t *_nCell_TILE;
  
  Float_t *_etx_HEC;
  Float_t *_ety_HEC;
  Float_t *_sumet_HEC;
  Float_t *_phi_HEC;
  Float_t *_nCell_HEC;

  friend class MissingETData;
};

class MissingETData {
public:
  MissingETData();
  ~MissingETData();
  MissingETData(D3PD *met_from_data);
  
  // RefFinal
  MissingET *refFinal() {return &_met_refFinal;}
  MissingET *refEle() {return &_met_refEle;}
  MissingET *refGamma() {return &_met_refGamma;}
  MissingET *refJet() {return &_met_refJet;}
  MissingET *softJets() {return &_met_softJets;}
  MissingET *refTau() {return &_met_refTau;}
  MissingET *MuonBoy() {return &_met_MuonBoy;}
  MissingET *refMuon() {return &_met_refMuon;}  
  MissingET *cellOut() {return &_met_cellOut;}
  MissingET *cellOut_Eflow() {return &_met_cellOut_Eflow;}
  MissingET *cryo() {return &_METCryo;}
  
  // RefFinal_em
  MissingET *refFinal_em() {return &_met_refFinal_em;}
  MissingET *refEle_em() {return &_met_refEle_em;}
  MissingET *refGamma_em() {return &_met_refGamma_em;}
  MissingET *refJet_em() {return &_met_refJet_em;}
  MissingET *refTau_em() {return &_met_refTau_em;}
  MissingET *refMuon_em() {return &_met_refMuon_em;}  
  MissingET *cellOut_em() {return &_met_cellOut_em;}
  
  MissingET *truth_int() {return &_met_truth_int;}
  MissingET *truth_nonInt() {return &_met_truth_nonInt;}
  MissingET *truth_muons() {return &_met_truth_muons;}

  MissingET *DM_All() {return &_METDM_All;}
  MissingET *DM_Crack1() {return &_METDM_Crack1;}
  MissingET *DM_Crack2() {return &_METDM_Crack2;}
  MissingET *DM_Cryo() {return &_METDM_Cryo;}
  MissingET *final() {return &_METFinal;}
  MissingET *topoObj() {return &_METTopoObj;}
  MissingET *muonBoy_Spectro() {return &_METMuonBoy_Spectro;}
  MissingET *muonBoy_Track() {return &_METMuonBoy_Track;}
  MissingET *muon() {return &_METMuon;}
  MissingET *corrTopo() {return &_met_corrTopo;}
  MissingET *calib() {return &_met_calib;}
  MissingET *cryoCone() {return &_met_cryoCone;}

  MissingET *base() {return &_met_base;}
  MissingET *base0() {return &_METBase0;}
  MissingET *topo() {return &_METTopo;}
  MissingET *locHadTopo() {return &_METLocHadTopo;}

  //Pisa
  MissingET *refFinal_GCW_pt20() {return &_met_refFinal_GCW_pt20;}
  MissingET *refEle_GCW_pt20() {return &_met_refEle_GCW_pt20;}
  MissingET *refGamma_GCW_pt20() {return &_met_refGamma_GCW_pt20;}
  MissingET *refJet_GCW_pt20() {return &_met_refJet_GCW_pt20;}
  MissingET *softJets_GCW_pt20() {return &_met_softJets_GCW_pt20;}
  MissingET *refTau_GCW_pt20() {return &_met_refTau_GCW_pt20;}
  MissingET *Muon_Total_GCW_pt20() {return &_met_Muon_Total_Staco_GCW_pt20;}
  MissingET *Muon_Isol_GCW_pt20() {return &_met_Muon_Isol_Staco_GCW_pt20;}
  MissingET *Muon_NonIsol_GCW_pt20() {return &_met_Muon_NonIsol_Staco_GCW_pt20;}
  MissingET *refMuon_GCW_pt20() {return &_met_RefMuon_Staco_GCW_pt20;}
  MissingET *refMuon_Track_GCW_pt20() {return &_met_RefMuon_Track_Staco_GCW_pt20;}
  MissingET *cellOut_GCW_pt20() {return &_met_cellOut_GCW_pt20;}
  MissingET *cryo_GCW_pt20() {return &_METCryo_GCW_pt20;}

  MissingET *refFinal_LCW_pt20() {return &_met_refFinal_LCW_pt20;}
  MissingET *refEle_LCW_pt20() {return &_met_refEle_LCW_pt20;}
  MissingET *refGamma_LCW_pt20() {return &_met_refGamma_LCW_pt20;}
  MissingET *refJet_LCW_pt20() {return &_met_refJet_LCW_pt20;}
  MissingET *softJets_LCW_pt20() {return &_met_softJets_LCW_pt20;}
  MissingET *refTau_LCW_pt20() {return &_met_refTau_LCW_pt20;}
  MissingET *Muon_Total_LCW_pt20() {return &_met_Muon_Total_Muid_LCW_pt20;}
  MissingET *Muon_Isol_LCW_pt20() {return &_met_Muon_Isol_Muid_LCW_pt20;}
  MissingET *Muon_NonIsol_LCW_pt20() {return &_met_Muon_NonIsol_Muid_LCW_pt20;}
  MissingET *refMuon_LCW_pt20() {return &_met_RefMuon_Muid_LCW_pt20;}
  MissingET *refMuon_Track_LCW_pt20() {return &_met_RefMuon_Track_Muid_LCW_pt20;}
  MissingET *cellOut_LCW_pt20() {return &_met_cellOut_LCW_pt20;}

  MissingET *refFinal_LCW_NI_eflow4() {return &_met_refFinal_LCW_NI_eflow4;}
  MissingET *refEle_LCW_NI_eflow4() {return &_met_refEle_LCW_NI_eflow4;}
  MissingET *refGamma_LCW_NI_eflow4() {return &_met_refGamma_LCW_NI_eflow4;}
  MissingET *refJet_LCW_NI_eflow4() {return &_met_refJet_LCW_NI_eflow4;}
  MissingET *softJets_LCW_NI_eflow4() {return &_met_softJets_LCW_NI_eflow4;}
  MissingET *refTau_LCW_NI_eflow4() {return &_met_refTau_LCW_NI_eflow4;}
  MissingET *Muon_Total_LCW_NI_eflow4() {return &_met_Muon_Total_Staco_LCW_NI_eflow4;}
  MissingET *Muon_Isol_LCW_NI_eflow4() {return &_met_Muon_Isol_Staco_LCW_NI_eflow4;}
  MissingET *Muon_NonIsol_LCW_NI_eflow4() {return &_met_Muon_NonIsol_Staco_LCW_NI_eflow4;}
  MissingET *refMuon_LCW_NI_eflow4() {return &_met_RefMuon_Staco_LCW_NI_eflow4;}
  MissingET *refMuon_Track_LCW_NI_eflow4() {return &_met_RefMuon_Track_Staco_LCW_NI_eflow4;}
  MissingET *cellOut_LCW_NI_eflow4() {return &_met_cellOut_LCW_NI_eflow4;}
  MissingET *cryo_LCW_NI_eflow4() {return &_METCryo_LCW_NI_eflow4;}
  
  MissingET *refFinal_LCW_NI_eflow_400() {return &_met_refFinal_LCW_NI_eflow_400;}
  MissingET *refEle_LCW_NI_eflow_400() {return &_met_refEle_LCW_NI_eflow_400;}
  MissingET *refGamma_LCW_NI_eflow_400() {return &_met_refGamma_LCW_NI_eflow_400;}
  MissingET *refJet_LCW_NI_eflow_400() {return &_met_refJet_LCW_NI_eflow_400;}
  MissingET *softJets_LCW_NI_eflow_400() {return &_met_softJets_LCW_NI_eflow_400;}
  MissingET *refTau_LCW_NI_eflow_400() {return &_met_refTau_LCW_NI_eflow_400;}
  MissingET *Muon_Total_LCW_NI_eflow_400() {return &_met_Muon_Total_Staco_LCW_NI_eflow_400;}
  MissingET *Muon_Isol_LCW_NI_eflow_400() {return &_met_Muon_Isol_Staco_LCW_NI_eflow_400;}
  MissingET *Muon_NonIsol_LCW_NI_eflow_400() {return &_met_Muon_NonIsol_Staco_LCW_NI_eflow_400;}
  MissingET *refMuon_LCW_NI_eflow_400() {return &_met_RefMuon_Staco_LCW_NI_eflow_400;}
  MissingET *refMuon_Track_LCW_NI_eflow_400() {return &_met_RefMuon_Track_Staco_LCW_NI_eflow_400;}
  MissingET *cellOut_LCW_NI_eflow_400() {return &_met_cellOut_LCW_NI_eflow_400;}
  MissingET *cryo_LCW_NI_eflow_400() {return &_METCryo_LCW_NI_eflow_400;}
  
  MissingET *refFinal_LCW_NI_eflow_CB() {return &_met_refFinal_LCW_NI_eflow_CB;}
  MissingET *refEle_LCW_NI_eflow_CB() {return &_met_refEle_LCW_NI_eflow_CB;}
  MissingET *refGamma_LCW_NI_eflow_CB() {return &_met_refGamma_LCW_NI_eflow_CB;}
  MissingET *refJet_LCW_NI_eflow_CB() {return &_met_refJet_LCW_NI_eflow_CB;}
  MissingET *softJets_LCW_NI_eflow_CB() {return &_met_softJets_LCW_NI_eflow_CB;}
  MissingET *refTau_LCW_NI_eflow_CB() {return &_met_refTau_LCW_NI_eflow_CB;}
  MissingET *Muon_Total_LCW_NI_eflow_CB() {return &_met_Muon_Total_Staco_LCW_NI_eflow_CB;}
  MissingET *Muon_Isol_LCW_NI_eflow_CB() {return &_met_Muon_Isol_Staco_LCW_NI_eflow_CB;}
  MissingET *Muon_NonIsol_LCW_NI_eflow_CB() {return &_met_Muon_NonIsol_Staco_LCW_NI_eflow_CB;}
  MissingET *refMuon_LCW_NI_eflow_CB() {return &_met_RefMuon_Staco_LCW_NI_eflow_CB;}
  MissingET *refMuon_Track_LCW_NI_eflow_CB() {return &_met_RefMuon_Track_Staco_LCW_NI_eflow_CB;}
  MissingET *cellOut_LCW_NI_eflow_CB() {return &_met_cellOut_LCW_NI_eflow_CB;}
  MissingET *cryo_LCW_NI_eflow_CB() {return &_METCryo_LCW_NI_eflow_CB;}
  
  float HT_emscale();
  float HT_emscale_frac() {return HT_emscale() / (topo()->sumet()+MuonBoy()->sumet());}
//float HTx_emscale();
//float HTy_emscale();
  float HT_H1scale();
  float HT_H1scale_frac() {return HT_H1scale() / (corrTopo()->sumet()+MuonBoy()->sumet());}

/*
  unsigned int njets() {return d3pd->jet_AntiKt4LCTopo_n;}
  float LeadingJetPt() {return (d3pd->jet_AntiKt4LCTopo_n!=0 ? d3pd->jet_AntiKt4LCTopo_emscale_pt->at(0) : -999.);}
  float LeadingJetEta() {return (d3pd->jet_AntiKt4LCTopo_n!=0 ? d3pd->jet_AntiKt4LCTopo_eta->at(0) : -999.);}
  float JetPt1() {return (d3pd->jet_AntiKt4LCTopo_n>1 ? d3pd->jet_AntiKt4LCTopo_emscale_pt->at(1) : -999.);}
  float JetPt2() {return (d3pd->jet_AntiKt4LCTopo_n>1 ? d3pd->jet_AntiKt4LCTopo_emscale_pt->at(2) : -999.);}
  float JetPt3() {return (d3pd->jet_AntiKt4LCTopo_n>1 ? d3pd->jet_AntiKt4LCTopo_emscale_pt->at(3) : -999.);}
  float JetPt4() {return (d3pd->jet_AntiKt4LCTopo_n>1 ? d3pd->jet_AntiKt4LCTopo_emscale_pt->at(4) : -999.);}
  float JetPt5() {return (d3pd->jet_AntiKt4LCTopo_n>1 ? d3pd->jet_AntiKt4LCTopo_emscale_pt->at(5) : -999.);}
  float JetPt6() {return (d3pd->jet_AntiKt4LCTopo_n>1 ? d3pd->jet_AntiKt4LCTopo_emscale_pt->at(6) : -999.);}
  float JetPt7() {return (d3pd->jet_AntiKt4LCTopo_n>1 ? d3pd->jet_AntiKt4LCTopo_emscale_pt->at(7) : -999.);}
  float JetPt8() {return (d3pd->jet_AntiKt4LCTopo_n>1 ? d3pd->jet_AntiKt4LCTopo_emscale_pt->at(8) : -999.);}
  float JetPt9() {return (d3pd->jet_AntiKt4LCTopo_n>1 ? d3pd->jet_AntiKt4LCTopo_emscale_pt->at(9) : -999.);}

  float JetEta1() {return (d3pd->jet_AntiKt4LCTopo_n>1 ? d3pd->jet_AntiKt4LCTopo_eta->at(1) : -999.);}
*/

  ////unsigned int njets() {return d3pd->jet_AntiKt4LCTopo_n;}
  Int_t njets() {return d3pd->jet_AntiKt4LCTopo_n;}
  float JetPt(int i) {return (d3pd->jet_AntiKt4LCTopo_n>i ? d3pd->jet_AntiKt4LCTopo_emscale_pt->at(i): 0.);}
  float JetPhi(int i) {return (d3pd->jet_AntiKt4LCTopo_n>i ? d3pd->jet_AntiKt4LCTopo_emscale_phi->at(i): 0.);}
  float JetEta(int i) {return (d3pd->jet_AntiKt4LCTopo_n>i ? d3pd->jet_AntiKt4LCTopo_emscale_eta->at(i): 0.);}

  float v_type(int i) {return d3pd->vxp_type->at(i);}
  float v_nTracks(int i) {return d3pd->vxp_nTracks->at(i);}

/*
  float JetPtX() {
    float pjet_x = 0.;
    for (unsigned int i=0;i<njets();i++ ){
      pjet_x += d3pd->jet_AntiKt4LCTopo_pt->at(i)*cos(d3pd->jet_AntiKt4LCTopo_phi->at(i));
    }
    return pjet_x;
  }
  float JetPtY() {
    float pjet_y = 0.;
    for (unsigned int i=0;i<njets();i++ ){
      pjet_y += d3pd->jet_AntiKt4LCTopo_pt->at(i)*sin(d3pd->jet_AntiKt4LCTopo_phi->at(i));
    }
    return pjet_y;
  }
  
  */
/* float HTx_emscale() {

  float hx_emscale = 0.;

  for (Int_t i = 0; i < d3pd->jet_AntiKt6H1Topo_n; i++) {
    hx_emscale += d3pd->jet_AntiKt6H1Topo_emscale_pt->at(i);
   hx_emscale = hx_emscale *(cos(d3pd->jet_AntiKt6H1Topo_emscale_phi->at(i)));
  }
  return hx_emscale;
}
float HTy_emscale() {

  float hy_emscale = 0;

  for (Int_t i = 0; i < d3pd->jet_AntiKt6H1Topo_n; i++) {
    hy_emscale += d3pd->jet_AntiKt6H1Topo_emscale_pt->at(i);
   hy_emscale = hy_emscale *(sin(d3pd->jet_AntiKt6H1Topo_emscale_phi->at(i)));
  }
  return hy_emscale;
}
*/

  Int_t vxp_n() {return d3pd->vxp_n;}
  float vxp_x(int i) {return (d3pd->vxp_n>i ? d3pd->vxp_x->at(i): -999999.);}
  float vxp_y(int i) {return (d3pd->vxp_n>i ? d3pd->vxp_y->at(i): -999999.);}
  float vxp_z(int i) {return (d3pd->vxp_n>i ? d3pd->vxp_z->at(i): -999999.);}
  float vxp_nTracks(int i) {return (d3pd->vxp_n>i ? d3pd->vxp_nTracks->at(i): -999999.);}

  /*
  Int_t el_n() {return d3pd->el_n;}
  float el_eta(int i) {return (d3pd->el_n>i ? d3pd->el_eta->at(i): -999999.);}
  float el_pt(int i) {return (d3pd->el_n>i ? d3pd->el_pt->at(i): -999999.);}
  float el_phi(int i) {return (d3pd->el_n>i ? d3pd->el_phi->at(i): -999999.);}
  int el_author(int i) {return (d3pd->el_n>i ? d3pd->el_author->at(i): -999999.);}
  int el_tight(int i) {return (d3pd->el_n>i ? d3pd->el_tight->at(i): -999999.);}
  */


 private:
  D3PD *d3pd;

  //MissingET objects
  MissingET _met_base;
  MissingET _met_corrTopo;
  MissingET _met_calib;
  MissingET _met_cryoCone;
  
  MissingET _met_refFinal;
  MissingET _met_refEle;
  MissingET _met_refMuon;
  MissingET _met_refGamma;
  MissingET _met_refJet;
  MissingET _met_softJets;
  MissingET _met_refTau;
  MissingET _met_cellOut;
  MissingET _met_cellOut_Eflow;
  MissingET _met_MuonBoy;
  
  //RefFinal_em
  MissingET _met_refFinal_em;
  MissingET _met_refEle_em;
  MissingET _met_refMuon_em;
  MissingET _met_refGamma_em;
  MissingET _met_refJet_em;
  MissingET _met_refTau_em;
  MissingET _met_cellOut_em;
    
  MissingET _METCryo;
  MissingET _METDM_All;
  MissingET _METDM_Crack1;
  MissingET _METDM_Crack2;
  MissingET _METDM_Cryo;
  MissingET _METFinal;
  MissingET _METTopoObj;
  MissingET _METMuonBoy_Spectro;
  MissingET _METMuonBoy_Track;
  MissingET _METMuon;
  MissingET _METBase0;
  MissingET _METTopo;
  MissingET _METLocHadTopo;

  MissingET _met_truth_int;
  MissingET _met_truth_nonInt;
  MissingET _met_truth_muons;

  //AFTER Pisa terms
  MissingET _met_refFinal_GCW_pt20;
  MissingET _met_refEle_GCW_pt20;
  MissingET _met_refGamma_GCW_pt20;
  MissingET _met_refJet_GCW_pt20;
  MissingET _met_softJets_GCW_pt20;
  MissingET _met_refTau_GCW_pt20;
  MissingET _met_Muon_Total_Staco_GCW_pt20;
  MissingET _met_Muon_Isol_Staco_GCW_pt20;
  MissingET _met_Muon_NonIsol_Staco_GCW_pt20;
  MissingET _met_RefMuon_Staco_GCW_pt20;
  MissingET _met_RefMuon_Track_Staco_GCW_pt20;
  MissingET _met_cellOut_GCW_pt20;
  MissingET _METCryo_GCW_pt20;
	 
  MissingET _met_refFinal_LCW_pt20;
  MissingET _met_refEle_LCW_pt20;
  MissingET _met_refGamma_LCW_pt20;
  MissingET _met_refJet_LCW_pt20;
  MissingET _met_softJets_LCW_pt20;
  MissingET _met_refTau_LCW_pt20;
  MissingET _met_Muon_Total_Muid_LCW_pt20;
  MissingET _met_Muon_Isol_Muid_LCW_pt20;
  MissingET _met_Muon_NonIsol_Muid_LCW_pt20;
  MissingET _met_RefMuon_Muid_LCW_pt20;
  MissingET _met_RefMuon_Track_Muid_LCW_pt20;
  MissingET _met_cellOut_LCW_pt20;
	 
  MissingET _met_refFinal_LCW_NI_eflow4;
  MissingET _met_refEle_LCW_NI_eflow4;
  MissingET _met_refGamma_LCW_NI_eflow4;
  MissingET _met_refJet_LCW_NI_eflow4;
  MissingET _met_softJets_LCW_NI_eflow4;
  MissingET _met_refTau_LCW_NI_eflow4;
  MissingET _met_Muon_Total_Staco_LCW_NI_eflow4;
  MissingET _met_Muon_Isol_Staco_LCW_NI_eflow4;
  MissingET _met_Muon_NonIsol_Staco_LCW_NI_eflow4;
  MissingET _met_RefMuon_Staco_LCW_NI_eflow4;
  MissingET _met_RefMuon_Track_Staco_LCW_NI_eflow4;
  MissingET _met_cellOut_LCW_NI_eflow4;
  MissingET _METCryo_LCW_NI_eflow4;

  MissingET _met_refFinal_LCW_NI_eflow_400;
  MissingET _met_refEle_LCW_NI_eflow_400;
  MissingET _met_refGamma_LCW_NI_eflow_400;
  MissingET _met_refJet_LCW_NI_eflow_400;
  MissingET _met_softJets_LCW_NI_eflow_400;
  MissingET _met_refTau_LCW_NI_eflow_400;
  MissingET _met_Muon_Total_Staco_LCW_NI_eflow_400;
  MissingET _met_Muon_Isol_Staco_LCW_NI_eflow_400;
  MissingET _met_Muon_NonIsol_Staco_LCW_NI_eflow_400;
  MissingET _met_RefMuon_Staco_LCW_NI_eflow_400;
  MissingET _met_RefMuon_Track_Staco_LCW_NI_eflow_400;
  MissingET _met_cellOut_LCW_NI_eflow_400;
  MissingET _METCryo_LCW_NI_eflow_400;
  
  MissingET _met_refFinal_LCW_NI_eflow_CB;
  MissingET _met_refEle_LCW_NI_eflow_CB;
  MissingET _met_refGamma_LCW_NI_eflow_CB;
  MissingET _met_refJet_LCW_NI_eflow_CB;
  MissingET _met_softJets_LCW_NI_eflow_CB;
  MissingET _met_refTau_LCW_NI_eflow_CB;
  MissingET _met_Muon_Total_Staco_LCW_NI_eflow_CB;
  MissingET _met_Muon_Isol_Staco_LCW_NI_eflow_CB;
  MissingET _met_Muon_NonIsol_Staco_LCW_NI_eflow_CB;
  MissingET _met_RefMuon_Staco_LCW_NI_eflow_CB;
  MissingET _met_RefMuon_Track_Staco_LCW_NI_eflow_CB;
  MissingET _met_cellOut_LCW_NI_eflow_CB;
  MissingET _METCryo_LCW_NI_eflow_CB;

	 
};

#endif // MISSINGETDATA_H
