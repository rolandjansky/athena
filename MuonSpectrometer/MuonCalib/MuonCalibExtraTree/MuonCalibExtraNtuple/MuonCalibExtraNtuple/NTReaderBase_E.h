/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef NTReaderBase_E_h
#define NTReaderBase_E_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <vector>

#include "CalibNtupleReader/NTReaderBase.h"

/**
   @class NTReaderBase_E
   Extended version of NTReaderBase, making it capable of holding variables from the extended Branches too.
   
   @author Zdenko.Van.Kesteren@cern.ch
*/

namespace MuonCalib {

  class NTReaderBase_E : virtual public NTReaderBase {
  public :
      
    // Declaration of leaf types
    Int_t           ctp_nTriggerInfo;
    UInt_t          ctp_numberBC;
    UInt_t          ctp_L1A_BC;
    UInt_t          ctp_timeNs;
    UInt_t          ctp_randomTrig;
    UInt_t          ctp_firedItemsBeforePrescale;
    UInt_t          ctp_prescaledClock;
    Int_t           ctp_bcid[7680];
    Int_t           ctp_bcIndexTriggerItems[7680];
    Int_t           ctp_bcIndexPIT[7680];
    Int_t           ctp_pit[7680];
    Int_t           ctp_tbp[7680];
    Int_t           ctp_tap[7680];
    Int_t           ctp_tav[7680];
    Int_t           ctp_type[7680];
    Float_t         ctp_delay[7680];
            
    Int_t           muctpi_nHits;
    Int_t           muctpi_roiWord[100];
    Int_t           muctpi_bcID[100];  
    Int_t           muctpi_sysID[100];
    Int_t           muctpi_subsysID[100];
    Int_t           muctpi_sectorID[100];
    Int_t           muctpi_thresholdNumber[100];
    Int_t           muctpi_thresholdValue[100];
    Int_t           muctpi_roINumber[100];
    Int_t           muctpi_overlap[100];
    Int_t           muctpi_firstCandidate[100];
    Int_t           muctpi_sectorOverflow[100];
    Int_t           muctpi_padOverflow[100];
    Float_t         muctpi_phi[100];
    Float_t         muctpi_eta[100];

    //Backward compatibility
    Int_t           rpcSl_nLogics;
    Int_t           rpcSl_sectorId[100];
    Int_t           rpcSl_fel1Id[100];
    Int_t           rpcSl_bcid[100];
    Int_t           rpcSl_errorCode[100];
    Int_t           rpcSl_crc[100];
    Int_t           rpcSl_hasMoreThan2TriggerCand[100];
    Int_t           rpcSl_nLogicsTimes;
    Int_t           rpcSl_slIndex[100];
    Float_t         rpcSl_triggerRates[100];
    Int_t           rpcSl_counters[100];
    Int_t           rpcSl_nSLHits;
    Int_t           rpcSl_slIndex2[100];  
    Int_t           rpcSl_rowinBcid[100];
    Int_t           rpcSl_padid[100];
    Int_t           rpcSl_ptid[100];
    Int_t           rpcSl_roi[100];
    Int_t           rpcSl_outerPlane[100];
    Int_t           rpcSl_overlapPhi[100];
    Int_t           rpcSl_overlapEta[100];
    Int_t           rpcSl_triggerBcid[100];
    Int_t           rpcSl_isInput[100];
    //End bwd comp
    Int_t           rpc_SL_nSectors;
    Int_t           rpc_SL_sectorId[100];   
    Int_t           rpc_SL_lvl1id[100];   
    Int_t           rpc_SL_bcid[100];   
    Int_t           rpc_SL_errorCode[100];   
    Int_t           rpc_SL_crc[100];   
    Int_t           rpc_SL_hasMoreThan2TriggerCand[100];   
    Int_t           rpc_SLnTriggerHits;
    Int_t           rpc_SLhit_sector[100];   
    Int_t           rpc_SLhit_rowinBcid[100];   
    Int_t           rpc_SLhit_padId[100];   
    Int_t           rpc_SLhit_ptId[100];   
    Int_t           rpc_SLhit_roi[100];   
    Int_t           rpc_SLhit_outerPlane[100];   
    Int_t           rpc_SLhit_overlapPhi[100];   
    Int_t           rpc_SLhit_overlapEta[100];   
    Int_t           rpc_SLhit_triggerBcid[100];   
    Int_t           rpc_SLhit_isInput[100];   
        
    Int_t           rawRpcCoin_nRpcCoin;
    Int_t           rawRpcCoin_occupancy[1000];   
    Int_t           rawRpcCoin_id[1000];   
    Float_t         rawRpcCoin_t[1000];   
    Float_t         rawRpcCoin_width[1000];   
    Float_t         rawRpcCoin_length[1000];   
    Float_t         rawRpcCoin_gPosX[1000];   
    Float_t         rawRpcCoin_gPosY[1000];   
    Float_t         rawRpcCoin_gPosZ[1000];   
    Int_t           rawRpcCoin_ijk[1000];   
    Int_t           rawRpcCoin_threshold[1000];   
    Int_t           rawRpcCoin_overlap[1000];   
    Int_t           rawRpcCoin_parentCmId[1000];   
    Int_t           rawRpcCoin_parentPadId[1000];   
    Int_t           rawRpcCoin_parentSectorId[1000];   
    Int_t           rawRpcCoin_lowPtCm[1000];   
    
    Int_t           calo_nHits;
    Int_t           calo_id[100];
    Float_t         calo_posX[100];
    Float_t         calo_posY[100];
    Float_t         calo_posZ[100];
    Float_t         calo_charge[100];
    Float_t         calo_time[100];
   
    Int_t           mbts_nHits;
    Int_t           mbts_id[100];
    Float_t         mbts_posX[100];
    Float_t         mbts_posY[100];
    Float_t         mbts_posZ[100];
    Float_t         mbts_charge[100];
    Float_t         mbts_time[100];
   
    Int_t           phi_nPhiHits;
    Int_t           phi_phiPatIndex[6000];
    UInt_t          phi_id[6000];
    Float_t         phi_posX[6000];
    Float_t         phi_posY[6000];
    Float_t         phi_posZ[6000];
    Float_t         phi_error[6000];
   
    Int_t           trk_nTracks;
    Float_t         trk_x0[3000];
    Float_t         trk_y0[3000];
    Float_t         trk_z0[3000];
    Float_t         trk_theta[3000];
    Float_t         trk_phi[3000];
    Float_t         trk_qOverP[3000];
//    Float_t         trk_errInvP[3000];
    Int_t           trk_author[3000] ;
    Float_t         trk_cov00[3000];
    Float_t         trk_cov01[3000];
    Float_t         trk_cov02[3000];
    Float_t         trk_cov03[3000];
    Float_t         trk_cov04[3000];
    Float_t         trk_cov11[3000];
    Float_t         trk_cov12[3000];
    Float_t         trk_cov13[3000];
    Float_t         trk_cov14[3000];
    Float_t         trk_cov22[3000];
    Float_t         trk_cov23[3000];
    Float_t         trk_cov24[3000];
    Float_t         trk_cov33[3000];
    Float_t         trk_cov34[3000];
    Float_t         trk_cov44[3000];
    Float_t         trk_chi2[3000];
    Int_t           trk_ndof[3000];

    Int_t           trkHit_nHits;
    UInt_t          trkHit_id[3000];
    Int_t           trkHit_trackIndex[3000];
    Float_t         trkHit_posX[3000];
    Float_t         trkHit_posY[3000];
    Float_t         trkHit_posZ[3000];
    Float_t         trkHit_driftTime[3000];
    Float_t         trkHit_driftRadius[3000];
    Float_t         trkHit_error[3000];
    Float_t         trkHit_resi[3000];
    Float_t         trkHit_pull[3000];
    Int_t           trkHit_type[3000];

    Int_t           trkHole_nHoles;
    UInt_t          trkHole_id[3000];
    Int_t           trkHole_trackIndex[3000];
    Float_t         trkHole_posX[3000];
    Float_t         trkHole_posY[3000];
    Float_t         trkHole_posZ[3000];

    Int_t           trkSeg_nTrkSegs;
    UShort_t        trkSeg_trkIndex[3000];
    UShort_t        trkSeg_segIndex[3000];
    
    bool            hasTrkSegs;

    NTReaderBase_E(TTree *tree=0);               //!< default constructor, opening directory SegmentNtuple.root:/PatternNtupleMaker/Segments
    virtual ~NTReaderBase_E();                   //!< destructor
    virtual Int_t    Cut(Long64_t entry);        //!< empty routine
    virtual Int_t    GetEntry(Long64_t entry);   //!< Read contents of entry
    virtual Long64_t LoadTree(Long64_t entry);   //!< Set the environment to read one entry
    virtual void     Init(TTree *tree);          //!<The Init() function is called when the selector needs to initialize a new tree or chain.
    virtual void     Loop();                     //!< empty routine
    virtual void     Show(Long64_t entry = -1);  //!< Print contents of entry.

  };
} //namespace MuonCalib
#endif
