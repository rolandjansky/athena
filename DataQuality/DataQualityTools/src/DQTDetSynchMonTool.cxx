/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DQTDetSynchMonTool.cxx
// PACKAGE:  DataQualityTools  
//
// AUTHORS:   Luca Fiorini <Luca.Fiorini@cern.ch>
//            Updated by:
//             Jahred Adelman (jahred.adelman@cern.ch)
//            and Max Baak (mbaakcern.ch)
//            and Sam King (samking@physics.ubc.ca)
//            and Simon Viel (svielcern.ch)
//
// ********************************************************************

#include "DataQualityTools/DQTDetSynchMonTool.h"
#include "TrigT1Result/CTP_RIO.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"

#include "TProfile.h"

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0) 
#   define CAN_REBIN(hist)  hist->SetCanExtend(TH1::kAllAxes)
#else
#   define CAN_REBIN(hist)  hist->SetBit(TH1::kCanRebin)
#endif

//----------------------------------------------------------------------------------
DQTDetSynchMonTool::DQTDetSynchMonTool(const std::string & type, 
                                       const std::string & name,
                                       const IInterface* parent)
   : DataQualityFatherMonTool(type, name, parent),
     m_field("AtlasFieldSvc", name),
     m_CTP_BCID(0),
     m_SCT_BCID(0),
     m_TRT_BCID(0),
     m_LAR_BCID(0),
     m_Tile_BCID(0),
     m_RPC_BCID(0),
     m_Pixel_BCID(0),
     m_diff_Pixel_SCT_BCID(0),
     m_diff_Pixel_CTP_BCID(0),
     m_diff_Pixel_TRT_BCID(0),
     m_diff_Pixel_LAR_BCID(0),
     m_diff_Pixel_Tile_BCID(0),
     m_diff_Pixel_RPC_BCID(0),
     m_diff_Pixel_SCT_L1ID(0),
     m_diff_Pixel_CTP_L1ID(0),
     m_diff_Pixel_TRT_L1ID(0),
     m_diff_Pixel_LAR_L1ID(0),
     m_diff_Pixel_Tile_L1ID(0),
     m_diff_Pixel_RPC_L1ID(0),
     m_diff_CTP_SCT_BCID(0),
     m_diff_CTP_TRT_BCID(0),
     m_diff_CTP_LAR_BCID(0),
     m_diff_CTP_Tile_BCID(0),
     m_diff_CTP_RPC_BCID(0),
     m_diff_SCT_TRT_BCID(0),
     m_diff_SCT_LAR_BCID(0),
     m_diff_SCT_Tile_BCID(0),
     m_diff_SCT_RPC_BCID(0),
     m_diff_TRT_LAR_BCID(0),
     m_diff_TRT_Tile_BCID(0),
     m_diff_TRT_RPC_BCID(0),
     m_diff_LAR_Tile_BCID(0),
     m_diff_LAR_RPC_BCID(0),
     m_diff_Tile_RPC_BCID(0),
     m_diff_CTP_SCT_BCID_lumi(0),
     m_diff_CTP_TRT_BCID_lumi(0),
     m_diff_CTP_LAR_BCID_lumi(0),
     m_diff_CTP_Tile_BCID_lumi(0),
     m_diff_CTP_RPC_BCID_lumi(0),
     m_diff_CTP_Pixel_BCID_lumi(0),
     m_CTP_L1ID(0),
     m_SCT_L1ID(0),
     m_TRT_L1ID(0),
     m_LAR_L1ID(0),
     m_Tile_L1ID(0),
     m_RPC_L1ID(0),
     m_Pixel_L1ID(0),
     m_diff_CTP_SCT_L1ID(0),
     m_diff_CTP_TRT_L1ID(0),
     m_diff_CTP_LAR_L1ID(0),
     m_diff_CTP_Tile_L1ID(0),
     m_diff_CTP_RPC_L1ID(0),
     m_diff_SCT_TRT_L1ID(0),
     m_diff_SCT_LAR_L1ID(0),
     m_diff_SCT_Tile_L1ID(0),
     m_diff_SCT_RPC_L1ID(0),
     m_diff_TRT_LAR_L1ID(0),
     m_diff_TRT_Tile_L1ID(0),
     m_diff_TRT_RPC_L1ID(0),
     m_diff_LAR_Tile_L1ID(0),
     m_diff_LAR_RPC_L1ID(0),
     m_diff_Tile_RPC_L1ID(0),
     m_diff_CTP_SCT_L1ID_lumi(0),
     m_diff_CTP_TRT_L1ID_lumi(0),
     m_diff_CTP_LAR_L1ID_lumi(0),
     m_diff_CTP_Tile_L1ID_lumi(0),
     m_diff_CTP_RPC_L1ID_lumi(0),
     m_diff_CTP_Pixel_L1ID_lumi(0),
     m_diff_Pixel_SCT_BCID_Rebin(0),
     m_diff_Pixel_CTP_BCID_Rebin(0),
     m_diff_Pixel_TRT_BCID_Rebin(0),
     m_diff_Pixel_LAR_BCID_Rebin(0),
     m_diff_Pixel_Tile_BCID_Rebin(0),
     m_diff_Pixel_RPC_BCID_Rebin(0),
     m_diff_Pixel_SCT_L1ID_Rebin(0),
     m_diff_Pixel_CTP_L1ID_Rebin(0),
     m_diff_Pixel_TRT_L1ID_Rebin(0),
     m_diff_Pixel_LAR_L1ID_Rebin(0),
     m_diff_Pixel_Tile_L1ID_Rebin(0),
     m_diff_Pixel_RPC_L1ID_Rebin(0),
     m_diff_SCT_TRT_BCID_Rebin(0),
     m_diff_SCT_LAR_BCID_Rebin(0),
     m_diff_SCT_Tile_BCID_Rebin(0),
     m_diff_SCT_RPC_BCID_Rebin(0),
     m_diff_TRT_LAR_BCID_Rebin(0),
     m_diff_TRT_Tile_BCID_Rebin(0),
     m_diff_TRT_RPC_BCID_Rebin(0),
     m_diff_LAR_Tile_BCID_Rebin(0),
     m_diff_LAR_RPC_BCID_Rebin(0),
     m_diff_Tile_RPC_BCID_Rebin(0),
     m_diff_CTP_SCT_BCID_Rebin(0),
     m_diff_CTP_TRT_BCID_Rebin(0),
     m_diff_CTP_LAR_BCID_Rebin(0),
     m_diff_CTP_Tile_BCID_Rebin(0),
     m_diff_CTP_RPC_BCID_Rebin(0),
     m_diff_CTP_SCT_L1ID_Rebin(0),
     m_diff_CTP_TRT_L1ID_Rebin(0),
     m_diff_CTP_LAR_L1ID_Rebin(0),
     m_diff_CTP_Tile_L1ID_Rebin(0),
     m_diff_CTP_RPC_L1ID_Rebin(0),
     m_diff_SCT_TRT_L1ID_Rebin(0),
     m_diff_SCT_LAR_L1ID_Rebin(0),
     m_diff_SCT_Tile_L1ID_Rebin(0),
     m_diff_SCT_RPC_L1ID_Rebin(0),
     m_diff_TRT_LAR_L1ID_Rebin(0),
     m_diff_TRT_Tile_L1ID_Rebin(0),
     m_diff_TRT_RPC_L1ID_Rebin(0),
     m_diff_LAR_Tile_L1ID_Rebin(0),
     m_diff_LAR_RPC_L1ID_Rebin(0),
     m_diff_Tile_RPC_L1ID_Rebin(0),
     m_Bfield_solenoid(0),
     m_Bfield_toroid(0),
     m_Bfield_solenoid_vsLB(0),    
     m_Bfield_toroid_vsLB(0),    
     m_diff_BCID(0),
     m_diff_BCID_rate(0), //rate plot; bcid diff relative to ctp
     m_diff_L1ID(0),
     //m_nevents(0),
     //m_n_sct_nrobs(0),
     //m_n_trt_nrobs(0),
     //m_n_lar_nrobs(0),
     //m_n_tile_nrobs(0),
     //m_n_pixel_nrobs(0),
     //m_n_sct_bcid_nrobs(0),
     //m_n_trt_bcid_nrobs(0),
     //m_n_sct_lvl1_nrobs(0),
     //m_n_trt_lvl1_nrobs(0),
     //m_n_pixel_bcid_nrobs(0),
     //m_n_pixel_lvl1_nrobs(0),
     m_printedErrorCTP_RIO(false),
     m_printedErrorTRT_BCID(false),
     m_printedErrorSCT_BCID(false),
     m_printedErrorPixel_BCID(false),
     m_printedErrorTRT_LVL1ID(false),
     m_printedErrorSCT_LVL1ID(false),
     m_printedErrorPixel_LVL1ID(false),
     m_printedErrorLAr(false),
     m_printedErrorTile(false),
     m_printedErrorTileCtr(false),
     m_printedErrorRPC(false)
{
   declareInterface<IMonitorToolBase>(this);
   m_path = "GLOBAL/DQTSynch",
     //declareProperty("doRunCosmics", m_doRunCosmics = 1);
     //declareProperty("doRunBeam", m_doRunBeam = 1);
     //declareProperty("doOfflineHists", m_doOfflineHists = 1);
     //declareProperty("doOnlineHists", m_doOnlineHists = 1);
   declareProperty("MagFieldTool", m_field);
   declareProperty("SolenoidPositionX", m_solenoidPositionX = 0);
   declareProperty("SolenoidPositionY", m_solenoidPositionY = 10);
   declareProperty("SolenoidPositionZ", m_solenoidPositionZ = 0);
   declareProperty("ToroidPositionX", m_toroidPositionX = 0);
   declareProperty("ToroidPositionY", m_toroidPositionY = 6000);
   declareProperty("ToroidPositionZ", m_toroidPositionZ = 0);
}

//----------------------------------------------------------------------------------
DQTDetSynchMonTool::~DQTDetSynchMonTool()
//----------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------
StatusCode DQTDetSynchMonTool::initialize() {
//----------------------------------------------------------------------------------
  ATH_CHECK( m_EventInfoKey.initialize() );
  ATH_CHECK( m_InDetTimeCollectionKeys.initialize() );
  ATH_CHECK( m_LArFebHeaderContainerKey.initialize() );
  ATH_CHECK( m_TileDigitsContainerKey.initialize() );
  ATH_CHECK( m_RpcPadContainerKey.initialize() );
  return DataQualityFatherMonTool::initialize();
}


//----------------------------------------------------------------------------------
StatusCode DQTDetSynchMonTool::bookHistograms( )
//StatusCode DQTDetSynchMonTool::bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun )
//----------------------------------------------------------------------------------
{
   bool failure(false);

   m_printedErrorCTP_RIO = false;
   m_printedErrorTRT_BCID = false;
   m_printedErrorSCT_BCID = false;
   m_printedErrorPixel_BCID = false;

   m_printedErrorTRT_LVL1ID = false;
   m_printedErrorSCT_LVL1ID = false;
   m_printedErrorPixel_LVL1ID = false;

   m_printedErrorLAr = false;
   m_printedErrorTile = false;
   m_printedErrorTileCtr = false;
   m_printedErrorRPC = false;     


   ////  if (isNewEventsBlock || isNewLumiBlock || isNewRun) {
   //if (newRun) {
      MsgStream log(msgSvc(), name());
  
      //log << MSG::DEBUG << "in bookHistograms() and m_doRunCosmics = " << m_doRunCosmics << " and m_doRunBeam = " << m_doRunBeam << endmsg;
      //log << MSG::DEBUG << "Using base path " << m_path << endmsg;
  
      failure = bookBCID(); 
      failure = failure || bookL1ID();
      failure = failure || bookBfield();
   //}
   //else if (newEventsBlock || newLumiBlock) {
   //   return StatusCode::SUCCESS;
   //}
   if (failure) {return  StatusCode::FAILURE;}
   else {return StatusCode::SUCCESS;}
}	


//----------------------------------------------------------------------------------
bool DQTDetSynchMonTool::bookBCID()
//----------------------------------------------------------------------------------
{
   bool failure(false);
   //  if (isNewEventsBlock || isNewLumiBlock || isNewRun) {
   MsgStream log(msgSvc(), name());
  
  
   std::string  fullPathBCID=m_path+"/BCID";

// Book summary plot

   failure = failure | registerHist(fullPathBCID, m_diff_BCID = TH2I_LW::create("m_BCID","BCID subdetector summary ", 7, 0.5, 7.5, 7, 0.5, 7.5)).isFailure();
   if (!failure) {
      m_diff_BCID->GetXaxis()->SetBinLabel(1, "CTP");
      m_diff_BCID->GetXaxis()->SetBinLabel(2, "SCT");
      m_diff_BCID->GetXaxis()->SetBinLabel(3, "TRT");
      m_diff_BCID->GetXaxis()->SetBinLabel(4, "LAr");
      m_diff_BCID->GetXaxis()->SetBinLabel(5, "Tile");
      m_diff_BCID->GetXaxis()->SetBinLabel(6, "RPC");
      m_diff_BCID->GetXaxis()->SetBinLabel(7, "Pixel");
      m_diff_BCID->GetYaxis()->SetBinLabel(1, "CTP");
      m_diff_BCID->GetYaxis()->SetBinLabel(2, "SCT");
      m_diff_BCID->GetYaxis()->SetBinLabel(3, "TRT");
      m_diff_BCID->GetYaxis()->SetBinLabel(4, "LAr");
      m_diff_BCID->GetYaxis()->SetBinLabel(5, "Tile");
      m_diff_BCID->GetYaxis()->SetBinLabel(6, "RPC");
      m_diff_BCID->GetYaxis()->SetBinLabel(7, "Pixel");
   }

   //Book rate plot
   failure = failure | registerHist(fullPathBCID, m_diff_BCID_rate = TH2I_LW::create("m_BCID_rate","BCID subdetector rate summary ", 6, 0.5, 6.5, 20, 0.0, 1.0)).isFailure();
   if (!failure) {
     m_diff_BCID_rate->GetXaxis()->SetBinLabel(1, "SCT");
     m_diff_BCID_rate->GetXaxis()->SetBinLabel(2, "TRT");
     m_diff_BCID_rate->GetXaxis()->SetBinLabel(3, "LAr");
     m_diff_BCID_rate->GetXaxis()->SetBinLabel(4, "Tile");
     m_diff_BCID_rate->GetXaxis()->SetBinLabel(5, "RPC");
     m_diff_BCID_rate->GetXaxis()->SetBinLabel(6, "Pixel");
   }
   
   // Booking subdetectors BCID values
   failure = failure | registerHist(fullPathBCID, m_CTP_BCID      = TH1I_LW::create("m_CTP_BCID", "BCID of CTP", 4096, -0.5, 4095.5)).isFailure();
   failure = failure | registerHist(fullPathBCID, m_SCT_BCID      = TH1I_LW::create("m_SCT_BCID", "BCID of SCT detector", 4096, -0.5, 4095.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_TRT_BCID      = TH1I_LW::create("m_TRT_BCID", "BCID of TRT detector", 4096, -0.5, 4095.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_LAR_BCID      = TH1I_LW::create("m_LAR_BCID", "BCID of LAR detector", 4096, -0.5, 4095.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_Tile_BCID      = TH1I_LW::create("m_Tile_BCID", "BCID of Tile detector", 4096, -0.5, 4095.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_RPC_BCID      = TH1I_LW::create("m_RPC_BCID", "BCID of RPC detector", 4096, -0.5, 4095.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_Pixel_BCID      = TH1I_LW::create("m_Pixel_BCID", "BCID of Pixel detector", 4096, -0.5, 4095.5)).isFailure();

   // Booking subdetectors BCID differences wrt CTP
   failure = failure |registerHist(fullPathBCID, m_diff_CTP_SCT_BCID  = TH1I_LW::create("m_diff_CTP_SCT_BCID", "BCID difference between CTP and SCT detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_diff_CTP_TRT_BCID = TH1I_LW::create("m_diff_CTP_TRT_BCID", "BCID difference between CTP and TRT detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_diff_CTP_LAR_BCID = TH1I_LW::create("m_diff_CTP_LAR_BCID", "BCID difference between CTP and LAR detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_diff_CTP_Tile_BCID  = TH1I_LW::create("m_diff_CTP_Tile_BCID", "BCID difference between CTP and Tile detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_diff_CTP_RPC_BCID  = TH1I_LW::create("m_diff_CTP_RPC_BCID", "BCID difference between CTP and RPC detectors", 51, -25.5, 25.5)).isFailure();


  
   // Booking subdetectors BCID differences wrt CTP as a function of LumiBlock
   if (!m_doOnlineHists) {  
      failure = failure |registerHist(fullPathBCID, m_diff_CTP_SCT_BCID_lumi  = new TProfile("m_diff_CTP_SCT_BCID_lumi", "BCID difference between CTP and SCT detectors as a function of the LumiBlock", 30, 0., 30,-4096.,4096)).isFailure();
      failure = failure |registerHist(fullPathBCID, m_diff_CTP_TRT_BCID_lumi = new TProfile("m_diff_CTP_TRT_BCID_lumi", "BCID difference between CTP and TRT detectors as a function of the LumiBlock", 30, 0., 30,-4096.,4096)).isFailure();
      failure = failure |registerHist(fullPathBCID, m_diff_CTP_LAR_BCID_lumi = new TProfile("m_diff_CTP_LAR_BCID_lumi", "BCID difference between CTP and LAR detectors as a function of the LumiBlock", 30, 0., 30,-4096.,4096)).isFailure();
      failure = failure |registerHist(fullPathBCID, m_diff_CTP_Tile_BCID_lumi  = new TProfile("m_diff_CTP_Tile_BCID_lumi", "BCID difference between CTP and Tile detectors as a function of the LumiBlock", 30, 0., 30,-4096.,4096 )).isFailure();
      failure = failure |registerHist(fullPathBCID, m_diff_CTP_RPC_BCID_lumi  = new TProfile("m_diff_CTP_RPC_BCID_lumi", "BCID difference between CTP and RPC detectors as a function of the LumiBlock", 30, 0., 30,-4096.,4096)).isFailure();
      failure = failure |registerHist(fullPathBCID, m_diff_CTP_Pixel_BCID_lumi  = new TProfile("m_diff_CTP_Pixel_BCID_lumi", "BCID difference between CTP and Pixel detectors as a function of the LumiBlock", 30, 0., 30,-4096.,4096)).isFailure();
      CAN_REBIN(m_diff_CTP_RPC_BCID_lumi);
      CAN_REBIN(m_diff_CTP_SCT_BCID_lumi);
      CAN_REBIN(m_diff_CTP_TRT_BCID_lumi);
      CAN_REBIN(m_diff_CTP_LAR_BCID_lumi);
      CAN_REBIN(m_diff_CTP_Tile_BCID_lumi);
      CAN_REBIN(m_diff_CTP_Pixel_BCID_lumi);
   }

   // Booking subdetectors BCID differences
   // Add those that were missing
   failure = failure |registerHist(fullPathBCID, m_diff_SCT_TRT_BCID = TH1I_LW::create("m_diff_SCT_TRT_BCID", "BCID difference between SCT and TRT detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_diff_SCT_LAR_BCID = TH1I_LW::create("m_diff_SCT_LAR_BCID", "BCID difference between SCT and LAR detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_diff_SCT_Tile_BCID  = TH1I_LW::create("m_diff_SCT_Tile_BCID", "BCID difference between SCT and Tile detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_diff_SCT_RPC_BCID  = TH1I_LW::create("m_diff_SCT_RPC_BCID", "BCID difference between SCT and RPC detectors", 51, -25.5, 25.5)).isFailure();

   failure = failure |registerHist(fullPathBCID, m_diff_TRT_LAR_BCID = TH1I_LW::create("m_diff_TRT_LAR_BCID", "BCID difference between TRT and LAR detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_diff_TRT_Tile_BCID  = TH1I_LW::create("m_diff_TRT_Tile_BCID", "BCID difference between TRT and Tile detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_diff_TRT_RPC_BCID  = TH1I_LW::create("m_diff_TRT_RPC_BCID", "BCID difference between TRT and RPC detectors", 51, -25.5, 25.5)).isFailure();

   failure = failure |registerHist(fullPathBCID, m_diff_LAR_Tile_BCID  = TH1I_LW::create("m_diff_LAR_Tile_BCID", "BCID difference between LAR and Tile detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_diff_LAR_RPC_BCID  = TH1I_LW::create("m_diff_LAR_RPC_BCID", "BCID difference between LAR and RPC detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_diff_Tile_RPC_BCID  = TH1I_LW::create("m_diff_Tile_RPC_BCID", "BCID difference between Tile and RPC detectors", 51, -25.5, 25.5)).isFailure();


   // Add new Pixel histograms
   failure = failure |registerHist(fullPathBCID, m_diff_Pixel_SCT_BCID  = TH1I_LW::create("m_diff_Pixel_SCT_BCID", "BCID difference between Pixel and SCT detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_diff_Pixel_TRT_BCID = TH1I_LW::create("m_diff_Pixel_TRT_BCID", "BCID difference between Pixel and TRT detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_diff_Pixel_LAR_BCID = TH1I_LW::create("m_diff_Pixel_LAR_BCID", "BCID difference between Pixel and LAR detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_diff_Pixel_Tile_BCID  = TH1I_LW::create("m_diff_Pixel_Tile_BCID", "BCID difference between Pixel and Tile detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_diff_Pixel_RPC_BCID  = TH1I_LW::create("m_diff_Pixel_RPC_BCID", "BCID difference between Pixel and RPC detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathBCID, m_diff_Pixel_CTP_BCID  = TH1I_LW::create("m_diff_Pixel_CTP_BCID", "BCID difference between Pixel and CTP", 51, -25.5, 25.5)).isFailure();

// Now book rebinned histograms if we are not online

   if (!m_doOnlineHists) {

      // Booking subdetectors BCID differences wrt CTP in resizable histograms  
      failure = failure |registerHist(fullPathBCID, m_diff_CTP_SCT_BCID_Rebin  = new TH1I("m_diff_CTP_SCT_BCID_Rebin", "BCID difference between CTP and SCT detectors. Full Range.", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_CTP_SCT_BCID_Rebin);
      failure = failure |registerHist(fullPathBCID, m_diff_CTP_TRT_BCID_Rebin = new TH1I("m_diff_CTP_TRT_BCID_Rebin", "BCID difference between CTP and TRT detectors. Full Range.", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_CTP_TRT_BCID_Rebin);
      failure = failure |registerHist(fullPathBCID, m_diff_CTP_LAR_BCID_Rebin = new TH1I("m_diff_CTP_LAR_BCID_Rebin", "BCID difference between CTP and LAR detectors. Full Range.", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_CTP_LAR_BCID_Rebin);
      failure = failure |registerHist(fullPathBCID, m_diff_CTP_Tile_BCID_Rebin  = new TH1I("m_diff_CTP_Tile_BCID_Rebin", "BCID difference between CTP and Tile detectors. Full Range.", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_CTP_Tile_BCID_Rebin);
      failure = failure |registerHist(fullPathBCID, m_diff_CTP_RPC_BCID_Rebin  = new TH1I("m_diff_CTP_RPC_BCID_Rebin", "BCID difference between CTP and RPC detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_CTP_RPC_BCID_Rebin);
     
     
      // Booking subdetectors BCID differences in resizable histograms, these were missing
      failure = failure |registerHist(fullPathBCID, m_diff_SCT_TRT_BCID_Rebin = new TH1I("m_diff_SCT_TRT_BCID_Rebin", "BCID difference between SCT and TRT detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_SCT_TRT_BCID_Rebin);
     
      failure = failure |registerHist(fullPathBCID, m_diff_SCT_LAR_BCID_Rebin = new TH1I("m_diff_SCT_LAR_BCID_Rebin", "BCID difference between SCT and LAR detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_SCT_LAR_BCID_Rebin);
     
      failure = failure |registerHist(fullPathBCID, m_diff_SCT_Tile_BCID_Rebin  = new TH1I("m_diff_SCT_Tile_BCID_Rebin", "BCID difference between SCT and Tile detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_SCT_TRT_BCID_Rebin);
     
      failure = failure |registerHist(fullPathBCID, m_diff_SCT_RPC_BCID_Rebin  = new TH1I("m_diff_SCT_RPC_BCID_Rebin", "BCID difference between SCT and RPC detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_SCT_RPC_BCID_Rebin);
      failure = failure |registerHist(fullPathBCID, m_diff_TRT_LAR_BCID_Rebin = new TH1I("m_diff_TRT_LAR_BCID_Rebin", "BCID difference between TRT and LAR detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_TRT_LAR_BCID_Rebin);
      failure = failure |registerHist(fullPathBCID, m_diff_TRT_Tile_BCID_Rebin  = new TH1I("m_diff_TRT_Tile_BCID_Rebin", "BCID difference between TRT and Tile detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_TRT_Tile_BCID_Rebin);
      failure = failure |registerHist(fullPathBCID, m_diff_TRT_RPC_BCID_Rebin  = new TH1I("m_diff_TRT_RPC_BCID_Rebin", "BCID difference between TRT and RPC detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_TRT_RPC_BCID_Rebin);
      failure = failure |registerHist(fullPathBCID, m_diff_LAR_Tile_BCID_Rebin  = new TH1I("m_diff_LAR_Tile_BCID_Rebin", "BCID difference between LAR and Tile detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_LAR_Tile_BCID_Rebin);
      failure = failure |registerHist(fullPathBCID, m_diff_LAR_RPC_BCID_Rebin  = new TH1I("m_diff_LAR_RPC_BCID_Rebin", "BCID difference between LAR and RPC detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_LAR_RPC_BCID_Rebin);
      failure = failure |registerHist(fullPathBCID, m_diff_Tile_RPC_BCID_Rebin  = new TH1I("m_diff_Tile_RPC_BCID_Rebin", "BCID difference between Tile and RPC detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_Tile_RPC_BCID_Rebin);
     
          
      failure = failure |registerHist(fullPathBCID, m_diff_Pixel_SCT_BCID_Rebin  = new TH1I("m_diff_Pixel_SCT_BCID_Rebin", "BCID difference between Pixel and SCT detectors. Full Range.", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_Pixel_SCT_BCID_Rebin);
      failure = failure |registerHist(fullPathBCID, m_diff_Pixel_TRT_BCID_Rebin = new TH1I("m_diff_Pixel_TRT_BCID_Rebin", "BCID difference between Pixel and TRT detectors. Full Range.", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_Pixel_TRT_BCID_Rebin);
      failure = failure |registerHist(fullPathBCID, m_diff_Pixel_LAR_BCID_Rebin = new TH1I("m_diff_Pixel_LAR_BCID_Rebin", "BCID difference between Pixel and LAR detectors. Full Range.", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_Pixel_LAR_BCID_Rebin);
      failure = failure |registerHist(fullPathBCID, m_diff_Pixel_Tile_BCID_Rebin  = new TH1I("m_diff_Pixel_Tile_BCID_Rebin", "BCID difference between Pixel and Tile detectors. Full Range.", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_Pixel_Tile_BCID_Rebin);
      failure = failure |registerHist(fullPathBCID, m_diff_Pixel_RPC_BCID_Rebin  = new TH1I("m_diff_Pixel_RPC_BCID_Rebin", "BCID difference between Pixel and RPC detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_Pixel_RPC_BCID_Rebin);
      failure = failure |registerHist(fullPathBCID, m_diff_Pixel_CTP_BCID_Rebin  = new TH1I("m_diff_Pixel_CTP_BCID_Rebin", "BCID difference between Pixel and CTP detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_Pixel_CTP_BCID_Rebin);
   }
  
  
   if (failure) 
   {
      log << MSG::WARNING << "Error Booking BCID histograms " << endmsg;
   }
   return failure;

}
		

//----------------------------------------------------------------------------------
bool DQTDetSynchMonTool::bookL1ID()
//----------------------------------------------------------------------------------
{
   bool failure(false);
   MsgStream log(msgSvc(), name());
  
   std::string  fullPathL1ID=m_path+"/L1ID";

   failure = failure | registerHist(fullPathL1ID, m_diff_L1ID = TH2I_LW::create("m_L1ID","L1ID subdetector summary ", 7, 0.5, 7.5, 7, 0.5, 7.5)).isFailure();
   if (!failure) {
      m_diff_L1ID->GetXaxis()->SetBinLabel(1, "CTP");
      m_diff_L1ID->GetXaxis()->SetBinLabel(2, "SCT");
      m_diff_L1ID->GetXaxis()->SetBinLabel(3, "TRT");
      m_diff_L1ID->GetXaxis()->SetBinLabel(4, "LAr");
      m_diff_L1ID->GetXaxis()->SetBinLabel(5, "Tile");
      m_diff_L1ID->GetXaxis()->SetBinLabel(6, "RPC");
      m_diff_L1ID->GetXaxis()->SetBinLabel(7, "Pixel");
      m_diff_L1ID->GetYaxis()->SetBinLabel(1, "CTP");
      m_diff_L1ID->GetYaxis()->SetBinLabel(2, "SCT");
      m_diff_L1ID->GetYaxis()->SetBinLabel(3, "TRT");
      m_diff_L1ID->GetYaxis()->SetBinLabel(4, "LAr");
      m_diff_L1ID->GetYaxis()->SetBinLabel(5, "Tile");
      m_diff_L1ID->GetYaxis()->SetBinLabel(6, "RPC");
      m_diff_L1ID->GetYaxis()->SetBinLabel(7, "Pixel");
   }


   // Booking subdetectors L1ID values
   failure = failure |registerHist(fullPathL1ID, m_CTP_L1ID      = TH1I_LW::create("m_CTP_L1ID", "L1ID of CTP detector", 10000, -0.5, 9999.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_SCT_L1ID      = TH1I_LW::create("m_SCT_L1ID", "L1ID of SCT detector", 1000, -0.5, 9999.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_TRT_L1ID      = TH1I_LW::create("m_TRT_L1ID", "L1ID of TRT detector", 1000, -0.5, 9999.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_LAR_L1ID      = TH1I_LW::create("m_LAR_L1ID", "L1ID of LAR detector", 1000, -0.5, 9999.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_Tile_L1ID      = TH1I_LW::create("m_Tile_L1ID", "L1ID of Tile detector", 1000, -0.5, 9999.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_RPC_L1ID      = TH1I_LW::create("m_RPC_L1ID", "L1ID of RPC detector", 1000, -0.5, 9999.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_Pixel_L1ID      = TH1I_LW::create("m_Pixel_L1ID", "L1ID of Pixel detector", 1000, -0.5, 9999.5)).isFailure();

   // Booking subdetectors L1ID differences wrt CTP in fixed range histograms
   failure = failure |registerHist(fullPathL1ID, m_diff_CTP_SCT_L1ID  = TH1I_LW::create("m_diff_CTP_SCT_L1ID", "L1ID difference between CTP and SCT detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_diff_CTP_TRT_L1ID = TH1I_LW::create("m_diff_CTP_TRT_L1ID", "L1ID difference between CTP and TRT detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_diff_CTP_LAR_L1ID = TH1I_LW::create("m_diff_CTP_LAR_L1ID", "L1ID difference between CTP and LAR detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_diff_CTP_Tile_L1ID  = TH1I_LW::create("m_diff_CTP_Tile_L1ID", "L1ID difference between CTP and Tile detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_diff_CTP_RPC_L1ID  = TH1I_LW::create("m_diff_CTP_RPC_L1ID", "L1ID difference between CTP and RPC detectors", 51, -25.5, 25.5)).isFailure();
    
  
   // Booking subdetectors L1ID differences as a function of the lumiblock
   if (!m_doOnlineHists) {
      failure = failure |registerHist(fullPathL1ID, m_diff_CTP_SCT_L1ID_lumi  = new TProfile("m_diff_CTP_SCT_L1ID_lumi", "L1ID difference between CTP and SCT detectors as a function of the LumiBlock", 30, 0., 30,-4096.,4096)).isFailure();
      failure = failure |registerHist(fullPathL1ID, m_diff_CTP_TRT_L1ID_lumi = new TProfile("m_diff_CTP_TRT_L1ID_lumi", "L1ID difference between CTP and TRT detectors as a function of the LumiBlock", 30, 0., 30,-4096.,4096)).isFailure();
      failure = failure |registerHist(fullPathL1ID, m_diff_CTP_LAR_L1ID_lumi = new TProfile("m_diff_CTP_LAR_L1ID_lumi", "L1ID difference between CTP and LAR detectors as a function of the LumiBlock", 30, 0., 30,-4096.,4096)).isFailure();
      failure = failure |registerHist(fullPathL1ID, m_diff_CTP_Tile_L1ID_lumi  = new TProfile("m_diff_CTP_Tile_L1ID_lumi", "L1ID difference between CTP and Tile detectors as a function of the LumiBlock", 30, 0., 30,-4096.,4096 )).isFailure();
      failure = failure |registerHist(fullPathL1ID, m_diff_CTP_RPC_L1ID_lumi  = new TProfile("m_diff_CTP_RPC_L1ID_lumi", "L1ID difference between CTP and RPC detectors as a function of the LumiBlock", 30, 0., 30,-4096.,4096)).isFailure();
      failure = failure |registerHist(fullPathL1ID, m_diff_CTP_Pixel_L1ID_lumi  = new TProfile("m_diff_CTP_Pixel_L1ID_lumi", "L1ID difference between CTP and Pixel detectors as a function of the LumiBlock", 30, 0., 30,-4096.,4096)).isFailure();     
      CAN_REBIN(m_diff_CTP_SCT_L1ID_lumi);
      CAN_REBIN(m_diff_CTP_TRT_L1ID_lumi);
      CAN_REBIN(m_diff_CTP_LAR_L1ID_lumi);
      CAN_REBIN(m_diff_CTP_Tile_L1ID_lumi);
      CAN_REBIN(m_diff_CTP_Pixel_L1ID_lumi);
      CAN_REBIN(m_diff_CTP_RPC_L1ID_lumi);
   }

   // Add those that were missing
   failure = failure |registerHist(fullPathL1ID, m_diff_SCT_TRT_L1ID = TH1I_LW::create("m_diff_SCT_TRT_L1ID", "L1ID difference between SCT and TRT detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_diff_SCT_LAR_L1ID = TH1I_LW::create("m_diff_SCT_LAR_L1ID", "L1ID difference between SCT and LAR detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_diff_SCT_Tile_L1ID  = TH1I_LW::create("m_diff_SCT_Tile_L1ID", "L1ID difference between SCT and Tile detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_diff_SCT_RPC_L1ID  = TH1I_LW::create("m_diff_SCT_RPC_L1ID", "L1ID difference between SCT and RPC detectors", 51, -25.5, 25.5)).isFailure();

   failure = failure |registerHist(fullPathL1ID, m_diff_TRT_LAR_L1ID = TH1I_LW::create("m_diff_TRT_LAR_L1ID", "L1ID difference between TRT and LAR detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_diff_TRT_Tile_L1ID  = TH1I_LW::create("m_diff_TRT_Tile_L1ID", "L1ID difference between TRT and Tile detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_diff_TRT_RPC_L1ID  = TH1I_LW::create("m_diff_TRT_RPC_L1ID", "L1ID difference between TRT and RPC detectors", 51, -25.5, 25.5)).isFailure();

   failure = failure |registerHist(fullPathL1ID, m_diff_LAR_Tile_L1ID  = TH1I_LW::create("m_diff_LAR_Tile_L1ID", "L1ID difference between LAR and Tile detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_diff_LAR_RPC_L1ID  = TH1I_LW::create("m_diff_LAR_RPC_L1ID", "L1ID difference between LAR and RPC detectors", 51, -25.5, 25.5)).isFailure();

   failure = failure |registerHist(fullPathL1ID, m_diff_Tile_RPC_L1ID  = TH1I_LW::create("m_diff_Tile_RPC_L1ID", "L1ID difference between Tile and RPC detectors", 51, -25.5, 25.5)).isFailure();


// Add new Pixel histograms
   failure = failure |registerHist(fullPathL1ID, m_diff_Pixel_SCT_L1ID  = TH1I_LW::create("m_diff_Pixel_SCT_L1ID", "L1ID difference between Pixel and SCT detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_diff_Pixel_TRT_L1ID = TH1I_LW::create("m_diff_Pixel_TRT_L1ID", "L1ID difference between Pixel and TRT detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_diff_Pixel_LAR_L1ID = TH1I_LW::create("m_diff_Pixel_LAR_L1ID", "L1ID difference between Pixel and LAR detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_diff_Pixel_Tile_L1ID  = TH1I_LW::create("m_diff_Pixel_Tile_L1ID", "L1ID difference between Pixel and Tile detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_diff_Pixel_RPC_L1ID  = TH1I_LW::create("m_diff_Pixel_RPC_L1ID", "L1ID difference between Pixel and RPC detectors", 51, -25.5, 25.5)).isFailure();
   failure = failure |registerHist(fullPathL1ID, m_diff_Pixel_CTP_L1ID  = TH1I_LW::create("m_diff_Pixel_CTP_L1ID", "L1ID difference between Pixel and CTP", 51, -25.5, 25.5)).isFailure();
  
  
   // Booking subdetectors L1ID differences in resizable histograms, these were missing
   if (!m_doOnlineHists) {
     
      // Booking subdetectors L1ID differences wrt CTP in resizable histograms
      failure = failure |registerHist(fullPathL1ID, m_diff_CTP_SCT_L1ID_Rebin  = new TH1I("m_diff_CTP_SCT_L1ID_Rebin", "L1ID difference between CTP and SCT detectors. Full Range.", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_CTP_SCT_L1ID_Rebin);
      failure = failure |registerHist(fullPathL1ID, m_diff_CTP_TRT_L1ID_Rebin = new TH1I("m_diff_CTP_TRT_L1ID_Rebin", "L1ID difference between CTP and TRT detectors. Full Range.", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_CTP_TRT_L1ID_Rebin);
      failure = failure |registerHist(fullPathL1ID, m_diff_CTP_LAR_L1ID_Rebin = new TH1I("m_diff_CTP_LAR_L1ID_Rebin", "L1ID difference between CTP and LAR detectors. Full Range.", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_CTP_LAR_L1ID_Rebin);
      failure = failure |registerHist(fullPathL1ID, m_diff_CTP_Tile_L1ID_Rebin  = new TH1I("m_diff_CTP_Tile_L1ID_Rebin", "L1ID difference between CTP and Tile detectors. Full Range.", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_CTP_Tile_L1ID_Rebin);
      failure = failure |registerHist(fullPathL1ID, m_diff_CTP_RPC_L1ID_Rebin  = new TH1I("m_diff_CTP_RPC_L1ID_Rebin", "L1ID difference between CTP and RPC detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_CTP_RPC_L1ID_Rebin);
     

      failure = failure |registerHist(fullPathL1ID, m_diff_SCT_TRT_L1ID_Rebin = new TH1I("m_diff_SCT_TRT_L1ID_Rebin", "L1ID difference between SCT and TRT detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_SCT_TRT_L1ID_Rebin);
     
      failure = failure |registerHist(fullPathL1ID, m_diff_SCT_LAR_L1ID_Rebin = new TH1I("m_diff_SCT_LAR_L1ID_Rebin", "L1ID difference between SCT and LAR detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_SCT_LAR_L1ID_Rebin);
      failure = failure |registerHist(fullPathL1ID, m_diff_SCT_Tile_L1ID_Rebin  = new TH1I("m_diff_SCT_Tile_L1ID_Rebin", "L1ID difference between SCT and Tile detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_SCT_Tile_L1ID_Rebin);
      failure = failure |registerHist(fullPathL1ID, m_diff_SCT_RPC_L1ID_Rebin  = new TH1I("m_diff_SCT_RPC_L1ID_Rebin", "L1ID difference between SCT and RPC detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_SCT_RPC_L1ID_Rebin);
      failure = failure |registerHist(fullPathL1ID, m_diff_TRT_LAR_L1ID_Rebin = new TH1I("m_diff_TRT_LAR_L1ID_Rebin", "L1ID difference between TRT and LAR detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_TRT_LAR_L1ID_Rebin);
      failure = failure |registerHist(fullPathL1ID, m_diff_TRT_Tile_L1ID_Rebin  = new TH1I("m_diff_TRT_Tile_L1ID_Rebin", "L1ID difference between TRT and Tile detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_TRT_Tile_L1ID_Rebin);
      failure = failure |registerHist(fullPathL1ID, m_diff_TRT_RPC_L1ID_Rebin  = new TH1I("m_diff_TRT_RPC_L1ID_Rebin", "L1ID difference between TRT and RPC detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_TRT_RPC_L1ID_Rebin);
      failure = failure |registerHist(fullPathL1ID, m_diff_LAR_Tile_L1ID_Rebin  = new TH1I("m_diff_LAR_Tile_L1ID_Rebin", "L1ID difference between LAR and Tile detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_LAR_Tile_L1ID_Rebin);
      failure = failure |registerHist(fullPathL1ID, m_diff_LAR_RPC_L1ID_Rebin  = new TH1I("m_diff_LAR_RPC_L1ID_Rebin", "L1ID difference between LAR and RPC detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_LAR_RPC_L1ID_Rebin);
      failure = failure |registerHist(fullPathL1ID, m_diff_Tile_RPC_L1ID_Rebin  = new TH1I("m_diff_Tile_RPC_L1ID_Rebin", "L1ID difference between Tile and RPC detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_Tile_RPC_L1ID_Rebin);
     
      failure = failure |registerHist(fullPathL1ID, m_diff_Pixel_SCT_L1ID_Rebin  = new TH1I("m_diff_Pixel_SCT_L1ID_Rebin", "L1ID difference between Pixel and SCT detectors. Full Range.", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_Pixel_SCT_L1ID_Rebin);
      failure = failure |registerHist(fullPathL1ID, m_diff_Pixel_TRT_L1ID_Rebin = new TH1I("m_diff_Pixel_TRT_L1ID_Rebin", "L1ID difference between Pixel and TRT detectors. Full Range.", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_Pixel_TRT_L1ID_Rebin);
      failure = failure |registerHist(fullPathL1ID, m_diff_Pixel_LAR_L1ID_Rebin = new TH1I("m_diff_Pixel_LAR_L1ID_Rebin", "L1ID difference between Pixel and LAR detectors. Full Range.", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_Pixel_LAR_L1ID_Rebin);
      failure = failure |registerHist(fullPathL1ID, m_diff_Pixel_Tile_L1ID_Rebin  = new TH1I("m_diff_Pixel_Tile_L1ID_Rebin", "L1ID difference between Pixel and Tile detectors. Full Range.", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_Pixel_Tile_L1ID_Rebin);
      failure = failure |registerHist(fullPathL1ID, m_diff_Pixel_RPC_L1ID_Rebin  = new TH1I("m_diff_Pixel_RPC_L1ID_Rebin", "L1ID difference between Pixel and RPC detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_Pixel_RPC_L1ID_Rebin);
      failure = failure |registerHist(fullPathL1ID, m_diff_Pixel_CTP_L1ID_Rebin  = new TH1I("m_diff_Pixel_CTP_L1ID_Rebin", "L1ID difference between Pixel and CTP detectors. Full Range", 51, -25.5, 25.5)).isFailure();
      CAN_REBIN(m_diff_Pixel_CTP_L1ID_Rebin);
     
   }

   if (failure) 
   {
      log << MSG::WARNING << "Error Booking L1ID histograms " << endmsg;
   }
   return failure;
  
}	

//----------------------------------------------------------------------------------
bool DQTDetSynchMonTool::bookBfield()
//----------------------------------------------------------------------------------
{
   bool failure(false);
   MsgStream log(msgSvc(), name());
  
   std::string  fullPathBfield=m_path+"/Bfield";
    
   failure = failure |registerHist(fullPathBfield, m_Bfield_solenoid  = TH1I_LW::create("m_Bfield_solenoid", "Bz of Solenoid", 50, -1, 4)).isFailure();
   failure = failure |registerHist(fullPathBfield, m_Bfield_toroid  = TH1I_LW::create("m_Bfield_toroid", "Bx of Toroid", 50, -1, 4)).isFailure();

   failure = failure |registerHist(fullPathBfield, m_Bfield_solenoid_vsLB  = TProfile_LW::create("m_Bfield_solenoid_vsLB","m_Bfield_solenoid_vsLB", 1500, -0.5, 1499.5)).isFailure();
   failure = failure |registerHist(fullPathBfield, m_Bfield_toroid_vsLB  = TProfile_LW::create("m_Bfield_toroid_vsLB","m_Bfield_toroid_vsLB", 1500, -0.5, 1499.5)).isFailure();

   if (failure) 
   {
      log << MSG::WARNING << "Error Booking L1ID histograms " << endmsg;
   }
   return failure;
}    



void DQTDetSynchMonTool::clearMultisets() {

   m_sctbcidset.clear();
   m_trtbcidset.clear();
   m_larbcidset.clear();
   m_tilebcidset.clear();
   m_rpcbcidset.clear();
   m_sctl1idset.clear();
   m_trtl1idset.clear();
   m_larl1idset.clear();
   m_tilel1idset.clear();
   m_rpcl1idset.clear();
   m_pixelbcidset.clear();
   m_pixell1idset.clear();
}

//----------------------------------------------------------------------------------
StatusCode DQTDetSynchMonTool::fillHistograms()
//----------------------------------------------------------------------------------
{
   MsgStream log(msgSvc(), name());

   //  log << MSG::DEBUG << "in fillHists()" << endmsg;

   clearMultisets(); // clear previous event entries

   uint16_t sctbcid(0), trtbcid(0), larbcid(0), tilebcid(0), rpcbcid(0), pixelbcid(0);
   uint32_t sctl1id(0), trtl1id(0), larl1id(0), tilel1id(0), rpcl1id(0), pixell1id(0);  

   float sctfrac(0.0), trtfrac(0.0), larfrac(0.0), tilefrac(0.0), rpcfrac(0.0), pixelfrac(0.0);

   uint16_t ctpbcid(9999);
   uint32_t ctpl1id(9999999);

   uint32_t lumi(0), evtNum(0);
   
   //Retrieve CTP, other things from EventInfo
   SG::ReadHandle<xAOD::EventInfo> thisEventInfo(m_EventInfoKey);
   if(! thisEventInfo.isValid())
   {
     ATH_MSG_WARNING( "Could not find EventInfo in evtStore" );
   }
   else
   {
     ctpbcid = thisEventInfo->bcid();
     lumi    = thisEventInfo->lumiBlock() ;
     evtNum  = thisEventInfo->eventNumber();
     ctpl1id = thisEventInfo->extendedLevel1ID();
   }

   auto inDetTimeCollections = m_InDetTimeCollectionKeys.makeHandles();

   if (inDetTimeCollections[0].isValid()) {
     auto& TRT_BCIDColl(inDetTimeCollections[0]);
     for ( InDetTimeCollection::const_iterator itrt_bcid 
	     = TRT_BCIDColl->begin();
	   itrt_bcid != TRT_BCIDColl->end(); ++itrt_bcid ) {
       //log << MSG::DEBUG << "TRT BCID pointer found! " << endmsg;
       if (!(*itrt_bcid)) continue;
       //Current bcid
       m_trtbcidset.insert((uint16_t)(*itrt_bcid)->second);
       //log << MSG::DEBUG << "TRT BCID found " << (*itrt_bcid)->second  << endmsg;      
       
     } // End for loop
   }
   else {
     ATH_MSG_WARNING( "Could not get any TRT_BCID containers " );
   }

   
   if (inDetTimeCollections[1].isValid()) {
     auto& SCT_BCIDColl(inDetTimeCollections[1]);
     for ( InDetTimeCollection::const_iterator isct_bcid 
	     = SCT_BCIDColl->begin();
	   isct_bcid != SCT_BCIDColl->end(); ++isct_bcid ) {
       //log << MSG::DEBUG << "SCT BCID pointer found! " << endmsg;
       if (!(*isct_bcid)) continue;
       //Current bcid
       m_sctbcidset.insert((uint16_t)(*isct_bcid)->second);
       //log << MSG::DEBUG << "SCT BCID found " << (*isct_bcid)->second  << endmsg;            
     } // End for loop
   }
   else {
     ATH_MSG_WARNING( "Could not get any SCT_BCID containers " );
   }
     

   if (inDetTimeCollections[2].isValid()) {
     auto& Pixel_BCIDColl(inDetTimeCollections[2]);
     for ( InDetTimeCollection::const_iterator ipixel_bcid 
	     = Pixel_BCIDColl->begin();
	   ipixel_bcid != Pixel_BCIDColl->end(); ++ipixel_bcid ) {
       //log << MSG::DEBUG << "Pixel BCID pointer found! " << endmsg;
       if (!(*ipixel_bcid)) continue;
       //Current bcid
       m_pixelbcidset.insert((uint16_t)(*ipixel_bcid)->second);
       //log << MSG::DEBUG << "Pixel BCID found " << (*ipixel_bcid)->second  << endmsg;            
     } // End for loop
   }
   else {
     ATH_MSG_WARNING( "Could not get any Pixel_BCID containers " );
   }


   if (inDetTimeCollections[3].isValid()) {
     auto& TRT_LVL1IDColl(inDetTimeCollections[3]);
     for ( InDetTimeCollection::const_iterator itrt_lvl1id 
	     = TRT_LVL1IDColl->begin();
	   itrt_lvl1id != TRT_LVL1IDColl->end(); ++itrt_lvl1id ) {
       //log << MSG::DEBUG << "TRT LVL1 ID pointer found! " << endmsg;
       if (!(*itrt_lvl1id)) continue;
       //Current lvl1id
       m_trtl1idset.insert((uint16_t)(*itrt_lvl1id)->second);
       //log << MSG::DEBUG << "TRT LVL1 ID found " << (*itrt_lvl1id)->second  << endmsg;      
     } // End for loop
   }
   else {
     ATH_MSG_WARNING( "Could not get TRT_LVL1ID container " );
   }


   if (inDetTimeCollections[4].isValid()) {
     auto& SCT_LVL1IDColl(inDetTimeCollections[4]);
     for ( InDetTimeCollection::const_iterator isct_lvl1id 
	     = SCT_LVL1IDColl->begin();
	   isct_lvl1id != SCT_LVL1IDColl->end(); ++isct_lvl1id ) {
       //log << MSG::DEBUG << "SCT LVL1 ID pointer found! " << endmsg;
       if (!(*isct_lvl1id)) continue;
       //Current lvl1id
       m_sctl1idset.insert((uint16_t)(*isct_lvl1id)->second);
       //log << MSG::DEBUG << "SCT LVL1 ID found " << (*isct_lvl1id)->second  << endmsg;            
     } // End for loop
   }
   else {
     ATH_MSG_WARNING( "Could not get SCT_LVL1ID container " );
   }
  

   if (inDetTimeCollections[5].isValid()) {
     auto& Pixel_LVL1IDColl(inDetTimeCollections[5]);
     for ( InDetTimeCollection::const_iterator ipixel_lvl1id 
	     = Pixel_LVL1IDColl->begin();
	   ipixel_lvl1id != Pixel_LVL1IDColl->end(); ++ipixel_lvl1id ) {
       //log << MSG::DEBUG << "Pixel LVL1 ID pointer found! " << endmsg;
       if (!(*ipixel_lvl1id)) continue;
       //Current lvl1id
       m_pixell1idset.insert((uint16_t)(*ipixel_lvl1id)->second);
       //log << MSG::DEBUG << "Pixel LVL1 ID found " << (*ipixel_lvl1id)->second  << endmsg;            
     } // End for loop
   }
   else {
     ATH_MSG_WARNING( "Could not get Pixel_LVL1ID container " );
   }


   sctbcid=findid(m_sctbcidset);
   sctfrac=findfrac(m_sctbcidset,ctpbcid);
   trtbcid=findid(m_trtbcidset);
   trtfrac=findfrac(m_trtbcidset,ctpbcid);
   sctl1id=findid(m_sctl1idset);
   trtl1id=findid(m_trtl1idset);
   pixell1id=findid(m_pixell1idset);
   pixelbcid=findid(m_pixelbcidset);
   pixelfrac=findfrac(m_pixelbcidset,ctpbcid);
   
   SG::ReadHandle<LArFebHeaderContainer> hdrCont(m_LArFebHeaderContainerKey);
   if (! hdrCont.isValid()) {
      ATH_MSG_WARNING( "No LArFEB container found in TDS" );
   }
   else {
      //log << MSG::DEBUG << "LArFEB container found" <<endmsg; 
      LArFebHeaderContainer::const_iterator it = hdrCont->begin(); 
      LArFebHeaderContainer::const_iterator itend = hdrCont->end(); 
      for ( ; it!=itend;++it) {
	//HWIdentifier febid=(*it)->FEBId();
        unsigned int febid=((*it)->FEBId()).get_identifier32().get_compact();
         if (febid >= 0x38000000 && febid <= 0x3bc60000) {
            m_larbcidset.insert((uint16_t) (*it)->BCId() );
            m_larl1idset.insert((uint32_t) (*it)->ELVL1Id() );
         }
      }
   }
   larbcid=findid(m_larbcidset);
   larfrac=findfrac(m_larbcidset,larbcid);
   larl1id=findid(m_larl1idset);
   
   SG::ReadHandle<TileDigitsContainer> DigitsCnt(m_TileDigitsContainerKey);
   if (! DigitsCnt.isValid()) {
     ATH_MSG_WARNING( "No Tile Digits container found in TDS" );
   } 
   else {
     TileDigitsContainer::const_iterator collItr=DigitsCnt->begin();
     TileDigitsContainer::const_iterator lastColl=DigitsCnt->end();
     for (;collItr!=lastColl;++collItr){
       m_tilebcidset.insert( (*collItr)->getRODBCID() );
       m_tilel1idset.insert( (*collItr)->getLvl1Id() );
     }
   }
   tilebcid=findid(m_tilebcidset);
   tilefrac=findfrac(m_tilebcidset,tilebcid);
   tilel1id=findid(m_tilel1idset);


   SG::ReadHandle<RpcPadContainer> rpcRDO(m_RpcPadContainerKey);
   if (! rpcRDO.isValid())
   { 
     ATH_MSG_WARNING( "No RPC Pad container found in TDS" );
   }
   else 
   {

      RpcPadContainer::const_iterator pad = rpcRDO->begin();
      RpcPadContainer::const_iterator endpad = rpcRDO->end();
      for (; pad !=  endpad ; ++pad ) {
         if ( (*pad) ) {
            if ( (*pad)->size() > 0 ) 
            {
               m_rpcbcidset.insert( (*pad)->bcId() );
               m_rpcl1idset.insert( (*pad)->lvl1Id() );
            }
         }
      }
   }



   rpcbcid=findid(m_rpcbcidset);
   rpcfrac=findfrac(m_rpcbcidset,rpcbcid);
   rpcl1id=findid(m_rpcl1idset);
  
   uint32_t ctp_l1id16 = ctpl1id & 0xFFFF;
   uint32_t ctp_l1id9 = ctpl1id & 0x1FF;
   uint32_t tile_l1id16 = tilel1id & 0xFFFF;
   uint32_t tile_l1id9 = tilel1id & 0x1FF;
   uint32_t sct_l1id9 = sctl1id & 0x1FF;
   uint32_t trt_l1id9 = trtl1id & 0x1FF;
   uint32_t lar_l1id9 = larl1id & 0x1FF;
   uint32_t pixel_l1id9 = pixell1id & 0x1FF;
  
   m_CTP_BCID->Fill(ctpbcid);
   m_SCT_BCID->Fill(sctbcid);
   m_TRT_BCID->Fill(trtbcid);
   m_LAR_BCID->Fill(larbcid);


   m_Tile_BCID->Fill(tilebcid);
   m_RPC_BCID->Fill(rpcbcid);
   m_Pixel_BCID->Fill(pixelbcid);

   
   m_diff_BCID_rate->Fill(1,sctfrac);
   m_diff_BCID_rate->Fill(2,trtfrac);
   m_diff_BCID_rate->Fill(3,larfrac);
   m_diff_BCID_rate->Fill(4,tilefrac);
   m_diff_BCID_rate->Fill(5,rpcfrac);
   m_diff_BCID_rate->Fill(6,pixelfrac);
   

   if (((Int_t)ctpbcid-(Int_t)sctbcid) > 0) m_diff_BCID->Fill(1,2);
   else if (((Int_t)ctpbcid-(Int_t)sctbcid) < 0) m_diff_BCID->Fill(2,1);
   else {m_diff_BCID->Fill(1,1); m_diff_BCID->Fill(2,2);}

   if (((Int_t)ctpbcid-(Int_t)trtbcid) > 0) m_diff_BCID->Fill(1,3);
   else if (((Int_t)ctpbcid-(Int_t)trtbcid) < 0) m_diff_BCID->Fill(3,1);
   else {m_diff_BCID->Fill(1,1); m_diff_BCID->Fill(3,3);}

   if (((Int_t)ctpbcid-(Int_t)larbcid) > 0) m_diff_BCID->Fill(1,4);
   else if (((Int_t)ctpbcid-(Int_t)larbcid) < 0) m_diff_BCID->Fill(4,1);
   else {m_diff_BCID->Fill(1,1); m_diff_BCID->Fill(4,4);}

   if (((Int_t)ctpbcid-(Int_t)tilebcid) > 0) m_diff_BCID->Fill(1,5);
   else if (((Int_t)ctpbcid-(Int_t)tilebcid) < 0) m_diff_BCID->Fill(5,1);
   else {m_diff_BCID->Fill(1,1); m_diff_BCID->Fill(5,5);}

   if (((Int_t)ctpbcid-(Int_t)rpcbcid) > 0) m_diff_BCID->Fill(1,6);
   else if (((Int_t)ctpbcid-(Int_t)rpcbcid) < 0) m_diff_BCID->Fill(6,1);
   else {m_diff_BCID->Fill(1,1); m_diff_BCID->Fill(6,6);}

   if (((Int_t)ctpbcid-(Int_t)pixelbcid) > 0) m_diff_BCID->Fill(1,7);
   else if (((Int_t)ctpbcid-(Int_t)pixelbcid) < 0) m_diff_BCID->Fill(7,1);
   else {m_diff_BCID->Fill(1,1); m_diff_BCID->Fill(7,7);}

   if (((Int_t)sctbcid-(Int_t)trtbcid) > 0) m_diff_BCID->Fill(2,3);
   else if (((Int_t)sctbcid-(Int_t)trtbcid) < 0) m_diff_BCID->Fill(3,2);
   else {m_diff_BCID->Fill(2,2); m_diff_BCID->Fill(3,3);}

   if (((Int_t)sctbcid-(Int_t)larbcid) > 0) m_diff_BCID->Fill(2,4);
   else if (((Int_t)sctbcid-(Int_t)larbcid) < 0) m_diff_BCID->Fill(4,2);
   else {m_diff_BCID->Fill(2,2); m_diff_BCID->Fill(4,4);}

   if (((Int_t)sctbcid-(Int_t)tilebcid) > 0) m_diff_BCID->Fill(2,5);
   else if (((Int_t)sctbcid-(Int_t)tilebcid) < 0) m_diff_BCID->Fill(5,2);
   else {m_diff_BCID->Fill(2,2); m_diff_BCID->Fill(5,5);}

   if (((Int_t)sctbcid-(Int_t)rpcbcid) > 0) m_diff_BCID->Fill(2,6);
   else if (((Int_t)sctbcid-(Int_t)rpcbcid) < 0) m_diff_BCID->Fill(6,2);
   else {m_diff_BCID->Fill(2,2); m_diff_BCID->Fill(6,6);}

   if (((Int_t)sctbcid-(Int_t)pixelbcid) > 0) m_diff_BCID->Fill(2,7);
   else if (((Int_t)sctbcid-(Int_t)pixelbcid) < 0) m_diff_BCID->Fill(7,2);
   else {m_diff_BCID->Fill(2,2); m_diff_BCID->Fill(7,7);}

   if (((Int_t)trtbcid-(Int_t)larbcid) > 0) m_diff_BCID->Fill(3,4);
   else if (((Int_t)trtbcid-(Int_t)larbcid) < 0) m_diff_BCID->Fill(4,3);
   else {m_diff_BCID->Fill(3,3); m_diff_BCID->Fill(4,4);}

   if (((Int_t)trtbcid-(Int_t)tilebcid) > 0) m_diff_BCID->Fill(3,5);
   else if (((Int_t)trtbcid-(Int_t)tilebcid) < 0) m_diff_BCID->Fill(5,3);
   else {m_diff_BCID->Fill(3,3); m_diff_BCID->Fill(5,5);}

   if (((Int_t)trtbcid-(Int_t)rpcbcid) > 0) m_diff_BCID->Fill(3,6);
   else if (((Int_t)trtbcid-(Int_t)rpcbcid) < 0) m_diff_BCID->Fill(6,3);
   else {m_diff_BCID->Fill(3,3); m_diff_BCID->Fill(6,6);}

   if (((Int_t)trtbcid-(Int_t)pixelbcid) > 0) m_diff_BCID->Fill(3,7);
   else if (((Int_t)trtbcid-(Int_t)pixelbcid) < 0) m_diff_BCID->Fill(7,3);
   else {m_diff_BCID->Fill(3,3); m_diff_BCID->Fill(7,7);}


   if (((Int_t)larbcid-(Int_t)tilebcid) > 0) m_diff_BCID->Fill(4,5);
   else if (((Int_t)larbcid-(Int_t)tilebcid) < 0) m_diff_BCID->Fill(5,4);
   else {m_diff_BCID->Fill(4,4); m_diff_BCID->Fill(5,5);}

   if (((Int_t)larbcid-(Int_t)rpcbcid) > 0) m_diff_BCID->Fill(4,6);
   else if (((Int_t)larbcid-(Int_t)rpcbcid) < 0) m_diff_BCID->Fill(6,4);
   else {m_diff_BCID->Fill(4,4); m_diff_BCID->Fill(6,6);}

   if (((Int_t)larbcid-(Int_t)pixelbcid) > 0) m_diff_BCID->Fill(4,7);
   else if (((Int_t)larbcid-(Int_t)pixelbcid) < 0) m_diff_BCID->Fill(7,4);
   else {m_diff_BCID->Fill(4,4); m_diff_BCID->Fill(7,7);}

   if (((Int_t)tilebcid-(Int_t)rpcbcid) > 0) m_diff_BCID->Fill(5,6);
   else if (((Int_t)tilebcid-(Int_t)rpcbcid) < 0) m_diff_BCID->Fill(6,5);
   else {m_diff_BCID->Fill(5,5); m_diff_BCID->Fill(6,6);}

   if (((Int_t)tilebcid-(Int_t)pixelbcid) > 0) m_diff_BCID->Fill(5,7);
   else if (((Int_t)tilebcid-(Int_t)pixelbcid) < 0) m_diff_BCID->Fill(7,5);
   else {m_diff_BCID->Fill(5,5); m_diff_BCID->Fill(7,7);}

   if (((Int_t)rpcbcid-(Int_t)pixelbcid) > 0) m_diff_BCID->Fill(6,7);
   else if (((Int_t)rpcbcid-(Int_t)pixelbcid) < 0) m_diff_BCID->Fill(7,6);
   else {m_diff_BCID->Fill(6,6); m_diff_BCID->Fill(7,7);}


// Now l1id
   if (((Int_t)ctp_l1id16-(Int_t)sctl1id) > 0) m_diff_L1ID->Fill(1,2);
   else if (((Int_t)ctp_l1id16-(Int_t)sctl1id) < 0) m_diff_L1ID->Fill(2,1);
   else {m_diff_L1ID->Fill(1,1); m_diff_L1ID->Fill(2,2);}

   if (((Int_t)ctp_l1id16-(Int_t)trtl1id) > 0) m_diff_L1ID->Fill(1,3);
   else if (((Int_t)ctp_l1id16-(Int_t)trtl1id) < 0) m_diff_L1ID->Fill(3,1);
   else {m_diff_L1ID->Fill(1,1); m_diff_L1ID->Fill(3,3);}

   if (((Int_t)ctp_l1id16-(Int_t)larl1id) > 0) m_diff_L1ID->Fill(1,4);
   else if (((Int_t)ctp_l1id16-(Int_t)larl1id) < 0) m_diff_L1ID->Fill(4,1);
   else {m_diff_L1ID->Fill(1,1); m_diff_L1ID->Fill(4,4);}

   if (((Int_t)ctp_l1id16-(Int_t)tile_l1id16) > 0) m_diff_L1ID->Fill(1,5);
   else if (((Int_t)ctp_l1id16-(Int_t)tile_l1id16) < 0) m_diff_L1ID->Fill(5,1);
   else {m_diff_L1ID->Fill(1,1); m_diff_L1ID->Fill(5,5);}

   if (((Int_t)ctp_l1id9-(Int_t)rpcl1id) > 0) m_diff_L1ID->Fill(1,6);
   else if (((Int_t)ctp_l1id9-(Int_t)rpcl1id) < 0) m_diff_L1ID->Fill(6,1);
   else {m_diff_L1ID->Fill(1,1); m_diff_L1ID->Fill(6,6);}

   if (((Int_t)ctp_l1id16-(Int_t)pixell1id) > 0) m_diff_L1ID->Fill(1,7);
   else if (((Int_t)ctp_l1id16-(Int_t)pixell1id) < 0) m_diff_L1ID->Fill(7,1);
   else {m_diff_L1ID->Fill(1,1); m_diff_L1ID->Fill(7,7);}

   if (((Int_t)sctl1id-(Int_t)trtl1id) > 0) m_diff_L1ID->Fill(2,3);
   else if (((Int_t)sctl1id-(Int_t)trtl1id) < 0) m_diff_L1ID->Fill(3,2);
   else {m_diff_L1ID->Fill(2,2); m_diff_L1ID->Fill(3,3);}

   if (((Int_t)sctl1id-(Int_t)larl1id) > 0) m_diff_L1ID->Fill(2,4);
   else if (((Int_t)sctl1id-(Int_t)larl1id) < 0) m_diff_L1ID->Fill(4,2);
   else {m_diff_L1ID->Fill(2,2); m_diff_L1ID->Fill(4,4);}

   if (((Int_t)sctl1id-(Int_t)tile_l1id16) > 0) m_diff_L1ID->Fill(2,5);
   else if (((Int_t)sctl1id-(Int_t)tile_l1id16) < 0) m_diff_L1ID->Fill(5,2);
   else {m_diff_L1ID->Fill(2,2); m_diff_L1ID->Fill(5,5);}

   if (((Int_t)sct_l1id9-(Int_t)rpcl1id) > 0) m_diff_L1ID->Fill(2,6);
   else if (((Int_t)sct_l1id9-(Int_t)rpcl1id) < 0) m_diff_L1ID->Fill(6,2);
   else {m_diff_L1ID->Fill(2,2); m_diff_L1ID->Fill(6,6);}

   if (((Int_t)sctl1id-(Int_t)pixell1id) > 0) m_diff_L1ID->Fill(2,7);
   else if (((Int_t)sctl1id-(Int_t)pixell1id) < 0) m_diff_L1ID->Fill(7,2);
   else {m_diff_L1ID->Fill(2,2); m_diff_L1ID->Fill(7,7);}

   if (((Int_t)trtl1id-(Int_t)larl1id) > 0) m_diff_L1ID->Fill(3,4);
   else if (((Int_t)trtl1id-(Int_t)larl1id) < 0) m_diff_L1ID->Fill(4,3);
   else {m_diff_L1ID->Fill(3,3); m_diff_L1ID->Fill(4,4);}

   if (((Int_t)trtl1id-(Int_t)tile_l1id16) > 0) m_diff_L1ID->Fill(3,5);
   else if (((Int_t)trtl1id-(Int_t)tile_l1id16) < 0) m_diff_L1ID->Fill(5,3);
   else {m_diff_L1ID->Fill(3,3); m_diff_L1ID->Fill(5,5);}

   if (((Int_t)trt_l1id9-(Int_t)rpcl1id) > 0) m_diff_L1ID->Fill(3,6);
   else if (((Int_t)trt_l1id9-(Int_t)rpcl1id) < 0) m_diff_L1ID->Fill(6,3);
   else {m_diff_L1ID->Fill(3,3); m_diff_L1ID->Fill(6,6);}

   if (((Int_t)trtl1id-(Int_t)pixell1id) > 0) m_diff_L1ID->Fill(3,7);
   else if (((Int_t)trtl1id-(Int_t)pixell1id) < 0) m_diff_L1ID->Fill(7,3);
   else {m_diff_L1ID->Fill(3,3); m_diff_L1ID->Fill(7,7);}


   if (((Int_t)larl1id-(Int_t)tile_l1id16) > 0) m_diff_L1ID->Fill(4,5);
   else if (((Int_t)larl1id-(Int_t)tile_l1id16) < 0) m_diff_L1ID->Fill(5,4);
   else {m_diff_L1ID->Fill(4,4); m_diff_L1ID->Fill(5,5);}

   if (((Int_t)lar_l1id9-(Int_t)rpcl1id) > 0) m_diff_L1ID->Fill(4,6);
   else if (((Int_t)lar_l1id9-(Int_t)rpcl1id) < 0) m_diff_L1ID->Fill(6,4);
   else {m_diff_L1ID->Fill(4,4); m_diff_L1ID->Fill(6,6);}

   if (((Int_t)larl1id-(Int_t)pixell1id) > 0) m_diff_L1ID->Fill(4,7);
   else if (((Int_t)larl1id-(Int_t)pixell1id) < 0) m_diff_L1ID->Fill(7,4);
   else {m_diff_L1ID->Fill(4,4); m_diff_L1ID->Fill(7,7);}

   if (((Int_t)tile_l1id9-(Int_t)rpcl1id) > 0) m_diff_L1ID->Fill(5,6);
   else if (((Int_t)tile_l1id9-(Int_t)rpcl1id) < 0) m_diff_L1ID->Fill(6,5);
   else {m_diff_L1ID->Fill(5,5); m_diff_L1ID->Fill(6,6);}

   if (((Int_t)tile_l1id16-(Int_t)pixell1id) > 0) m_diff_L1ID->Fill(5,7);
   else if (((Int_t)tile_l1id16-(Int_t)pixell1id) < 0) m_diff_L1ID->Fill(7,5);
   else {m_diff_L1ID->Fill(5,5); m_diff_L1ID->Fill(7,7);}

   if (((Int_t)rpcl1id-(Int_t)pixel_l1id9) > 0) m_diff_L1ID->Fill(6,7);
   else if (((Int_t)rpcl1id-(Int_t)pixel_l1id9) < 0) m_diff_L1ID->Fill(7,6);
   else {m_diff_L1ID->Fill(6,6); m_diff_L1ID->Fill(7,7);}


   m_CTP_L1ID->Fill(ctp_l1id16);
   m_SCT_L1ID->Fill(sctl1id);
   m_TRT_L1ID->Fill(trtl1id);
   m_LAR_L1ID->Fill(larl1id);
   m_Tile_L1ID->Fill(tile_l1id16);
   m_RPC_L1ID->Fill(rpcl1id);
   m_Pixel_L1ID->Fill(pixell1id);
  

   m_diff_Pixel_SCT_BCID->Fill((int)(pixelbcid-sctbcid));
   m_diff_Pixel_TRT_BCID->Fill((int)(pixelbcid-trtbcid));
   m_diff_Pixel_LAR_BCID->Fill((int)(pixelbcid-larbcid));
   m_diff_Pixel_Tile_BCID->Fill((int)(pixelbcid-tilebcid));
   m_diff_Pixel_RPC_BCID->Fill((int)(pixelbcid-rpcbcid));
   m_diff_Pixel_CTP_BCID->Fill((int)(pixelbcid-ctpbcid));
  
   m_diff_CTP_SCT_BCID->Fill((int)(ctpbcid-sctbcid));
   m_diff_CTP_TRT_BCID->Fill((int)(ctpbcid-trtbcid));
   m_diff_CTP_LAR_BCID->Fill((int)(ctpbcid-larbcid));
   m_diff_CTP_Tile_BCID->Fill((int)(ctpbcid-tilebcid));
   m_diff_CTP_RPC_BCID->Fill((int)(ctpbcid-rpcbcid));
   m_diff_SCT_TRT_BCID->Fill((int)(sctbcid-trtbcid));
   m_diff_SCT_LAR_BCID->Fill((int)(sctbcid-larbcid));
   m_diff_SCT_Tile_BCID->Fill((int)(sctbcid-tilebcid));
   m_diff_SCT_RPC_BCID->Fill((int)(sctbcid-rpcbcid));
   m_diff_TRT_LAR_BCID->Fill((int)(trtbcid-larbcid));
   m_diff_TRT_Tile_BCID->Fill((int)(trtbcid-tilebcid));
   m_diff_TRT_RPC_BCID->Fill((int)(trtbcid-rpcbcid));
   m_diff_LAR_Tile_BCID->Fill((int)(larbcid-tilebcid));
   m_diff_LAR_RPC_BCID->Fill((int)(larbcid-rpcbcid));
   m_diff_Tile_RPC_BCID->Fill((int)(tilebcid-rpcbcid));


   m_diff_CTP_SCT_L1ID->Fill((int)(ctp_l1id16-sctl1id));
   m_diff_CTP_TRT_L1ID->Fill((int)(ctp_l1id16-trtl1id));
   m_diff_CTP_LAR_L1ID->Fill((int)(ctp_l1id16-larl1id));
   m_diff_CTP_Tile_L1ID->Fill((int)(ctp_l1id16-tile_l1id16));
   m_diff_CTP_RPC_L1ID->Fill((int)((int)ctp_l1id9-(int)rpcl1id));


   m_diff_Pixel_SCT_L1ID->Fill((int)(pixell1id-sctl1id));
   m_diff_Pixel_TRT_L1ID->Fill((int)(pixell1id-trtl1id));
   m_diff_Pixel_LAR_L1ID->Fill((int)(pixell1id-larl1id));
   m_diff_Pixel_Tile_L1ID->Fill((int)(pixell1id-tile_l1id16));
   m_diff_Pixel_RPC_L1ID->Fill((int)((int)(pixel_l1id9-rpcl1id)));
   m_diff_Pixel_CTP_L1ID->Fill((int)(pixell1id-ctp_l1id16));

   m_diff_SCT_TRT_L1ID->Fill((int)(sctl1id-trtl1id));
   m_diff_SCT_LAR_L1ID->Fill((int)(sctl1id-larl1id));
   m_diff_SCT_Tile_L1ID->Fill((int)(sctl1id-tile_l1id16));
   m_diff_SCT_RPC_L1ID->Fill((int)(sct_l1id9-rpcl1id));
   m_diff_TRT_LAR_L1ID->Fill((int)(trtl1id-larl1id));
   m_diff_TRT_Tile_L1ID->Fill((int)(trtl1id-tile_l1id16));
   m_diff_TRT_RPC_L1ID->Fill((int)(trt_l1id9-rpcl1id));
   m_diff_LAR_Tile_L1ID->Fill((int)(larl1id-tile_l1id16));
   m_diff_LAR_RPC_L1ID->Fill((int)(lar_l1id9-rpcl1id));
   m_diff_Tile_RPC_L1ID->Fill((int)(tile_l1id9-rpcl1id));


   if (!m_doOnlineHists) {
      m_diff_Pixel_SCT_BCID_Rebin->Fill((int)(pixelbcid-sctbcid));
      m_diff_Pixel_TRT_BCID_Rebin->Fill((int)(pixelbcid-trtbcid));
      m_diff_Pixel_LAR_BCID_Rebin->Fill((int)(pixelbcid-larbcid));
      m_diff_Pixel_Tile_BCID_Rebin->Fill((int)(pixelbcid-tilebcid));
      m_diff_Pixel_RPC_BCID_Rebin->Fill((int)(pixelbcid-rpcbcid));
      m_diff_Pixel_CTP_BCID_Rebin->Fill((int)(pixelbcid-ctpbcid));  
      m_diff_CTP_SCT_BCID_Rebin->Fill((int)(ctpbcid-sctbcid));
      m_diff_CTP_TRT_BCID_Rebin->Fill((int)(ctpbcid-trtbcid));
      m_diff_CTP_LAR_BCID_Rebin->Fill((int)(ctpbcid-larbcid));
      m_diff_CTP_Tile_BCID_Rebin->Fill((int)(ctpbcid-tilebcid));
      m_diff_CTP_RPC_BCID_Rebin->Fill((int)(ctpbcid-rpcbcid));
      m_diff_SCT_TRT_BCID_Rebin->Fill((int)(sctbcid-trtbcid));
      m_diff_SCT_LAR_BCID_Rebin->Fill((int)(sctbcid-larbcid));
      m_diff_SCT_Tile_BCID_Rebin->Fill((int)(sctbcid-tilebcid));
      m_diff_SCT_RPC_BCID_Rebin->Fill((int)(sctbcid-rpcbcid));
      m_diff_TRT_LAR_BCID_Rebin->Fill((int)(trtbcid-larbcid));
      m_diff_TRT_Tile_BCID_Rebin->Fill((int)(trtbcid-tilebcid));
      m_diff_TRT_RPC_BCID_Rebin->Fill((int)(trtbcid-rpcbcid));
      m_diff_LAR_Tile_BCID_Rebin->Fill((int)(larbcid-tilebcid));
      m_diff_LAR_RPC_BCID_Rebin->Fill((int)(larbcid-rpcbcid));
      m_diff_Tile_RPC_BCID_Rebin->Fill((int)(tilebcid-rpcbcid));

      m_diff_CTP_SCT_L1ID_Rebin->Fill((int)(ctp_l1id16-sctl1id));
      m_diff_CTP_TRT_L1ID_Rebin->Fill((int)(ctp_l1id16-trtl1id));
      m_diff_CTP_LAR_L1ID_Rebin->Fill((int)(ctp_l1id16-larl1id));
      m_diff_CTP_Tile_L1ID_Rebin->Fill((int)(ctp_l1id16-tile_l1id16));
      m_diff_CTP_RPC_L1ID_Rebin->Fill((int)(ctp_l1id9-rpcl1id));
      m_diff_Pixel_SCT_L1ID_Rebin->Fill((int)(pixell1id-sctl1id));
      m_diff_Pixel_TRT_L1ID_Rebin->Fill((int)(pixell1id-trtl1id));
      m_diff_Pixel_LAR_L1ID_Rebin->Fill((int)(pixell1id-larl1id));
      m_diff_Pixel_Tile_L1ID_Rebin->Fill((int)(pixell1id-tile_l1id16));
      m_diff_Pixel_RPC_L1ID_Rebin->Fill((int)(pixel_l1id9-rpcl1id));
      m_diff_Pixel_CTP_L1ID_Rebin->Fill((int)(pixell1id-ctp_l1id16));
      m_diff_SCT_TRT_L1ID_Rebin->Fill((int)(sctl1id-trtl1id));
      m_diff_SCT_LAR_L1ID_Rebin->Fill((int)(sctl1id-larl1id));
      m_diff_SCT_Tile_L1ID_Rebin->Fill((int)(sctl1id-tile_l1id16));
      m_diff_SCT_RPC_L1ID_Rebin->Fill((int)(sct_l1id9-rpcl1id));
      m_diff_TRT_LAR_L1ID_Rebin->Fill((int)(trtl1id-larl1id));
      m_diff_TRT_Tile_L1ID_Rebin->Fill((int)(trtl1id-tile_l1id16));
      m_diff_TRT_RPC_L1ID_Rebin->Fill((int)(trt_l1id9-rpcl1id));
      m_diff_LAR_Tile_L1ID_Rebin->Fill((int)(larl1id-tile_l1id16));
      m_diff_LAR_RPC_L1ID_Rebin->Fill((int)(lar_l1id9-rpcl1id));
      m_diff_Tile_RPC_L1ID_Rebin->Fill((int)(tile_l1id9-rpcl1id));

   }


   if (!m_doOnlineHists) {
      m_diff_CTP_SCT_BCID_lumi->Fill(lumi,float(ctpbcid-sctbcid));
      m_diff_CTP_TRT_BCID_lumi->Fill(lumi,float(ctpbcid-trtbcid));
      m_diff_CTP_LAR_BCID_lumi->Fill(lumi,float(ctpbcid-larbcid));
      m_diff_CTP_Tile_BCID_lumi->Fill(lumi,float(ctpbcid-tilebcid));
      m_diff_CTP_RPC_BCID_lumi->Fill(lumi,float(ctpbcid-rpcbcid));
      m_diff_CTP_Pixel_BCID_lumi->Fill(lumi,float(ctpbcid-pixelbcid));
     
      m_diff_CTP_SCT_L1ID_lumi->Fill(lumi,float(ctp_l1id16-sctl1id));
      m_diff_CTP_TRT_L1ID_lumi->Fill(lumi,float(ctp_l1id16-trtl1id));
      m_diff_CTP_LAR_L1ID_lumi->Fill(lumi,float(ctp_l1id16-larl1id));
      m_diff_CTP_Tile_L1ID_lumi->Fill(lumi,float(ctp_l1id16-tilel1id));
      m_diff_CTP_RPC_L1ID_lumi->Fill(lumi,float(ctp_l1id9-rpcl1id));
      m_diff_CTP_Pixel_L1ID_lumi->Fill(evtNum,float(ctp_l1id16-pixell1id));
   }


   if ( m_field.retrieve().isFailure() ) {
      log << MSG::WARNING << "Failed to retrieve MagneticField tool " << m_field << endmsg;
      return StatusCode::FAILURE;
   } else {
      //log << MSG::DEBUG << "Retrieved MagneticField tool " << m_field << endmsg;
   }
  
   //REL19 
   Amg::Vector3D f; 
   //REL18 CLHEP::Hep3Vector f; 
   //REL19 
   Amg::Vector3D gP1(m_solenoidPositionX, m_solenoidPositionY, m_solenoidPositionZ);
   //REL18 Trk::GlobalPosition gP1(m_solenoidPositionX, m_solenoidPositionY, m_solenoidPositionZ);
   //REL19 
   m_field->getField(&gP1,&f);
   //REL18 m_field->getMagneticFieldKiloGauss(gP1,f);
   float solenoid_bz = f[2];
   //REL19: field is in kilotesla (!)
   solenoid_bz *= 1000;
   //REL18 : was in kilogauss
   //solenoid_bz/=10;
   m_Bfield_solenoid->Fill(solenoid_bz); 
   m_Bfield_solenoid_vsLB->Fill(lumi, solenoid_bz);
   //REL19 
   Amg::Vector3D  gP2(m_toroidPositionX, m_toroidPositionY, m_toroidPositionZ);
   //REL18 Trk::GlobalPosition gP2(m_toroidPositionX, m_toroidPositionY, m_toroidPositionZ);
   //REL19 
   m_field->getField(&gP2,&f);
   //REL18 m_field->getMagneticFieldKiloGauss(gP2,f);
   float toroid_bx = f[0];
   //REL19: field is in kilotesla (!)
   toroid_bx *= 1000;
   //REL18 : was in kilogauss
   //toroid_bx/=10;
   m_Bfield_toroid->Fill(toroid_bx);    
   m_Bfield_toroid_vsLB->Fill(lumi, toroid_bx);


   log << MSG::VERBOSE << "evt Num  : "  << evtNum    << "\tLumi      : "  << lumi     << endmsg;
   log << MSG::VERBOSE << "evt L1ID : "  << ctpl1id   << "\tevt BCID  : "  << ctpbcid  << endmsg;
   log << MSG::VERBOSE << "CTP L1ID : "  << ctpl1id   << "\tCTP BCID  : "  << ctpbcid  << endmsg;
   log << MSG::VERBOSE << "LAR L1ID : "  << larl1id   << "\tLAR BCID  : "  << larbcid  << endmsg;
   log << MSG::VERBOSE << "TILE L1ID: "  << tilel1id  << "\tTILE BCID : "  << tilebcid << endmsg;
   log << MSG::VERBOSE << "RPC L1ID : "  << rpcl1id   << "\tRPC BCID  : "  << rpcbcid  << endmsg;
   log << MSG::VERBOSE << "Pixel L1ID : "  << pixell1id   << "\tPixel BCID  : "  << pixelbcid  << endmsg;
   log << MSG::VERBOSE << "CTP-RPC L1ID : "  << ctp_l1id9-rpcl1id   << "\tCTP-LAR L1ID  : "  << ctp_l1id16-larl1id  << endmsg;
   return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------------
StatusCode DQTDetSynchMonTool::procHistograms( )
//StatusCode DQTDetSynchMonTool::procHistograms( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun )
//----------------------------------------------------------------------------------
{
  if ( endOfLumiBlockFlag() || endOfRunFlag() ) {
      MsgStream log(msgSvc(), name());

      //log << MSG::DEBUG << "in finalHists()" << endmsg;
   }

   return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------------
StatusCode DQTDetSynchMonTool::checkHists(bool /* fromFinalize */)
//----------------------------------------------------------------------------------
{
   MsgStream log(msgSvc(), name());

   //log << MSG::DEBUG << "in checkHists()" << endmsg;

   return StatusCode::SUCCESS;
}


//----------------------------------------------------------------------------------
uint32_t DQTDetSynchMonTool::findid(std::multiset<uint32_t>& mset)
//----------------------------------------------------------------------------------
{
   MsgStream log(msgSvc(), name());
   uint32_t id(9999999),refid(9999999);
   int refcount(0),count(0);
   std::multiset<uint32_t>::iterator it = mset.begin();
   std::multiset<uint32_t>::iterator itend = mset.end();

   if (it!=itend && !mset.empty()){ //if empty do nothing 
     // the following uses invalid iterator
     // if ( (*it)==(*itend) ) { log << "all ids equal: " << (*it) << endmsg; return (*it);} // if all ids equal, return the result immediately
      for (;it!=itend;++it) {
         if ( (*it) != id ) { //skip check if the id value has been already evaluated

            id=*it; count = mset.count(*it);
            log << MSG::VERBOSE << "new id found: " << (*it) << " with counts: " << count <<endmsg;
            if ( ( (*it) !=refid )&&(count>refcount) ){ // if count> previous maximum and double check the id value
               refid = (*it);
               refcount=count;
               log << MSG::VERBOSE << "new REFERENCE id: " << refid << " with REFERENCE counts: " << refcount <<endmsg;
            }
         }
      }
   }
   log << MSG::VERBOSE << "Returning REFERENCE id: " << refid << " with REFERENCE counts: " << refcount <<endmsg;
   return refid;
}

//----------------------------------------------------------------------------------
float DQTDetSynchMonTool::findfrac(std::multiset<uint32_t>& mset, uint16_t ctpid)
//----------------------------------------------------------------------------------
{
  MsgStream log(msgSvc(), name());
  std::multiset<uint32_t>::iterator it = mset.begin();
  std::multiset<uint32_t>::iterator itend = mset.end();

  int totalCounter=0;
  int nonctpIdCounter=0;
  float frac = 0.0;

  if (it!=itend && !mset.empty()){ //if empty do nothing
    // The following is not valid C++
    // if ( (*it)==(*itend) ) { // if all ids equal, return the result immediately
    //   if ((*it)==ctpid) return 0.0;
    //   else if (!((*it)==ctpid)) return 1.0;
    // }
    for (;it!=itend;++it) {
      totalCounter++;
      if ( (*it) != ctpid ) nonctpIdCounter++;
    }
  }

  if (totalCounter>0)
    frac = ((float) nonctpIdCounter)/((float) totalCounter);
  else
    frac = 1.0; //set frac = 1 if totalCounter counts zero

  //std::cout << "Returning frac: " << nonctpIdCounter << "/" << totalCounter << "=" << frac << std::endl;
  return frac;
}
