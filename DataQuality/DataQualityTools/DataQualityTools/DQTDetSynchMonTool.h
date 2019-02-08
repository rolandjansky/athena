/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DQTDetSynchMonTool.h
// PACKAGE:  DataQualityTools
//
// AUTHOR:   Luca Fiorini <Luca.Fiorini@cern.ch>
//	     
//
// ********************************************************************
#ifndef DQTDetSynchMonTool_H
#define DQTDetSynchMonTool_H

#include <set>
#include "GaudiKernel/ToolHandle.h"
#include <stdint.h>
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "DataQualityTools/DataQualityFatherMonTool.h"
#include "TH1.h"
#include "TH2.h"
#include "LWHists/TProfile_LW.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include "InDetRawData/InDetTimeCollection.h"
#include "TileEvent/TileDigitsContainer.h"
#include "LArRawEvent/LArFebHeaderContainer.h"
#include "MuonRDO/RpcPadContainer.h"

namespace Trk {
   //REL18 class IMagneticFieldTool; 
   //REL19 
   class IMagFieldSvc;
}

class TProfile;

class DQTDetSynchMonTool: public DataQualityFatherMonTool
{

 public:
  
  DQTDetSynchMonTool(const std::string & type, const std::string & name, const IInterface* parent);

  ~DQTDetSynchMonTool();

  StatusCode initialize();
    
  StatusCode bookHistograms( );
  //StatusCode bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );
  StatusCode fillHistograms();
  StatusCode procHistograms( );
  //StatusCode procHistograms( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun );
  StatusCode checkHists(bool fromFinalize);
  uint32_t findid(std::multiset<uint32_t>& mset);
  float findfrac(std::multiset<uint32_t>& mset, uint16_t ctpid);
  void clearMultisets();

  bool bookBCID();
  bool bookL1ID();
  bool bookBfield();


private:

  ServiceHandle<MagField::IMagFieldSvc> m_field;
  Int_t m_solenoidPositionX;
  Int_t m_solenoidPositionY;
  Int_t m_solenoidPositionZ;
  Int_t m_toroidPositionX;
  Int_t m_toroidPositionY;
  Int_t m_toroidPositionZ;

  TH1I_LW* m_CTP_BCID;
  TH1I_LW* m_SCT_BCID;
  TH1I_LW* m_TRT_BCID;
  TH1I_LW* m_LAR_BCID;
  TH1I_LW* m_Tile_BCID;
  TH1I_LW* m_RPC_BCID;
  TH1I_LW* m_Pixel_BCID;

  TH1I_LW *m_diff_Pixel_SCT_BCID;
  TH1I_LW *m_diff_Pixel_CTP_BCID;
  TH1I_LW *m_diff_Pixel_TRT_BCID;
  TH1I_LW *m_diff_Pixel_LAR_BCID;
  TH1I_LW *m_diff_Pixel_Tile_BCID;
  TH1I_LW *m_diff_Pixel_RPC_BCID;

  TH1I_LW *m_diff_Pixel_SCT_L1ID;
  TH1I_LW *m_diff_Pixel_CTP_L1ID;
  TH1I_LW *m_diff_Pixel_TRT_L1ID;
  TH1I_LW *m_diff_Pixel_LAR_L1ID;
  TH1I_LW *m_diff_Pixel_Tile_L1ID;
  TH1I_LW *m_diff_Pixel_RPC_L1ID;


  TH1I_LW* m_diff_CTP_SCT_BCID;
  TH1I_LW* m_diff_CTP_TRT_BCID;
  TH1I_LW* m_diff_CTP_LAR_BCID;
  TH1I_LW* m_diff_CTP_Tile_BCID;
  TH1I_LW* m_diff_CTP_RPC_BCID;

  TH1I_LW* m_diff_SCT_TRT_BCID;
  TH1I_LW* m_diff_SCT_LAR_BCID;
  TH1I_LW* m_diff_SCT_Tile_BCID;
  TH1I_LW* m_diff_SCT_RPC_BCID;
  TH1I_LW* m_diff_TRT_LAR_BCID;
  TH1I_LW* m_diff_TRT_Tile_BCID;
  TH1I_LW* m_diff_TRT_RPC_BCID;
  TH1I_LW* m_diff_LAR_Tile_BCID;
  TH1I_LW* m_diff_LAR_RPC_BCID;
  TH1I_LW* m_diff_Tile_RPC_BCID;

  TProfile* m_diff_CTP_SCT_BCID_lumi;
  TProfile* m_diff_CTP_TRT_BCID_lumi;
  TProfile* m_diff_CTP_LAR_BCID_lumi;
  TProfile* m_diff_CTP_Tile_BCID_lumi;
  TProfile* m_diff_CTP_RPC_BCID_lumi;
  TProfile* m_diff_CTP_Pixel_BCID_lumi;



  TH1I_LW* m_CTP_L1ID;
  TH1I_LW* m_SCT_L1ID;
  TH1I_LW* m_TRT_L1ID;
  TH1I_LW* m_LAR_L1ID;
  TH1I_LW* m_Tile_L1ID;
  TH1I_LW* m_RPC_L1ID;
  TH1I_LW* m_Pixel_L1ID;

  TH1I_LW* m_diff_CTP_SCT_L1ID;
  TH1I_LW* m_diff_CTP_TRT_L1ID;
  TH1I_LW* m_diff_CTP_LAR_L1ID;
  TH1I_LW* m_diff_CTP_Tile_L1ID;
  TH1I_LW* m_diff_CTP_RPC_L1ID;


  TH1I_LW* m_diff_SCT_TRT_L1ID;
  TH1I_LW* m_diff_SCT_LAR_L1ID;
  TH1I_LW* m_diff_SCT_Tile_L1ID;
  TH1I_LW* m_diff_SCT_RPC_L1ID;
  TH1I_LW* m_diff_TRT_LAR_L1ID;
  TH1I_LW* m_diff_TRT_Tile_L1ID;
  TH1I_LW* m_diff_TRT_RPC_L1ID;
  TH1I_LW* m_diff_LAR_Tile_L1ID;
  TH1I_LW* m_diff_LAR_RPC_L1ID;
  TH1I_LW* m_diff_Tile_RPC_L1ID;

  TProfile* m_diff_CTP_SCT_L1ID_lumi;
  TProfile* m_diff_CTP_TRT_L1ID_lumi;
  TProfile* m_diff_CTP_LAR_L1ID_lumi;
  TProfile* m_diff_CTP_Tile_L1ID_lumi;
  TProfile* m_diff_CTP_RPC_L1ID_lumi;
  TProfile* m_diff_CTP_Pixel_L1ID_lumi;


  TH1I *m_diff_Pixel_SCT_BCID_Rebin;
  TH1I *m_diff_Pixel_CTP_BCID_Rebin;
  TH1I *m_diff_Pixel_TRT_BCID_Rebin;
  TH1I *m_diff_Pixel_LAR_BCID_Rebin;
  TH1I *m_diff_Pixel_Tile_BCID_Rebin;
  TH1I *m_diff_Pixel_RPC_BCID_Rebin;


  TH1I *m_diff_Pixel_SCT_L1ID_Rebin;
  TH1I *m_diff_Pixel_CTP_L1ID_Rebin;
  TH1I *m_diff_Pixel_TRT_L1ID_Rebin;
  TH1I *m_diff_Pixel_LAR_L1ID_Rebin;
  TH1I *m_diff_Pixel_Tile_L1ID_Rebin;
  TH1I *m_diff_Pixel_RPC_L1ID_Rebin;

  TH1I* m_diff_SCT_TRT_BCID_Rebin;
  TH1I* m_diff_SCT_LAR_BCID_Rebin;
  TH1I* m_diff_SCT_Tile_BCID_Rebin;
  TH1I* m_diff_SCT_RPC_BCID_Rebin;
  TH1I* m_diff_TRT_LAR_BCID_Rebin;
  TH1I* m_diff_TRT_Tile_BCID_Rebin;
  TH1I* m_diff_TRT_RPC_BCID_Rebin;
  TH1I* m_diff_LAR_Tile_BCID_Rebin;
  TH1I* m_diff_LAR_RPC_BCID_Rebin;
  TH1I* m_diff_Tile_RPC_BCID_Rebin;


  TH1I* m_diff_CTP_SCT_BCID_Rebin;
  TH1I* m_diff_CTP_TRT_BCID_Rebin;
  TH1I* m_diff_CTP_LAR_BCID_Rebin;
  TH1I* m_diff_CTP_Tile_BCID_Rebin;
  TH1I* m_diff_CTP_RPC_BCID_Rebin;
      
  TH1I* m_diff_CTP_SCT_L1ID_Rebin;
  TH1I* m_diff_CTP_TRT_L1ID_Rebin;
  TH1I* m_diff_CTP_LAR_L1ID_Rebin;
  TH1I* m_diff_CTP_Tile_L1ID_Rebin;
  TH1I* m_diff_CTP_RPC_L1ID_Rebin;


  TH1I* m_diff_SCT_TRT_L1ID_Rebin;
  TH1I* m_diff_SCT_LAR_L1ID_Rebin;
  TH1I* m_diff_SCT_Tile_L1ID_Rebin;
  TH1I* m_diff_SCT_RPC_L1ID_Rebin;
  TH1I* m_diff_TRT_LAR_L1ID_Rebin;
  TH1I* m_diff_TRT_Tile_L1ID_Rebin;
  TH1I* m_diff_TRT_RPC_L1ID_Rebin;
  TH1I* m_diff_LAR_Tile_L1ID_Rebin;
  TH1I* m_diff_LAR_RPC_L1ID_Rebin;
  TH1I* m_diff_Tile_RPC_L1ID_Rebin;

  TH1I_LW* m_Bfield_solenoid;
  TH1I_LW* m_Bfield_toroid;

  TProfile_LW* m_Bfield_solenoid_vsLB;    
  TProfile_LW* m_Bfield_toroid_vsLB;    

  TH2I_LW* m_diff_BCID;
  TH2I_LW* m_diff_BCID_rate;
  TH2I_LW* m_diff_L1ID;

  std::multiset<uint32_t> m_sctbcidset;
  std::multiset<uint32_t> m_trtbcidset;
  std::multiset<uint32_t> m_larbcidset;
  std::multiset<uint32_t> m_tilebcidset;
  std::multiset<uint32_t> m_rpcbcidset;
  std::multiset<uint32_t> m_pixelbcidset;

  std::multiset<uint32_t> m_sctl1idset;
  std::multiset<uint32_t> m_trtl1idset;
  std::multiset<uint32_t> m_larl1idset;
  std::multiset<uint32_t> m_tilel1idset;
  std::multiset<uint32_t> m_rpcl1idset;
  std::multiset<uint32_t> m_pixell1idset;

  //int m_nevents;

  //int m_n_sct_nrobs;
  //int m_n_trt_nrobs;
  //int m_n_lar_nrobs;
  //int m_n_tile_nrobs;
  //int m_n_pixel_nrobs;
  

  //int m_n_sct_bcid_nrobs;
  //int m_n_trt_bcid_nrobs;
  //int m_n_sct_lvl1_nrobs;
  //int m_n_trt_lvl1_nrobs;
  //int m_n_pixel_bcid_nrobs;
  //int m_n_pixel_lvl1_nrobs;
  
  // Use these so we don't print out endless messages!
  bool m_printedErrorCTP_RIO;
  bool m_printedErrorTRT_BCID;
  bool m_printedErrorSCT_BCID;
  bool m_printedErrorPixel_BCID;

  bool m_printedErrorTRT_LVL1ID;
  bool m_printedErrorSCT_LVL1ID;
  bool m_printedErrorPixel_LVL1ID;

  bool m_printedErrorLAr;
  bool m_printedErrorTile;
  bool m_printedErrorTileCtr;
  bool m_printedErrorRPC;

  // detector indices
  //std::map<int, std::string>
  
  // storegate keys
  SG::ReadHandleKey<xAOD::EventInfo> m_EventInfoKey
    { "EventInfo" };
  SG::ReadHandleKeyArray<InDetTimeCollection> m_InDetTimeCollectionKeys
    { "TRT_BCID", "SCT_BCID", "PixelBCID", "TRT_LVL1ID", "SCT_LVL1ID", "PixelLVL1ID" } ;
  SG::ReadHandleKey<LArFebHeaderContainer> m_LArFebHeaderContainerKey
    { "LArFebHeader" };
  SG::ReadHandleKey<TileDigitsContainer> m_TileDigitsContainerKey
    { "TileDigitsFlt" };
  SG::ReadHandleKey<RpcPadContainer> m_RpcPadContainerKey
    { "RPCPAD" };
};

#endif
