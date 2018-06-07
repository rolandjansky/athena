/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************************************************
// IDAlignMonResiduals.cxx
// AUTHORS: Beate Heinemann, Tobias Golling, Ben Cooper, John Alison
// ***************************************************************************************

#include "IDAlignMonResiduals.h"

#include <sstream>
#include <math.h>
#include "TH1.h"
#include "LWHists/TH1F_LW.h"
#include "TF1.h" 
#include "TH2.h"
#include "TH3.h"
#include "TProfile.h"
#include "TMath.h"
#include "TProfile2D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"

//#include "EventInfo/EventID.h"
#include "xAODEventInfo/EventInfo.h"

#include "GaudiKernel/IJobOptionsSvc.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
//#include "IdDictDetDescr/IdDictManager.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "TrkTrack/TrackCollection.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/SCT_Cluster.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/ResidualPull.h"

#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/Layer.h"
#include "TrkSurfaces/Surface.h"

#include "TrkToolInterfaces/IUpdator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"

#include "InDetAlignGenTools/IInDetAlignHitQualSelTool.h"
//#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

#include "TrackSelectionTool.h"

#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"


// *********************************************************************
// Public Methods
// *********************************************************************

struct IDAlignMonResiduals::TRTBarrelHistograms{
	
	/** Residual in the TRT Barrel */
	TH1F_LW* residualR[3];
	TH1F_LW* residualR_notube[3];
	/** Pull in the TRT Barrel */
	TH1F_LW* pullR[3];
	TH1F_LW* pullR_notube[3];
	
	/** Average residual and residual RMS vs phi sector in the TRT Barrel */
	/** the fist "3" is for the three layers, the second is for (both A and C, A-side, C-Side)*/
	TProfile* aveRes_l[3][3];
	TProfile* rmsRes_l[3][3];
	
	/** Average residual and residual RMS vs straw layer for each phiSec(0-31) */
  
  
	TProfile* aveResVsStrawLayerStackLevel[3][32];
	TProfile* rmsResVsStrawLayerStackLevel[3][32];
    
	
	/** Average residual and residual RMS Intergrated over each phi module vs straw layer */
	/** the "5" is for (total, upper, lower, left, right)*/
	TProfile* aveResOverPhiVsStrawLayer[5][3];
	TProfile* rmsResOverPhiVsStrawLayer[5][3];
	
	/** Drift radius from track fit */
	TH1F_LW* PredictedR[3];
	
	/** Measured drift radius */
	TH1F_LW* MeasuredR[3];
	
	/** Residual vs eta (of the track) in the module layers(0-2) */
	TProfile* aveResVsTrackEta_l[3][3];
	
	/** R(t) relation */
	TH2F* rt[3];
	
	/** Residuals and pulls vs pT in the TRT Barrel */
	TH2F* residualR_pt[3];
	TH2F* pullR_pt[3];
	TH2F* pullR_notube_pt[3];

  
        /** Residuals and pulls vs mu in the TRT Barrel */
	TH2F* residualR_mu[3];
	TH2F* pullR_mu[3];
	TH2F* pullR_notube_mu[3];

	
	/** L/R assignment */
	TH1F_LW* lr[3];
	TProfile* lr_l[3][3];
	TProfile* lrOverPhiVsStrawLayer[3];

        /** Residual maps vs phi-sector and z-position in the TRT Barrel */
        TH3F* resVsPhiZ[3][3]; // 3D histogram with residuals vs phi & z, will be used to fill following th2f
        TH2F* aveResVsPhiZ[3][3];
        TH2F* rmsResVsPhiZ[3][3];
        /** Residual maps vs phi-sector and eta in the TRT Barrel (this is a test until I get z working...) */
        TH3F* resVsPhiEta[3][3]; // 3D histogram with residuals vs phi & z, will be used to fill following th2f
        TH2F* aveResVsPhiEta[3][3];
        TH2F* rmsResVsPhiEta[3][3];

	TRTBarrelHistograms(){
		
		for(unsigned int lay=0;lay<3; ++lay){
			residualR[lay] = 0;
			pullR[lay] = 0;
			pullR_notube[lay] = 0;
			residualR_pt[lay] = 0;
			pullR_pt[lay] = 0;
			pullR_notube_pt[lay] = 0;
			residualR_mu[lay] = 0;
			pullR_mu[lay] = 0;
			pullR_notube_mu[lay] = 0;
			PredictedR[lay] = 0;
			MeasuredR[lay] = 0;
			rt[lay] = 0;
			lr[lay] = 0;
			lrOverPhiVsStrawLayer[lay] = 0;

			for(unsigned int side=0; side<3; ++side){
				aveRes_l[lay][side] = 0;
				rmsRes_l[lay][side] = 0;
				aveResVsTrackEta_l[lay][side] = 0;
				lr_l[lay][side] = 0;
				resVsPhiZ[lay][side] = 0;
				aveResVsPhiZ[lay][side] = 0;
				aveResVsPhiZ[lay][side] = 0;
				resVsPhiEta[lay][side] = 0;
				aveResVsPhiEta[lay][side] = 0;
				rmsResVsPhiEta[lay][side] = 0;
			}
			
			
			    for(unsigned int phi=0; phi<32; ++phi){
			      aveResVsStrawLayerStackLevel[lay][phi] = 0;
			      rmsResVsStrawLayerStackLevel[lay][phi] = 0;
			    
			}
		}
	};
	
};

struct IDAlignMonResiduals::TRTEndcapHistograms{
	
	/** Residual in TRT Endcap */
	/** the 41 is the forty rings plus the total*/
        /** In the case of no extendedPlots use only the total */

  

        TH1F_LW* residualR[2][41];
	TH1F_LW* residualR_notube[2][41];
        

	/** Pull in the TRT Endcap */
	/** The 41 is the forty rings plus the total*/
	TH1F_LW* pullR[2][41];
	TH1F_LW* pullR_notube[2][41];
	
	/** Average residual and residual RMS vs endcap ring */
	TProfile* aveResVsRing[2];	
	TProfile* rmsResVsRing[2];	
	
	/** Average residual and residual RMS vs phi sector */
	TProfile* aveResVsPhiSec[2];
	TProfile* rmsResVsPhiSec[2];
	
	/** Drift radius from track fit */
	TH1F_LW* PredictedR[2];	
	
	/** Measured drift radius */
	TH1F_LW* MeasuredR[2];
	
	/** Residual vs eta (of the track) in the Endcap */
	TProfile* aveResVsTrackEta[2];
	
	/** Residuals and pulls vs pT in the TRT Endcap */
	TH2F* residualR_pt[2];
	TH2F* pullR_pt[2];
	TH2F* pullR_notube_pt[2];

        /** Residuals and pulls vs mu in the TRT Endcap */
	TH2F* residualR_mu[2];
	TH2F* pullR_mu[2];
	TH2F* pullR_notube_mu[2];
	
	/** R(t) relation */
	TH2F* rt[2];
	
	/** L/R assignment */
	TH1F_LW* lr[2];
	TProfile* lrVsPhiSec[2];
	TProfile* lrVsRing[2];

        /** Residual maps of the Endcaps */
        TH3F* resVsPhiWheel[2];
        TH2F* aveResVsPhiWheel[2];
        TH2F* rmsResVsPhiWheel[2];

        /** Residual maps for low pT tracks separated by charge in the TRT Endcap */
        TH3F* resVsRadiusWheelPos[2];
        TH2F* aveResVsRadiusWheelPos[2];
        TH2F* rmsResVsRadiusWheelPos[2];
        TH3F* resVsRadiusWheelNeg[2];
        TH2F* aveResVsRadiusWheelNeg[2];
        TH2F* rmsResVsRadiusWheelNeg[2];

	TRTEndcapHistograms(){
		
		for(unsigned int side=0; side<2; ++side){
			
			aveResVsRing[side] = 0;
			rmsResVsRing[side] = 0;
			aveResVsPhiSec[side] = 0;
			rmsResVsPhiSec[side] = 0;
			PredictedR[side] = 0;
			MeasuredR[side] = 0;
			aveResVsTrackEta[side] = 0;
			rt[side] = 0;
			lr[side] = 0;
			lrVsPhiSec[side] = 0;
			lrVsRing[side] = 0;
			residualR_pt[side] = 0;
			pullR_pt[side] = 0;
			pullR_notube_pt[side] = 0;
			residualR_mu[side] = 0;
			pullR_mu[side] = 0;
			pullR_notube_mu[side] = 0;
			resVsPhiWheel[side] = 0;
			aveResVsPhiWheel[side] = 0;
			rmsResVsPhiWheel[side] = 0;
			resVsRadiusWheelPos[side] = 0;
			aveResVsRadiusWheelPos[side] = 0;
			rmsResVsRadiusWheelPos[side] = 0;
			resVsRadiusWheelNeg[side] = 0;
			aveResVsRadiusWheelNeg[side] = 0;
			rmsResVsRadiusWheelNeg[side] = 0;

			//The extra 1 is for the total
			for(unsigned int ring=0; ring<41; ++ring){
				residualR[side][ring] = 0;
				pullR[side][ring] = 0;
				pullR_notube[side][ring] = 0;
			}
			
		}
		
	};
	
};


IDAlignMonResiduals::IDAlignMonResiduals( const std::string & type, const std::string & name, const IInterface* parent )
 :ManagedMonitorToolBase( type, name, parent ),
  theComTime(0),
  m_comTimeObjectName("ComTime"),
  m_trtcaldbSvc("TRT_CalDbSvc",name),
  m_hWeightInFile(0),
  m_etapTWeight(0)
  
{
	m_iUpdator               = ToolHandle<Trk::IUpdator>("Trk::KalmanUpdator");
	m_propagator             = ToolHandle<Trk::IPropagator>("Trk::RungeKuttaPropagator");
	m_residualPullCalculator = ToolHandle<Trk::IResidualPullCalculator>("Trk::ResidualPullCalculator/ResidualPullCalculator");
	m_trackSelection = ToolHandle<InDetAlignMon::TrackSelectionTool>("InDetAlignMon::TrackSelectionTool");
	//m_idtrackSelection = ToolHandle<InDet::IInDetTrackSelectionTool>("InDetTrackSelectionTool/InDetTrackSelectionTool"); 
	m_hitQualityTool       = ToolHandle<IInDetAlignHitQualSelTool>("");
	m_trt_b_hist  = new TRTBarrelHistograms();
	m_trt_ec_hist = new TRTEndcapHistograms();
	m_tracksName  = "ExtendedTracks";
	m_triggerChainName = "NoTriggerSelection";
	m_z_fix = 366.5; // IBL Stave fixing screw position [mm]
	m_NLumiBlocksMon = 1;
	m_minTRTResWindow      = -1.0;   
	m_maxTRTResWindow      = 1.0;  
	m_minSiResMeanWindow   = -0.1;   
	m_maxSiResMeanWindow   = 0.1;  
	m_minSiResWidthWindow  = 0.0;   
	m_maxSiResWidthWindow  = 0.3;  
	m_minSiPullWidthWindow = 0.0;   
	m_maxSiPullWidthWindow = 2.0;
	m_minSiResFillRange    = -0.5;   
	m_maxSiResFillRange    = 0.5;
	m_maxPIXResXFillRange  = 0.;  // if 0, it will use the m_maxSiResFillRange value
	m_minPIXResXFillRange  = 0.;  // if 0, it will use the m_minSiResFillRange value
	m_maxPIXResYFillRange  = 0.;
	m_minPIXResYFillRange  = 0.;
	m_maxSCTResFillRange   = 0.;
	m_minSCTResFillRange   = 0.;
	m_ClusterSizeRange     = 10.;
	m_IncidentThetaRange   = 2;
	m_IncidentPhiRange     = 0.8;
	m_RangeOfPullHistos    = 10.; 
	m_PtRange              = 20.;
	m_nBinsMuRange         = 100.;
	m_muRangeMin           = 0.;
	m_muRangeMax           = 100;
	m_nBinsPtRange         = 100;
	m_mapSplit             = 1; // Create the residual maps splitting the modules in nxn pieces
	m_do3DOverlapHistos     = false;
	m_extendedPlots         = false;
	m_doClusterSizeHistos   = false;
	m_FinerBinningFactor    = 1;
	m_LBGranularity         = 1;
	m_LBRangeMin            = -0.5;
	m_LBRangeMax            = 2599.5; // 1199.5
	m_nBinsLB               = 52; // 24
	m_gap_pix = 4;
	m_gap_sct = 4;
	m_mu = 0.;
	m_nBinsMuRange         = 100.;
	m_muRangeMin           = 0.;
	m_muRangeMax           = 100;
	nIBLHitsPerLB          = 0;
	m_minIBLhits           = -1;
	m_hasBeenCalledThisEvent=false;
	m_doIBLLBPlots         = false;
	m_useGausFit           = false;
	m_maxPtEC              = 10.;

	InitializeHistograms();
	
	
	declareProperty("tracksName"                , m_tracksName);
	declareProperty("CheckRate"                 , m_checkrate=1000);
	declareProperty("triggerChainName"          , m_triggerChainName);
	declareProperty("minTRTResidualWindow"      , m_minTRTResWindow);
	declareProperty("maxTRTResidualWindow"      , m_maxTRTResWindow);
	declareProperty("iUpdator"                  , m_iUpdator);
	declareProperty("propagator"                , m_propagator);
	declareProperty("trackSelection"            , m_trackSelection);
	//declareProperty("idtrackSelection"          , m_idtrackSelection);   //For retreiving from jO
	declareProperty("ResidualPullCalculatorTool", m_residualPullCalculator);
	declareProperty("HitQualityTool"            , m_hitQualityTool);
	declareProperty("Pixel_Manager"             , m_Pixel_Manager);
	declareProperty("SCT_Manager"               , m_SCT_Manager);
	declareProperty("TRT_Manager"               , m_TRT_Manager);
	declareProperty("minSiResFillRange"         , m_minSiResFillRange);
	declareProperty("maxSiResFillRange"         , m_maxSiResFillRange);
	declareProperty("do3DOverlapHistos"         , m_do3DOverlapHistos);
	declareProperty("doClusterSizeHistos"       , m_doClusterSizeHistos);
	declareProperty("ITRT_CalDbSvc"             , m_trtcaldbSvc);
	declareProperty("ComTimeObjectName"         , m_comTimeObjectName = "TRT_Phase");
	declareProperty("useExtendedPlots"          , m_extendedPlots);
	declareProperty("maxPIXResXFillRange"       , m_maxPIXResXFillRange);
	declareProperty("minPIXResXFillRange"       , m_minPIXResXFillRange);
	declareProperty("maxPIXResYFillRange"       , m_maxPIXResYFillRange);
	declareProperty("minPIXResYFillRange"       , m_minPIXResYFillRange);
	declareProperty("maxSCTResFillRange"        , m_maxSCTResFillRange);
	declareProperty("minSCTResFillRange"        , m_minSCTResFillRange);
	declareProperty("RangeOfPullHistos"         , m_RangeOfPullHistos);
	declareProperty("PtRange"                   , m_PtRange);
	declareProperty("NBinsPtRange"              , m_nBinsPtRange);
	declareProperty("NBinsLB"                   , m_nBinsLB);
	declareProperty("FinerBinningFactor"        , m_FinerBinningFactor);
	declareProperty("NSplitMap"                 , m_mapSplit);
	declareProperty("applyHistWeight"           , m_applyHistWeight = false);
	declareProperty("hWeightInFileName"         , m_hWeightInFileName  = "hWeight.root" ); 
	declareProperty("hWeightHistName"           , m_hWeightHistName    = "trk_pT_vs_eta" );
	declareProperty("doIBLLBPlots"              , m_doIBLLBPlots );
	declareProperty("useGausFit"                , m_useGausFit);
	declareProperty("lowPtCut"                  , m_maxPtEC);

}

//---------------------------------------------------------------------------------------

IDAlignMonResiduals::~IDAlignMonResiduals() {
	delete m_trt_b_hist;
	delete m_trt_ec_hist;
}


void IDAlignMonResiduals::InitializeHistograms()
{
  
  m_totalEvents = 0;
  m_oldlumiblock = 0;
  m_sirescalcfailure =0;
  m_pix_eca_xresvsmodphidisk_3d = 0;
  m_pix_ecc_xresvsmodphidisk_3d = 0;
  m_pix_eca_yresvsmodphidisk_3d = 0;
  m_pix_ecc_yresvsmodphidisk_3d = 0;
  m_dbm_xresvsmodphi_2d = 0;
  m_sct_b_pullx_pt = 0;
  m_sct_eca_pullx_pt = 0       ;
  m_sct_ecc_pullx_pt = 0;
  m_sct_b0_xresvsmodetaphi_3d = 0;
  m_sct_b1_xresvsmodetaphi_3d = 0;
  m_sct_b2_xresvsmodetaphi_3d = 0;
  m_sct_b3_xresvsmodetaphi_3d = 0;

  m_si_residualx = 0;
  m_si_b_residualx = 0;

  m_si_barrel_resX_mean = 0;
  m_si_eca_resX_mean = 0;
  m_si_ecc_resX_mean = 0;
  m_si_barrel_resY_mean = 0;
  m_si_eca_resY_mean = 0;
  m_si_ecc_resY_mean = 0;

  m_si_barrel_resX_mean_profile = 0;
  m_si_barrel_resY_mean_profile = 0;


  m_si_barrel_resX_rms = 0;
  m_si_eca_resX_rms = 0;
  m_si_ecc_resX_rms = 0;
  m_si_barrel_resY_rms = 0;
  m_si_eca_resY_rms = 0;
  m_si_ecc_resY_rms = 0;

  m_si_barrel_pullX = 0;
  m_si_eca_pullX = 0;
  m_si_ecc_pullX = 0;
  m_si_barrel_pullY = 0;
  m_si_eca_pullY = 0;
  m_si_ecc_pullY = 0;

  m_si_barrel_resX = 0;
  m_si_eca_resX = 0;
  m_si_ecc_resX = 0;
  m_si_barrel_resY = 0;
  m_si_eca_resY = 0;
  m_si_ecc_resY = 0;

  m_si_barrel_pullX_width = 0;
  m_si_eca_pullX_width = 0;
  m_si_ecc_pullX_width = 0;
  m_si_barrel_pullY_width = 0;
  m_si_eca_pullY_width = 0;
  m_si_ecc_pullY_width = 0;

  m_si_barrel_pullX_mean = 0;
  m_si_eca_pullX_mean = 0;
  m_si_ecc_pullX_mean = 0;
  m_si_barrel_pullY_mean = 0;
  m_si_eca_pullY_mean = 0;
  m_si_ecc_pullY_mean = 0;
  m_pix_b_residualx = 0;
  m_pix_b_residualy = 0;
  m_pix_b_residualx_fine = 0;
  m_pix_b_residualy_fine = 0;
  m_pix_b_biased_residualx = 0;
  m_pix_b_biased_residualy = 0;
  m_pix_eca_residualx = 0;
  m_pix_eca_residualy = 0;
  m_pix_ecc_residualx = 0;
  m_pix_ecc_residualy = 0;
  m_pix_eca_residualx_fine = 0;
  m_pix_eca_residualy_fine = 0;
  m_pix_ecc_residualx_fine = 0;
  m_pix_ecc_residualy_fine = 0;
 
  m_pix_eca_pullx = 0;
  m_pix_eca_pully = 0;
  m_pix_ecc_pullx = 0;
  m_pix_ecc_pully = 0;
  m_pix_bec_Oxresx_mean = 0;
  m_pix_bec_Oyresx_mean = 0;
  m_pix_bec_Oxresy_mean = 0;
  m_pix_bec_Oyresy_mean = 0;
  m_pix_bec_Oxresx_rms = 0;
  m_pix_bec_Oyresx_rms = 0;
  m_pix_bec_Oxresy_rms = 0;
  m_pix_bec_Oyresy_rms = 0;

  m_pix_b_xresvsmodeta = 0;
  m_pix_b_xresvsmodphi = 0;
  m_pix_b_yresvsmodeta = 0;
  m_pix_b_yresvsmodphi = 0;
  m_pix_eca_xresvsmodphi = 0;
  m_pix_ecc_xresvsmodphi = 0;
  m_pix_eca_yresvsmodphi = 0;
  m_pix_ecc_yresvsmodphi = 0;
  m_pix_b_biased_residualx_pt = 0;
  m_pix_b_biased_residualy_pt  = 0;
  m_pix_eca_biased_residualx  = 0;
  m_pix_eca_biased_residualy  = 0;
  m_pix_eca_biased_residualx_pt = 0;
  m_pix_eca_biased_residualy_pt = 0;
  m_pix_ecc_biased_residualx  = 0;
  m_pix_ecc_biased_residualy  = 0;
  m_pix_ecc_biased_residualx_pt = 0;
  m_pix_ecc_biased_residualy_pt = 0;
  m_pix_eca_xresvsmodphi_2d = 0;
  m_pix_ecc_xresvsmodphi_2d = 0;
  m_pix_eca_yresvsmodphi_2d = 0;
  m_pix_ecc_yresvsmodphi_2d = 0;
  
  m_pix_b_Oxresxvsmodeta = 0;
  m_pix_b_Oxresxvsmodphi = 0;
  m_pix_b_Oyresyvsmodeta = 0;
  m_pix_b_Oyresyvsmodphi = 0;
  m_pix_eca_Oxresxvsmodphi = 0;
  m_pix_ecc_Oxresxvsmodphi = 0;
  m_pix_eca_Oyresyvsmodphi = 0;
  m_pix_ecc_Oyresyvsmodphi = 0;
  m_pix_eca_unbiased_xresvsmodphi = 0;
  m_pix_eca_unbiased_yresvsmodphi = 0;
  m_pix_ecc_unbiased_xresvsmodphi = 0;
  m_pix_ecc_unbiased_yresvsmodphi = 0;
  m_sct_b_residualx = 0;
  m_sct_b_residualx_fine = 0;
  m_sct_b_biasedresidualx = 0;
  m_sct_eca_residualx = 0;
  m_sct_ecc_residualx = 0;
  m_sct_eca_residualx_fine = 0;
  m_sct_ecc_residualx_fine = 0;
  
  m_sct_eca_pullx = 0;
  m_sct_ecc_pullx = 0;
  	
  m_sct_bec_Oxresx_mean = 0;
  m_sct_bec_Oyresx_mean = 0;
  m_sct_bec_Oxresx_rms = 0;
  m_sct_bec_Oyresx_rms = 0;

  m_sct_eca_xresvsmodphi_2d = 0;
  m_sct_ecc_xresvsmodphi_2d = 0;

  m_sct_b_xresvsmodeta = 0;
  m_sct_b_xresvsmodphi = 0;
  m_sct_eca_xresvsmodphi = 0;
  m_sct_ecc_xresvsmodphi = 0;

  m_sct_b_Oxresxvsmodeta = 0;
  m_sct_b_Oxresxvsmodphi = 0;
  m_sct_b_Oyresxvsmodeta = 0;
  m_sct_b_Oyresxvsmodphi = 0;
  m_sct_eca_Oxresxvsmodphi = 0;
  m_sct_ecc_Oxresxvsmodphi = 0;
  m_pix_b_residualx_pt = 0;
  m_pix_b_residualy_pt = 0;
  m_pix_eca_residualx_pt = 0;
  m_pix_eca_residualy_pt = 0;
  m_pix_ecc_residualx_pt = 0;
  m_pix_ecc_residualy_pt = 0;
  
  m_sct_b_biased_residualx = 0;
  m_sct_b_biased_residualx_pt = 0;
  m_sct_b_residualx_pt = 0;
  m_sct_b_biased_residualx_qoverp2 = 0;
  m_sct_b_unbiased_residualx_qoverp2 = 0;
  
  m_sct_eca_biased_residualx = 0;
  m_sct_eca_biased_residualy = 0;
  m_sct_ecc_biased_residualx = 0;
  m_sct_ecc_biased_residualy = 0;
  
  m_sct_eca_biased_residualx_pt = 0;
  m_sct_ecc_biased_residualx_pt = 0;

  m_sct_eca_residualx_pt = 0;
  m_sct_ecc_residualx_pt = 0;
  
  m_sct_eca_biased_residualx_qoverp2 =0 ;	
  m_sct_ecc_biased_residualx_qoverp2= 0;	

  m_sct_eca_unbiased_residualx_qoverp2= 0;	
  m_sct_ecc_unbiased_residualx_qoverp2= 0;	
  // Local positions (extrapolated)
  m_pix_b_extrapolated_localx= 0;
  m_pix_b_extrapolated_localy= 0;
  m_sct_b_extrapolated_st_localx= 0;
  m_sct_b_extrapolated_st_localy= 0;
  m_sct_b_extrapolated_nst_localx= 0;
  m_sct_b_extrapolated_nst_localy= 0; 
  
  // Local positions (mesasured)
  m_pix_b_measured_localx= 0;
  m_pix_b_measured_localy= 0;
  m_sct_b_measured_st_localx= 0;
  m_sct_b_measured_st_localy= 0;  
  m_sct_b_measured_nst_localx= 0;
  m_sct_b_measured_nst_localy= 0;  
  
  
  // Hit errors
  
  m_hiterror_sct_b= 0;
  m_hiterror_sct_ec= 0;
  m_hiterror_sct_b_WideRange= 0;
  m_hiterror_sct_ec_WideRange= 0;

  m_hiterror_x_pix_b= 0;
  m_hiterror_x_pix_ec= 0;
  m_hiterror_y_pix_b= 0;
  m_hiterror_y_pix_ec= 0;

  m_hiterror_x_pix_b_WideRange= 0;
  m_hiterror_x_pix_ec_WideRange= 0;
  m_hiterror_y_pix_b_WideRange= 0;
  m_hiterror_y_pix_ec_WideRange= 0;

  m_hiterror_x_ibl_b= 0;
  m_hiterror_x_ibl_ec= 0;
  m_hiterror_y_ibl_b= 0;
  m_hiterror_y_ibl_ec= 0;

  m_hiterror_x_ibl_b_WideRange= 0;
  m_hiterror_x_ibl_ec_WideRange= 0;
  m_hiterror_y_ibl_b_WideRange= 0;
  m_hiterror_y_ibl_ec_WideRange= 0;

  // Pulls vs pt
  // Pixel
  
  m_pix_b_pullx_pt= 0;
  m_pix_b_pully_pt= 0;
  m_pix_eca_pullx_pt= 0;
  m_pix_eca_pully_pt= 0;
  m_pix_ecc_pullx_pt= 0;
  m_pix_ecc_pully_pt =0;
}


StatusCode IDAlignMonResiduals::initialize()
{
    
	m_events=0;
	m_histosBooked = 0;
	
	// initialize ranges of residual histograms
	if(m_maxPIXResXFillRange==0.)
		m_maxPIXResXFillRange=m_maxSiResFillRange;
	if(m_minPIXResXFillRange==0.)
		m_minPIXResXFillRange=m_minSiResFillRange;
	if(m_maxPIXResYFillRange==0.)
		m_maxPIXResYFillRange=m_maxSiResFillRange;
	if(m_minPIXResYFillRange==0.)
		m_minPIXResYFillRange= m_minSiResFillRange; // the pixels are broader in Y (400 microns) than in X (50 microns)
	if(m_maxSCTResFillRange==0.)
		m_maxSCTResFillRange= m_maxSiResFillRange;
	if(m_minSCTResFillRange==0.)
		m_minSCTResFillRange=m_minSiResFillRange;  
	
	// Detector sizes
	PixelBarrelXSize = 16.44;  // mm
	PixelBarrelYSize = 60.2;  // mm 
	SCTBarrelXSize = 61.54;  // mm
	SCTBarrelYSize = 128.;  // mm  
	
	if(msgLvl(MSG::VERBOSE)) msg() << ">> Range of histograms: m_minSiResFillRange= "<< m_minSCTResFillRange  << endreq;
	if(msgLvl(MSG::VERBOSE)) msg() << ">> Range of histograms: m_maxSiResFillRange= "<< m_maxSCTResFillRange  << endreq;
	if(msgLvl(MSG::VERBOSE)) msg() << ">> Range of histograms: m_RangeOfPullHistos= "<< m_RangeOfPullHistos  << endreq;
	
	//initialize tools and services
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Calling initialize() to setup tools/services" << endreq;
	StatusCode sc = setupTools();
	if (sc.isFailure()) {
		if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed to initialize tools/services!" << endreq;
		return StatusCode::SUCCESS;
	} 
	else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Successfully initialized tools/services" << endreq;                                   
	
	sc = ManagedMonitorToolBase::initialize();
	if (sc.isFailure()) {
		if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed to initialize ManagedMonitorToolBase!" << endreq;
		return StatusCode::SUCCESS;
	} 
	
	/** Get TRTCalDbTool */
	if (m_trtcaldbSvc.name() == ""){
		if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "TRT_CalDbSvc not given."<<endreq;
	} else {  
		if(m_trtcaldbSvc.retrieve().isFailure()){
			msg(MSG::ERROR) << "Cannot get TRTCalDBSvc !"<<endreq;
		}
	}
	
	// Get the weight histogram
	if(m_applyHistWeight){
	  m_hWeightInFile =  new TFile( m_hWeightInFileName.c_str() ,"read");

	  if (m_hWeightInFile->IsZombie() || !(m_hWeightInFile->IsOpen()) ) {
	    ATH_MSG_FATAL( " Problem reading TFile " << m_hWeightInFileName );
	    return StatusCode::FAILURE;
	  }

	  ATH_MSG_INFO("Opened  file containing the contraints" << m_hWeightInFileName);

	  m_etapTWeight = (TH2F*) m_hWeightInFile -> Get( m_hWeightHistName.c_str() );
	  if( !m_etapTWeight ){
	    ATH_MSG_FATAL( " Problem getting constraints Hist.  Name " << m_hWeightHistName );
	    m_hWeightInFile -> Close();
	    delete m_hWeightInFile;
	    return StatusCode::FAILURE;
	  }
	}

	return StatusCode::SUCCESS;
}

StatusCode IDAlignMonResiduals::bookHistograms()
{
  if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }
  
  if ( AthenaMonManager::dataType() == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }
  
  std::string outputDirName = "IDAlignMon/" + m_tracksName + "_" + m_triggerChainName + "/Residuals";
  //std::cout << " -- SALVA -- histos for track collection: " <<  m_tracksName << "_" << m_triggerChainName << std::endl;

  MonGroup al_mon ( this, outputDirName, run );
	
  if ( newLowStat ) {  
  }
  if ( newLumiBlock ) {  
  }
  if ( newRun ) {
	  
    //if user environment specified we don't want to book new histograms at every run boundary
    //we instead want one histogram per job
    if(m_histosBooked!=0 && AthenaMonManager::environment()==AthenaMonManager::user) return StatusCode::SUCCESS;
	  
    //-----------------------------------
    //
    // Pixel and SCT Shifter Histograms
    //
    //-----------------------------------
	  
    const Int_t nx = 21;
    TString siliconLayers[nx] = {"Pix L0","Pix L1","Pix L2","SCT L0 S0","S1","SCT L1 S0","S1","SCT L2 S0","S1","SCT L3 S0","S1","SCT L4 S0","S1","SCT L5 S0","S1","SCT L6 S0","S1","SCT L7 S0","S1","SCT L8 S0","S1"};  
	  
    //These histograms are filled in post-processing - only initiated here such that they can be registered as "shift".
	  
    //Residual X mean as a function of silicon (Pixel & SCT on one plot) layer for barrel and endcaps
    // m_si_barrel_resX_mean = new TH1F("si_barrel_resX_mean","Mean Residual X vs Silicon Barrel Layer;Mean Residual X",11,-0.5,10.5);  
    //for (int i=1;i<=11;i++) m_si_barrel_resX_mean->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);  
    //RegisterHisto(al_mon,m_si_barrel_resX_mean);
    m_si_eca_resX_mean = new TH1F("si_eca_resX_mean","Mean Residual X vs Silicon ECA Layer; Mean Residual X",21,-0.5,21.5);  
    for (int i=1;i<=nx;i++) m_si_eca_resX_mean->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    RegisterHisto(al_mon,m_si_eca_resX_mean);
    m_si_ecc_resX_mean = new TH1F("si_ecc_resX_mean","Mean Residual X vs Silicon ECC Layer; Mean Residual X",21,-0.5,21.5);  
    for (int i=1;i<=nx;i++) m_si_ecc_resX_mean->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_si_ecc_resX_mean);
    
    //m_si_barrel_resY_mean = new TH1F("si_barrel_resY_mean","Mean Residual Y vs Silicon Barrel Layer;Mean Residual Y",11,-0.5,10.5);  
    //for (int i=1;i<=11;i++) m_si_barrel_resY_mean->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    //RegisterHisto(al_mon,m_si_barrel_resY_mean);
    

    m_si_eca_resY_mean = new TH1F("si_eca_resY_mean","Mean Residual Y vs Silicon ECA Layer;Mean Residual Y",21,-0.5,20.5);  
    for (int i=1;i<=nx;i++) m_si_eca_resY_mean->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    RegisterHisto(al_mon,m_si_eca_resY_mean);
    m_si_ecc_resY_mean = new TH1F("si_ecc_resY_mean","Mean Residual Y vs Silicon ECC Layer;Mean Residual Y",21,-0.5,20.5);  
    for (int i=1;i<=nx;i++) m_si_ecc_resY_mean->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    RegisterHisto(al_mon,m_si_ecc_resY_mean);
		
		
    //-----------------------------------
    //
    // Pixel and SCT Shift Detail Histograms
    //
    //-----------------------------------
    
    //these go into the DQMF Shift Detail folder - DQMF tests are run on these histograms but the shifter 
    //does not look at them unless they fail
 					
    //These histograms are filled in post-processing - only initiated here such that they can be registered as "shift".    
    //Width of Gaussian fit to pull distribution in each layer of the silicon
    //m_si_barrel_pullX_width = new TH1F("si_barrel_pullX_width","Pull X Gaussian Width vs Silicon Barrel Layer",11,-0.5,10.5);  
    //for (int i=1;i<=11;i++) m_si_barrel_pullX_width->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    //m_si_barrel_pullX_width->GetYaxis()->SetTitle("Pull X Gaussian Width");
    //RegisterHisto(al_mon,m_si_barrel_pullX_width);
    m_si_eca_pullX_width = new TH1F("si_eca_pullX_width","Pull X Gaussian Width vs Silicon ECA Layer",21,-0.5,20.5);  
    for (int i=1;i<=nx;i++) m_si_eca_pullX_width->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    m_si_eca_pullX_width->GetYaxis()->SetTitle("Pull X Gaussian Width");
    RegisterHisto(al_mon,m_si_eca_pullX_width);
    m_si_ecc_pullX_width = new TH1F("si_ecc_pullX_width","Pull X Gaussian Width vs Silicon ECC Layer",21,-0.5,20.5);  
    for (int i=1;i<=nx;i++) m_si_ecc_pullX_width->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    m_si_ecc_pullX_width->GetYaxis()->SetTitle("Pull X Gaussian Width");
    RegisterHisto(al_mon,m_si_ecc_pullX_width);
    //m_si_barrel_pullY_width = new TH1F("si_barrel_pullY_width","Pull Y Gaussian Width vs Silicon Barrel Layer",11,-0.5,10.5);  
    //for (int i=1;i<=11;i++) m_si_barrel_pullY_width->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    //m_si_barrel_pullY_width->GetYaxis()->SetTitle("Pull Y Gaussian Width");
    //RegisterHisto(al_mon,m_si_barrel_pullY_width);
    m_si_eca_pullY_width = new TH1F("si_eca_pullY_width","Pull Y Gaussian Width vs Silicon ECA Layer",21,-0.5,20.5);  
    for (int i=1;i<=nx;i++) m_si_eca_pullY_width->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    m_si_eca_pullY_width->GetYaxis()->SetTitle("Pull Y Gaussian Width");
    RegisterHisto(al_mon,m_si_eca_pullY_width);
    m_si_ecc_pullY_width = new TH1F("si_ecc_pullY_width","Pull Y Gaussian Width vs Silicon ECC Layer",21,-0.5,20.5);  
    for (int i=1;i<=nx;i++) m_si_ecc_pullY_width->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    m_si_ecc_pullY_width->GetYaxis()->SetTitle("Pull Y Gaussian Width");
    RegisterHisto(al_mon,m_si_ecc_pullY_width);
		
    //These histograms are filled in post-processing - only initiated here such that they can be registered as "shift".    
    //Residual X RMS as a function of silicon (Pixel & SCT on one plot) layer for barrel and endcaps
    //m_si_barrel_resX_rms = new TH1F("si_barrel_resX_rms","Residual X RMS vs Silicon Barrel Layer",11,-0.5,10.5);  
    //for (int i=1;i<=11;i++) m_si_barrel_resX_rms->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);  
    //m_si_barrel_resX_rms->GetYaxis()->SetTitle("Residual X RMS");  
    //RegisterHisto(al_mon,m_si_barrel_resX_rms);
    m_si_eca_resX_rms = new TH1F("si_eca_resX_rms","Residual X RMS vs Silicon ECA Layer",21,-0.5,20.5);  
    for (int i=1;i<=nx;i++) m_si_eca_resX_rms->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    m_si_eca_resX_rms->GetYaxis()->SetTitle("Residual X RMS");  
    RegisterHisto(al_mon,m_si_eca_resX_rms);
    m_si_ecc_resX_rms = new TH1F("si_ecc_resX_rms","Residual X RMS vs Silicon ECC Layer",21,-0.5,20.5);  
    for (int i=1;i<=nx;i++) m_si_ecc_resX_rms->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    m_si_ecc_resX_rms->GetYaxis()->SetTitle("Residual X RMS");  
    RegisterHisto(al_mon,m_si_ecc_resX_rms);
    //Residual Y (only Pixel is filled currently - don't have SCT Y residuals yet)
    
    m_si_eca_resY_rms = new TH1F("si_eca_resY_rms","Residual Y RMS vs Silicon ECA Layer",21,-0.5,20.5);  
    for (int i=1;i<=nx;i++) m_si_eca_resY_rms->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    m_si_eca_resY_rms->GetYaxis()->SetTitle("Residual Y RMS");  
    RegisterHisto(al_mon,m_si_eca_resY_rms);
    m_si_ecc_resY_rms = new TH1F("si_ecc_resY_rms","Residual Y RMS vs Silicon ECC Layer",21,-0.5,20.5);  
    for (int i=1;i<=nx;i++) m_si_ecc_resY_rms->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    m_si_ecc_resY_rms->GetYaxis()->SetTitle("Residual Y RMS");  
    RegisterHisto(al_mon,m_si_ecc_resY_rms);
		
    //-----------------------------------
    //
    // Pixel and SCT Expert Histograms
    //
    //-----------------------------------
		
    //histogram to record numer of hits which we have problems with ResidualPullCalculator
    m_sirescalcfailure = new TH1F("sirescalcfailure","Failed ResidualPullCalc DetType",2,0,2); 
    RegisterHisto(al_mon,m_sirescalcfailure) ;
		
    //Si (Pix & SCT) residual histograms
    m_si_residualx = new TH1F("si_residualx","Silicon UnBiased X Residual",500,m_minSiResFillRange,m_maxSiResFillRange); 
    RegisterHisto(al_mon,m_si_residualx) ;
    m_si_b_residualx = new TH1F("si_b_residualx","Silicon Barrel Only UnBiased X Residual",500,m_minSiResFillRange,m_maxSiResFillRange);
    RegisterHisto(al_mon,m_si_b_residualx);  
		
    //Pull 2D histograms for filling of Pull Gaussian width and mean plots in post-processing
    
    m_si_eca_pullX = new TH2F("si_eca_pullX","Pull X vs Silicon ECA Layer",21,-0.5,20.5,100,-m_RangeOfPullHistos, m_RangeOfPullHistos);  
    for (int i=1;i<=nx;i++) m_si_eca_pullX->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    RegisterHisto(al_mon,m_si_eca_pullX);
    m_si_ecc_pullX = new TH2F("si_ecc_pullX","Pull X vs Silicon ECC Layer",21,-0.5,20.5,100,-m_RangeOfPullHistos, m_RangeOfPullHistos);  
    for (int i=1;i<=nx;i++) m_si_ecc_pullX->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    RegisterHisto(al_mon,m_si_ecc_pullX);
   
    m_si_eca_pullY = new TH2F("si_eca_pullY","Pull Y vs Silicon ECA Layer",21,-0.5,20.5,100,-m_RangeOfPullHistos, m_RangeOfPullHistos);  
    for (int i=1;i<=nx;i++) m_si_eca_pullY->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    RegisterHisto(al_mon,m_si_eca_pullY);
    m_si_ecc_pullY = new TH2F("si_ecc_pullY","Pull Y vs Silicon ECC Layer",21,-0.5,20.5,100,-m_RangeOfPullHistos, m_RangeOfPullHistos);  
    for (int i=1;i<=nx;i++) m_si_ecc_pullY->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    RegisterHisto(al_mon,m_si_ecc_pullY);
		
    //Residual 2D histograms for filling of Residual Gaussian width and mean plots in post-processing
   
    m_si_eca_resX = new TH2F("si_eca_resX","Residual X vs Silicon ECA Layer",21,-0.5,20.5,100*m_FinerBinningFactor,m_minSiResFillRange,m_maxSiResFillRange);  
    for (int i=1;i<=nx;i++) m_si_eca_resX->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    RegisterHisto(al_mon,m_si_eca_resX);
    m_si_ecc_resX = new TH2F("si_ecc_resX","Residual X vs Silicon ECC Layer",21,-0.5,20.5,100*m_FinerBinningFactor,m_minSiResFillRange,m_maxSiResFillRange);  
    for (int i=1;i<=nx;i++) m_si_ecc_resX->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    RegisterHisto(al_mon,m_si_ecc_resX);
    
    m_si_eca_resY = new TH2F("si_eca_resY","Residual Y vs Silicon ECA Layer",21,-0.5,20.5,100*m_FinerBinningFactor,m_minSiResFillRange,m_maxSiResFillRange);  
    for (int i=1;i<=nx;i++) m_si_eca_resY->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    RegisterHisto(al_mon,m_si_eca_resY);
    m_si_ecc_resY = new TH2F("si_ecc_resY","Residual Y vs Silicon ECC Layer",21,-0.5,20.5,100*m_FinerBinningFactor,m_minSiResFillRange,m_maxSiResFillRange);  
    for (int i=1;i<=nx;i++) m_si_ecc_resY->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    RegisterHisto(al_mon,m_si_ecc_resY);
				
    //These histograms are filled in post-processing - only initiated here such that they can be registered to dqm
    //mean of Gaussian fit to pull distribution in each layer of the silicon
    
    m_si_eca_pullX_mean = new TH1F("si_eca_pullX_mean","Pull X Gaussian Mean vs Silicon ECA Layer",21,-0.5,20.5);  
    for (int i=1;i<=nx;i++) m_si_eca_pullX_mean->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    RegisterHisto(al_mon,m_si_eca_pullX_mean);
    m_si_ecc_pullX_mean = new TH1F("si_ecc_pullX_mean","Pull X Gaussian Mean vs Silicon ECC Layer",21,-0.5,20.5);  
    for (int i=1;i<=nx;i++) m_si_ecc_pullX_mean->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    RegisterHisto(al_mon,m_si_ecc_pullX_mean);
    
    m_si_eca_pullY_mean = new TH1F("si_eca_pullY_mean","Pull Y Gaussian Mean vs Silicon ECA Layer",21,-0.5,20.5);  
    for (int i=1;i<=nx;i++) m_si_eca_pullY_mean->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    RegisterHisto(al_mon,m_si_eca_pullY_mean);
    m_si_ecc_pullY_mean = new TH1F("si_ecc_pullY_mean","Pull Y Gaussian Mean vs Silicon ECC Layer",21,-0.5,20.5);  
    for (int i=1;i<=nx;i++) m_si_ecc_pullY_mean->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);    
    RegisterHisto(al_mon,m_si_ecc_pullY_mean);		
		
    //Overlap Residual Plots - Pixel Mean and RMS vs Layer histograms (concatenated - both barrel and endcaps in one plot)
    m_pix_bec_Oxresx_mean = new TProfile("pix_bec_Oxresx_mean","X_Overlap Residual X Mean vs Pixel Layer (ECC, Barrel, ECA)",11,-3.0,8.0,m_minPIXResXFillRange,m_maxPIXResXFillRange);
    RegisterHisto(al_mon,m_pix_bec_Oxresx_mean);
    m_pix_bec_Oxresx_rms = new TProfile("pix_bec_Oxresx_rms","X_Overlap Residual X RMS vs Pixel Layer (ECC, Barrel, ECA)",11,-3.0,8.0,m_minPIXResXFillRange,m_maxPIXResXFillRange);
    RegisterHisto(al_mon,m_pix_bec_Oxresx_rms);
    m_pix_bec_Oxresy_mean = new TProfile("pix_bec_Oxresy_mean","X_Overlap Residual Y Mean vs Pixel Layer (ECC, Barrel, ECA)",11,-3.0,8.0,m_minPIXResYFillRange,m_maxPIXResYFillRange);
    RegisterHisto(al_mon,m_pix_bec_Oxresy_mean);
    m_pix_bec_Oxresy_rms = new TProfile("pix_bec_Oxresy_rms","X_Overlap Residual Y RMS vs Pixel Layer (ECC, Barrel, ECA)",11,-3.0,8.0,m_minPIXResYFillRange,m_maxPIXResYFillRange);
    RegisterHisto(al_mon,m_pix_bec_Oxresy_rms);
		
    m_pix_bec_Oyresy_mean = new TProfile("pix_bec_Oyresy_mean","Y_Overlap Residual Y Mean vs Pixel Layer (ECC, Barrel, ECA)",11,-3.0,8.0,m_minPIXResYFillRange,m_maxPIXResYFillRange);
    RegisterHisto(al_mon,m_pix_bec_Oyresy_mean);
    m_pix_bec_Oyresy_rms = new TProfile("pix_bec_Oyresy_rms","Y_Overlap Residual Y RMS vs Pixel Layer (ECC, Barrel, ECA)",11,-3.0,8.0,m_minPIXResYFillRange,m_maxPIXResYFillRange);
    RegisterHisto(al_mon,m_pix_bec_Oyresy_rms);
    m_pix_bec_Oyresx_mean = new TProfile("pix_bec_Oyresx_mean","Y_Overlap Residual X Mean vs Pixel Layer (ECC, Barrel, ECA)",11,-3.0,8.0,m_minPIXResXFillRange,m_maxPIXResXFillRange);
    RegisterHisto(al_mon,m_pix_bec_Oyresx_mean);
    m_pix_bec_Oyresx_rms = new TProfile("pix_bec_Oyresx_rms","Y_Overlap Residual X RMS vs Pixel Layer (ECC, Barrel, ECA)",11,-3.0,8.0,m_minPIXResXFillRange,m_maxPIXResXFillRange);
    RegisterHisto(al_mon,m_pix_bec_Oyresx_rms);
    
		
    //Overlap Residual Plots - SCT Mean and RMS vs Layer histograms (concatenated - both barrel and endcaps in one plot)
    m_sct_bec_Oxresx_mean = new TProfile("sct_bec_Oxresx_mean","X_Overlap Residual X Mean vs SCT Layer (ECC, Barrel, ECA)",24,-9.0,15.0,m_minSCTResFillRange,m_maxSCTResFillRange);
    RegisterHisto(al_mon,m_sct_bec_Oxresx_mean);
    m_sct_bec_Oxresx_rms = new TProfile("sct_bec_Oxresx_rms","X_Overlap Residual X RMS vs SCT Layer (ECC, Barrel, ECA)",24,-9.0,15.0,m_minSCTResFillRange,m_maxSCTResFillRange);
    RegisterHisto(al_mon,m_sct_bec_Oxresx_rms);
    m_sct_bec_Oyresx_mean = new TProfile("sct_bec_Oyresx_mean","Y_Overlap Residual X Mean vs SCT Layer (ECC, Barrel, ECA)",24,-9.0,15.0,m_minSCTResFillRange,m_maxSCTResFillRange);
    RegisterHisto(al_mon,m_sct_bec_Oyresx_mean);
    m_sct_bec_Oyresx_rms = new TProfile("sct_bec_Oyresx_rms","Y_Overlap Residual X RMS vs SCT Layer (ECC, Barrel, ECA)",24,-9.0,15.0,m_minSCTResFillRange,m_maxSCTResFillRange);
    RegisterHisto(al_mon,m_sct_bec_Oyresx_rms);
	
    //Special histograms
    if (m_extendedPlots)
      {
	m_totalEvents  = new TH1F("TotalEvents","TotalEvents",2,0,2);
	RegisterHisto(al_mon,m_totalEvents);
      
	m_mu_perEvent = new TH1F("mu_perEvent","<#mu> per event",m_nBinsMuRange,m_muRangeMin,m_muRangeMax);
	RegisterHisto(al_mon,m_mu_perEvent);
      }
    //Lumi wise histo

    //All modules 
    m_pix_b0_resXvsetaLumiBlock = new TProfile2D("pix_b0_resXvsetaLumiBlock","2D profile of X unbiased residuals vs IBL eta module per Lumi Block; LumiBlock;Module Eta",m_nBinsLB,m_LBRangeMin,m_LBRangeMax,20,-10.5,9.5,m_minPIXResXFillRange,m_maxPIXResXFillRange);
    RegisterHisto(al_mon,m_pix_b0_resXvsetaLumiBlock);
    
    //Only planars
    
    m_pix_b0_resXvsetaLumiBlock_planars = new TProfile2D("pix_b0_resXvsetaLumiBlock_planars","2D profile of X unbiased residuals vs IBL eta module per Lumi Block;LumiBlock; Module Eta",m_nBinsLB,m_LBRangeMin,m_LBRangeMax,12,-6.5,5.5,m_minPIXResXFillRange,m_maxPIXResXFillRange);
    RegisterHisto(al_mon,m_pix_b0_resXvsetaLumiBlock_planars);

    
    if (m_doIBLLBPlots )
      {

	//All modules
	m_pix_b0_resXvsetaLumiBlock_3d = new TH3F("pix_b0_resXvsetaLumiBlock_3d","TH3 of X unbiased residuals vs IBL eta module per Lumi Block; LumiBlock;Module Eta",
						  m_nBinsLB, m_LBRangeMin, m_LBRangeMax,
						  20, -10.5, 9.5, 
						  50*m_FinerBinningFactor, m_minPIXResXFillRange,m_maxPIXResXFillRange);
	RegisterHisto(al_mon,m_pix_b0_resXvsetaLumiBlock_3d);
	
    //Only planars
    
	m_pix_b0_resXvsetaLumiBlock_planars_3d = new TH3F("pix_b0_resXvsetaLumiBlock_planars_3d","TH3 of X unbiased residuals vs IBL eta module per Lumi Block;LumiBlock; Module Eta",m_nBinsLB,m_LBRangeMin,m_LBRangeMax,12,-6.5,5.5,50*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
	RegisterHisto(al_mon,m_pix_b0_resXvsetaLumiBlock_planars_3d);
	
	m_pix_b0_resXvsetaLumiBlock_stave0 = new TH3F("pix_b0_resXvsetaLumiBlock_stave0","TH3 of X unbiased residuals vs IBL eta module per Lumi Block for stave 0;LumiBlock; Module Eta",m_nBinsLB,m_LBRangeMin,m_LBRangeMax,20,-10.5,9.5,50*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
	RegisterHisto(al_mon,m_pix_b0_resXvsetaLumiBlock_stave0);
  
	m_pix_b0_resXvsetaLumiBlock_stave1 = new TH3F("pix_b0_resXvsetaLumiBlock_stave1","TH3 of X unbiased residuals vs IBL eta module per Lumi Block for stave 1;LumiBlock; Module Eta",m_nBinsLB,m_LBRangeMin,m_LBRangeMax,20,-10.5,9.5,50*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
	RegisterHisto(al_mon,m_pix_b0_resXvsetaLumiBlock_stave1);
  
	m_pix_b0_resXvsetaLumiBlock_stave2 = new TH3F("pix_b0_resXvsetaLumiBlock_stave2","TH3 of X unbiased residuals vs IBL eta module per Lumi Block for stave 2;LumiBlock; Module Eta",m_nBinsLB,m_LBRangeMin,m_LBRangeMax,20,-10.5,9.5,50*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
	RegisterHisto(al_mon,m_pix_b0_resXvsetaLumiBlock_stave2);
  
	m_pix_b0_resXvsetaLumiBlock_stave3 = new TH3F("pix_b0_resXvsetaLumiBlock_stave3","TH3 of X unbiased residuals vs IBL eta module per Lumi Block for stave 3;LumiBlock; Module Eta",m_nBinsLB,m_LBRangeMin,m_LBRangeMax,20,-10.5,9.5,50*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
	RegisterHisto(al_mon,m_pix_b0_resXvsetaLumiBlock_stave3);

	m_pix_b0_resXvsetaLumiBlock_stave4 = new TH3F("pix_b0_resXvsetaLumiBlock_stave4","TH3 of X unbiased residuals vs IBL eta module per Lumi Block for stave 4;LumiBlock; Module Eta",m_nBinsLB,m_LBRangeMin,m_LBRangeMax,20,-10.5,9.5,50*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
	RegisterHisto(al_mon,m_pix_b0_resXvsetaLumiBlock_stave4);
	
	m_pix_b0_resXvsetaLumiBlock_stave5 = new TH3F("pix_b0_resXvsetaLumiBlock_stave5","TH3 of X unbiased residuals vs IBL eta module per Lumi Block for stave 5;LumiBlock; Module Eta",m_nBinsLB,m_LBRangeMin,m_LBRangeMax,20,-10.5,9.5,50*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
	RegisterHisto(al_mon,m_pix_b0_resXvsetaLumiBlock_stave5);
	
	m_pix_b0_resXvsetaLumiBlock_stave6 = new TH3F("pix_b0_resXvsetaLumiBlock_stave6","TH3 of X unbiased residuals vs IBL eta module per Lumi Block for stave 6;LumiBlock; Module Eta",m_nBinsLB,m_LBRangeMin,m_LBRangeMax,20,-10.5,9.5,50*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
	RegisterHisto(al_mon,m_pix_b0_resXvsetaLumiBlock_stave6);
	
	m_pix_b0_resXvsetaLumiBlock_stave7 = new TH3F("pix_b0_resXvsetaLumiBlock_stave7","TH3 of X unbiased residuals vs IBL eta module per Lumi Block for stave 7;LumiBlock; Module Eta",m_nBinsLB,m_LBRangeMin,m_LBRangeMax,20,-10.5,9.5,50*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
	RegisterHisto(al_mon,m_pix_b0_resXvsetaLumiBlock_stave7);
    
	m_pix_b0_resXvsetaLumiBlock_stave8 = new TH3F("pix_b0_resXvsetaLumiBlock_stave8","TH3 of X unbiased residuals vs IBL eta module per Lumi Block for stave 8;LumiBlock; Module Eta",m_nBinsLB,m_LBRangeMin,m_LBRangeMax,20,-10.5,9.5,50*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
	RegisterHisto(al_mon,m_pix_b0_resXvsetaLumiBlock_stave8);
	
	m_pix_b0_resXvsetaLumiBlock_stave9 = new TH3F("pix_b0_resXvsetaLumiBlock_stave9","TH3 of X unbiased residuals vs IBL eta module per Lumi Block for stave 9;LumiBlock; Module Eta",m_nBinsLB,m_LBRangeMin,m_LBRangeMax,20,-10.5,9.5,50*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
	RegisterHisto(al_mon,m_pix_b0_resXvsetaLumiBlock_stave9);
	
	m_pix_b0_resXvsetaLumiBlock_stave10 = new TH3F("pix_b0_resXvsetaLumiBlock_stave10","TH3 of X unbiased residuals vs IBL eta module per Lumi Block for stave 10;LumiBlock; Module Eta",m_nBinsLB,m_LBRangeMin,m_LBRangeMax,20,-10.5,9.5,50*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
	RegisterHisto(al_mon,m_pix_b0_resXvsetaLumiBlock_stave10);
  
	m_pix_b0_resXvsetaLumiBlock_stave11 = new TH3F("pix_b0_resXvsetaLumiBlock_stave11","TH3 of X unbiased residuals vs IBL eta module per Lumi Block for stave 11;LumiBlock; Module Eta",m_nBinsLB,m_LBRangeMin,m_LBRangeMax,20,-10.5,9.5,50*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
	RegisterHisto(al_mon,m_pix_b0_resXvsetaLumiBlock_stave11);
    
	m_pix_b0_resXvsetaLumiBlock_stave12 = new TH3F("pix_b0_resXvsetaLumiBlock_stave12","TH3 of X unbiased residuals vs IBL eta module per Lumi Block for stave 12;LumiBlock; Module Eta",m_nBinsLB,m_LBRangeMin,m_LBRangeMax,20,-10.5,9.5,50*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
	RegisterHisto(al_mon,m_pix_b0_resXvsetaLumiBlock_stave12);
  
	m_pix_b0_resXvsetaLumiBlock_stave13 = new TH3F("pix_b0_resXvsetaLumiBlock_stave13","TH3 of X unbiased residuals vs IBL eta module per Lumi Block for stave 13;LumiBlock; Module Eta",m_nBinsLB,m_LBRangeMin,m_LBRangeMax,20,-10.5,9.5,50*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
	RegisterHisto(al_mon,m_pix_b0_resXvsetaLumiBlock_stave13);	

      }
    
    
    //mag + base as function of lb
    
    m_mag_vs_LB = new TH1D("mag_vs_LB","IBL 2pi averaged bowing magnitude vs LumiBlock;LumiBlock;Magnitude [mm]",m_nBinsLB, m_LBRangeMin,m_LBRangeMax);
    RegisterHisto(al_mon,m_mag_vs_LB);
    
    m_base_vs_LB = new TH1D("base_vs_LB","IBL 2pi averaged bowing baseline vs LumiBlock;LumiBlock;Baseline [mm]",m_nBinsLB, m_LBRangeMin,m_LBRangeMax);
    RegisterHisto(al_mon,m_base_vs_LB);


    m_mag_vs_LB_planars  = new TH1D("mag_vs_LB_planars","IBL 2pi averaged bowing magnitude vs LumiBlock;LumiBlock;Magnitude [mm]",m_nBinsLB, m_LBRangeMin,m_LBRangeMax);
    RegisterHisto(al_mon,m_mag_vs_LB_planars);
    
    m_base_vs_LB_planars = new TH1D("base_vs_LB_planars","IBL 2pi averaged bowing baseline vs LumiBlock;LumiBlock;Baseline [mm]",m_nBinsLB, m_LBRangeMin,m_LBRangeMax);
    RegisterHisto(al_mon,m_base_vs_LB_planars);


    std::cout<<"INITIALIZED GENERALHISTOS FOR RESIDUALS"<<std::endl;
    
    //PrintIBLGeometry();
    MakePIXBarrelHistograms (al_mon);
    MakePIXEndCapsHistograms(al_mon);  
    MakeSCTBarrelHistograms (al_mon);
    MakeSCTEndcapsHistograms(al_mon);
    
    //The TRT Histograms
    MakeTRTHistograms(al_mon);
	
    MakeSiliconHistograms(al_mon);
    std::cout<<"INITIALIZED GENERALHISTOS FOR RESIDUALS 2"<<std::endl;
    ++m_histosBooked;
  }
	
  return StatusCode::SUCCESS;
  
}

/** Convert from an int to a string */
std::string IDAlignMonResiduals::intToString(int input){
  std::ostringstream stm;
  stm << input;
  return stm.str();
}

/** Make Histograms */
TH1F_LW* IDAlignMonResiduals::MakeHist(const std::string& name, const std::string& title, int nBins, float xLow, float xUp, const std::string& xAxisTitle, const std::string& yAxisTitle) {
  TH1F_LW* hist = TH1F_LW::create(name.c_str(), title.c_str(), nBins, xLow, xUp);
  hist->GetXaxis()->SetLabelSize(0.03);
  hist->GetYaxis()->SetLabelSize(0.03);
  hist->GetXaxis()->SetTitle(xAxisTitle.c_str());
  hist->GetYaxis()->SetTitle(yAxisTitle.c_str());
  return hist;
}

TH2F* IDAlignMonResiduals::MakeHist(const std::string& name, const std::string& title, int nXBins, float xLow, float xUp, int nYBins, float yLow, float yUp, const std::string& xAxisTitle, const std::string& yAxisTitle) {
  TH2F* hist = new TH2F(name.c_str(), title.c_str(), nXBins, xLow, xUp, nYBins, yLow, yUp);
  hist->GetXaxis()->SetLabelSize(0.03);
  hist->GetYaxis()->SetLabelSize(0.03);
  hist->GetXaxis()->SetTitle(xAxisTitle.c_str());
  hist->GetYaxis()->SetTitle(yAxisTitle.c_str());
  return hist;
}

/** Make Profiles */
TProfile* IDAlignMonResiduals::MakeProfile(const std::string& name, const std::string& title, int nBins, float xLow, float xUp, float yLow, float yUp, const std::string& xAxisTitle, const std::string& yAxisTitle, bool setMinZero) {
  TProfile* profile = new TProfile(name.c_str(),title.c_str(),nBins,xLow, xUp, yLow, yUp);
  if(setMinZero)
    profile->SetMinimum(0);
  profile->GetXaxis()->SetLabelSize(0.03);
  profile->GetYaxis()->SetLabelSize(0.03);
  profile->GetXaxis()->SetTitle(xAxisTitle.c_str());
  profile->GetYaxis()->SetTitle(yAxisTitle.c_str());    
  
  return profile;
}


void IDAlignMonResiduals::RegisterHisto(MonGroup& mon, TProfile2D* histo) {
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot book TProfile2D Histogram:" << endreq;
  }
}

void IDAlignMonResiduals::RegisterHisto(MonGroup& mon, TH3* histo) {
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot book TH3 Histogram:" << endreq;
  }
}


void IDAlignMonResiduals::RegisterHisto(MonGroup& mon, TH1* histo) {
  
  //histo->Sumw2(); this uses a lot of memory and isn't needed!
  histo->SetOption("e");
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot book TH1 Histogram:" << endreq;
  }
}

void IDAlignMonResiduals::RegisterHisto(MonGroup& mon, TH1F_LW* histo) {
  
  //histo->Sumw2(); this uses a lot of memory and isn't needed!
  //histo->SetOption("e");
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot book TH1F_LW Histogram:" << endreq;
  }
}

void IDAlignMonResiduals::RegisterHisto(MonGroup& mon, TProfile* histo) {
  
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot book TProfile Histogram:" << endreq;
  }
}

void IDAlignMonResiduals::RegisterHisto(MonGroup& mon, TH2* histo) {
  
  //histo->Sumw2(); this uses a lot of memory and isn't needed!
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot book TH2 Histogram:" << endreq;
  }
}

//---------------------------------------------------------------------------------------

StatusCode IDAlignMonResiduals::fillHistograms()
{
  
  ++m_events;

  m_hasBeenCalledThisEvent=false;
  m_mu=0.;

  const DataHandle<xAOD::EventInfo> eventInfo;
  if (StatusCode::SUCCESS != evtStore()->retrieve( eventInfo ) ){
    msg(MSG::ERROR) << "Cannot get event info." << endreq;
  }
  
  
  m_changedlumiblock = false;
  lumiblock = eventInfo->lumiBlock();
  if (lumiblock!=m_oldlumiblock){   //Changes every m_LBGranularity
    m_oldlumiblock=lumiblock;
    m_changedlumiblock=true;
  }
  
  if (!m_hasBeenCalledThisEvent){
    m_mu = eventInfo->averageInteractionsPerCrossing();
    m_hasBeenCalledThisEvent=true;
  }
  else
    m_mu = -999;
  
  if (m_extendedPlots){
    m_mu_perEvent->Fill(m_mu);
    m_totalEvents->Fill(0);
  }
  
  if (evtStore()->contains<ComTime>(m_comTimeObjectName)){
    if(evtStore()->retrieve(theComTime, m_comTimeObjectName).isFailure() ){
      if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "ComTime object not found with name " << m_comTimeObjectName << "!!!" << endreq;
      //return StatusCode::FAILURE;
    }
    else{
      if (msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "ComTime object found successfully " << endreq;
    }
  }
	
  float timeCor=0.0;
  if(theComTime){
    timeCor = theComTime->getTime();
  }
  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " ** IDAlignMonResiduals::fillHistograms() ** going to fill histos for " << m_tracksName << " tracks" << endreq;
  if (!evtStore()->contains<TrackCollection>(m_tracksName)) {
    if(m_events == 1) {if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Unable to get " << m_tracksName << " tracks from TrackCollection" << endreq;}
    else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " ** IDAlignMonResiduals::fillHistograms() ** Unable to get " << m_tracksName << " tracks from TrackCollection " << endreq;
    return StatusCode::SUCCESS;
  }

  // Code is able to get a weight from an external file and appy it to all histograms
  double hweight = 1.;
  // NB the weight is a "per track" weight, so histograms such as BS info are never weighted

	
  DataVector<Trk::Track>* tracks = m_trackSelection->selectTracks(m_tracksName);
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "IDAlignMonResiduals::fillHistograms() -- event: " << m_events
					 << " with Track collection " << m_tracksName << " has size =" << tracks->size()
					 << endreq;
  
  int nTracks = 0;
  int nHitsEvent = 0;
  
  //DataVector<Trk::Track>::const_iterator trackItr  = tracks->begin();
  //DataVector<Trk::Track>::const_iterator trackItrE = tracks->end();
  for (DataVector<Trk::Track>::const_iterator trackItr  = tracks->begin(); trackItr != tracks->end(); ++trackItr) { //looping over tracks
        
    //need to get the Trk::Track object from which the TrackParticle object was created
    //this has the hit information
    const Trk::Track* track = *trackItr;
    if(track == NULL){
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No associated Trk::Track object found for track "<< nTracks << endreq;
      continue;
    }
		
    //debugging - check that all TSOS of track have track parameters defined (required to compute residuals/pulls)
    
    if(msgLvl(MSG::DEBUG)){
      if(trackRequiresRefit(track)){
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Not all TSOS contain track parameters - will be missing residuals/pulls " << endreq;
      }
      else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "All TSOS contain track parameters - Good! " << endreq;
    }
		
    //trackStateOnSurfaces is a vector of Trk::TrackStateOnSurface objects which contain information 
    //on track at each (inner)detector surface it crosses eg hit used to fit track
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "******************* Track = " << nTracks << endreq;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "******************* Track Pointer = " << track << endreq;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found " << track->trackStateOnSurfaces()->size() << " TrkSurfaces " << endreq;
		
    int nHits =  0;//counts number of tsos from which we can define residual/pull
    int nTSOS = -1;//counts all TSOS on the track
    float trkpt      = -999;
    float trkqoverp2 = -999;
    float trketa_w   = -999;
    
    if(m_extendedPlots)
      {
	trkqoverp2 =  track->perigeeParameters()->parameters()[Trk::qOverP]*fabs(track->perigeeParameters()->parameters()[Trk::qOverP])*1000000.;
      }

    trkpt    = (track->perigeeParameters()->charge())*(track->perigeeParameters()->pT())/1000.; // signed and in GeV?
    trketa_w =  track->perigeeParameters()->eta();

    // per track weight, if required
    if ( m_applyHistWeight ){ 
      int binNumber = m_etapTWeight->FindBin( trketa_w, trkpt );
      hweight       = m_etapTWeight->GetBinContent( binNumber );
    }

    //looping over the hits
    for (std::vector<const Trk::TrackStateOnSurface*>::const_iterator iter_tsos=(track->trackStateOnSurfaces()->begin());
	 iter_tsos!=track->trackStateOnSurfaces()->end(); ++iter_tsos) {//looping over hits
					
      ++nTSOS;
			
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "***************** TSOS (hit) = " << nTSOS << endreq;       
			
      if ((*iter_tsos) == NULL) continue;
			
      //skipping outliers
      if(!(*iter_tsos)->type(Trk::TrackStateOnSurface::Measurement)) {
      	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Skipping TSOS " << nTSOS << " because it is an outlier (or the first TSOS on the track)" << endreq;
      continue;
      }		
      const Trk::MeasurementBase* mesh =(*iter_tsos)->measurementOnTrack();
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Defined hit MeasurementBase " << endreq;
			
      //Trk::RIO_OnTrack object contains information on the hit used to fit the track at this surface
      const Trk::RIO_OnTrack* hit = dynamic_cast <const Trk::RIO_OnTrack*>(mesh);
      if (hit== NULL) { 
	//for some reason the first tsos has no associated hit - maybe because this contains the defining parameters?
	if (nHits > 0 && msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No hit associated with TSOS "<< nTSOS << endreq; 
	continue;
      }
			
      const Trk::TrackParameters* trackParameter = (*iter_tsos)->trackParameters();
      if(trackParameter==NULL) {
	//if no TrackParameters for TSOS we cannot define residuals
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Skipping TSOS " << nTSOS << " because does not have TrackParameters" << endreq;
	continue;
      }
      
      //getting the incident angle
      //should be activated only offline
      float incidencePhi = -999;
      float incidenceTheta = -999;
      
      const Trk::TrkDetElementBase *de = hit->detectorElement(); //  rio is the measurment at a particular suface
      const InDetDD::SiDetectorElement *side = dynamic_cast<const
	InDetDD::SiDetectorElement *>(de);
      if(side){
	Amg::Vector3D mytrack = trackParameter->momentum(); // track parameters at that surface
	Amg::Vector3D mynormal = side->normal();
	Amg::Vector3D myphiax = side->phiAxis();
	Amg::Vector3D myetaax = side->etaAxis();
	float trketacomp = mytrack.dot(myetaax);
	float trkphicomp = mytrack.dot(myphiax);
	float trknormcomp = mytrack.dot(mynormal);
	incidencePhi = atan2(trkphicomp,trknormcomp);
	incidenceTheta = atan2(trketacomp,trknormcomp);
      }
            
      const AmgSymMatrix(5)* TrackParCovariance = trackParameter ? trackParameter->covariance() : NULL;

      if(TrackParCovariance==NULL) {
	//if no MeasuredTrackParameters the hit will not have associated convariance error matrix and will not 
	//be able to define a pull or unbiased residual (errors needed for propagation)
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Skipping TSOS " << nTSOS << " because does not have MeasuredTrackParameters" << endreq;
	continue;
      }

      float residualX = 9999.0;
      float residualY = 9999.0;
      float pullX = 9999.0;
      float pullY = 9999.0;
      float biasedResidualX = 9999.0;
      float biasedResidualY = 9999.0;
      float biasedPullX = 9999.0;
      //float biasedPullY = 9999.0;
      float overlapXResidualX = 9999.0;
      float overlapXResidualY = 9999.0;
      float overlapYResidualX = 9999.0;
      float overlapYResidualY = 9999.0;
      float elocalX = 9999.0;
      float elocalY = 9999.0;
      float mlocalX = 9999.0;
      float mlocalY = 9999.0;
      float hitErrorX = -9999.0;
      float hitErrorY = -9999.0;
      //float globR = -9999.0;
      //Needed for the TRT
      int detType    = 99;
      //int detType    = 0;
      int barrelEC   = 99;
      int layerDisk  = 99;
      int sctSide    = 99;
      int modEta     = 9999;
      int modPhi     = 9999;
      bool isStereo      = false;
      bool foundXOverlap = false;
      bool foundYOverlap = false;

      //Cluster Size

      int ClusSizePhi = -999;
      int ClusSizeZ   = -999;
      int ClusSize    = -999;
      			
      const Identifier & hitId = hit->identify();
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Defined  hit Identifier " << endreq;
      if (m_idHelper->is_trt(hitId)) detType = 2;
      else if (m_idHelper->is_sct(hitId)) detType = 1;
      else  detType = 0;
      if(detType==2){
	//have identified a TRT hit
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Hit is from the TRT, finding residuals... " << endreq;
	bool isTubeHit = (mesh->localCovariance()(Trk::locX,Trk::locX) > 1.0) ? 1 : 0;			
	const Trk::TrackParameters* trackParameter = (*iter_tsos)->trackParameters();
	float hitR = hit->localParameters()[Trk::driftRadius];
	float trketa = (*iter_tsos)->trackParameters()->eta();
	float pullR = -9.9;
	
	const Identifier& id = m_trtID->layer_id(hitId);
	int m_barrel_ec      = m_trtID->barrel_ec(id);
	int m_layer_or_wheel = m_trtID->layer_or_wheel(id);
	int m_phi_module     = m_trtID->phi_module(id);
	int m_straw_layer    = m_trtID->straw_layer(id);

	//finding residuals
	if(!trackParameter){
	  if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No TrackParameters associated with TRT TrkSurface "<<nTSOS<< endreq;
	  continue;
	}
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found Trk::TrackParameters" << endreq;
	
	//getting unbiased track parameters by removing the hit from the track and refitting
	const Trk::TrackParameters* trackParameterUnbiased = getUnbiasedTrackParameters(track,*iter_tsos);
	
	if(!trackParameterUnbiased){//updator can fail
	  if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot define unbiased parameters for hit, skipping it." << endreq;
	  continue;
	}
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found UnBiased TrackParameters" << endreq;
	
	float perdictR = trackParameterUnbiased->parameters()[Trk::locR];
				
	//Theoretically there should be no difference in the pull, but in practice, there can be differences
	bool isPullUnbiased = true;
				
	const Trk::MeasurementBase* mesh =(*iter_tsos)->measurementOnTrack(); //This has been defined in line 869. Should be safe to keep it without redefinition
	//Not clear to me here. isPullUnbiased is set to true. Why inside the Function I am checking if it is true or not? Useless I would say.
	const Trk::ResidualPull* residualPull = m_residualPullCalculator->residualPull(mesh, 
										       trackParameterUnbiased, 
										       (isPullUnbiased) ? 
										       Trk::ResidualPull::Unbiased :
										       Trk::ResidualPull::Biased);
	if (residualPull) {
	  pullR = residualPull->pull()[Trk::locR];
	}
	else {
	  if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " no covariance of the track parameters given, can not calc pull!" << endreq;
	}
				
	delete trackParameterUnbiased;
	delete residualPull;
				
	float residualR = hitR - perdictR;
	//float eventPhase = timeCor;  I think this is useless. I would prefer not to assign this and pass directly timeCor

	const InDet::TRT_DriftCircleOnTrack *trtCircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>((*iter_tsos)->measurementOnTrack());

	
	  
	
	const InDet::TRT_DriftCircle *RawDriftCircle(NULL);
	
	if (trtCircle!=NULL)
	  {
	    ATH_MSG_DEBUG("Getting RawDriftCircle");
	    RawDriftCircle = dynamic_cast<const InDet::TRT_DriftCircle*>(trtCircle->prepRawData());
	  }
	else
	  ATH_MSG_DEBUG("trtCircle is a NULL pointer");
	
	if ( trtCircle != NULL){
	  bool isValid;
	  float leadingEdge = -999;
	  if (RawDriftCircle != NULL)
	    {
	      leadingEdge=RawDriftCircle->driftTime(isValid);
	    }
	  else 
	    ATH_MSG_DEBUG("RawDriftCircles are NULL pointer");
	  
	  Identifier DCoTId = trtCircle->identify();   
	  float t0 = m_trtcaldbSvc->getT0(DCoTId, TRTCond::ExpandedIdentifier::STRAW);
	  
	  ATH_MSG_DEBUG("Filling TRT HISTOS");

	  // Global positions
	  float trkz0 = track->perigeeParameters()->parameters()[Trk::z0];
	  float theta = track->perigeeParameters()->parameters()[Trk::theta];
	  /*  Estimates the global Z position of a TRT barrel hit by using the x,y position of the 
	      straw and the track theta */	  
	  float hitZ = -999.;
	  Amg::Vector3D  center(-9999.,-9999.,-9999);;
	  if (RawDriftCircle!=NULL)
	    center = RawDriftCircle->detectorElement()->surface( id ).center() ;
	  
	  if( fabs(m_barrel_ec) == 1 && RawDriftCircle!=NULL){
	    hitZ = sqrt(center.x()*center.x()+center.y()*center.y())*tan(M_PI/2. - theta) + trkz0;
	    //std::cout << "z: " << hitZ << std::endl;
	  }
	  /* Estimates the global R position of a TRT EC hit using the global z position of the straw
	     and the track theta */
	  float hitGlobalR = -9999.; // -999. is a possible value :)
	  if( fabs(m_barrel_ec) == 2 && RawDriftCircle!=NULL){
	    hitGlobalR = (center.z() - trkz0) / tan(M_PI/2. - theta);
	    //std::cout << "R: " << hitGlobalR << std::endl;
	  }
	  
	  /** filling TRT histograms */
	  fillTRTHistograms(m_barrel_ec
			    ,m_layer_or_wheel
			    ,m_phi_module
			    ,m_straw_layer
			    ,perdictR
			    ,hitR
			    ,hitZ
			    ,hitGlobalR
			    ,residualR
			    ,pullR
			    ,leadingEdge
			    ,timeCor
			    ,t0
			    ,isTubeHit
			    ,trketa
			    ,trkpt, hweight);
	}
      }//if detType ==2
      
      //if (detType==0 || detType==1) 
      else {//have identified pixel or SCT hit
	
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Hit is pixel or SCT, finding residuals... " << endreq;
	if(m_doHitQuality) {
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "applying hit quality cuts to Silicon hit..." << endreq;
	  
	  hit = m_hitQualityTool->getGoodHit(*iter_tsos);
	  if(hit==NULL) {
	    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "hit failed quality cuts and is rejected." << endreq;
	    continue;
	  }
	  else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "hit passed quality cuts" << endreq;
	}
	else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "hit quality cuts NOT APPLIED to Silicon hit." << endreq;
	
	if (de){
	  const InDetDD::SiDetectorElement *side = dynamic_cast<const InDetDD::SiDetectorElement *>(de);
	  if(side){
	    Amg::Vector3D mytrack = trackParameter->momentum(); // track parameters at that surface
	    Amg::Vector3D mynormal = side->normal();
	    Amg::Vector3D myphiax = side->phiAxis();
	    Amg::Vector3D myetaax = side->etaAxis();
	    float trketacomp = mytrack.dot(myetaax);
	    float trkphicomp = mytrack.dot(myphiax);
	    float trknormcomp = mytrack.dot(mynormal);
	    incidencePhi = atan2(trkphicomp,trknormcomp);
	    incidenceTheta = atan2(trketacomp,trknormcomp);
	  }
	}
	
	
	//determining Si module physical position (can modify residual calculation eg. SCT endcaps)
	if (detType==0){//pixel
	  
	  const Identifier& id = m_pixelID->wafer_id(hitId);
	  barrelEC  = m_pixelID -> barrel_ec(id);
	  layerDisk = m_pixelID -> layer_disk(id);
	  modEta    = m_pixelID -> eta_module(id);  //For the endcaps these are the rings
	  modPhi    = m_pixelID -> phi_module(id);
	  const InDet::PixelCluster *clus = dynamic_cast<const InDet::PixelCluster*>(hit->prepRawData());
	  if (clus){
	    const InDet::SiWidth cw = clus->width();
	    ClusSizePhi = (int)cw.colRow()[0];
	    ClusSizeZ   = (int)cw.colRow()[1];
	    ClusSize    = (int)clus->rdoList().size();
	  }



	}
	else {//sct. Since detType == 0 or detType == 1 here
	  
	  const Identifier& id = m_sctID->wafer_id(hitId);
	  barrelEC  = m_sctID->barrel_ec(id);
	  layerDisk = m_sctID->layer_disk(id);
	  modEta    = m_sctID->eta_module(id);
	  modPhi    = m_sctID->phi_module(id);
	  sctSide   = m_sctID->side(id);
	  const InDetDD::SiDetectorElement *siDet = dynamic_cast<const InDetDD::SiDetectorElement*>(hit->detectorElement());
	  isStereo = siDet->isStereo();
	  const InDet::SCT_Cluster *clus = dynamic_cast<const InDet::SCT_Cluster*>(hit->prepRawData());
	  if(clus){
	    const InDet::SiWidth cw = clus->width();
	    ClusSizePhi = (int)cw.colRow()[0];
	    ClusSize = (int)clus->rdoList().size();
	  }
	}
				
	//const Trk::TrackParameters* trackParameter = (*iter_tsos)->trackParameters();  // Already defined before in line 880. Should be safe here to take the one up since there are the continues.
				
	//finding residuals
	if(trackParameter){//should always have TrackParameters since we now skip tracks with no MeasuredTrackParameters
					
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found Trk::TrackParameters " << trackParameter << endreq;	  
					
	  double unbiasedResXY[4] = {9999.0,9999.0,9999.0,9999.0};
	  double biasedResXY[4] = {9999.0,9999.0,9999.0,9999.0};
					
	  //finding unbiased single residuals
	  StatusCode sc;
	  sc = getSiResiduals(track,*iter_tsos,true,unbiasedResXY);
	  if (sc.isFailure()) {
	    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Problem in determining unbiased residuals! Hit is skipped." << endreq;
	    m_sirescalcfailure -> Fill(detType, hweight);
	    continue;
	  }
	  else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "unbiased residuals found ok" << endreq;
	  residualX = (float)unbiasedResXY[0];
	  residualY = (float)unbiasedResXY[1];
	  pullX     = (float)unbiasedResXY[2];
	  pullY     = (float)unbiasedResXY[3];
					
	  //finding biased single residuals (for interest)
	  sc = getSiResiduals(track,*iter_tsos,false,biasedResXY);
	  if (sc.isFailure()) {
	    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Problem in determining biased residuals! Hit is skipped." << endreq;
	    continue;
	  }
	  else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "biased residuals found ok" << endreq;
	  biasedResidualX = (float)biasedResXY[0];
	  biasedResidualY = (float)biasedResXY[1];
	  biasedPullX = (float)biasedResXY[2];
	  //biasedPullY = (float)biasedResXY[3];
	  if(m_extendedPlots){
	    mlocalX = (float)mesh->localParameters()[Trk::locX]; // local X measured position 
	    mlocalY = (float)mesh->localParameters()[Trk::locY];	 // local Y measured position
	    elocalX = (float)trackParameter->localPosition()[Trk::locX]; // local X extrapolated position
	    elocalY = (float)trackParameter->localPosition()[Trk::locY]; // local Y extrapolated position
	    hitErrorX = (float)sqrt(fabs(mesh->localCovariance()(Trk::loc1,Trk::loc1)));
	    hitErrorY = (float)sqrt(fabs(mesh->localCovariance()(Trk::loc2,Trk::loc2)));
	    //globR = (float)sqrt(mesh->globalPosition().x()*mesh->globalPosition().x()+mesh->globalPosition().y()*mesh->globalPosition().y());
						
	    if (detType==1 && barrelEC!=0){  // Hit error calculation for the SCT Endcaps
	      const InDetDD::SiDetectorElement *siDet = dynamic_cast<const InDetDD::SiDetectorElement*>(hit->detectorElement());

	      // MeasurementBase --> virtual const Amg::Vector3D& globalPosition() const = 0;
	       /// Angle of strip in local frame with respect to the etaAxis.
	      /// Zero for all elements except trapezoidal detectors (ie SCT forward modules).
	      ////SiDetectorElement -->double sinStereoLocal(const Amg::Vector2D &localPos) const;
	      /// See previous method
	      ////SiDetectorElement -->double sinStereoLocal(const HepGeom::Point3D<double> &globalPos) const;
	      
	      if (siDet != NULL)
		{
		  double sinAlpha = siDet->sinStereoLocal(siDet->localPosition(mesh->globalPosition()));
		  double cosAlpha = sqrt(1 - sinAlpha*sinAlpha);
		  AmgMatrix(2,2) RotMat;
		  RotMat.setZero();
		  RotMat(0,0) = cosAlpha;
		  RotMat(0,1) = sinAlpha;
		  RotMat(1,0) = -sinAlpha;
		  RotMat(1,1) = cosAlpha;
		  
		  AmgSymMatrix(2) transformedROTCov = mesh->localCovariance().similarity(RotMat);
		  hitErrorX = sqrt(transformedROTCov(0,0));
		}
	    }
	  }
					
					
	  //looking for an overlapping module in the X,Y direction 
	  //double counting is avoided by requiring that the overlap is at greater radius
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "looking for overlaps hits..." << endreq;
	  std::pair<const Trk::TrackStateOnSurface*,const Trk::TrackStateOnSurface*> overlap = findOverlapHit(track,hit);
	  const Trk::TrackStateOnSurface* xOverlap = overlap.first;//will be NULL if no overlap found
	  const Trk::TrackStateOnSurface* yOverlap = overlap.second;//will be NULL if no overlap found
					
	  //find residuals for overlapping modules (if any) and calculate overlap residual
	  if(xOverlap){//identified outer module overlapping in localX direction
						
	    foundXOverlap = true;
	    //float overlapXResX = 9999.0;
	    //float overlapXResY = 9999.0;
	    double unbiasedOverlapRes[4] = {9999.0,9999.0,9999.0,9999.0};
	    //calculating unbiased residual for the overlapping module
	    sc = getSiResiduals(track,xOverlap,true,unbiasedOverlapRes);
	    if (sc.isFailure()) {
	      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Problem in determining unbiasedOverlapRes! Hit is skipped." << endreq;
	      //return StatusCode::SUCCESS;
	      continue;
	    } else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "unbiasedOverlapRes found ok" << endreq;
	    /*
	    overlapXResX = (float)unbiasedOverlapRes[0];
	    overlapXResY = (float)unbiasedOverlapRes[1];
						
	    //finally computing the overlap residuals
	    overlapXResidualX = overlapXResX - residualX;
	    overlapXResidualY = overlapXResY - residualY;
	    */
	    overlapXResidualX = (float)unbiasedOverlapRes[0] - residualX;
	    overlapXResidualY = (float)unbiasedOverlapRes[1] - residualY;
	    
	  }
					
	  if(yOverlap){//identified outer module overlapping in localY direction
						
	    foundYOverlap = true;
	    //float overlapYResX = 9999.0;
	    //float overlapYResY = 9999.0;
	    double unbiasedOverlapRes[4] = {9999.0,9999.0,9999.0,9999.0};
	    //calculating unbiased residual for the overlapping module
	    sc = getSiResiduals(track,yOverlap,true,unbiasedOverlapRes);
	    if (sc.isFailure()) {
	      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Problem in determining unbiasedOverlapRes! Hit is skipped." << endreq;
	      //return StatusCode::SUCCESS;
	      continue;
	    } else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "unbiasedOverlapRes found ok" << endreq;
	   
	    overlapYResidualX = (float)unbiasedOverlapRes[0] - residualX;
	    overlapYResidualY = (float)unbiasedOverlapRes[1] - residualY;
	  }
	}
	else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No TrackParameters associated with Si TrkSurface "<< nTSOS << " - Hit is probably an outlier" << endreq; 
      }
	
      //--------------------------------------------
      //
      // Filling Residual Histograms for different subsystems
      //
      //--------------------------------------------
      
   
      if (detType==0) {//filling pixel histograms
	m_si_residualx -> Fill(residualX, hweight);
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " This is a PIXEL hit " << hitId  << " - filling histograms" << endreq;
				
	if(barrelEC==0){//filling pixel barrel histograms
	  
	  if (layerDisk==0)
	    nIBLHitsPerLB++;

	  m_si_b_residualx -> Fill(residualX, hweight);
	  int m_layerModEtaShift[4] = {10,30,48,65};       //HARDCODED!
	  int m_layerModPhiShift[4] = {0,18,44,86};
	  m_si_barrel_pullX -> Fill(layerDisk,pullX   , hweight);
	  m_si_barrel_pullY -> Fill(layerDisk,pullY   , hweight);					
	  m_si_barrel_resX -> Fill(layerDisk,residualX, hweight);
	  m_si_barrel_resY -> Fill(layerDisk,residualY, hweight);					
	  m_pix_b_residualx      -> Fill(residualX      , hweight);
	  m_pix_b_biased_residualx-> Fill(biasedResidualX, hweight);
	  m_pix_b_residualy      -> Fill(residualY      , hweight);
	  m_pix_b_biased_residualy-> Fill(biasedResidualY, hweight);
	  m_pix_b_residualsx[layerDisk]-> Fill(residualX, hweight); 
	  m_pix_b_residualsy[layerDisk]-> Fill(residualY, hweight); 
	  m_pix_b_pullsx[layerDisk]    -> Fill(pullX    , hweight); 
	  m_pix_b_pullsy[layerDisk]    -> Fill(pullY    , hweight); 


	  m_pix_b_xresvsmodetaphi_3ds[layerDisk] -> Fill(modEta, modPhi, residualX, hweight);
	  m_pix_b_yresvsmodetaphi_3ds[layerDisk] -> Fill(modEta, modPhi, residualY, hweight);

	  if (layerDisk == 0) {
	    m_pix_b0_resXvsetaLumiBlock->Fill(float(lumiblock), modEta, residualX, hweight);

	    if (modEta<=6 && modEta>=-6)
	      m_pix_b0_resXvsetaLumiBlock_planars->Fill(float(lumiblock),modEta,residualX,hweight);
	    
	    if (m_doIBLLBPlots)
	      {
		m_pix_b0_resXvsetaLumiBlock_3d->Fill(float(lumiblock), modEta, residualX, hweight);
		if (modEta<=6 && modEta>=-6) m_pix_b0_resXvsetaLumiBlock_planars->Fill(float(lumiblock),modEta,residualX,hweight);
		if (modPhi==0) m_pix_b0_resXvsetaLumiBlock_stave0->Fill(float(lumiblock),modEta,residualX,hweight);
		if (modPhi==1) m_pix_b0_resXvsetaLumiBlock_stave1->Fill(float(lumiblock),modEta,residualX,hweight);
		if (modPhi==2) m_pix_b0_resXvsetaLumiBlock_stave2->Fill(float(lumiblock),modEta,residualX,hweight);
		if (modPhi==3) m_pix_b0_resXvsetaLumiBlock_stave3->Fill(float(lumiblock),modEta,residualX,hweight);
		if (modPhi==4) m_pix_b0_resXvsetaLumiBlock_stave4->Fill(float(lumiblock),modEta,residualX,hweight);
		if (modPhi==5) m_pix_b0_resXvsetaLumiBlock_stave5->Fill(float(lumiblock),modEta,residualX,hweight);
		if (modPhi==6) m_pix_b0_resXvsetaLumiBlock_stave6->Fill(float(lumiblock),modEta,residualX,hweight);
		if (modPhi==7) m_pix_b0_resXvsetaLumiBlock_stave7->Fill(float(lumiblock),modEta,residualX,hweight);
		if (modPhi==8) m_pix_b0_resXvsetaLumiBlock_stave8->Fill(float(lumiblock),modEta,residualX,hweight);
		if (modPhi==9) m_pix_b0_resXvsetaLumiBlock_stave9->Fill(float(lumiblock),modEta,residualX,hweight);
		if (modPhi==10) m_pix_b0_resXvsetaLumiBlock_stave10->Fill(float(lumiblock),modEta,residualX,hweight);
		if (modPhi==11) m_pix_b0_resXvsetaLumiBlock_stave11->Fill(float(lumiblock),modEta,residualX,hweight);
		if (modPhi==12) m_pix_b0_resXvsetaLumiBlock_stave12->Fill(float(lumiblock),modEta,residualX,hweight);
		if (modPhi==13) m_pix_b0_resXvsetaLumiBlock_stave13->Fill(float(lumiblock),modEta,residualX,hweight);
	      }
	  }

	  if (foundXOverlap) {
	    m_pix_bec_Oxresx_mean -> Fill(layerDisk+1.1,overlapXResidualX, hweight);
	    m_pix_bec_Oxresy_mean -> Fill(layerDisk+1.1,overlapXResidualY, hweight);
	    m_pix_b_Oxresxvsmodeta-> Fill(modEta+m_layerModEtaShift[layerDisk],overlapXResidualX,     hweight);
	    m_pix_b_Oxresxvsmodphi-> Fill(modPhi+m_layerModPhiShift[layerDisk],overlapXResidualX, hweight);
	    if (m_do3DOverlapHistos){
	      m_pix_b_Oxresxvsmodetaphi_3ds[layerDisk] -> Fill(modEta,modPhi,overlapXResidualX, hweight);
	      m_pix_b_Oxresyvsmodetaphi_3ds[layerDisk] -> Fill(modEta,modPhi,overlapXResidualY, hweight);
	    }
	  }//fXO
	  if (foundYOverlap){
	    m_pix_bec_Oyresx_mean -> Fill(layerDisk+1.1,overlapYResidualX, hweight);
	    m_pix_bec_Oyresy_mean -> Fill(layerDisk+1.1,overlapYResidualY, hweight);
	    m_pix_b_Oyresyvsmodeta-> Fill(modEta+m_layerModEtaShift[layerDisk],overlapYResidualY,     hweight);
	    m_pix_b_Oyresyvsmodphi-> Fill(modPhi+m_layerModPhiShift[layerDisk],overlapYResidualY,     hweight);
	    if (m_do3DOverlapHistos){
	      m_pix_b_Oyresxvsmodetaphi_3ds[layerDisk] -> Fill(modEta,modPhi,overlapYResidualX, hweight);
	      m_pix_b_Oyresyvsmodetaphi_3ds[layerDisk] -> Fill(modEta,modPhi,overlapYResidualY, hweight);
	    }
	  }//fYO
	}
	
	else if(barrelEC==2){//three Pixel endcap disks from 0-2
	  int ModPhiShift[3] = {0,55,110};
	  m_si_eca_pullX-> Fill(layerDisk,pullX    , hweight);
	  m_si_eca_pullY-> Fill(layerDisk,pullY    , hweight);
	  m_si_eca_resX -> Fill(layerDisk,residualX, hweight);
	  m_si_eca_resY -> Fill(layerDisk,residualY, hweight);
	  m_pix_eca_residualx     -> Fill(residualX, hweight);
	  m_pix_eca_residualy     -> Fill(residualY, hweight);
	  m_pix_eca_pullx         -> Fill(pullX    , hweight);
	  m_pix_eca_pully         -> Fill(pullY    , hweight);
	  
	  float disk = layerDisk + 0.1;
	  if(foundXOverlap) {
	    m_pix_bec_Oxresx_mean -> Fill(disk+5.0,overlapXResidualX, hweight);
	    m_pix_bec_Oxresy_mean -> Fill(disk+5.0,overlapXResidualY, hweight);
	    m_pix_eca_Oxresxvsmodphi -> Fill(modPhi+ModPhiShift[layerDisk],overlapXResidualX, hweight);
	  }
					
	  if(foundYOverlap) {
	    m_pix_bec_Oyresx_mean -> Fill(disk+5.0,overlapYResidualX, hweight);
	    m_pix_bec_Oyresy_mean -> Fill(disk+5.0,overlapYResidualY, hweight);
	    m_pix_eca_Oyresyvsmodphi -> Fill(modPhi+ModPhiShift[layerDisk],overlapYResidualY, hweight);
	  }

	  m_pix_eca_xresvsmodphi_2d -> Fill(modPhi+ModPhiShift[layerDisk],residualX, hweight);
	  m_pix_eca_yresvsmodphi_2d -> Fill(modPhi+ModPhiShift[layerDisk],residualY, hweight);
	  m_pix_eca_unbiased_xresvsmodphi_disks[layerDisk] -> Fill(modPhi,residualX, hweight);
	  m_pix_eca_unbiased_yresvsmodphi_disks[layerDisk] -> Fill(modPhi,residualY, hweight);
	  //m_pix_eca_unbiased_xresvsmodphi -> Fill(modPhi+ModPhiShift[layerDisk],residualX,hweight);
	  //m_pix_eca_unbiased_yresvsmodphi -> Fill(modPhi+ModPhiShift[layerDisk],residualY,hweight);
	  
	}
	else if(barrelEC==-2){
	  int ModPhiShift[3] = {0,55,110};
	  m_si_ecc_pullX-> Fill(layerDisk,pullX    , hweight);
	  m_si_ecc_pullY-> Fill(layerDisk,pullY    , hweight);
	  m_si_ecc_resX -> Fill(layerDisk,residualX, hweight);
	  m_si_ecc_resY -> Fill(layerDisk,residualY, hweight);
					
	  float disk = (float)layerDisk;
	  disk =  -1.0*(disk + 0.1);
	  m_pix_ecc_residualx     -> Fill(residualX, hweight);
	  m_pix_ecc_residualy     -> Fill(residualY, hweight);
	  m_pix_ecc_pullx         -> Fill(pullX    , hweight);
	  m_pix_ecc_pully         -> Fill(pullY    , hweight);					
	  if(foundXOverlap) {
	    m_pix_bec_Oxresx_mean -> Fill(disk,overlapXResidualX, hweight);
	    m_pix_bec_Oxresy_mean -> Fill(disk,overlapXResidualY, hweight);
	    m_pix_ecc_Oxresxvsmodphi -> Fill(modPhi+ModPhiShift[layerDisk],overlapXResidualX, hweight);
	  }					
	  if(foundYOverlap) {
	    m_pix_bec_Oyresx_mean -> Fill(disk,overlapYResidualX, hweight);
	    m_pix_bec_Oyresy_mean -> Fill(disk,overlapYResidualY, hweight);
	    m_pix_ecc_Oyresyvsmodphi -> Fill(modPhi+ModPhiShift[layerDisk],overlapYResidualY, hweight);
	  }
	  m_pix_ecc_xresvsmodphi_2d -> Fill(modPhi+ModPhiShift[layerDisk],residualX, hweight);
	  m_pix_ecc_yresvsmodphi_2d -> Fill(modPhi+ModPhiShift[layerDisk],residualY, hweight);
	  //m_pix_ecc_unbiased_xresvsmodphi -> Fill(modPhi+ModPhiShift[layerDisk],residualX,hweight);
	  //m_pix_ecc_unbiased_yresvsmodphi -> Fill(modPhi+ModPhiShift[layerDisk],residualY,hweight);	
	  m_pix_ecc_unbiased_xresvsmodphi_disks[layerDisk] -> Fill(modPhi,residualX, hweight);
	  m_pix_ecc_unbiased_yresvsmodphi_disks[layerDisk] -> Fill(modPhi,residualY, hweight);	  
	  
	}
	  
      }
      else if (detType==1) {//filling SCT histograms
	m_si_residualx -> Fill(residualX, hweight);			
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " This is an SCT hit " << hitId << " - filling histograms" << endreq;
				
	if(barrelEC==0){//filling SCT barrel histograms
	  m_si_b_residualx -> Fill(residualX, hweight);
	  int ModPhiShift[4] = {0,37,82,135};
	  int ModEtaShift[4] = {6,24,42,60};				
	  m_si_barrel_pullX->Fill(m_NPixLayers + 2*(layerDisk) + sctSide, pullX, hweight);
	  //m_si_barrel_pullY->Fill(3 + 2*(layerDisk) + sctSide, pullY);//no SCT Y residuals yet
	 
	  m_si_barrel_resX->Fill(m_NPixLayers + 2*(layerDisk) + sctSide, residualX, hweight);
	  //m_si_barrel_resY->Fill(3 + 2*(layerDisk) + sctSide, residualY);//no SCT Y residuals yet
					
	  m_sct_b_residualx      -> Fill(residualX      , hweight);
	  m_sct_b_biasedresidualx-> Fill(biasedResidualX, hweight);
	  
	  if(foundXOverlap) m_sct_bec_Oxresx_mean -> Fill(layerDisk+1.1,overlapXResidualX, hweight);
	  if(foundYOverlap) m_sct_bec_Oyresx_mean -> Fill(layerDisk+1.1,overlapYResidualX, hweight);
	  
	  m_sct_b_residualsx[layerDisk]-> Fill(residualX, hweight); 
	  m_sct_b_pullsx[layerDisk]    -> Fill(pullX    , hweight); 
	  
	  m_sct_b_xresvsmodetaphi_3ds[layerDisk] -> Fill(modEta,modPhi,residualX, hweight);
	  
	  	  
	  if(foundXOverlap) {
	    m_sct_b_xoverlapresidualsx[layerDisk]-> Fill(overlapXResidualX         , hweight); 
	    m_sct_b_Oxresxvsmodeta    -> Fill(modEta+ModEtaShift[layerDisk],overlapXResidualX, hweight);
	    m_sct_b_Oxresxvsmodphi    -> Fill(modPhi+ModPhiShift[layerDisk],overlapXResidualX  , hweight);
	    if (m_do3DOverlapHistos)
	      m_sct_b_Oxresxvsmodetaphi_3ds[layerDisk] -> Fill(modEta,modPhi,overlapXResidualX, hweight);
	  }								  
	  if(foundYOverlap) {
	    m_sct_b_yoverlapresidualsx[layerDisk]-> Fill(overlapYResidualX         , hweight); 
	    m_sct_b_Oyresxvsmodeta    -> Fill(modEta+ModEtaShift[layerDisk],overlapYResidualX, hweight);
	    m_sct_b_Oyresxvsmodphi    -> Fill(modPhi+ModPhiShift[layerDisk],overlapYResidualX  , hweight);
	    if(m_do3DOverlapHistos)
	      m_sct_b_Oyresxvsmodetaphi_3ds[layerDisk] -> Fill(modEta,modPhi,overlapYResidualX, hweight);
	  }
	  // if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unexpected SCT layer number "<< layerDisk << endreq; 
	}

	else if(barrelEC==2){//nine SCT endcap disks from 0-8
	  //ASSUMPTION: the outer rings of the SCT endcap disks have the same number of modules. WARNING! hardcoded! 
	  int Nmods = 52;
	  //int ModPhiShift[9] = {0,55,110,165,220,275,330,385,440};
	  m_si_eca_pullX->Fill(3 + 2*(layerDisk) + sctSide, pullX, hweight);
	  //m_si_eca_pullY->Fill(3 + 2*(layerDisk) + sctSide, pullY);//no SCT Y residuals yet
	  m_si_eca_resX->Fill(3 + 2*(layerDisk) + sctSide, residualX, hweight);
	  //m_si_eca_resY->Fill(3 + 2*(layerDisk) + sctSide, residualY);//no SCT Y residuals yet
	  
	  float disk = layerDisk + 0.1;
	  m_sct_eca_residualx     -> Fill(residualX, hweight);
	  m_sct_eca_pullx         -> Fill(pullX    , hweight);
	  
	  m_sct_eca_xresvsmodphi_2d -> Fill(modPhi+(layerDisk - 1)* (m_gap_sct + Nmods),residualX, hweight);
	  					
	  if(foundXOverlap) {
	    m_sct_bec_Oxresx_mean->Fill(disk+6.0,overlapXResidualX);						
	    m_sct_eca_Oxresxvsmodphi -> Fill(modPhi+ (layerDisk - 1)* (m_gap_sct + Nmods),overlapXResidualX,hweight);
	    //                               modPhi+ ModPhiShift[layerDisk],overlapXResidualX    , hweight);
	  }
					
	}

	
	else if(barrelEC==-2){
	  int Nmods = 52;
	  m_si_ecc_pullX->Fill(3 + 2*(layerDisk) + sctSide, pullX, hweight);
	  //m_si_ecc_pullY->Fill(3 + 2*(layerDisk) + sctSide, pullY);//no SCT Y residuals yet
	  m_si_ecc_resX->Fill(3 + 2*(layerDisk) + sctSide, residualX, hweight);
	  //m_si_ecc_resY->Fill(3 + 2*(layerDisk) + sctSide, residualY);//no SCT Y residuals yet
					
	  float disk = (float)layerDisk;
	  disk =  -1.0*(disk + 0.1);
	  m_sct_ecc_residualx     ->Fill(residualX, hweight);
	  m_sct_ecc_pullx         ->Fill(pullX    , hweight);
	  
	  m_sct_ecc_xresvsmodphi_2d -> Fill(modPhi+(layerDisk - 1)* (m_gap_sct + Nmods)    ,residualX, hweight);	  					
	  if(foundXOverlap) {
	    m_sct_bec_Oxresx_mean->Fill(disk,overlapXResidualX);
	    m_sct_ecc_Oxresxvsmodphi -> Fill(modPhi+(layerDisk - 1)* (m_gap_sct + Nmods)    ,overlapXResidualX, hweight);
	  }
	}
      }
   
      // Extended pixel histograms
      if(m_extendedPlots){  
	if (detType==0){   //Pixel histograms
	  if(barrelEC==0){ // barrel
	    static int m_phiIdentifier_min[3]    = {0,-1 ,22};
	    static int m_phiIdentifier_max[3]    = {11,20,49};// Barrel
	    m_pix_b_biased_residualx_pt-> Fill(trkpt, biasedResidualX, hweight);
	    m_pix_b_biased_residualy_pt-> Fill(trkpt, biasedResidualY, hweight);
	    m_pix_b_residualx_pt       -> Fill(trkpt, residualX, hweight);
	    m_pix_b_residualy_pt       -> Fill(trkpt, residualY, hweight);
	    m_pix_b_pullx_pt           -> Fill(trkpt, pullX, hweight);
	    m_pix_b_pully_pt           -> Fill(trkpt, pullY, hweight);
	    m_pix_b_residualx_fine -> Fill(residualX      , hweight);
	    m_pix_b_residualy_fine -> Fill(residualY      , hweight);
	    m_pix_b_measured_localx-> Fill(mlocalX     , hweight);
	    m_pix_b_measured_localy-> Fill(mlocalY     , hweight);
	    m_pix_b_extrapolated_localx-> Fill(elocalX     , hweight);
	    m_pix_b_extrapolated_localy-> Fill(elocalY     , hweight);
	    if (layerDisk != 0) { // Pixel barrel layers
	      m_hiterror_x_pix_b         -> Fill(hitErrorX   , hweight); 	    
	      m_hiterror_x_pix_b_WideRange-> Fill(hitErrorX   , hweight);
	      m_hiterror_y_pix_b         -> Fill(hitErrorY   , hweight);	    
	      m_hiterror_y_pix_b_WideRange-> Fill(hitErrorY   , hweight);
	    }
	    if (layerDisk == 0) { // IBL
	      m_hiterror_x_ibl_b      ->Fill(hitErrorX,hweight);
	      m_hiterror_y_ibl_b      ->Fill(hitErrorX,hweight);
	      m_hiterror_x_ibl_b_WideRange      ->Fill(hitErrorX,hweight);
	      m_hiterror_y_ibl_b_WideRange      ->Fill(hitErrorX,hweight);
	    }
	    
	    if(mlocalY >  PixelBarrelYSize / 2.05 )	mlocalY =  PixelBarrelYSize/2.05;
	    if(mlocalY < -PixelBarrelYSize / 2.05 )	mlocalY = -PixelBarrelYSize/2.05;
	    if(mlocalX >  PixelBarrelXSize / 2.05 )	mlocalX =  PixelBarrelXSize/2.05;
	    if(mlocalX < -PixelBarrelXSize / 2.05 )	mlocalX = -PixelBarrelXSize/2.05;

	    m_pix_b_residualsx_incitheta[layerDisk] ->Fill(incidenceTheta,residualX,hweight);
	    m_pix_b_residualsy_incitheta[layerDisk] ->Fill(incidenceTheta,residualY,hweight);
	    m_pix_b_residualsx_inciphi[layerDisk] ->Fill(incidencePhi,residualX,hweight);
	    m_pix_b_residualsy_inciphi[layerDisk] ->Fill(incidencePhi,residualY,hweight);
	    m_pix_b_residualsx_pt[layerDisk]       -> Fill(trkpt, residualX      , hweight);
	    m_pix_b_residualsy_pt[layerDisk]       -> Fill(trkpt, residualY      , hweight);
	    m_pix_b_residualsx_qoverp2[layerDisk]-> Fill(trkqoverp2, residualX      , hweight);
	    m_pix_b_residualsy_qoverp2[layerDisk]-> Fill(trkqoverp2, residualY      , hweight);
	    m_pix_b_biased_residualsx[layerDisk]   -> Fill(biasedResidualX       , hweight);
	    m_pix_b_biased_residualsy[layerDisk]   -> Fill(biasedResidualY       , hweight);
	    m_pix_b_biased_residualsx_pt[layerDisk]-> Fill(trkpt, biasedResidualX, hweight);
	    m_pix_b_biased_residualsy_pt[layerDisk]-> Fill(trkpt, biasedResidualY, hweight);
	    m_pix_b_biased_residualsx_qoverp2[layerDisk]  -> Fill(trkqoverp2, biasedResidualX, hweight);
	    m_pix_b_biased_residualsy_qoverp2[layerDisk]  -> Fill(trkqoverp2, biasedResidualY, hweight);
	    m_pix_b_pullsx_pt[layerDisk]          -> Fill(trkpt, pullX          , hweight);
	    m_pix_b_pullsy_pt[layerDisk]           -> Fill(trkpt, pullY          , hweight);
	    if(foundXOverlap) {
	      m_pix_b_xoverlapresidualsx[layerDisk]-> Fill(overlapXResidualX          , hweight); 
	      m_pix_b_xoverlapresidualsy[layerDisk]-> Fill(overlapXResidualY          , hweight); 
	    }
	    
	    if(foundYOverlap) {
	      m_pix_b_yoverlapresidualsx[layerDisk]-> Fill(overlapYResidualX          , hweight); 
	      m_pix_b_yoverlapresidualsy[layerDisk]-> Fill(overlapYResidualY          , hweight); 
	    }
	    // int modoffset = 6.5;       // default for old pixel layers
	    PixelBarrelXSize = 16.44;  // mm
	    PixelBarrelYSize = 60.2;   // mm 

	    if (layerDisk == 0) { // in case of IBL
	      PixelBarrelXSize = 18.75; // Extracted from: arXiv:1209.1906v1, Figure 2
	      PixelBarrelYSize = 41.32;
	      // case of 3D sensors have slighlty different size
	      if ( modEta < -6 || modEta > 5) {
		PixelBarrelXSize = 20.45; // Extracted from: arXiv:1209.1906v1, Figure 2
		PixelBarrelYSize = 19.20 ;
	      }
	    }

	    float xValueForHist = modEta +  mlocalY / PixelBarrelYSize; 
	    float yValueForHist = modPhi +  mlocalX / PixelBarrelXSize;

	    if (false) {
	      std::cout << " -- SALVA -- filling detailed pixel maps -- layer = " << layerDisk 
			<< "  eta: " << modEta 
			<< "  phi " << modPhi
			<< "  local (" << mlocalX << " / " << PixelBarrelXSize 
			<< ", "        << mlocalY << " / " << PixelBarrelYSize << ") "
			<< "  normalized (" << mlocalY/PixelBarrelYSize * m_mapSplit << ", " << mlocalX/PixelBarrelXSize * m_mapSplit << ") "
			<< std::endl;
	      std::cout << "                                                      " 
			<< " fill (" << xValueForHist << ", " << yValueForHist << ") "
			<< std::endl;
	    }
	    // biased 3d histos turned into detailed 3d histos (for the time being use unbiased residuals)
	    //m_pix_b_biased_xresvsmodetaphi_3ds[layerDisk] -> Fill(ModCenterPosX+mlocalY, ModCenterPosY+mlocalX, residualX, hweight);
	    //m_pix_b_biased_yresvsmodetaphi_3ds[layerDisk] -> Fill(ModCenterPosX+mlocalY, ModCenterPosY+mlocalX, residualY, hweight);
	    m_pix_b_detailed_xresvsmodetaphi_3ds[layerDisk] -> Fill( xValueForHist, yValueForHist, residualX, hweight);
	    m_pix_b_detailed_yresvsmodetaphi_3ds[layerDisk] -> Fill( xValueForHist, yValueForHist, residualY, hweight);
	    
	    // pixel B-layer halfshell phi identifier association
		// Layer 0                  Layer 1                     Layer 2
		// top: 1-10                0-19                        0-22 49-51
		// bottom: 11-21 and 0      20-37                       23-48
	    if (layerDisk != 2) // Necessary because the logic changes
	      if(modPhi>m_phiIdentifier_min[layerDisk] && modPhi<m_phiIdentifier_max[layerDisk]) { 
		m_pix_b_top_residualsx[layerDisk] -> Fill(residualX, hweight); 
		m_pix_b_top_residualsy[layerDisk] -> Fill(residualY, hweight);
		m_pix_b_top_biased_residualsx[layerDisk] -> Fill(biasedResidualX, hweight);
		m_pix_b_top_biased_residualsy[layerDisk] -> Fill(biasedResidualY, hweight); 
	      }
	      else { 
		m_pix_b_btm_residualsx[layerDisk] -> Fill(residualX, hweight); 
		m_pix_b_btm_residualsy[layerDisk] -> Fill(residualY, hweight);
		m_pix_b_btm_biased_residualsx[layerDisk] -> Fill(biasedResidualX, hweight); 
		m_pix_b_btm_biased_residualsy[layerDisk] -> Fill(biasedResidualY, hweight); 
	      }
		else 
		  if(modPhi>m_phiIdentifier_min[layerDisk] && modPhi<m_phiIdentifier_max[layerDisk]) { 
		    m_pix_b_btm_residualsx[layerDisk] -> Fill(residualX, hweight); 
		    m_pix_b_btm_residualsy[layerDisk] -> Fill(residualY, hweight);
		    m_pix_b_btm_biased_residualsx[layerDisk] -> Fill(biasedResidualX, hweight);
		    m_pix_b_btm_biased_residualsy[layerDisk] -> Fill(biasedResidualY, hweight); 
		  }
		  else { 
		    m_pix_b_top_residualsx[layerDisk] -> Fill(residualX, hweight); 
		    m_pix_b_top_residualsy[layerDisk] -> Fill(residualY, hweight);
		    m_pix_b_top_biased_residualsx[layerDisk] -> Fill(biasedResidualX, hweight); 
		    m_pix_b_top_biased_residualsy[layerDisk] -> Fill(biasedResidualY, hweight); 
		  }
	    if (m_doClusterSizeHistos)
	      {
		m_pix_b_clustersize[layerDisk] -> Fill(ClusSize,hweight);
		m_pix_b_clustersizePhi[layerDisk] -> Fill(ClusSizePhi,hweight);
		m_pix_b_clustersizeZ[layerDisk] -> Fill(ClusSizeZ,hweight);
		m_pix_b_residualsx_clustersize[layerDisk]->Fill(ClusSize,residualX,hweight);
		m_pix_b_residualsy_clustersize[layerDisk]->Fill(ClusSize,residualY,hweight);
		m_pix_b_residualsx_clustersizePhi[layerDisk]->Fill(ClusSizePhi,residualX,hweight);
		m_pix_b_residualsy_clustersizePhi[layerDisk]->Fill(ClusSizePhi,residualY,hweight);
		m_pix_b_residualsx_clustersizeZ[layerDisk]->Fill(ClusSizeZ,residualX,hweight);
		m_pix_b_residualsy_clustersizeZ[layerDisk]->Fill(ClusSizeZ,residualY,hweight);
		m_pix_b_residualsx_clustersizeP[layerDisk]->Fill(ClusSize,residualX,hweight);
		m_pix_b_residualsy_clustersizeP[layerDisk]->Fill(ClusSize,residualY,hweight);
		m_pix_b_residualsx_clustersizePhiP[layerDisk]->Fill(ClusSizePhi,residualX,hweight);
		m_pix_b_residualsy_clustersizePhiP[layerDisk]->Fill(ClusSizePhi,residualY,hweight);
		m_pix_b_residualsx_clustersizeZP[layerDisk]->Fill(ClusSizeZ,residualX,hweight);
		m_pix_b_residualsy_clustersizeZP[layerDisk]->Fill(ClusSizeZ,residualY,hweight);
		m_pix_b_clustersizePhi_incidentAngle[layerDisk]->Fill(incidenceTheta,ClusSizePhi,hweight);
		m_pix_b_clustersizeZ_incidentAngle[layerDisk]->Fill(incidenceTheta,ClusSizeZ,hweight);
		m_pix_b_clustersize_incidentAngle[layerDisk]->Fill(incidenceTheta,ClusSize,hweight);
		m_pix_b_clustersizePhi_incidentAnglePhi[layerDisk]->Fill(incidencePhi,ClusSizePhi,hweight);
		m_pix_b_clustersizeZ_incidentAnglePhi[layerDisk]->Fill(incidencePhi,ClusSizeZ,hweight);
		m_pix_b_clustersize_incidentAnglePhi[layerDisk]->Fill(incidencePhi,ClusSize,hweight);
	      }
	  }
	  else if (barrelEC==2){ // Endcap A
	    m_pix_eca_xresvsmodphidisk_3d->Fill(layerDisk,modPhi,residualX,hweight);
	    m_pix_eca_yresvsmodphidisk_3d->Fill(layerDisk,modPhi,residualY,hweight);
	    m_pix_eca_biased_residualx->Fill(biasedResidualX,hweight);
	    m_pix_eca_biased_residualy->Fill(biasedResidualY,hweight);
	    m_pix_eca_residualx_fine-> Fill(residualX, hweight);
	    m_pix_eca_residualy_fine-> Fill(residualY, hweight);
	    m_pix_eca_biased_residualx_pt-> Fill(trkpt,biasedResidualX, hweight);
	    m_pix_eca_biased_residualy_pt-> Fill(trkpt,biasedResidualY, hweight);
	    m_pix_eca_residualx_pt-> Fill(trkpt, residualX, hweight);
	    m_pix_eca_residualy_pt-> Fill(trkpt, residualY, hweight);
	    m_pix_eca_pullx_pt    -> Fill(trkpt, pullX    , hweight);
	    m_pix_eca_pully_pt    -> Fill(trkpt, pullY    , hweight);

	    m_hiterror_x_pix_ec   -> Fill(hitErrorX       , hweight); 	    m_hiterror_x_pix_ec_WideRange-> Fill(hitErrorX       , hweight);
	    m_hiterror_y_pix_ec   -> Fill(hitErrorY       , hweight);       m_hiterror_y_pix_ec_WideRange-> Fill(hitErrorY       , hweight);    

	    m_pix_eca_biased_residualsx[layerDisk]   -> Fill(biasedResidualX      , hweight);
	    m_pix_eca_biased_residualsy[layerDisk]   -> Fill(biasedResidualY      , hweight);
	    m_pix_eca_biased_residualsx_pt[layerDisk]-> Fill(trkpt,biasedResidualX, hweight);
	    m_pix_eca_biased_residualsy_pt[layerDisk]-> Fill(trkpt,biasedResidualY, hweight);
	    m_pix_eca_residualsx_pt[layerDisk]-> Fill(trkpt,residualX, hweight);
	    m_pix_eca_residualsy_pt[layerDisk]-> Fill(trkpt,residualY, hweight);
	    m_pix_eca_pullsx_pt[layerDisk]    -> Fill(trkpt,pullX    , hweight);
	    m_pix_eca_pullsy_pt[layerDisk]     -> Fill(trkpt,pullY    , hweight);
	    m_pix_eca_biased_residualsx_qoverp2[layerDisk]   -> Fill(trkqoverp2,biasedResidualX, hweight);
	    m_pix_eca_biased_residualsy_qoverp2[layerDisk]   -> Fill(trkqoverp2,biasedResidualY, hweight);
	    m_pix_eca_residualsx_qoverp2[layerDisk] -> Fill(trkqoverp2,residualX      , hweight);
	    m_pix_eca_residualsy_qoverp2[layerDisk] -> Fill(trkqoverp2,residualY      , hweight);
	    m_pix_eca_biased_xresvsmodphi_2ds[layerDisk]  -> Fill(modPhi,biasedResidualX, hweight);
	    m_pix_eca_biased_yresvsmodphi_2ds[layerDisk]  -> Fill(modPhi,biasedResidualY, hweight);
	    
	    if (m_doClusterSizeHistos)
	      {
		m_pix_eca_clustersize[layerDisk] -> Fill(ClusSize,hweight);
		m_pix_eca_clustersizePhi[layerDisk] -> Fill(ClusSizePhi,hweight);
		m_pix_eca_clustersizeZ[layerDisk] -> Fill(ClusSizeZ,hweight);
		m_pix_eca_residualsx_clustersize[layerDisk]->Fill(ClusSize,residualX,hweight);
		m_pix_eca_residualsy_clustersize[layerDisk]->Fill(ClusSize,residualY,hweight);
		m_pix_eca_residualsx_clustersizePhi[layerDisk]->Fill(ClusSizePhi,residualX,hweight);
		m_pix_eca_residualsy_clustersizePhi[layerDisk]->Fill(ClusSizePhi,residualY,hweight);
		m_pix_eca_residualsx_clustersizeZ[layerDisk]->Fill(ClusSizeZ,residualX,hweight);
		m_pix_eca_residualsy_clustersizeZ[layerDisk]->Fill(ClusSizeZ,residualY,hweight);
		m_pix_eca_residualsx_clustersizeP[layerDisk]->Fill(ClusSize,residualX,hweight);
		m_pix_eca_residualsy_clustersizeP[layerDisk]->Fill(ClusSize,residualY,hweight);
		m_pix_eca_residualsx_clustersizePhiP[layerDisk]->Fill(ClusSizePhi,residualX,hweight);
		m_pix_eca_residualsy_clustersizePhiP[layerDisk]->Fill(ClusSizePhi,residualY,hweight);
		m_pix_eca_residualsx_clustersizeZP[layerDisk]->Fill(ClusSizeZ,residualX,hweight);
		m_pix_eca_residualsy_clustersizeZP[layerDisk]->Fill(ClusSizeZ,residualY,hweight);
		m_pix_eca_clustersizePhi_incidentAngle[layerDisk]->Fill(incidenceTheta,ClusSizePhi,hweight);
		m_pix_eca_clustersizeZ_incidentAngle[layerDisk]->Fill(incidenceTheta,ClusSizeZ,hweight);
		m_pix_eca_clustersize_incidentAngle[layerDisk]->Fill(incidenceTheta,ClusSize,hweight);
		m_pix_eca_clustersizePhi_incidentAnglePhi[layerDisk]->Fill(incidencePhi,ClusSizePhi,hweight);
		m_pix_eca_clustersizeZ_incidentAnglePhi[layerDisk]->Fill(incidencePhi,ClusSizeZ,hweight);
		m_pix_eca_clustersize_incidentAnglePhi[layerDisk]->Fill(incidencePhi,ClusSize,hweight);
		
	      }
	    
	    }
	  else if (barrelEC==-2){
	    m_pix_ecc_xresvsmodphidisk_3d->Fill(layerDisk,modPhi,residualX,hweight);
	    m_pix_ecc_yresvsmodphidisk_3d->Fill(layerDisk,modPhi,residualY,hweight);
	    m_pix_ecc_biased_residualx->Fill(biasedResidualX,hweight);
	    m_pix_ecc_biased_residualy->Fill(biasedResidualY,hweight);
	    m_pix_ecc_residualx_fine-> Fill(residualX, hweight);
	    m_pix_ecc_residualy_fine-> Fill(residualY, hweight);
	    m_pix_ecc_biased_residualx_pt-> Fill(trkpt,biasedResidualX, hweight);
	    m_pix_ecc_biased_residualy_pt-> Fill(trkpt,biasedResidualY, hweight);
	    m_pix_ecc_residualx_pt-> Fill(trkpt, residualX, hweight);
	    m_pix_ecc_residualy_pt-> Fill(trkpt, residualY, hweight);
	    m_pix_ecc_pullx_pt    -> Fill(trkpt, pullX    , hweight);
	    m_pix_ecc_pully_pt    -> Fill(trkpt, pullY    , hweight);
	    m_hiterror_x_pix_ec   -> Fill(hitErrorX       , hweight);  m_hiterror_x_pix_ec_WideRange-> Fill(hitErrorX       , hweight); 
	    m_hiterror_y_pix_ec   -> Fill(hitErrorY       , hweight);  m_hiterror_y_pix_ec_WideRange-> Fill(hitErrorY       , hweight);
	   
	    m_pix_ecc_biased_residualsx[layerDisk]   -> Fill(biasedResidualX      , hweight);
	    m_pix_ecc_biased_residualsy[layerDisk]   -> Fill(biasedResidualY      , hweight);
	    m_pix_ecc_biased_residualsx_pt[layerDisk]-> Fill(trkpt,biasedResidualX, hweight);
	    m_pix_ecc_biased_residualsy_pt[layerDisk]-> Fill(trkpt,biasedResidualY, hweight);
	    m_pix_ecc_residualsx_pt[layerDisk]-> Fill(trkpt,residualX, hweight);
	    m_pix_ecc_residualsy_pt[layerDisk]-> Fill(trkpt,residualY, hweight);
	    m_pix_ecc_pullsx_pt[layerDisk]    -> Fill(trkpt,pullX    , hweight);
	    m_pix_ecc_pullsy_pt[layerDisk]     -> Fill(trkpt,pullY    , hweight);
	    m_pix_ecc_biased_residualsx_qoverp2[layerDisk]   -> Fill(trkqoverp2,biasedResidualX, hweight);
	    m_pix_ecc_biased_residualsy_qoverp2[layerDisk]   -> Fill(trkqoverp2,biasedResidualY, hweight);
	    m_pix_ecc_residualsx_qoverp2[layerDisk] -> Fill(trkqoverp2,residualX      , hweight);
	    m_pix_ecc_residualsy_qoverp2[layerDisk] -> Fill(trkqoverp2,residualY      , hweight);
	    m_pix_ecc_biased_xresvsmodphi_2ds[layerDisk]  -> Fill(modPhi,biasedResidualX, hweight);
	    m_pix_ecc_biased_yresvsmodphi_2ds[layerDisk]  -> Fill(modPhi,biasedResidualY, hweight);
	    
	    if (m_doClusterSizeHistos)
	      {
		m_pix_ecc_clustersize[layerDisk] -> Fill(ClusSize,hweight);
		m_pix_ecc_clustersizePhi[layerDisk] -> Fill(ClusSizePhi,hweight);
		m_pix_ecc_clustersizeZ[layerDisk] -> Fill(ClusSizeZ,hweight);
		m_pix_ecc_residualsx_clustersize[layerDisk]->Fill(ClusSize,residualX,hweight);
		m_pix_ecc_residualsy_clustersize[layerDisk]->Fill(ClusSize,residualY,hweight);
		m_pix_ecc_residualsx_clustersizePhi[layerDisk]->Fill(ClusSizePhi,residualX,hweight);
		m_pix_ecc_residualsy_clustersizePhi[layerDisk]->Fill(ClusSizePhi,residualY,hweight);
		m_pix_ecc_residualsx_clustersizeZ[layerDisk]->Fill(ClusSizeZ,residualX,hweight);
		m_pix_ecc_residualsy_clustersizeZ[layerDisk]->Fill(ClusSizeZ,residualY,hweight);
		m_pix_ecc_residualsx_clustersizeP[layerDisk]->Fill(ClusSize,residualX,hweight);
		m_pix_ecc_residualsy_clustersizeP[layerDisk]->Fill(ClusSize,residualY,hweight);
		m_pix_ecc_residualsx_clustersizePhiP[layerDisk]->Fill(ClusSizePhi,residualX,hweight);
		m_pix_ecc_residualsy_clustersizePhiP[layerDisk]->Fill(ClusSizePhi,residualY,hweight);
		m_pix_ecc_residualsx_clustersizeZP[layerDisk]->Fill(ClusSizeZ,residualX,hweight);
		m_pix_ecc_residualsy_clustersizeZP[layerDisk]->Fill(ClusSizeZ,residualY,hweight);
		m_pix_ecc_clustersizePhi_incidentAngle[layerDisk]->Fill(incidenceTheta,ClusSizePhi,hweight);
		m_pix_ecc_clustersizeZ_incidentAngle[layerDisk]->Fill(incidenceTheta,ClusSizeZ,hweight);
		m_pix_ecc_clustersize_incidentAngle[layerDisk]->Fill(incidenceTheta,ClusSize,hweight);
		m_pix_ecc_clustersizePhi_incidentAnglePhi[layerDisk]->Fill(incidencePhi,ClusSizePhi,hweight);
		m_pix_ecc_clustersizeZ_incidentAnglePhi[layerDisk]->Fill(incidencePhi,ClusSizeZ,hweight);
		m_pix_ecc_clustersize_incidentAnglePhi[layerDisk]->Fill(incidencePhi,ClusSize,hweight);
	      }
	  }
	}
	else if(detType==1) {
	  if(barrelEC==0){ // barrel part
	    m_sct_b_residualx_fine -> Fill(residualX      , hweight);
	    m_sct_b_biased_residualx_pt     -> Fill(trkpt, biasedResidualX     , hweight);
	    m_sct_b_residualx_pt            -> Fill(trkpt, residualX           , hweight);
	    m_sct_b_pullx_pt                -> Fill(trkpt, pullX               , hweight);
	    m_sct_b_biased_residualx_qoverp2-> Fill(trkqoverp2, biasedResidualX, hweight);
	    m_sct_b_unbiased_residualx_qoverp2-> Fill(trkqoverp2, residualX, hweight);
	    //m_sct_b_residualx_qoverp2 -> Fill(trkqoverp2, residualX   , hweight); !! Not in the original
	   
	    m_hiterror_sct_b                   -> Fill(hitErrorX               , hweight);
	    m_hiterror_sct_b_WideRange         -> Fill(hitErrorX               , hweight);

	    if(isStereo){						       
	      m_sct_b_extrapolated_st_localx-> Fill(elocalX, hweight);
	      m_sct_b_extrapolated_st_localy-> Fill(elocalY, hweight);
	      m_sct_b_measured_st_localx    -> Fill(mlocalX, hweight);
	      m_sct_b_measured_st_localy    -> Fill(mlocalY, hweight);
	    }
	    else{
	      m_sct_b_extrapolated_nst_localx-> Fill(elocalX, hweight);
	      m_sct_b_extrapolated_nst_localy-> Fill(elocalY, hweight);
	      m_sct_b_measured_nst_localx    -> Fill(mlocalX, hweight);
	      m_sct_b_measured_nst_localy    -> Fill(mlocalY, hweight);
	    }
	    
	    if(fabs(elocalY)>SCTBarrelYSize/2.){
	      if (elocalY<0) elocalY = -SCTBarrelYSize/2.05; 
	      if (elocalY>0) elocalY =  SCTBarrelYSize/2.05;
	    }
	    if(fabs(mlocalX)>SCTBarrelXSize/2.){
	      if (mlocalX<0) mlocalX = -SCTBarrelXSize/2.05; 
	      if (mlocalX>0) mlocalX =  SCTBarrelXSize/2.05;
	    }
	    if(elocalY >  SCTBarrelYSize/2.05)	      elocalY =  SCTBarrelYSize/2.05;
	    if(elocalY < -SCTBarrelYSize/2.05)	      elocalY = -SCTBarrelYSize/2.05;
	    if(mlocalX >  SCTBarrelXSize/2.05)	      mlocalX =  SCTBarrelXSize/2.05;
	    if(mlocalX < -SCTBarrelXSize/2.05)	      mlocalX = -SCTBarrelXSize/2.05;
	   				
	    m_sct_b_biased_residualsx[layerDisk]   -> Fill(biasedResidualX       , hweight);
	    m_sct_b_biased_residualsx_pt[layerDisk]-> Fill(trkpt, biasedResidualX, hweight);
	    m_sct_b_residualsx_pt[layerDisk]       -> Fill(trkpt, residualX      , hweight);
	    m_sct_b_pullsx_pt[layerDisk]           -> Fill(trkpt, pullX          , hweight);
	    m_sct_b_biased_residualsx_qoverp2[layerDisk]-> Fill(trkqoverp2, biasedResidualX, hweight);
	    m_sct_b_residualsx_qoverp2[layerDisk]-> Fill(trkqoverp2, residualX      , hweight);
	    m_sct_b_biased_pullsx[layerDisk]       -> Fill(biasedPullX                , hweight);
	    if (m_doClusterSizeHistos)
	      {
		m_sct_b_clustersizePhi[layerDisk] ->Fill(ClusSizePhi,hweight);
		m_sct_b_residualsx_clustersizePhi[layerDisk] ->Fill(ClusSizePhi,residualX,hweight);
		m_sct_b_residualsx_clustersizePhiP[layerDisk] ->Fill(ClusSizePhi,residualX,hweight);
		if (sctSide==0)  {
		  m_sct_b_residualsx_incidentAngle_s0 [layerDisk] ->Fill(incidenceTheta,residualX,hweight);
		  m_sct_b_residualsx_incidentAnglePhi_s0[layerDisk] ->Fill(incidencePhi,residualX,hweight);
		  m_sct_b_clustersizePhivsmodetaphi_3ds_s0[layerDisk] -> Fill(modEta,modPhi,ClusSizePhi, hweight);
		}
		if (sctSide==1) {
		  m_sct_b_residualsx_incidentAngle_s1 [layerDisk] ->Fill(incidenceTheta,residualX,hweight);
		  m_sct_b_residualsx_incidentAnglePhi_s1[layerDisk] ->Fill(incidencePhi,residualX,hweight);
		  m_sct_b_clustersizePhivsmodetaphi_3ds_s1[layerDisk] -> Fill(modEta,modPhi,ClusSizePhi, hweight);
		}
		m_sct_b_clustersizePhi_incidentAngle[layerDisk] ->Fill(incidenceTheta,ClusSizePhi,hweight);
		m_sct_b_clustersizePhi_incidentAnglePhi[layerDisk] ->Fill(incidencePhi,ClusSizePhi,hweight);
	      }
	    Float_t corrEta;							    
	    if(modEta>0)  corrEta = modEta+5.5;
	    else          corrEta = modEta+6.5;
	    float ModCenterPosX = m_sct_b_s0_biased_xresvsmodetaphi_3ds[layerDisk]->GetXaxis()->GetXmin() 
	      + corrEta*SCTBarrelYSize;
	    float ModCenterPosY = m_sct_b_s0_biased_xresvsmodetaphi_3ds[layerDisk]->GetYaxis()->GetXmin() 
	      + (modPhi+0.5)*SCTBarrelXSize;
	    if(sctSide==0)
	      {
		// SALVA (13/May/2015) temporary fix.. used the biased histogram to store the unbiased residuals when modules divides in a Nsplit x Nsplit grid
		// SALVA m_sct_b_s0_biased_xresvsmodetaphi_3ds[layerDisk]->Fill(ModCenterPosX+elocalY, ModCenterPosY+mlocalX,biasedResidualX, hweight);
		m_sct_b_s0_biased_xresvsmodetaphi_3ds[layerDisk]->Fill(ModCenterPosX+elocalY, ModCenterPosY+mlocalX, residualX, hweight);
		m_sct_b_s0_xresvsmodetaphi_3ds[layerDisk] -> Fill(modEta, modPhi, residualX, hweight);
	      }
	    else
	      {
		// SALVA m_sct_b_s1_biased_xresvsmodetaphi_3ds[layerDisk]->Fill(ModCenterPosX+elocalY, ModCenterPosY+mlocalX,biasedResidualX, hweight);		   
		// SALVA (13/May/2015) temporary fix.. used the biased histogram to store the unbiased residuals when modules divides in a Nsplit x Nsplit grid
		m_sct_b_s1_biased_xresvsmodetaphi_3ds[layerDisk]->Fill(ModCenterPosX+elocalY, ModCenterPosY+mlocalX, residualX, hweight);
		m_sct_b_s1_xresvsmodetaphi_3ds[layerDisk] -> Fill(modEta, modPhi, residualX, hweight);
	      }
	  }
	 
	  else if(barrelEC==2) {//SCT endcap A
	    //WARNING! hardcoded!
	    int RingShift[3] = {0,52+m_gap_sct,92+2*m_gap_sct};
	    m_sct_eca_residualx_fine-> Fill(residualX, hweight);
	    m_sct_eca_biased_residualx   -> Fill(biasedResidualX       , hweight);
	    m_sct_eca_biased_residualx_pt-> Fill(trkpt, biasedResidualX, hweight);
	    m_sct_eca_residualx_pt       -> Fill(trkpt, residualX      , hweight);
	    m_sct_eca_pullx_pt           -> Fill(trkpt, pullX          , hweight);
	    m_sct_eca_biased_residualx_qoverp2  -> Fill(trkqoverp2, biasedResidualX, hweight);
	    m_sct_eca_unbiased_residualx_qoverp2-> Fill(trkqoverp2, residualX      , hweight);
	    m_hiterror_sct_ec                   -> Fill(hitErrorX                  , hweight);
	    m_hiterror_sct_ec_WideRange         -> Fill(hitErrorX                  , hweight);

	    m_sct_eca_biased_residualsx[layerDisk]   -> Fill(biasedResidualX       , hweight);
	    m_sct_eca_biased_residualsx_pt[layerDisk]-> Fill(trkpt, biasedResidualX, hweight);
	    m_sct_eca_residualsx_pt[layerDisk]       -> Fill(trkpt, residualX      , hweight);
	    m_sct_eca_pullsx_pt[layerDisk]           -> Fill(trkpt, pullX          , hweight);
	    m_sct_eca_biased_residualsx_qoverp2[layerDisk]  -> Fill(trkqoverp2, biasedResidualX, hweight);
	    m_sct_eca_residualsx_qoverp2[layerDisk]-> Fill(trkqoverp2, residualX      , hweight);
	    m_sct_eca_xresvsmodphi_disks[layerDisk]           -> Fill(modPhi + RingShift[layerDisk],residualX, hweight);
	    m_sct_eca_biased_xresvsmodphi_disks[layerDisk]    -> Fill(modPhi + RingShift[layerDisk],biasedResidualX, hweight);
	   
	    if (m_doClusterSizeHistos)
	      {
		m_sct_eca_clustersizePhi[layerDisk] ->Fill(ClusSizePhi,hweight);
		if (sctSide==0) 
		  {
		    m_sct_eca_residualsx_incidentAngle_s0 [layerDisk] ->Fill(incidenceTheta,residualX,hweight);
		    m_sct_eca_residualsx_incidentAnglePhi_s0[layerDisk] ->Fill(incidencePhi,residualX,hweight);
		  }
		if (sctSide==1) 
		  {
		    m_sct_eca_residualsx_incidentAngle_s1 [layerDisk] ->Fill(incidenceTheta,residualX,hweight);
		    m_sct_eca_residualsx_incidentAnglePhi_s1[layerDisk] ->Fill(incidencePhi,residualX,hweight);
		  }
		
		m_sct_eca_residualsx_clustersizePhi[layerDisk] ->Fill(ClusSizePhi,residualX,hweight);
		m_sct_eca_residualsx_clustersizePhiP[layerDisk] ->Fill(ClusSizePhi,residualX,hweight);
		m_sct_eca_clustersizePhi_incidentAngle[layerDisk] ->Fill(incidenceTheta,ClusSizePhi,hweight);
		m_sct_eca_clustersizePhi_incidentAnglePhi[layerDisk] ->Fill(incidencePhi,ClusSizePhi,hweight);
	      }
	    if(sctSide==0){
	      m_sct_eca_s0_xresvsmodetaphi_3ds[layerDisk] -> Fill(modEta,modPhi,residualX, hweight);
	      m_sct_eca_s0_biased_xresvsmodetaphi_3ds[layerDisk] -> Fill(modEta,modPhi,biasedResidualX, hweight);
	    }
	    else{
	      m_sct_eca_s1_xresvsmodetaphi_3ds[layerDisk] -> Fill(modEta,modPhi,residualX, hweight);
	      m_sct_eca_s1_biased_xresvsmodetaphi_3ds[layerDisk] -> Fill(modEta,modPhi,biasedResidualX, hweight);
	    }
	  }
	   
	  else if(barrelEC==-2) {
	    //WARNING! hardcoded!
	    int RingShift[3] = {0,52+m_gap_sct,92+2*m_gap_sct};
	    m_sct_ecc_residualx_fine-> Fill(residualX, hweight);
	    m_sct_ecc_biased_residualx   -> Fill(biasedResidualX       , hweight);
	    m_sct_ecc_biased_residualx_pt-> Fill(trkpt, biasedResidualX, hweight);
	    m_sct_ecc_residualx_pt       -> Fill(trkpt, residualX      , hweight);
	    m_sct_ecc_pullx_pt           -> Fill(trkpt, pullX          , hweight);
	    m_sct_ecc_biased_residualx_qoverp2  -> Fill(trkqoverp2, biasedResidualX, hweight);
	    m_sct_ecc_unbiased_residualx_qoverp2-> Fill(trkqoverp2, residualX      , hweight);
	    m_hiterror_sct_ec                   -> Fill(hitErrorX                  , hweight);
	    m_hiterror_sct_ec_WideRange         -> Fill(hitErrorX                  , hweight);

	    m_sct_ecc_biased_residualsx[layerDisk]   -> Fill(biasedResidualX       , hweight);
	    m_sct_ecc_biased_residualsx_pt[layerDisk]-> Fill(trkpt, biasedResidualX, hweight);
	    m_sct_ecc_residualsx_pt[layerDisk]       -> Fill(trkpt, residualX      , hweight);
	    m_sct_ecc_pullsx_pt[layerDisk]          -> Fill(trkpt, pullX          , hweight);
	    m_sct_ecc_biased_residualsx_qoverp2[layerDisk]  -> Fill(trkqoverp2, biasedResidualX, hweight);
	    m_sct_ecc_residualsx_qoverp2[layerDisk]-> Fill(trkqoverp2, residualX      , hweight);
	    m_sct_ecc_xresvsmodphi_disks[layerDisk]  -> Fill(modPhi    + RingShift[layerDisk], residualX      , hweight);
	    m_sct_ecc_biased_xresvsmodphi_disks[layerDisk]    -> Fill(modPhi    + RingShift[layerDisk], biasedResidualX, hweight);
	    if (m_doClusterSizeHistos){
	      m_sct_ecc_clustersizePhi[layerDisk] ->Fill(ClusSizePhi,hweight);
	      if (sctSide==0) 
		  {
		    m_sct_ecc_residualsx_incidentAngle_s0 [layerDisk] ->Fill(incidenceTheta,residualX,hweight);
		    m_sct_ecc_residualsx_incidentAnglePhi_s0[layerDisk] ->Fill(incidencePhi,residualX,hweight);
		  }
		if (sctSide==1) 
		  {
		    m_sct_ecc_residualsx_incidentAngle_s1 [layerDisk] ->Fill(incidenceTheta,residualX,hweight);
		    m_sct_ecc_residualsx_incidentAnglePhi_s1[layerDisk] ->Fill(incidencePhi,residualX,hweight);
		  }
	     
	      m_sct_ecc_residualsx_clustersizePhi[layerDisk] ->Fill(ClusSizePhi,residualX,hweight);
	      m_sct_ecc_residualsx_clustersizePhiP[layerDisk] ->Fill(ClusSizePhi,residualX,hweight);
	      m_sct_ecc_clustersizePhi_incidentAngle[layerDisk] ->Fill(incidenceTheta,ClusSizePhi,hweight);
	      m_sct_ecc_clustersizePhi_incidentAnglePhi[layerDisk] ->Fill(incidencePhi,ClusSizePhi,hweight);
	      
	    }
	      if(sctSide==0){
	      m_sct_ecc_s0_xresvsmodetaphi_3ds[layerDisk] -> Fill(modEta,modPhi,residualX, hweight);
	      m_sct_ecc_s0_biased_xresvsmodetaphi_3ds[layerDisk] -> Fill(modEta,modPhi,biasedResidualX, hweight);
	    }
	    else
	      {
		m_sct_ecc_s1_xresvsmodetaphi_3ds[layerDisk] -> Fill(modEta,modPhi,residualX, hweight);
		m_sct_ecc_s1_biased_xresvsmodetaphi_3ds[layerDisk] -> Fill(modEta,modPhi,biasedResidualX, hweight);
	      }
	  }
	 
	}
      }
      ++nHits; 
      ++nHitsEvent;			
    }//end of loop on track surfaces
		
    ++nTracks;
		
  } // end of loop on tracks
  
  if (m_changedlumiblock)
    {
      
      
  //Making the fit for the IBL bowing. This has to be done at lowest time granularity possible: we will try to make it lumiBlock wise.
    
      
      //If too less events I want to put those ones. 
      float mag = -999., base = -999., mag_er=0.,base_er=0.;
      //minimum numbers of entries per lumiblock to perform the fit: if -1 don't make the fit. Only for testing.
      
      int lumibin = lumiblock+1;
      
      if ( nIBLHitsPerLB > m_minIBLhits )
	{
	  TH1D* projection_lumiblock = m_pix_b0_resXvsetaLumiBlock->ProjectionY(("iblBowingProjection_lumiblock_"+intToString(lumibin-1)).c_str(),lumibin,lumibin);
	  //if (projection_lumiblock->GetEntries() > min_entries)
	  //{
	  MakeStaveShapeFit(mag,mag_er,base,base_er,projection_lumiblock);
	  m_mag_vs_LB->SetBinContent(lumibin,mag);
	  m_mag_vs_LB->SetBinError(lumibin,mag_er);
	  m_base_vs_LB->SetBinContent(lumibin,base);
	  m_base_vs_LB->SetBinError(lumibin,base_er);
	  TH1D* projection_lumiblock_planars = m_pix_b0_resXvsetaLumiBlock_planars->ProjectionY(("planars_iblBowingProjection_lumiblock_"+intToString(lumibin-1)).c_str(),lumibin,lumibin);
	  MakeStaveShapeFit(mag,mag_er,base,base_er,projection_lumiblock_planars);
	  m_mag_vs_LB_planars->SetBinContent(lumibin,mag);
	  m_mag_vs_LB_planars->SetBinError(lumibin,mag_er);
	  m_base_vs_LB_planars->SetBinContent(lumibin,base);
	  m_base_vs_LB_planars->SetBinError(lumibin,base_er);
	  //}
	  //else
	  //  if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "Fit IBL Shape for LumiBlock : "<< lumiblock<<" disabled because of too few entries!  "<<projection_lumiblock->GetEntries() <<endreq;
	  
	  delete projection_lumiblock;
	}
      else
	if(msgLvl(MSG::WARNING)) 
	  msg(MSG::WARNING) << "Fit IBL Shape for LumiBlock : "<< lumiblock<<" disabled. Too Few hits"<<endreq; 
  
      

      nIBLHitsPerLB=0;
    }// End of lumiblock
    
  delete tracks;	
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Number of tracks : "<< nTracks << endreq;
  
  
  
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------------------

StatusCode IDAlignMonResiduals::procHistograms()
{  
  if( endOfLowStat ) {
  }
  if( endOfLumiBlock ) {
    
  }
  if( endOfRun || ( ( AthenaMonManager::environment() == AthenaMonManager::online ) && endOfLumiBlock ) ) { 
    for(unsigned int side=0;side<3;++side){
      
      //single TRT residuals
      for(unsigned int lay=0;  lay<3; ++lay) {
	fillRMSFromProfile(m_trt_b_hist->aveRes_l[side][lay],m_trt_b_hist->rmsRes_l[side][lay]);
	SetMinWindow(m_trt_b_hist->aveRes_l[side][lay], m_minTRTResWindow, m_maxTRTResWindow);
	SetMinWindow(m_trt_b_hist->aveResVsTrackEta_l[side][lay], m_minTRTResWindow, m_maxTRTResWindow);
	/* new resVsPhiZ/Eta plots */
	if(m_extendedPlots){
	  meanRMSProjection2D(m_trt_b_hist->resVsPhiZ[side][lay],m_trt_b_hist->aveResVsPhiZ[side][lay],0,m_useGausFit);
	  meanRMSProjection2D(m_trt_b_hist->resVsPhiZ[side][lay],m_trt_b_hist->rmsResVsPhiZ[side][lay],1,m_useGausFit);
	  meanRMSProjection2D(m_trt_b_hist->resVsPhiEta[side][lay],m_trt_b_hist->aveResVsPhiEta[side][lay],0,m_useGausFit);
	  meanRMSProjection2D(m_trt_b_hist->resVsPhiEta[side][lay],m_trt_b_hist->rmsResVsPhiEta[side][lay],1,m_useGausFit);
	}
      }
      
      
      if (m_extendedPlots)
	{
	  for(unsigned int phi=0; phi <32; ++phi){
	    fillRMSFromProfile(m_trt_b_hist->aveResVsStrawLayerStackLevel[side][phi],m_trt_b_hist->rmsResVsStrawLayerStackLevel[side][phi]);
	    SetMinWindow(m_trt_b_hist->aveResVsStrawLayerStackLevel[side][phi],m_minTRTResWindow, m_maxTRTResWindow);
	  }
	}
	  
      for(unsigned int position=0; position<5; ++position){ 
	fillRMSFromProfile(m_trt_b_hist->aveResOverPhiVsStrawLayer[position][side],m_trt_b_hist->rmsResOverPhiVsStrawLayer[position][side]);
      }
    }
    
    for(unsigned int endcap=0; endcap<2;++endcap){
      fillRMSFromProfile(m_trt_ec_hist->aveResVsRing[endcap],m_trt_ec_hist->rmsResVsRing[endcap]);
      fillRMSFromProfile(m_trt_ec_hist->aveResVsPhiSec[endcap],m_trt_ec_hist->rmsResVsPhiSec[endcap]);
      
      SetMinWindow(m_trt_ec_hist->aveResVsRing[endcap], m_minTRTResWindow, m_maxTRTResWindow);
      SetMinWindow(m_trt_ec_hist->aveResVsPhiSec[endcap], m_minTRTResWindow, m_maxTRTResWindow);
      SetMinWindow(m_trt_ec_hist->aveResVsTrackEta[endcap], m_minTRTResWindow, m_maxTRTResWindow);

      if(m_extendedPlots){
	meanRMSProjection2D(m_trt_ec_hist->resVsPhiWheel[endcap],m_trt_ec_hist->aveResVsPhiWheel[endcap],0,m_useGausFit);
	meanRMSProjection2D(m_trt_ec_hist->resVsPhiWheel[endcap],m_trt_ec_hist->rmsResVsPhiWheel[endcap],1,m_useGausFit);
	
	meanRMSProjection2D(m_trt_ec_hist->resVsRadiusWheelPos[endcap],m_trt_ec_hist->aveResVsRadiusWheelPos[endcap],0,m_useGausFit);
	meanRMSProjection2D(m_trt_ec_hist->resVsRadiusWheelPos[endcap],m_trt_ec_hist->rmsResVsRadiusWheelPos[endcap],1,m_useGausFit);
	meanRMSProjection2D(m_trt_ec_hist->resVsRadiusWheelNeg[endcap],m_trt_ec_hist->aveResVsRadiusWheelNeg[endcap],0,m_useGausFit);
	meanRMSProjection2D(m_trt_ec_hist->resVsRadiusWheelNeg[endcap],m_trt_ec_hist->rmsResVsRadiusWheelNeg[endcap],1,m_useGausFit);

	// TRT EC Maps
	meanRMSProjection2D(m_trt_ec_hist->resVsRadiusWheelPos[endcap],m_trt_ec_hist->aveResVsRadiusWheelPos[endcap],0,m_useGausFit);
	meanRMSProjection2D(m_trt_ec_hist->resVsRadiusWheelPos[endcap],m_trt_ec_hist->rmsResVsRadiusWheelPos[endcap],1,m_useGausFit);
	meanRMSProjection2D(m_trt_ec_hist->resVsRadiusWheelNeg[endcap],m_trt_ec_hist->aveResVsRadiusWheelNeg[endcap],0,m_useGausFit);
	meanRMSProjection2D(m_trt_ec_hist->resVsRadiusWheelNeg[endcap],m_trt_ec_hist->rmsResVsRadiusWheelNeg[endcap],1,m_useGausFit);
      }
    }
    //overlap residuals
    fillRMSFromProfile(m_pix_bec_Oxresx_mean,m_pix_bec_Oxresx_rms);
    fillRMSFromProfile(m_pix_bec_Oxresy_mean,m_pix_bec_Oxresy_rms);
    fillRMSFromProfile(m_pix_bec_Oyresx_mean,m_pix_bec_Oyresx_rms);
    fillRMSFromProfile(m_pix_bec_Oyresy_mean,m_pix_bec_Oyresy_rms);
    fillRMSFromProfile(m_sct_bec_Oxresx_mean,m_sct_bec_Oxresx_rms);
    fillRMSFromProfile(m_sct_bec_Oyresx_mean,m_sct_bec_Oyresx_rms);
    
  }
  
  return StatusCode::SUCCESS;
}


//__________________________________________________________________________
StatusCode  IDAlignMonResiduals::getSiResiduals(const Trk::Track* track, const Trk::TrackStateOnSurface* tsos, bool unBias, double* results)
{
  if (!m_doPulls) return StatusCode::FAILURE;
  
  StatusCode sc = StatusCode::SUCCESS;
  
  double residualX = -9999.0;
  double residualY = -9999.0;
  double pullX = -9999.0;
  double pullY = -9999.0;
  
  //extract the hit object from the tsos
  const Trk::MeasurementBase* mesh =tsos->measurementOnTrack();
  const Trk::RIO_OnTrack* hit = dynamic_cast <const Trk::RIO_OnTrack*>(mesh);
  
  //get the unbiased track parameters (can fail if no MeasuredTrackParameters exists)
  const Trk::TrackParameters* trackParameterUnbiased = NULL;
  if(unBias) trackParameterUnbiased = getUnbiasedTrackParameters(track,tsos);
	
  //updator can fail in defining unbiased parameters, in which case we use biased
  const Trk::TrackParameters* trackParameterForResiduals = NULL;
  if(trackParameterUnbiased) trackParameterForResiduals = trackParameterUnbiased;
  else {
    //use the original biased track parameters
    trackParameterForResiduals = tsos->trackParameters();
  }
	
  if (!m_residualPullCalculator.empty() && !m_residualPullCalculator.retrieve().isFailure()) {
		
    if (hit && trackParameterForResiduals) {
			
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<" got hit and track parameters " << endreq;
			
      //const Trk::ResidualPull* residualPull = m_residualPullCalculator->residualPull(hit, trackParameterForResiduals, unBias);
      const Trk::ResidualPull* residualPull = NULL;
      if(unBias) residualPull = m_residualPullCalculator->residualPull(mesh, trackParameterForResiduals, Trk::ResidualPull::Unbiased);
      else residualPull = m_residualPullCalculator->residualPull(mesh, trackParameterForResiduals, Trk::ResidualPull::Biased);
			
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<" got hit and track parameters...done " << endreq;
      if (residualPull) {
				
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " got residual pull object" << endreq;
	residualX = residualPull->residual()[Trk::loc1];
	if(residualPull->isPullValid()) pullX = residualPull->pull()[Trk::loc1];
	else {
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ResidualPullCalculator finds invalid X Pull!!!" << endreq;
	  sc = StatusCode::FAILURE;
	}
				
	if (residualPull->dimension() >= 2){
					
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " residualPull dim >= 2" << endreq;
	  residualY = residualPull->residual()[Trk::loc2];
					
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " residual Y = " << residualY << endreq; 
	  if(residualPull->isPullValid()) pullY = residualPull->pull()[Trk::loc2];
	  else {
	    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ResidualPullCalculator finds invalid Y Pull!!!" << endreq;
	    sc = StatusCode::FAILURE;
	  }
	}
				
	delete residualPull;
				
      }
      else {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ResidualPullCalculator failed!" << endreq;
	sc = StatusCode::FAILURE;
      }
    }
  }
	
  // for SCT modules the residual pull calculator only finds the (rotated) Rphi residual
  // for each of the SCT sides; residualPull->dimension()==1 always.
	
  //std::pair <double, double> result(residualX, residualY);
  results[0] = residualX; 
  results[1] = residualY;
  results[2] = pullX;
  results[3] = pullY;
	
  if(pullX!=pullX || pullY!=pullY){
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ResidualPullCalculator finds Pull=NAN!!!" << endreq;
    sc = StatusCode::FAILURE;
  }
	
  //delete these TrackParameters which are newly created in the getUnbiasedTrackParameters(track,tsos) method
  delete trackParameterUnbiased;
	
  return sc;
	
}

//__________________________________________________________________________
std::pair<const Trk::TrackStateOnSurface*, const Trk::TrackStateOnSurface*> IDAlignMonResiduals::findOverlapHit(const Trk::Track* trk, const Trk::RIO_OnTrack* hit)
{
  
  // this method identifies which hits on modules for Trk::Track* trk overlap with Trk::RIO_OnTrack* hit
  // - only one overlapping module should be returned
  // - the overlapping module must be at a greater global radius than Trk::RIO_OnTrack* hit 
  //   this avoids double counting when you are trying to find all the unique overlaps for a track
  // - for SCT overlaps, the overlapping module must have the same axial/stereo orientation as the Trk::RIO_OnTrack* hit
  //   otherwise the computation of the overlap residual is using two different coordinate frames and will be distorted
  // returns two Trk::TrackStateOnSurface, one for a potential x overlaps and one for a potential y overlap
  // if no overlap is found the corresponding Trk::TrackStateOnSurface will be null
  //For IBL there is noOverlap. So If detType==0 and layerDisk==0 return null xOverlap and yOverlap
  

  
  const Trk::TrackStateOnSurface* xOverlap = NULL;
  const Trk::TrackStateOnSurface* yOverlap = NULL;
     
      const Identifier & hitId = hit->identify();
      int detType = 99; 
      int barrelEC = 99;
      int layerDisk = 99;
      int modEta = 99; 
      int modPhi = 99;
      if (m_idHelper->is_sct(hitId)) detType = 1;
      else detType = 0;
            
      //determining Si module physical position
      if (detType==1){//sct
	const Identifier& id = m_sctID->wafer_id(hitId);
	barrelEC = m_sctID->barrel_ec(id);
	layerDisk = m_sctID->layer_disk(id);
	modEta = m_sctID->eta_module(id);
	modPhi = m_sctID->phi_module(id);
      }
      
      else {//pixel
	const Identifier& id = m_pixelID->wafer_id(hitId);
	barrelEC = m_pixelID->barrel_ec(id);
	layerDisk = m_pixelID->layer_disk(id);
	modEta = m_pixelID->eta_module(id);
	modPhi = m_pixelID->phi_module(id);
      }
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "******** looking for overlaps for new hit detType = " << detType 
					   << ", modEta = " << modEta << ", modPhi = " << modPhi << " , layerDisk= "<<layerDisk
					   << ", barrelEC= "<<barrelEC<< endreq;
      
      int nHits = 0;
      for (std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsos2=trk->trackStateOnSurfaces()->begin();tsos2!=trk->trackStateOnSurfaces()->end(); ++tsos2) {
	const Trk::MeasurementBase* mesh =(*tsos2)->measurementOnTrack();
	const Trk::RIO_OnTrack* hit2 = dynamic_cast <const Trk::RIO_OnTrack*>(mesh);
	if (hit2== NULL) continue;//the first hit on the track never has associated RIO_OnTrack - just stores track parameters
	++nHits;
	int detType2 = -99; 
	int barrelEC2 = -99;
	int layerDisk2 = -99;
	int modEta2 = -99; 
	int modPhi2 = -99;
			
	//const Identifier& hitId2 = (dynamic_cast <const Trk::RIO_OnTrack*>((*tsos2)->measurementOnTrack()))->identify();
	const Identifier& hitId2 = hit2->identify();
	
	if (m_idHelper->is_sct(hitId2)) 
	  detType2 = 1;	    
	else if (m_idHelper->is_pixel(hitId2)) 
	  detType2 = 0;
	else
	  detType2 = 2;
			
	//determining Si module physical position
	if (detType2==1){//sct
	  const Identifier& id = m_sctID->wafer_id(hitId2);
	  barrelEC2 = m_sctID->barrel_ec(id);
	  layerDisk2 = m_sctID->layer_disk(id);
	  modEta2 = m_sctID->eta_module(id);
	  modPhi2 = m_sctID->phi_module(id);
	}
	else if (detType2==0) {//pixel
	  const Identifier& id = m_pixelID->wafer_id(hitId2);
	  barrelEC2 = m_pixelID->barrel_ec(id);
	  layerDisk2 = m_pixelID->layer_disk(id);
	  modEta2 = m_pixelID->eta_module(id);
	  modPhi2 = m_pixelID->phi_module(id);
	}
	else { //hit in the trt so I skip it
	  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) <<"Skipping hit in the trt? "<< m_idHelper->is_trt(hitId2)<<endreq;
	  continue;
	}
	  
	if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "testing hit2 " << nHits << " for overlap detType = " << detType2 
					   << ", modEta = " << modEta2 << ", modPhi = " << modPhi2 << " , layerDisk= "<<layerDisk2
					   << ", barrelEC= "<<barrelEC2<< endreq;
			
	if(!(*tsos2)->type(Trk::TrackStateOnSurface::Measurement)) {
	  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "overlap rejected because hit is an outlier" << endreq;
	  continue;
	}
			
	if(detType!=detType2) {
	  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "overlap rejected because not the same detector" << endreq;
	  continue;
	}
	if(barrelEC!=barrelEC2) {
	  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "overlap rejected because not the same barrel/endcap" << endreq;
	  continue;
	}
	if(layerDisk!=layerDisk2) {
	  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "overlap rejected because not the same layer/disk" << endreq;
	  continue;
	}
			
	if(modEta==modEta2 && modPhi==modPhi2){
	  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "overlap rejected because this is the original hit (or the opposite side for SCT)" << endreq;
	  continue;
	}
			
	if (detType==1){
	  const InDetDD::SiDetectorElement *siDet = dynamic_cast<const InDetDD::SiDetectorElement*>(hit->detectorElement());
	  bool stereo = siDet->isStereo();
	  const InDetDD::SiDetectorElement *siDet2 = dynamic_cast<const InDetDD::SiDetectorElement*>(hit2->detectorElement());
	  bool stereo2 = siDet2->isStereo();
	  if(stereo!=stereo2){
	    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "SCT overlap rejected because the modules are not both stereo/radial modules" << endreq;
	    continue;
	  }
	}
			
			
	const Trk::TrackParameters* measuredTrackParameter = (*tsos2)->trackParameters();
	const AmgSymMatrix(5)* MeasTrackParCovariance = measuredTrackParameter ? measuredTrackParameter->covariance() : NULL;
	if(MeasTrackParCovariance==NULL) {
				
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "overlap rejected because overlap hit does not have associated measuredTrackParameters" << endreq;
	  continue;
	}
			
	if((modEta==modEta2 && modPhi!=modPhi2) || (modEta-modEta2 == 1 && modPhi==modPhi2)){
	  //potentially an overlap hit - apply hit quality cuts if tool configured
	  if((detType2==0 || detType2==1) && m_doHitQuality) {
	    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "applying hit quality cuts to overlap hit..." << endreq;
					
	    hit2 = m_hitQualityTool->getGoodHit(*tsos2);
	    if(hit2==NULL) {
	      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "overlap rejected because failed hit quality cuts." << endreq;
	      continue;
	    }
	    else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "overlap hit passed quality cuts" << endreq;
	  }
	  else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "hit quality cuts NOT APPLIED to overlap hit." << endreq;
	}
			
	bool close = false; //added by LT
			
	if(modEta==modEta2 && modPhi!=modPhi2){
			
	  //BAD HARDCODING

	  
	  //IBL Overlap has to be disabled.

	  // begin added by TG 
	  // Pixel barrel #phi: 22, 38, 52
	  // Pixel EC #phi: 48
	  // SCT barrel #phi: 32, 40, 48, 56
	  // SCT EC #phi: 52 ???
	  if(modPhi-modPhi2 == 1){
	    close = true;
	    // Pix barrel special cases 
	  } else if( (detType==0 && barrelEC==0 && layerDisk==3) && modPhi-modPhi2 == -51){
	    close = true;
	  } else if ( (detType==0 && barrelEC==0 && layerDisk==2) && modPhi-modPhi2 == -37){
	    close = true;    
	  } else if ( (detType==0 && barrelEC==0 && layerDisk==1) && modPhi-modPhi2 == -21){
	    close = true;
	    // SCT barrel special cases
	  } else if( (detType==1 && barrelEC==0 && layerDisk==3) && modPhi-modPhi2 == -55){
	    close = true;
	  } else if( (detType==1 && barrelEC==0 && layerDisk==2) && modPhi-modPhi2 == -47){
	    close = true;
	  } else if ( (detType==1 && barrelEC==0 && layerDisk==1) && modPhi-modPhi2 == -39){
	    close = true;    
	  } else if ( (detType==1 && barrelEC==0 && layerDisk==0) && modPhi-modPhi2 == -31){
	    close = true;
	    // Pix EC special cases
	  } else if( (detType==0 && abs(barrelEC)==2) && modPhi-modPhi2 == -47){
	    close = true;
	    // SCT EC special cases: check!!!
	  } else if( (detType==1 && abs(barrelEC)==2) && modPhi-modPhi2 == -51){
	    close = true;
	  }
	  if(close){  //end add by TG
	    //	if(msgLvl(MSG::DEBUG)) msg() <<  "original module radius = " << radius << endreq;
	    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) <<  "***** identified local X overlap in the IBL" << endreq;
	    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) <<  "original module phi, eta,layerDisk,barrelEC  = " << modEta <<", "<<modPhi<<",  "<< layerDisk <<" , "<< barrelEC  << endreq;
	    //if(msgLvl(MSG::DEBUG)) msg() <<  "overlap module radius = " << radius2 << endreq;
	    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) <<  "second module phi, eta,layerDisk,barrelEC  = " << modEta2 <<", "<<modPhi2<<layerDisk<<barrelEC<< endreq;
	    xOverlap = (*tsos2);
	  } //added by LT
				
	}
	if(modEta-modEta2 == 1 && modPhi==modPhi2){
	  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) <<  "***** identified local Y overlap" << endreq;
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "modEta2 = " << modEta2 << endreq;
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "modPhi2 = " << modPhi2 << endreq;
	  //if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "original module radius = " << radius << endreq;
	  //if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "overlap module radius = " << radius2 << endreq;
	  yOverlap = (*tsos2);	  
	}
      }		
    
  //std::pair <const Trk::TrackStateOnSurface*, const Trk::TrackStateOnSurface*> result(xOverlap, yOverlap);
  return std::pair <const Trk::TrackStateOnSurface*, const Trk::TrackStateOnSurface*> (xOverlap, yOverlap);
}

//__________________________________________________________________________
const Trk::TrackParameters* IDAlignMonResiduals::getUnbiasedTrackParameters(const Trk::Track* trkPnt, const Trk::TrackStateOnSurface* tsos)
{
	
  const Trk::TrackParameters* TrackParams;
  const Trk::TrackParameters* UnbiasedTrackParams(0);
  const Trk::TrackParameters* PropagatedTrackParams(0);
  const Trk::TrackParameters* OtherSideUnbiasedTrackParams(0);
	
  //controls if the SCT residuals will be 'truly' unbiased - removing also the opposite side hit.
  bool trueUnbiased = true;
	
  Identifier surfaceID;
	
	
  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "original track parameters: " << *(tsos->trackParameters()) <<endreq;
	
	
  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Trying to unbias track parameters." << endreq;
	
  const Trk::RIO_OnTrack* hitOnTrack = dynamic_cast <const Trk::RIO_OnTrack*>(tsos->measurementOnTrack());
	
  if (hitOnTrack != 0) 
    surfaceID = hitOnTrack->identify(); 
                                                             

  // if SCT Hit and TrueUnbiased then remove other side hit first
  if (surfaceID.is_valid() && trueUnbiased && m_idHelper->is_sct(surfaceID)) {  //there's no TrueUnbiased for non-SCT (pixel) hits)
    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Entering True Unbiased loop." << endreq;
    // check if other module side was also hit and try to remove other hit as well
    const Trk::TrackStateOnSurface* OtherModuleSideHit(0);
    const Identifier& OtherModuleSideID = m_SCT_Mgr->getDetectorElement(surfaceID)->otherSide()->identify();
		
    for (std::vector<const Trk::TrackStateOnSurface*>::const_iterator TempTsos=trkPnt->trackStateOnSurfaces()->begin();TempTsos!=trkPnt->trackStateOnSurfaces()->end(); ++TempTsos) {
			
      const Trk::RIO_OnTrack* TempHitOnTrack = dynamic_cast <const Trk::RIO_OnTrack*>((*TempTsos)->measurementOnTrack());
      if (TempHitOnTrack != 0) {
	//const Identifier& trkID = TempHitOnTrack->identify();
	//if (m_sctID->wafer_id(trkID) == OtherModuleSideID) {
	if (m_sctID->wafer_id(TempHitOnTrack->identify()) == OtherModuleSideID) {
	  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "True unbiased residual. Removing OtherModuleSide Hit " << m_idHelper->show_to_string(OtherModuleSideID,0,'/') << endreq;
	  OtherModuleSideHit = *TempTsos;
	}
      }
    }
		
    if (OtherModuleSideHit) {
			
      const Trk::TrackParameters* OMSHmeasuredTrackParameter = OtherModuleSideHit->trackParameters();
      const AmgSymMatrix(5)* OMSHmeasuredTrackParameterCov = OMSHmeasuredTrackParameter ? OMSHmeasuredTrackParameter->covariance() : NULL;
			
      // check that the hit on the other module side has measuredtrackparameters, otherwise it cannot be removed from the track
      if (OMSHmeasuredTrackParameterCov) {
				
	if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "OtherSideTrackParameters: " << *(OtherModuleSideHit->trackParameters()) << endreq;
	OtherSideUnbiasedTrackParams = m_iUpdator->removeFromState(*(OtherModuleSideHit->trackParameters()),
								   OtherModuleSideHit->measurementOnTrack()->localParameters(),
								   OtherModuleSideHit->measurementOnTrack()->localCovariance());
				
	if (OtherSideUnbiasedTrackParams) {
	  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Unbiased OtherSideTrackParameters: " << *OtherSideUnbiasedTrackParams << endreq;
					
					
	  const Trk::Surface* TempSurface = &(OtherModuleSideHit->measurementOnTrack()->associatedSurface());
					
	  const Trk::MagneticFieldProperties* TempField = 0;
	  if (TempSurface)
	    {
	      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "After OtherSide surface call. Surface exists" << endreq;
	      if (TempSurface->associatedLayer())
		{
		  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TempSurface->associatedLayer() exists" << endreq;
		  if(TempSurface->associatedLayer()->enclosingTrackingVolume())
		    {
		      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TempSurface->associatedLayer()->enclosingTrackingVolume exists" << endreq;
								
		      TempField = dynamic_cast <const Trk::MagneticFieldProperties*>(TempSurface->associatedLayer()->enclosingTrackingVolume());
		      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "After MagneticFieldProperties cast" << endreq;
		      
		      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Before other side unbiased propagation" << endreq;
		      if (TempSurface->associatedLayer() && TempField) PropagatedTrackParams = m_propagator->propagate(*OtherSideUnbiasedTrackParams,
														       tsos->measurementOnTrack()->associatedSurface(),
														       Trk::anyDirection, false,
														       *TempField,
														       Trk::nonInteracting);
	
							
		    } else {
		    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TempSurface->associatedLayer()->enclosingTrackingVolume does not exist" << endreq;
		  }
		} else {
		if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TempSurface->associatedLayer() does not exist" << endreq;
	      }
	    } else {
	    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "After OtherSide surface call. Surface does not exist" << endreq;
	  }
					
					
	  				
	  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "After other side unbiased propagation" << endreq;
	  delete OtherSideUnbiasedTrackParams;
	  if (PropagatedTrackParams) {
	    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Propagated Track Parameters: " << *PropagatedTrackParams << endreq;
	  } else {
	    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Propagation of unbiased OtherSideParameters failed" << endreq;
	  }
	} else {
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "RemoveFromState did not work for OtherSideParameters" << endreq;
	}
      } else {
	if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "No OtherModuleSideHit Measured Track Parameters found" << endreq;
      }
    } else {
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "No OtherModuleSideHit found" << endreq;
    }
  }
	
  // if propagation failed or no TrueUnbiased or no SCT then use original TrackParams
  if (!PropagatedTrackParams) {
    PropagatedTrackParams = tsos->trackParameters()->clone();
  }
	
    
  UnbiasedTrackParams = m_iUpdator->removeFromState(*PropagatedTrackParams, tsos->measurementOnTrack()->localParameters(), tsos->measurementOnTrack()->localCovariance());
	
  delete PropagatedTrackParams;
	
  if (UnbiasedTrackParams) {
    if(msgLvl(MSG::VERBOSE) && surfaceID.is_valid()) msg(MSG::VERBOSE) << "Unbiased residual. Removing original Hit " << m_idHelper->show_to_string(surfaceID,0,'/') << endreq;
    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Unbiased Trackparameters: " << *UnbiasedTrackParams << endreq;
		
    TrackParams = UnbiasedTrackParams->clone();
		
  } else { // Unbiasing went awry.
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "RemoveFromState did not work, using original TrackParameters" << endreq;
    TrackParams = tsos->trackParameters()->clone();
  }
	
	
  delete UnbiasedTrackParams;  
  return TrackParams;
	
}

//---------------------------------------------------------------------------------------

StatusCode IDAlignMonResiduals::setupTools()
{
  //initializing tools
	
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In setupTools()" << endreq;
	
  StatusCode sc;
   //Get the PIX manager from the detector store

  sc = detStore()->retrieve(m_PIX_Mgr,m_Pixel_Manager);
  if (sc.isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get PIX_Manager !" << endreq;
    return StatusCode::FAILURE;
   }
  
  //Get the SCT manager from the detector store
  sc = detStore()->retrieve(m_SCT_Mgr, m_SCT_Manager);
  if (sc.isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get SCT_Manager !" << endreq;
    return StatusCode::FAILURE;
  }
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialized SCTManager" << endreq;
	
  sc = detStore()->retrieve(m_pixelID, "PixelID");
  if (sc.isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get Pixel ID helper !" << endreq;
    return StatusCode::FAILURE;
  }
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialized PixelIDHelper" << endreq;
	
  sc = detStore()->retrieve(m_sctID, "SCT_ID");
  if (sc.isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get SCT ID helper !" << endreq;
    return StatusCode::FAILURE;
  }
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialized SCTIDHelper" << endreq;
	
  sc = detStore()->retrieve(m_trtID, "TRT_ID");
  if (sc.isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get TRT ID helper !" << endreq;
    return StatusCode::FAILURE;
  }
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialized TRTIDHelper" << endreq;
	
  //ID Helper
  sc = detStore()->retrieve(m_idHelper, "AtlasID" );
  if (sc.isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get AtlasDetectorID !" << endreq;
    return StatusCode::SUCCESS;
  }else{
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found AtlasDetectorID" << endreq;
  }
	
  if (m_iUpdator.retrieve().isFailure() ) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed to retrieve tool " << m_iUpdator << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_iUpdator << endreq;
  }
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved IUpdator" << endreq;
	
	
  if (m_propagator.retrieve().isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Can not retrieve Propagator tool of type "
					       << m_propagator.typeAndName() << endreq;
    return StatusCode::FAILURE;
  } else msg(MSG::INFO) << "Retrieved tool " << m_propagator.typeAndName() << endreq;
	
  if (m_trackSelection.retrieve().isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Can not retrieve TrackSelection tool of type "
					       << m_trackSelection.typeAndName() << endreq;
    return StatusCode::FAILURE;
  } else msg(MSG::INFO) << "Retrieved tool " << m_trackSelection.typeAndName() << endreq;
	
  if (m_residualPullCalculator.empty()) {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << 
			     "No residual/pull calculator for general hit residuals configured."
					   << endreq;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << 
			     "It is recommended to give R/P calculators to the det-specific tool"
					   << " handle lists then." << endreq;
    m_doPulls = false;
  } else if (m_residualPullCalculator.retrieve().isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve "<< m_residualPullCalculator 
					       <<" (to calculate residuals and pulls) "<< endreq;
    m_doPulls = false;
		
  } else {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) 
			     << "Generic hit residuals&pulls will be calculated in one or both "
			     << "available local coordinates" << endreq;
    m_doPulls = true;
  }
	
  if (m_hitQualityTool.empty()) {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << 
			     "No hit quality tool configured - not hit quality cuts will be imposed"
					   << endreq;
    m_doHitQuality = false;
  } else if (m_hitQualityTool.retrieve().isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve "<< m_hitQualityTool 
					       <<" (to apply hit quality cuts to Si hits) "<< endreq;
    m_doHitQuality = false;
  } else {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) 
			     << "Hit quality tool setup "
			     << "- hit quality cuts will be applied to Si hits" << endreq;
    m_doHitQuality = true;
  }
	
	
  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------------------------

bool IDAlignMonResiduals::trackRequiresRefit(const Trk::Track* track)
{
	
  // Checks to see if any of the measurements on track do not have track parameters associated 
  // (as happens for certain track collections in e.g. ESD)
  // If this is the case we cannot define residuals and track needs to be refitted (return true)
	
  bool refitTrack = false;
	
  int nHits = 0;
  int nHitsNoParams = 0;
	
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Testing track to see if requires refit..." << endreq;
	
  for (std::vector<const Trk::TrackStateOnSurface*>::const_iterator iter_tsos=(track->trackStateOnSurfaces()->begin());
       iter_tsos!=track->trackStateOnSurfaces()->end(); ++iter_tsos) {//looping over hits
		
    if((*iter_tsos) == NULL) continue;
		
    //skipping outliers
    if(!(*iter_tsos)->type(Trk::TrackStateOnSurface::Measurement)) continue;
		
    const Trk::MeasurementBase* mesh =(*iter_tsos)->measurementOnTrack();
    if (mesh==NULL) continue;
    const Trk::RIO_OnTrack* hit = dynamic_cast <const Trk::RIO_OnTrack*>(mesh);
    if (hit==NULL) continue;
		
    ++nHits;
		
    const Trk::TrackParameters* trackParameter = (*iter_tsos)->trackParameters();
    if(trackParameter==NULL) ++nHitsNoParams; //if no TrackParameters for TSOS we cannot define residuals
		
  }
	
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Total nhits on track (excluding outliers) = " << nHits << ", nhits without trackparameters = " << nHitsNoParams << endreq;
	
  if(nHitsNoParams>0) {
    refitTrack = true;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Track Requires refit to get residuals!!!" << endreq;
  }
	
  return refitTrack;
}

//--------------------------------------------------------------------------------------------

void IDAlignMonResiduals::meanRMSProjections(TH2F* h2d, TH1F* h,int meanrms)
{
	
  //fills a 1-D histogram with either the mean or RMS of the residual distribution for each layer in the
  //barrel. Does this by projecting a 2-D histo of residual vs layer. 
	
  int nbins_2d = h2d->GetNbinsX();
  int nbins_h = h->GetNbinsX();
	
  if(nbins_2d!=nbins_h) if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Mean/RMS Histograms not set up correctly - nbins mismatch" << endreq;
	
  //calling this means that the histogram bin content is flagged 
  //as being an average and so adding histos from different jobs 
  //will produce weighted mean
  h->SetBit(TH1::kIsAverage);
	
  for(int i = 1; i!=nbins_2d+1; ++i){
		
    TH1F* hproj = (TH1F*)h2d->ProjectionY("proj",i,i,"e");
		
    //do not fill if there are 5 or less entries in the bin
    if(hproj->GetEntries()<=5 || hproj->Integral()<=5) {
      delete hproj;
      continue;
    }
		
    if(meanrms==0){
      h->SetBinContent(i,hproj->GetMean());
      h->SetBinError(i,hproj->GetMeanError());
			
    }
    else if(meanrms==1){
      h->SetBinContent(i,hproj->GetRMS());
      h->SetBinError(i,hproj->GetRMSError());
    }
    else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Incorrect switch in MeanRMSProjectionsBarrel()" << endreq;
		
    delete hproj;
  }
	
  return;
	
}

//--------------------------------------------------------------------------------------------

void IDAlignMonResiduals::meanRMSProjection2D(TH3F* h3d, TH2F* h2d,int meanrms,bool fitGaus)
{
	
  int nbins_x_3d = h3d->GetNbinsX();
  int nbins_y_3d = h3d->GetNbinsY();
  int nbins_x_2d = h2d->GetNbinsX();
  int nbins_y_2d = h2d->GetNbinsY();
	
  if(nbins_x_3d!=nbins_x_2d) if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Mean/RMS Histograms not set up correctly - nbins mismatch" << endreq;
  if(nbins_y_3d!=nbins_y_2d) if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Mean/RMS Histograms not set up correctly - nbins mismatch" << endreq;
	
	
  for(int i = 1; i!=nbins_x_2d+1; ++i){
		
    for(int j = 1; j!=nbins_y_2d+1; ++j){
			
      TH1F* hproj = (TH1F*)h3d->ProjectionZ("proj",i,i,j,j,"e");
			
      //do not fill if there are 5 or less entries in the bin
      if(hproj->GetEntries()<=5 || hproj->Integral()<=5) {
	delete hproj;
	continue;
      }

      if(fitGaus){ // fill histo with mean/sigma from a gaussian fit
	hproj->Fit("gaus");
	TF1 *fit = hproj->GetFunction("gaus");
	if(meanrms==0){
	  double mean = fit->GetParameter(1);
	  double meanerr = fit->GetParError(1);
	  h2d->SetBinContent(i,j,mean);
	  h2d->SetBinError(i,j,meanerr);  
	}
	else if(meanrms==1){
	  double sigma = fit->GetParameter(2);
	  double sigmaerr = fit->GetParError(2);
	  h2d->SetBinContent(i,j,sigma);
	  h2d->SetBinError(i,j,sigmaerr);
	}
	else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Incorrect switch in MeanRMSProjections2D()" << endreq;
      }
      else {// !fitGaus
	if(meanrms==0){
	  //std::cout << "Bin (" << i << "," << j << ") has " << hproj->GetEntries() << " entries & mean " << hproj->GetMean() << std::endl;
	  h2d->SetBinContent(i,j,hproj->GetMean());
	  h2d->SetBinError(i,j,hproj->GetMeanError());
	}
	else if(meanrms==1){
	  h2d->SetBinContent(i,j,hproj->GetRMS());
	  h2d->SetBinError(i,j,hproj->GetRMSError());
	}
	else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Incorrect switch in MeanRMSProjections2D()" << endreq;
      }
      delete hproj;
    }
  }
	
  return;
	
}

//--------------------------------------------------------------------------------------------

void IDAlignMonResiduals::fillRMSFromProfile(TProfile* hprof, TProfile* h)
{
  if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
      h->Reset("ICE");
  }
    
  //Uses the spread information in an already filled TProfile to fill a second TProfile 
  //with the (correctly weigthed by N events) RMS of each bin
	
  int nbins = hprof->GetNbinsX();
	
  //h->BuildOptions(h->GetYmin(),h->GetYmax()," ");
  //h->SetErrorOption(" ");
	
  for(int i = 1; i!=nbins+1; ++i){
		
    float centre   = (float)hprof->GetBinCenter(i);
    float err      = (float)hprof->GetBinError(i);//By default this is RMS/sqrt(N)
    float nentries = (float)hprof->GetBinEntries(i);//getting N
		
    float rms = (float)err*(sqrt(nentries));//calculating the RMS from the above quantities
		
    //filling the profile with the RMS value N times to get the statistics right
    //for when RMS TProfile's from several different jobs are merged
    for(int j = 0; j < nentries; ++j){
      h->Fill(centre,rms);      //Need to understand!
    }
  }
  return;
}


//--------------------------------------------------------------------------------------------

void IDAlignMonResiduals::fillGaussianMeanOrWidth(TH2F* h2d, TH1F* h, float fitMin, float fitMax, int iopt)
{
	
  //Makes a Gaussian fit to each bin of a TH2F and fills a TH1F with 
  //the either the mean or width of this Gaussian
  // iopt=0; Mean
  // iopt=1; Width
	
  //calling this means that the histogram bin content is flagged 
  //as being an average and so adding histos from different jobs 
  //will produce weighted mean
  h->SetBit(TH1::kIsAverage);
	
  int nbins_2d = h2d->GetNbinsX();
  int nbins_h = h->GetNbinsX();
	
  if(nbins_2d!=nbins_h) if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Mean/Width Histograms not set up correctly - nbins mismatch" << endreq;
	
  for(int i = 1; i!=nbins_2d+1; ++i){
		
    TH1F* hproj = (TH1F*)h2d->ProjectionY("proj",i,i,"e");
		
    //do not fill if there are 5 or less entries in the bin
    if(hproj->GetEntries()<=5 || hproj->Integral()<=5) {
      delete hproj;
      continue;
    }
		
    TF1 *fit = new TF1("fit","gaus",fitMin,fitMax);
		
    hproj->Fit("fit","RQNM");
    float mean = fit->GetParameter(1);
    float meanErr = fit->GetParError(1);
    float width = fit->GetParameter(2);
    float widthErr = fit->GetParError(2);
		
    if(iopt==0){
      h->SetBinContent(i,mean);
      h->SetBinError(i,meanErr);
    }
    else if(iopt==1){
      h->SetBinContent(i,width);
      h->SetBinError(i,widthErr);
    }
    else  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Incorrect switch in fillGaussianMeanOrWidth" << endreq;
		
    delete hproj;
    delete fit;
  }
	
  return;
	
}

//================================================================
// Establishes a minimim window for the TProfile 
//================================================================
void IDAlignMonResiduals::SetMinWindow(TProfile* hProf, float windowMin, float windowMax){
	
  float min=hProf->GetMinimum();
  float max=hProf->GetMaximum();
  float margin=0.05*(max-min);
  if(min > windowMin) min=windowMin-margin;
  if(max < windowMax) max=windowMax+margin;
  hProf->SetMinimum(min);
  hProf->SetMaximum(max);
	
}

//================================================================
// Establishes a minimim window for TH1
//================================================================
void IDAlignMonResiduals::SetMinWindow(TH1* h1, float windowMin, float windowMax){
	
  float min=h1->GetMinimum();
  float max=h1->GetMaximum();
  float margin=0.05*(max-min);
  if(min > windowMin) min=windowMin-margin;
  if(max < windowMax) max=windowMax+margin;
  h1->SetMinimum(min);
  h1->SetMaximum(max);
	
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void IDAlignMonResiduals::MakePIXBarrelHistograms(MonGroup& al_mon)
{//Pix barrel basic residuals: layers 0,1,2 and the index 3 is for the sum.
  //std::string layerName[4] = {"0","1","2",""};
  //std::string layerTitle[4]= {"Layer 0","Layer 1","Layer 2",""};
  //float layerPhiMod[3]     = {21.5, 37.5, 51.5}; //Phi modules for each barrel layer
  //EtaModuleBegin = -6, EtaModuleEnd = 7
  int totalLayers = m_PIX_Mgr->numerology().numLayers();
  m_NPixLayers = totalLayers; 

  int totalEtaModules = m_gap_pix*(totalLayers-1);  //to allow separation between layers for profiles
  int totalPhiModules = m_gap_pix*(totalLayers-1);  //to allow separation between layers for profiles

  for (int iLayer=0; iLayer < totalLayers;++iLayer) 
    {
      if (!m_PIX_Mgr->numerology().useLayer(iLayer)){
	msg(MSG::WARNING) << "Layer "<<iLayer<<" Not in Use"<<endreq;
	continue;} 
      m_siliconBarrelLayersLabels.push_back("PIXL"+intToString(iLayer));
      float EtaModules= m_PIX_Mgr->numerology().endEtaModuleForLayer(iLayer) - m_PIX_Mgr->numerology().beginEtaModuleForLayer(iLayer); //(i put float in order to divide by 2)
      totalEtaModules+=EtaModules;
      
      float EtaModulesMin = -(EtaModules/2.);
      float EtaModulesMax =  (EtaModules/2.);

      if (iLayer == 0) {
	// IBL has one ring less (from -10 to +9)
	EtaModules--;
	EtaModulesMax--;
      }
 
      int maxPhiModulesPerLayer = m_PIX_Mgr->numerology().numPhiModulesForLayer(iLayer);
      totalPhiModules+=maxPhiModulesPerLayer;
      m_pix_b_residualsx.push_back( new TH1F(("pix_b"+intToString(iLayer)+"_residualx").c_str(),("UnBiased X Residual Pixel Barrel "+intToString(iLayer)).c_str(),100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
      RegisterHisto(al_mon,m_pix_b_residualsx[iLayer]);
      m_pix_b_residualsy.push_back( new TH1F(("pix_b"+intToString(iLayer)+"_residualy").c_str(),("UnBiased Y  Residual Pixel Barrel "+intToString(iLayer)).c_str(),100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
      RegisterHisto(al_mon,m_pix_b_residualsy[iLayer]);     
      m_pix_b_pullsx.push_back(     new TH1F(("pix_b"+intToString(iLayer)+"_pullx").c_str(),("UnBiased X Pull Pixel Barrel "+intToString(iLayer)).c_str(),100*m_FinerBinningFactor,-m_RangeOfPullHistos, m_RangeOfPullHistos));
      RegisterHisto(al_mon,m_pix_b_pullsx[iLayer]);
      m_pix_b_pullsy.push_back(     new TH1F(("pix_b"+intToString(iLayer)+"_pully").c_str(),("UnBiased Y Pull Pixel Barrel "+intToString(iLayer)).c_str(),100*m_FinerBinningFactor,-m_RangeOfPullHistos, m_RangeOfPullHistos));
      RegisterHisto(al_mon,m_pix_b_pullsy[iLayer]);
      m_pix_b_xresvsmodetaphi_3ds.push_back( new TH3F(("pix_b"+intToString(iLayer)+"_xresvsmodetaphi_3d").c_str(),("X Residual Distbn vs Module Eta-Phi-ID Pixel Barrel "+intToString(iLayer)).c_str(),
						      EtaModules, EtaModulesMin, EtaModulesMax, 
						      maxPhiModulesPerLayer, -0.5, maxPhiModulesPerLayer-0.5,
						      50*m_FinerBinningFactor, m_minPIXResXFillRange, m_maxPIXResXFillRange));  //I need a good idea for the x axis 
      RegisterHisto(al_mon,m_pix_b_xresvsmodetaphi_3ds[iLayer]);
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " -- SALVA -- build pix 3d histos -- m_minPIXResXFillRange: " << m_minPIXResXFillRange << "       m_maxPIXResXFillRange: " << m_maxPIXResXFillRange << endreq;
      m_pix_b_yresvsmodetaphi_3ds.push_back( new TH3F(("pix_b"+intToString(iLayer)+"_yresvsmodetaphi_3d").c_str(),("Y Residual Distbn vs Module Eta-Phi-ID Pixel Barrel "+intToString(iLayer)).c_str(),
						      EtaModules, EtaModulesMin, EtaModulesMax, 
						      maxPhiModulesPerLayer,-0.5, maxPhiModulesPerLayer-0.5,
						      50*m_FinerBinningFactor,m_minPIXResYFillRange, m_maxPIXResYFillRange));  //I need a good idea for the x axis 
      RegisterHisto(al_mon,m_pix_b_yresvsmodetaphi_3ds[iLayer]);
      
      
      if(m_do3DOverlapHistos) {
      	m_pix_b_Oxresxvsmodetaphi_3ds.push_back( new TH3F(("pix_b"+intToString(iLayer)+"_Oxresxvsmodetaphi_3d").c_str(),("X_Overlap Residual X Distbn vs Module Eta-Phi-ID Pixel Barrel "+intToString(iLayer)+";ModEta;ModPhi;OResidual [mm]").c_str(),
							  EtaModules*m_mapSplit, EtaModulesMin, EtaModulesMax, 
							  maxPhiModulesPerLayer*m_mapSplit, -0.5, maxPhiModulesPerLayer-0.5,
							  100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	RegisterHisto(al_mon,m_pix_b_Oxresxvsmodetaphi_3ds[iLayer]);
	m_pix_b_Oxresyvsmodetaphi_3ds.push_back( new TH3F(("pix_b"+intToString(iLayer)+"_Oxresyvsmodetaphi_3d").c_str(),("X_Overlap Residual Y Distbn vs Module Eta-Phi-ID Pixel Barrel "+intToString(iLayer)+";ModEta;ModPhi;OResidual [mm]").c_str(),
							  EtaModules*m_mapSplit, EtaModulesMin, EtaModulesMax, 
							  maxPhiModulesPerLayer*m_mapSplit, -0.5, maxPhiModulesPerLayer-0.5,
							  100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
	RegisterHisto(al_mon,m_pix_b_Oxresyvsmodetaphi_3ds[iLayer]);
	m_pix_b_Oyresxvsmodetaphi_3ds.push_back( new TH3F(("pix_b"+intToString(iLayer)+"_Oyresxvsmodetaphi_3d").c_str(),("Y_Overlap Residual X Distbn vs Module Eta-Phi-ID Pixel Barrel "+intToString(iLayer)+";ModEta;ModPhi;OResidual [mm]").c_str(),
							  EtaModules*m_mapSplit, EtaModulesMin, EtaModulesMax, 
							  maxPhiModulesPerLayer*m_mapSplit, -0.5, maxPhiModulesPerLayer-0.5,
							  100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	RegisterHisto(al_mon,m_pix_b_Oyresxvsmodetaphi_3ds[iLayer]);
	m_pix_b_Oyresyvsmodetaphi_3ds.push_back( new TH3F(("pix_b"+intToString(iLayer)+"_Oyresyvsmodetaphi_3d").c_str(),("Y_Overlap Residual Y Distbn vs Module Eta-Phi-ID Pixel Barrel "+intToString(iLayer)+";ModEta;ModPhi;OResidual [mm]").c_str(),
							  EtaModules*m_mapSplit, EtaModulesMin, EtaModulesMax, 
							  maxPhiModulesPerLayer*m_mapSplit, -0.5, maxPhiModulesPerLayer-0.5,
							  100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
	RegisterHisto(al_mon,m_pix_b_Oyresyvsmodetaphi_3ds[iLayer]);
      }
      
      if (m_extendedPlots){
	Double_t momentum[21] = {15,18,21,25,29,34,40,48,59,77,0,77,59,48,40,34,29,25,21,18,15};
	Double_t customaxis[21];
	for (int i=0; i<=20; ++i){
	  if(i<10)			
	    customaxis[i]=-1/(momentum[i]*momentum[i]);
	  else if(i==10)
	    customaxis[i]=0;
	  else
	    customaxis[i]=1/(momentum[i]*momentum[i]);
	}
	
	//Pixel_unbiased residuals vs incident angle theta
	m_pix_b_residualsx_incitheta.push_back( new TH2F(("pix_b"+intToString(iLayer)+"_residualx_incitheta").c_str(),("Unbiased X Residual Vs incident #theta angle Pixel Barrel Layer "+intToString(iLayer)+";Incident #theta angle;PIX Res (mm)").c_str(),20,-m_IncidentThetaRange,m_IncidentThetaRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	RegisterHisto(al_mon,m_pix_b_residualsx_incitheta[iLayer]);
	m_pix_b_residualsy_incitheta.push_back( new TH2F(("pix_b"+intToString(iLayer)+"_residualy_incitheta").c_str(),("Unbiased Y Residual Vs incident #theta angle Pixel Barrel Layer "+intToString(iLayer)+";Incident #theta angle;PIX Res (mm)").c_str(),20,-m_IncidentThetaRange,m_IncidentThetaRange,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
	RegisterHisto(al_mon,m_pix_b_residualsy_incitheta[iLayer]);
	//Pixel_unbiased residuals vs incident angle phi
	m_pix_b_residualsx_inciphi.push_back( new TH2F(("pix_b"+intToString(iLayer)+"_residualx_inciphi").c_str(),("Unbiased X Residual Vs incident #phi angle Pixel Barrel Layer "+intToString(iLayer)+";Incident #phi angle;PIX Res (mm)").c_str(),20,-m_IncidentPhiRange,m_IncidentPhiRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	RegisterHisto(al_mon,m_pix_b_residualsx_inciphi[iLayer]);
	m_pix_b_residualsy_inciphi.push_back( new TH2F(("pix_b"+intToString(iLayer)+"_residualy_inciphi").c_str(),("Unbiased Y Residual Vs incident #phi angle Pixel Barrel Layer "+intToString(iLayer)+";Incident #phi angle;PIX Res (mm)").c_str(),20,-m_IncidentPhiRange,m_IncidentPhiRange,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
	RegisterHisto(al_mon,m_pix_b_residualsy_inciphi[iLayer]);
	//Pixel unbiased residuals vs pt
	m_pix_b_residualsx_pt.push_back( new TH2F(("pix_b"+intToString(iLayer)+"_residualx_pt").c_str(),("Unbiased X Residual Vs Pt Pixel Barrel Layer "+intToString(iLayer)+";Track pT (GeV);PIX #eta Res (mm)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	RegisterHisto(al_mon,m_pix_b_residualsx_pt[iLayer]);
	m_pix_b_residualsy_pt.push_back( new TH2F(("pix_b"+intToString(iLayer)+"_residualy_pt").c_str(),("Unbiased Y Residual Vs Pt Pixel Barrel Layer "+intToString(iLayer)+";Track pT (GeV);PIX #eta Res (mm)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
	RegisterHisto(al_mon,m_pix_b_residualsy_pt[iLayer]);
	//Pixel unbiased residuals vs qoverp2
	m_pix_b_residualsx_qoverp2.push_back( new TH2F(("pix_b"+intToString(iLayer)+"_unbiased_residualx_qoverp2").c_str(),("Unbiased X Residual Vs Q/P^{2} Pixel Barrel Layer "+intToString(iLayer)+";Track Q/P^{2} (GeV^{-2});PIX Res (mm)").c_str(),20,customaxis,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	RegisterHisto(al_mon,m_pix_b_residualsx_qoverp2[iLayer]);
	m_pix_b_residualsy_qoverp2.push_back( new TH2F(("pix_b"+intToString(iLayer)+"_unbiased_residualy_qoverp2").c_str(),("Unbiased Y Residual Vs Q/P^{2} Pixel Barrel Layer "+intToString(iLayer)+";Track Q/P^{2} (GeV^{-2});PIX Res (mm)").c_str(),20,customaxis,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
	RegisterHisto(al_mon,m_pix_b_residualsy_qoverp2[iLayer]);	
	// Pixel biased residuals
	m_pix_b_biased_residualsx.push_back( new TH1F(("pix_b"+intToString(iLayer)+"_biased_residualx").c_str(),("Biased X Residual Pixel Barrel "+intToString(iLayer)+";Residual [mm]").c_str(),100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	RegisterHisto(al_mon,m_pix_b_biased_residualsx[iLayer]);
	m_pix_b_biased_residualsy.push_back( new TH1F(("pix_b"+intToString(iLayer)+"_biased_residualy").c_str(),("Biased Y Residual Pixel Barrel "+intToString(iLayer)+";Residual [mm]").c_str(),100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
	RegisterHisto(al_mon,m_pix_b_biased_residualsy[iLayer]);
	//Biased residuals vs pt
	m_pix_b_biased_residualsx_pt.push_back( new TH2F(("pix_b"+intToString(iLayer)+"_biased_residualx_pt").c_str(),("Biased X Residual Vs Pt Pixel Barrel Layer "+intToString(iLayer)+";Track pT (GeV);PIX Res (mm)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	RegisterHisto(al_mon,m_pix_b_biased_residualsx_pt[iLayer]);
	m_pix_b_biased_residualsy_pt.push_back( new TH2F(("pix_b"+intToString(iLayer)+"_biased_residualy_pt").c_str(),("Biased Y Residual Vs Pt Pixel Barrel Layer "+intToString(iLayer)+";Track pT (GeV);PIX Res (mm)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
	RegisterHisto(al_mon,m_pix_b_biased_residualsy_pt[iLayer]);
	//Pixel biased residuals vs qoverp2
	m_pix_b_biased_residualsx_qoverp2.push_back( new TH2F(("pix_b"+intToString(iLayer)+"_biased_residualx_qoverp2").c_str(),("Biased X Residual Vs Q/P^{2} Pixel Barrel Layer "+intToString(iLayer)+";Track Q/P^{2} (GeV^{-2});PIX Res (mm)").c_str(),20,customaxis,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	RegisterHisto(al_mon,m_pix_b_biased_residualsx_qoverp2[iLayer]);
	m_pix_b_biased_residualsy_qoverp2.push_back( new TH2F(("pix_b"+intToString(iLayer)+"_biased_residualy_qoverp2").c_str(),("Biased Y Residual Vs Q/P^{2} Pixel Barrel Layer "+intToString(iLayer)+";Track Q/P^{2} (GeV^{-2});PIX Res (mm)").c_str(),20,customaxis,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
	RegisterHisto(al_mon,m_pix_b_biased_residualsy_qoverp2[iLayer]);
	//Pulls vs pt
	m_pix_b_pullsx_pt.push_back( new TH2F(("pix_b"+intToString(iLayer)+"_pullx_pt").c_str(),("X Pull Vs Pt Pixel Barrel Layer"+intToString(iLayer)+";Track pT (GeV);PIX #phi Pull").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos));
	RegisterHisto(al_mon,m_pix_b_pullsx_pt[iLayer]);
	m_pix_b_pullsy_pt.push_back( new TH2F(("pix_b"+intToString(iLayer)+"_pully_pt").c_str(),("Y Pull Vs Pt Pixel Barrel Layer"+intToString(iLayer)+";Track pT (GeV);PIX #phi Pull").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos));
	RegisterHisto(al_mon,m_pix_b_pullsy_pt[iLayer]);
	//overlap residuals for pixel
	m_pix_b_xoverlapresidualsx.push_back( new TH1F(("pix_b"+intToString(iLayer)+"_xoverlapresidualx").c_str(),("X_Overlap Residual X Pixel Barrel Layer "+intToString(iLayer)).c_str(),100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	RegisterHisto(al_mon,m_pix_b_xoverlapresidualsx[iLayer]);
	m_pix_b_xoverlapresidualsy.push_back( new TH1F(("pix_b"+intToString(iLayer)+"_xoverlapresidualy").c_str(),("X_Overlap Residual Y Pixel Barrel Layer "+intToString(iLayer)).c_str(),100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
	RegisterHisto(al_mon,m_pix_b_xoverlapresidualsy[iLayer]);
	m_pix_b_yoverlapresidualsx.push_back( new TH1F(("pix_b"+intToString(iLayer)+"_yoverlapresidualx").c_str(),("Y_Overlap Residual X Pixel Barrel Layer "+intToString(iLayer)).c_str(),100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	RegisterHisto(al_mon,m_pix_b_yoverlapresidualsx[iLayer]);
	m_pix_b_yoverlapresidualsy.push_back( new TH1F(("pix_b"+intToString(iLayer)+"_yoverlapresidualy").c_str(),("Y_Overlap Residual Y Pixel Barrel Layer "+intToString(iLayer)).c_str(),100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
	RegisterHisto(al_mon,m_pix_b_yoverlapresidualsy[iLayer]);
	//3d biased residuals modetamodphi
	//m_pix_b_biased_xresvsmodetaphi_3ds[iLayer] = new TH3F(("pix_b"+intToString(iLayer)+"_biased_xresvsmodetaphi_3d").c_str(),("X Biased Residual Distbn vs Module Eta-Phi-ID Pixel Barrel "+intToString(iLayer)+";ModEta;ModPhi").c_str(),
	//EtaModules,-(EtaModules/2.),(EtaModules/2.),maxPhiModulesPerLayer,0,maxPhiModulesPerLayer,100,m_minPIXResXFillRange,m_maxPIXResXFillRange);  //I need a good idea for the x axis 

	//	m_pix_b_biased_xresvsmodetaphi_3ds.push_back( new TH3F(("pix_b"+intToString(iLayer)+"_biased_xresvsmodetaphi_3d").c_str(),
	//						       ("X Biased Residual Distbn vs Module Eta-Phi-ID Pixel Barrel "+intToString(iLayer)+";ModEta;ModPhi").c_str(),
	//						       EtaModules*m_mapSplit, -PixelBarrelYSize*EtaModules/2, PixelBarrelYSize*EtaModules/2,
	//						       maxPhiModulesPerLayer*m_mapSplit,0,maxPhiModulesPerLayer*PixelBarrelXSize,
	//						       100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange)); 
	m_pix_b_detailed_xresvsmodetaphi_3ds.push_back( new TH3F(("pix_b"+intToString(iLayer)+"_detailed_xresvsmodetaphi_3d").c_str(),
							       ("Detailed X Residual Distbn vs Module Eta-Phi-ID Pixel Barrel "+intToString(iLayer)+";ModEta;ModPhi").c_str(),
							       EtaModules*m_mapSplit, EtaModulesMin, EtaModulesMax,
							       maxPhiModulesPerLayer*m_mapSplit, -0.5, maxPhiModulesPerLayer-0.5,
							       100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange)); 
	RegisterHisto(al_mon,m_pix_b_detailed_xresvsmodetaphi_3ds[iLayer]);

	//m_pix_b_biased_yresvsmodetaphi_3ds[iLayer] = new TH3F(("pix_b"+intToString(iLayer)+"_biased_yresvsmodetaphi_3d").c_str(),("Y Biased Residual Distbn vs Module Eta-Phi-ID Pixel Barrel "+intToString(iLayer)+";ModEta;ModPhi").c_str(),
	//EtaModules,-(EtaModules/2.),(EtaModules/2.),maxPhiModulesPerLayer,0,maxPhiModulesPerLayer,100,m_minPIXResXFillRange,m_maxPIXResXFillRange);  //I need a good idea for the x axis 
	//m_pix_b_biased_yresvsmodetaphi_3ds.push_back( new TH3F(("pix_b"+intToString(iLayer)+"_biased_yresvsmodetaphi_3d").c_str(),
	//						       ("Y Biased Residual Distbn vs Module Eta-Phi-ID Pixel Barrel "+intToString(iLayer)+";ModEta;ModPhi").c_str(),
	//						       EtaModules*m_mapSplit, -PixelBarrelYSize*EtaModules/2, PixelBarrelYSize*EtaModules/2,
	//						       maxPhiModulesPerLayer*m_mapSplit,0,maxPhiModulesPerLayer*PixelBarrelXSize,
	//						       100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange)); 
	m_pix_b_detailed_yresvsmodetaphi_3ds.push_back( new TH3F(("pix_b"+intToString(iLayer)+"_detailed_yresvsmodetaphi_3d").c_str(),
							       ("Detailed Y residual Distbn vs Module Eta-Phi-ID Pixel Barrel "+intToString(iLayer)+";ModEta;ModPhi").c_str(),
							       EtaModules*m_mapSplit, EtaModulesMin, EtaModulesMax,
							       maxPhiModulesPerLayer*m_mapSplit, -0.5, maxPhiModulesPerLayer-0.5,
							       100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange)); 
	RegisterHisto(al_mon,m_pix_b_detailed_yresvsmodetaphi_3ds[iLayer]);
	
	//Half Shell
	m_pix_b_top_residualsx.push_back(  new TH1F (("pix_b"+intToString(iLayer)+"_top_residualx").c_str(),("UnBiased X Residual Pixel Barrel Layer "+intToString(iLayer)+" (Top Half Shell)").c_str(),100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	RegisterHisto(al_mon,m_pix_b_top_residualsx[iLayer]);
	m_pix_b_top_residualsy.push_back(  new TH1F (("pix_b"+intToString(iLayer)+"_top_residualy").c_str(),("UnBiased Y Residual Pixel Barrel Layer "+intToString(iLayer)+" (Top Half Shell)").c_str(),100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
	RegisterHisto(al_mon,m_pix_b_top_residualsy[iLayer]);
	m_pix_b_btm_residualsx.push_back(  new TH1F (("pix_b"+intToString(iLayer)+"_btm_residualx").c_str(),("UnBiased X Residual Pixel Barrel Layer "+intToString(iLayer)+" (Btm Half Shell)").c_str(),100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	RegisterHisto(al_mon,m_pix_b_btm_residualsx[iLayer]);
	m_pix_b_btm_residualsy.push_back(  new TH1F (("pix_b"+intToString(iLayer)+"_btm_residualy").c_str(),("UnBiased Y Residual Pixel Barrel Layer "+intToString(iLayer)+" (Btm Half Shell)").c_str(),100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
	RegisterHisto(al_mon,m_pix_b_btm_residualsy[iLayer]);
	//Half Shell Biased
	m_pix_b_top_biased_residualsx.push_back(  new TH1F (("pix_b"+intToString(iLayer)+"_top_biased_residualx").c_str(),("Biased X Residual Pixel Barrel Layer "+intToString(iLayer)+" (Top Half Shell)").c_str(),100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	RegisterHisto(al_mon,m_pix_b_top_biased_residualsx[iLayer]);
	m_pix_b_top_biased_residualsy.push_back(  new TH1F (("pix_b"+intToString(iLayer)+"_top_biased_residualy").c_str(),("Biased Y Residual Pixel Barrel Layer "+intToString(iLayer)+" (Top Half Shell)").c_str(),100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
	RegisterHisto(al_mon,m_pix_b_top_biased_residualsy[iLayer]);
	m_pix_b_btm_biased_residualsx.push_back(  new TH1F (("pix_b"+intToString(iLayer)+"_btm_biased_residualx").c_str(),("Biased X Residual Pixel Barrel Layer "+intToString(iLayer)+" (Btm Half Shell)").c_str(),100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	RegisterHisto(al_mon,m_pix_b_btm_biased_residualsx[iLayer]);
	m_pix_b_btm_biased_residualsy.push_back(  new TH1F (("pix_b"+intToString(iLayer)+"_btm_biased_residualy").c_str(),("Biased Y Residual Pixel Barrel Layer "+intToString(iLayer)+" (Btm Half Shell)").c_str(),100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
	RegisterHisto(al_mon,m_pix_b_btm_biased_residualsy[iLayer]);
	//finely binned histograms with wide range for calculation of resolution
	

	if (m_doClusterSizeHistos)
	  {
	    m_pix_b_clustersize.push_back(new TH1F(("pix_b"+intToString(iLayer)+"_clustersize").c_str(),("Cluster size Pixel Barrel Layer "+intToString(iLayer)).c_str(),m_ClusterSizeRange+1,-0.5,m_ClusterSizeRange+0.5));
	    RegisterHisto(al_mon,m_pix_b_clustersize[iLayer]);
	    m_pix_b_clustersizePhi.push_back(new TH1F(("pix_b"+intToString(iLayer)+"_clustersizePhi").c_str(),("Cluster Phi size Pixel Barrel Layer "+intToString(iLayer)).c_str(),m_ClusterSizeRange+1,-0.5,m_ClusterSizeRange+0.5));
	    RegisterHisto(al_mon,m_pix_b_clustersizePhi[iLayer]);
	    m_pix_b_clustersizeZ.push_back(new TH1F(("pix_b"+intToString(iLayer)+"_clustersizeZ").c_str(),("Cluster Z size Pixel Barrel Layer "+intToString(iLayer)).c_str(),m_ClusterSizeRange+1,-0.5,m_ClusterSizeRange+0.5));
	    RegisterHisto(al_mon,m_pix_b_clustersizeZ[iLayer]);
	    
	    m_pix_b_residualsx_clustersize.push_back( new TH2F(("pix_b"+intToString(iLayer)+"_residualx_clustersize").c_str(),("Unbiased X Residual Vs Cluster Size Pixel Barrel Layer "+intToString(iLayer)+";Cluster Size;PIX Res (mm)").c_str(),
							       m_ClusterSizeRange+1, -0.5, m_ClusterSizeRange+0.5,
							       100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	    RegisterHisto(al_mon,m_pix_b_residualsx_clustersize[iLayer]);
	    m_pix_b_residualsy_clustersize.push_back( new TH2F(("pix_b"+intToString(iLayer)+"_residualy_clustersize").c_str(),("Unbiased Y Residual Vs Cluster Size Pixel Barrel Layer "+intToString(iLayer)+";Cluster Size;PIX Res (mm)").c_str(),
							       m_ClusterSizeRange+1, -0.5, m_ClusterSizeRange+0.5,
							       100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	    RegisterHisto(al_mon,m_pix_b_residualsy_clustersize[iLayer]);

	    m_pix_b_residualsx_clustersizeP.push_back( new TProfile(("pix_b"+intToString(iLayer)+"_residualx_clustersize_p").c_str(),("Unbiased X Residual Vs Cluster Size Pixel Barrel Layer "+intToString(iLayer)+";Cluster Size;PIX Res (mm)").c_str(),
								    m_ClusterSizeRange+1,-0.5, m_ClusterSizeRange+0.5
								    ,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	    RegisterHisto(al_mon,m_pix_b_residualsx_clustersizeP[iLayer]);
	    m_pix_b_residualsy_clustersizeP.push_back( new TProfile(("pix_b"+intToString(iLayer)+"_residualy_clustersize_p").c_str(),("Unbiased Y Residual Vs Cluster Size Pixel Barrel Layer "+intToString(iLayer)+";Cluster Size;PIX Res (mm)").c_str(),
								    m_ClusterSizeRange+1, -0.5, m_ClusterSizeRange+0.5
								    ,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	    RegisterHisto(al_mon,m_pix_b_residualsy_clustersizeP[iLayer]);
	    
	    m_pix_b_residualsx_clustersizePhi.push_back( new TH2F(("pix_b"+intToString(iLayer)+"_residualx_clustersizePhi").c_str(),("Unbiased X Residual Vs Cluster Phi Size Pixel Barrel Layer "+intToString(iLayer)+";Cluster Phi Size;PIX Res (mm)").c_str(),
								  m_ClusterSizeRange+1, -0.5, m_ClusterSizeRange+0.5
								  ,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	    RegisterHisto(al_mon,m_pix_b_residualsx_clustersizePhi[iLayer]);
	    m_pix_b_residualsy_clustersizePhi.push_back( new TH2F(("pix_b"+intToString(iLayer)+"_residualy_clustersizePhi").c_str(),("Unbiased Y Residual Vs Cluster Phi Size Pixel Barrel Layer "+intToString(iLayer)+";Cluster Phi Size;PIX Res (mm)").c_str(),
								  m_ClusterSizeRange+1, -0.5, m_ClusterSizeRange+0.5,
								  100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	    RegisterHisto(al_mon,m_pix_b_residualsy_clustersizePhi[iLayer]);

	    m_pix_b_residualsx_clustersizePhiP.push_back( new TProfile(("pix_b"+intToString(iLayer)+"_residualx_clustersizePhi_p").c_str(),("Unbiased X Residual Vs Cluster Phi Size Pixel Barrel Layer "+intToString(iLayer)+";Cluster Phi Size;PIX Res (mm)").c_str(),
								       m_ClusterSizeRange+1, -0.5, m_ClusterSizeRange+0.5,
								       m_minPIXResXFillRange,m_maxPIXResXFillRange));
	    RegisterHisto(al_mon,m_pix_b_residualsx_clustersizePhiP[iLayer]);
	    m_pix_b_residualsy_clustersizePhiP.push_back( new TProfile(("pix_b"+intToString(iLayer)+"_residualy_clustersizePhi_p").c_str(),("Unbiased Y Residual Vs Cluster Phi Size Pixel Barrel Layer "+intToString(iLayer)+";Cluster Phi Size;PIX Res (mm)").c_str(),
								       m_ClusterSizeRange+1, -0.5, m_ClusterSizeRange+0.5,
								       m_minPIXResXFillRange,m_maxPIXResXFillRange));
	    RegisterHisto(al_mon,m_pix_b_residualsy_clustersizePhiP[iLayer]);
	    

	    m_pix_b_residualsx_clustersizeZ.push_back( new TH2F(("pix_b"+intToString(iLayer)+"_residualx_clustersizeZ").c_str(),("Unbiased X Residual Vs Cluster Z Size Pixel Barrel Layer "+intToString(iLayer)+";Cluster Z Size;PIX Res (mm)").c_str(),
								m_ClusterSizeRange+1, -0.5, m_ClusterSizeRange+0.5,
								100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	    RegisterHisto(al_mon,m_pix_b_residualsx_clustersizeZ[iLayer]);
	    m_pix_b_residualsy_clustersizeZ.push_back( new TH2F(("pix_b"+intToString(iLayer)+"_residualy_clustersizeZ").c_str(),("Unbiased Y Residual Vs Cluster Z Size Pixel Barrel Layer "+intToString(iLayer)+";Cluster Z Size;PIX Res (mm)").c_str(),
								m_ClusterSizeRange+1, -0.5, m_ClusterSizeRange+0.5,
								100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
	    RegisterHisto(al_mon,m_pix_b_residualsy_clustersizeZ[iLayer]);

	    m_pix_b_residualsx_clustersizeZP.push_back( new TProfile(("pix_b"+intToString(iLayer)+"_residualx_clustersizeZ_p").c_str(),("Unbiased X Residual Vs Cluster Z Size Pixel Barrel Layer "+intToString(iLayer)+";Cluster Z Size;PIX Res (mm)").c_str(),
								     m_ClusterSizeRange+1, -0.5, m_ClusterSizeRange+0.5,
								     m_minPIXResXFillRange,m_maxPIXResXFillRange));
	    RegisterHisto(al_mon,m_pix_b_residualsx_clustersizeZP[iLayer]);
	    m_pix_b_residualsy_clustersizeZP.push_back( new TProfile(("pix_b"+intToString(iLayer)+"_residualy_clustersizeZ_p").c_str(),("Unbiased Y Residual Vs Cluster Z Size Pixel Barrel Layer "+intToString(iLayer)+";Cluster Z Size;PIX Res (mm)").c_str(),
								     m_ClusterSizeRange+1, -0.5, m_ClusterSizeRange+0.5,
								     m_minPIXResXFillRange,m_maxPIXResXFillRange));
	    RegisterHisto(al_mon,m_pix_b_residualsy_clustersizeZP[iLayer]);
	    
	    m_pix_b_clustersize_incidentAngle.push_back(new TProfile(("pix_b"+intToString(iLayer)+"_clustersize_incidentAngle").c_str(),("Cluster Size Pixel vs Incident Theta Angle Barrel Layer "+intToString(iLayer)+"Incident Theta Angle (rad);Cluster Size").c_str(),
								     20,-m_IncidentThetaRange,m_IncidentThetaRange, -0.5, m_ClusterSizeRange+0.5));
	    RegisterHisto(al_mon,m_pix_b_clustersize_incidentAngle[iLayer]);
	    m_pix_b_clustersizePhi_incidentAngle.push_back(new TProfile(("pix_b"+intToString(iLayer)+"_clustersizePhi_incidentAngle").c_str(),("Cluster Phi Size vs Incident Theta Angle Pixel Barrel Layer "+intToString(iLayer)+"Incident Theta Angle (rad);Cluster Phi Size").c_str(),
									20,-m_IncidentThetaRange,m_IncidentThetaRange, -0.5, m_ClusterSizeRange+0.5));
	    RegisterHisto(al_mon,m_pix_b_clustersizePhi_incidentAngle[iLayer]);
	    m_pix_b_clustersizeZ_incidentAngle.push_back(new TProfile(("pix_b"+intToString(iLayer)+"_clustersizeZ_incidentAngle").c_str(),("Cluster Size vs Incident Theta Angle Pixel Barrel Layer "+intToString(iLayer)+"Incident Theta Angle (rad);Cluster Z Size").c_str(),
								      20,-m_IncidentThetaRange,m_IncidentThetaRange, -0.5, m_ClusterSizeRange+0.5));
	    RegisterHisto(al_mon,m_pix_b_clustersizeZ_incidentAngle[iLayer]);

	    m_pix_b_clustersize_incidentAnglePhi.push_back(new TProfile(("pix_b"+intToString(iLayer)+"_clustersize_incidentAnglePhi").c_str(),("Cluster Size Pixel vs Incident Phi Angle Barrel Layer "+intToString(iLayer)+"Incident Phi Angle (rad);Cluster Size").c_str(),
									20,-m_IncidentPhiRange,m_IncidentPhiRange, -0.5, m_ClusterSizeRange+0.5));
	    RegisterHisto(al_mon,m_pix_b_clustersize_incidentAnglePhi[iLayer]);
	    m_pix_b_clustersizePhi_incidentAnglePhi.push_back(new TProfile(("pix_b"+intToString(iLayer)+"_clustersizePhi_incidentAnglePhi").c_str(),("Cluster Phi Size vs Incident Phi Angle Pixel Barrel Layer "+intToString(iLayer)+"Incident Phi Angle (rad);Cluster Phi Size").c_str(),
									   20,-m_IncidentPhiRange,m_IncidentPhiRange, -0.5, m_ClusterSizeRange+0.5));
	    RegisterHisto(al_mon,m_pix_b_clustersizePhi_incidentAnglePhi[iLayer]);
	    m_pix_b_clustersizeZ_incidentAnglePhi.push_back(new TProfile(("pix_b"+intToString(iLayer)+"_clustersizeZ_incidentAnglePhi").c_str(),("Cluster Size vs Incident Phi Angle Pixel Barrel Layer "+intToString(iLayer)+"Incident Phi Angle (rad);Cluster Z Size").c_str(),
									 20,-m_IncidentPhiRange,m_IncidentPhiRange, -0.5, m_ClusterSizeRange+0.5));
	    RegisterHisto(al_mon,m_pix_b_clustersizeZ_incidentAnglePhi[iLayer]);
	    
	    
	    	    

	  }//doClusterSizeHistos
	
	



      }//Extended Plots
      
    } //Layer loop
  
  //basic residual histograms
  m_pix_b_residualx = new TH1F("pix_b_residualx","UnBiased X Residual Pixel Barrel;Residual [mm]",100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
  RegisterHisto(al_mon,m_pix_b_residualx);  
  m_pix_b_residualy = new TH1F("pix_b_residualy","UnBiased Y Residual Pixel Barrel;Residual [mm]",100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange);
  RegisterHisto(al_mon,m_pix_b_residualy);  
  m_pix_b_biased_residualx = new TH1F("pix_b_biasedresidualx","Biased X Residual Pixel Barrel;Residual [mm]",100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
  RegisterHisto(al_mon,m_pix_b_biased_residualx);  
  m_pix_b_biased_residualy = new TH1F("pix_b_biasedresidualy","Biased Y Residual Pixel Barrel;Residual [mm]",100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange);
  RegisterHisto(al_mon,m_pix_b_biased_residualy); 
  // Oxx,Oxy,Oyy,Oyx vs module eta/phi
  m_pix_b_Oxresxvsmodeta = new TProfile("pix_b_Oxresxvsmodeta","X_Overlap Residual X Mean vs (Modified) Module Eta Pixel Barrel",totalEtaModules,0,totalEtaModules,m_minPIXResXFillRange,m_maxPIXResXFillRange);
  RegisterHisto(al_mon,m_pix_b_Oxresxvsmodeta);  
  m_pix_b_Oxresxvsmodphi = new TProfile("pix_b_Oxresxvsmodphi","X_Overlap Residual X Mean vs (Modified) Module Phi Pixel Barrel",totalPhiModules,0,totalPhiModules,m_minPIXResXFillRange,m_maxPIXResXFillRange);
  RegisterHisto(al_mon,m_pix_b_Oxresxvsmodphi);  
  m_pix_b_Oyresyvsmodeta = new TProfile("pix_b_Oyresyvsmodeta","Y_Overlap Residual Y Mean vs (Modified) Module Eta Pixel Barrel",totalEtaModules,0,totalEtaModules,m_minPIXResYFillRange,m_maxPIXResYFillRange);
  RegisterHisto(al_mon,m_pix_b_Oyresyvsmodeta);  
  m_pix_b_Oyresyvsmodphi = new TProfile("pix_b_Oyresyvsmodphi","Y_Overlap Residual Y Mean vs (Modified) Module Phi Pixel Barrel",totalPhiModules,0,totalPhiModules,m_minPIXResYFillRange,m_maxPIXResYFillRange);
  RegisterHisto(al_mon,m_pix_b_Oyresyvsmodphi);
  //These histograms are filled in post-processing - only initiated here such that they can be registered as "shift".    
  //PIXEL histograms for monitoring residual dependence on module eta/phi (stave/ring)
  m_pix_b_xresvsmodeta = new TH1F("pix_b_xresvsmodeta","X Residual Mean vs (Modified) Module Eta Pixel Barrel;(Modified) Module Eta Identifier;Mean Residual X",
				  totalEtaModules+2, -1.5, totalEtaModules+1.5);
  RegisterHisto(al_mon,m_pix_b_xresvsmodeta);  
  m_pix_b_xresvsmodphi = new TH1F("pix_b_xresvsmodphi","X Residual Mean vs (Modified) Module Phi Pixel Barrel;(Modified) Module Phi Identifier;Mean Residual X",
				  totalPhiModules+2, -1.5, totalPhiModules+1.5);
  RegisterHisto(al_mon,m_pix_b_xresvsmodphi);  
  m_pix_b_yresvsmodeta = new TH1F("pix_b_yresvsmodeta","Y Residual Mean vs (Modified) Module Eta Pixel Barrel;(Modified) Module Eta Identifier;Mean Residual Y",
				  totalEtaModules+2, -1.5, totalEtaModules+1.5);
  RegisterHisto(al_mon,m_pix_b_yresvsmodeta);  
  m_pix_b_yresvsmodphi = new TH1F("pix_b_yresvsmodphi","Y Residual Mean vs (Modified) Module Phi Pixel Barrel;(Modified) Module Phi Identifier;Mean Residual Y",
				  totalPhiModules+2, -1.5, totalPhiModules+1.5);
  RegisterHisto(al_mon,m_pix_b_yresvsmodphi); 

  // - x axis should label correct eta ring
  //int m_layerModEtaShift[4] = {10,30,48,65};       //HARDCODED!
  //int m_layerModPhiShift[4] = {0,18,44,86};

  int phibinid = 0;
  for (int ibin=1; ibin <= m_pix_b_xresvsmodphi->GetNbinsX(); ibin++) {
    // - SALVA - 
    // there is a free bin at the beginning and the end
    // IBL has 14 staves: starts at bin 2 
    if (2 <= ibin && ibin <= 15) {
      phibinid++;
      if (ibin == 2) phibinid = 1; // IBL stave count starts at 1
      m_pix_b_xresvsmodphi->GetXaxis()->SetBinLabel(ibin,("IBL_"+intToString(phibinid)).c_str());
      m_pix_b_yresvsmodphi->GetXaxis()->SetBinLabel(ibin,("IBL_"+intToString(phibinid)).c_str());
    }
    // B layer has 22 staves: starts at bin 24 
    if (24 <= ibin && ibin <= 45) {
      phibinid++;
      if (ibin == 24) phibinid = 0;
      m_pix_b_xresvsmodphi->GetXaxis()->SetBinLabel(ibin,("BLay_"+intToString(phibinid)).c_str());
      m_pix_b_yresvsmodphi->GetXaxis()->SetBinLabel(ibin,("BLay_"+intToString(phibinid)).c_str());
    }
  } 

  //
  
  if (m_extendedPlots){
    
    //finely binned histograms with wide range for calculation of resolution
    m_pix_b_residualx_fine = new TH1F("pix_b_residualx_fine","UnBiased X Residual Pixel Barrel;Residual [mm]",2000,-2.0,2.0);
    RegisterHisto(al_mon,m_pix_b_residualx_fine); 
    m_pix_b_residualy_fine = new TH1F("pix_b_residualy_fine","UnBiased Y Residual Pixel Barrel;Residual [mm]",2000,-2.0,2.0);
    RegisterHisto(al_mon,m_pix_b_residualy_fine);  
    
    // Pixel biased residuals vs Pt
    m_pix_b_biased_residualx_pt = new TH2F("pix_b_biased_residualx_pt","Biased X Residual Vs Pt Pixel Barrel;Track pT (GeV); Res (mm)",m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange); 
    RegisterHisto(al_mon,m_pix_b_biased_residualx_pt);			
    m_pix_b_biased_residualy_pt = new TH2F("pix_b_biased_residualy_pt","Biased Y Residual Vs Pt Pixel Barrel;Track pT (GeV); Res (mm)",m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange); 
    RegisterHisto(al_mon,m_pix_b_biased_residualy_pt);						  			
    
    // Pixel unbiased residuals vs Pt
    m_pix_b_residualx_pt = new TH2F("pix_b_residualx_pt","X Residual Vs Pt Pixel Barrel;Track pT (GeV);Res (mm)",m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minSiResFillRange,m_maxSiResFillRange);
    RegisterHisto(al_mon,m_pix_b_residualx_pt);			
    m_pix_b_residualy_pt = new TH2F("pix_b_residualy_pt","Y Residual Vs Pt Pixel Barrel;Track pT (GeV);Res (mm)",m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minSiResFillRange,m_maxSiResFillRange);
    RegisterHisto(al_mon,m_pix_b_residualy_pt);			
    // Pixel local positions
    m_pix_b_extrapolated_localx = new TH1F("pix_b_extrapolated_localx","Pixel Barrel Extrapolated Local X;localX (mm)",500,-(PixelBarrelXSize/2)*1.1,(PixelBarrelXSize/2)*1.1);
    RegisterHisto(al_mon,m_pix_b_extrapolated_localx); 			
    m_pix_b_extrapolated_localy = new TH1F("pix_b_extrapolated_localy","Pixel Barrel Extrapolated Local Y;localY (mm)",500,-(PixelBarrelYSize/2)*1.1,(PixelBarrelYSize/2)*1.1);
    RegisterHisto(al_mon,m_pix_b_extrapolated_localy);
    m_pix_b_measured_localx = new TH1F("pix_b_measured_localx","Pixel Barrel Measured Local X;localX (mm)",500,-(PixelBarrelXSize/2)*1.1,(PixelBarrelXSize/2)*1.1);
    RegisterHisto(al_mon,m_pix_b_measured_localx); 
    m_pix_b_measured_localy = new TH1F("pix_b_measured_localy","Pixel Barrel Measured Local Y;local Y(mm)",500,-(PixelBarrelYSize/2)*1.1,(PixelBarrelYSize/2)*1.1);
    RegisterHisto(al_mon,m_pix_b_measured_localy);
    
    // Pulls vs pt
    m_pix_b_pullx_pt = new TH2F("pix_b_pullx_pt","X Pull Vs Pt Pixel Barrel;Track pT (GeV)",m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos);
    RegisterHisto(al_mon,m_pix_b_pullx_pt);  
    m_pix_b_pully_pt = new TH2F("pix_b_pully_pt","Y Pull Vs Pt Pixel Barrel;Track pT (GeV)",m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos);
    RegisterHisto(al_mon,m_pix_b_pully_pt);
  }
  
}


void IDAlignMonResiduals::MakePIXEndCapsHistograms(MonGroup& al_mon){
  //Pix endcaps residuals. 
  //std::string diskName[4] = {"d0_","d1_","d2_",""};
  //std::string diskTitle[4]= {"Disk 0","Disk 1","Disk 2",""};
  int totalPhiModules = 0;
  int NDisks = m_PIX_Mgr->numerology().numDisks();
  int maxPhiModulesPerRing=0;
  // Pixel biased residuals vs QoverP^2
  // Variable binning for res vs qoverp2 plots
  Double_t momentum[21] = {15,18,21,25,29,34,40,48,59,77,0,77,59,48,40,34,29,25,21,18,15};  //this variable has to be created just once. I don't like this replication.
  Double_t customaxis[21];
  for (int i=0; i<=20; i++){
    if(i<10)			
      customaxis[i]=-1/(momentum[i]*momentum[i]);
    else if(i==10)
      customaxis[i]=0;
    else
      customaxis[i]=1/(momentum[i]*momentum[i]);
  } 
  for (int iECIndex = 0; iECIndex < m_PIX_Mgr->numerology().numEndcaps(); ++iECIndex)
    {
      totalPhiModules = m_gap_pix * (NDisks - 1); //I reset to avoid double counting for the Endcaps
      int iSide = m_PIX_Mgr->numerology().endcapId(iECIndex);
      for (int iWheel=0; iWheel < NDisks;++iWheel)
	{
	  int rings=0;
	  if (!m_PIX_Mgr->numerology().useDisk(iWheel)){//To check if the Wheel is in use.
	    msg(MSG::WARNING) << "Wheel "<<iWheel<<" Not in Use"<<endreq;
	    continue;}
	  else
	    {
	      rings=m_PIX_Mgr->numerology().numRingsForDisk(iWheel);
	      if (iSide>0)
		m_siliconEndcapLayersLabels.push_back("PIXL"+intToString(iWheel));
	    }
	  maxPhiModulesPerRing=0;
	  for (int iEta = 0; iEta < rings; ++iEta)  //actually number of rings for PIX = 1
	    {
	      if (maxPhiModulesPerRing < m_PIX_Mgr->numerology().numPhiModulesForDiskRing(iWheel,iEta)) //maybe a !=
		maxPhiModulesPerRing = m_PIX_Mgr->numerology().numPhiModulesForDiskRing(iWheel,iEta);
	      totalPhiModules+=maxPhiModulesPerRing;
	    }	  
	  if (iSide>0)
	    {
	      m_pix_eca_unbiased_xresvsmodphi_disks.push_back(new TProfile(("pix_eca_unbiased_xresvsmodphi_disk"+intToString(iWheel)).c_str(),("UNBIASED X Residual Average vs Module Phi of Pixel Endcap A Disk "+intToString(iWheel)).c_str(),maxPhiModulesPerRing,-0.5,maxPhiModulesPerRing-0.5));
	      RegisterHisto(al_mon,m_pix_eca_unbiased_xresvsmodphi_disks[iWheel]);
	      m_pix_eca_unbiased_yresvsmodphi_disks.push_back(new TProfile(("pix_eca_unbiased_yresvsmodphi_disk"+intToString(iWheel)).c_str(),("UNBIASED Y Residual Average vs Module Phi of Pixel Endcap A Disk "+intToString(iWheel)).c_str(),maxPhiModulesPerRing,-0.5,maxPhiModulesPerRing-0.5));
	      RegisterHisto(al_mon,m_pix_eca_unbiased_yresvsmodphi_disks[iWheel]);
	      	      
	      if (m_extendedPlots)
		{
		  //biased residuals
		  m_pix_eca_biased_residualsx.push_back(new TH1F(("pix_eca_d"+intToString(iWheel)+"_biased_residualx").c_str(),("Biased X Residual Pixel EndCap A Disk"+intToString(iWheel)+";Residual (mm)").c_str(),100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		  RegisterHisto(al_mon,m_pix_eca_biased_residualsx[iWheel]);
		  m_pix_eca_biased_residualsy.push_back(new TH1F(("pix_eca_d"+intToString(iWheel)+"_biased_residualy").c_str(),("Biased Y Residual Pixel EndCap A Disk "+intToString(iWheel)+";Residual (mm)").c_str(),100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
		  RegisterHisto(al_mon,m_pix_eca_biased_residualsy[iWheel]);
		  //residual pt
		  m_pix_eca_residualsx_pt.push_back(new TH2F(("pix_eca_d"+intToString(iWheel)+"_residualx_pt").c_str(),("Unbiased X Residual Vs Pt Pixel EndCap A Disk "+intToString(iWheel)+";Track pT (GeV);Res (mm)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		  RegisterHisto(al_mon,m_pix_eca_residualsx_pt[iWheel]);
		  m_pix_eca_residualsy_pt.push_back(new TH2F(("pix_eca_d"+intToString(iWheel)+"_residualy_pt").c_str(),("Unbiased Y Residual Vs Pt Pixel EndCap A Disk "+intToString(iWheel)+";Track pT (GeV);Res (mm)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
		  RegisterHisto(al_mon,m_pix_eca_residualsy_pt[iWheel]);
		  //pull pt
		  m_pix_eca_pullsx_pt.push_back(new TH2F(("pix_eca_d"+intToString(iWheel)+"_pullx_pt").c_str(),("Unbiased X Pull Vs Pt Pixel EndCap A Disk "+intToString(iWheel)+";Track pT (GeV)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos));
		  RegisterHisto(al_mon,m_pix_eca_pullsx_pt[iWheel]);
		  m_pix_eca_pullsy_pt.push_back(new TH2F(("pix_eca_d"+intToString(iWheel)+"_pully_pt").c_str(),("Unbiased Y Pull Vs Pt Pixel EndCap A Disk "+intToString(iWheel)+";Track pT (GeV)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos));
		  RegisterHisto(al_mon,m_pix_eca_pullsy_pt[iWheel]);
		  // Pixel unbiased residuals vs QoverP^2
		  m_pix_eca_residualsx_qoverp2.push_back(new TH2F(("pix_eca_d"+intToString(iWheel)+"_unbiased_residualx_qoverp2").c_str(),("Unbiased X Residual Vs Q/P^{2} Pixel EndCap A Disk "+intToString(iWheel)+";Track Q/P^{2} (GeV^{-2});Res (mm)").c_str(),20,customaxis,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		  RegisterHisto(al_mon,m_pix_eca_residualsx_qoverp2[iWheel]);
		  m_pix_eca_residualsy_qoverp2.push_back(new TH2F(("pix_eca_d"+intToString(iWheel)+"_unbiased_residualy_qoverp2").c_str(),("Unbiased Y Residual Vs Q/P^{2} Pixel EndCap A Disk "+intToString(iWheel)+";Track Q/P^{2} (GeV^{-2});Res (mm)").c_str(),20,customaxis,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
		  RegisterHisto(al_mon,m_pix_eca_residualsy_qoverp2[iWheel]);
		  //biased residual pt
		  m_pix_eca_biased_residualsx_pt.push_back(new TH2F(("pix_eca_d"+intToString(iWheel)+"_biased_residualx_pt").c_str(),("Biased X Residual Vs Pt Pixel EndCap A Disk "+intToString(iWheel)+";Track pT (GeV);Res (mm)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		  RegisterHisto(al_mon,m_pix_eca_biased_residualsx_pt[iWheel]);
		  m_pix_eca_biased_residualsy_pt.push_back(new TH2F(("pix_eca_d"+intToString(iWheel)+"_biased_residualy_pt").c_str(),("Biased Y Residual Vs Pt Pixel EndCap A Disk "+intToString(iWheel)+";Track pT (GeV);Res (mm)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
		  RegisterHisto(al_mon,m_pix_eca_biased_residualsy_pt[iWheel]);
		  //biased residual qoverp2
		  m_pix_eca_biased_residualsx_qoverp2.push_back(new TH2F(("pix_eca_d"+intToString(iWheel)+"_biased_residualx_qoverp2").c_str(),("Biased X Residual Vs Q/P^{2} Pixel EndCap A Disk "+intToString(iWheel)+";Track Q/P^{2} (GeV^{-2});Res (mm)").c_str(),20,customaxis,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		  RegisterHisto(al_mon,m_pix_eca_biased_residualsx_qoverp2[iWheel]);
		  m_pix_eca_biased_residualsy_qoverp2.push_back(new TH2F(("pix_eca_d"+intToString(iWheel)+"_biased_residualy_qoverp2").c_str(),("Biased Y Residual Vs Q/P^{2} Pixel EndCap A Disk "+intToString(iWheel)+";Track Q/P^{2} (GeV^{-2});Res (mm)").c_str(),20,customaxis,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
		  RegisterHisto(al_mon,m_pix_eca_biased_residualsy_qoverp2[iWheel]);
		  //biased resvsmodphi
		  m_pix_eca_biased_xresvsmodphi_2ds.push_back(new TH2F(("pix_eca_d"+intToString(iWheel)+"_biased_xresvsmodphi_2d").c_str(),("X Biased Residual Distbn vs Module Phi Pixel ECA D"+intToString(iWheel)+";Module Phi ID; Residual X (mm)").c_str(),maxPhiModulesPerRing,0,maxPhiModulesPerRing,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		  RegisterHisto(al_mon,m_pix_eca_biased_xresvsmodphi_2ds[iWheel]);
		  m_pix_eca_biased_yresvsmodphi_2ds.push_back(new TH2F(("pix_eca_d"+intToString(iWheel)+"_biased_yresvsmodphi_2d").c_str(),("Y Biased Residual Distbn vs Module Phi Pixel ECA D"+intToString(iWheel)+";Module Phi ID; Residual Y (mm)").c_str(),maxPhiModulesPerRing,0,maxPhiModulesPerRing,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
		  RegisterHisto(al_mon,m_pix_eca_biased_yresvsmodphi_2ds[iWheel]);
		  
		  if (m_doClusterSizeHistos)
		    {
		      m_pix_eca_clustersize.push_back(new TH1F(("pix_eca_d"+intToString(iWheel)+"_clustersize").c_str(),("Cluster size Pixel ECA Disk "+intToString(iWheel)).c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange));
		      RegisterHisto(al_mon,m_pix_eca_clustersize[iWheel]);
		      m_pix_eca_clustersizePhi.push_back(new TH1F(("pix_eca_d"+intToString(iWheel)+"_clustersizePhi").c_str(),("Cluster Phi size Pixel ECA Disk "+intToString(iWheel)).c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange));
		      RegisterHisto(al_mon,m_pix_eca_clustersizePhi[iWheel]);
		      m_pix_eca_clustersizeZ.push_back(new TH1F(("pix_eca_d"+intToString(iWheel)+"_clustersizeZ").c_str(),("Cluster Z size Pixel ECA Disk "+intToString(iWheel)).c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange));
		      RegisterHisto(al_mon,m_pix_eca_clustersizeZ[iWheel]);
		      
		      m_pix_eca_residualsx_clustersize.push_back( new TH2F(("pix_eca_d"+intToString(iWheel)+"_residualx_clustersize").c_str(),("Unbiased X Residual Vs Cluster Size Pixel ECA Disk "+intToString(iWheel)+";Cluster Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_eca_residualsx_clustersize[iWheel]);
		      m_pix_eca_residualsy_clustersize.push_back( new TH2F(("pix_eca_d"+intToString(iWheel)+"_residualy_clustersize").c_str(),("Unbiased Y Residual Vs Cluster Size Pixel ECA Disk "+intToString(iWheel)+";Cluster Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_eca_residualsy_clustersize[iWheel]);
		      
		      m_pix_eca_residualsx_clustersizePhi.push_back( new TH2F(("pix_eca_d"+intToString(iWheel)+"_residualx_clustersizePhi").c_str(),("Unbiased X Residual Vs Cluster Phi Size ECA Disk "+intToString(iWheel)+";Cluster Phi Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_eca_residualsx_clustersizePhi[iWheel]);
		      m_pix_eca_residualsy_clustersizePhi.push_back( new TH2F(("pix_eca_d"+intToString(iWheel)+"_residualy_clustersizePhi").c_str(),("Unbiased Y Residual Vs Cluster Phi Size Pixel ECA Disk "+intToString(iWheel)+";Cluster Phi Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_eca_residualsy_clustersizePhi[iWheel]);
		      
		      m_pix_eca_residualsx_clustersizeZ.push_back( new TH2F(("pix_eca_d"+intToString(iWheel)+"_residualx_clustersizeZ").c_str(),("Unbiased X Residual Vs Cluster Z Size Pixel ECA Disk "+intToString(iWheel)+";Cluster Z Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_eca_residualsx_clustersizeZ[iWheel]);
		      m_pix_eca_residualsy_clustersizeZ.push_back( new TH2F(("pix_eca_d"+intToString(iWheel)+"_residualy_clustersizeZ").c_str(),("Unbiased Y Residual Vs Cluster Z Size Pixel ECA Disk "+intToString(iWheel)+";Cluster Z Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_eca_residualsy_clustersizeZ[iWheel]);

		      m_pix_eca_residualsx_clustersizeP.push_back( new TProfile(("pix_eca_d"+intToString(iWheel)+"_residualx_clustersize_p").c_str(),("Unbiased X Residual Vs Cluster Size Pixel ECA Disk "+intToString(iWheel)+";Cluster Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_eca_residualsx_clustersizeP[iWheel]);
		      m_pix_eca_residualsy_clustersizeP.push_back( new TProfile(("pix_eca_d"+intToString(iWheel)+"_residualy_clustersize_p").c_str(),("Unbiased Y Residual Vs Cluster Size Pixel ECA Disk "+intToString(iWheel)+";Cluster Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_eca_residualsy_clustersizeP[iWheel]);
		      
		      m_pix_eca_residualsx_clustersizePhiP.push_back( new TProfile(("pix_eca_d"+intToString(iWheel)+"_residualx_clustersizePhi_p").c_str(),("Unbiased X Residual Vs Cluster Phi Size ECA Disk "+intToString(iWheel)+";Cluster Phi Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_eca_residualsx_clustersizePhiP[iWheel]);
		      m_pix_eca_residualsy_clustersizePhiP.push_back( new TProfile(("pix_eca_d"+intToString(iWheel)+"_residualy_clustersizePhi_p").c_str(),("Unbiased Y Residual Vs Cluster Phi Size Pixel ECA Disk "+intToString(iWheel)+";Cluster Phi Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_eca_residualsy_clustersizePhiP[iWheel]);
		      
		      m_pix_eca_residualsx_clustersizeZP.push_back( new TProfile(("pix_eca_d"+intToString(iWheel)+"_residualx_clustersizeZ_p").c_str(),("Unbiased X Residual Vs Cluster Z Size Pixel ECA Disk "+intToString(iWheel)+";Cluster Z Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_eca_residualsx_clustersizeZP[iWheel]);
		      m_pix_eca_residualsy_clustersizeZP.push_back( new TProfile(("pix_eca_d"+intToString(iWheel)+"_residualy_clustersizeZ_p").c_str(),("Unbiased Y Residual Vs Cluster Z Size Pixel ECA Disk "+intToString(iWheel)+";Cluster Z Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_eca_residualsy_clustersizeZP[iWheel]);

		      m_pix_eca_clustersize_incidentAngle.push_back(new TProfile(("pix_eca_d"+intToString(iWheel)+"_clustersize_incidentAngle").c_str(),("Cluster Size Pixel Vs Incident Theta Angle Pixel ECA Disk "+intToString(iWheel)+"Incident Theta Angle (rad);Cluster Size").c_str(),20,-m_IncidentThetaRange,m_IncidentThetaRange,0,m_ClusterSizeRange));
		      RegisterHisto(al_mon,m_pix_eca_clustersize_incidentAngle[iWheel]);
		      m_pix_eca_clustersizePhi_incidentAngle.push_back(new TProfile(("pix_eca_d"+intToString(iWheel)+"_clustersizePhi_incidentAngle").c_str(),("Cluster Size Pixel Vs Incident Theta Angle Pixel ECA Disk "+intToString(iWheel)+"Incident Theta Angle (rad);Cluster Phi Size").c_str(),20,-m_IncidentThetaRange,m_IncidentThetaRange,0,m_ClusterSizeRange));
		      RegisterHisto(al_mon,m_pix_eca_clustersizePhi_incidentAngle[iWheel]);
		      m_pix_eca_clustersizeZ_incidentAngle.push_back(new TProfile(("pix_eca_d"+intToString(iWheel)+"_clustersizeZ_incidentAngle").c_str(),("Cluster Size Pixel Vs Incident Theta Angle Pixel ECA Disk "+intToString(iWheel)+"Incident Theta Angle (rad);Cluster Z Size").c_str(),20,-m_IncidentThetaRange,m_IncidentThetaRange,0,m_ClusterSizeRange));
		      RegisterHisto(al_mon,m_pix_eca_clustersizeZ_incidentAngle[iWheel]);

		      m_pix_eca_clustersize_incidentAnglePhi.push_back(new TProfile(("pix_eca_d"+intToString(iWheel)+"_clustersize_incidentAnglePhi").c_str(),("Cluster Size Pixel Vs Incident Phi Angle Pixel ECA Disk "+intToString(iWheel)+"Incident Phi Angle (rad);Cluster Size").c_str(),20,-m_IncidentPhiRange,m_IncidentPhiRange,0,m_ClusterSizeRange));
		      RegisterHisto(al_mon,m_pix_eca_clustersize_incidentAnglePhi[iWheel]);
		      m_pix_eca_clustersizePhi_incidentAnglePhi.push_back(new TProfile(("pix_eca_d"+intToString(iWheel)+"_clustersizePhi_incidentAnglePhi").c_str(),("Cluster Phi Size Pixel Vs Incident Phi Angle Pixel ECA Disk "+intToString(iWheel)+"Incident Phi Angle (rad);Cluster Phi Size").c_str(),20,-m_IncidentPhiRange,m_IncidentPhiRange,0,m_ClusterSizeRange));
		      RegisterHisto(al_mon,m_pix_eca_clustersizePhi_incidentAnglePhi[iWheel]);
		      m_pix_eca_clustersizeZ_incidentAnglePhi.push_back(new TProfile(("pix_eca_d"+intToString(iWheel)+"_clustersizeZ_incidentAnglePhi").c_str(),("Cluster Z Size Pixel Vs Incident Phi Angle Pixel ECA Disk "+intToString(iWheel)+"Incident Phi Angle (rad);Cluster Z Size").c_str(),20,-m_IncidentPhiRange,m_IncidentPhiRange,0,m_ClusterSizeRange));
		      RegisterHisto(al_mon,m_pix_eca_clustersizeZ_incidentAnglePhi[iWheel]);
		    }
		}
	    }
	  else if (iSide<0)
	    {
	      m_pix_ecc_unbiased_xresvsmodphi_disks.push_back(new TProfile(("pix_ecc_unbiased_xresvsmodphi_disk"+intToString(iWheel)).c_str(),("UNBIASED X Residual Average vs Module Phi of Pixel Endcap C Disk "+intToString(iWheel)).c_str(),maxPhiModulesPerRing,-0.5,maxPhiModulesPerRing-0.5));
	      RegisterHisto(al_mon,m_pix_ecc_unbiased_xresvsmodphi_disks[iWheel]);
	      m_pix_ecc_unbiased_yresvsmodphi_disks.push_back(new TProfile(("pix_ecc_unbiased_yresvsmodphi_disk"+intToString(iWheel)).c_str(),("UNBIASED Y Residual Average vs Module Phi of Pixel Endcap C Disk "+intToString(iWheel)).c_str(),maxPhiModulesPerRing,-0.5,maxPhiModulesPerRing-0.5));
	      RegisterHisto(al_mon,m_pix_ecc_unbiased_yresvsmodphi_disks[iWheel]);
	    
	      
	      if (m_extendedPlots)
		{
		  // Pixel biased residuals
		  m_pix_ecc_biased_residualsx.push_back(new TH1F(("pix_ecc_d"+intToString(iWheel)+"_biased_residualx").c_str(),("Biased X Residual Pixel EndCap C Disk "+intToString(iWheel)+";Residual (mm)").c_str(),100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		  RegisterHisto(al_mon,m_pix_ecc_biased_residualsx[iWheel]);
		  m_pix_ecc_biased_residualsy.push_back(new TH1F(("pix_ecc_d"+intToString(iWheel)+"_biased_residualy").c_str(),("Biased Y Residual Pixel EndCap C Disk "+intToString(iWheel)+";Residual (mm)").c_str(),100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
		  RegisterHisto(al_mon,m_pix_ecc_biased_residualsy[iWheel]);
		  //residual pt
		  m_pix_ecc_residualsx_pt.push_back(new TH2F(("pix_ecc_d"+intToString(iWheel)+"_residualx_pt").c_str(),("Unbiased X Residual Vs Pt Pixel EndCap C Disk "+intToString(iWheel)+";Track pT (GeV);Res (mm)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		  RegisterHisto(al_mon,m_pix_ecc_residualsx_pt[iWheel]);
		  m_pix_ecc_residualsy_pt.push_back(new TH2F(("pix_ecc_d"+intToString(iWheel)+"_residualy_pt").c_str(),("Unbiased Y Residual Vs Pt Pixel EndCap C Disk "+intToString(iWheel)+";Track pT (GeV);Res (mm)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
		  RegisterHisto(al_mon,m_pix_ecc_residualsy_pt[iWheel]);
		  //pull pt
		  m_pix_ecc_pullsx_pt.push_back(new TH2F(("pix_ecc_d"+intToString(iWheel)+"_pullx_pt").c_str(),("Unbiased X Pull Vs Pt Pixel EndCap C Disk "+intToString(iWheel)+";Track pT (GeV)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos));
		  RegisterHisto(al_mon,m_pix_ecc_pullsx_pt[iWheel]);
		  m_pix_ecc_pullsy_pt.push_back(new TH2F(("pix_ecc_d"+intToString(iWheel)+"_pully_pt").c_str(),("Unbiased Y Pull Vs Pt Pixel EndCap C Disk "+intToString(iWheel)+";Track pT (GeV)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos));
		  RegisterHisto(al_mon,m_pix_ecc_pullsy_pt[iWheel]);
		  // Pixel unbiased residuals vs QoverP^2
		  m_pix_ecc_residualsx_qoverp2.push_back(new TH2F(("pix_ecc_d"+intToString(iWheel)+"_unbiased_residualx_qoverp2").c_str(),("Unbiased X Residual Vs Q/P^{2} Pixel EndCap C Disk "+intToString(iWheel)+";Track Q/P^{2} (GeV^{-2});Res (mm)").c_str(),20,customaxis,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		  RegisterHisto(al_mon,m_pix_ecc_residualsx_qoverp2[iWheel]);
		  m_pix_ecc_residualsy_qoverp2.push_back(new TH2F(("pix_ecc_d"+intToString(iWheel)+"_unbiased_residualy_qoverp2").c_str(),("Unbiased Y Residual Vs Q/P^{2} Pixel EndCap C Disk "+intToString(iWheel)+";Track Q/P^{2} (GeV^{-2});Res (mm)").c_str(),20,customaxis,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
		  RegisterHisto(al_mon,m_pix_ecc_residualsy_qoverp2[iWheel]);
		  //biased residual pt
		  m_pix_ecc_biased_residualsx_pt.push_back(new TH2F(("pix_ecc_d"+intToString(iWheel)+"_biased_residualx_pt").c_str(),("Biased X Residual Vs Pt Pixel EndCap C Disk "+intToString(iWheel)+";Track pT (GeV);Res (mm)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		  RegisterHisto(al_mon,m_pix_ecc_biased_residualsx_pt[iWheel]);
		  m_pix_ecc_biased_residualsy_pt.push_back(new TH2F(("pix_ecc_d"+intToString(iWheel)+"_biased_residualy_pt").c_str(),("Biased Y Residual Vs Pt Pixel EndCap C Disk "+intToString(iWheel)+";Track pT (GeV);Res (mm)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
		  RegisterHisto(al_mon,m_pix_ecc_biased_residualsy_pt[iWheel]);
		  //biased residual qoverp2
		  m_pix_ecc_biased_residualsx_qoverp2.push_back(new TH2F(("pix_ecc_d"+intToString(iWheel)+"_biased_residualx_qoverp2").c_str(),("Biased X Residual Vs Q/P^{2} Pixel EndCap C Disk "+intToString(iWheel)+";Track Q/P^{2} (GeV^{-2});Res (mm)").c_str(),20,customaxis,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		  RegisterHisto(al_mon,m_pix_ecc_biased_residualsx_qoverp2[iWheel]);
		  m_pix_ecc_biased_residualsy_qoverp2.push_back(new TH2F(("pix_ecc_d"+intToString(iWheel)+"_biased_residualy_qoverp2").c_str(),("Biased Y Residual Vs Q/P^{2} Pixel EndCap C Disk "+intToString(iWheel)+";Track Q/P^{2} (GeV^{-2});Res (mm)").c_str(),20,customaxis,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
		  RegisterHisto(al_mon,m_pix_ecc_biased_residualsy_qoverp2[iWheel]);
		  //biased resvsmodphi
		  m_pix_ecc_biased_xresvsmodphi_2ds.push_back(new TH2F(("pix_ecc_d"+intToString(iWheel)+"_biased_xresvsmodphi_2d").c_str(),("X Biased Residual Distbn vs Module Phi Pixel ECC D"+intToString(iWheel)+";Module Phi ID; Residual X (mm)").c_str(),maxPhiModulesPerRing,0,maxPhiModulesPerRing,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		  RegisterHisto(al_mon,m_pix_ecc_biased_xresvsmodphi_2ds[iWheel]);
		  m_pix_ecc_biased_yresvsmodphi_2ds.push_back(new TH2F(("pix_ecc_d"+intToString(iWheel)+"_biased_yresvsmodphi_2d").c_str(),("Y Biased Residual Distbn vs Module Phi Pixel ECC D"+intToString(iWheel)+";Module Phi ID; Residual Y (mm)").c_str(),maxPhiModulesPerRing,0,maxPhiModulesPerRing,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange));
		  RegisterHisto(al_mon,m_pix_ecc_biased_yresvsmodphi_2ds[iWheel]);	
		  if (m_doClusterSizeHistos)
		    {
		      m_pix_ecc_clustersize.push_back(new TH1F(("pix_ecc_d"+intToString(iWheel)+"_clustersize").c_str(),("Cluster size Pixel ECC Disk "+intToString(iWheel)).c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange));
		      RegisterHisto(al_mon,m_pix_ecc_clustersize[iWheel]);
		      m_pix_ecc_clustersizePhi.push_back(new TH1F(("pix_ecc_d"+intToString(iWheel)+"_clustersizePhi").c_str(),("Cluster Phi size Pixel ECC Disk "+intToString(iWheel)).c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange));
		      RegisterHisto(al_mon,m_pix_ecc_clustersizePhi[iWheel]);
		      m_pix_ecc_clustersizeZ.push_back(new TH1F(("pix_ecc_d"+intToString(iWheel)+"_clustersizeZ").c_str(),("Cluster Z size Pixel ECC Disk "+intToString(iWheel)).c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange));
		      RegisterHisto(al_mon,m_pix_ecc_clustersizeZ[iWheel]);
		      
		      m_pix_ecc_residualsx_clustersize.push_back( new TH2F(("pix_ecc_d"+intToString(iWheel)+"_residualx_clustersize").c_str(),("Unbiased X Residual Vs Cluster Size Pixel ECC Disk "+intToString(iWheel)+";Cluster Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_ecc_residualsx_clustersize[iWheel]);
		      m_pix_ecc_residualsy_clustersize.push_back( new TH2F(("pix_ecc_d"+intToString(iWheel)+"_residualy_clustersize").c_str(),("Unbiased Y Residual Vs Cluster Size Pixel ECC Disk "+intToString(iWheel)+";Cluster Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_ecc_residualsy_clustersize[iWheel]);
	    
		      m_pix_ecc_residualsx_clustersizePhi.push_back( new TH2F(("pix_ecc_d"+intToString(iWheel)+"_residualx_clustersizePhi").c_str(),("Unbiased X Residual Vs Cluster Phi Size ECC Disk "+intToString(iWheel)+";Cluster Phi Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_ecc_residualsx_clustersizePhi[iWheel]);
		      m_pix_ecc_residualsy_clustersizePhi.push_back( new TH2F(("pix_ecc_d"+intToString(iWheel)+"_residualy_clustersizePhi").c_str(),("Unbiased Y Residual Vs Cluster Phi Size Pixel ECC Disk "+intToString(iWheel)+";Cluster Phi Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_ecc_residualsy_clustersizePhi[iWheel]);

		      m_pix_ecc_residualsx_clustersizeZ.push_back( new TH2F(("pix_ecc_d"+intToString(iWheel)+"_residualx_clustersizeZ").c_str(),("Unbiased X Residual Vs Cluster Z Size Pixel ECC Disk "+intToString(iWheel)+";Cluster Z Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_ecc_residualsx_clustersizeZ[iWheel]);
		      m_pix_ecc_residualsy_clustersizeZ.push_back( new TH2F(("pix_ecc_d"+intToString(iWheel)+"_residualy_clustersizeZ").c_str(),("Unbiased Y Residual Vs Cluster Z Size Pixel ECC Disk "+intToString(iWheel)+";Cluster Z Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_ecc_residualsy_clustersizeZ[iWheel]);
		      
		      m_pix_ecc_residualsx_clustersizeP.push_back( new TProfile(("pix_ecc_d"+intToString(iWheel)+"_residualx_clustersize_p").c_str(),("Unbiased X Residual Vs Cluster Size Pixel ECC Disk "+intToString(iWheel)+";Cluster Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_ecc_residualsx_clustersizeP[iWheel]);
		      m_pix_ecc_residualsy_clustersizeP.push_back( new TProfile(("pix_ecc_d"+intToString(iWheel)+"_residualy_clustersize_p").c_str(),("Unbiased Y Residual Vs Cluster Size Pixel ECC Disk "+intToString(iWheel)+";Cluster Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_ecc_residualsy_clustersizeP[iWheel]);
		      
		      m_pix_ecc_residualsx_clustersizePhiP.push_back( new TProfile(("pix_ecc_d"+intToString(iWheel)+"_residualx_clustersizePhi_p").c_str(),("Unbiased X Residual Vs Cluster Phi Size ECC Disk "+intToString(iWheel)+";Cluster Phi Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_ecc_residualsx_clustersizePhiP[iWheel]);
		      m_pix_ecc_residualsy_clustersizePhiP.push_back( new TProfile(("pix_ecc_d"+intToString(iWheel)+"_residualy_clustersizePhi_p").c_str(),("Unbiased Y Residual Vs Cluster Phi Size Pixel ECC Disk "+intToString(iWheel)+";Cluster Phi Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_ecc_residualsy_clustersizePhiP[iWheel]);
		      
		      m_pix_ecc_residualsx_clustersizeZP.push_back( new TProfile(("pix_ecc_d"+intToString(iWheel)+"_residualx_clustersizeZ_p").c_str(),("Unbiased X Residual Vs Cluster Z Size Pixel ECC Disk "+intToString(iWheel)+";Cluster Z Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_ecc_residualsx_clustersizeZP[iWheel]);
		      m_pix_ecc_residualsy_clustersizeZP.push_back( new TProfile(("pix_ecc_d"+intToString(iWheel)+"_residualy_clustersizeZ_p").c_str(),("Unbiased Y Residual Vs Cluster Z Size Pixel ECC Disk "+intToString(iWheel)+";Cluster Z Size;PIX Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,m_minPIXResXFillRange,m_maxPIXResXFillRange));
		      RegisterHisto(al_mon,m_pix_ecc_residualsy_clustersizeZP[iWheel]);
		      
		      m_pix_ecc_clustersize_incidentAngle.push_back(new TProfile(("pix_ecc_d"+intToString(iWheel)+"_clustersize_incidentAngle").c_str(),("Cluster Size Pixel Vs Incident Theta Angle Pixel ECC Disk "+intToString(iWheel)+";Incident Theta Angle [rad];Cluster Size").c_str(),20,-m_IncidentThetaRange,m_IncidentThetaRange,0,m_ClusterSizeRange));
		      RegisterHisto(al_mon,m_pix_ecc_clustersize_incidentAngle[iWheel]);
		      m_pix_ecc_clustersizePhi_incidentAngle.push_back(new TProfile(("pix_ecc_d"+intToString(iWheel)+"_clustersizePhi_incidentAngle").c_str(),("Cluster Phi Size Pixel Vs Incident Theta Angle Pixel ECC Disk "+intToString(iWheel)+";Incident Theta Angle [rad];Cluster Phi Size").c_str(),20,-m_IncidentThetaRange,m_IncidentThetaRange,0,m_ClusterSizeRange));
		      RegisterHisto(al_mon,m_pix_ecc_clustersizePhi_incidentAngle[iWheel]);
		      m_pix_ecc_clustersizeZ_incidentAngle.push_back(new TProfile(("pix_ecc_d"+intToString(iWheel)+"_clustersizeZ_incidentAngle").c_str(),("Cluster Z Size Pixel Vs Incident Theta Angle Pixel ECC Disk "+intToString(iWheel)+";Incident Theta Angle [rad];Cluster Z Size").c_str(),20,-m_IncidentThetaRange,m_IncidentThetaRange,0,m_ClusterSizeRange));
		      RegisterHisto(al_mon,m_pix_ecc_clustersizeZ_incidentAngle[iWheel]);
		      
		      m_pix_ecc_clustersize_incidentAnglePhi.push_back(new TProfile(("pix_ecc_d"+intToString(iWheel)+"_clustersize_incidentAnglePhi").c_str(),("Cluster Size Pixel Vs Incident Phi Angle Pixel ECC Disk "+intToString(iWheel)+";Incident Phi Angle [rad];Cluster Size").c_str(),20,-m_IncidentPhiRange,m_IncidentPhiRange,0,m_ClusterSizeRange));
		      RegisterHisto(al_mon,m_pix_ecc_clustersize_incidentAnglePhi[iWheel]);
		      m_pix_ecc_clustersizePhi_incidentAnglePhi.push_back(new TProfile(("pix_ecc_d"+intToString(iWheel)+"_clustersizePhi_incidentAnglePhi").c_str(),("Cluster Phi Size Pixel Vs Incident Phi Angle Pixel ECC Disk "+intToString(iWheel)+";Incident Phi Angle [rad];Cluster Phi Size").c_str(),20,-m_IncidentPhiRange,m_IncidentPhiRange,0,m_ClusterSizeRange));
		      RegisterHisto(al_mon,m_pix_ecc_clustersizePhi_incidentAnglePhi[iWheel]);
		      m_pix_ecc_clustersizeZ_incidentAnglePhi.push_back(new TProfile(("pix_ecc_d"+intToString(iWheel)+"_clustersizeZ_incidentAnglePhi").c_str(),("Cluster Z Size Pixel Vs Incident Phi Angle Pixel ECC Disk "+intToString(iWheel)+";Incident Phi Angle [rad];Cluster Z Size").c_str(),20,-m_IncidentPhiRange,m_IncidentPhiRange,0,m_ClusterSizeRange));
		      RegisterHisto(al_mon,m_pix_ecc_clustersizeZ_incidentAnglePhi[iWheel]);
		    }
		}
	    }
	  else
	    ATH_MSG_ERROR(" iSide ==0: Creating endcaps histos but obtained a barrel identifier");
	} //iWheel   
    }
  //General Basic plots
  //Residuals
  m_pix_eca_residualx = new TH1F("pix_eca_residualx","UnBiased X Residual Pixel EndCap A",100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
  RegisterHisto(al_mon,m_pix_eca_residualx);
  m_pix_eca_residualy = new TH1F("pix_eca_residualy","UnBiased Y Residual Pixel EndCap A",100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange);
  RegisterHisto(al_mon,m_pix_eca_residualy);
  m_pix_ecc_residualx = new TH1F("pix_ecc_residualx","UnBiased X Residual Pixel EndCap C",100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
  RegisterHisto(al_mon,m_pix_ecc_residualx);
  m_pix_ecc_residualy = new TH1F("pix_ecc_residualy","UnBiased Y Residual Pixel EndCap A",100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange);
  RegisterHisto(al_mon,m_pix_ecc_residualy);

  //pulls
  m_pix_eca_pullx = new TH1F("pix_eca_pullx","UnBiased X Pull Pixel Endcap A",100,-m_RangeOfPullHistos, m_RangeOfPullHistos);
  RegisterHisto(al_mon,m_pix_eca_pullx);  
  m_pix_eca_pully = new TH1F("pix_eca_pully","UnBiased Y Pull Pixel Endcap A",100,-m_RangeOfPullHistos, m_RangeOfPullHistos);
  RegisterHisto(al_mon,m_pix_eca_pully);  
  m_pix_ecc_pullx = new TH1F("pix_ecc_pullx","UnBiased X Pull Pixel Endcap C",100,-m_RangeOfPullHistos, m_RangeOfPullHistos);
  RegisterHisto(al_mon,m_pix_ecc_pullx);  
  m_pix_ecc_pully = new TH1F("pix_ecc_pully","UnBiased Y Pull Pixel Endcap C",100,-m_RangeOfPullHistos, m_RangeOfPullHistos);
  RegisterHisto(al_mon,m_pix_ecc_pully);
  //2d histograms of residual versus module phi in endcaps for filling of Residual mean plots in post-processing
  //std::cout<<"TOTALPHIMODULES "<<totalPhiModules<<std::endl;
  m_pix_eca_xresvsmodphi_2d = new TH2F("pix_eca_xresvsmodphi_2d","X Residual vs (Modified) Module Phi Pixel Endcap A;(modified) Mod Phi;Residual [mm]",totalPhiModules,0,totalPhiModules,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
  RegisterHisto(al_mon,m_pix_eca_xresvsmodphi_2d);
  m_pix_ecc_xresvsmodphi_2d = new TH2F("pix_ecc_xresvsmodphi_2d","X Residual vs (Modified) Module Phi Pixel Endcap C;(modified) Mod Phi;Residual [mm]",totalPhiModules,0,totalPhiModules,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
  RegisterHisto(al_mon,m_pix_ecc_xresvsmodphi_2d);
  m_pix_eca_yresvsmodphi_2d = new TH2F("pix_eca_yresvsmodphi_2d","Y Residual vs (Modified) Module Phi Pixel Endcap A;(modified) Mod Phi;Residual [mm]",totalPhiModules,0,totalPhiModules,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
  RegisterHisto(al_mon,m_pix_eca_yresvsmodphi_2d);
  m_pix_ecc_yresvsmodphi_2d = new TH2F("pix_ecc_yresvsmodphi_2d","Y Residual vs (Modified) Module Phi Pixel Endcap C;(modified) Mod Phi;Residual [mm]",totalPhiModules,0,totalPhiModules,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
  RegisterHisto(al_mon,m_pix_ecc_yresvsmodphi_2d);
  //Profile histograms that summarizes the unbiased residuals for the endcaps
  //m_pix_eca_unbiased_xresvsmodphi = new TProfile("pix_eca_unbiased_xresvsmodphi","UNBIASED X Residual Average vs Module Phi of Pixel Endcap A;(modified) ModPhi; Avg Res (mm)",totalPhiModules,0,totalPhiModules,m_minPIXResXFillRange,m_maxPIXResXFillRange);
  //RegisterHisto(al_mon,m_pix_eca_unbiased_xresvsmodphi);
  //m_pix_ecc_unbiased_xresvsmodphi = new TProfile("pix_ecc_unbiased_xresvsmodphi","UNBIASED X Residual Average vs Module Phi of Pixel Endcap C;(modified) ModPhi; Avg Res (mm)",totalPhiModules,0,totalPhiModules,m_minPIXResXFillRange,m_maxPIXResXFillRange);
  //RegisterHisto(al_mon,m_pix_ecc_unbiased_xresvsmodphi);
  //m_pix_eca_unbiased_yresvsmodphi = new TProfile("pix_eca_unbiased_yresvsmodphi","UNBIASED Y Residual Average vs Module Phi of Pixel Endcap A;(modified) ModPhi; Avg Res (mm)",totalPhiModules,0,totalPhiModules,m_minPIXResYFillRange,m_maxPIXResYFillRange);
  //RegisterHisto(al_mon,m_pix_eca_unbiased_yresvsmodphi);
  //m_pix_ecc_unbiased_yresvsmodphi = new TProfile("pix_ecc_unbiased_yresvsmodphi","UNBIASED Y Residual Average vs Module Phi of Pixel Endcap C;(modified) ModPhi; Avg Res (mm)",totalPhiModules,0,totalPhiModules,m_minPIXResYFillRange,m_maxPIXResYFillRange);
  //RegisterHisto(al_mon,m_pix_ecc_unbiased_yresvsmodphi);    
  //Profile histograms that summarizes the overlap residuals for the endcaps
  m_pix_eca_Oxresxvsmodphi = new TProfile("pix_eca_Oxresxvsmodphi","X_Overlap Residual X Mean vs (Modified) Module Phi Pixel Endcap A;(mod) ModPhi; Mean Res [mm]",totalPhiModules,0,totalPhiModules,m_minPIXResXFillRange,m_maxPIXResXFillRange);
  RegisterHisto(al_mon,m_pix_eca_Oxresxvsmodphi);  
  m_pix_eca_Oyresyvsmodphi = new TProfile("pix_eca_Oyresyvsmodphi","Y_Overlap Residual Y Mean vs (Modified) Module Phi Pixel Endcap A;(mod) ModPhi; Mean Res [mm]",totalPhiModules,0,totalPhiModules,m_minPIXResYFillRange,m_maxPIXResYFillRange);
  RegisterHisto(al_mon,m_pix_eca_Oyresyvsmodphi);  
  m_pix_ecc_Oxresxvsmodphi = new TProfile("pix_ecc_Oxresxvsmodphi","X_Overlap Residual X Mean vs (Modified) Module Phi Pixel Endcap C;(mod) ModPhi; Mean Res [mm]",totalPhiModules,0,totalPhiModules,m_minPIXResXFillRange,m_maxPIXResXFillRange);
  RegisterHisto(al_mon,m_pix_ecc_Oxresxvsmodphi);  
  m_pix_ecc_Oyresyvsmodphi = new TProfile("pix_ecc_Oyresyvsmodphi","Y_Overlap Residual Y Mean vs (Modified) Module Phi Pixel Endcap C;(mod) ModPhi; Mean Res [mm]",totalPhiModules,0,totalPhiModules,m_minPIXResYFillRange,m_maxPIXResYFillRange);
  RegisterHisto(al_mon,m_pix_ecc_Oyresyvsmodphi);
  //These histograms are filled in post-processing - only initiated here such that they can be registered as "shift".    
  //residual mean as function of module eta/phi in endcaps
  m_pix_eca_xresvsmodphi = new TH1F("pix_eca_xresvsmodphi","X Residual Mean vs (Modified) Module Phi Pixel Endcap A;(Modified) Module Phi Identifier;Mean Residual X",165,0,165);
  RegisterHisto(al_mon,m_pix_eca_xresvsmodphi);  
  m_pix_eca_yresvsmodphi = new TH1F("pix_eca_yresvsmodphi","Y Residual Mean vs (Modified) Module Phi Pixel Endcap A;(Modified) Module Phi Identifier;Mean Residual Y",165,0,165);
  RegisterHisto(al_mon,m_pix_eca_yresvsmodphi);  
  m_pix_ecc_xresvsmodphi = new TH1F("pix_ecc_xresvsmodphi","X Residual Mean vs (Modified) Module Phi Pixel Endcap C;(Modified) Module Phi Identifier;Mean Residual X",165,0,165);
  RegisterHisto(al_mon,m_pix_ecc_xresvsmodphi);  
  m_pix_ecc_yresvsmodphi = new TH1F("pix_ecc_yresvsmodphi","Y Residual Mean vs (Modified) Module Phi Pixel Endcap C;(Modified) Module Phi Identifier;Mean Residual Y",165,0,165); 
  RegisterHisto(al_mon,m_pix_ecc_yresvsmodphi); 
  
  if (m_extendedPlots)
    {
      //Unbiased 3D residuals plots
      m_pix_eca_xresvsmodphidisk_3d = new TH3F("pix_eca_xresvsmodphidisk_3d","X Residual Distbn vs Module Eta-Phi-ID Pixel ECA",NDisks,0,NDisks,maxPhiModulesPerRing,0,maxPhiModulesPerRing,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
      RegisterHisto(al_mon,m_pix_eca_xresvsmodphidisk_3d);
      m_pix_eca_yresvsmodphidisk_3d = new TH3F("pix_eca_yresvsmodphidisk_3d","Y Residual Distbn vs Module Eta-Phi-ID Pixel ECA",NDisks,0,NDisks,maxPhiModulesPerRing,0,maxPhiModulesPerRing,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange);
      RegisterHisto(al_mon,m_pix_eca_yresvsmodphidisk_3d);
      m_pix_ecc_xresvsmodphidisk_3d = new TH3F("pix_ecc_xresvsmodphidisk_3d","X Residual Distbn vs Module Eta-Phi-ID Pixel ECC",NDisks,0,NDisks,maxPhiModulesPerRing,0,maxPhiModulesPerRing,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
      RegisterHisto(al_mon,m_pix_ecc_xresvsmodphidisk_3d);
      m_pix_ecc_yresvsmodphidisk_3d = new TH3F("pix_ecc_yresvsmodphidisk_3d","Y Residual Distbn vs Module Eta-Phi-ID Pixel ECC",NDisks,0,NDisks,maxPhiModulesPerRing,0,maxPhiModulesPerRing,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange);
      RegisterHisto(al_mon,m_pix_ecc_yresvsmodphidisk_3d);
      //biased
      m_pix_eca_biased_residualx = new TH1F("pix_eca_biased_residualx","Biased X Residual Pixel EndCap A",100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
      RegisterHisto(al_mon,m_pix_eca_biased_residualx);  
      m_pix_eca_biased_residualy = new TH1F("pix_eca_biased_residualy","Biased Y Residual Pixel EndCap A",100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange);
      RegisterHisto(al_mon,m_pix_eca_biased_residualy);  
      m_pix_ecc_biased_residualx = new TH1F("pix_ecc_biased_residualx","Biased X Residual Pixel EndCap C",100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
      RegisterHisto(al_mon,m_pix_ecc_biased_residualx);  
      m_pix_ecc_biased_residualy = new TH1F("pix_ecc_biased_residualy","Biased Y Residual Pixel EndCap C",100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange);
      RegisterHisto(al_mon,m_pix_ecc_biased_residualy);

      //finely binned residuals
      m_pix_eca_residualx_fine = new TH1F("pix_eca_residualx_fine","UnBiased X Residual Pixel Endcap A;Residual [mm]",2000,-2.0,2.0);
      RegisterHisto(al_mon,m_pix_eca_residualx_fine);  
      m_pix_eca_residualy_fine = new TH1F("pix_eca_residualy_fine","UnBiased Y Residual Pixel Endcap A;Residual [mm]",2000,-2.0,2.0);
      RegisterHisto(al_mon,m_pix_eca_residualy_fine); 
      m_pix_ecc_residualx_fine = new TH1F("pix_ecc_residualx_fine","UnBiased X Residual Pixel Endcap C;Residual [mm]",2000,-2.0,2.0);
      RegisterHisto(al_mon,m_pix_ecc_residualx_fine);  
      m_pix_ecc_residualy_fine = new TH1F("pix_ecc_residualy_fine","UnBiased Y Residual Pixel Endcap C;Residual [mm]",2000,-2.0,2.0);
      RegisterHisto(al_mon,m_pix_ecc_residualy_fine);

      //unbiased pt
      m_pix_eca_residualx_pt = new TH2F("pix_eca_residualx_pt","X Residual Vs Pt Pixel EndCap A;Track pT (GeV);Res (mm)",m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minSiResFillRange,m_maxSiResFillRange);
      RegisterHisto(al_mon,m_pix_eca_residualx_pt);  			
      m_pix_eca_residualy_pt = new TH2F("pix_eca_residualy_pt","Y Residual Vs Pt Pixel EndCap A;Track pT (GeV);Res (mm)",m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minSiResFillRange,m_maxSiResFillRange);
      RegisterHisto(al_mon,m_pix_eca_residualy_pt);
      
      
      m_pix_ecc_residualx_pt = new TH2F("pix_ecc_residualx_pt","X Residual Vs Pt Pixel EndCap C;Track pT (GeV);Res (mm)",m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minSiResFillRange,m_maxSiResFillRange);
      RegisterHisto(al_mon,m_pix_ecc_residualx_pt);        
      m_pix_ecc_residualy_pt = new TH2F("pix_ecc_residualy_pt","Y Residual Vs Pt Pixel EndCap C;Track pT (GeV);Res (mm)",m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minSiResFillRange,m_maxSiResFillRange);
      RegisterHisto(al_mon,m_pix_ecc_residualy_pt);
      //biased pt
      m_pix_eca_biased_residualx_pt = new TH2F("pix_eca_biased_residualx_pt","Biased X Residual Vs Pt Pixel EndCap A;Track pT (GeV); Res (mm)",m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
      RegisterHisto(al_mon,m_pix_eca_biased_residualx_pt);  			
      m_pix_eca_biased_residualy_pt = new TH2F("pix_eca_biased_residualy_pt","Biased Y Residual Vs Pt Pixel EndCap A;Track pT (GeV); Res (mm)",m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange);
      RegisterHisto(al_mon,m_pix_eca_biased_residualy_pt);            			
      m_pix_ecc_biased_residualx_pt = new TH2F("pix_ecc_biased_residualx_pt","Biased X Residual Vs Pt Pixel EndCap C;Track pT (GeV); Res (mm)",m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minPIXResXFillRange,m_maxPIXResXFillRange);
      RegisterHisto(al_mon,m_pix_ecc_biased_residualx_pt);  			
      m_pix_ecc_biased_residualy_pt = new TH2F("pix_ecc_biased_residualy_pt","Biased Y Residual Vs Pt Pixel EndCap A;Track pT (GeV); Res (mm)",m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minPIXResYFillRange,m_maxPIXResYFillRange);
      RegisterHisto(al_mon,m_pix_ecc_biased_residualy_pt);
      //pull vs pt
      m_pix_eca_pullx_pt = new TH2F("pix_eca_pullx_pt","X Pull Vs Pt Pixel EndCap A;Track pT (GeV);X Pull",m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos);
      RegisterHisto(al_mon,m_pix_eca_pullx_pt);  			
      m_pix_eca_pully_pt = new TH2F("pix_eca_pully_pt","Y Pull Vs Pt Pixel EndCap A;Track pT (GeV);Y Pull",m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos);
      RegisterHisto(al_mon,m_pix_eca_pully_pt);
      m_pix_ecc_pullx_pt = new TH2F("pix_ecc_pullx_pt","X Pull Vs Pt Pixel EndCap C;Track pT (GeV);X Pull",m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos);
      RegisterHisto(al_mon,m_pix_ecc_pullx_pt);  			
      m_pix_ecc_pully_pt = new TH2F("pix_ecc_pully_pt","Y Pull Vs Pt Pixel EndCap C;Track pT (GeV);Y Pull",m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos);
      RegisterHisto(al_mon,m_pix_ecc_pully_pt);  
      // Hit Errors
      m_hiterror_x_ibl_b = new TH1F("m_hiterror_x_ibl_b",   "Pixel Barrel X Hit Error;X Hit Error [mm];IBL Hits", 80, 0., 0.1);      
      RegisterHisto(al_mon,m_hiterror_x_ibl_b);
      m_hiterror_x_pix_b = new TH1F("m_hiterror_x_pix_b",   "Pixel Barrel X Hit Error;X Hit Error [mm];PIX Hits", 80, 0., 0.1);
      RegisterHisto(al_mon,m_hiterror_x_pix_b);			
      m_hiterror_x_pix_ec = new TH1F("m_hiterror_x_pix_ec", "Pixel Endcap X Hit Error;X Hit Error [mm];PIX Hits", 80, 0., 0.1);
      RegisterHisto(al_mon,m_hiterror_x_pix_ec);			
      m_hiterror_y_ibl_b = new TH1F("m_hiterror_y_ibl_b",   "Pixel Barrel Y Hit Error;Y Hit Error [mm];IBL Hits", 80, 0., 0.2); 
      RegisterHisto(al_mon,m_hiterror_y_ibl_b);
      m_hiterror_y_pix_b = new TH1F("m_hiterror_y_pix_b",   "Pixel Barrel Y Hit Error;Y Hit Error [mm];PIX Hits", 80, 0., 0.2);
      RegisterHisto(al_mon,m_hiterror_y_pix_b);			
      m_hiterror_y_pix_ec = new TH1F("m_hiterror_y_pix_ec", "Pixel Endcap Y Hit Error;Y Hit Error [mm];PIX Hits", 80, 0., 0.2);
      RegisterHisto(al_mon,m_hiterror_y_pix_ec);
      // hit errors in broad range
      m_hiterror_x_ibl_b_WideRange = new TH1F("m_hiterror_x_ibl_b_WideRange",   "Pixel Barrel X Hit Error;X Hit Error [mm];IBL Hits", 100, 0., 1.0);
      RegisterHisto(al_mon,m_hiterror_x_ibl_b_WideRange);
      m_hiterror_x_pix_b_WideRange = new TH1F("m_hiterror_x_pix_b_WideRange",   "Pixel Barrel X Hit Error;X Hit Error [mm];PIX Hits", 100, 0., 1.0);
      RegisterHisto(al_mon,m_hiterror_x_pix_b_WideRange);
      m_hiterror_x_pix_ec_WideRange = new TH1F("m_hiterror_x_pix_ec_WideRange", "Pixel Endcap X Hit Error;X Hit Error [mm];PIX Hits", 100, 0., 1.0);
      RegisterHisto(al_mon,m_hiterror_x_pix_ec_WideRange);			
      m_hiterror_y_ibl_b_WideRange = new TH1F("m_hiterror_y_ibl_b_WideRange",   "Pixel Barrel Y Hit Error;Y Hit Error [mm];IBL Hits", 100, 0., 2.0);
      RegisterHisto(al_mon,m_hiterror_y_ibl_b_WideRange);
      m_hiterror_y_pix_b_WideRange = new TH1F("m_hiterror_y_pix_b_WideRange",   "Pixel Barrel Y Hit Error;Y Hit Error [mm];PIX Hits", 100, 0., 2.0);
      RegisterHisto(al_mon,m_hiterror_y_pix_b_WideRange);			
      m_hiterror_y_pix_ec_WideRange = new TH1F("m_hiterror_y_pix_ec_WideRange", "Pixel Endcap Y Hit Error;Y Hit Error [mm];PIX Hits", 100, 0., 2.0);
      RegisterHisto(al_mon,m_hiterror_y_pix_ec_WideRange);
    }
  }


void IDAlignMonResiduals::MakeSCTBarrelHistograms(MonGroup& al_mon){
  int totalLayers        = m_SCT_Mgr->numerology().numLayers();
  int totalEtaModules = m_gap_sct*(totalLayers-1);  //to allow separation between layers for profiles
  int totalPhiModules = m_gap_sct*(totalLayers-1);  //to allow separation between layers for profiles
  // vs QoverP2
  Double_t momentum[21] = {15,18,21,25,29,34,40,48,59,77,0,77,59,48,40,34,29,25,21,18,15};
  Double_t customaxis[21];
  for (int i=0; i<=20; i++){
    if(i<10)			
      customaxis[i]=-1/(momentum[i]*momentum[i]);
    else if(i==10)
      customaxis[i]=0;
    else
      customaxis[i]=1/(momentum[i]*momentum[i]);}
  for (int iLayer=0; iLayer < totalLayers;++iLayer) 
    { 
      //ATH_MSG_INFO("iLayer= " << iLayer); 
      if (!m_SCT_Mgr->numerology().useLayer(iLayer)){
	msg(MSG::WARNING) << "Layer "<<iLayer<<" Not Present"<<endreq;
	continue;}
      float maxPhiModulesPerLayer = m_SCT_Mgr->numerology().numPhiModulesForLayer(iLayer);
      totalPhiModules+=maxPhiModulesPerLayer; 
      //just for checking purposes. Not useful. I will cancel it soon. Peo
      for (int iEta = m_SCT_Mgr->numerology().beginEtaModuleForLayer(iLayer); iEta < m_SCT_Mgr->numerology().endEtaModuleForLayer(iLayer);++iEta)
	{
	  if (!iEta && m_SCT_Mgr->numerology().skipEtaZeroForLayer(iLayer))
	    //iEta=0 is not defined for the SCT
	    continue;
	  //ATH_MSG_INFO("iEta= " << iEta);
	}
      m_siliconBarrelLayersLabels.push_back("SCTL"+intToString(iLayer)+" S0");
      m_siliconBarrelLayersLabels.push_back("SCTL"+intToString(iLayer)+" S1");
      float EtaModules= m_SCT_Mgr->numerology().endEtaModuleForLayer(iLayer) - m_SCT_Mgr->numerology().beginEtaModuleForLayer(iLayer); //(i put float in order to divide by 2. I am not skipping the empty layer. It might be nice to see the separation)
      totalEtaModules+=EtaModules;
      //residuals
      m_sct_b_residualsx.push_back(new TH1F(("sct_b"+intToString(iLayer)+"_residualx").c_str(),("Unbiased X Residual SCT Barrel Layer "+intToString(iLayer)+";Residual [mm]").c_str(),100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
      RegisterHisto(al_mon,m_sct_b_residualsx[iLayer]);
      //pulls
      m_sct_b_pullsx.push_back(new TH1F(("sct_b"+intToString(iLayer)+"_pullx").c_str(),("Unbiased X Pull SCT Barrel Layer "+intToString(iLayer)+";Pull").c_str(),100,-m_RangeOfPullHistos,m_RangeOfPullHistos));
      RegisterHisto(al_mon,m_sct_b_pullsx[iLayer]);
      //overlap residuals for SCT
      m_sct_b_xoverlapresidualsx.push_back(new TH1F(("sct_b"+intToString(iLayer)+"_xoverlapresidualx").c_str(),("Unbiased X_Overlap Residual X SCT Barrel Layer "+intToString(iLayer)+";Residual [mm]").c_str(),100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
      RegisterHisto(al_mon,m_sct_b_xoverlapresidualsx[iLayer]);
      m_sct_b_yoverlapresidualsx.push_back(new TH1F(("sct_b"+intToString(iLayer)+"_yoverlapresidualx").c_str(),("Unbiased Y_Overlap Residual X SCT Barrel Layer "+intToString(iLayer)+";Residual [mm]").c_str(),100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
      RegisterHisto(al_mon,m_sct_b_yoverlapresidualsx[iLayer]);
      //3d histograms of residual distribution versus module eta/phi in barrel - eta on xaxis, phi on yaxis
      //Used in post-prcessing to fill 2-d residual map histograms of each silicon layer
      m_sct_b_xresvsmodetaphi_3ds.push_back(new TH3F(("sct_b"+intToString(iLayer)+"_xresvsmodetaphi_3d").c_str(),("X Residual Distbn vs Module Eta-Phi-ID SCT Barrel L"+intToString(iLayer)+";Mod Eta; Mod Phi").c_str(),EtaModules*m_mapSplit,-(EtaModules/2.),(EtaModules/2.),maxPhiModulesPerLayer*m_mapSplit,0,maxPhiModulesPerLayer,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
      RegisterHisto(al_mon,m_sct_b_xresvsmodetaphi_3ds[iLayer]); 
      
      if(m_do3DOverlapHistos) {
	m_sct_b_Oxresxvsmodetaphi_3ds.push_back(new TH3F(("sct_b"+intToString(iLayer)+"_Oxresxvsmodetaphi_3d").c_str(),("X_Overlap Residual X Distbn vs Module Eta-Phi-ID SCT Barrel L"+intToString(iLayer)+";Mod Eta; Mod Phi").c_str(),EtaModules*m_mapSplit,-(EtaModules/2.),(EtaModules/2.),maxPhiModulesPerLayer*m_mapSplit,0,maxPhiModulesPerLayer,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
	RegisterHisto(al_mon,m_sct_b_Oxresxvsmodetaphi_3ds[iLayer]);
	m_sct_b_Oyresxvsmodetaphi_3ds.push_back(new TH3F(("sct_b"+intToString(iLayer)+"_Oyresxvsmodetaphi_3d").c_str(),("Y_Overlap Residual X Distbn vs Module Eta-Phi-ID SCT Barrel L0"+intToString(iLayer)+";Mod Eta; Mod Phi").c_str(),EtaModules*m_mapSplit,-(EtaModules/2.),(EtaModules/2.),maxPhiModulesPerLayer*m_mapSplit,0,maxPhiModulesPerLayer,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
	RegisterHisto(al_mon,m_sct_b_Oyresxvsmodetaphi_3ds[iLayer]);
      }

      /////////////////////////////////////////////////
      ///////////////////DOUBLE CHECK THIS/////////////
      /////////////////////////////////////////////////

      // m_sct_b0_s1_biased_xresvsmodetaphi_3d = new TH3F("sct_b0_s1_biased_xresvsmodetaphi_3d","X Biased Residual Distbn vs Module Eta-Phi-ID SCT Barrel L0 Side1",12*m_mapSplit,-SCTBarrelYSize*6, SCTBarrelYSize*6,32*m_mapSplit,0,32.*SCTBarrelXSize,100,m_minSCTResFillRange,m_maxSCTResFillRange);
      //m_sct_b0_s1_biased_xresvsmodetaphi_3d->SetXTitle("Eta module");								
      //m_sct_b0_s1_biased_xresvsmodetaphi_3d->SetXTitle("Phi module");
      //RegisterHisto(al_mon,m_sct_b0_s1_biased_xresvsmodetaphi_3d);  
      			 
      if (m_extendedPlots){

	  m_sct_b_s0_xresvsmodetaphi_3ds.push_back(new TH3F(("sct_b"+intToString(iLayer)+"_s0_xresvsmodetaphi_3d").c_str(),("X Residual Distbn vs Module Eta-Phi-ID SCT Barrel L"+intToString(iLayer)+" Side 0;Mod Eta; Mod Phi").c_str(),EtaModules,-(EtaModules/2.),(EtaModules/2.),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
      RegisterHisto(al_mon,m_sct_b_s0_xresvsmodetaphi_3ds[iLayer]);

      m_sct_b_s1_xresvsmodetaphi_3ds.push_back(new TH3F(("sct_b"+intToString(iLayer)+"_s1_xresvsmodetaphi_3d").c_str(),("X Residual Distbn vs Module Eta-Phi-ID SCT Barrel L"+intToString(iLayer)+" Side 1;Mod Eta; Mod Phi").c_str(),EtaModules,-(EtaModules/2.),(EtaModules/2.),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
      RegisterHisto(al_mon,m_sct_b_s1_xresvsmodetaphi_3ds[iLayer]);

      //3d histograms of biased residual distribution versus module eta/phi in barrel - eta on xaxis, phi on yaxis, residual in Z    - I am using the mapSplit here!!
      // side 0
      m_sct_b_s0_biased_xresvsmodetaphi_3ds.push_back(new TH3F(("sct_b"+intToString(iLayer)+"_s0_biased_xresvsmodetaphi_3d").c_str(),("Detailed x-residual dist. vs Module Eta-Phi-ID SCT Barrel L"+intToString(iLayer)+" Side0;Eta module;Phi module").c_str(),EtaModules*m_mapSplit,-SCTBarrelYSize*6, SCTBarrelYSize*6,maxPhiModulesPerLayer*m_mapSplit,0,maxPhiModulesPerLayer*SCTBarrelXSize,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
      RegisterHisto(al_mon,m_sct_b_s0_biased_xresvsmodetaphi_3ds[iLayer]);
      // side 1
      m_sct_b_s1_biased_xresvsmodetaphi_3ds.push_back(new TH3F(("sct_b"+intToString(iLayer)+"_s1_biased_xresvsmodetaphi_3d").c_str(),("Detailed x-residual dist. vs Module Eta-Phi-ID SCT Barrel L"+intToString(iLayer)+" Side1;Eta module;Phi module").c_str(),EtaModules*m_mapSplit,-SCTBarrelYSize*6, SCTBarrelYSize*6,maxPhiModulesPerLayer*m_mapSplit,0,maxPhiModulesPerLayer*SCTBarrelXSize,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
      RegisterHisto(al_mon,m_sct_b_s1_biased_xresvsmodetaphi_3ds[iLayer]);

      //unbiased vs pT
      m_sct_b_residualsx_pt.push_back(new TH2F(("sct_b"+intToString(iLayer)+"_residualx_pt").c_str(),("Unbiased X Residual Vs Pt SCT Barrel Layer "+intToString(iLayer)+";Track pT (GeV);SCT Res (mm)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
      RegisterHisto(al_mon,m_sct_b_residualsx_pt[iLayer]);

      //Unbiased vs  qoverp2
      m_sct_b_residualsx_qoverp2.push_back(new TH2F(("sct_b"+intToString(iLayer)+"_unbiased_residualx_qoverp2").c_str(),("Unbiased X Residual Vs Q/P^{2} SCT Barrel Layer "+intToString(iLayer)+";Track Q/P^{2} (GeV^{-2});SCT Res (mm)").c_str(),20,customaxis,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
      RegisterHisto(al_mon,m_sct_b_residualsx_qoverp2[iLayer]);

      //Unbiased Pull vs pt
      m_sct_b_pullsx_pt.push_back(new TH2F(("sct_b"+intToString(iLayer)+"_pullx_pt").c_str(),("X Pull Vs Pt SCT Barrel Layer "+intToString(iLayer)+";Track pT (GeV);SCT Pull").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos));
      RegisterHisto(al_mon,m_sct_b_pullsx_pt[iLayer]);

      //biased
      m_sct_b_biased_residualsx.push_back(new TH1F(("sct_b"+intToString(iLayer)+"_biased_residualx").c_str(),("Biased X Residual SCT Barrel Layer "+intToString(iLayer)+";Residual [mm]").c_str(),100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
      RegisterHisto(al_mon,m_sct_b_biased_residualsx[iLayer]);

      //biased pull
      m_sct_b_biased_pullsx.push_back(new TH1F(("sct_b"+intToString(iLayer)+"_biased_pullx").c_str(),("Biased X Pull SCT Barrel Layer "+intToString(iLayer)).c_str(),100,-m_RangeOfPullHistos, m_RangeOfPullHistos));
      RegisterHisto(al_mon,m_sct_b_biased_pullsx[iLayer]);

      //biased vs  pT
      m_sct_b_biased_residualsx_pt.push_back(new TH2F(("sct_b"+intToString(iLayer)+"_biased_residualx_pt").c_str(),("Biased X Residual Vs Pt SCT Barrel Layer "+intToString(iLayer)+";Track pT (GeV);SCT Res (mm)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
      RegisterHisto(al_mon,m_sct_b_biased_residualsx_pt[iLayer]);

      //biased vs  qoverp2
      m_sct_b_biased_residualsx_qoverp2.push_back(new TH2F(("sct_b"+intToString(iLayer)+"_biased_residualx_qoverp2").c_str(),("Biased X Residual Vs Q/P^{2} SCT Barrel Layer "+intToString(iLayer)+";Track Q/P^{2} (GeV^{-2});SCT Res (mm)").c_str(),20,customaxis,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
      RegisterHisto(al_mon,m_sct_b_biased_residualsx_qoverp2[iLayer]);	 
	  
	  if (m_doClusterSizeHistos)
	    {
	      m_sct_b_residualsx_incidentAnglePhi_s0.push_back(new TH2F(("sct_b"+intToString(iLayer)+"_residualsx_incidentAnglePhi_s0").c_str(),("Residuals X vs Incident Phi Angle SCT Barrel Layer "+intToString(iLayer)+"Incident Phi Angle (rad) Side 0;Local X Residual").c_str(),20,-m_IncidentPhiRange,m_IncidentPhiRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
	      RegisterHisto(al_mon,m_sct_b_residualsx_incidentAnglePhi_s0[iLayer]);
	      m_sct_b_residualsx_incidentAngle_s0.push_back(new TH2F(("sct_b"+intToString(iLayer)+"_residualsx_incidentAngle_s0").c_str(),("Residuals X vs Incident Theta Angle SCT Barrel Layer "+intToString(iLayer)+"Incident Theta Angle (rad) Side 0;Local X Residual").c_str(),20,-m_IncidentThetaRange,m_IncidentThetaRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
	      RegisterHisto(al_mon,m_sct_b_residualsx_incidentAngle_s0[iLayer]);

	      m_sct_b_residualsx_incidentAnglePhi_s1.push_back(new TH2F(("sct_b"+intToString(iLayer)+"_residualsx_incidentAnglePhi_s1").c_str(),("Residuals X vs Incident Phi Angle SCT Barrel Layer "+intToString(iLayer)+"Incident Phi Angle (rad) Side 1;Local X Residual").c_str(),20,-m_IncidentPhiRange,m_IncidentPhiRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
	      RegisterHisto(al_mon,m_sct_b_residualsx_incidentAnglePhi_s1[iLayer]);
	      m_sct_b_residualsx_incidentAngle_s1.push_back(new TH2F(("sct_b"+intToString(iLayer)+"_residualsx_incidentAngle_s1").c_str(),("Residuals X vs Incident Theta Angle SCT Barrel Layer "+intToString(iLayer)+"Incident Theta Angle (rad) Side 1;Local X Residual").c_str(),20,-m_IncidentThetaRange,m_IncidentThetaRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
	      RegisterHisto(al_mon,m_sct_b_residualsx_incidentAngle_s1[iLayer]);

	      
	      m_sct_b_clustersizePhi.push_back(new TH1F(("sct_b"+intToString(iLayer)+"_clustersizePhi").c_str(),("Cluster Phi size SCT Barrel Layer "+intToString(iLayer)).c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange));
	      RegisterHisto(al_mon,m_sct_b_clustersizePhi[iLayer]);
	      m_sct_b_residualsx_clustersizePhi.push_back( new TH2F(("sct_b"+intToString(iLayer)+"_residualx_clustersizePhi").c_str(),("Unbiased X Residual Vs Cluster Phi Size SCT Barrel Layer "+intToString(iLayer)+";Cluster Phi Size;SCT Res (mm)").c_str(),m_ClusterSizeRange+1,-0.5,m_ClusterSizeRange+0.5,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
	      RegisterHisto(al_mon,m_sct_b_residualsx_clustersizePhi[iLayer]);
	      m_sct_b_residualsx_clustersizePhiP.push_back( new TProfile(("sct_b"+intToString(iLayer)+"_residualx_clustersizePhi_p").c_str(),("Unbiased X Residual Vs Cluster Phi Size SCT Barrel Layer "+intToString(iLayer)+";Cluster Phi Size;SCT Res (mm)").c_str(),m_ClusterSizeRange+1,-0.5,m_ClusterSizeRange+0.5,m_minSCTResFillRange,m_maxSCTResFillRange));
	      RegisterHisto(al_mon,m_sct_b_residualsx_clustersizePhiP[iLayer]);
	      m_sct_b_clustersizePhi_incidentAngle.push_back(new TProfile(("sct_b"+intToString(iLayer)+"_clustersizePhi_incidentAngle").c_str(),("Cluster Phi Size vs Incident Theta Angle SCT Barrel Layer "+intToString(iLayer)+"Incident Theta Angle (rad);Cluster Phi Size").c_str(),20,-m_IncidentThetaRange,m_IncidentThetaRange,0,m_ClusterSizeRange));
	      RegisterHisto(al_mon,m_sct_b_clustersizePhi_incidentAngle[iLayer]);
	      m_sct_b_clustersizePhi_incidentAnglePhi.push_back(new TProfile(("sct_b"+intToString(iLayer)+"_clustersizePhi_incidentAnglePhi").c_str(),("Cluster Phi Size vs Incident Phi Angle SCT Barrel Layer "+intToString(iLayer)+"Incident Phi Angle (rad);Cluster Phi Size").c_str(),20,-m_IncidentPhiRange,m_IncidentPhiRange,0,m_ClusterSizeRange));
	      RegisterHisto(al_mon,m_sct_b_clustersizePhi_incidentAnglePhi[iLayer]);

	      m_sct_b_clustersizePhivsmodetaphi_3ds_s0.push_back(new TH3F(("sct_b"+intToString(iLayer)+"_clustersizePhivsmodetaphi_3d_s0").c_str(),("ClustersizePhi Distribution vs Module Eta-Phi-ID SCT Barrel L"+intToString(iLayer)+" Side 0;Mod Eta; Mod Phi").c_str(),EtaModules*m_mapSplit,-(EtaModules/2.),(EtaModules/2.),maxPhiModulesPerLayer*m_mapSplit,0,maxPhiModulesPerLayer,m_ClusterSizeRange+1,-0.5,m_ClusterSizeRange+0.5));
	      RegisterHisto(al_mon,m_sct_b_clustersizePhivsmodetaphi_3ds_s0[iLayer]); 
	      
	      m_sct_b_clustersizePhivsmodetaphi_3ds_s1.push_back(new TH3F(("sct_b"+intToString(iLayer)+"_clustersizePhivsmodetaphi_3d_s1").c_str(),("ClustersizePhi Distribution vs Module Eta-Phi-ID SCT Barrel L"+intToString(iLayer)+" Side 1;Mod Eta; Mod Phi").c_str(),EtaModules*m_mapSplit,-(EtaModules/2.),(EtaModules/2.),maxPhiModulesPerLayer*m_mapSplit,0,maxPhiModulesPerLayer,m_ClusterSizeRange+1,-0.5,m_ClusterSizeRange+0.5));
	      RegisterHisto(al_mon,m_sct_b_clustersizePhivsmodetaphi_3ds_s1[iLayer]); 
	    }
      }
    }//Layer Loop
  
  //Overlap Residuals barrel  - check the number of eta Modules. Total Eta = 15 + 4 * 13 (0 included), Total Phi = 15 + 32+40+48+56
  m_sct_b_Oxresxvsmodeta = new TProfile("sct_b_Oxresxvsmodeta","X_Overlap Residual X Mean vs (Modified) Module Eta SCT Barrel",totalEtaModules,0,totalEtaModules,m_minSCTResFillRange,m_maxSCTResFillRange);
  RegisterHisto(al_mon,m_sct_b_Oxresxvsmodeta);  
  m_sct_b_Oxresxvsmodphi = new TProfile("sct_b_Oxresxvsmodphi","X_Overlap Residual X Mean vs (Modified) Module Phi SCT Barrel",totalPhiModules,0,totalPhiModules,m_minSCTResFillRange,m_maxSCTResFillRange);
  RegisterHisto(al_mon,m_sct_b_Oxresxvsmodphi);  
  m_sct_b_Oyresxvsmodeta = new TProfile("sct_b_Oyresxvsmodeta","Y_Overlap Residual X Mean vs (Modified) Module Eta SCT Barrel",totalEtaModules,0,totalEtaModules,m_minSCTResFillRange,m_maxSCTResFillRange);
  RegisterHisto(al_mon,m_sct_b_Oyresxvsmodeta);  
  m_sct_b_Oyresxvsmodphi = new TProfile("sct_b_Oyresxvsmodphi","Y_Overlap Residual X Mean vs (Modified) Module Phi SCT Barrel",totalPhiModules,0,totalPhiModules,m_minSCTResFillRange,m_maxSCTResFillRange);
  RegisterHisto(al_mon,m_sct_b_Oyresxvsmodphi);

  m_sct_b_residualx = new TH1F("sct_b_residualx","UnBiased X Residual SCT Barrel;Residual [mm]",100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange);
  RegisterHisto(al_mon,m_sct_b_residualx);  
  m_sct_b_biasedresidualx = new TH1F("sct_b_biasedresidualx","Biased X Residual SCT Barrel;Residual [mm]",100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange);
  RegisterHisto(al_mon,m_sct_b_biasedresidualx); 
  
  //These histograms are filled in post-processing - only initiated here such that they can be registered as "shift".    
  //SCT histograms for monitoring residual dependence on module eta/phi (stave/ring)
  m_sct_b_xresvsmodeta = new TH1F("sct_b_xresvsmodeta","X Residual Mean vs (Modified) Module Eta SCT Barrel;(Modified) Module Eta Identifier;Mean Residual X",totalEtaModules,0,totalEtaModules); //67
  RegisterHisto(al_mon,m_sct_b_xresvsmodeta);  
  m_sct_b_xresvsmodphi = new TH1F("sct_b_xresvsmodphi","X Residual Mean vs (Modified) Module Phi SCT Barrel;(Modified) Module Phi Identifier;Mean Residual X",totalPhiModules,0,totalPhiModules); //191
  RegisterHisto(al_mon,m_sct_b_xresvsmodphi); 
  
  if (m_extendedPlots)
    {
      m_sct_b_residualx_fine = new TH1F("sct_b_residualx_fine","UnBiased X Residual SCT Barrel;Residual [mm]",2000,-2.0,2.0);
      RegisterHisto(al_mon,m_sct_b_residualx_fine); 
      // SCT local positions - stereo side
      m_sct_b_extrapolated_st_localx = new TH1F("sct_b_extrapolated_st_localx","SCT Barrel - Stereo Side - Extrapolated Local X;localX (mm)",2000,-(SCTBarrelXSize/2)*1.1,(SCTBarrelXSize/2)*1.1);
      RegisterHisto(al_mon,m_sct_b_extrapolated_st_localx); 
      m_sct_b_extrapolated_st_localy = new TH1F("sct_b_extrapolated_st_localy","SCT Barrel - Stereo Side - Extrapolated Local Y;localY (mm)",2000,-(SCTBarrelYSize/2)*1.1,(SCTBarrelYSize/2)*1.1);
      RegisterHisto(al_mon,m_sct_b_extrapolated_st_localy);
      m_sct_b_measured_st_localx = new TH1F("sct_b_measured_st_localx","SCT Barrel Measured - Stereo Side - Local X;localX (mm)",2000,-(SCTBarrelXSize/2)*1.1,(SCTBarrelXSize/2)*1.1);
      RegisterHisto(al_mon,m_sct_b_measured_st_localx); 
      m_sct_b_measured_st_localy = new TH1F("sct_b_measured_st_localy","SCT Barrel Measured - Stereo Side - Local Y;localY (mm)",2000,-(SCTBarrelYSize/2)*1.1,(SCTBarrelYSize/2)*1.1);
      RegisterHisto(al_mon,m_sct_b_measured_st_localy);
      // SCT local positions - no stereo side
      m_sct_b_extrapolated_nst_localx = new TH1F("sct_b_extrapolated_nst_localx","SCT Barrel - Stereo Side - Extrapolated Local X;localX (mm)",2000,-(SCTBarrelXSize/2)*1.1,(SCTBarrelXSize/2)*1.1);
      RegisterHisto(al_mon,m_sct_b_extrapolated_nst_localx); 
      m_sct_b_extrapolated_nst_localy = new TH1F("sct_b_extrapolated_nst_localy","SCT Barrel - Stereo Side - Extrapolated Local Y;localY (mm)",2000,-(SCTBarrelYSize/2)*1.1,(SCTBarrelYSize/2)*1.1);
      RegisterHisto(al_mon,m_sct_b_extrapolated_nst_localy);
      m_sct_b_measured_nst_localx = new TH1F("sct_b_measured_nst_localx","SCT Barrel Measured - Stereo Side - Local X;localX (mm)",2000,-(SCTBarrelXSize/2)*1.1,(SCTBarrelXSize/2)*1.1);
      RegisterHisto(al_mon,m_sct_b_measured_nst_localx); 
      m_sct_b_measured_nst_localy = new TH1F("sct_b_measured_nst_localy","SCT Barrel Measured - Stereo Side - Local Y;localY (mm)",2000,-(SCTBarrelYSize/2)*1.1,(SCTBarrelYSize/2)*1.1);
      RegisterHisto(al_mon,m_sct_b_measured_nst_localy);
      m_sct_b_biased_residualx_pt = new TH2F("sct_b_biased_residualx_pt","Biased X Residual Vs Pt SCT Barrel;Track pT (GeV); SCT Res (mm)",m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange);
      RegisterHisto(al_mon,m_sct_b_biased_residualx_pt);  
      m_sct_b_residualx_pt = new TH2F("sct_b_residualx_pt","X Residual Vs Pt SCT Barrel;Track pT (GeV); SCT Res (mm)",m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minSiResFillRange,m_maxSiResFillRange);
      RegisterHisto(al_mon,m_sct_b_residualx_pt); 
      m_sct_b_biased_residualx_qoverp2 = new TH2F("sct_b_biased_residualx_qoverp2","Biased X Residual Vs Q/P^{2} SCT Barrel;Track Q/P^{2} (GeV^{-2});SCT Res (mm)",20,customaxis,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange);
      RegisterHisto(al_mon,m_sct_b_biased_residualx_qoverp2); 
      m_sct_b_unbiased_residualx_qoverp2 = new TH2F("sct_b_unbiased_residualx_qoverp2","Unbiased X Residual Vs Q/P^{2} SCT Barrel;Track Q/P^{2} (GeV^{-2});SCT Res (mm)",20,customaxis,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange);
      RegisterHisto(al_mon,m_sct_b_unbiased_residualx_qoverp2);  
      m_sct_b_pullx_pt = new TH2F("sct_b_pullx_pt","X Pull Vs Pt SCT Barrel;Track pT (GeV); SCT Pull",m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos);
      RegisterHisto(al_mon,m_sct_b_pullx_pt); 

      // Hit errors
      m_hiterror_sct_b = new TH1F("m_hiterror_sct_b", "SCT Barrel Hit Error;Hit Error[mm];SCT Hits", 80, 0., 0.25);
      RegisterHisto(al_mon,m_hiterror_sct_b);
      // Hit errors wide range
      m_hiterror_sct_b_WideRange = new TH1F("m_hiterror_sct_b_WideRange", "SCT Barrel Hit Error;Hit Error[mm];SCT Hits", 80, 0., 2.5);
      RegisterHisto(al_mon,m_hiterror_sct_b_WideRange);
    }
  
}


void IDAlignMonResiduals::MakeSCTEndcapsHistograms(MonGroup& al_mon){
  Double_t momentum[21] = {15,18,21,25,29,34,40,48,59,77,0,77,59,48,40,34,29,25,21,18,15};
  Double_t customaxis[21];
  int numDisks        = m_SCT_Mgr->numerology().numDisks();
  int totalPhiModules = 0;
  int totalPhiModulesOuterLayer = 0;
  for (int i=0; i<=20; i++){
    if(i<10)			
      customaxis[i]=-1/(momentum[i]*momentum[i]);
    else if(i==10)
      customaxis[i]=0;
    else
      customaxis[i]=1/(momentum[i]*momentum[i]);
  } 


  for (int iECIndex = 0; iECIndex < m_SCT_Mgr->numerology().numEndcaps(); ++iECIndex)
    {
      totalPhiModules = m_gap_sct * (numDisks - 1); //It is resetted to starting value for ECA. Only ECC modules are counted. This avoid double counting
      totalPhiModulesOuterLayer = m_gap_sct * (numDisks - 1);
      int iSide = m_SCT_Mgr->numerology().endcapId(iECIndex);
      for (int iWheel=0; iWheel < numDisks;++iWheel)
	{
	  int rings=0;
	  rings=m_SCT_Mgr->numerology().numRingsForDisk(iWheel);
	  if (!m_SCT_Mgr->numerology().useDisk(iWheel))
	    {//To check if the Wheel is in use.
	      msg(MSG::WARNING) << "Wheel "<<iWheel<<" Not in Use"<<endreq;
	      continue;}
	  
	  else
	    {
	      //rings=m_SCT_Mgr->numerology().numRingsForDisk(iWheel);
	      if (iSide>0)
		{//I assume symmetry between the two endcaps to avoid double counting
		  m_siliconEndcapLayersLabels.push_back("SCTL"+intToString(iWheel)+" S0");
		  m_siliconEndcapLayersLabels.push_back("SCTL"+intToString(iWheel)+" S1");
		  }
	    }
	  
	  int maxModulesPerRing(0);
	  int ModulesPerRing(0);
	  int maxModulesPerDisk(m_gap_sct*(rings-1));
	  for (int iEta = 0; iEta < rings; ++iEta)   //iEta<m_SCT_Mgr->numerology().numRingsForDisk(iWheel);
	    { 
	      ModulesPerRing = m_SCT_Mgr->numerology().numPhiModulesForDiskRing(iWheel,iEta);
	      if (maxModulesPerRing < ModulesPerRing)
		maxModulesPerRing = ModulesPerRing;
	      //std::cout<< m_SCT_Mgr->numerology().numPhiModulesForDiskRing(iWheel,iEta)<< "  "<<iWheel<<" "<<iEta<<std::endl;
	      //std::cout<<totalPhiModules<<std::endl;
	      totalPhiModules+=ModulesPerRing;
	      maxModulesPerDisk+=ModulesPerRing;
	    }
	  totalPhiModulesOuterLayer+=maxModulesPerRing;
	  if (iSide>0){  	    
	    if (m_extendedPlots)
	      {
		//std::cout<<"Checking rings for each endcap that is created iSide:"<<iSide<< "Wheel: "<<iWheel<<" Rings: "<<rings<<std::endl;
		m_sct_eca_s0_xresvsmodetaphi_3ds.push_back(new TH3F(("sct_eca_d"+intToString(iWheel)+"_s0_xresvsmodetaphi_3d").c_str(),("X UnBiased Residual Distbn vs Module Eta-Phi-ID SCT ECA D"+intToString(iWheel)+" Side0;modEta;modPhi").c_str(),rings,0,rings,maxModulesPerRing,0,maxModulesPerRing,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		RegisterHisto(al_mon,m_sct_eca_s0_xresvsmodetaphi_3ds[iWheel]);

		m_sct_eca_s1_xresvsmodetaphi_3ds.push_back(new TH3F(("sct_eca_d"+intToString(iWheel)+"_s1_xresvsmodetaphi_3d").c_str(),("X UnBiased Residual Distbn vs Module Eta-Phi-ID SCT ECA D"+intToString(iWheel)+" Side1;modEta;modPhi").c_str(),rings,0,rings,maxModulesPerRing,0,maxModulesPerRing,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		RegisterHisto(al_mon,m_sct_eca_s1_xresvsmodetaphi_3ds[iWheel]);
		m_sct_eca_xresvsmodphi_disks.push_back(new TProfile(("sct_eca_unbiased_xresvsmodphi_disk"+intToString(iWheel)).c_str(),("UNBIASED X Residual Average vs Module Phi of SCT Endcap A Disk "+intToString(iWheel)+";Module Phi Identifier;Residual X (mm)").c_str(),maxModulesPerDisk,0,maxModulesPerDisk));
		RegisterHisto(al_mon,m_sct_eca_xresvsmodphi_disks[iWheel]);
		//3d biased res vs mod eta mod phi
		m_sct_eca_s0_biased_xresvsmodetaphi_3ds.push_back(new TH3F(("sct_eca_d"+intToString(iWheel)+"_s0_biased_xresvsmodetaphi_3d").c_str(),("X Biased Residual Distbn vs Module Eta-Phi-ID SCT ECA D"+intToString(iWheel)+" Side0;modEta;modPhi").c_str(),rings,0,rings,maxModulesPerRing,0,maxModulesPerRing,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		RegisterHisto(al_mon,m_sct_eca_s0_biased_xresvsmodetaphi_3ds[iWheel]);
		m_sct_eca_s1_biased_xresvsmodetaphi_3ds.push_back(new TH3F(("sct_eca_d"+intToString(iWheel)+"_s1_biased_xresvsmodetaphi_3d").c_str(),("X Biased Residual Distbn vs Module Eta-Phi-ID SCT ECA D"+intToString(iWheel)+" Side1;modEta;modPhi").c_str(),rings,0,rings,maxModulesPerRing,0,maxModulesPerRing,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		RegisterHisto(al_mon,m_sct_eca_s1_biased_xresvsmodetaphi_3ds[iWheel]);
			//unbiased residual vs pT
		m_sct_eca_residualsx_pt.push_back(new TH2F(("sct_eca_d"+intToString(iWheel)+"_residualx_pt").c_str(),("X Residual Vs Pt SCT Endcap A Disk"+intToString(iWheel)+";Track pT (GeV);SCT Res (mm)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minSiResFillRange,m_maxSiResFillRange)); 
		RegisterHisto(al_mon,m_sct_eca_residualsx_pt[iWheel]);
			//unbiased residual vs QoverP2
		m_sct_eca_residualsx_qoverp2.push_back(new TH2F(("sct_eca_d"+intToString(iWheel)+"_unbiased_residualx_qoverp2").c_str(),("UnBiased X Residual Vs Q/P^{2} SCT Endcap a Disk "+intToString(iWheel)+";Track Q/P^{2} (GeV^{-2});SCT Res (mm)").c_str(),20,customaxis,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange)); 
		RegisterHisto(al_mon,m_sct_eca_residualsx_qoverp2[iWheel]);
			//unbiased pulls vs pt
		m_sct_eca_pullsx_pt.push_back(new TH2F(("sct_eca_d"+intToString(iWheel)+"_pullx_pt").c_str(),("X Pull Vs Pt SCT Endcap A Disk "+intToString(iWheel)+";Track pT (GeV); SCT Pull").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos)); 
		RegisterHisto(al_mon,m_sct_eca_pullsx_pt[iWheel]);
			//SCT endcaps BIASED residuals vs modPhi identifier for each disk
		m_sct_eca_biased_xresvsmodphi_disks.push_back(new TProfile(("sct_eca_BIASED_xresvsmodphi_disk"+intToString(iWheel)).c_str(),("BIASED X Residual Average vs Module Phi of SCT Endcap A Disk "+intToString(iWheel)+";Module Phi Identifier;Residual X (mm)").c_str(),maxModulesPerDisk,0,maxModulesPerDisk));
		RegisterHisto(al_mon,m_sct_eca_biased_xresvsmodphi_disks[iWheel]);
			//biased 
		m_sct_eca_biased_residualsx.push_back(new TH1F(("sct_eca_d"+intToString(iWheel)+"_biased_residualx").c_str(),("Biased X Residual SCT Endcap A Disk "+intToString(iWheel)).c_str(),100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		RegisterHisto(al_mon,m_sct_eca_biased_residualsx[iWheel]);
			//biased vs pT
		m_sct_eca_biased_residualsx_pt.push_back(new TH2F(("sct_eca_d"+intToString(iWheel)+"_biased_residualx_pt").c_str(),("Biased X Residual Vs Pt SCT Endcap A Disk "+intToString(iWheel)+";Track pT (GeV);SCT Res (mm)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		RegisterHisto(al_mon,m_sct_eca_biased_residualsx_pt[iWheel]);
		//biased vs QoverP2
		m_sct_eca_biased_residualsx_qoverp2.push_back(new TH2F(("sct_eca_d"+intToString(iWheel)+"_biased_residualx_qoverp2").c_str(),("Biased X Residual Vs Q/P^{2} SCT Endcap A Disk "+intToString(iWheel)+";Track Q/P^{2} (GeV^{-2});SCT Res (mm)").c_str(),20,customaxis,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		RegisterHisto(al_mon,m_sct_eca_biased_residualsx_qoverp2[iWheel]);
		
		if (m_doClusterSizeHistos)
		  {
		    
		    m_sct_eca_residualsx_incidentAnglePhi_s0.push_back(new TH2F(("sct_eca"+intToString(iWheel)+"_residualsx_incidentAnglePhi_s0").c_str(),("Residuals X vs Incident Phi Angle SCT ECA Disk "+intToString(iWheel)+"Incident Phi Angle (rad) Side 0;Local X Residual").c_str(),20,-m_IncidentPhiRange,m_IncidentPhiRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		    RegisterHisto(al_mon,m_sct_eca_residualsx_incidentAnglePhi_s0[iWheel]);
		    m_sct_eca_residualsx_incidentAngle_s0.push_back(new TH2F(("sct_eca"+intToString(iWheel)+"_residualsx_incidentAngle_s0").c_str(),("Residuals X vs Incident Theta Angle SCT ECA Layer "+intToString(iWheel)+"Incident Theta Angle (rad) Side 1;Local X Residual").c_str(),20,-m_IncidentThetaRange,m_IncidentThetaRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		    RegisterHisto(al_mon,m_sct_eca_residualsx_incidentAngle_s0[iWheel]);


		    m_sct_eca_residualsx_incidentAnglePhi_s1.push_back(new TH2F(("sct_eca"+intToString(iWheel)+"_residualsx_incidentAnglePhi_s1").c_str(),("Residuals X vs Incident Phi Angle SCT ECA Disk "+intToString(iWheel)+"Incident Phi Angle (rad) Side 1;Local X Residual").c_str(),20,-m_IncidentPhiRange,m_IncidentPhiRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		    RegisterHisto(al_mon,m_sct_eca_residualsx_incidentAnglePhi_s1[iWheel]);
		    m_sct_eca_residualsx_incidentAngle_s1.push_back(new TH2F(("sct_eca"+intToString(iWheel)+"_residualsx_incidentAngle_s1").c_str(),("Residuals X vs Incident Theta Angle SCT ECA Layer "+intToString(iWheel)+"Incident Theta Angle (rad) Side 1;Local X Residual").c_str(),20,-m_IncidentThetaRange,m_IncidentThetaRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		    RegisterHisto(al_mon,m_sct_eca_residualsx_incidentAngle_s1[iWheel]);

		    m_sct_eca_clustersizePhi.push_back(new TH1F(("sct_eca_d"+intToString(iWheel)+"_clustersizePhi").c_str(),("Cluster Phi size SCT ECA Disk "+intToString(iWheel)).c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange));
		    RegisterHisto(al_mon,m_sct_eca_clustersizePhi[iWheel]);
		    	      
		    m_sct_eca_residualsx_clustersizePhi.push_back( new TH2F(("sct_eca_d"+intToString(iWheel)+"_residualx_clustersizePhi").c_str(),("Unbiased X Residual Vs Cluster Phi Size SCT ECA Disk "+intToString(iWheel)+";Cluster Phi Size;SCT Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		    RegisterHisto(al_mon,m_sct_eca_residualsx_clustersizePhi[iWheel]);

		    m_sct_eca_residualsx_clustersizePhiP.push_back( new TProfile(("sct_eca_d"+intToString(iWheel)+"_residualx_clustersizePhi_p").c_str(),("Unbiased X Residual Vs Cluster Phi Size SCT ECA Disk "+intToString(iWheel)+";Cluster Phi Size;SCT Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,m_minSCTResFillRange,m_maxSCTResFillRange));
		    RegisterHisto(al_mon,m_sct_eca_residualsx_clustersizePhiP[iWheel]);

		    
		    m_sct_eca_clustersizePhi_incidentAngle.push_back(new TProfile(("sct_eca_d"+intToString(iWheel)+"_clustersizePhi_incidentAngle").c_str(),("Cluster Phi Size Vs Incident Theta Angle SCT ECA Disk "+intToString(iWheel)+"Incident Theta Angle (rad);Cluster Phi Size").c_str(),20,-m_IncidentThetaRange,m_IncidentThetaRange,0,m_ClusterSizeRange));
		    RegisterHisto(al_mon,m_sct_eca_clustersizePhi_incidentAngle[iWheel]);
		    m_sct_eca_clustersizePhi_incidentAnglePhi.push_back(new TProfile(("sct_eca_d"+intToString(iWheel)+"_clustersizePhi_incidentAnglePhi").c_str(),("Cluster Phi Size Vs Incident Phi Angle SCT ECA Disk "+intToString(iWheel)+"Incident Phi Angle (rad);Cluster Phi Size").c_str(),20,-m_IncidentPhiRange,m_IncidentPhiRange,0,m_ClusterSizeRange));
		    RegisterHisto(al_mon,m_sct_eca_clustersizePhi_incidentAnglePhi[iWheel]);
		  }
	      }
	  }
	  else if (iSide<0){
	    if (m_extendedPlots)
	      {
		//std::cout<<"Checking rings for each endcap that is created iSide:"<<iSide<< "Wheel: "<<iWheel<<" Rings: "<<rings<<std::endl;
		m_sct_ecc_s0_xresvsmodetaphi_3ds.push_back(new TH3F(("sct_ecc_d"+intToString(iWheel)+"_s0_xresvsmodetaphi_3d").c_str(),("X UnBiased Residual Distbn vs Module Eta-Phi-ID SCT ECC D"+intToString(iWheel)+" Side0;modEta;modPhi").c_str(),rings,0,rings,maxModulesPerRing,0,maxModulesPerRing,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		RegisterHisto(al_mon,m_sct_ecc_s0_xresvsmodetaphi_3ds[iWheel]);
		m_sct_ecc_s1_xresvsmodetaphi_3ds.push_back(new TH3F(("sct_ecc_d"+intToString(iWheel)+"_s1_xresvsmodetaphi_3d").c_str(),("X UnBiased Residual Distbn vs Module Eta-Phi-ID SCT ECC D"+intToString(iWheel)+" Side1;modEta;modPhi").c_str(),rings,0,rings,maxModulesPerRing,0,maxModulesPerRing,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		RegisterHisto(al_mon,m_sct_ecc_s1_xresvsmodetaphi_3ds[iWheel]);
		m_sct_ecc_xresvsmodphi_disks.push_back(new TProfile(("sct_ecc_unbiased_xresvsmodphi_disk"+intToString(iWheel)).c_str(),("UNBIASED X Residual Average vs Module Phi of SCT Endcap C Disk "+intToString(iWheel)+";Module Phi Identifier;Residual X (mm)").c_str(),maxModulesPerDisk,0,maxModulesPerDisk));
		RegisterHisto(al_mon,m_sct_ecc_xresvsmodphi_disks[iWheel]);   
			//3d biased res vs mod eta mod phi
		m_sct_ecc_s0_biased_xresvsmodetaphi_3ds.push_back(new TH3F(("sct_ecc_d"+intToString(iWheel)+"_s0_biased_xresvsmodetaphi_3d").c_str(),("X Biased Residual Distbn vs Module Eta-Phi-ID SCT ECC D"+intToString(iWheel)+" Side0;modEta;modPhi").c_str(),rings,0,rings,maxModulesPerRing,0,maxModulesPerRing,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		RegisterHisto(al_mon,m_sct_ecc_s0_biased_xresvsmodetaphi_3ds[iWheel]);
		m_sct_ecc_s1_biased_xresvsmodetaphi_3ds.push_back(new TH3F(("sct_ecc_d"+intToString(iWheel)+"_s1_biased_xresvsmodetaphi_3d").c_str(),("X Biased Residual Distbn vs Module Eta-Phi-ID SCT ECC D"+intToString(iWheel)+" Side1;modEta;modPhi").c_str(),rings,0,rings,maxModulesPerRing,0,maxModulesPerRing,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		RegisterHisto(al_mon,m_sct_ecc_s1_biased_xresvsmodetaphi_3ds[iWheel]);
			//unbiased residual vs pT
		m_sct_ecc_residualsx_pt.push_back(new TH2F(("sct_ecc_d"+intToString(iWheel)+"_residualx_pt").c_str(),("X Residual Vs Pt SCT Endcap C Disk"+intToString(iWheel)+";Track pT (GeV);SCT Res (mm)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minSiResFillRange,m_maxSiResFillRange)); 
		RegisterHisto(al_mon,m_sct_ecc_residualsx_pt[iWheel]);
			//unbiased vs Qoverp2
		m_sct_ecc_residualsx_qoverp2.push_back(new TH2F(("sct_ecc_d"+intToString(iWheel)+"_unbiased_residualx_qoverp2").c_str(),("UnBiased X Residual Vs Q/P^{2} SCT Endcap C Disk "+intToString(iWheel)+";Track Q/P^{2} (GeV^{-2});SCT Res (mm)").c_str(),20,customaxis,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange)); 
		RegisterHisto(al_mon,m_sct_ecc_residualsx_qoverp2[iWheel]);
			//unbiased pulls vs pt
		m_sct_ecc_pullsx_pt.push_back(new TH2F(("sct_ecc_d"+intToString(iWheel)+"_pullx_pt").c_str(),("X Pull Vs Pt SCT Endcap C Disk "+intToString(iWheel)+";Track pT (GeV); SCT Pull").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos)); 
		RegisterHisto(al_mon,m_sct_ecc_pullsx_pt[iWheel]);
		//SCT endcaps BIASED residuals vs modPhi identifier for each disk
		m_sct_ecc_biased_xresvsmodphi_disks.push_back(new TProfile(("sct_ecc_BIASED_xresvsmodphi_disk"+intToString(iWheel)).c_str(),("BIASED X Residual Average vs Module Phi of SCT Endcap C Disk "+intToString(iWheel)+";Module Phi Identifier;Residual X (mm)").c_str(),maxModulesPerDisk,0,maxModulesPerDisk));
		RegisterHisto(al_mon,m_sct_ecc_biased_xresvsmodphi_disks[iWheel]);
	
		//biased 
		m_sct_ecc_biased_residualsx.push_back(new TH1F(("sct_ecc_d"+intToString(iWheel)+"_biased_residualx").c_str(),("Biased X Residual SCT Endcap C Disk "+intToString(iWheel)).c_str(),100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		RegisterHisto(al_mon,m_sct_ecc_biased_residualsx[iWheel]);
		//biased vs pT
		m_sct_ecc_biased_residualsx_pt.push_back(new TH2F(("sct_ecc_d"+intToString(iWheel)+"_biased_residualx_pt").c_str(),("Biased X Residual Vs Pt SCT Endcap C Disk "+intToString(iWheel)+";Track pT (GeV);SCT Res (mm)").c_str(),m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		RegisterHisto(al_mon,m_sct_ecc_biased_residualsx_pt[iWheel]);
		//biased vs Qoverp2
		m_sct_ecc_biased_residualsx_qoverp2.push_back(new TH2F(("sct_ecc_d"+intToString(iWheel)+"_biased_residualx_qoverp2").c_str(),("Biased X Residual Vs Q/P^{2} SCT Endcap C Disk "+intToString(iWheel)+";Track Q/P^{2} (GeV^{-2});SCT Res (mm)").c_str(),20,customaxis,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange)); 
		RegisterHisto(al_mon,m_sct_ecc_biased_residualsx_qoverp2[iWheel]);
		if (m_doClusterSizeHistos)
		  {
		    m_sct_ecc_residualsx_incidentAnglePhi_s0.push_back(new TH2F(("sct_ecc"+intToString(iWheel)+"_residualsx_incidentAnglePhi_s0").c_str(),("Residuals X vs Incident Phi Angle SCT ECC Disk "+intToString(iWheel)+"Incident Phi Angle (rad) Side 0;Local X Residual").c_str(),20,-m_IncidentPhiRange,m_IncidentPhiRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		    RegisterHisto(al_mon,m_sct_ecc_residualsx_incidentAnglePhi_s0[iWheel]);
		    m_sct_ecc_residualsx_incidentAngle_s0.push_back(new TH2F(("sct_ecc"+intToString(iWheel)+"_residualsx_incidentAngle_s0").c_str(),("Residuals X vs Incident Theta Angle SCT ECC Layer "+intToString(iWheel)+"Incident Theta Angle (rad) Side 1;Local X Residual").c_str(),20,-m_IncidentThetaRange,m_IncidentThetaRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		    RegisterHisto(al_mon,m_sct_ecc_residualsx_incidentAngle_s0[iWheel]);


		    m_sct_ecc_residualsx_incidentAnglePhi_s1.push_back(new TH2F(("sct_ecc"+intToString(iWheel)+"_residualsx_incidentAnglePhi_s1").c_str(),("Residuals X vs Incident Phi Angle SCT ECC Disk "+intToString(iWheel)+"Incident Phi Angle (rad) Side 1;Local X Residual").c_str(),20,-m_IncidentPhiRange,m_IncidentPhiRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		    RegisterHisto(al_mon,m_sct_ecc_residualsx_incidentAnglePhi_s1[iWheel]);
		    m_sct_ecc_residualsx_incidentAngle_s1.push_back(new TH2F(("sct_ecc"+intToString(iWheel)+"_residualsx_incidentAngle_s1").c_str(),("Residuals X vs Incident Theta Angle SCT ECC Layer "+intToString(iWheel)+"Incident Theta Angle (rad) Side 1;Local X Residual").c_str(),20,-m_IncidentThetaRange,m_IncidentThetaRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		    RegisterHisto(al_mon,m_sct_ecc_residualsx_incidentAngle_s1[iWheel]);
		    		  
		    m_sct_ecc_clustersizePhi.push_back(new TH1F(("sct_ecc_d"+intToString(iWheel)+"_clustersizePhi").c_str(),("Cluster Phi size SCT ECC Disk "+intToString(iWheel)).c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange));
		    RegisterHisto(al_mon,m_sct_ecc_clustersizePhi[iWheel]);
		    		    
		    m_sct_ecc_residualsx_clustersizePhi.push_back( new TH2F(("sct_ecc_d"+intToString(iWheel)+"_residualx_clustersizePhi").c_str(),("Unbiased X Residual Vs Cluster Phi Size SCT ECC Disk "+intToString(iWheel)+";Cluster Phi Size;SCT Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange));
		    RegisterHisto(al_mon,m_sct_ecc_residualsx_clustersizePhi[iWheel]);
		    		    
		    m_sct_ecc_residualsx_clustersizePhiP.push_back( new TProfile(("sct_ecc_d"+intToString(iWheel)+"_residualx_clustersizePhi_p").c_str(),("Unbiased X Residual Vs Cluster Phi Size SCT ECC Disk "+intToString(iWheel)+";Cluster Phi Size;SCT Res (mm)").c_str(),m_ClusterSizeRange,0,m_ClusterSizeRange,m_minSCTResFillRange,m_maxSCTResFillRange));
		    RegisterHisto(al_mon,m_sct_ecc_residualsx_clustersizePhiP[iWheel]);
		    
		    m_sct_ecc_clustersizePhi_incidentAngle.push_back(new TProfile(("sct_ecc_d"+intToString(iWheel)+"_clustersizePhi_incidentAngle").c_str(),("Cluster Phi Size Vs Incident Theta Angle SCT ECC Disk "+intToString(iWheel)+"Incident Theta Angle (rad);Cluster Phi Size").c_str(),20,-m_IncidentThetaRange,m_IncidentThetaRange,0,m_ClusterSizeRange));
		    RegisterHisto(al_mon,m_sct_ecc_clustersizePhi_incidentAngle[iWheel]);
		    m_sct_ecc_clustersizePhi_incidentAnglePhi.push_back(new TProfile(("sct_ecc_d"+intToString(iWheel)+"_clustersizePhi_incidentAnglePhi").c_str(),("Cluster Phi Size Vs Incident Phi Angle SCT ECC Disk "+intToString(iWheel)+"Incident Phi Angle (rad);Cluster Phi Size").c_str(),20,-m_IncidentPhiRange,m_IncidentPhiRange,0,m_ClusterSizeRange));
		    RegisterHisto(al_mon,m_sct_ecc_clustersizePhi_incidentAnglePhi[iWheel]);
		  }
	      }	    
	  }
	  else
	    ATH_MSG_ERROR(" iSide ==0: Creating endcaps histos but obtained a barrel identifier");
	}   
    }
  m_sct_eca_residualx = new TH1F("sct_eca_residualx","UnBiased X Residual SCT EndCap A;Residual [mm]",100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange);
  RegisterHisto(al_mon,m_sct_eca_residualx);  
  m_sct_ecc_residualx = new TH1F("sct_ecc_residualx","UnBiased X Residual SCT EndCap C;Residual [mm]",100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange);
  RegisterHisto(al_mon,m_sct_ecc_residualx);
  m_sct_eca_pullx = new TH1F("sct_eca_pullx","UnBiased X Pull SCT Endcap A",100,-m_RangeOfPullHistos, m_RangeOfPullHistos);
  RegisterHisto(al_mon,m_sct_eca_pullx);  
  m_sct_ecc_pullx = new TH1F("sct_ecc_pullx","UnBiased X Pull SCT Endcap C",100,-m_RangeOfPullHistos, m_RangeOfPullHistos);
  RegisterHisto(al_mon,m_sct_ecc_pullx);  
  //Do I need to separate between s0 and s1?
  //for (std::vector<int>::size_type i=0; i!= m_sct_eca_xresvsmodetaphi_3ds.size(); ++i)
  //RegisterHisto(al_mon,m_sct_eca_xresvsmodetaphi_3ds[i]); Added
  //for (std::vector<int>::size_type i=0; i!= m_sct_ecc_xresvsmodetaphi_3ds.size(); ++i)
  //RegisterHisto(al_mon,m_sct_ecc_xresvsmodetaphi_3ds[i]); Added
  //std::cout<<"totalPhiModules= "<<totalPhiModules<<std::endl;
  m_sct_eca_xresvsmodphi_2d = new TH2F("sct_eca_xresvsmodphi_2d","X Residual Mean vs (Modified) Module Phi SCT Endcap A",totalPhiModulesOuterLayer,0,totalPhiModulesOuterLayer,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange);
  RegisterHisto(al_mon,m_sct_eca_xresvsmodphi_2d);  
  m_sct_ecc_xresvsmodphi_2d = new TH2F("sct_ecc_xresvsmodphi_2d","X Residual Mean vs (Modified) Module Phi SCT Endcap C",totalPhiModulesOuterLayer,0,totalPhiModulesOuterLayer,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange);
  RegisterHisto(al_mon,m_sct_ecc_xresvsmodphi_2d);
  
  m_sct_eca_Oxresxvsmodphi = new TProfile("sct_eca_Oxresxvsmodphi","X_Overlap Residual X Mean vs (Modified) Module Phi SCT Endcap A",totalPhiModulesOuterLayer,0,totalPhiModulesOuterLayer,m_minSCTResFillRange,m_maxSCTResFillRange);
  RegisterHisto(al_mon,m_sct_eca_Oxresxvsmodphi);  
  m_sct_ecc_Oxresxvsmodphi = new TProfile("sct_ecc_Oxresxvsmodphi","X_Overlap Residual X Mean vs (Modified) Module Phi SCT Endcap C",totalPhiModulesOuterLayer,0,totalPhiModulesOuterLayer,m_minSCTResFillRange,m_maxSCTResFillRange);
  RegisterHisto(al_mon,m_sct_ecc_Oxresxvsmodphi);  
  //These histograms are filled in post-processing - only initiated here such that they can be registered as "shift".    
  //residual mean as function module phi in endcaps
  m_sct_eca_xresvsmodphi = new TH1F("sct_eca_xresvsmodphi","X Residual Mean vs (Modified) Module Phi SCT Endcap A;(Modified) Module Phi Identifier;Mean Residual X",totalPhiModulesOuterLayer,0,totalPhiModulesOuterLayer); //495
  RegisterHisto(al_mon,m_sct_eca_xresvsmodphi);  
  m_sct_ecc_xresvsmodphi = new TH1F("sct_ecc_xresvsmodphi","X Residual Mean vs (Modified) Module Phi SCT Endcap C;(Modified) Module Phi Identifier;Mean Residual X",totalPhiModulesOuterLayer,0,totalPhiModulesOuterLayer);
  RegisterHisto(al_mon,m_sct_ecc_xresvsmodphi); 
  if (m_extendedPlots)
    {
      m_sct_eca_residualx_fine = new TH1F("sct_eca_residualx_fine","UnBiased X Residual SCT Endcap A;Residual [mm]",2000,-2.0,2.0);
      RegisterHisto(al_mon,m_sct_eca_residualx_fine);  
      m_sct_ecc_residualx_fine = new TH1F("sct_ecc_residualx_fine","UnBiased X Residual SCT Endcap C;Residual [mm]",2000,-2.0,2.0);
      RegisterHisto(al_mon,m_sct_ecc_residualx_fine);  
      m_sct_eca_biased_residualx = new TH1F("sct_eca_biased_residualx","Biased X Residual SCT Endcap A",100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange);
      RegisterHisto(al_mon,m_sct_eca_biased_residualx);  
      m_sct_ecc_biased_residualx = new TH1F("sct_ecc_biased_residualx","Biased X Residual SCT Endcap C",100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange);
      RegisterHisto(al_mon,m_sct_ecc_biased_residualx);  
      m_sct_eca_biased_residualx_pt = new TH2F("sct_eca_biased_residualx_pt","Biased X Residual Vs Pt SCT Endcap A;Track pT (GeV);SCT Res (mm)",m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange);
      RegisterHisto(al_mon,m_sct_eca_biased_residualx_pt ); 
      m_sct_ecc_biased_residualx_pt = new TH2F("sct_ecc_biased_residualx_pt","Biased X Residual Vs Pt SCT Endcap C;Track pT (GeV);SCT Res (mm)",m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange);
      RegisterHisto(al_mon,m_sct_ecc_biased_residualx_pt );  
      m_sct_eca_residualx_pt = new TH2F("sct_eca_residualx_pt","X Residual Vs Pt SCT Endcap A;Track pT (GeV);SCT Res (mm)",m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minSiResFillRange,m_maxSiResFillRange);
      RegisterHisto(al_mon,m_sct_eca_residualx_pt ); 
      m_sct_ecc_residualx_pt = new TH2F("sct_ecc_residualx_pt","X Residual Vs Pt SCT Endcap C;Track pT (GeV);SCT Res (mm)",m_nBinsPtRange,-m_PtRange,m_PtRange,100*m_FinerBinningFactor,m_minSiResFillRange,m_maxSiResFillRange); 
      RegisterHisto(al_mon,m_sct_ecc_residualx_pt );
      m_sct_eca_biased_residualx_qoverp2 = new TH2F("sct_eca_biased_residualx_qoverp2","Biased X Residual Vs Q/P^{2} SCT Endcap A;Track Q/P^{2} (GeV^{-2});SCT Res (mm)",20,customaxis,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange);
      RegisterHisto(al_mon,m_sct_eca_biased_residualx_qoverp2 ); 
      m_sct_ecc_biased_residualx_qoverp2 = new TH2F("sct_ecc_biased_residualx_qoverp2","Biased X Residual Vs Q/P^{2} SCT Endcap C;Track Q/P^{2} (GeV^{-2});SCT Res (mm)",20,customaxis,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange); 
      RegisterHisto(al_mon,m_sct_ecc_biased_residualx_qoverp2 );  
      m_sct_eca_unbiased_residualx_qoverp2 = new TH2F("sct_eca_unbiased_residualx_qoverp2","Unbiased X Residual Vs Q/P^{2} SCT Endcap A;Track Q/P^{2} (GeV^{-2});SCT Res (mm)",20,customaxis,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange);
      RegisterHisto(al_mon,m_sct_eca_unbiased_residualx_qoverp2 ); 
      m_sct_ecc_unbiased_residualx_qoverp2 = new TH2F("sct_ecc_unbiased_residualx_qoverp2","Unbiased X Residual Vs Q/P^{2} SCT Endcap C;Track Q/P^{2} (GeV^{-2});SCT Res (mm)",20,customaxis,100*m_FinerBinningFactor,m_minSCTResFillRange,m_maxSCTResFillRange); 
      RegisterHisto(al_mon,m_sct_ecc_unbiased_residualx_qoverp2 );
      m_sct_eca_pullx_pt = new TH2F("sct_eca_pullx_pt","X Pull Vs Pt SCT Endcap A;Track pT (GeV);SCT Pull",m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos);
      RegisterHisto(al_mon,m_sct_eca_pullx_pt ); 
      m_sct_ecc_pullx_pt = new TH2F("sct_ecc_pullx_pt","X Pull Vs Pt SCT Endcap C;Track pT (GeV);SCT Pull",m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos); 
      RegisterHisto(al_mon,m_sct_ecc_pullx_pt );
      // hit error
      m_hiterror_sct_ec = new TH1F("m_hiterror_sct_ec", "SCT Endcap Hit Error;Hit Error (mm);SCT Hits",  80, 0., 0.2);
      RegisterHisto(al_mon,m_hiterror_sct_ec);
      // hit error wide range
      m_hiterror_sct_ec_WideRange = new TH1F("m_hiterror_sct_ec_WideRange", "SCT Endcap Hit Error;Hit Error (mm);SCT Hits",  80, 0., 2.0);
      RegisterHisto(al_mon,m_hiterror_sct_ec_WideRange);
    }

}


void IDAlignMonResiduals::MakeSiliconHistograms(MonGroup& al_mon)
{
  
  //Barrel!!
  
  m_si_barrel_resX_mean = new TH1F("si_barrel_resX_mean","Mean Residual X vs Silicon Barrel Layer;;Mean Residual X",m_siliconBarrelLayersLabels.size(),-0.5,m_siliconBarrelLayersLabels.size()-0.5);
  m_si_barrel_resY_mean = new TH1F("si_barrel_resY_mean","Mean Residual Y vs Silicon Barrel Layer;;Mean Residual Y",m_siliconBarrelLayersLabels.size(),-0.5,m_siliconBarrelLayersLabels.size()-0.5);  
  m_si_barrel_pullX_width = new TH1F("si_barrel_pullX_width","Pull X Gaussian Width vs Silicon Barrel Layer;;Pull X Gaussian Width",m_siliconBarrelLayersLabels.size(),-0.5,m_siliconBarrelLayersLabels.size()-0.5);     
  m_si_barrel_pullY_width = new TH1F("si_barrel_pullY_width","Pull Y Gaussian Width vs Silicon Barrel Layer;;Pull Y Gaussian Width",m_siliconBarrelLayersLabels.size(),-0.5,m_siliconBarrelLayersLabels.size()-0.5);     
  m_si_barrel_resX_rms = new TH1F("si_barrel_resX_rms","Residual X RMS vs Silicon Barrel Layer;;Residual X RMS",m_siliconBarrelLayersLabels.size(),-0.5,m_siliconBarrelLayersLabels.size()-0.5);   
  m_si_barrel_resY_rms = new TH1F("si_barrel_resY_rms","Residual Y RMS vs Silicon Barrel Layer;;Residual Y RMS",m_siliconBarrelLayersLabels.size(),-0.5,m_siliconBarrelLayersLabels.size()-0.5);  
  m_si_barrel_resX = new TH2F("si_barrel_resX","Residual X vs Silicon Barrel Layer",m_siliconBarrelLayersLabels.size(),-0.5,m_siliconBarrelLayersLabels.size()-0.5,100*m_FinerBinningFactor,m_minSiResFillRange,m_maxSiResFillRange);  
  m_si_barrel_resY = new TH2F("si_barrel_resY","Residual Y vs Silicon Barrel Layer",m_siliconBarrelLayersLabels.size(),-0.5,m_siliconBarrelLayersLabels.size()-0.5,100*m_FinerBinningFactor,m_minSiResFillRange,m_maxSiResFillRange);
  //Residual Y (only Pixel is filled currently - don't have SCT Y residuals yet)
  m_si_barrel_pullX_mean = new TH1F("si_barrel_pullX_mean","Pull X Gaussian Mean vs Silicon Barrel Layer",m_siliconBarrelLayersLabels.size(),-0.5,m_siliconBarrelLayersLabels.size()-0.5);  m_si_barrel_pullY_mean = new TH1F("si_barrel_pullY_mean","Pull Y Gaussian Mean vs Silicon Barrel Layer",m_siliconBarrelLayersLabels.size(),-0.5,m_siliconBarrelLayersLabels.size()-0.5);  m_si_barrel_pullX = new TH2F("si_barrel_pullX","Pull X vs Silicon Barrel Layer",m_siliconBarrelLayersLabels.size(),-0.5,m_siliconBarrelLayersLabels.size()-0.5,100,-m_RangeOfPullHistos, m_RangeOfPullHistos);  
  m_si_barrel_pullY = new TH2F("si_barrel_pullY","Pull Y vs Silicon Barrel Layer",m_siliconBarrelLayersLabels.size(),-0.5,m_siliconBarrelLayersLabels.size()-0.5,100,-m_RangeOfPullHistos, m_RangeOfPullHistos);  
  
  for (int i=1;i<= (int)m_siliconBarrelLayersLabels.size();++i){
    m_si_barrel_resX_mean->GetXaxis()  ->SetBinLabel(i,m_siliconBarrelLayersLabels[i-1]);
    m_si_barrel_resY_mean->GetXaxis()  ->SetBinLabel(i,m_siliconBarrelLayersLabels[i-1]);
    m_si_barrel_pullX_width->GetXaxis()->SetBinLabel(i,m_siliconBarrelLayersLabels[i-1]);
    m_si_barrel_pullY_width->GetXaxis()->SetBinLabel(i,m_siliconBarrelLayersLabels[i-1]);
    m_si_barrel_resX_rms->GetXaxis()   ->SetBinLabel(i,m_siliconBarrelLayersLabels[i-1]);
    m_si_barrel_resY_rms->GetXaxis()   ->SetBinLabel(i,m_siliconBarrelLayersLabels[i-1]);
    m_si_barrel_resX->GetXaxis()       ->SetBinLabel(i,m_siliconBarrelLayersLabels[i-1]);
    m_si_barrel_resY->GetXaxis()       ->SetBinLabel(i,m_siliconBarrelLayersLabels[i-1]);
    m_si_barrel_pullX_mean->GetXaxis() ->SetBinLabel(i,m_siliconBarrelLayersLabels[i-1]);
    m_si_barrel_pullY_mean->GetXaxis() ->SetBinLabel(i,m_siliconBarrelLayersLabels[i-1]);
    m_si_barrel_pullX->GetXaxis()      ->SetBinLabel(i,m_siliconBarrelLayersLabels[i-1]);
    m_si_barrel_pullY->GetXaxis()      ->SetBinLabel(i,m_siliconBarrelLayersLabels[i-1]);
  }
  RegisterHisto(al_mon,m_si_barrel_resY_rms);   
  RegisterHisto(al_mon,m_si_barrel_resX_rms);
  RegisterHisto(al_mon,m_si_barrel_pullY_width);
  RegisterHisto(al_mon,m_si_barrel_pullX_width);
  RegisterHisto(al_mon,m_si_barrel_resX_mean);
  RegisterHisto(al_mon,m_si_barrel_resY_mean);
  RegisterHisto(al_mon,m_si_barrel_resX);
  RegisterHisto(al_mon,m_si_barrel_resY);
  RegisterHisto(al_mon,m_si_barrel_pullX_mean);
  RegisterHisto(al_mon,m_si_barrel_pullY_mean);
  RegisterHisto(al_mon,m_si_barrel_pullX);
  RegisterHisto(al_mon,m_si_barrel_pullY);
  //EndCaps!  //TO DO
}

void IDAlignMonResiduals::MakeTRTHistograms(MonGroup& al_mon){
  /** Make the barrel hists */
  MakeTRTBarrelHistograms(al_mon);
  /** Make the endcap hists */
  MakeTRTEndcapHistograms(al_mon);
  return;
}

void IDAlignMonResiduals::MakeTRTBarrelHistograms(MonGroup& al_mon){
  /** TRT barrel */
  std::string sideName[3] = {"","Side_A","Side_C"};
	
  for(unsigned int side = 0; side<3; ++side){
    /** Residual in the TRT Barrel */ 
    m_trt_b_hist->residualR[side] = MakeHist("trt_b_residualR"+sideName[side],"UnBiased Residual for the TRT Barrel "+sideName[side],500, m_minTRTResWindow, m_maxTRTResWindow, "Residual (mm)","Entries");
    RegisterHisto(al_mon,m_trt_b_hist->residualR[side]);
    m_trt_b_hist->residualR_notube[side] = MakeHist("trt_b_residualR"+sideName[side]+"_notube","UnBiased Residual (notube) for the TRT Barrel "+sideName[side],500, m_minTRTResWindow, m_maxTRTResWindow, "Residual (mm)","Entries");
    RegisterHisto(al_mon,m_trt_b_hist->residualR_notube[side]);
		
    /** Pull in the TRT Barrel */
    m_trt_b_hist->pullR[side] = MakeHist("trt_b_pullR"+sideName[side],"Pull for the TRT Barrel "+sideName[side],100,-m_RangeOfPullHistos, m_RangeOfPullHistos, "Pull","Entries");
    RegisterHisto(al_mon,m_trt_b_hist->pullR[side]);  
		
    /** Pull in the TRT Barrel */
    m_trt_b_hist->pullR_notube[side] = MakeHist("trt_b_pullR_notube"+sideName[side],"Pull for the TRT Barrel "+sideName[side],100,-m_RangeOfPullHistos, m_RangeOfPullHistos, "Pull","Entries");
    RegisterHisto(al_mon,m_trt_b_hist->pullR_notube[side]);  
		
    /** Measured drift radius */
    m_trt_b_hist->MeasuredR[side] = MakeHist("trt_b_MeasuredR"+sideName[side],"Measured at drift radius for TRT Barrel "+sideName[side],100,-m_RangeOfPullHistos, m_RangeOfPullHistos,"Measured Drift Radius (mm)", "Entries");
    RegisterHisto(al_mon,m_trt_b_hist->MeasuredR[side]);
		
    /** Drift radius from track fit */
    m_trt_b_hist->PredictedR[side] = MakeHist("trt_b_PredictedR"+sideName[side],"Measured drift radius from TRT Barrel "+sideName[side],100,-m_RangeOfPullHistos, m_RangeOfPullHistos,"Predicted Drift Radius (mm)","Entries");
    RegisterHisto(al_mon,m_trt_b_hist->PredictedR[side]);
		
    		
    /** L/R assignment */
    m_trt_b_hist->lr[side] = MakeHist("trt_b_lr"+sideName[side],"|0= LRcor !isTube | 1= LRcor isTube| 2= !LRcor !isTube | 3= !LRcor isTube  "+sideName[side],4,0,4,"","Entries");
    RegisterHisto(al_mon,m_trt_b_hist->lr[side]);  

    if(m_extendedPlots){
      
      /** R(t) relation */
      m_trt_b_hist->rt[side] = MakeHist("trt_b_rt"+sideName[side], "rt relation (Barrel) "+sideName[side], 100,-12.5,59.375,100,0,2,"leading edge [ns] corrected for t0 and EP ","dist from wire to track (mm)");
      RegisterHisto(al_mon,m_trt_b_hist->rt[side]);
    
      
      /**Pull vs mu */
      m_trt_b_hist->pullR_notube_mu[side] = MakeHist("trt_b_pullRnotube_mu_"+sideName[side],"UnBiased Pull vs mu for the TRT Barrel (no tube hits)"+sideName[side],100,0,100,100,-m_RangeOfPullHistos,m_RangeOfPullHistos, "#mu","Pull");
      RegisterHisto(al_mon,m_trt_b_hist->pullR_notube_mu[side]); 
          
      /** Residuals and pulls vs pT*/
      m_trt_b_hist->residualR_pt[side] = MakeHist("trt_b_residualR_pt_"+sideName[side],"UnBiased Residual vs pT for the TRT Barrel "+sideName[side],m_nBinsPtRange,-m_PtRange,m_PtRange,200,-1.0,1.0, "Track p_{T} [GeV]","Residual [mm]");
      RegisterHisto(al_mon,m_trt_b_hist->residualR_pt[side]);  
			
      m_trt_b_hist->pullR_pt[side] = MakeHist("trt_b_pullR_pt_"+sideName[side],"UnBiased Pull vs pT for the TRT Barrel "+sideName[side],m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos, "Track p_{T} [GeV]","Pull");
      RegisterHisto(al_mon,m_trt_b_hist->pullR_pt[side]);  
      
      m_trt_b_hist->pullR_mu[side] = MakeHist("trt_b_pullR_mu_"+sideName[side],"UnBiased Pull vs mu for the TRT Barrel "+sideName[side],100,0,100,100,-m_RangeOfPullHistos,m_RangeOfPullHistos, "#mu","Pull");
      RegisterHisto(al_mon,m_trt_b_hist->pullR_mu[side]);


      m_trt_b_hist->pullR_notube_pt[side] = MakeHist("trt_b_pullRnotube_pt_"+sideName[side],"UnBiased Pull vs pT for the TRT Barrel (no tube hits)"+sideName[side],m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos, "Track p_{T} [GeV]","Pull");
      RegisterHisto(al_mon,m_trt_b_hist->pullR_notube_pt[side]);    
			
      /** Residuals and pulls vs mu*/
      m_trt_b_hist->residualR_mu[side] = MakeHist("trt_b_residualR_mu_"+sideName[side],"UnBiased Residual vs mu for the TRT Barrel "+sideName[side],100,0,100,200,-1.0,1.0, "#mu","Residual [mm]");
      RegisterHisto(al_mon,m_trt_b_hist->residualR_mu[side]);  
    }
		
    /** Over Barrel Layers*/
    for(int lay=0; lay<3; ++lay){
      /** Average Res Vs PhiSector for the 3 Barrel Layers (A and C, A Only, C Only)*/
      m_trt_b_hist->aveRes_l[side][lay] = MakeProfile("trt_b_aveRes_l"+intToString(lay)+sideName[side], "Average Residual vs Phi Sector for TRT Barrel Layer"+intToString(lay)+" "+sideName[side],32,0,32,-1.0, 1.0, "Phi Sector", "Average Residual [mm]", false);
      RegisterHisto(al_mon,m_trt_b_hist->aveRes_l[side][lay]);
      			
      /** Residual RMS Vs PhiSector for the 3 Barrel Layers (A and C, A Only, C Only)*/
      m_trt_b_hist->rmsRes_l[side][lay] = MakeProfile("trt_b_rmsRes_l"+intToString(lay)+sideName[side],"Residual RMS vs Phi Sector for TRT Barrel Layer"+intToString(lay)+" "+sideName[side],32,0,32,0.0, 2.0, "Phi Sector", "Residual RMS");
      RegisterHisto(al_mon,m_trt_b_hist->rmsRes_l[side][lay]);
			
      /** Residual Vs eta (of the track) for the 3 Barrel Layers */
      m_trt_b_hist->aveResVsTrackEta_l[side][lay] = MakeProfile("trt_b_aveResVsTrackEta_l"+intToString(lay)+sideName[side],"Average Residual vs Track eta for TRT Barrel Module Layer "+intToString(lay)+" "+sideName[side],20,-2.5,2.5,-1.0, 1.0,"Track eta","Average Residual [mm]");
      RegisterHisto(al_mon,m_trt_b_hist->aveResVsTrackEta_l[side][lay]);
			
      /** L/R assignment Vs PhiSector for the 3 Barrel Layers (A and C, A Only, C Only)*/
      m_trt_b_hist->lr_l[side][lay] = MakeProfile("trt_b_lr_l"+intToString(lay)+sideName[side],"LR assignment vs Phi Sector for TRT Barrel Layer "+intToString(lay)+" "+sideName[side],32,0,32,0, 1.0,"Phi Sector","fraction of LR assignment correct");
      RegisterHisto(al_mon,m_trt_b_hist->lr_l[side][lay]);

      if(m_extendedPlots){
	/** Residuals vs PhiSector & Z for 3 Barrel Layers (A and C, A Only, C Only)*/
	// it's silly to do side a, and side c for these (since it's a map in Z), but it's how the code is set up. maybe they'll be useful for something...
	m_trt_b_hist->resVsPhiZ[side][lay] = new TH3F(("trt_b_resVsPhiZ_l"+intToString(lay)+sideName[side]).c_str(),("Residual Distribution vs Phi Sector & Z for TRT Barrel Modules in layer "+intToString(lay)+sideName[side]).c_str(),60,-712,712/*size of barrel according to TRT SW people*/,32,0,32,50*m_FinerBinningFactor,m_minTRTResWindow,m_maxTRTResWindow);
	RegisterHisto(al_mon,m_trt_b_hist->resVsPhiZ[side][lay]);

	/** Average residuals vs PhiSector & Z for 3 Barrel Layers (A and C, A Only, C Only)*/
	m_trt_b_hist->aveResVsPhiZ[side][lay] = MakeHist("trt_b_aveResVsPhiZ_l"+intToString(lay)+sideName[side],"Average Residual vs Phi Sector & Z for TRT Barrel Modules in layer "+intToString(lay)+sideName[side],60,-712,712,32,0,32,"z [mm]","#phi Sector");
	RegisterHisto(al_mon,m_trt_b_hist->aveResVsPhiZ[side][lay]);
	
	/** Residual RMS vs PhiSector & Z for 3 Barrel Layers (A and C, A Only, C Only)*/
	m_trt_b_hist->rmsResVsPhiZ[side][lay] = MakeHist("trt_b_rmsResVsPhiZ_l"+intToString(lay)+sideName[side],"Residual RMS vs Phi Sector & Z for TRT Barrel Modules in layer "+intToString(lay)+sideName[side],60,-712,712,32,0,32,"z [mm]","#phi Sector");
	RegisterHisto(al_mon,m_trt_b_hist->rmsResVsPhiZ[side][lay]);
	
	/** Residuals vs PhiSector & Eta for 3 Barrel Layers (A and C, A Only, C Only)*/
	m_trt_b_hist->resVsPhiEta[side][lay] = new TH3F(("trt_b_resVsPhiEta_l"+intToString(lay)+sideName[side]).c_str(),("Residual Distribution vs Phi Sector & Eta for TRT Barrel Modules in layer "+intToString(lay)+sideName[side]).c_str(),60,-1.1,1.1,32,0,32,50*m_FinerBinningFactor,m_minTRTResWindow,m_maxTRTResWindow);
	RegisterHisto(al_mon,m_trt_b_hist->resVsPhiEta[side][lay]);
	
	/** Average residuals vs PhiSector & Eta for 3 Barrel Layers (A and C, A Only, C Only)*/
	m_trt_b_hist->aveResVsPhiEta[side][lay] = MakeHist("trt_b_aveResVsPhiEta_l"+intToString(lay)+sideName[side],"Average Residual vs Phi Sector & Eta for TRT Barrel Modules in layer "+intToString(lay)+sideName[side],60,-1.1,1.1,32,0,32,"#eta","#phi Sector");
	RegisterHisto(al_mon,m_trt_b_hist->aveResVsPhiEta[side][lay]);

	/** Residual RMS vs PhiSector & Eta for 3 Barrel Layers (A and C, A Only, C Only)*/
	m_trt_b_hist->rmsResVsPhiEta[side][lay] = MakeHist("trt_b_rmsResVsPhiEta_l"+intToString(lay)+sideName[side],"Residual RMS vs Phi Sector & Eta for TRT Barrel Modules in layer "+intToString(lay)+sideName[side],60,-1.1,1.1,32,0,32,"#eta","#phi Sector");
	RegisterHisto(al_mon,m_trt_b_hist->rmsResVsPhiEta[side][lay]);
      } // extendedPlots
      
    } 
    
    if (m_extendedPlots)
      {
	/** Over Phi Sectors*/    
	for(unsigned int phiSec=0; phiSec<32; ++phiSec){ 
	  /** Average Res Vs Strawlayer for the 3 Barrel Layers (A and C, A Only, C Only)*/
	  m_trt_b_hist->aveResVsStrawLayerStackLevel[side][phiSec] = MakeProfile("trt_b_aveResVsStrawLayerStackLevel_"+intToString(phiSec)+"_"+sideName[side],"Average Residual vs Straw Layer for TRT Barrel Modules in phi sector "+intToString(phiSec)+" "+sideName[side],73,0,73,-1.0, 1.0,"Straw layer","Average Residual (mm)",false);
	  RegisterHisto(al_mon,m_trt_b_hist->aveResVsStrawLayerStackLevel[side][phiSec]);
	  
	  /** Residual RMS Vs Strawlayer for the 3 Barrel Layers (A and C, A Only, C Only)*/
	  m_trt_b_hist->rmsResVsStrawLayerStackLevel[side][phiSec] = MakeProfile("trt_b_rmsResVsStrawLayerStackLevel_"+intToString(phiSec)+"_"+sideName[side],"Residual RMS vs Straw Layer for TRT Barrel Modules in phi sector "+intToString(phiSec)+" "+sideName[side],73,0,73,-1.0, 1.0,"Straw layer","Residual RMS (mm)");
	  RegisterHisto(al_mon,m_trt_b_hist->rmsResVsStrawLayerStackLevel[side][phiSec]);
	}
      }
    std::string posName[5] = {""," Upper"," Lower"," Left"," Right"};
    /** Average residual and residual RMS Intergrated over each phi module vs straw layer */
    for(unsigned int position=0; position<5; ++position){
      m_trt_b_hist->aveResOverPhiVsStrawLayer[position][side] = MakeProfile("trt_b_aveResOverPhiVsStrawLayer"+sideName[side]+posName[position],"Average Residual (Integrated over Phi) Vs Straw Layer "+sideName[side]+posName[position],73,0,73,-1.0, 1.0,"","",false);
      RegisterHisto(al_mon,m_trt_b_hist->aveResOverPhiVsStrawLayer[position][side]);
			
      m_trt_b_hist->rmsResOverPhiVsStrawLayer[position][side] = MakeProfile("trt_b_rmsResOverPhiVsStrawLayer"+sideName[side]+posName[position],"Residual RMS (Integrated over Phi) Vs Straw Layer "+sideName[side]+posName[position],73,0,73,-1.0, 1.0,"","");
      RegisterHisto(al_mon,m_trt_b_hist->rmsResOverPhiVsStrawLayer[position][side]);
    }
		
    m_trt_b_hist->lrOverPhiVsStrawLayer[side] = MakeProfile("trt_b_lrOverPhiVsStrawLayer"+sideName[side],"Fraction of LR correct (Integrated over Phi) Vs Straw Layer "+sideName[side],73,0,73,0, 1.0,"","",false);
    RegisterHisto(al_mon,m_trt_b_hist->lrOverPhiVsStrawLayer[side]);
		
  }//Over sides
	
  return;
}

void IDAlignMonResiduals::MakeTRTEndcapHistograms(MonGroup& al_mon){
  std::string endcapName[2] = {"Endcap_A","Endcap_C"}; 
	
  /** TRT EndCapC */
  /** Residual in TRT EndcapC */
  for(unsigned int endcap =0; endcap <2; ++endcap){
        
        
    if(m_extendedPlots){
      
      /** Pull noTube vs mu */

      m_trt_ec_hist->pullR_notube_mu[endcap] = MakeHist("trt_ec_pullRnotube_mu_"+endcapName[endcap],"UnBiased Pull vs mu for the TRT Barrel (no tube hits)"+endcapName[endcap],m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos, "#mu","Pull");
      RegisterHisto(al_mon,m_trt_ec_hist->pullR_notube_mu[endcap]);  
    
      
      /** Residuals and pulls vs pT*/
      m_trt_ec_hist->residualR_pt[endcap] = MakeHist("trt_ec_residualR_pt_"+endcapName[endcap],"UnBiased Residual vs pT for the TRT Barrel "+endcapName[endcap],m_nBinsPtRange,-m_PtRange,m_PtRange,200,-1.0,1.0, "Track p_{T} [GeV]","Residual [mm]");
      RegisterHisto(al_mon,m_trt_ec_hist->residualR_pt[endcap]);  
			
      m_trt_ec_hist->pullR_pt[endcap] = MakeHist("trt_ec_pullR_pt_"+endcapName[endcap],"UnBiased Pull vs pT for the TRT Barrel "+endcapName[endcap],m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos, "Track p_{T} [GeV]","Pull");
      RegisterHisto(al_mon,m_trt_ec_hist->pullR_pt[endcap]);  
			
      m_trt_ec_hist->pullR_notube_pt[endcap] = MakeHist("trt_ec_pullRnotube_pt_"+endcapName[endcap],"UnBiased Pull vs pT for the TRT Barrel (no tube hits)"+endcapName[endcap],m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos, "Track p_{T} [GeV]","Pull");
      RegisterHisto(al_mon,m_trt_ec_hist->pullR_notube_pt[endcap]);  

      /** Residuals and pulls vs mu*/
      m_trt_ec_hist->residualR_mu[endcap] = MakeHist("trt_ec_residualR_mu_"+endcapName[endcap],"UnBiased Residual vs mu for the TRT Barrel "+endcapName[endcap],m_nBinsPtRange,-m_PtRange,m_PtRange,200,-1.0,1.0, "#mu ","Residual [mm]");
      RegisterHisto(al_mon,m_trt_ec_hist->residualR_mu[endcap]);  
			
      m_trt_ec_hist->pullR_mu[endcap] = MakeHist("trt_ec_pullR_mu_"+endcapName[endcap],"UnBiased Pull vs mu for the TRT Barrel "+endcapName[endcap],m_nBinsPtRange,-m_PtRange,m_PtRange,100,-m_RangeOfPullHistos,m_RangeOfPullHistos, "#mu","Pull");
      RegisterHisto(al_mon,m_trt_ec_hist->pullR_mu[endcap]);  
    }
	
    if(m_extendedPlots){
      /** Endcap Residual plots vs Phi & Wheel */
      m_trt_ec_hist->resVsPhiWheel[endcap] = new TH3F(("trt_ec_resVsPhiWheel_"+endcapName[endcap]).c_str(),("Residual Distribution vs Phi Sector & Wheel for TRT "+endcapName[endcap]).c_str(),40,0,40,32,0,32,50*m_FinerBinningFactor,m_minTRTResWindow,m_maxTRTResWindow);
      RegisterHisto(al_mon,m_trt_ec_hist->resVsPhiWheel[endcap]);
      m_trt_ec_hist->aveResVsPhiWheel[endcap] = MakeHist("trt_ec_aveResVsPhiWheel_"+endcapName[endcap],"Average Residual vs Phi Sector & Wheel for TRT "+endcapName[endcap],40,0,40,32,0,32,"End-cap 4-plane wheel","#phi Sector");
      RegisterHisto(al_mon,m_trt_ec_hist->aveResVsPhiWheel[endcap]);
      m_trt_ec_hist->rmsResVsPhiWheel[endcap] = MakeHist("trt_ec_rmsResVsPhiWheel_"+endcapName[endcap],"Residual RMS vs Phi Sector & Wheel for TRT "+endcapName[endcap],40,0,40,32,0,32,"End-cap 4-plane wheel","#phi Sector");
      RegisterHisto(al_mon,m_trt_ec_hist->rmsResVsPhiWheel[endcap]);
      
      /** Endcap Residual plots vs Radius & Wheel */
      m_trt_ec_hist->resVsRadiusWheelPos[endcap] = new TH3F(("trt_ec_resVsRadiusWheelPos_"+endcapName[endcap]).c_str(),("Residual Distribution vs Wheel & Radius on Wheel for TRT "+endcapName[endcap]).c_str(),40,0,40,60,644,1004/*these are the radius limits in mm according to TRT SW*/,50*m_FinerBinningFactor,m_minTRTResWindow,m_maxTRTResWindow);
      RegisterHisto(al_mon,m_trt_ec_hist->resVsRadiusWheelPos[endcap]);
      m_trt_ec_hist->aveResVsRadiusWheelPos[endcap] = MakeHist("trt_ec_aveResVsRadiusWheelPos_"+endcapName[endcap],"Average Residual vs Wheel & Radius on Wheel for TRT "+endcapName[endcap],40,0,40,60,644,1004,"Wheel Number","Radius on Wheel [mm]");
      RegisterHisto(al_mon,m_trt_ec_hist->aveResVsRadiusWheelPos[endcap]);
      m_trt_ec_hist->rmsResVsRadiusWheelPos[endcap] = MakeHist("trt_ec_rmsResVsRadiusWheelPos_"+endcapName[endcap],"Residual RMS vs Wheel & Radius on Wheel for TRT "+endcapName[endcap],40,0,40,60,644,1004,"Wheel Number","Radius on Wheel [mm]");
      RegisterHisto(al_mon,m_trt_ec_hist->rmsResVsRadiusWheelPos[endcap]);
      m_trt_ec_hist->resVsRadiusWheelNeg[endcap] = new TH3F(("trt_ec_resVsRadiusWheelNeg_"+endcapName[endcap]).c_str(),("Residual Distribution vs Wheel & Radius on Wheel for TRT "+endcapName[endcap]).c_str(),40,0,40,60,644,1004/*these are the radius limits in mm according to TRT SW*/,50*m_FinerBinningFactor,m_minTRTResWindow,m_maxTRTResWindow);
      RegisterHisto(al_mon,m_trt_ec_hist->resVsRadiusWheelNeg[endcap]);
      m_trt_ec_hist->aveResVsRadiusWheelNeg[endcap] = MakeHist("trt_ec_aveResVsRadiusWheelNeg_"+endcapName[endcap],"Average Residual vs Wheel & Radius on Wheel for TRT "+endcapName[endcap],40,0,40,60,644,1004,"Wheel Number","Radius on Wheel [mm]");
      RegisterHisto(al_mon,m_trt_ec_hist->aveResVsRadiusWheelNeg[endcap]);
      m_trt_ec_hist->rmsResVsRadiusWheelNeg[endcap] = MakeHist("trt_ec_rmsResVsRadiusWheelNeg_"+endcapName[endcap],"Residual RMS vs Wheel & Radius on Wheel for TRT "+endcapName[endcap],40,0,40,60,644,1004,"Wheel Number","Radius on Wheel [mm]");
      RegisterHisto(al_mon,m_trt_ec_hist->rmsResVsRadiusWheelNeg[endcap]);

      /** R(t) relation */
      m_trt_ec_hist->rt[endcap] = MakeHist("trt_ec_rt_"+endcapName[endcap],"rt relation for TRT "+endcapName[endcap], 100,-12.5,59.375,100,0,2,"leading edge [ns] corrected for t0 and EP ","dist from wire to track (mm)");
      RegisterHisto(al_mon,m_trt_ec_hist->rt[endcap]);
    
    
      
    } // extended plots

    unsigned int totalRings= (m_extendedPlots) ? 41 : 1;
    
    for(unsigned int ring=0; ring<totalRings; ++ring){//The "extra one is the total"
      if(!ring){
	m_trt_ec_hist->residualR[endcap][ring] = MakeHist("trt_ec_residualR_"+endcapName[endcap],"UnBiased Residual for TRT "+endcapName[endcap],500,m_minTRTResWindow,m_maxTRTResWindow,"Residual (mm)","Entries");
	RegisterHisto(al_mon,m_trt_ec_hist->residualR[endcap][ring]);  
	m_trt_ec_hist->residualR_notube[endcap][ring] = MakeHist("trt_ec_residualR_notube"+endcapName[endcap],"UnBiased Residual (notube) for TRT "+endcapName[endcap],500,m_minTRTResWindow,m_maxTRTResWindow,"Residual (mm)","Entries");
	RegisterHisto(al_mon,m_trt_ec_hist->residualR_notube[endcap][ring]);  
	m_trt_ec_hist->pullR[endcap][ring] = MakeHist("trt_ec_pullR_"+endcapName[endcap],"Pull for TRT "+endcapName[endcap],100,-m_RangeOfPullHistos, m_RangeOfPullHistos,"Pull","Entries");
	RegisterHisto(al_mon,m_trt_ec_hist->pullR[endcap][ring]);  
	m_trt_ec_hist->pullR_notube[endcap][ring] = MakeHist("trt_ec_pullR_notube_"+endcapName[endcap],"Pull for TRT "+endcapName[endcap],100,-m_RangeOfPullHistos, m_RangeOfPullHistos,"Pull","Entries");
	RegisterHisto(al_mon,m_trt_ec_hist->pullR_notube[endcap][ring]);  
      }else{
	m_trt_ec_hist->residualR[endcap][ring] = MakeHist("trt_ec_residualR_ring_"+intToString(ring-1)+"_"+endcapName[endcap],"UnBiased Residual for TRT "+endcapName[endcap]+" Ring "+intToString(ring-1),100,m_minTRTResWindow,m_maxTRTResWindow,"Residual (mm)","Entries");
	RegisterHisto(al_mon,m_trt_ec_hist->residualR[endcap][ring]);
	m_trt_ec_hist->residualR_notube[endcap][ring] = MakeHist("trt_ec_residualR_ring_"+intToString(ring-1)+"_"+endcapName[endcap]+"_notube","UnBiased Residual (notube) for TRT "+endcapName[endcap]+" Ring "+intToString(ring-1),100,m_minTRTResWindow,m_maxTRTResWindow,"Residual (mm)","Entries");
	RegisterHisto(al_mon,m_trt_ec_hist->residualR_notube[endcap][ring]);
	m_trt_ec_hist->pullR[endcap][ring] = MakeHist("trt_ec_pullR_ring_"+intToString(ring-1)+"_"+endcapName[endcap],"Pull for TRT "+endcapName[endcap]+" Ring "+intToString(ring-1),100,-m_RangeOfPullHistos, m_RangeOfPullHistos,"Pull","Entries");
	RegisterHisto(al_mon,m_trt_ec_hist->pullR[endcap][ring]);  
	m_trt_ec_hist->pullR_notube[endcap][ring] = MakeHist("trt_ec_pullR_notube_ring_"+intToString(ring-1)+"_"+endcapName[endcap],"Pull for TRT "+endcapName[endcap]+" Ring "+intToString(ring-1),100,-m_RangeOfPullHistos, m_RangeOfPullHistos,"Pull","Entries");
	RegisterHisto(al_mon,m_trt_ec_hist->pullR_notube[endcap][ring]);  
      }
			
    }    
		
    /** Measured drift radius */
    m_trt_ec_hist->MeasuredR[endcap] = MakeHist("trt_ec_MeasuredR_"+endcapName[endcap],"Measured at line drift radius from TRT "+endcapName[endcap],100,-m_RangeOfPullHistos, m_RangeOfPullHistos,"Measured Drift Radius (mm)","Entries");
    RegisterHisto(al_mon,m_trt_ec_hist->MeasuredR[endcap]);
		
    /** Drift radius from track fit */
    m_trt_ec_hist->PredictedR[endcap] = MakeHist("trt_ec_PredictedR_"+endcapName[endcap],"Predicted drift radius for TRT "+endcapName[endcap],100,-m_RangeOfPullHistos, m_RangeOfPullHistos, "Predicted Drift Radius (mm)", "Entries");
    RegisterHisto(al_mon,m_trt_ec_hist->PredictedR[endcap]);
		
    /** Average residual and residual RMS vs Ring */
    m_trt_ec_hist->aveResVsRing[endcap] = MakeProfile("trt_ec_aveResVsRing_"+endcapName[endcap],"Average Residual vs Ring for TRT "+endcapName[endcap],40,0,40,-1.0, 1.0,"Endcap Ring","Average Residual [mm]",false);
    RegisterHisto(al_mon,m_trt_ec_hist->aveResVsRing[endcap]);    
		
    m_trt_ec_hist->rmsResVsRing[endcap] = MakeProfile("trt_ec_rmsResVsRing_"+endcapName[endcap],"Residual RMS vs Ring for TRT "+endcapName[endcap],40,0,40,0, 1.0,"Endcap Ring","Residual RMS");
    RegisterHisto(al_mon,m_trt_ec_hist->rmsResVsRing[endcap]);    
		
    /** Average residual and residual RMS vs Phi sector */
    m_trt_ec_hist->aveResVsPhiSec[endcap] = MakeProfile("trt_ec_aveResVsPhiSec_"+endcapName[endcap],"Average Residual vs PhiSec for TRT "+endcapName[endcap],32,0,32,-1.0, 1.0,"Phi Sector","Average Residual [mm]",false);
    RegisterHisto(al_mon,m_trt_ec_hist->aveResVsPhiSec[endcap]);    
		
    m_trt_ec_hist->rmsResVsPhiSec[endcap] = MakeProfile("trt_ec_rmsResVsPhiSec_"+endcapName[endcap],"Residual RMS vs Ring for TRT "+endcapName[endcap],32,0,32,0, 1.0, "Endcap PhiSec","Residual RMS");
    RegisterHisto(al_mon,m_trt_ec_hist->rmsResVsPhiSec[endcap]);    
		
    m_trt_ec_hist->aveResVsTrackEta[endcap] = MakeProfile("trt_ec_aveResVsTrackEta_"+endcapName[endcap],"Average Residual vs Eta for TRT "+endcapName[endcap],20,-2.5,2.5,-1.0, 1.0,"Track Eta","Average Residual [mm]",false);
    RegisterHisto(al_mon,m_trt_ec_hist->aveResVsTrackEta[endcap]);    
  
    /** L/R Assignment */
    m_trt_ec_hist->lr[endcap] = MakeHist("trt_ec_lr_"+endcapName[endcap],"|0= LRcor !isTube | 1= LRcor isTube| 2= !LRcor !isTube | 3= !LRcor isTube "+endcapName[endcap],4,0,4,"","Entries");
    RegisterHisto(al_mon,m_trt_ec_hist->lr[endcap]);  
		
    m_trt_ec_hist->lrVsPhiSec[endcap] = MakeProfile("trt_ec_lrVsPhiSec_"+endcapName[endcap],"LR assignment vs Phi Sector for TRT "+endcapName[endcap],32,0,32,0, 1.0,"Phi Sector","fraction of LR assignment correct");
    RegisterHisto(al_mon,m_trt_ec_hist->lrVsPhiSec[endcap]);
		
    m_trt_ec_hist->lrVsRing[endcap] = MakeProfile("trt_ec_lrVsRing_"+endcapName[endcap],"LR assignment vs Ring for TRT "+endcapName[endcap],40,0,40,0, 1.0,"Ring","fraction of LR assignment correct");
    RegisterHisto(al_mon,m_trt_ec_hist->lrVsRing[endcap]);
  }
	
  return;
}

void IDAlignMonResiduals::fillTRTHistograms(int m_barrel_ec, int m_layer_or_wheel, int m_phi_module, int m_straw_layer,float perdictR, float hitR, float hitZ, float hitGlobalR, float residualR, float pullR, float LE, float EP, float t0, bool isTubeHit ,float trketa, float trkpt, double hweight){
	
  bool LRcorrect = true;
  if( perdictR*hitR < 0)
    LRcorrect = false;
	
  //Need to correct the TRT residual on the C-side.
  if(m_barrel_ec == -1){
    residualR *= -1;
  }
	
  if(m_barrel_ec==1 || m_barrel_ec== -1)
    fillTRTBarrelHistograms(m_barrel_ec
			    ,m_layer_or_wheel
			    ,m_phi_module
			    ,m_straw_layer
			    ,perdictR
			    ,hitR
			    ,hitZ
			    ,residualR
			    ,pullR
			    ,LRcorrect
			    ,LE
			    ,EP
			    ,t0
			    ,isTubeHit
			    ,trketa
			    ,trkpt, hweight);
  	
  /** Filling EndCapA histograms */
  if(m_barrel_ec==2 || m_barrel_ec==-2)
    fillTRTEndcapHistograms(m_barrel_ec
			    ,m_layer_or_wheel
			    ,m_phi_module
			    ,m_straw_layer
			    ,perdictR
			    ,hitR
			    ,hitGlobalR
			    ,residualR
			    ,pullR
			    ,LRcorrect
			    ,LE
			    ,EP
			    ,t0
			    ,isTubeHit
			    ,trketa
			    ,trkpt, hweight);
  	
  return;
}

//Filling barrel histograms
void IDAlignMonResiduals::fillTRTBarrelHistograms(int m_barrel_ec, int m_layer_or_wheel, int m_phi_module, int m_straw_layer,float perdictR, float hitR, float hitZ, float residualR, float pullR, bool LRcorrect, float LE, float EP, float t0, bool isTubeHit ,float trketa, float trkpt, double hweight){
  //Logic to determine Left, Right, Upper, Lower
  bool m_isUpper = (m_phi_module > 4 && m_phi_module < 12 ) ? true : false;
  bool m_isLower = (m_phi_module > 20 && m_phi_module < 28 ) ? true : false;
  bool m_isRight = (m_phi_module <= 4 || m_phi_module >= 28 ) ? true : false;
  bool m_isLeft = (m_phi_module >= 12 && m_phi_module <= 20 ) ? true : false;
	
  //Loop over the barrel sides
  for(unsigned int side = 0; side<3; ++side){
		
    bool doFill = false;
    if(!side)
      doFill = true;
    else if(side == 1 && m_barrel_ec == 1)
      doFill = true;
    else if(side == 2 && m_barrel_ec == -1)
      doFill = true;
		
    if(!doFill)
      continue;
		
    m_trt_b_hist->PredictedR[side]-> Fill(perdictR , hweight);
    m_trt_b_hist->MeasuredR[side] -> Fill(hitR     , hweight);
    m_trt_b_hist->residualR[side] -> Fill(residualR, hweight);
   
    m_trt_b_hist->pullR[side]     -> Fill(pullR    , hweight);
    if (!isTubeHit) {
      m_trt_b_hist->pullR_notube[side] -> Fill(pullR, hweight);
      m_trt_b_hist->residualR_notube[side] -> Fill(residualR,hweight);
    }
    
    		
    if(LRcorrect  && !isTubeHit)      m_trt_b_hist->lr[side] -> Fill(0.5, hweight);
    if(LRcorrect  && isTubeHit )      m_trt_b_hist->lr[side] -> Fill(1.5, hweight);
    if(!LRcorrect && !isTubeHit)      m_trt_b_hist->lr[side] -> Fill(2.5, hweight);
    if(!LRcorrect && isTubeHit )      m_trt_b_hist->lr[side] -> Fill(3.5, hweight);
        
    
    
    if(m_extendedPlots){
      if (LE != -999)
	m_trt_b_hist->rt[side]->Fill(LE - EP - t0,fabs(perdictR), hweight);
      m_trt_b_hist->residualR_pt[side]-> Fill(trkpt, residualR, hweight);
      m_trt_b_hist->pullR_pt[side]    -> Fill(trkpt, pullR    , hweight);
      if (!isTubeHit) m_trt_b_hist->pullR_notube_pt[side] -> Fill(trkpt,pullR, hweight);
      m_trt_b_hist->residualR_mu[side]-> Fill(m_mu, residualR, hweight);
      m_trt_b_hist->pullR_mu[side]    -> Fill(m_mu, pullR    , hweight);
      if (!isTubeHit) m_trt_b_hist->pullR_notube_mu[side] -> Fill(m_mu,pullR, hweight);
    }
    		
    //There are different number of straw layers in the differnt types of module layers
    // and the TRT_Id helper returns the layer with the current module (not global the layer)
    unsigned int numStrawLayers[3] = {0,19,19+24};
		
    for(int lay=0; lay<3; ++lay){
      if(lay == m_layer_or_wheel){
	m_trt_b_hist->lrOverPhiVsStrawLayer[side] -> Fill(numStrawLayers[lay]+m_straw_layer, LRcorrect, hweight);
	if(m_extendedPlots && hitZ!=-999){
	  m_trt_b_hist->resVsPhiZ[side][lay]->Fill(hitZ,m_phi_module,residualR,hweight);
	  m_trt_b_hist->resVsPhiEta[side][lay]->Fill(trketa,m_phi_module,residualR,hweight);
	}
	if (m_extendedPlots)
	  {
	    for(int m_testPhi=0; m_testPhi < 32; ++m_testPhi ){
	      if(m_phi_module == m_testPhi)
		m_trt_b_hist->aveResVsStrawLayerStackLevel[side][m_testPhi] -> Fill(numStrawLayers[lay]+m_straw_layer, residualR, hweight);
	    }
	  }
				
	for(unsigned int position=0; position<5;++position){
	  bool doFillPosition = false;
	  if(!position)
	    doFillPosition = true;
	  else if(position == 1 && m_isUpper)
	    doFillPosition = true;
	  else if(position == 2 && m_isLower)
	    doFillPosition = true;
	  else if(position == 3 && m_isLeft)
	    doFillPosition = true;
	  else if(position == 4 && m_isRight)
	    doFillPosition = true;
					
	  if(!doFillPosition)
	    continue;
					
	  m_trt_b_hist->aveResOverPhiVsStrawLayer[position][side] -> Fill(numStrawLayers[lay]+m_straw_layer, residualR, hweight);
	}
      }
    }//over layers
		
    for(int lay=0; lay<3; ++lay){//Filling layer lay of barrel 
      if(m_layer_or_wheel == lay){
	m_trt_b_hist->aveResVsTrackEta_l[side][lay] -> Fill(trketa,residualR, hweight);
	for(int phi=0; phi<32; ++phi){  //Filling phimodule phi of barrel 
	  if(m_phi_module == phi){
	    m_trt_b_hist->aveRes_l[side][lay]-> Fill(phi, residualR, hweight);
	    m_trt_b_hist->lr_l[side][lay]    -> Fill(phi, LRcorrect, hweight);
	    //m_trt_b_hist->aveResVsStrawLayer[lay][phi]->Fill(m_straw_layer, residualR);
	  }
	}
      }
    }
  }//Over sides
	
  return;
}//fillTRTBarrelHistograms

void IDAlignMonResiduals::fillTRTEndcapHistograms(int m_barrel_ec, int m_layer_or_wheel, int m_phi_module, int m_straw_layer,float perdictR, float hitR, float hitGlobalR, float residualR, float pullR, bool LRcorrect, float LE, float EP, float t0, bool isTubeHit ,float trketa, float trkpt, double hweight){
	
  for(unsigned int endcap=0; endcap<2; ++endcap){
    bool doFill=false;
    if(!endcap && m_barrel_ec == 2)
      doFill = true;
    else if(endcap && m_barrel_ec == -2)
      doFill = true;
		
    if(!doFill)
      continue;
		
    m_trt_ec_hist->PredictedR[endcap]  -> Fill(perdictR , hweight);
    m_trt_ec_hist->MeasuredR[endcap]   -> Fill(hitR     , hweight);
    m_trt_ec_hist->residualR[endcap][0]-> Fill(residualR, hweight);
    m_trt_ec_hist->pullR[endcap][0]    -> Fill(pullR    , hweight);
    if (!isTubeHit) 
      {
	m_trt_ec_hist->pullR_notube[endcap][0]-> Fill(pullR           , hweight); 
	m_trt_ec_hist->residualR_notube[endcap][0] -> Fill(residualR,hweight);
      }
    m_trt_ec_hist->aveResVsTrackEta[endcap] -> Fill(trketa,residualR, hweight);
		
    if(m_extendedPlots){
      if (LE != -999)
	m_trt_ec_hist->rt[endcap] -> Fill(LE - EP - t0,fabs(perdictR), hweight);
      m_trt_ec_hist->residualR_pt[endcap]-> Fill(trkpt, residualR, hweight);
      m_trt_ec_hist->pullR_pt[endcap]    -> Fill(trkpt, pullR    , hweight);
      if (!isTubeHit) m_trt_ec_hist->pullR_notube_pt[endcap] -> Fill(trkpt,pullR, hweight);
      
      m_trt_ec_hist->residualR_mu[endcap]-> Fill(m_mu, residualR, hweight);
      m_trt_ec_hist->pullR_mu[endcap]    -> Fill(m_mu, pullR    , hweight);
      if (!isTubeHit) m_trt_ec_hist->pullR_notube_mu[endcap] -> Fill(m_mu,pullR, hweight);
    }
    
    if(LRcorrect  && !isTubeHit)      m_trt_ec_hist->lr[endcap] -> Fill(0.5, hweight);
    if(LRcorrect  && isTubeHit )      m_trt_ec_hist->lr[endcap] -> Fill(1.5, hweight);
    if(!LRcorrect && !isTubeHit)      m_trt_ec_hist->lr[endcap] -> Fill(2.5, hweight);
    if(!LRcorrect && isTubeHit )      m_trt_ec_hist->lr[endcap] -> Fill(3.5, hweight);
		
    for(int phi=0; phi<32; ++phi){ 
      if(m_phi_module == phi){
	m_trt_ec_hist->aveResVsPhiSec[endcap]-> Fill(phi,residualR, hweight);
	m_trt_ec_hist->lrVsPhiSec[endcap]    -> Fill(phi,LRcorrect, hweight);
      }
    }
		
    // fill TH3F of ave residual vs phi & wheel
    if(m_extendedPlots){
      m_trt_ec_hist->resVsPhiWheel[endcap]->Fill(getRing(m_layer_or_wheel,m_straw_layer),m_phi_module,residualR,hweight);
      // fill TH3F of ave residual vs wheel & radius vs charge & LOW PT ONLY
      if(fabs(trkpt) < m_maxPtEC){
	int charge = (trkpt > 0 ? 1 : -1);
	if (hitGlobalR != -9999)
	  {
	    if(charge > 0){
	      m_trt_ec_hist->resVsRadiusWheelPos[endcap]->Fill(getRing(m_layer_or_wheel,m_straw_layer),hitGlobalR,residualR,hweight);
	    }
	    else{
	      m_trt_ec_hist->resVsRadiusWheelNeg[endcap]->Fill(getRing(m_layer_or_wheel,m_straw_layer),hitGlobalR,residualR,hweight);
	    }
	  }
      }
    }
    
    unsigned int totalRings = (m_extendedPlots) ? 41 : 1;

    for(unsigned int ring=0; ring<totalRings-1; ++ring){
      if(getRing(m_layer_or_wheel,m_straw_layer) == ring){
	m_trt_ec_hist->residualR[endcap][ring+1]-> Fill(residualR, hweight);
	m_trt_ec_hist->pullR[endcap][ring+1]    -> Fill(pullR    , hweight);
	if (!isTubeHit) {
	  m_trt_ec_hist->pullR_notube[endcap][ring+1] -> Fill(pullR, hweight); 
	  m_trt_ec_hist->residualR_notube[endcap][ring+1]-> Fill(residualR, hweight);
	}
	m_trt_ec_hist->aveResVsRing[endcap]-> Fill(ring,residualR, hweight);
	m_trt_ec_hist->lrVsRing[endcap]    -> Fill(ring,LRcorrect, hweight);
      }
    }
  }
	
  return;
}

unsigned int IDAlignMonResiduals::getRing(unsigned int wheel,unsigned int strawlayer){
  if (wheel < 6)
    return (16*wheel + strawlayer) / 4;
  else
    return (16*6+8*(wheel-6) + strawlayer) /4;
}

void IDAlignMonResiduals::MakeStaveShapeFit(float& mag, float& mag_er, float& base, float& base_er, TH1D* projection)
{
  TGraphErrors* g = ConvertHistoInGraph(projection);
  TF1 fit("fit", "[1] - ([2]*(x*x-[0]*[0]))/([0]*[0])",-m_z_fix,m_z_fix);
  //fit = new TF1("fit", "[0]-[1]*(x+[2]) * (4.0*[2]*(x+[2])**2 - (x+[2])**3 - (2.0*[2])**3)", -m_z_fix, m_z_fix );
  
  fit.FixParameter(0, m_z_fix);
  TFitResultPtr r = g->Fit(&fit,"EX0SQ");
  //mag       =  5.0 * fit->GetParameter(1) * (m_z_fix*m_z_fix*m_z_fix*m_z_fix);
  //mag_er    =  5.0 * fit->GetParError(1)  * (m_z_fix*m_z_fix*m_z_fix*m_z_fix);
  
  mag = fit.GetParameter(2);
  mag_er = fit.GetParError(2);

  base     =fit.GetParameter(1);
  base_er  =fit.GetParError(1);
  
  delete g;
  return;
}

TGraphErrors* IDAlignMonResiduals::ConvertHistoInGraph(TH1D* histo)
{
  TGraphErrors* graph = new TGraphErrors();
  std::vector<int> filled_bins;
  for (int etabin=1;etabin < 21;++etabin){
    if (histo->GetBinContent(etabin) != 0.){
      filled_bins.push_back(etabin);}
  }
  for (int ibin=0;ibin < (int) filled_bins.size();++ibin){
    graph->Set(ibin+1);
    graph->SetPoint(ibin,z_axis[filled_bins.at(ibin)-1],histo->GetBinContent(filled_bins.at(ibin)));
    graph->SetPointError(ibin,0,histo->GetBinError(filled_bins.at(ibin)));
  }
  graph->GetXaxis()->SetRangeUser(-m_z_fix,m_z_fix);
  return graph;
}
