/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileInfoLoader
//  Author   : Ed Frank
//  Created  : May, 2002
//
//  DESCRIPTION:
//     Implementation comments only.  Class level comments go in .h file.
//
//  HISTORY:
//     17May02  efrank  First version.
//
//  BUGS:
//
//*****************************************************************************

// Tile includes
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileInfo.h"
#include "TileConditions/TileInfoLoader.h"
#include "TileConditions/TileCablingSvc.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/TilePulseShapes.h"
#include "TileConditions/TileOptFilterWeights.h"

// Calo includes
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/TileTBID.h"
#include "CaloIdentifier/CaloLVL1_ID.h"

// Athena includes
#include "StoreGate/StoreGateSvc.h"
#include "PathResolver/PathResolver.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "AthenaKernel/errorcheck.h"

// Gaudi includes
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"


#include "TMatrixD.h"
#include "cstdio"
#include "cstdlib"
#include <iomanip>
#include <string>
#include <fstream>

//*****************************************************************************
TileInfoLoader::TileInfoLoader(const std::string& name,
			       ISvcLocator* pSvcLocator)
//*****************************************************************************
  : AthService(name, pSvcLocator)
  , m_detStore("DetectorStore", name)
  , m_info(new TileInfo(pSvcLocator))
  , m_pulsevar(new TilePulseShapes())
  , m_OFWeights(new TileOptFilterWeights())
  , m_eorCalled(true)
{

  //==========================================================
  //=== expected number of ADC samples in digitization
  //==========================================================
  declareProperty("NSamples",            m_info->m_nSamples=7,                   "Number of 25ns time slices in TileDigits");
  declareProperty("TrigSample",          m_info->m_iTrigSample=3,                "sample index of t=0 in TileDigits");

  //==========================================================
  //=== Noise and conversion factors used in digitization
  //==========================================================
  declareProperty("TileNoise",           m_info->m_tileNoise=true ,              "true => add noise to TileDigits and TileRawChannels (used in MC generation)");
  declareProperty("TileCoherNoise",      m_info->m_tileCoherNoise=false,         "true => add coherent noise to TileDigits and TileRawChannels (used in MC generation)");
  declareProperty("TileZeroSuppress",    m_info->m_tileZeroSuppress=false,       "true => applies threshold cuts in HitToRC, HitToDigits, RCToCells");
  declareProperty("NoiseScaleIndex",     m_info->m_noiseScaleIndex=1,            "allowed values 0-3 to choose proper ADC->OF noise scale factor for different OF methods");
  declareProperty("ThresholdRawChannel", m_info->m_thresholdRawChannel=-3.0,     "threshold for TileRawChannel [adc] (high gain only, negative -> cut on abs value)");
  declareProperty("ThresholdDigits",     m_info->m_thresholdDigits=-2.0,         "threshold for TileDigits [adc] (high gain only, negative -> cut on abs value)");

  // values for TileCal Level1 Trigger Towers
  declareProperty("TTL1Calib",           m_info->m_ttL1Calib=4.1,                "converts pCb to mV for TTL1");
  declareProperty("TTL1NoiseSigma",      m_info->m_ttL1NoiseSigma=2.5,           "sigma of noise in TTL1 (mV)");
  declareProperty("TTL1Thresh",          m_info->m_ttL1Thresh=5.0,               "threshold cut for TTL1 signals (mV)");
  declareProperty("TTL1Ped",             m_info->m_ttL1Ped=0.0,                  "pedestal for TTL1 signals (mV)");
  declareProperty("TTL1Max",             m_info->m_ttL1Max=1850.0,               "max value for TTL1 signals (mV)");

  // values for TileCal Muon Receiver Boards (!!! TO UPDATE !!! DUMMY Values) ~1mV = 1 ADC count
  declareProperty("MuRcvCalib",           m_info->m_MuRcvCalib      = 14.20,     "converts pCb to ADC counts for MuRcv");
  declareProperty("MuRcvNoiseSigma",      m_info->m_MuRcvNoiseSigma = 2,          "sigma of noise in MuRcv (ADC counts)");
  declareProperty("MuRcvThresh",          m_info->m_MuRcvThresh     = 5,          "threshold cut for MuRcv signals (ADC count)");
  declareProperty("MuRcvPed",             m_info->m_MuRcvPed        = 11.73,      "pedestal for MuRcv signals (ADC count)");
  declareProperty("MuRcvMax",             m_info->m_MuRcvMax        = 255,       "max value for MuRcv signals (ADC count)");

  // values for Level1 Muon output (not used currently)
  declareProperty("MuL1Calib",           m_info->m_muL1Calib=204.2,              "converts pCb to mV for Mu");
  declareProperty("MuL1NoiseSigma",      m_info->m_muL1NoiseSigma=15.6,          "sigma of noise in Mu (mV)");
  declareProperty("MuL1Thresh",          m_info->m_muL1Thresh=5.0,               "threshold cut for Mu signals (mV)");
  declareProperty("MuL1Ped",             m_info->m_muL1Ped=0.0,                  "pedestal for Mu signals (mV)");
  declareProperty("MuL1Max",             m_info->m_muL1Max=1850.0,               "max value for Mu signals (mV)");

  // new values for high gain MBTS output connected to normal trigger tower
  declareProperty("MBTSL1Calib",         m_info->m_mbtsL1Calib=222.0,            "converts pCb to mV for MBTS");
  declareProperty("MBTSL1NoiseSigma",    m_info->m_mbtsL1NoiseSigma=0.6,         "sigma of noise in MBTS (mV)");
  declareProperty("MBTSL1Thresh",        m_info->m_mbtsL1Thresh=5.0,             "threshold cut for MBTS signals (mV)");
  declareProperty("MBTSL1Ped",           m_info->m_mbtsL1Ped=0.0,                "pedestal for MBTS signals (mV)");
  declareProperty("MBTSL1Max",           m_info->m_mbtsL1Max=1850.0,             "max value for MBTS signals (mV)");

  // values for custom TileCal cosmic trigger boards
  declareProperty("TTL1CosmicsGain",     m_info->m_ttL1CosmicsGain=5.0,          "gain of the Cosmics board");
  declareProperty("TTL1CosmicsThresh",   m_info->m_ttL1CosmicsThresh=5.0,        "threshold in the Cosmics board");

  // EM scale for old multiple scattering - default up to release 13.2.0
  //declareProperty("EmScaleA",            m_info->m_emscaleA=35.9,                "conversion from Hit to Cell energy");
  // EM scale for new multiple scattering - default starting from release 14.0.0
  //declareProperty("EmScaleA",            m_info->m_emscaleA=34.3,                "conversion from Hit to Cell energy");
  // EM scale for new multiple scattering - default starting from release 17.0.1

  declareProperty("EmScaleA",            m_info->m_emscaleA=34.0,                "conversion from Hit to Cell energy");

  // new EM scale for special C10 cells starting from release 16.0.0.4

  declareProperty("EmScaleC10sp",        m_info->m_emscaleE[9]=45.,              "conversion from Hit to Cell energy for special C10 cells");

  // new EM scale for gap/crack scintillators starting from release 16.0.0

  declareProperty("EmScaleE1",           m_info->m_emscaleE[10]=125.0,           "conversion from Hit to Cell energy for cells E1 (gap scin)");
  declareProperty("EmScaleE2",           m_info->m_emscaleE[11]=107.0,           "conversion from Hit to Cell energy for cells E2 (gap scin)");
  declareProperty("EmScaleE3",           m_info->m_emscaleE[13]=97.0,            "conversion from Hit to Cell energy for cells E3 (crack scin)");
  declareProperty("EmScaleE4",           m_info->m_emscaleE[15]=75.0,            "conversion from Hit to Cell energy for cells E4 (crack scin)");
  declareProperty("EmScaleMBTSinner",    m_info->m_emscaleMBTS[0]=1.0,           "conversion from Hit to Cell energy for MBTS inner cell");
  declareProperty("EmScaleMBTSouter",    m_info->m_emscaleMBTS[1]=1.0,           "conversion from Hit to Cell energy for MBTS outer cell");
  declareProperty("EmScaleE4prime",      m_info->m_emscaleMBTS[2]=-1.0,          "conversion from Hit to Cell energy for E4' if negative - use the same as for E4");

  // Number of photoelectrons per GeV - used in simulation of photostatistics
  declareProperty("NPhElec",             m_info->m_nPhElec      =  70,           "default number of photo electrons per GeV");
  declareProperty("NPhElecA",            m_info->m_nPhElecVec[0]=  70,           "default number of photo electrons per GeV in A layer");
  declareProperty("NPhElecC",            m_info->m_nPhElecVec[1]=  70,           "default number of photo electrons per GeV in B/C layer");
  declareProperty("NPhElecD",            m_info->m_nPhElecVec[2]=  70,           "default number of photo electrons per GeV in D layer");
  declareProperty("NPhElecE",            m_info->m_nPhElecVec[3]=  70,           "default number of photo electrons per GeV in E layer");
  declareProperty("NPhElecMBTSinner",    m_info->m_nPhElecVec[4]=1500,           "default number of photo electrons per GeV in MBTS, inner cell");
  declareProperty("NPhElecMBTSouter",    m_info->m_nPhElecVec[5]=1070,           "default number of photo electrons per GeV in MBTS, outer cell");
  declareProperty("NPhElecE4prime",      m_info->m_nPhElecVec[6]=  -1,           "default number of photo electrons per GeV in E4' cell, if negative - use the same as for E4");

  //==========================================================
  //=== Digits pulse shapes
  //==========================================================
  declareProperty("DigitsShapeFileHi",      m_digitsShapeFileHi = "pulsehi_physics.dat",     "Filename for high gain pulse shape");
  declareProperty("DigitsShapeFileLo",      m_digitsShapeFileLo = "pulselo_physics.dat",     "Filename for low gain pulse shape");
  //===Trigger Tower Pulse shapes
  declareProperty("filename_TTL1ShapeFile"  ,m_TTL1ShapeFile   = "pulse_adder_tower_physics.dat", "Filename for trigger tower pulse shape");
  declareProperty("filename_MuRcvShapeFile" ,m_MuRcvShapeFile  = "pulse_adder_muonRcv_physics.dat", "Filename for Muon Receiver pulse shape"); // using the sames as for TTL1ShapeFile !!! TO UPDATE !!!
  declareProperty("filename_MuL1ShapeFile"  ,m_MuL1ShapeFile   = "pulse_adder_muon_physics.dat",  "Filename for muon level1 trigger pulse shape");

  //==========================================================
  //=== Reference pulseshapes
  //==========================================================
  //===CIS Pulse shapes and derivatives
  declareProperty("filename_lo_cis"      ,m_pulsevar->m_filenameLoCIS    = "pulselo_cis_100.dat"  ,"Low gain CIS pulseshape 100pF");
  declareProperty("filename_lo_cis_der"  ,m_pulsevar->m_filenameLoCISDer = "dpulselo_cis_100.dat" ,"Low gain CIS pulseshape derivative 100pF");
  declareProperty("filename_slo_cis"     ,m_pulsevar->m_filenameSLoCIS   = "pulselo.0230055.12.cap.5.2.dat"  ,"Low gain CIS pulseshape 5pF");
  declareProperty("filename_slo_cis_der" ,m_pulsevar->m_filenameSLoCISDer= "dpulselo.0230055.12.cap.5.2.dat" ,"Low gain CIS pulseshape derivative pF");
  declareProperty("filename_hi_cis"      ,m_pulsevar->m_filenameHiCIS    = "pulsehi_cis_100.dat"  ,"High gain CIS pulseshape 100pF");
  declareProperty("filename_hi_cis_der"  ,m_pulsevar->m_filenameHiCISDer = "dpulsehi_cis_100.dat" ,"High gain CIS pulseshape derivative 100pF");
  declareProperty("filename_shi_cis"     ,m_pulsevar->m_filenameSHiCIS   = "pulsehi.0230055.12.cap.5.2.dat"  ,"High gain CIS pulseshape 5pF");
  declareProperty("filename_shi_cis_der" ,m_pulsevar->m_filenameSHiCISDer= "dpulsehi.0230055.12.cap.5.2.dat" ,"High gain CIS pulseshape derivative 5pF");
  //===Physics Pulse shapes and derivatives
  declareProperty("filename_lo_phys"    ,m_pulsevar->m_filenameLoPhys    = "pulselo_physics.dat"  ,"Low gain physics pulseshape");
  declareProperty("filename_hi_phys"    ,m_pulsevar->m_filenameHiPhys    = "pulsehi_physics.dat"  ,"Low gain physics pulseshape derivative");
  declareProperty("filename_lo_phys_der",m_pulsevar->m_filenameLoPhysDer = "dpulselo_physics.dat" ,"High gain physics pulseshape");
  declareProperty("filename_hi_phys_der",m_pulsevar->m_filenameHiPhysDer = "dpulsehi_physics.dat" ,"High gain physics pulseshape derivative");
  //=== Laser Pulse shapes and derivatives
  declareProperty("filename_lo_las"     ,m_pulsevar->m_filenameLoLas     = "pulselo_laser.dat"  ,"Low gain laser pulseshape");
  declareProperty("filename_hi_las"     ,m_pulsevar->m_filenameHiLas     = "pulsehi_laser.dat"  ,"Low gain laser pulseshape derivative");
  declareProperty("filename_lo_las_der" ,m_pulsevar->m_filenameLoLasDer  = "dpulselo_laser.dat" ,"High gain laser pulseshape");
  declareProperty("filename_hi_las_der" ,m_pulsevar->m_filenameHiLasDer  = "dpulsehi_laser.dat" ,"High gain laser pulseshape derivative");
  //=== CIS Leakage Pulse shapes and derivatives
  declareProperty("filename_sleaklo"    ,m_pulsevar->m_filenameSLeakLo   = "leaklo.0230055.12.cap.5.2.dat" ,"Low gain CIS leakage pulse 5pF");
  declareProperty("filename_leaklo"     ,m_pulsevar->m_filenameLeakLo    = "leaklo_100.dat"  ,"Low gain CIS leakage pulse 100pF");
  declareProperty("filename_dsleaklo"   ,m_pulsevar->m_filenameDSLeakLo  = "dsleaklo.0230055.12.cap.5.2.dat","Derivative low gain CIS leakage pulse 5pF");
  declareProperty("filename_dleaklo"    ,m_pulsevar->m_filenameDLeakLo   = "dleaklo_100.dat" ,"Derivative low gain CIS leakage pulse 100pF");
  declareProperty("filename_sleakhi"    ,m_pulsevar->m_filenameSLeakHi   = "leakhi.0230055.12.cap.5.2.dat" ,"High gain CIS leakage pulse 5pF");
  declareProperty("filename_leakhi"     ,m_pulsevar->m_filenameLeakHi    = "leakhi_100.dat"  ,"High gain CIS leakage pulse 100pF");
  declareProperty("filename_dsleakhi"   ,m_pulsevar->m_filenameDSLeakHi  = "dsleakhi.0230055.12.cap.5.2.dat","Derivative high gain CIS leakage pulse 5pF");
  declareProperty("filename_dleakhi"    ,m_pulsevar->m_filenameDLeakHi   = "dleakhi_100.dat" ,"Derivative high gain CIS leakage pulse 100pF");
  //=== Digitizer noise RMS as a function of channel number
  declareProperty("filename_orig_noise" ,m_pulsevar->m_filenameOrigNoise = "noise_digi_orig.dat" ,"file with noise rms per channel");
  declareProperty("filename_nk_noise"   ,m_pulsevar->m_filenameNkNoise   = "noise_digi_nk.dat"   ,"file with noise rms per channel");

  //==========================================================
  //=== Covariance matrices for noise simulation
  //==========================================================
  declareProperty("filename_DecoCovaFilePrefix" ,m_DecoCovaFilePrefix  = "DecoCovaMatrix");

  //==========================================================
  //=== TileOptFilterWeights configuration
  //==========================================================
  declareProperty("LoadOptFilterWeights"       ,m_loadOptFilterWeights          = false);
  declareProperty("OFPhysicsNSamples"          ,m_OFWeights->m_NSamples_Phys    = 7);
  declareProperty("filenameNoiseCISSuffix"     ,m_OFWeights->m_noiseCISSuffix     = "510082_CIS");
  declareProperty("filenameNoisePhysicsSuffix" ,m_OFWeights->m_noisePhysicsSuffix = "520020_Phys");
  declareProperty("filenameDeltaCISSuffix"     ,m_OFWeights->m_deltaCISSuffix     = "of2_Delta_CIS_7Samples");
  declareProperty("filenameDeltaPhysicsSuffix" ,m_OFWeights->m_deltaPhysicsSuffix = "of2_Delta_Phys_7Samples");
  declareProperty("LoadOptFilterCorrelation"   ,m_loadOptFilterCorrelation      = false);
  declareProperty("filenameNoiseCorrSuffix"    ,m_OFWeights->m_noiseCorrSuffix    = "520020_Phys");
  declareProperty("filenameDeltaCorrSuffix"    ,m_OFWeights->m_deltaCorrSuffix    = "Delta_Phys_9Samples");
  declareProperty("DeltaConf"                  ,m_OFWeights->m_DeltaConf        = true);
}

//*****************************************************************************
TileInfoLoader::~TileInfoLoader() {
//*****************************************************************************
  delete m_pulsevar;
  delete m_OFWeights;
  if (m_eorCalled) delete m_info;
}

//*****************************************************************************
StatusCode TileInfoLoader::initialize() {
//*****************************************************************************

  ATH_MSG_INFO( "Initializing...." << name() );

  m_OFWeights->m_NSamples_Phys = m_info->m_nSamples; // to make sure that everything is consistent

  //=== Find the detector store service.
  CHECK( m_detStore.retrieve() );

  // initialize sampling fraction for all C10 cells to default value
  for (int i=0; i<64; ++i) {
    m_info->m_emscaleC[i] =  m_info->m_emscaleA;
  }

  const IGeoModelSvc *geoModel=0;

  if(service("GeoModelSvc", geoModel).isFailure()) {
    ATH_MSG_ERROR( "Could not locate GeoModelSvc" );
  } else {
    // check the DetDescr version
    std::string atlasVersion = geoModel->atlasVersion();
    int geo  = atlasVersion.compare(0,9,"ATLAS-GEO");
    int run1 = atlasVersion.compare(0,8,"ATLAS-R1");
    int ibl  = atlasVersion.compare(0,9,"ATLAS-IBL");
    int run2 = atlasVersion.compare(0,8,"ATLAS-R2");
    int comm = atlasVersion.compare(0,10,"ATLAS-Comm");
    int slhc = atlasVersion.compare(0,10,"ATLAS-SLHC") ;
    int upg  = atlasVersion.compare(0,7,"ATLAS-P") ;
    int ver = 0;

    bool nothing_found = (geo*run1*ibl*run2*comm*slhc*upg != 0);
    GeoModel::GeoConfig geoConfig = geoModel->geoConfig();
    bool RUN2 = (nothing_found && (geoConfig==GeoModel::GEO_RUN2
                                   || geoConfig==GeoModel::GEO_RUN3
                                   || geoConfig==GeoModel::GEO_RUN4
                                   || geoConfig==GeoModel::GEO_ITk
                     )) || (run2 == 0); // calibration for all geometries >=RUN2 are the same as in RUN2

    if (geo == 0 || comm == 0 || ibl == 0 || slhc == 0 || run1 == 0 || run2 == 0 || upg == 0 || RUN2) {
      int pos = (atlasVersion.substr(9)).find("-");
      if(run1 == 0 || run2 == 0) pos = (atlasVersion.substr(13)).find("-") + 4;
      std::string geoVersion = atlasVersion.substr(pos+10,2)
                             + atlasVersion.substr(pos+13,2)
                             + atlasVersion.substr(pos+16,2);
      ver = atoi(geoVersion.c_str());
      ATH_MSG_INFO( "New ATLAS geometry detected: " << atlasVersion << " (" << geoVersion << ")"  << " version " << ver  );

      if (fabs(m_info->m_ttL1Calib - 4.1) < 0.001) {
        // They are TTL1Calib = 4.1 and TTL1NoiseSigma = 2.5 but should be 6.9 and 2.8 respectively.

        msg(MSG::INFO) << "Changing TTL1 calib from " << m_info->m_ttL1Calib;
        m_info->m_ttL1Calib = 6.9;
        msg(MSG::INFO) << " to " << m_info->m_ttL1Calib << endmsg;

        msg(MSG::INFO) << "Changing TTL1 noise sigma from " << m_info->m_ttL1NoiseSigma;
        m_info->m_ttL1NoiseSigma = 2.8;
        msg(MSG::INFO) << " to " << m_info->m_ttL1NoiseSigma << endmsg;
      }
    } else {
      ATH_MSG_INFO( "ATLAS geometry " << atlasVersion << " version " << ver );
    }

    if (m_info->m_emscaleA < 0.0) { // negative value from jobOptions
      m_info->m_emscaleA = -m_info->m_emscaleA; // convert to positive value and use it without any other checks
      ATH_MSG_INFO("Sampling fraction in jobOptions is negative - use absolute value of it");
    } else {
      if (geo == 0 && ver < 180000) { // GEO versions used for MC08,MC09,MC10 - use old SF by default
        if (fabs(m_info->m_emscaleA - 34.3) > 0.001) {
          m_info->m_emscaleA = 34.3;
          ATH_MSG_INFO("ATLAS geometry " << ver << " < 180000 - use old default sampling fraction");
          ATH_MSG_INFO("If you want to override old default, put negative value in jobOptions");
        }
      }
    }

    if (ver >= 140000 || run1 == 0 || ibl == 0 || slhc == 0 || run2 == 0 || upg == 0 || RUN2) {
      if (ibl == 0 || run2 == 0 || RUN2)
        ATH_MSG_INFO( "ATLAS IBL geometry - special sampling fractions for gap/crack scin are allowed" );
      else if (slhc == 0 || upg == 0)
        ATH_MSG_INFO( "ATLAS SLHC geometry - special sampling fractions for gap/crack scin are allowed" );
      else
        ATH_MSG_INFO( "ATLAS geometry " << ver << " >= 140000 - special sampling fractions for gap/crack scin are allowed" );

      if (m_info->m_emscaleE[9] < 0.0) {
        m_info->m_emscaleE[9] = m_info->m_emscaleA;
      } else {
        for (int i = 0; i < 64; ++i) {
          if (!TileCablingService::C10_connected(i)) {
            m_info->m_emscaleC[i] =  m_info->m_emscaleE[9];
            ATH_MSG_INFO( "Special C10 in EB module " << i + 1 );
          }
        }

        m_info->m_emscaleE[12] = m_info->m_emscaleE[13];
        m_info->m_emscaleE[14] = m_info->m_emscaleE[15];
      }
    } else {
      ATH_MSG_INFO( "ATLAS geometry " << ver << " < 140000 - disabling special sampling fractions for gap/crack scin" );

      m_info->m_emscaleE[9] = m_info->m_emscaleA;
      for (int i = 10; i < 16; ++i) {
        m_info->m_emscaleE[i] = 75.;
      }
    }

    // sampling fraction for E4' stored in the same array as MBTS (eta index = 2)
    if (m_info->m_emscaleMBTS[2] < 0.0)  m_info->m_emscaleMBTS[2] = m_info->m_emscaleE[15];

    // number of photoelectrons for E4' - if it is not set, assume the same number as for all E-cells
    if (m_info->m_nPhElecVec[6]  < 0.0)  m_info->m_nPhElecVec[6]  = m_info->m_nPhElecVec[3];

    ATH_MSG_INFO( "Sampling fraction for normal cells 1/" << m_info->m_emscaleA );
    ATH_MSG_INFO( "Sampling fraction for special C10 cells 1/" << m_info->m_emscaleE[9] );
    ATH_MSG_INFO( "Sampling fraction for E1 cells 1/" << m_info->m_emscaleE[10] );
    ATH_MSG_INFO( "Sampling fraction for E2 cells 1/" << m_info->m_emscaleE[11] );
    ATH_MSG_INFO( "Sampling fraction for E3 cells 1/" << m_info->m_emscaleE[13] );
    ATH_MSG_INFO( "Sampling fraction for E4 cells 1/" << m_info->m_emscaleE[15] );
    ATH_MSG_INFO( "Sampling fraction for E4' cells 1/" << m_info->m_emscaleMBTS[2] );

  }

  //=== Read in Shape profile of digits & TTL1
  CHECK( buildDigitsShapesHiLo() );

  CHECK( buildTTL1Shapes(m_TTL1ShapeFile, m_info->m_TTL1NBins, m_info->m_TTL1Time0Bin
                         , m_info->m_TTL1BinsPerX, m_info->m_TTL1FullShape, m_info->m_TTL1Shape) );


  CHECK( buildTTL1Shapes(m_MuRcvShapeFile, m_info->m_MuRcvNBins, m_info->m_MuRcvTime0Bin
                         , m_info->m_MuRcvBinsPerX, m_info->m_MuRcvFullShape, m_info->m_MuRcvShape) );

  CHECK( buildTTL1Shapes(m_MuL1ShapeFile, m_info->m_MuL1NBins, m_info->m_MuL1Time0Bin
                        , m_info->m_MuL1BinsPerX, m_info->m_MuL1FullShape, m_info->m_MuL1Shape) );

  //=== Read in coherent noise if required
  if (m_info->TileCoherNoise()) buildCovMatrix();

  // put pointer to new TilePulseShapes in TileInfo
  // only if we want to use them (i.e. when we also read all calib files)
  m_info->m_pulseShapes = m_pulsevar;

  // point to OptFilterWeights or Correlation
  if (m_loadOptFilterWeights) m_info->m_OptFilterWeights=m_OFWeights;
  if (m_loadOptFilterCorrelation) m_info->m_OptFilterCorrelation= m_OFWeights;

  //=== Initialize and register TileInfo object
  CHECK( m_info->initialize() );

  CHECK( m_detStore->record(m_info, "TileInfo", false ) );
  ATH_MSG_INFO(  "Placed TileInfo object in the detector store."  );

  m_eorCalled = false;

  // Listen for end of run
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
  CHECK(incSvc.retrieve());
  int PRIORITY = 100;
  incSvc->addListener(this, "BeginRun", PRIORITY);
  incSvc->addListener(this, "EndRun", PRIORITY);

  return StatusCode::SUCCESS;
}

//****************************************************************************
// Implementation of Incident listener
void TileInfoLoader::handle ( const Incident& incident ) {
//****************************************************************************

  if (incident.type() == IncidentType::BeginRun) {

    ATH_MSG_DEBUG( "Handling BeginRun incident" );

    if (m_eorCalled) {
      if (m_detStore->record(m_info, "TileInfo", false ).isFailure()) {
        ATH_MSG_WARNING( "Failed to register new TileInfo object in the detector store." );
      } else {
        ATH_MSG_INFO( "Placed TileInfo object in the detector store." );
        m_eorCalled = false;
      }
    }

  } else {

    ATH_MSG_INFO( "Handling EndRun incident" );

    if (!m_eorCalled) {
      ATH_MSG_DEBUG( "TileInfo counter before release " << m_detStore->typeCount<TileInfo>() );

      m_detStore->releaseObject(m_info->clID(), "TileInfo");
      if (m_detStore->remove(m_info).isFailure()) {
        ATH_MSG_WARNING( "Failed to remove TileInfo object from detector store." );
      } else {
        ATH_MSG_INFO( "Removed TileInfo object from detector store." );
      }
      ATH_MSG_DEBUG( "TileInfo counter after release " << m_detStore->typeCount<TileInfo>() );

      m_eorCalled = true;
    }
  }
}

//****************************************************************************
StatusCode TileInfoLoader::finalize() {
//****************************************************************************

  ATH_MSG_INFO( "TileInfoLoader::finalize()" );

  return Service::finalize();
}



////////////////////////////////////////////////////////////////////////////////
StatusCode TileInfoLoader::buildDigitsShapesHiLo() {

  ATH_MSG_DEBUG( "In buildDigitsShapesHiLo" );

  // Read in Shape profile of digits signal from external file.

  double It0;
  double dbuff;
  std::string line;

  // Hi gain
  ATH_MSG_DEBUG( " **** Start of High gain Pulse shapes in TileInfo " );

  // Find the full path to filename:
  std::string file_hi = PathResolver::find_file(m_digitsShapeFileHi, "DATAPATH");
  ATH_MSG_INFO( "Reading file  " << file_hi );

  //=== Test if files are there and can be opened
  if (file_hi.size() == 0) {
    ATH_MSG_ERROR( "Could not find input file " << m_digitsShapeFileHi );
    return StatusCode::FAILURE;
  }

  std::ifstream shape_file_hi(file_hi.c_str());
  if (!shape_file_hi.is_open()) {
    ATH_MSG_ERROR( "Could not open input file " << file_hi );
    return StatusCode::FAILURE;
  }

  if (std::getline(shape_file_hi, line)) {
    ATH_MSG_DEBUG( endmsg << line );
  }

  if (std::getline(shape_file_hi, line)) {
    if (sscanf(line.c_str(), "%80d", &m_info->m_digitsNBinsHi) == 1)
      ATH_MSG_DEBUG(  std::setw(3) << m_info->m_digitsNBinsHi << " number of bins in shaping function" );
  }

  if (std::getline(shape_file_hi, line)) {
    if (sscanf(line.c_str(), "%80d", &m_info->m_digitsTime0BinHi) == 1)
      ATH_MSG_DEBUG( std::setw(3) << m_info->m_digitsTime0BinHi << " index of in-time bin" );
  }

  if (std::getline(shape_file_hi, line)) {
    if (sscanf(line.c_str(), "%80d", &m_info->m_digitsBinsPerXHi) == 1)
      ATH_MSG_DEBUG( std::setw(3) << m_info->m_digitsBinsPerXHi << " bins per beam crossing" );
  }

  // Read in the Hi gain shape vector.
  m_info->m_digitsFullShapeHi.resize(m_info->m_digitsNBinsHi, 0.);
  size_t jt = 0;
  while (std::getline(shape_file_hi, line) && jt < m_info->m_digitsFullShapeHi.size()) {
    int nread = sscanf(line.c_str(), "%80lf %80lf", &It0, &dbuff);
    if (nread > 1) m_info->m_digitsFullShapeHi[jt++] = dbuff;
    ATH_MSG_VERBOSE( "t= " << It0 << " a= " << dbuff );
  }

  shape_file_hi.close();

  // Lo gain
  ATH_MSG_DEBUG( " **** Start of Low gain Pulse shapes in TileInfo " );

  // Find the full path to filename:
  std::string file_lo = PathResolver::find_file(m_digitsShapeFileLo, "DATAPATH");
  ATH_MSG_INFO( "Reading file  " << file_lo );

  //=== Test if files are there and can be opened
  if (file_lo.size() == 0) {
    ATH_MSG_ERROR( "Could not find input file " << m_digitsShapeFileLo );
    return StatusCode::FAILURE;
  }

  std::ifstream shape_file_lo(file_lo.c_str());
  if (!shape_file_lo.is_open()) {
    ATH_MSG_ERROR( "Could not open input file " << file_lo  );
    return StatusCode::FAILURE;
  }

  if (std::getline(shape_file_lo, line)) {
    ATH_MSG_DEBUG( endmsg << line );
  }

  if (std::getline(shape_file_lo, line)) {
    if (sscanf(line.c_str(), "%80d", &m_info->m_digitsNBinsLo) == 1)
      ATH_MSG_DEBUG( std::setw(3) << m_info->m_digitsNBinsLo << " number of bins in shaping function" );
  }

  if (std::getline(shape_file_lo, line)) {
    if (sscanf(line.c_str(), "%80d", &m_info->m_digitsTime0BinLo) == 1)
      ATH_MSG_DEBUG( std::setw(3) << m_info->m_digitsTime0BinLo << " index of in-time bin" );
  }

  if (std::getline(shape_file_lo, line)) {
    if (sscanf(line.c_str(), "%80d", &m_info->m_digitsBinsPerXLo) == 1)
      ATH_MSG_DEBUG( std::setw(3) << m_info->m_digitsBinsPerXLo << " bins per beam crossing" );
  }

  // Read in the Lo gain shape vector.
  m_info->m_digitsFullShapeLo.resize(m_info->m_digitsNBinsLo, 0.);
  jt = 0;
  while (std::getline(shape_file_lo, line) && jt < m_info->m_digitsFullShapeLo.size()) {
    int nread = sscanf(line.c_str(), "%80lf %80lf", &It0, &dbuff);
    if (nread > 1) m_info->m_digitsFullShapeLo[jt++] = dbuff;
    ATH_MSG_VERBOSE( "t= " << It0 << " a= " << dbuff );
  }

  shape_file_lo.close();

  //find the greatest value in digitsFullShape (should equal m_info->m_digitsTime0Bin)
  int it0_hi = 0;
  int it0_lo = 0;
  double peak_hi = 0;
  double peak_lo = 0;
  for (int i = 0; i < m_info->m_digitsNBinsHi; i++) {
    if (m_info->m_digitsFullShapeHi[i] > peak_hi) {
      peak_hi = m_info->m_digitsFullShapeHi[i];
      it0_hi = i;
    }
  }

  for (int i = 0; i < m_info->m_digitsNBinsLo; i++) {
    if (m_info->m_digitsFullShapeLo[i] > peak_lo) {
      peak_lo = m_info->m_digitsFullShapeLo[i];
      it0_lo = i;
    }
  }

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << " High Gain: Peak value = " << peak_hi
                    << " at bin = " << it0_hi
                    << ", m_info->m_digitsTime0BinHi = " << m_info->m_digitsTime0BinHi << endmsg;

    msg(MSG::DEBUG) << " Low Gain:  Peak value = " << peak_lo
                    << " at bin = " << it0_lo
                    << ", m_info->m_digitsTime0BinLow = " << m_info->m_digitsTime0BinLo << endmsg;
  }

  // Create vector of time profiles at beam crossings.
  m_info->m_digitsShapeHi.resize(m_info->m_nSamples, 0.);
  m_info->m_digitsShapeLo.resize(m_info->m_nSamples, 0.);
//  ATH_MSG_DEBUG( " Pasa el resize y va a entrar en el lopp" );

  for (int i = 0; i < m_info->m_nSamples; i++) {
    int j = m_info->m_digitsTime0BinHi + (i - m_info->m_iTrigSample) * m_info->m_digitsBinsPerXHi;
    if (j < 0) j = 0;
    if (j > m_info->m_digitsNBinsHi) j = m_info->m_digitsNBinsHi;
    m_info->m_digitsShapeHi[i] = m_info->m_digitsFullShapeHi[j];
    int k = m_info->m_digitsTime0BinLo + (i - m_info->m_iTrigSample) * m_info->m_digitsBinsPerXLo;
    if (k < 0) k = 0;
    if (k > m_info->m_digitsNBinsLo) k = m_info->m_digitsNBinsLo;
    m_info->m_digitsShapeLo[i] = m_info->m_digitsFullShapeLo[k];
  }

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << " Shaping profile at beam crossings:  "
                    << " Number of samples = " << m_info->m_nSamples
                    << ", Sample at Time 0 = " << m_info->m_iTrigSample << endmsg;

    msg(MSG::DEBUG) << "  High gain Shape factor = ";
    for (int i = 0; i < m_info->m_nSamples; i++) {
      msg(MSG::DEBUG) << std::setiosflags(std::ios::fixed)
          << std::setiosflags(std::ios::showpoint) << std::setw(9) << std::setprecision(5)
          << m_info->m_digitsShapeHi[i];
    }

    msg(MSG::DEBUG) << endmsg;

    msg(MSG::DEBUG) << "  Low  gain Shape factor = ";
    for (int i = 0; i < m_info->m_nSamples; i++) {
      msg(MSG::DEBUG) << std::setiosflags(std::ios::fixed)
          << std::setiosflags(std::ios::showpoint) << std::setw(9) << std::setprecision(5)
          << m_info->m_digitsShapeLo[i];
    }
    msg(MSG::DEBUG) << endmsg;

  }

  // Create vector of derivatives from shaping vector.
  m_info->m_digitsDerivativeHi.resize(m_info->m_digitsNBinsHi, 0.);
  m_info->m_digitsDerivativeLo.resize(m_info->m_digitsNBinsLo, 0.);
  for (int i = 1; i < m_info->m_digitsNBinsHi - 1; i++) {
    m_info->m_digitsDerivativeHi[i] = (m_info->m_digitsFullShapeHi[i + 1]
        - m_info->m_digitsFullShapeHi[i - 1]) / 2;
  }

  for (int i = 1; i < m_info->m_digitsNBinsLo - 1; i++) {
    m_info->m_digitsDerivativeLo[i] = (m_info->m_digitsFullShapeLo[i + 1]
        - m_info->m_digitsFullShapeLo[i - 1]) / 2;
  }

  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
StatusCode TileInfoLoader::buildTTL1Shapes(std::string ShapeFile, int &NBins, int &Time0Bin, int &BinsPerX
                                           , std::vector<double> &FullShape, std::vector<double> &Shape) {

  // Read in Shape profile of signal from external file.
  ATH_MSG_DEBUG( " **** Start of TTL1 shapes in TileInfo "  );

  // Find the full path to filename:
  std::string file = PathResolver::find_file(ShapeFile, "DATAPATH");
  ATH_MSG_INFO( "Reading file  " << file );

  if (file.size() == 0) {
    ATH_MSG_ERROR( "Could not find input file " << ShapeFile );
    return StatusCode::FAILURE;
  }

  std::ifstream shape_file(file.c_str());
  if (!shape_file.is_open()) {
    ATH_MSG_ERROR( "Could not open input file " << file );
    return StatusCode::FAILURE;
  }

  double It0;
  double dbuff;
  std::string line;

  if (std::getline(shape_file, line)) {
    ATH_MSG_DEBUG( endmsg << MSG::DEBUG << line );

  }

  if (std::getline(shape_file, line)) {
    if (sscanf(line.c_str(), "%80d", &NBins) == 1)
      ATH_MSG_DEBUG( std::setw(3) << NBins << " number of bins in shaping function" );
  }

  if (std::getline(shape_file, line)) {
    if (sscanf(line.c_str(), "%80d", &Time0Bin) == 1)
      ATH_MSG_DEBUG( std::setw(3) << Time0Bin << " index of in-time bin" );
  }

  if (std::getline(shape_file, line)) {
    if (sscanf(line.c_str(), "%80d", &BinsPerX) == 1)
      ATH_MSG_DEBUG( BinsPerX << " bins per beam crossing" );
  }

  // Read in the ttl1shape vector.
  FullShape.resize(NBins, 0.);
  size_t jt = 0;
  while (std::getline(shape_file, line) && jt < FullShape.size()) {
    int nread = sscanf(line.c_str(), "%80lf %80lf", &It0, &dbuff);
    if (nread > 1) FullShape[jt++] = dbuff;
    ATH_MSG_VERBOSE( "t= " << It0 << " a= " << dbuff );
  }

  shape_file.close();

  // Print out ttl1shape values.
  int nline = (NBins - 1) / 8 + 1;
  if (msgLvl(MSG::DEBUG)) {
    for (int jline = 0; jline < nline; jline++) {
      int j1 = 8 * jline;
      int j2 = j1 + 8;
      if (j2 > NBins) j2 = NBins;
      msg(MSG::DEBUG) << "  " << std::setw(3) << j1 << "  ";

      for (int i = j1; i < j2; i++) {
        msg(MSG::DEBUG) << std::setiosflags(std::ios::fixed)
            << std::setiosflags(std::ios::showpoint) << std::setw(9) << std::setprecision(5)
            << FullShape[i];
      }
      msg(MSG::DEBUG) << endmsg;
    }
  }

  //find the greatest value in ttl1shape (should equal Time0Bin)
  int it0 = 0;
  double peak = 0;
  for (int i = 0; i < NBins; i++) {
    if (FullShape[i] > peak) {
      peak = FullShape[i];
      it0 = i;
    }
  }

  ATH_MSG_DEBUG( " Peak value = " << peak
                << " at bin = " << it0
                << ", Time0Bin = " << Time0Bin );


  // Create vector of time profiles at beam crossings.
  Shape.resize(m_info->m_nSamples, 0.);
  for (int i = 0; i < m_info->m_nSamples; i++) {
    int j = Time0Bin + (i - m_info->m_iTrigSample) * BinsPerX;
    if (j < 0) j = 0;
    if (j > NBins) j = NBins;
    Shape[i] = FullShape[j];
  }

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << " TileInfo: profile at beam crossings:  "
                    << " Number of samples = " << m_info->m_nSamples
                    << ", Sample at Time 0 = " << m_info->m_iTrigSample << endmsg;

    msg(MSG::DEBUG) << "  Shape factor = ";
    for (int i = 0; i < m_info->m_nSamples; i++) {
      msg(MSG::DEBUG) << std::setiosflags(std::ios::fixed)
          << std::setiosflags(std::ios::showpoint) << std::setw(9) << std::setprecision(5)
          << Shape[i];
    }
    msg(MSG::DEBUG) << endmsg;
  }

  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////
void TileInfoLoader::buildCovMatrix() {
// author:F Spano' - beg.
// TOUPDATE:: In the future maybe read from a root file
// Read in Covariance Matrix for Hi Gain/Lo Gain - Barrel/Ext Barrel
// from four external files

// Changed by F. Veloso: full coverage of the Tilecal without using
// the same correlation matrix for all modules.

  char partstr[10];
  char gainstr[10];
  char buff[1024];
  std::string line;

  // Cicle over 4 partitions, 64 modules and 2 gains
  for (int part = 0; part < 4; ++part) {
    // Create DecoCovaria[part]
//    ATH_MSG_INFO( "begin DecoCovaria.push_back" );
    m_info->m_decoCovaria.push_back(std::vector<std::vector<TMatrixD*> >());
//    ATH_MSG_INFO( "DecoCovaria.push_back done" );
    for (int modu = 0; modu < 64; ++modu) {
      // Create DecoCovaria[part][modu]
//      ATH_MSG_INFO( "  begin DecoCovaria[part].push_back" );
      m_info->m_decoCovaria[part].push_back(std::vector<TMatrixD*>());
//      ATH_MSG_INFO( "  DecoCovaria[part].push_back done" );

      for (int gain = 0; gain < 2; ++gain) {
//        ATH_MSG_INFO( "  part=" << part << " modu=" << modu << " gain=" << gain );

        // Open file which corresponds to DecoCovaria[part][modu][gain]
        if (part == 0) sprintf(partstr, "EBA");
        else if (part == 1) sprintf(partstr, "LBA");
        else if (part == 2) sprintf(partstr, "LBC");
        else sprintf(partstr, "EBC");

        if (gain == 0) sprintf(gainstr, "Hi");
        else sprintf(gainstr, "Lo");

        sprintf(buff, "%s%s%02d%s.txt", m_DecoCovaFilePrefix.c_str(), partstr, modu + 1, gainstr);
        std::string file = PathResolver::find_file(buff, "DATAPATH");
//        ATH_MSG_INFO( "  buff=" << buff  << " file=" << file.c_str()  );

        if (file.size() == 0) {
          ATH_MSG_WARNING( "Could not find input file " << buff  );
        } else {
          ATH_MSG_INFO( "Reading file  " << file );

          std::ifstream cov_file(file.c_str());

          if (!cov_file.is_open()) {
            ATH_MSG_WARNING( "Can't read input files, setting tileCoherNoise flag to FALSE" );

            m_info->m_tileCoherNoise = false;
            return;
          } else {

            ATH_MSG_DEBUG( " **** Start of Decomposed Covariance Matrix Read Out" );

            if (std::getline(cov_file, line)) {
              ATH_MSG_DEBUG( "Matrix is being loaded: " << line );
            }

            //define Matrix dimension
            int dima = 0;
            if (std::getline(cov_file, line)) {
              if (sscanf(line.c_str(), "%80d", &dima) == 1)
                ATH_MSG_DEBUG( "The Dimension of the matrix is " << dima );
            }

            // define Matrix: at this level we cannot initialize it with a given one
            TMatrixD* pDecoCova = new TMatrixD(dima, dima);

            // load tokens to be searched for in a string
            char* word;
            const char* TOKENS = { " \t\n" };

            // read Matrix
            int row = 0;
            while (std::getline(cov_file, line) && row < dima) {
              ATH_MSG_DEBUG( "line " << row << " is " << line );

              strncpy(buff, line.c_str(), 1023);
              for (int column = 0; column < dima; column++) {
                // Check for comment lines
                if (*buff == '!' || *buff == '*') continue;
                //
                if (column == 0) {
                  word = strtok(buff, TOKENS);
                } else {
                  word = strtok(NULL, TOKENS);
                }
                double pippo = (word) ? atof(word) : 0.0;
                //            int nread = sscanf(buff, "%1f", &pippo);
                ATH_MSG_VERBOSE( "elem " << column << " is " << pippo );
                (*pDecoCova)(row, column) = pippo;
              }
              row++;
            }

            cov_file.close();
            // Fill DecoCovaria[part][modu][gain]
            m_info->m_decoCovaria[part][modu].push_back(pDecoCova);

          }
        }
      }
    }
  }
}
