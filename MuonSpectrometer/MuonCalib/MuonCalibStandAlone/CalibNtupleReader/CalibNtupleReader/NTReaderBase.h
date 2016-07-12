/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Nov 23 13:43:27 2005 by ROOT version 4.04/02f
// from TChain NTReaderBase/
//////////////////////////////////////////////////////////

#ifndef NTReaderBase_h
#define NTReaderBase_h

#include <string>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
/**
   @class NTReaderBase
   created by the ROOT MakeClass() method on the
   CalibrationNtuple. Holds all variables of the CalibrationNtuple
   @author Zdenko.Van.Kesteren@cern.ch
*/
namespace MuonCalib {

class NTReaderBase {
public :
  TTree          *fChain;   //<! pointer to the analyzed TTree or TChain
  Int_t           fCurrent; //<! current Tree number in a TChain

  // Declaration of leaf types
  Int_t           truth_nTruth;
  Double_t        truth_kinEnergy[3000];   //[truth_nTruth]
  Double_t        truth_gPosX[3000];   //[truth_nTruth]
  Double_t        truth_gPosY[3000];   //[truth_nTruth]
  Double_t        truth_gPosZ[3000];   //[truth_nTruth]
  Double_t        truth_pX[3000];   //[truth_nTruth]
  Double_t        truth_pY[3000];   //[truth_nTruth]
  Double_t        truth_pZ[3000];   //[truth_nTruth]
  Int_t           truth_PDGCode[3000];   //[truth_nTruth]
  Int_t           truth_barCode[3000];
  Int_t           mdtTruth_nMdtTruthHit;
  Int_t           mdtTruth_id[3000];
  Int_t           mdtTruth_barCode[3000];
  Double_t        mdtTruth_driftRadius[3000];
  Double_t        mdtTruth_posAlongTube[3000]; 
  Int_t           rpcTruth_nRpcTruthHit;
  Int_t           rpcTruth_id[3000];   //[rpcTruth_nRpcTruthHit]
  Int_t           rpcTruth_barCode[3000];   //[rpcTruth_nRpcTruthHit]
  Double_t        rpcTruth_time[3000];   //[rpcTruth_nRpcTruthHit]
  Int_t           tgcTruth_nTgcTruthHit;
  Int_t           tgcTruth_id[3000];   //[tgcTruth_nTgcTruthHit]
  Int_t           tgcTruth_barCode[3000];   //[tgcTruth_nTgcTruthHit]
  Double_t        tgcTruth_time[3000];   //[tgcTruth_nTgcTruthHit]
  Int_t           cscTruth_nCscTruthHit;
  Int_t           cscTruth_id[3000];   //[cscTruth_nCscTruthHit]
  Int_t           cscTruth_barCode[3000];   //[cscTruth_nCscTruthHit]
  Double_t        cscTruth_time[3000];   //[cscTruth_nCscTruthHit]  
  Int_t           event_nEvent;
  Int_t           event_eventNumber;   //[event_nEvent]
  Int_t           event_runNumber;   //[event_nEvent]
  Int_t           event_timeStamp;   //[event_nEvent]
  Int_t           event_lumiBlock;   //[event_nEvent]
  Int_t           event_bcId;   //[event_nEvent]
  Float_t         event_mbtsTimeDiff;   //[event_nEvent]
  Float_t         event_larTimeDiff;   //[event_nEvent]
  Char_t          event_eventTag[64];   //[event_nEvent]
  Int_t           event_n_on_TAV_bits;
  Int_t           event_on_TAV_bits[200];
  Int_t           event_n_on_TAP_bits;
  Int_t           event_on_TAP_bits[200];
  Int_t           pat_nPatterns;
  Float_t         pat_chi2[500];   //[pat_nPatterns]
  Float_t         pat_z0[500];   //[pat_nPatterns]
  Float_t         pat_r0[500];   //[pat_nPatterns]
  Float_t         pat_invP[500];   //[pat_nPatterns]
  Float_t         pat_phi[500];   //[pat_nPatterns]
  Float_t         pat_theta[500];   //[pat_nPatterns]
  Int_t           pat_nSegments[500];   //[pat_nPatterns]
  Int_t           pat_nmdt[500];   //[pat_nPatterns]
  Int_t           pat_nrpc[500];   //[pat_nPatterns]
  Int_t           pat_ncsc[500];   //[pat_nPatterns]
  Int_t           pat_ntgc[500];   //[pat_nPatterns]
  Int_t           seg_nSegments;
  Int_t           seg_patIndex[500];   //[seg_nSegments]
  Int_t           seg_author[500];   //[seg_nSegments]
  Int_t           seg_quality[500];   //[seg_nSegments]
  Float_t         seg_chi2[500];   //[seg_nSegments]
  Float_t         seg_fittedT0[500];   //[seg_nSegments]
  Float_t         seg_gPosX[500];   //[seg_nSegments]
  Float_t         seg_gPosY[500];   //[seg_nSegments]
  Float_t         seg_gPosZ[500];   //[seg_nSegments]
  Float_t         seg_gDirX[500];   //[seg_nSegments]
  Float_t         seg_gDirY[500];   //[seg_nSegments]
  Float_t         seg_gDirZ[500];   //[seg_nSegments]
  Float_t         seg_posX[500];   //[seg_nSegments]
  Float_t         seg_posY[500];   //[seg_nSegments]
  Float_t         seg_posZ[500];   //[seg_nSegments]
  Float_t         seg_dirX[500];   //[seg_nSegments]
  Float_t         seg_dirY[500];   //[seg_nSegments]
  Float_t         seg_dirZ[500];   //[seg_nSegments]
  Int_t           seg_nHits[500];   //[seg_nSegments]
  Int_t           seg_nMdtHits[500];   //[seg_nSegments]
  Int_t           seg_nRpcHits[500];   //[seg_nSegments]
  Int_t           seg_nTgcHits[500];   //[seg_nSegments]
  Int_t           seg_nCscHits[500];   //[seg_nSegments]
  Double_t        seg_transPhi[500];   //[seg_nSegments]
  Double_t        seg_transTheta[500];   //[seg_nSegments]
  Double_t        seg_transPsi[500];   //[seg_nSegments]
  Double_t        seg_transX[500];   //[seg_nSegments]
  Double_t        seg_transY[500];   //[seg_nSegments]
  Double_t        seg_transZ[500];   //[seg_nSegments]
  Int_t           mdt_nMdt;
  Int_t           mdt_segIndex[3000];   //[mdt_nMdt]
  Int_t           mdt_id[3000];         //[mdt_nMdt]
  Int_t           mdt_tdc[3000];        //[mdt_nMdt]
  Int_t           mdt_adc[3000];        //[mdt_nMdt]
  Float_t         mdt_t[3000];          //[mdt_nMdt]
  Float_t         mdt_r[3000];          //[mdt_nMdt]
  Float_t         mdt_dr[3000];         //[mdt_nMdt]
  Float_t         mdt_rTrk[3000];       //[mdt_nMdt]
  Float_t         mdt_drTrk[3000];      //[mdt_nMdt]
  Float_t         mdt_resi[3000];       //[mdt_nMdt]
  Float_t         mdt_distRO[3000];     //[mdt_nMdt]
  Float_t         mdt_slewTime[3000];   //[mdt_nMdt]
  Float_t         mdt_lorTime[3000];    //[mdt_nMdt]
  Float_t         mdt_propTime[3000];   //[mdt_nMdt]
  Float_t         mdt_tof[3000];        //[mdt_nMdt]
  Float_t         mdt_bFieldPerp[3000]; //[mdt_nMdt]
  Float_t         mdt_bFieldPara[3000]; //[mdt_nMdt]
  Float_t         mdt_temperature[3000];//[mdt_nMdt]
  Float_t         mdt_projSag[3000];    //[mdt_nMdt]
  Float_t         mdt_posX[3000];       //[mdt_nMdt]
  Float_t         mdt_posY[3000];       //[mdt_nMdt]
  Float_t         mdt_posZ[3000];       //[mdt_nMdt]
  Float_t         mdt_gPosX[3000];      //[mdt_nMdt]
  Float_t         mdt_gPosY[3000];      //[mdt_nMdt]
  Float_t         mdt_gPosZ[3000];      //[mdt_nMdt]
  Float_t         mdt_tube_t0[3000];    //[mdt_nMdt]
  Float_t         mdt_tube_adccal[3000];//[mdt_nMdt]
  Float_t         mdt_xtwin[3000];            //[mdt_nMdt]
  Bool_t          mdt_segmentT0Applied[3000]; //[mdt_nMdt]
  
  // old RPC nomenclature (for a time of transition)
  Int_t           rpc_nRpcHits;
  Int_t           rpc_segIndex[3000];   //[rpc_nRpcHits]
  Int_t           rpc_id[3000];   //[rpc_nRpcHits]
  Int_t           rpc_nStrips[3000];   //[rpc_nRpcHits]
  Float_t         rpc_stripWidth[3000];   //[rpc_nRpcHits]
  Float_t         rpc_stripLength[3000];
  Float_t         rpc_error[3000];   //[rpc_nRpcHits]
  Float_t         rpc_time[3000];   //[rpc_nRpcHits]
  Float_t         rpc_posX[3000];   //[rpc_nRpcHits]
  Float_t         rpc_posY[3000];   //[rpc_nRpcHits]
  Float_t         rpc_posZ[3000];   //[rpc_nRpcHits]
  Float_t         rpc_gPosX[3000];   //[rpc_nRpcHits]
  Float_t         rpc_gPosY[3000];   //[rpc_nRpcHits]
  Float_t         rpc_gPosZ[3000];   //[rpc_nRpcHits]
  Float_t         rpc_distanceToRO[3000];
  //
  Int_t           rpcOs_nRpcHits;
  Int_t           rpcOs_segIndex[3000];   //[rpcOs_nRpcHits]
  Int_t           rpcOs_id[3000];   //[rpcOs_nRpcHits]
  Int_t           rpcOs_nStrips[3000];   //[rpcOs_nRpcHits]
  Float_t         rpcOs_stripWidth[3000];   //[rpcOs_nRpcHits]
  Float_t         rpcOs_stripLength[3000];
  Float_t         rpcOs_error[3000];   //[rpcOs_nRpcHits]
  Float_t         rpcOs_time[3000];   //[rpcOs_nRpcHits]
  Float_t         rpcOs_posX[3000];   //[rpcOs_nRpcHits]
  Float_t         rpcOs_posY[3000];   //[rpcOs_nRpcHits]
  Float_t         rpcOs_posZ[3000];   //[rpcOs_nRpcHits]
  Float_t         rpcOs_gPosX[3000];   //[rpcOs_nRpcHits]
  Float_t         rpcOs_gPosY[3000];   //[rpcOs_nRpcHits]
  Float_t         rpcOs_gPosZ[3000];   //[rpcOs_nRpcHits]
  Float_t         rpcOs_distanceToRO[3000];
  Int_t           rpc_fchan;
  Int_t           rpc_sector[3000];           
  Int_t           rpc_padId[3000];            
  Int_t           rpc_status[3000];           
  Int_t           rpc_ercode[3000];        
  Int_t           rpc_cmaId[3000];            
  Int_t           rpc_fel1Id[3000];           
  Int_t           rpc_febcId[3000];           
  Int_t           rpc_crc[3000];              
  Int_t           rpc_bcId[3000];             
  Int_t           rpc_ticks[3000];             
  Int_t           rpc_ijk[3000];              
  Int_t           rpc_cmachan[3000];          
  Int_t           rpc_overlap[3000];          
  Int_t           rpc_threshold[3000];
  Int_t           tgc_nTgc;
  Int_t           tgc_segIndex[3000];   //[tgc_nTgc]
  Int_t           tgc_id[3000];   //[tgc_nTgc]
  Int_t           tgc_nStrips[3000];   //[tgc_nTgc]
  Float_t         tgc_stripWidth[3000];   //[tgc_nTgc]
  Float_t         tgc_stripLength[3000];   //[tgc_nTgc]
  Float_t         tgc_error[3000];   //[tgc_nTgc]
  Float_t         tgc_posX[3000];   //[tgc_nTgc]
  Float_t         tgc_posY[3000];   //[tgc_nTgc]
  Float_t         tgc_posZ[3000];   //[tgc_nTgc]
  Float_t         tgc_gPosX[3000];   //[tgc_nTgc]
  Float_t         tgc_gPosY[3000];   //[tgc_nTgc]
  Float_t         tgc_gPosZ[3000];   //[tgc_nTgc]
  Int_t           csc_nCsc;
  Int_t           csc_segIndex[3000];   //[csc_nCsc]
  Int_t           csc_id[3000];   //[csc_nCsc]
  Int_t           csc_nStrips[3000];   //[csc_nCsc]
  Float_t         csc_stripWidth[3000];   //[csc_nCsc]
  Int_t           csc_charge[3000];   //[csc_nCsc]
  Float_t         csc_error[3000];   //[csc_nCsc]
  Float_t         csc_posX[3000];   //[csc_nCsc]
  Float_t         csc_posY[3000];   //[csc_nCsc]
  Float_t         csc_posZ[3000];   //[csc_nCsc]
  Float_t         csc_gPosX[3000];   //[csc_nCsc]
  Float_t         csc_gPosY[3000];   //[csc_nCsc]
  Float_t         csc_gPosZ[3000];   //[csc_nCsc]
  Int_t           rawMdt_nRMdt;
  Int_t           rawMdt_occupancy[13000];   //[rawMdt_nRMdt]
  Int_t           rawMdt_id[13000];   //[rawMdt_nRMdt]
  Int_t           rawMdt_tdc[13000];   //[rawMdt_nRMdt]
  Int_t           rawMdt_adc[13000];   //[rawMdt_nRMdt]
  Float_t         rawMdt_t[13000];   //[rawMdt_nRMdt]
  Float_t         rawMdt_r[13000];   //[rawMdt_nRMdt]
  Float_t         rawMdt_dr[13000];   //[rawMdt_nRMdt]
  Float_t         rawMdt_posX[13000];   //[rawMdt_nRMdt]
  Float_t         rawMdt_posY[13000];   //[rawMdt_nRMdt]
  Float_t         rawMdt_posZ[13000];   //[rawMdt_nRMdt]
  Float_t         rawMdt_gPosX[13000];   //[rawMdt_nRMdt]
  Float_t         rawMdt_gPosY[13000];   //[rawMdt_nRMdt]
  Float_t         rawMdt_gPosZ[13000];   //[rawMdt_nRMdt]
  Int_t           rawRpc_nRRpc;
  Int_t           rawRpc_occupancy[3000];   //[rawRpc_nRRpc]
  Int_t           rawRpc_id[3000];   //[rawRpc_nRRpc]
  Float_t         rawRpc_t[3000];   //[rawRpc_nRRpc]
  Float_t         rawRpc_width[3000];   //[rawRpc_nRRpc]
  Float_t         rawRpc_length[3000]; 
  Float_t         rawRpc_gPosX[3000];   //[rawRpc_nRRpc]
  Float_t         rawRpc_gPosY[3000];   //[rawRpc_nRRpc]
  Float_t         rawRpc_gPosZ[3000];   //[rawRpc_nRRpc]
  Int_t           rawTgc_nRTgc_All_Bunch;
  Int_t           rawTgc_bcTag[3000];   //[rawTgc_nRTgc_All_Bunch]
  Int_t           rawTgc_num_Prev[3000];   //[rawTgc_nRTgc_All_Bunch]
  Int_t           rawTgc_num_Curr[3000];   //[rawTgc_nRTgc_All_Bunch]
  Int_t           rawTgc_num_Next[3000];   //[rawTgc_nRTgc_All_Bunch]
  Int_t           rawTgc_nRTgc;
  Int_t           rawTgc_occupancy[3000];   //[rawTgc_nRTgc]
  Int_t           rawTgc_id[3000];   //[rawTgc_nRTgc]
  Int_t           rawTgc_station[3000];   //[rawTgc_nRTgc]
  Int_t           rawTgc_eta[3000];   //[rawTgc_nRTgc]
  Int_t           rawTgc_phi[3000];   //[rawTgc_nRTgc]
  Int_t           rawTgc_gasGap[3000];   //[rawTgc_nRTgc]
  Int_t           rawTgc_isStrip[3000];   //[rawTgc_nRTgc]
  Int_t           rawTgc_channel[3000];   //[rawTgc_nRTgc]
  Float_t         rawTgc_width[3000];   //[rawTgc_nRTgc]
  Float_t         rawTgc_shortWidth[3000];   //[rawTgc_nRTgc]
  Float_t         rawTgc_longWidth[3000];   //[rawTgc_nRTgc]
  Float_t         rawTgc_length[3000];   //[rawTgc_nRTgc]
  Float_t         rawTgc_gPosX[3000];   //[rawTgc_nRTgc]
  Float_t         rawTgc_gPosY[3000];   //[rawTgc_nRTgc]
  Float_t         rawTgc_gPosZ[3000];   //[rawTgc_nRTgc]
  Int_t           rawTgc_nRTgc_Prev;
  Int_t           rawTgc_occupancy_Prev[3000];   //[rawTgc_nRTgc_Prev]
  Int_t           rawTgc_id_Prev[3000];   //[rawTgc_nRTgc_Prev]
  Int_t           rawTgc_station_Prev[3000];   //[rawTgc_nRTgc_Prev]
  Int_t           rawTgc_eta_Prev[3000];   //[rawTgc_nRTgc_Prev]
  Int_t           rawTgc_phi_Prev[3000];   //[rawTgc_nRTgc_Prev]
  Int_t           rawTgc_gasGap_Prev[3000];   //[rawTgc_nRTgc_Prev]
  Int_t           rawTgc_isStrip_Prev[3000];   //[rawTgc_nRTgc_Prev]
  Int_t           rawTgc_channel_Prev[3000];   //[rawTgc_nRTgc_Prev]
  Float_t         rawTgc_width_Prev[3000];   //[rawTgc_nRTgc_Prev]
  Float_t         rawTgc_shortWidth_Prev[3000];   //[rawTgc_nRTgc_Prev]
  Float_t         rawTgc_longWidth_Prev[3000];   //[rawTgc_nRTgc_Prev]
  Float_t         rawTgc_length_Prev[3000];   //[rawTgc_nRTgc_Prev]
  Float_t         rawTgc_gPosX_Prev[3000];   //[rawTgc_nRTgc_Prev]
  Float_t         rawTgc_gPosY_Prev[3000];   //[rawTgc_nRTgc_Prev]
  Float_t         rawTgc_gPosZ_Prev[3000];   //[rawTgc_nRTgc_Prev]
  Int_t           rawTgc_nRTgc_Next;
  Int_t           rawTgc_occupancy_Next[3000];   //[rawTgc_nRTgc_Next]
  Int_t           rawTgc_id_Next[3000];   //[rawTgc_nRTgc_Next]
  Int_t           rawTgc_station_Next[3000];   //[rawTgc_nRTgc_Next]
  Int_t           rawTgc_eta_Next[3000];   //[rawTgc_nRTgc_Next]
  Int_t           rawTgc_phi_Next[3000];   //[rawTgc_nRTgc_Next]
  Int_t           rawTgc_gasGap_Next[3000];   //[rawTgc_nRTgc_Next]
  Int_t           rawTgc_isStrip_Next[3000];   //[rawTgc_nRTgc_Next]
  Int_t           rawTgc_channel_Next[3000];   //[rawTgc_nRTgc_Next]
  Float_t         rawTgc_width_Next[3000];   //[rawTgc_nRTgc_Next]
  Float_t         rawTgc_shortWidth_Next[3000];   //[rawTgc_nRTgc_Next]
  Float_t         rawTgc_longWidth_Next[3000];   //[rawTgc_nRTgc_Next]
  Float_t         rawTgc_length_Next[3000];   //[rawTgc_nRTgc_Next]
  Float_t         rawTgc_gPosX_Next[3000];   //[rawTgc_nRTgc_Next]
  Float_t         rawTgc_gPosY_Next[3000];   //[rawTgc_nRTgc_Next]
  Float_t         rawTgc_gPosZ_Next[3000];   //[rawTgc_nRTgc_Next]
  Int_t           rawTgcCoin_nRTgc_All;
  Int_t           rawTgcCoin_nRTgc;
  Int_t           rawTgcCoin_nRTgc_Prev;
  Int_t           rawTgcCoin_nRTgc_Next;
  Int_t           rawTgcCoin_nRTgc_tracklet;
  Int_t           rawTgcCoin_nRTgc_highpt;
  Int_t           rawTgcCoin_nRTgc_sl;
  Int_t           rawTgcCoin_nRTgcTracklet;
  Int_t           rawTgcCoin_nRTgcTracklet_Prev;
  Int_t           rawTgcCoin_nRTgcTracklet_Next;
  Int_t           rawTgcCoin_nRTgcHighPt;
  Int_t           rawTgcCoin_nRTgcHighPt_Prev;
  Int_t           rawTgcCoin_nRTgcHighPt_Next;
  Int_t           rawTgcCoin_nRTgcSl;
  Int_t           rawTgcCoin_nRTgcSl_Prev;
  Int_t           rawTgcCoin_nRTgcSl_Next;
  Int_t           rawTgcCoin_type_bcTag[3000];   //[rawTgcCoin_nRTg_All]
  Int_t           rawTgcCoin_num_tracklet_Prev[3000];   //[rawTgcCoin_nRTg_All]
  Int_t           rawTgcCoin_num_tracklet_Curr[3000];   //[rawTgcCoin_nRTg_All]
  Int_t           rawTgcCoin_num_tracklet_Next[3000];   //[rawTgcCoin_nRTg_All]
  Int_t           rawTgcCoin_num_highpt_Prev[3000];   //[rawTgcCoin_nRTg_All]
  Int_t           rawTgcCoin_num_highpt_Curr[3000];   //[rawTgcCoin_nRTg_All]
  Int_t           rawTgcCoin_num_highpt_Next[3000];   //[rawTgcCoin_nRTg_All]
  Int_t           rawTgcCoin_num_sl_Prev[3000];   //[rawTgcCoin_nRTg_All]
  Int_t           rawTgcCoin_num_sl_Curr[3000];   //[rawTgcCoin_nRTg_All]
  Int_t           rawTgcCoin_num_sl_Next[3000];   //[rawTgcCoin_nRTg_All]
  Int_t           rawTgcCoin_bcTag_tracklet[3000];   //[rawTgcCoin_nRTgc_tracklet]
  Int_t           rawTgcCoin_bcTag_highpt[3000];   //[rawTgcCoin_nRTgc_highpt]
  Int_t           rawTgcCoin_bcTag_sl[3000];   //[rawTgcCoin_nRTgc_sl]
  Int_t           rawTgcCoin_type[3000];   //[rawTgcCoin_nRTgc]
  Int_t           rawTgcCoin_type_Prev[3000];   //[rawTgcCoin_nRTgc_Prev]
  Int_t           rawTgcCoin_type_Next[3000];   //[rawTgcCoin_nRTgc_Next]
  Int_t           rawTgcCoin_id_tracklet[3000];   //[rawTgcCoin_nRTgcTracklet]
  Int_t           rawTgcCoin_eta_tracklet[3000];   //[rawTgcCoin_nRTgcTracklet]
  Int_t           rawTgcCoin_phi_tracklet[3000];   //[rawTgcCoin_nRTgcTracklet]
  Int_t           rawTgcCoin_sector_tracklet[3000];   //[rawTgcCoin_nRTgcTracklet]
  Int_t           rawTgcCoin_isForward_tracklet[3000];   //[rawTgcCoin_nRTgcTracklet]
  Int_t           rawTgcCoin_isStrip_tracklet[3000];   //[rawTgcCoin_nRTgcTracklet]
  Int_t           rawTgcCoin_trackletId_tracklet[3000];   //[rawTgcCoin_nRTgcTracklet]
  Int_t           rawTgcCoin_delta_tracklet[3000];   //[rawTgcCoin_nRTgcTracklet]
  Int_t           rawTgcCoin_sub_tracklet[3000];   //[rawTgcCoin_nRTgcTracklet]
  Float_t         rawTgcCoin_widthIn_tracklet[3000];   //[rawTgcCoin_nRTgcTracklet]
  Float_t         rawTgcCoin_widthOut_tracklet[3000];   //[rawTgcCoin_nRTgcTracklet]
  Float_t         rawTgcCoin_gPosInX_tracklet[3000];   //[rawTgcCoin_nRTgcTracklet]
  Float_t         rawTgcCoin_gPosInY_tracklet[3000];   //[rawTgcCoin_nRTgcTracklet]
  Float_t         rawTgcCoin_gPosInZ_tracklet[3000];   //[rawTgcCoin_nRTgcTracklet]
  Float_t         rawTgcCoin_gPosOutX_tracklet[3000];   //[rawTgcCoin_nRTgcTracklet]
  Float_t         rawTgcCoin_gPosOutY_tracklet[3000];   //[rawTgcCoin_nRTgcTracklet]
  Float_t         rawTgcCoin_gPosOutZ_tracklet[3000];   //[rawTgcCoin_nRTgcTracklet]
  Int_t           rawTgcCoin_id_highpt[3000];   //[rawTgcCoin_nRTgcHighPt]
  Int_t           rawTgcCoin_eta_highpt[3000];   //[rawTgcCoin_nRTgcHighPt]
  Int_t           rawTgcCoin_phi_highpt[3000];   //[rawTgcCoin_nRTgcHighPt]
  Int_t           rawTgcCoin_sector_highpt[3000];   //[rawTgcCoin_nRTgcHighPt]
  Int_t           rawTgcCoin_isForward_highpt[3000];   //[rawTgcCoin_nRTgcHighPt]
  Int_t           rawTgcCoin_isStrip_highpt[3000];   //[rawTgcCoin_nRTgcHighPt]
  Int_t           rawTgcCoin_trackletId_highpt[3000];   //[rawTgcCoin_nRTgcHighPt]
  Int_t           rawTgcCoin_delta_highpt[3000];   //[rawTgcCoin_nRTgcHighPt]
  Int_t           rawTgcCoin_sub_highpt[3000];   //[rawTgcCoin_nRTgcHighPt]
  Float_t         rawTgcCoin_widthIn_highpt[3000];   //[rawTgcCoin_nRTgcHighPt]
  Float_t         rawTgcCoin_widthOut_highpt[3000];   //[rawTgcCoin_nRTgcHighPt]
  Float_t         rawTgcCoin_gPosInX_highpt[3000];   //[rawTgcCoin_nRTgcHighPt]
  Float_t         rawTgcCoin_gPosInY_highpt[3000];   //[rawTgcCoin_nRTgcHighPt]
  Float_t         rawTgcCoin_gPosInZ_highpt[3000];   //[rawTgcCoin_nRTgcHighPt]
  Float_t         rawTgcCoin_gPosOutX_highpt[3000];   //[rawTgcCoin_nRTgcHighPt]
  Float_t         rawTgcCoin_gPosOutY_highpt[3000];   //[rawTgcCoin_nRTgcHighPt]
  Float_t         rawTgcCoin_gPosOutZ_highpt[3000];   //[rawTgcCoin_nRTgcHighPt]
  Int_t           rawTgcCoin_id_sl[3000];   //[rawTgcCoin_nRTgcSl]
  Int_t           rawTgcCoin_eta_sl[3000];   //[rawTgcCoin_nRTgcSl]
  Int_t           rawTgcCoin_phi_sl[3000];   //[rawTgcCoin_nRTgcSl]
  Int_t           rawTgcCoin_sector_sl[3000];   //[rawTgcCoin_nRTgcSl]
  Int_t           rawTgcCoin_isForward_sl[3000];   //[rawTgcCoin_nRTgcSl]
  Int_t           rawTgcCoin_trackletId_sl[3000];   //[rawTgcCoin_nRTgcSl]
  Int_t           rawTgcCoin_trackletIdStrip_sl[3000];   //[rawTgcCoin_nRTgcSl]
  Int_t           rawTgcCoin_roi_sl[3000];   //[rawTgcCoin_nRTgcSl]
  Int_t           rawTgcCoin_pt_sl[3000];   //[rawTgcCoin_nRTgcSl]
  Int_t           rawTgcCoin_isPositiveDeltaR_sl[3000];   //[rawTgcCoin_nRTgcSl]
  Float_t         rawTgcCoin_widthR_sl[3000];   //[rawTgcCoin_nRTgcSl]
  Float_t         rawTgcCoin_widthPhi_sl[3000];   //[rawTgcCoin_nRTgcSl]
  Float_t         rawTgcCoin_gPosOutX_sl[3000];   //[rawTgcCoin_nRTgcSl]
  Float_t         rawTgcCoin_gPosOutY_sl[3000];   //[rawTgcCoin_nRTgcSl]
  Float_t         rawTgcCoin_gPosOutZ_sl[3000];   //[rawTgcCoin_nRTgcSl]
  Int_t           rawTgcCoin_id_tracklet_Prev[3000];   //[rawTgcCoin_nRTgcTracklet_Prev]
  Int_t           rawTgcCoin_eta_tracklet_Prev[3000];   //[rawTgcCoin_nRTgcTracklet_Prev]
  Int_t           rawTgcCoin_phi_tracklet_Prev[3000];   //[rawTgcCoin_nRTgcTracklet_Prev]
  Int_t           rawTgcCoin_sector_tracklet_Prev[3000];   //[rawTgcCoin_nRTgcTracklet_Prev]
  Int_t           rawTgcCoin_isForward_tracklet_Prev[3000];   //[rawTgcCoin_nRTgcTracklet_Prev]
  Int_t           rawTgcCoin_isStrip_tracklet_Prev[3000];   //[rawTgcCoin_nRTgcTracklet_Prev]
  Int_t           rawTgcCoin_trackletId_tracklet_Prev[3000];   //[rawTgcCoin_nRTgcTracklet_Prev]
  Int_t           rawTgcCoin_delta_tracklet_Prev[3000];   //[rawTgcCoin_nRTgcTracklet_Prev]
  Int_t           rawTgcCoin_sub_tracklet_Prev[3000];   //[rawTgcCoin_nRTgcTracklet_Prev]
  Float_t         rawTgcCoin_widthIn_tracklet_Prev[3000];   //[rawTgcCoin_nRTgcTracklet_Prev]
  Float_t         rawTgcCoin_widthOut_tracklet_Prev[3000];   //[rawTgcCoin_nRTgcTracklet_Prev]
  Float_t         rawTgcCoin_gPosInX_tracklet_Prev[3000];   //[rawTgcCoin_nRTgcTracklet_Prev]
  Float_t         rawTgcCoin_gPosInY_tracklet_Prev[3000];   //[rawTgcCoin_nRTgcTracklet_Prev]
  Float_t         rawTgcCoin_gPosInZ_tracklet_Prev[3000];   //[rawTgcCoin_nRTgcTracklet_Prev]
  Float_t         rawTgcCoin_gPosOutX_tracklet_Prev[3000];   //[rawTgcCoin_nRTgcTracklet_Prev]
  Float_t         rawTgcCoin_gPosOutY_tracklet_Prev[3000];   //[rawTgcCoin_nRTgcTracklet_Prev]
  Float_t         rawTgcCoin_gPosOutZ_tracklet_Prev[3000];   //[rawTgcCoin_nRTgcTracklet_Prev]
  Int_t           rawTgcCoin_id_highpt_Prev[3000];   //[rawTgcCoin_nRTgcHighPt_Prev]
  Int_t           rawTgcCoin_eta_highpt_Prev[3000];   //[rawTgcCoin_nRTgcHighPt_Prev]
  Int_t           rawTgcCoin_phi_highpt_Prev[3000];   //[rawTgcCoin_nRTgcHighPt_Prev]
  Int_t           rawTgcCoin_sector_highpt_Prev[3000];   //[rawTgcCoin_nRTgcHighPt_Prev]
  Int_t           rawTgcCoin_isForward_highpt_Prev[3000];   //[rawTgcCoin_nRTgcHighPt_Prev]
  Int_t           rawTgcCoin_isStrip_highpt_Prev[3000];   //[rawTgcCoin_nRTgcHighPt_Prev]
  Int_t           rawTgcCoin_trackletId_highpt_Prev[3000];   //[rawTgcCoin_nRTgcHighPt_Prev]
  Int_t           rawTgcCoin_delta_highpt_Prev[3000];   //[rawTgcCoin_nRTgcHighPt_Prev]
  Int_t           rawTgcCoin_sub_highpt_Prev[3000];   //[rawTgcCoin_nRTgcHighPt_Prev]
  Float_t         rawTgcCoin_widthIn_highpt_Prev[3000];   //[rawTgcCoin_nRTgcHighPt_Prev]
  Float_t         rawTgcCoin_widthOut_highpt_Prev[3000];   //[rawTgcCoin_nRTgcHighPt_Prev]
  Float_t         rawTgcCoin_gPosInX_highpt_Prev[3000];   //[rawTgcCoin_nRTgcHighPt_Prev]
  Float_t         rawTgcCoin_gPosInY_highpt_Prev[3000];   //[rawTgcCoin_nRTgcHighPt_Prev]
  Float_t         rawTgcCoin_gPosInZ_highpt_Prev[3000];   //[rawTgcCoin_nRTgcHighPt_Prev]
  Float_t         rawTgcCoin_gPosOutX_highpt_Prev[3000];   //[rawTgcCoin_nRTgcHighPt_Prev]
  Float_t         rawTgcCoin_gPosOutY_highpt_Prev[3000];   //[rawTgcCoin_nRTgcHighPt_Prev]
  Float_t         rawTgcCoin_gPosOutZ_highpt_Prev[3000];   //[rawTgcCoin_nRTgcHighPt_Prev]
  Int_t           rawTgcCoin_id_sl_Prev[3000];   //[rawTgcCoin_nRTgcSl_Prev]
  Int_t           rawTgcCoin_eta_sl_Prev[3000];   //[rawTgcCoin_nRTgcSl_Prev]
  Int_t           rawTgcCoin_phi_sl_Prev[3000];   //[rawTgcCoin_nRTgcSl_Prev]
  Int_t           rawTgcCoin_sector_sl_Prev[3000];   //[rawTgcCoin_nRTgcSl_Prev]
  Int_t           rawTgcCoin_isForward_sl_Prev[3000];   //[rawTgcCoin_nRTgcSl_Prev]
  Int_t           rawTgcCoin_trackletId_sl_Prev[3000];   //[rawTgcCoin_nRTgcSl_Prev]
  Int_t           rawTgcCoin_trackletIdStrip_sl_Prev[3000];   //[rawTgcCoin_nRTgcSl_Prev]
  Int_t           rawTgcCoin_roi_sl_Prev[3000];   //[rawTgcCoin_nRTgcSl_Prev]
  Int_t           rawTgcCoin_pt_sl_Prev[3000];   //[rawTgcCoin_nRTgcSl_Prev]
  Int_t           rawTgcCoin_isPositiveDeltaR_sl_Prev[3000];   //[rawTgcCoin_nRTgcSl_Prev]
  Float_t         rawTgcCoin_widthR_sl_Prev[3000];   //[rawTgcCoin_nRTgcSl_Prev]
  Float_t         rawTgcCoin_widthPhi_sl_Prev[3000];   //[rawTgcCoin_nRTgcSl_Prev]
  Float_t         rawTgcCoin_gPosOutX_sl_Prev[3000];   //[rawTgcCoin_nRTgcSl_Prev]
  Float_t         rawTgcCoin_gPosOutY_sl_Prev[3000];   //[rawTgcCoin_nRTgcSl_Prev]
  Float_t         rawTgcCoin_gPosOutZ_sl_Prev[3000];   //[rawTgcCoin_nRTgcSl_Prev]
  Int_t           rawTgcCoin_id_tracklet_Next[3000];   //[rawTgcCoin_nRTgcTracklet_Next]
  Int_t           rawTgcCoin_eta_tracklet_Next[3000];   //[rawTgcCoin_nRTgcTracklet_Next]
  Int_t           rawTgcCoin_phi_tracklet_Next[3000];   //[rawTgcCoin_nRTgcTracklet_Next]
  Int_t           rawTgcCoin_sector_tracklet_Next[3000];   //[rawTgcCoin_nRTgcTracklet_Next]
  Int_t           rawTgcCoin_isForward_tracklet_Next[3000];   //[rawTgcCoin_nRTgcTracklet_Next]
  Int_t           rawTgcCoin_isStrip_tracklet_Next[3000];   //[rawTgcCoin_nRTgcTracklet_Next]
  Int_t           rawTgcCoin_trackletId_tracklet_Next[3000];   //[rawTgcCoin_nRTgcTracklet_Next]
  Int_t           rawTgcCoin_delta_tracklet_Next[3000];   //[rawTgcCoin_nRTgcTracklet_Next]
  Int_t           rawTgcCoin_sub_tracklet_Next[3000];   //[rawTgcCoin_nRTgcTracklet_Next]
  Float_t         rawTgcCoin_widthIn_tracklet_Next[3000];   //[rawTgcCoin_nRTgcTracklet_Next]
  Float_t         rawTgcCoin_widthOut_tracklet_Next[3000];   //[rawTgcCoin_nRTgcTracklet_Next]
  Float_t         rawTgcCoin_gPosInX_tracklet_Next[3000];   //[rawTgcCoin_nRTgcTracklet_Next]
  Float_t         rawTgcCoin_gPosInY_tracklet_Next[3000];   //[rawTgcCoin_nRTgcTracklet_Next]
  Float_t         rawTgcCoin_gPosInZ_tracklet_Next[3000];   //[rawTgcCoin_nRTgcTracklet_Next]
  Float_t         rawTgcCoin_gPosOutX_tracklet_Next[3000];   //[rawTgcCoin_nRTgcTracklet_Next]
  Float_t         rawTgcCoin_gPosOutY_tracklet_Next[3000];   //[rawTgcCoin_nRTgcTracklet_Next]
  Float_t         rawTgcCoin_gPosOutZ_tracklet_Next[3000];   //[rawTgcCoin_nRTgcTracklet_Next]
  Int_t           rawTgcCoin_id_highpt_Next[3000];   //[rawTgcCoin_nRTgcHighPt_Next]
  Int_t           rawTgcCoin_eta_highpt_Next[3000];   //[rawTgcCoin_nRTgcHighPt_Next]
  Int_t           rawTgcCoin_phi_highpt_Next[3000];   //[rawTgcCoin_nRTgcHighPt_Next]
  Int_t           rawTgcCoin_sector_highpt_Next[3000];   //[rawTgcCoin_nRTgcHighPt_Next]
  Int_t           rawTgcCoin_isForward_highpt_Next[3000];   //[rawTgcCoin_nRTgcHighPt_Next]
  Int_t           rawTgcCoin_isStrip_highpt_Next[3000];   //[rawTgcCoin_nRTgcHighPt_Next]
  Int_t           rawTgcCoin_trackletId_highpt_Next[3000];   //[rawTgcCoin_nRTgcHighPt_Next]
  Int_t           rawTgcCoin_delta_highpt_Next[3000];   //[rawTgcCoin_nRTgcHighPt_Next]
  Int_t           rawTgcCoin_sub_highpt_Next[3000];   //[rawTgcCoin_nRTgcHighPt_Next]
  Float_t         rawTgcCoin_widthIn_highpt_Next[3000];   //[rawTgcCoin_nRTgcHighPt_Next]
  Float_t         rawTgcCoin_widthOut_highpt_Next[3000];   //[rawTgcCoin_nRTgcHighPt_Next]
  Float_t         rawTgcCoin_gPosInX_highpt_Next[3000];   //[rawTgcCoin_nRTgcHighPt_Next]
  Float_t         rawTgcCoin_gPosInY_highpt_Next[3000];   //[rawTgcCoin_nRTgcHighPt_Next]
  Float_t         rawTgcCoin_gPosInZ_highpt_Next[3000];   //[rawTgcCoin_nRTgcHighPt_Next]
  Float_t         rawTgcCoin_gPosOutX_highpt_Next[3000];   //[rawTgcCoin_nRTgcHighPt_Next]
  Float_t         rawTgcCoin_gPosOutY_highpt_Next[3000];   //[rawTgcCoin_nRTgcHighPt_Next]
  Float_t         rawTgcCoin_gPosOutZ_highpt_Next[3000];   //[rawTgcCoin_nRTgcHighPt_Next]
  Int_t           rawTgcCoin_id_sl_Next[3000];   //[rawTgcCoin_nRTgcSl_Next]
  Int_t           rawTgcCoin_eta_sl_Next[3000];   //[rawTgcCoin_nRTgcSl_Next]
  Int_t           rawTgcCoin_phi_sl_Next[3000];   //[rawTgcCoin_nRTgcSl_Next]
  Int_t           rawTgcCoin_sector_sl_Next[3000];   //[rawTgcCoin_nRTgcSl_Next]
  Int_t           rawTgcCoin_isForward_sl_Next[3000];   //[rawTgcCoin_nRTgcSl_Next]
  Int_t           rawTgcCoin_trackletId_sl_Next[3000];   //[rawTgcCoin_nRTgcSl_Next]
  Int_t           rawTgcCoin_trackletIdStrip_sl_Next[3000];   //[rawTgcCoin_nRTgcSl_Next]
  Int_t           rawTgcCoin_roi_sl_Next[3000];   //[rawTgcCoin_nRTgcSl_Next]
  Int_t           rawTgcCoin_pt_sl_Next[3000];   //[rawTgcCoin_nRTgcSl_Next]
  Int_t           rawTgcCoin_isPositiveDeltaR_sl_Next[3000];   //[rawTgcCoin_nRTgcSl_Next]
  Float_t         rawTgcCoin_widthR_sl_Next[3000];   //[rawTgcCoin_nRTgcSl_Next]
  Float_t         rawTgcCoin_widthPhi_sl_Next[3000];   //[rawTgcCoin_nRTgcSl_Next]
  Float_t         rawTgcCoin_gPosOutX_sl_Next[3000];   //[rawTgcCoin_nRTgcSl_Next]
  Float_t         rawTgcCoin_gPosOutY_sl_Next[3000];   //[rawTgcCoin_nRTgcSl_Next]
  Float_t         rawTgcCoin_gPosOutZ_sl_Next[3000];   //[rawTgcCoin_nRTgcSl_Next]
  Int_t           rawCsc_nRCsc;
  Int_t           rawCsc_occupancy[3000];   //[rawCsc_nRCsc]
  Int_t           rawCsc_id[3000];   //[rawCsc_nRCsc]
  Float_t         rawCsc_width[3000];   //[rawCsc_nRCsc]
  Float_t         rawCsc_charge[3000];   //[rawCsc_nRCsc]
  Float_t         rawCsc_t[3000];   //[rawCsc_nRCsc]
  Float_t         rawCsc_gPosX[3000];   //[rawCsc_nRCsc]
  Float_t         rawCsc_gPosY[3000];   //[rawCsc_nRCsc]
  Float_t         rawCsc_gPosZ[3000];   //[rawCsc_nRCsc]
  
  NTReaderBase(TTree *tree=0);              //!< default constructor, opening directory SegmentNtuple.root:/PatternNtupleMaker/Segments
  virtual ~NTReaderBase();                  //!< destructor
  virtual Int_t    Cut(Long64_t entry);     //!< empty routine
  virtual Int_t    GetEntry(Long64_t entry);//!< Read contents of entry
  virtual Long64_t LoadTree(Long64_t entry);//!< Set the environment to read one entry
  virtual void     Init(TTree *tree);       //!< The Init() function is called when the selector needs to initialize a new tree or chain.
  virtual void     Loop();                  //!< empty routine
  virtual void     Show(Long64_t entry = -1); //!< Print contents of entry.
  inline virtual void Notify() {}
  virtual void setNoTruth() {m_storeTruth=false;} //!< toggles flag m_storeTruth to false
  
 protected:
  
  bool m_storeTruth;                              //!< flag keeping track whether truth info is stored on the CalibrationNtuple
};
}//namespace MuonCalib
#endif
