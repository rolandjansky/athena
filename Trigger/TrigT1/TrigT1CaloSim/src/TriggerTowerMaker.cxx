/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ================================================
// TriggerTowerMaker class Implementation
// ================================================
//
//
//

// This algorithm includes
#include "GaudiKernel/IIncidentSvc.h"
#include "TrigT1CaloSim/TriggerTowerMaker.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include <fstream>
#include <cmath>
#include <stdexcept> // for std::out_of_range
// #include "CaloEvent/CaloSampling.h"

// For the Athena-based random numbers.
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include <CLHEP/Random/Randomize.h>

#include "TrigConfL1Data/ThresholdConfig.h"
//#include "TrigT1CaloByteStream/IL1CaloMappingTool.h"
#include "TrigT1CaloUtils/InternalTriggerTower_ClassDEF.h"
#include "TrigT1CaloCalibConditions/L1CaloModuleType.h"
#include "AthContainers/ConstDataVector.h"

#include "LumiBlockComps/ILumiBlockMuTool.h"

// Utilities
#include "EventInfoUtils/EventIDFromStore.h"
#include "PathResolver/PathResolver.h"
#include <sys/types.h>

#include <numeric>

namespace LVL1 {

/** This is the constructor for TTMaker and is where you define the relevant
    parameters.
    Currently these are :
    - "TriggerTowerLocation" : the location of the TTs in the TES. You shouldn't have to touch this.
    - "EMCellLocation" : the location of the LAr EM cells in the TES. You shouldn't have to touch this.
    - "HECCellLocation" : the location of the LAr HEC cells in the TES. You shouldn't have to touch this.
    - "TileCellLocation" : the location of the TileCal cells in the TES. You shouldn't have to touch this.
    - "CellType" : set this to 3 for TTL1, 2 for TriggerTower or 1 for CaloCell

  Alter the values of these in jobOptions.txt
*/
  
TriggerTowerMaker::TriggerTowerMaker( const std::string& name, ISvcLocator* pSvcLocator ) 
  : AthAlgorithm( name, pSvcLocator ), 
    m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
    m_rndGenSvc("AtRndmGenSvc", name),
    m_rndmPeds(0),m_rndmADCs(0),
    m_TTtool("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"),
    m_mappingTool("LVL1::PpmMappingTool/PpmMappingTool"),
    m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool"),
    m_mergeSvc(0),
    m_caloMgr(0), 
    m_lvl1Helper(0), 
//     m_l1CaloTTIdTools(0), 
//     m_cells2tt(0),
    m_l1CaloTTIdTools("LVL1::L1CaloTTIdTools/L1CaloTTIdTools"),
    m_cells2tt("LVL1::L1CaloCells2TriggerTowers/L1CaloCells2TriggerTowers"),
    m_caloId(0),
    m_IntTTContainer(0),
    m_TTContainer(0),
    m_ADCContainer(0), 
    m_elementFir(true),
    m_correctFir(true)
{
 
  declareProperty("RndmSvc", m_rndGenSvc, "Random number service");
  declareProperty("PedEngine",m_pedEngine = "TrigT1CaloSim_Pedestal");
  declareProperty("DigiEngine",m_digiEngine = "TrigT1CaloSim_Digitization");
  
  declareProperty( "BaselineCorrection", m_correctFir) ;
  declareProperty( "PulseElements", m_elementFir) ;

  m_CaloCellLocation = TrigT1CaloDefs::CaloCellContainerLocation ;
  m_inputTTLocation  = TrigT1CaloDefs::TriggerTowerLocation ;
  
  m_overlayPed = 32.;
  m_doOverlay  = false;
  
  m_EmTTL1ContainerName   = "LArTTL1EM";
  m_HadTTL1ContainerName  = "LArTTL1HAD";
  m_TileTTL1ContainerName = "TileTTL1Cnt";
  
  m_requireAllCalos = true;
  m_ZeroSuppress = true;

  m_digitScale = 250.;

  m_outputLocation = TrigT1CaloDefs::TriggerTowerLocation ;
  m_cellType  = TTL1 ;
  m_emThresh  = 4000;
  m_hadThresh = 4000;

  // Multi-slice mode
  m_timeslicesLUT = 0;
  m_sliceLUT = 0;
  m_fullADCLocation = TrigT1CaloDefs::TriggerTowerLocation + "ADC";

  // Parameters for adding noise in cell-based tower building
  m_towerNoise = true;
  m_numberOfEmNoiseRegions  = 10;
  m_numberOfHadNoiseRegions = 10;
  m_maxIetaBins = 51; // max numbers of eta regions if we choose a granularity of 0.1 in eta

  /*
    set number of different bcideff values for em and had
    this parameter are only needed, if TTs are build up from calo cells
  */

  m_numberOfBcidEffEm  = 1;
  m_numberOfBcidEffHad = 1;

  // ADC simulation
  m_pedVal = 32.;
  m_pedVar = 1.;
  m_adcVar = 0.65;
  m_adcStep = 250.;
  m_gainCorr = 0.0;
  
  // New (1) or Old (0) LUT strategy
  m_LUTStrategy = 1;

  // Whether pedestal rounded inside LUT
  m_RoundPed = true;

  // Region-specific thresholds
  for (int i=0; i<m_TowerTypes; i++) {
    m_thresh[i] = 0; // use global defaults
  }
  
  // Full (eta) granularity thresholds
  m_Thresholds.resize(2);            // number of layers
  for (int i = 0; i<2; i++) {   
    m_Thresholds[i].resize(4);       // number of regions
    m_Thresholds[i][0].resize(25);   // |eta| < 2.5; Delta(eta) = 0.1
    m_Thresholds[i][1].resize(3);    // 2.5 < |eta| < 3.1; Delta(eta) = 0.2
    m_Thresholds[i][2].resize(1);    // 3.1 < |eta| < 3.2; Delta(eta) = 0.1
    m_Thresholds[i][3].resize(4);    // |eta| > 3.2; 4 eta bins
  }
  
  // Explicitly initialise these values to zero (use global/regional defaults)
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (unsigned int k = 0; k < m_Thresholds[i][j].size(); ++k) m_Thresholds[i][j][k] = 0;
    }
  }

  // Local calibration adjustments (for TTL1 inputs, i.e. receiver gain tweaks) 
  m_DoNotCalibrate = true;

  // First set up vectors
  m_Calib.resize(2);            // number of layers
  for (int i = 0; i<2; i++) {   
    m_Calib[i].resize(4);       // number of regions
    m_Calib[i][0].resize(25);   // |eta| < 2.5; Delta(eta) = 0.1
    m_Calib[i][1].resize(3);    // 2.5 < |eta| < 3.1; Delta(eta) = 0.2
    m_Calib[i][2].resize(1);    // 3.1 < |eta| < 3.2; Delta(eta) = 0.1
    m_Calib[i][3].resize(4);    // |eta| > 3.2; 4 eta bins
  }  

  // Now set some defaults

  // EM: for release 12 need to tweak default LArTTL1 calibrations to
  // obtain flat response vs eta
  // Main tweaks in barrel/OW region
  for (int k = 0; k < 7; k++) {
    m_Calib[0][0][k] = 1.1;
  }
  for (int k = 7; k < 10; k++) {
    m_Calib[0][0][k] = 1.12;
  }
  m_Calib[0][0][10] = 1.13;
  for (int k = 11; k < 14; k++) {
    m_Calib[0][0][k] = 1.125;
  }
  m_Calib[0][0][14] = 1.5;
  for (int k = 15; k < 17; k++) {
    m_Calib[0][0][k] = 1.1;
  }
  for (int k = 17; k < 20; k++) {
    m_Calib[0][0][k] = 0.96;
  }
  for (int k = 20; k < 22; k++) {
    m_Calib[0][0][k] = 0.94;
  }
  for (unsigned int k = 22; k < m_Calib[0][0].size();  k++) {
    m_Calib[0][0][k] = 0.9;
  }

  // constant in IW region
  for (int j = 1; j < 3; j++) {       
    for (unsigned int k = 0; k < m_Calib[0][j].size(); k++) {
      m_Calib[0][j][k] = 1.1;                        
    }
  }
  // Fcal EM 
  m_Calib[0][3][0] = 1.8; 
  m_Calib[0][3][1] = 1.4;  
  m_Calib[0][3][2] = 1.15;  
  m_Calib[0][3][3] = 1.15;  

  // Tile barrel
  for (int k = 0; k < 9; k++) {
    m_Calib[1][0][k] = 1.45;                        
  }
  // The barrel-extended barrel transition has a lot of material.
  m_Calib[1][0][9] = 1.8;
  // Tile ext-barrel
  for (int k = 10; k < 15; k++) {
    m_Calib[1][0][k] = 1.5;                        
  }
  // HEC Outer Wheel
  for (unsigned int k = 15; k < m_Calib[1][0].size(); k++) {
    m_Calib[1][0][k] = 1.2;                        
  }
  // HEC Inner Wheel
  for (int j = 1; j < 3; j++) {
    for (unsigned int k = 0; k < m_Calib[1][j].size(); k++) {
      m_Calib[1][j][k] = 1.2;                        
    }
  }
  // Fcal Had 
  for (unsigned int k = 0; k < m_Calib[1][3].size(); k++) {
    m_Calib[1][3][k] = 1.45;                        
  }

  // Local LUT calibration adjustments
  m_AutoCalibrateLUT = true;

  // First set up vectors
  m_CalibLUT.resize(2);            // number of layers
  for (int i = 0; i<2; i++) {   
    m_CalibLUT[i].resize(4);       // number of regions
    m_CalibLUT[i][0].resize(25);   // |eta| < 2.5; Delta(eta) = 0.1
    m_CalibLUT[i][1].resize(3);    // 2.5 < |eta| < 3.1; Delta(eta) = 0.2
    m_CalibLUT[i][2].resize(1);    // 3.1 < |eta| < 3.2; Delta(eta) = 0.1
    m_CalibLUT[i][3].resize(4);    // |eta| > 3.2; 4 eta bins
  }  

/** Set all of these factors to 1 by default. For m_AutoCalibrateLUT = true this means
    making no correction to the receiver calibration.
    Note: if you set m_AutoCalibrateLUT = false, you will need to load the actual LUT
          slopes into these variables, in which case you'd better know how the LUT slope
          actually works - 1 will *not* be the correct value in this case! */
  
  for (unsigned int i = 0; i < m_CalibLUT.size(); ++i) {
    for (unsigned int j = 0; j < m_CalibLUT[i].size(); ++j) {
      for (unsigned int k = 0; k < m_CalibLUT[i][j].size(); ++k) {
        m_CalibLUT[i][j][k] = 1.;
      }
    }
  }
    
  // Scale for converting ET -> counts
  m_TileToMeV = m_MeV/4.1;
  // TileTTL1 pedestal value - need to subtract if set non-zero 
  m_TileTTL1Ped = 0.;  
  // Option to automatically match FIR coefficients to pulse shape
  m_MatchFIR = true;

  // Default FIR parameters
  m_FIRCoeff.resize(m_TowerTypes);  
  for (int i=0; i<m_TowerTypes; i++) {
    m_FIRCoeff[i].resize(m_FirLength);
  }

  // Pedestal Correction: FIR parameters for elements (eta bins)
  m_FIRCoeffElement.resize(2);  // number of layers
  for (int i = 0; i < m_nLayer; i++) {
    m_FIRCoeffElement[i].resize(33); // number of elements (eta bins)
    for (int j = 0; j < m_nElement; j++) {
      m_FIRCoeffElement[i][j].resize(m_FirLength);
      for (unsigned int k = 0; k < m_FIRCoeffElement[i][j].size(); k++) {
        m_FIRCoeffElement[i][j][k] = 0;
      }
    }
  }

  // LAr EM
  m_FIRCoeff[EmB][0] = 0;
  m_FIRCoeff[EmB][1] = 7;
  m_FIRCoeff[EmB][2] = 15;
  m_FIRCoeff[EmB][3] = 10;
  m_FIRCoeff[EmB][4] = 0;
  
  m_FIRCoeff[EmEC][0] = 0;
  m_FIRCoeff[EmEC][1] = 7;
  m_FIRCoeff[EmEC][2] = 15;
  m_FIRCoeff[EmEC][3] = 10;
  m_FIRCoeff[EmEC][4] = 0;
   
  // LAr Hec
  m_FIRCoeff[Hec][0] = 0;
  m_FIRCoeff[Hec][1] = 7;
  m_FIRCoeff[Hec][2] = 15;
  m_FIRCoeff[Hec][3] = 10;
  m_FIRCoeff[Hec][4] = 0;
 
  // LAr Fcal EM
  m_FIRCoeff[FcalE][0] = 0;
  m_FIRCoeff[FcalE][1] = 7;
  m_FIRCoeff[FcalE][2] = 15;
  m_FIRCoeff[FcalE][3] = 10;
  m_FIRCoeff[FcalE][4] = 0;
   
  // LAr Fcal HAD
  m_FIRCoeff[FcalH][0] = 0;
  m_FIRCoeff[FcalH][1] = 7;
  m_FIRCoeff[FcalH][2] = 15;
  m_FIRCoeff[FcalH][3] = 10;
  m_FIRCoeff[FcalH][4] = 0;
 
  // Tile
  m_FIRCoeff[Tile][0] = 0;
  m_FIRCoeff[Tile][1] = 7;
  m_FIRCoeff[Tile][2] = 15;
  m_FIRCoeff[Tile][3] = 10;
  m_FIRCoeff[Tile][4] = 0;
 
  // BCID thresholds, ranges, decision criteria
  m_DecisionSource = 1;    // Use FIR for range decision
  m_EnergyLow = 0;
  m_EnergyHigh = 255;
  m_BcidDecision[0] = 0xF0;  // FIR+peakfinder
  m_BcidDecision[1] = 0xF0;  // FIR+peakfinder
  m_BcidDecision[2] = 0xCC;  // Saturated pulse BCID
  m_SatOverride[0] = 0;  
  m_SatOverride[1] = 0;  
  m_SatOverride[2] = 1; 
  m_PeakFinderCond = 0; 
  m_SatLevel = 1013;
  for (int i = 0; i < m_TowerTypes; i++) {
    m_nDrop[i]  = 0;
    m_FIRCal[i] = 0;
    m_FIRped[i] = 0;
    m_EnergyLevelLow[i]  = 0;
    m_EnergyLevelHigh[i] = 0;
    m_SatLow[i]   = 0;
    m_SatHigh[i]  = 0;
  }

  // Finally set up vectors for noise simulation
  m_EtaEmNoise.resize( m_numberOfEmNoiseRegions );   // number of eta slices
  m_EmNoise.resize( m_numberOfEmNoiseRegions );      // number of eta slices

  m_EtaHadNoise.resize( m_numberOfHadNoiseRegions ); // number of eta slices
  m_HadNoise.resize( m_numberOfHadNoiseRegions );    // number of eta slices

  //  so disable noise .
  for (int i = 0; i< m_numberOfEmNoiseRegions; i++) {
    m_EtaEmNoise[i] = -1.0;
    m_EmNoise[i] = 1.0;
  }

  for (int i = 0; i< m_numberOfHadNoiseRegions; i++) {
    m_EtaHadNoise[i] = -1.0;
    m_HadNoise[i] = 1.0;
  }
   
  // disable bcideff simulation
  if( m_numberOfBcidEffEm != 0)
    m_BcidEffEm.resize(m_numberOfBcidEffEm,100.0);
  if( m_numberOfBcidEffHad != 0)
    m_BcidEffHad.resize(m_numberOfBcidEffHad,100.0);
   
  m_EnableBcidStudy = false;

  // Setup for bad channel list
  m_badEMRegions.resize(4);      // 4 values needed to specify region
  m_badHadRegions.resize(4);      
  for (int i = 0; i<4; i++) {
    m_badEMRegions[i].clear();
    m_badHadRegions[i].clear();
  }

  // This is how you declare the parameters to Gaudi so that
  // they can be over-written via the job options file
  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
  declareProperty("PpmMappingTool", m_mappingTool); 
  declareProperty( "LumiBlockMuTool", m_lumiBlockMuTool );
  
  declareProperty( "CaloCellLocation", m_CaloCellLocation ) ;
  declareProperty( "inputTTLocation", m_inputTTLocation ) ;
  declareProperty( "EmTTL1ContainerName",m_EmTTL1ContainerName);
  declareProperty( "HadTTL1ContainerName",m_HadTTL1ContainerName);
  declareProperty( "TileTTL1ContainerName",m_TileTTL1ContainerName);
  declareProperty( "RequireAllCalos",m_requireAllCalos);

  declareProperty( "TriggerTowerLocation", m_outputLocation ) ;
  declareProperty( "CellType", m_cellType ) ;

  declareProperty( "FullADCLocation", m_fullADCLocation );
  declareProperty( "TimeslicesLUT", m_timeslicesLUT );
  declareProperty( "SliceLUT", m_sliceLUT );
  
  declareProperty( "EMTowerThreshold", m_emThresh ) ;
  declareProperty( "HadTowerThreshold", m_hadThresh ) ;
  declareProperty( "EmBThresh", m_thresh[EmB]) ;
  declareProperty( "EmECThresh",m_thresh[EmEC]) ;
  declareProperty( "FcalEThresh",m_thresh[FcalE]) ;
  declareProperty( "TileThresh",m_thresh[Tile]) ;
  declareProperty( "HecThresh",m_thresh[Hec]) ;
  declareProperty( "FcalHThresh",m_thresh[FcalH]) ;
  
  declareProperty( "EmReg1Thresh",m_Thresholds[0][0] );
  declareProperty( "EmReg2Thresh",m_Thresholds[0][1] );
  declareProperty( "EmReg3Thresh",m_Thresholds[0][2] );
  declareProperty( "FCAL1Thresh",m_Thresholds[0][3] );
  declareProperty( "HadReg1Thresh",m_Thresholds[1][0] );
  declareProperty( "HadReg2Thresh",m_Thresholds[1][1] );
  declareProperty( "HadReg3Thresh",m_Thresholds[1][2] );
  declareProperty( "FCAL23Thresh",m_Thresholds[1][3] );

  declareProperty( "TowerNoise", m_towerNoise) ;  
  declareProperty( "NumberOfEmNoiseRegions", m_numberOfEmNoiseRegions) ;
  declareProperty( "EmNoiseEtaLower", m_EtaEmNoise) ;
  declareProperty( "EmNoiseRMS", m_EmNoise) ;
  declareProperty( "NumberOfHadNoiseRegions", m_numberOfHadNoiseRegions) ;
  declareProperty( "HadNoiseEtaLower", m_EtaHadNoise) ;
  declareProperty( "HadNoiseRMS", m_HadNoise) ;

  declareProperty( "PedestalValue", m_pedVal) ;  
  declareProperty( "PedestalSpread", m_pedVar) ;  
  declareProperty( "ADCStep", m_adcStep) ;
  declareProperty( "ADCNoise", m_adcVar) ;
  declareProperty( "CalibrationUncertainty", m_gainCorr) ;
  
  declareProperty( "OverlayPedestal", m_overlayPed) ;
  declareProperty( "DoOverlay", m_doOverlay) ;

  declareProperty( "DoNotCalibrate", m_DoNotCalibrate );
  declareProperty( "EmReg1Calib",m_Calib[0][0] );
  declareProperty( "EmReg2Calib",m_Calib[0][1] );
  declareProperty( "EmReg3Calib",m_Calib[0][2] );
  declareProperty( "EmFwdCalib",m_Calib[0][3] );
  declareProperty( "HadReg1Calib",m_Calib[1][0] );
  declareProperty( "HadReg2Calib",m_Calib[1][1] );
  declareProperty( "HadReg3Calib",m_Calib[1][2] );
  declareProperty( "HadFwdCalib",m_Calib[1][3] );

  declareProperty( "AutoCalibrateLUT", m_AutoCalibrateLUT );
  declareProperty( "EmReg1Slope",m_CalibLUT[0][0] );
  declareProperty( "EmReg2Slope",m_CalibLUT[0][1] );
  declareProperty( "EmReg3Slope",m_CalibLUT[0][2] );
  declareProperty( "EmFwdSlope",m_CalibLUT[0][3] );
  declareProperty( "HadReg1Slope",m_CalibLUT[1][0] );
  declareProperty( "HadReg2Slope",m_CalibLUT[1][1] );
  declareProperty( "HadReg3Slope",m_CalibLUT[1][2] );
  declareProperty( "HadFwdSlope",m_CalibLUT[1][3] );
  declareProperty( "LUTStrategy", m_LUTStrategy );
  declareProperty( "RoundPedestal", m_RoundPed );
    
  declareProperty( "MatchFIR", m_MatchFIR );
  declareProperty( "EmBFilterCoeffs", m_FIRCoeff[EmB] );
  declareProperty( "EmECFilterCoeffs", m_FIRCoeff[EmEC] );
  declareProperty( "FcalEFilterCoeffs", m_FIRCoeff[FcalE] );
  declareProperty( "TileFilterCoeffs", m_FIRCoeff[Tile] );
  declareProperty( "HecFilterCoeffs", m_FIRCoeff[Hec] );
  declareProperty( "FcalHFilterCoeffs", m_FIRCoeff[FcalH] );
 
  // Pedestal Correction: Em
  declareProperty( "FilterCoeffsEmElement0", m_FIRCoeffElement[0][0] );
  declareProperty( "FilterCoeffsEmElement1", m_FIRCoeffElement[0][1] );
  declareProperty( "FilterCoeffsEmElement2", m_FIRCoeffElement[0][2] );
  declareProperty( "FilterCoeffsEmElement3", m_FIRCoeffElement[0][3] );
  declareProperty( "FilterCoeffsEmElement4", m_FIRCoeffElement[0][4] );
  declareProperty( "FilterCoeffsEmElement5", m_FIRCoeffElement[0][5] );
  declareProperty( "FilterCoeffsEmElement6", m_FIRCoeffElement[0][6] );
  declareProperty( "FilterCoeffsEmElement7", m_FIRCoeffElement[0][7] );
  declareProperty( "FilterCoeffsEmElement8", m_FIRCoeffElement[0][8] );
  declareProperty( "FilterCoeffsEmElement9", m_FIRCoeffElement[0][9] );
  declareProperty( "FilterCoeffsEmElement10", m_FIRCoeffElement[0][10] );
  declareProperty( "FilterCoeffsEmElement11", m_FIRCoeffElement[0][11] );
  declareProperty( "FilterCoeffsEmElement12", m_FIRCoeffElement[0][12] );
  declareProperty( "FilterCoeffsEmElement13", m_FIRCoeffElement[0][13] );
  declareProperty( "FilterCoeffsEmElement14", m_FIRCoeffElement[0][14] );
  declareProperty( "FilterCoeffsEmElement15", m_FIRCoeffElement[0][15] );
  declareProperty( "FilterCoeffsEmElement16", m_FIRCoeffElement[0][16] );
  declareProperty( "FilterCoeffsEmElement17", m_FIRCoeffElement[0][17] );
  declareProperty( "FilterCoeffsEmElement18", m_FIRCoeffElement[0][18] );
  declareProperty( "FilterCoeffsEmElement19", m_FIRCoeffElement[0][19] );
  declareProperty( "FilterCoeffsEmElement20", m_FIRCoeffElement[0][20] );
  declareProperty( "FilterCoeffsEmElement21", m_FIRCoeffElement[0][21] );
  declareProperty( "FilterCoeffsEmElement22", m_FIRCoeffElement[0][22] );
  declareProperty( "FilterCoeffsEmElement23", m_FIRCoeffElement[0][23] );
  declareProperty( "FilterCoeffsEmElement24", m_FIRCoeffElement[0][24] );
  declareProperty( "FilterCoeffsEmElement25", m_FIRCoeffElement[0][25] );
  declareProperty( "FilterCoeffsEmElement26", m_FIRCoeffElement[0][26] );
  declareProperty( "FilterCoeffsEmElement27", m_FIRCoeffElement[0][27] );
  declareProperty( "FilterCoeffsEmElement28", m_FIRCoeffElement[0][28] );
  declareProperty( "FilterCoeffsEmElement29", m_FIRCoeffElement[0][29] );
  declareProperty( "FilterCoeffsEmElement30", m_FIRCoeffElement[0][30] );
  declareProperty( "FilterCoeffsEmElement31", m_FIRCoeffElement[0][31] );
  declareProperty( "FilterCoeffsEmElement32", m_FIRCoeffElement[0][32] );
  // Pedestal Correction: Had
  declareProperty( "FilterCoeffsHadElement0", m_FIRCoeffElement[1][0] );
  declareProperty( "FilterCoeffsHadElement1", m_FIRCoeffElement[1][1] );
  declareProperty( "FilterCoeffsHadElement2", m_FIRCoeffElement[1][2] );
  declareProperty( "FilterCoeffsHadElement3", m_FIRCoeffElement[1][3] );
  declareProperty( "FilterCoeffsHadElement4", m_FIRCoeffElement[1][4] );
  declareProperty( "FilterCoeffsHadElement5", m_FIRCoeffElement[1][5] );
  declareProperty( "FilterCoeffsHadElement6", m_FIRCoeffElement[1][6] );
  declareProperty( "FilterCoeffsHadElement7", m_FIRCoeffElement[1][7] );
  declareProperty( "FilterCoeffsHadElement8", m_FIRCoeffElement[1][8] );
  declareProperty( "FilterCoeffsHadElement9", m_FIRCoeffElement[1][9] );
  declareProperty( "FilterCoeffsHadElement10", m_FIRCoeffElement[1][10] );
  declareProperty( "FilterCoeffsHadElement11", m_FIRCoeffElement[1][11] );
  declareProperty( "FilterCoeffsHadElement12", m_FIRCoeffElement[1][12] );
  declareProperty( "FilterCoeffsHadElement13", m_FIRCoeffElement[1][13] );
  declareProperty( "FilterCoeffsHadElement14", m_FIRCoeffElement[1][14] );
  declareProperty( "FilterCoeffsHadElement15", m_FIRCoeffElement[1][15] );
  declareProperty( "FilterCoeffsHadElement16", m_FIRCoeffElement[1][16] );
  declareProperty( "FilterCoeffsHadElement17", m_FIRCoeffElement[1][17] );
  declareProperty( "FilterCoeffsHadElement18", m_FIRCoeffElement[1][18] );
  declareProperty( "FilterCoeffsHadElement19", m_FIRCoeffElement[1][19] );
  declareProperty( "FilterCoeffsHadElement20", m_FIRCoeffElement[1][20] );
  declareProperty( "FilterCoeffsHadElement21", m_FIRCoeffElement[1][21] );
  declareProperty( "FilterCoeffsHadElement22", m_FIRCoeffElement[1][22] );
  declareProperty( "FilterCoeffsHadElement23", m_FIRCoeffElement[1][23] );
  declareProperty( "FilterCoeffsHadElement24", m_FIRCoeffElement[1][24] );
  declareProperty( "FilterCoeffsHadElement25", m_FIRCoeffElement[1][25] );
  declareProperty( "FilterCoeffsHadElement26", m_FIRCoeffElement[1][26] );
  declareProperty( "FilterCoeffsHadElement27", m_FIRCoeffElement[1][27] );
  declareProperty( "FilterCoeffsHadElement28", m_FIRCoeffElement[1][28] );
  declareProperty( "FilterCoeffsHadElement29", m_FIRCoeffElement[1][29] );
  declareProperty( "FilterCoeffsHadElement30", m_FIRCoeffElement[1][30] );
  declareProperty( "FilterCoeffsHadElement31", m_FIRCoeffElement[1][31] );
  declareProperty( "FilterCoeffsHadElement32", m_FIRCoeffElement[1][32] );

  declareProperty( "EnergyLow", m_EnergyLow) ;
  declareProperty( "EnergyHigh", m_EnergyHigh) ;
  declareProperty( "DecisionSource", m_DecisionSource) ;
  declareProperty( "BcidDecision1", m_BcidDecision[0]) ;
  declareProperty( "BcidDecision2", m_BcidDecision[1]) ;
  declareProperty( "BcidDecision3", m_BcidDecision[2]) ;
  declareProperty( "SatOverride1", m_SatOverride[0]) ;
  declareProperty( "SatOverride2", m_SatOverride[1]) ;
  declareProperty( "SatOverride3", m_SatOverride[2]) ;
  declareProperty( "PeakFinderCond", m_PeakFinderCond) ;
  
  declareProperty( "SatLevel", m_SatLevel) ;
  declareProperty( "EmBSatLow", m_SatLow[EmB]) ;
  declareProperty( "EmECSatLow", m_SatLow[EmEC]) ;
  declareProperty( "FcalESatLow", m_SatLow[FcalE]) ;
  declareProperty( "TileSatLow", m_SatLow[Tile]) ;
  declareProperty( "HecSatLow", m_SatLow[Hec]) ;
  declareProperty( "FcalHSatLow", m_SatLow[FcalH]) ;
  declareProperty( "EmBSatHigh", m_SatHigh[EmB]) ;
  declareProperty( "EmECSatHigh", m_SatHigh[EmEC]) ;
  declareProperty( "FcalESatHigh", m_SatHigh[FcalE]) ;
  declareProperty( "TileSatHigh", m_SatHigh[Tile]) ;
  declareProperty( "HecSatHigh", m_SatHigh[Hec]) ;
  declareProperty( "FcalHSatHigh", m_SatHigh[FcalH]) ;

  declareProperty( "ZeroSuppress", m_ZeroSuppress );

  declareProperty( "DeadEMRegionsEtaMin", m_badEMRegions[0]) ;
  declareProperty( "DeadEMRegionsPhiMin", m_badEMRegions[1]) ;
  declareProperty( "DeadEMRegionsEtaMax", m_badEMRegions[2]) ;
  declareProperty( "DeadEMRegionsPhiMax", m_badEMRegions[3]) ;
  declareProperty( "DeadHadRegionsEtaMin", m_badHadRegions[0]) ;
  declareProperty( "DeadHadRegionsPhiMin", m_badHadRegions[1]) ;
  declareProperty( "DeadHadRegionsEtaMax", m_badHadRegions[2]) ;
  declareProperty( "DeadHadRegionsPhiMax", m_badHadRegions[3]) ;


  // Create hash table for E->ET conversions
  m_sinThetaHash = 0x0;
  m_sinThetaHash = new double[m_maxIetaBins];
  
  if (m_sinThetaHash != 0x0) {

    /* Fill table with dummy values */
    for (unsigned int i = 0; i < m_maxIetaBins; i++) {
      m_sinThetaHash[i] = -1.0;
    }

    /* set values for barrel region with granularity of 0.1*/    
    for (unsigned int i = 0; i < 25; i++) {
      m_sinThetaHash[i] = 1.0/cosh( (i+0.5)* 0.1);
    }
    
    /* set values for EndCap with granularity of 0.2 except tt by |3.2|
       eta values are are: 2.6, 2.8, 3.0, 3.15 */
    m_sinThetaHash[26] = 1.0/cosh(2.6);
    m_sinThetaHash[28] = 1.0/cosh(2.8);
    m_sinThetaHash[30] = 1.0/cosh(3.0);
    m_sinThetaHash[31] = 1.0/cosh(3.15);
    
    /* fcal granularity is 0.425 */
    m_sinThetaHash[ (unsigned int)(32 + (0.5*0.425)*10.) ] = 1.0/cosh(3.2 + 0.5*0.425);
    m_sinThetaHash[ (unsigned int)(32 + (1.5*0.425)*10.) ] = 1.0/cosh(3.2 + 1.5*0.425);
    m_sinThetaHash[ (unsigned int)(32 + (2.5*0.425)*10.) ] = 1.0/cosh(3.2 + 2.5*0.425);
    m_sinThetaHash[ (unsigned int)(32 + (3.5*0.425)*10.) ] = 1.0/cosh(3.2 + 3.5*0.425);
  }
}
  
TriggerTowerMaker::~TriggerTowerMaker() {
  
  delete [] m_sinThetaHash;
  m_sinThetaHash = 0;
} 

/** the initialise() method is called at the start of processing, so we set up any histograms
    etc. here*/

StatusCode TriggerTowerMaker::initialize()
{
  // We must here instantiate items which can only be made after
  // any job options have been set

  
  ATH_MSG_INFO( "Initialising" );
   
  ATH_CHECK( detStore()->retrieve(m_caloMgr) );
  ATH_CHECK( detStore()->retrieve(m_caloId) );
  
  // Connect to the LVL1ConfigSvc for the global scale parameter
  ATH_CHECK( m_configSvc.retrieve() );

  /** get the random generator serice */
  ATH_CHECK( m_rndGenSvc.retrieve() );
  m_rndmPeds = m_rndGenSvc->GetEngine(m_pedEngine);
  m_rndmADCs = m_rndGenSvc->GetEngine(m_digiEngine);
  if (m_rndmPeds == 0 || m_rndmADCs == 0)
    ATH_MSG_ERROR ( "Failed to retrieve random engine" );

/// Retrieve tools for Cell->Tower mapping
//    IToolSvc* toolSvc;
//    StatusCode scTools = service( "ToolSvc",toolSvc  );
//    IAlgTool *algtool;
// 
//    if (scTools.isSuccess()) {
//       sc = toolSvc->retrieveTool("L1CaloTTIdTools", algtool); 
//       if (sc!=StatusCode::SUCCESS) { 
//         ATH_MSG_WARNING( " Cannot get L1CaloTTIdTools !" ); 
//       } 
//       m_l1CaloTTIdTools = dynamic_cast<L1CaloTTIdTools*> (algtool); 
// 
//       sc = toolSvc->retrieveTool("L1CaloCells2TriggerTowers", algtool);
//       if (sc!=StatusCode::SUCCESS) {
//         ATH_MSG_WARNING( " Cannot get L1CaloCells2TriggerTowers tool !" );
//       }
//       m_cells2tt = dynamic_cast<L1CaloCells2TriggerTowers*> (algtool);
//    }
//    else {
//      ATH_MSG_WARNING( "Unable to retrieve ToolSvc" );
//    }

  ATH_CHECK( m_l1CaloTTIdTools.retrieve() );
  ATH_CHECK( m_cells2tt.retrieve() );
  ATH_CHECK( m_mappingTool.retrieve() );
  ATH_CHECK( m_TTtool.retrieve() );

  if(m_correctFir && m_elementFir) {
    ATH_CHECK( m_lumiBlockMuTool.retrieve() );
  }
    
  /// Set TT thresholds to global defaults, if not specified by region
  for (int i=0; i<m_TowerTypes; i++) {
    if (i == EmB || i == EmEC || i == FcalE) {
      if (m_thresh[i]<=0.) m_thresh[i] = m_emThresh;
    } else {
      if (m_thresh[i]<=0.) m_thresh[i] = m_hadThresh;
    }
  }

  if (m_doOverlay) {
    // Turn off all noise if event overlay has been selected
    m_towerNoise = false;
    m_pedVar = 0.;
    m_adcVar = 0.;
    // The multislice patch is also incompatible
    m_timeslicesLUT = false;
   
    ATH_MSG_INFO( "Pileup and noise added by overlaying digits of random events" );

    ATH_CHECK( service("PileUpMergeSvc", m_mergeSvc) );
  }

  TriggerTowerKey test(0.,0.);
   
  // Setup bad tower list from bad regions
  m_badEMTowers.clear();
  std::vector<double>::const_iterator itEMEtaMin = m_badEMRegions[0].begin();
  std::vector<double>::const_iterator itEMPhiMin = m_badEMRegions[1].begin();
  std::vector<double>::const_iterator itEMEtaMax = m_badEMRegions[2].begin();
  std::vector<double>::const_iterator itEMPhiMax = m_badEMRegions[3].begin();
  while ( (itEMEtaMin != m_badEMRegions[0].end()) &&
          (itEMPhiMin != m_badEMRegions[1].end()) &&
          (itEMEtaMax != m_badEMRegions[2].end()) &&
          (itEMPhiMax != m_badEMRegions[3].end()) ) {
    // Print out regions to be vetoed
    ATH_MSG_INFO( "Disabled EM region (" << (*itEMEtaMin)
        << ", " << (*itEMPhiMin) << ") to (" << (*itEMEtaMax)
        << ", " << (*itEMPhiMax) << ")" );
    // Map phi limits to 0->2pi range
    double phiref = (*itEMPhiMin) + M_PI/128.;  // Offset to avoid rounding problems
    if (phiref < 0.)      phiref += 2.*M_PI;
    if (phiref > 2.*M_PI) phiref -= 2.*M_PI;
    double phimax = (*itEMPhiMax);
    if (phimax < 0.)      phimax += 2.*M_PI;
    if (phimax > 2.*M_PI) phimax -= 2.*M_PI;
    // Because of wrap-around, phimin may be > phimax. This logic deals with that.
    if (phiref > phimax) phimax += 2.*M_PI;
    double phi = phiref;
    while ( phiref < phimax ) {
      double eta = (*itEMEtaMin) + 0.025;         // Offsets to avoid rounding problems     
      while ( eta < (*itEMEtaMax) ) {
        // calculate key
        int key = test.ttKey(phi, eta);
        // check bad channel list and add if not present
        std::map<int, int>::iterator it=m_badEMTowers.find( key );
        if (it == m_badEMTowers.end()) m_badEMTowers.insert(std::map<int, int>::value_type(key,1));
        // increment eta
        eta += 0.1;
      }
      // increment phi
      phi    += M_PI/64.;
      if (phi > 2.*M_PI) phi -= 2.*M_PI;
      phiref += M_PI/64.;
    }
    // Increment iterators to find next region
    itEMEtaMin++;
    itEMEtaMax++;
    itEMPhiMin++;
    itEMPhiMax++;
  }

  // Repeat for bad hadronic regions
  m_badHadTowers.clear();
  std::vector<double>::const_iterator itHadEtaMin = m_badHadRegions[0].begin();
  std::vector<double>::const_iterator itHadPhiMin = m_badHadRegions[1].begin();
  std::vector<double>::const_iterator itHadEtaMax = m_badHadRegions[2].begin();
  std::vector<double>::const_iterator itHadPhiMax = m_badHadRegions[3].begin();
  while ( (itHadEtaMin != m_badHadRegions[0].end())  &&
          (itHadPhiMin != m_badHadRegions[1].end()) &&
          (itHadEtaMax != m_badHadRegions[2].end()) &&
          (itHadPhiMax != m_badHadRegions[3].end()) ) {
    // Print out regions to be vetoed
    ATH_MSG_INFO( "Disabled Had region (" << (*itHadEtaMin)
        << ", " << (*itHadPhiMin) << ") to (" << (*itHadEtaMax)
        << ", " << (*itHadPhiMax) << ")" );
    // Map phi limits to 0->2pi range
    double phiref = (*itHadPhiMin) + M_PI/128.;  // Offset to avoid rounding problems
    if (phiref < 0.)      phiref += 2.*M_PI;
    if (phiref > 2.*M_PI) phiref -= 2.*M_PI;
    double phimax = (*itHadPhiMax);
    if (phimax < 0.)      phimax += 2.*M_PI;
    if (phimax > 2.*M_PI) phimax -= 2.*M_PI;
    // Because of wrap-around, phimin may be > phimax. This logic deals with that.
    if (phiref > phimax) phimax += 2.*M_PI;
    double phi = phiref;
    while ( phiref < phimax ) {
      double eta = (*itHadEtaMin) + 0.025;         // Offsets to avoid rounding problems
      while ( eta < (*itHadEtaMax) ) {
        // calculate key
        int key = test.ttKey(phi, eta);
        // check bad channel list and add if not present
        std::map<int, int>::iterator it=m_badHadTowers.find( key );
        if (it == m_badHadTowers.end()) m_badHadTowers.insert(std::map<int, int>::value_type(key,1));
        // increment eta
        eta += 0.1;
      }
      // increment phi
      phi    += M_PI/64.;
      if (phi > 2.*M_PI) phi -= 2.*M_PI;
      phiref += M_PI/64.;
    }
    // Increment iterators to find next region
    itHadEtaMin++;
    itHadEtaMax++;
    itHadPhiMin++;
    itHadPhiMax++;
  }
  
  // Incident Service: 
  ServiceHandle<IIncidentSvc> incSvc ("IncidentSvc", name());
  ATH_CHECK( incSvc.retrieve() );

  //start listening to "BeginRun"
  incSvc->addListener(this, "BeginRun");
  return StatusCode::SUCCESS ;
}

/** Best if initialisation which uses COOL-derived values is done here
    rather than in initialize() */

void TriggerTowerMaker::handle(const Incident& inc) 
{
  if (inc.type()=="BeginRun") {

    
   
    /// Get global scale from configSvc

    double globalScale = m_configSvc->thresholdConfig()->caloInfo().globalScale();
    ATH_MSG_INFO( "REGTEST Digit scale = " << globalScale << " GeV/count" );
   
    /// globalScale is number of GeV/count. As code is already written to use
    /// MeV/count, safest thing here is to convert:
   
    m_digitScale = 1000.*globalScale;

    /// Now need to (re)-initialise parameters which depend on this value.

    StatusCode sc; 
    if (m_cellType == TTL1 || m_cellType == TRIGGERTOWERS) {
      sc = initPulse();
    }
    else {
      sc = initDelta();
    }
   
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Error initialising pulse" );
    }
  }
}

/** the initPulse() method sets up FIR coefficients & LUT parameters for
 each pulse type. Where possible it updates from the calorimeter configuration
 files or datastore. FIR coefficients may be set by user or automatically
 matched to pulse shape */
 
 StatusCode TriggerTowerMaker::initPulse()
 {
  
        
  /* FIR BCID simulation includes several factors which must be
     computed from the selected parameters. 
   
    So, the procedure used here is that for each pulse type:
      - start with a default pulse profile
      - update from calorimeter data if available
      - optionally match FIR coefficients to pulse shape
      - compute calibration factors and pedestal subtraction from pulse & FIR */

  // LAr EM
  double EmPulse[m_FirLength] = {0.10,0.47,1.,0.60,0.12};     // define a default - updated 180114
  std::vector<double> EMBPulse(EmPulse,EmPulse+m_FirLength);  // copy to vector
  std::vector<double> EMECPulse(EmPulse,EmPulse+m_FirLength); // copy to vector
  EMBPulse  = initLArEm(EMBPulse);                            // update
  EMECPulse = initLArEm(EMECPulse);                           // update
  if (m_MatchFIR) {                                           // match FIR
    m_FIRCoeff[EmB]  = matchFIR(EMBPulse);
    m_FIRCoeff[EmEC] = matchFIR(EMECPulse);        
 }

  // LAr HEC
  double HecPulse[m_FirLength] = {0.03,0.61,1.,0.76,0.39};
  std::vector<double> HECPulse(HecPulse,HecPulse+m_FirLength); // copy to vector
  HECPulse = initLArHec(HECPulse);
  if (m_MatchFIR)  m_FIRCoeff[Hec] = matchFIR(HECPulse);
  
  // LAr Em FCAL
  double FcalEPulse[m_FirLength] = {0.00,0.21,1.,0.30,-0.45};
  std::vector<double> FCAL1Pulse(FcalEPulse,FcalEPulse+m_FirLength);
  FCAL1Pulse = initLArFcal(FCAL1Pulse,1);    // Module 1 = EM sample
  if (m_MatchFIR) m_FIRCoeff[FcalE] = matchFIR(FCAL1Pulse);

  // LAr Had FCAL
  double FcalHPulse[m_FirLength] = {0.00,0.26,1.,0.52,-0.23};
  std::vector<double> FCAL23Pulse(FcalHPulse,FcalHPulse+m_FirLength);
  FCAL23Pulse = initLArFcal(FCAL23Pulse,2);   // Module 2 = first had sample
  if (m_MatchFIR) m_FIRCoeff[FcalH] = matchFIR(FCAL23Pulse);
  
  // Tile
  double TilePulse[m_FirLength] = {0.06,0.56,1.,0.67,0.28}; // updated 180114
  std::vector<double> TILEPulse(TilePulse,TilePulse+m_FirLength);
  TILEPulse = initTile(TILEPulse);
  if (m_MatchFIR) {
    m_FIRCoeff[Tile] = matchFIR(TILEPulse);
  }

  std::vector< std::vector<double> > Pulses;

  Pulses.push_back(EMBPulse);
  Pulses.push_back(EMECPulse);
  Pulses.push_back(FCAL1Pulse);
  Pulses.push_back(TILEPulse);
  Pulses.push_back(HECPulse);
  Pulses.push_back(FCAL23Pulse);

  // Pedestal Correction
  std::vector<  std::vector< std::vector<double> > > PulsesElement;
  PulsesElement.resize(m_nLayer);
  for (int i = 0; i<m_nLayer; i++) {
    PulsesElement[i].resize(m_nElement);
    for (int j=0; j<m_nElement; j++) {
      PulsesElement[i][j].resize(m_FirLength);
    }
  }
  for (int element=0; element<15; element++) {
    for (int sample=0; sample<m_FirLength; sample++) {
      PulsesElement[0][element][sample] = EMBPulse[sample];
      PulsesElement[1][element][sample] = TILEPulse[sample];
    }
  }
  for (int element=15; element<29; element++) {
    for (int sample=0; sample<m_FirLength; sample++) {
      PulsesElement[0][element][sample] = EMECPulse[sample];
      PulsesElement[1][element][sample] = HECPulse[sample];
    }
  }
  for (int element=29; element<33; element++) {
    for (int sample=0; sample<m_FirLength; sample++) {
      PulsesElement[0][element][sample] = FCAL1Pulse[sample];
      PulsesElement[1][element][sample] = FCAL23Pulse[sample];
    }
  }

  // Have all of the pulse parameters now, so finally can set up the LUTs
  if (m_elementFir) initLUTsPrepare(PulsesElement); // Pedestal Correction
  initLUTs(Pulses);

  // and the saturated BCID and ET range parameters
  initSatBCID(EmB,EMBPulse);
  initSatBCID(EmEC,EMECPulse);
  initSatBCID(Hec,HECPulse);
  initSatBCID(FcalE,FCAL1Pulse);
  initSatBCID(FcalH,FCAL23Pulse);
  initSatBCID(Tile,TILEPulse);
         
  return StatusCode::SUCCESS;
}

/** the initDelta() method sets up FIR coefficients & LUT parameters 
  appropriate for delta pulses (no pulse shape). Thus filter coefficients
  are set to passthrough for all pulse types. Number of pulse types
  retained for the moment, but that may change. */
 
 StatusCode TriggerTowerMaker::initDelta()
 {
  
   

/* FIR BCID simulation includes several factors which must be
   computed from the selected parameters. 
   
   So, the procedure used here is that for each pulse type:
     - start with a delta pulse profile     
     - set FIR coefficients to match
     - compute calibration factors and pedestal subtraction from pulse & FIR */

  int idelta[m_FirLength] = {0,0,1,0,0};
  double delta[m_FirLength] = {0.,0.,1.,0.,0.};
  std::vector<double> Pulse(delta,delta+m_FirLength);
  std::vector< std::vector<double> > Pulses;
  
 // As all pulses the same, we can use a loop
  for (int cal = 0; cal < m_TowerTypes; cal++) {   
    m_FIRCoeff[cal].assign(idelta,idelta+m_FirLength);
    m_EnergyLevelLow[cal] = int(m_EnergyLow*m_digitScale/m_adcStep + m_pedVal);   // ensure these are set even
    m_EnergyLevelHigh[cal] = int(m_EnergyHigh*m_digitScale/m_adcStep + m_pedVal); // though SatBCID not used
    Pulses.push_back(Pulse);
  }
  initLUTs(Pulses);
  
 /* Saturated pulse BCID is not designed for delta pulses, so switch it
    off, even if this means overriding the user's jobOptions */
    
  for (int range = 0; range < 3; range++) {
    m_BcidDecision[range] = 0xF0;  
  }
       
  return StatusCode::SUCCESS;
}

/** initLAr methods reads pulse shape from LAr config files */

std::vector<double> TriggerTowerMaker::initLArEm(std::vector<double>& Pulse)
{
  for (int i = 0; i < m_FirLength; i++) {
    ATH_MSG_DEBUG( "Initial pulse shape: Sample " << i 
        << " amp = " << Pulse[i] );
  }
  
  std::string filename = "LArEmLvl1.data";
  std::string pulsedataname=PathResolver::find_file (filename, "DATAPATH");
  if (pulsedataname == "") {
    ATH_MSG_ERROR( "Could not locate LArEmLvl1.data file" );
    ATH_MSG_ERROR( "Use defaults and trust to luck!" );
  } else {
    const char * pulsedatafile= pulsedataname.c_str() ;
    std::ifstream infile (pulsedatafile) ;

    if(!infile) {
      ATH_MSG_ERROR( " cannot open file " << filename );
    } else {
      ATH_MSG_DEBUG ( filename << " file opened " );
      // first read the pulse shape for all energies 
      // valid for both barrel and endcap (from Xavier de la Broise)
      const int nSamp = 7;
      float energy;
      float pulseShape[nSamp];
      infile >> energy 
        >> pulseShape[0] 
        >> pulseShape[1] 
        >> pulseShape[2] 
        >> pulseShape[3] 
        >> pulseShape[4] 
        >> pulseShape[5] 
        >> pulseShape[6];
       
      int iTrig = 3;     
      int offset = iTrig - int(m_FirLength/2);
      
      for (int i = 0; i < m_FirLength; i++) {
        int j = i + offset;
        if (j>=0 && j<nSamp) Pulse[i] = pulseShape[j]; 
        ATH_MSG_DEBUG( "Update LAr Em pulse: Sample " << i 
            << " amp = " << Pulse[i] );
      }
      
      infile.close();
    }
  }
  
  return Pulse;
}

/** initLAr methods reads pulse shape from LAr config files */

std::vector<double> TriggerTowerMaker::initLArHec(std::vector<double>& Pulse)
{
  for (int i = 0; i < m_FirLength; i++) {
    ATH_MSG_DEBUG( "Initial pulse shape: Sample " << i 
        << " amp = " << Pulse[i] );
  }
  
  std::string filename = "LArHecLvl1.data";
  std::string pulsedataname=PathResolver::find_file (filename, "DATAPATH");
  if (pulsedataname == "") {
    ATH_MSG_ERROR( "Could not locate LArHecLvl1.data file");  
    ATH_MSG_ERROR( "Use defaults and trust to luck!" );
  } else {
    const char * pulsedatafile= pulsedataname.c_str() ;
    std::ifstream infile (pulsedatafile) ;

    if(!infile) {
      ATH_MSG_ERROR( " cannot open file " << filename );
    } else {
      ATH_MSG_DEBUG ( filename << " file opened " );
      // first read the pulse shape for all energies 
      // valid for both barrel and endcap (from Xavier de la Broise)
      const int nSamp = 7;
      float pulseShape[nSamp];
      
      infile >> pulseShape[0] 
        >> pulseShape[1] 
        >> pulseShape[2] 
        >> pulseShape[3] 
        >> pulseShape[4] 
        >> pulseShape[5] 
        >> pulseShape[6];
         
      int iTrig = 3;     
      int offset = iTrig - int(m_FirLength/2);
      
      for (int i = 0; i < m_FirLength; i++) {
        int j = i + offset;
        if (j>=0 && j<nSamp)  Pulse[i] = pulseShape[j]; 
        ATH_MSG_DEBUG( "Update LAr Hec pulse: Sample " << i 
            << " amp = " << Pulse[i] );
      }
      
      infile.close() ;
    }
  }
  
  return Pulse;
}

/** initLAr methods reads pulse shape from LAr config files */

std::vector<double> TriggerTowerMaker::initLArFcal(std::vector<double>& Pulse, int Module)
{
  for (int i = 0; i < m_FirLength; i++) {
    ATH_MSG_DEBUG( "Initial pulse shape: Sample " << i 
        << " amp = " << Pulse[i] );
  }
  
  std::string filename = "LArFcalLvl1.data";
  std::string pulsedataname=PathResolver::find_file (filename, "DATAPATH");
  if (pulsedataname == "") {
    ATH_MSG_ERROR( "Could not locate LArFcalLvl1.data file"); 
    ATH_MSG_ERROR( "Use defaults and trust to luck!" );
  } else {
    const char * pulsedatafile= pulsedataname.c_str() ;
    std::ifstream infile (pulsedatafile) ;

    if(!infile) {
      ATH_MSG_ERROR(" cannot open file " << filename );
    } else {
      ATH_MSG_DEBUG ( filename << " file opened " );
    
      // first read the pulse shape for all energies 
      // Fcal datafile has all Fcal pulseshapes in it, plus other data
      // Therefore we need to do some "skipping through" to get what we want
      
      const int nSamp = 24;
      float pulseShape[nSamp];
      float pulseShapeDer[nSamp];
      for (int iSamp = 0; iSamp < nSamp; ++iSamp) {
        pulseShape[iSamp]    = 0.0;
        pulseShapeDer[iSamp] = 0.0;
      }
      int iskip;      // dummy variable for stuff we want to skip
      float fskip[4]; // as above, only floating point
      for (int iMod = 0; iMod < Module; iMod++) {
        infile >> iskip >> fskip[0] >> fskip[1] >> fskip[2] >> fskip[3] ; // module, noise RMS, sampling fraction
  //if (iMod < 2) infile >> fskip[0] >> fskip[1] >> fskip [2] >> fskip [3] ; // calibration coefficients
  //if (iMod < 1) infile >> fskip[0] >> fskip[1] >> fskip [2] >> fskip [3] ; // material coefficients
        infile  >> pulseShape[0] 
                >> pulseShape[1] 
                >> pulseShape[2] 
                >> pulseShape[3] 
                >> pulseShape[4] 
                >> pulseShape[5] 
                >> pulseShape[6]
                >> pulseShape[7] 
                >> pulseShape[8] 
                >> pulseShape[9] 
                >> pulseShape[10] 
                >> pulseShape[11] 
                >> pulseShape[12]
                >> pulseShape[13] 
                >> pulseShape[14] 
                >> pulseShape[15] 
                >> pulseShape[16]
                >> pulseShape[17] 
                >> pulseShape[18] 
                >> pulseShape[19] 
                >> pulseShape[20] 
                >> pulseShape[21] 
                >> pulseShape[22]
                >> pulseShape[23];
        infile  >> pulseShapeDer[0]
                >> pulseShapeDer[1]
                >> pulseShapeDer[2]
                >> pulseShapeDer[3]
                >> pulseShapeDer[4]
                >> pulseShapeDer[5]
                >> pulseShapeDer[6]
                >> pulseShapeDer[7]
                >> pulseShapeDer[8]
                >> pulseShapeDer[9]
                >> pulseShapeDer[10]
                >> pulseShapeDer[11]
                >> pulseShapeDer[12]
                >> pulseShapeDer[13]
                >> pulseShapeDer[14]
                >> pulseShapeDer[15]
                >> pulseShapeDer[16]
                >> pulseShapeDer[17]
                >> pulseShapeDer[18]
                >> pulseShapeDer[19]
                >> pulseShapeDer[20]
                >> pulseShapeDer[21]
                >> pulseShapeDer[22]
                >> pulseShapeDer[23];
      }
      
      // pulseShape is now that of last module read 
       
      int iTrig = 3;     
      int offset = iTrig - int(m_FirLength/2);
      
      for (int i = 0; i < m_FirLength; i++) {
        int j = i + offset;
        if (j>=0 && j<nSamp) Pulse[i] = pulseShape[j];
        ATH_MSG_DEBUG( "Update LAr Fcal pulse for Module " << Module  
                          << ": Sample " << i 
                          << " amp = " << Pulse[i] );
      }
      infile.close() ;
    }
  }
  
  return Pulse;
}
  
/** initTile method reads pulse shape & other parameters from TileInfo */
  
std::vector<double> TriggerTowerMaker::initTile(std::vector<double>& Pulse)
{
  
  
  std::string TileInfoName="TileInfo";
  const TileInfo* tileInfo;
  StatusCode sc = detStore()->retrieve(tileInfo, TileInfoName);
  
  if (!sc.isFailure()) {
    
    int nSamp = tileInfo->NdigitSamples(); // number of time slices for each chan
    int iTrig = tileInfo->ItrigSample();   // index of the triggering time slice
    std::vector<double> pulseShape = tileInfo->ttl1Shape();
   
    int offset = iTrig - int(m_FirLength/2);
    for (int i = 0; i < m_FirLength; i++) {
      int j = i + offset;
      if (j>=0 && j<nSamp) Pulse[i] = pulseShape[j];
      ATH_MSG_DEBUG( "Update Tile pulse: Sample " << i << " = " 
          << Pulse[i] );
    }
    
    Identifier dummy;
    m_TileToMeV = m_MeV/tileInfo->TTL1Calib(dummy);
    ATH_MSG_DEBUG( "Tile TTL1 calibration scale = " 
        << tileInfo->TTL1Calib(dummy) );
    m_TileTTL1Ped = tileInfo->TTL1Ped(dummy);
    ATH_MSG_DEBUG( "Tile TTL1 pedestal value = " 
        << m_TileTTL1Ped );
  } else {
    ATH_MSG_ERROR( "Failed to find TileInfo" );
    m_TileToMeV = m_MeV/4.1;
  }
  
  return Pulse;
}
 
/** matchFIR method computes matched filter coefficients from given pulse shape */

std::vector<int> TriggerTowerMaker::matchFIR(const std::vector<double>& Pulse) 
{
  /** Find matched filter which gives best resolution into LUT.
     Logic is arranges so that if two ranges give similar precision it
     is biassed towards the one with finer coefficients (larger range) */
  int maxBest = 0;
  int useBest = 0;
  for (int drop = 32; drop >= 16; drop /= 2) { /// loop over plausible LUT ranges
    int sum;
    int use;
    int max = 16;
    bool ok = false;
    while (!ok) {
      sum = 0;
      use = 0;
      max -= 1;
      std::vector<int> coeff = calcFIR(Pulse,max); /// calculate filter for this scale
      for (unsigned int sample = 0; sample < Pulse.size(); ++sample) {
        use += int((1023-m_pedVal-1)*Pulse[sample]*coeff[sample]);
        sum += int(( (1023-m_pedVal-1)*Pulse[sample] + m_pedVal+1 )*coeff[sample]);
      }
      if (sum/drop < 1023) ok = true; /// ensure max adc amp within lut range
    }
    /// Now have largest filter coeffs that fit within this LUT input range.
    /// Next we work out how effectively the range is used
    /// Requiring significant improvement means prefer larger coefficients if similar
    if (use/drop > 1.1*useBest) {
      useBest = use/drop;
      maxBest = max;
    } 
  } 

  return calcFIR(Pulse, maxBest);
}    

/** calcFIR computes matched FIR coefficients for a given pulse shape and
    maximum filter coefficient. Assumes input pulse normalised to peak = 1.
    Also respects hardware constraints (coefficient ranges) and does not
    apply negative weights if pulse in undershoot already */

std::vector<int> TriggerTowerMaker::calcFIR(const std::vector<double>& Pulse, int max) 
{
  std::vector<int> FIRco;
  int nSamples = Pulse.size();
  if (max > 15) max = 15;
  for (int sample = 0; sample < nSamples; sample++) {
    int coeff = int(Pulse[sample]*max+0.5);
    if (coeff < 0) coeff = 0;
    if ((sample == 0 || sample > 3) && coeff > 7) coeff = 7;
    FIRco.push_back(coeff);
  }
  return FIRco;
}    
    
/** InitLUTs method uses FIR coefficients and pulse profile to compute
    LUT calibration parameters */
  
void TriggerTowerMaker::initLUTs(const std::vector< std::vector<double> >& Pulses)
{
  

  // How to handle pedestal when truncating to 10 bit LUT range
  float round = ( m_RoundPed ? 0.5 : 0. );

  // Loop over regions and compute parameters
  for (int cal = 0; cal < m_TowerTypes; ++cal) {
   
   if (cal < int(Pulses.size())) {

    /** Calculate calibration and pedestal subtraction after FIR
        Will redo pedestal channel-by-channel, but use central value here for
        LUT range check */
    double FIRcal = 0.;
    int FIRsum = 0;
    int FIRsat = 0;
    int slope = 1;

    for (int sample = 0; sample < m_FirLength; sample++) {
      FIRsum += m_FIRCoeff[cal][sample];
      FIRcal += m_FIRCoeff[cal][sample]*Pulses[cal][sample]*m_MeV/m_adcStep;
      FIRsat += int( m_FIRCoeff[cal][sample]*( (Pulses[cal][sample]*(1023-m_pedVal)) + m_pedVal ) );

      ATH_MSG_INFO( " FIRco[" << sample << "] = "
          << m_FIRCoeff[cal][sample]
          << "  ideal pulse amplitude = " << Pulses[cal][sample]
          );
    }
    double FIRped = FIRsum*m_pedVal;

    ATH_MSG_DEBUG( "Full precision: FIRcal = " << FIRcal
        << " FIRsat = " << FIRsat
        << " FIRped = " << FIRped );

    // Identify 10 bit range to use in LUT

    int bitMax = 0;
    for (int i = 0; i < 16; i++) {
      if ( FIRsat > (1<<i) ) bitMax++;
    }
    m_nDrop[cal] = bitMax - 10;               // keep a 10 bit range below 2**bitMAx
    if (m_nDrop[cal] < 0) m_nDrop[cal] = 0; // should not be possible anyway

    ATH_MSG_DEBUG( "Highest bit used = " << bitMax
        << " bits to drop = " << m_nDrop[cal] );

    // Save the unshifted values for FIR energy range calibration
    m_FIRCal[cal] = int(FIRcal+0.5);
    m_FIRped[cal] = int(FIRped+0.5);

    // for LUT operations on selected 10 bits

    FIRcal /= pow(2.,m_nDrop[cal]);
    
    if (m_AutoCalibrateLUT) slope = int((4096./FIRcal)*(m_MeV/m_digitScale));
     
    ATH_MSG_DEBUG( "Bit-shifted FIRcal = " << FIRcal
        << " slope = " << slope
        << " threshold = " << m_thresh[cal] );

    // Now apply, with local corrections if any, to all towers in region
    int iElement=-1;
    int iLayer=0;
    if (cal == EmB) {
      for (int ieta = 0; ieta < 15; ++ieta) {
        iElement++;
        double etap = (float(ieta)+0.5)*0.1;
        double etam = -etap;
        float calslope = int(slope*m_CalibLUT[0][0][ieta]);
        // p.c. 1.
        if (m_elementFir) calslope = int(m_slopeElement[iLayer][iElement]*m_CalibLUT[0][0][ieta]);
        // Get noise cut value too, by eta bin if set
        int thresh = m_Thresholds[0][0][ieta];
        if (thresh <= 0) thresh = m_thresh[cal];
        for (int iphi = 0; iphi < 64; ++iphi) {
          double phi = (float(iphi)+0.5)*(M_PI/32.);

          double pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
          double firped = FIRsum*(m_pedVal+pedvar);
          // p.c. 2.
          if (m_elementFir) firped = m_FIRsumElement[iLayer][iElement]*(m_pedVal+pedvar);
          float pedsubReal = 0.;
          if (m_LUTStrategy == 0) {
            pedsubReal = firped/pow(2.,m_nDrop[cal]);
            // p.c. 3.
            if (m_elementFir) pedsubReal = firped/pow(2.,m_nDropElement[iLayer][iElement]);
          }
          else {
            pedsubReal = firped*calslope/pow(2.,m_nDrop[cal]) - calslope*0.5;
            // p.c. 4.
            if (m_elementFir) pedsubReal = firped*calslope/pow(2.,m_nDropElement[iLayer][iElement]) - calslope*0.5;
          }
          int pedsub = int(pedsubReal + round);
          L1CaloCoolChannelId idp = channelId(etap, phi, 0);
          m_pedValue.insert(std::map<unsigned int, double>::value_type(idp.id(),m_pedVal+pedvar));
          m_pedSub.insert(std::map<unsigned int, int>::value_type(idp.id(),pedsub));
          m_slope.insert(std::map<unsigned int, int>::value_type(idp.id(),calslope));
          m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(idp.id(),thresh));

          pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
          firped = FIRsum*(m_pedVal+pedvar);
          // p.c. 5.
          if (m_elementFir) firped =  m_FIRsumElement[iLayer][iElement]*(m_pedVal+pedvar);
          pedsubReal = 0.;
          if (m_LUTStrategy == 0) {
            pedsubReal = firped/pow(2.,m_nDrop[cal]);
            // p.c. 6.
            if (m_elementFir) pedsubReal = firped/pow(2.,m_nDropElement[iLayer][iElement]);
          }
          else {
            pedsubReal = firped*calslope/pow(2.,m_nDrop[cal]) - calslope*0.5;
            // p.c. 7.
            if (m_elementFir) pedsubReal = firped*calslope/pow(2.,m_nDropElement[iLayer][iElement]) - calslope*0.5;
          }
          pedsub = int(pedsubReal + round);
          L1CaloCoolChannelId idm = channelId(etam, phi, 0);
          m_pedValue.insert(std::map<unsigned int, double>::value_type(idm.id(),m_pedVal+pedvar));
          m_pedSub.insert(std::map<unsigned int, int>::value_type(idm.id(),pedsub));
          m_slope.insert(std::map<unsigned int, int>::value_type(idm.id(),calslope));
          m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(idm.id(),thresh));
        }
      }
    }
    else if (cal == EmEC) {
      iLayer=0;
      iElement=14;
      for (int ieta = 15; ieta < 25; ++ieta) {
        iElement++;
        double etap = (float(ieta)+0.5)*0.1;
        double etam = -etap;
        float calslope = int(slope*m_CalibLUT[0][0][ieta]);
        // p.c. 1.
        if (m_elementFir) calslope = int(m_slopeElement[iLayer][iElement]*m_CalibLUT[0][0][ieta]);
        int thresh = m_Thresholds[0][0][ieta];
        if (thresh <= 0) thresh = m_thresh[cal];
        for (int iphi = 0; iphi < 64; ++iphi) {
          double phi = (float(iphi)+0.5)*(M_PI/32.);

          double pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
          double firped = FIRsum*(m_pedVal+pedvar);
          // p.c. 2.
          if (m_elementFir) firped = m_FIRsumElement[iLayer][iElement]*(m_pedVal+pedvar);         
 
          float pedsubReal = 0.;
          if (m_LUTStrategy == 0) {
            pedsubReal = firped/pow(2.,m_nDrop[cal]);
            // p.c. 3.
            if (m_elementFir) pedsubReal = firped/pow(2.,m_nDropElement[iLayer][iElement]);
          }
          else {
            pedsubReal = firped*calslope/pow(2.,m_nDrop[cal]) - calslope*0.5;
            // p.c. 4.
            if (m_elementFir) pedsubReal = firped*calslope/pow(2.,m_nDropElement[iLayer][iElement]) - calslope*0.5;
          }
          int pedsub = int(pedsubReal + round);
          L1CaloCoolChannelId idp = channelId(etap, phi, 0);
          m_pedValue.insert(std::map<unsigned int, double>::value_type(idp.id(),m_pedVal+pedvar));
          m_pedSub.insert(std::map<unsigned int, int>::value_type(idp.id(),pedsub));
          m_slope.insert(std::map<unsigned int, int>::value_type(idp.id(),calslope));
          m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(idp.id(),thresh));

          pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
          firped = FIRsum*(m_pedVal+pedvar);
          // p.c. 5.
          if (m_elementFir) firped = m_FIRsumElement[iLayer][iElement]*(m_pedVal+pedvar);
          pedsubReal = 0.;
          if (m_LUTStrategy == 0) {
            pedsubReal = firped/pow(2.,m_nDrop[cal]);
            // p.c. 6.
            if (m_elementFir) pedsubReal = firped/pow(2.,m_nDropElement[iLayer][iElement]);
          }
          else {
            pedsubReal = firped*calslope/pow(2.,m_nDrop[cal]) - calslope*0.5;
            // p.c. 7.
            if (m_elementFir) pedsubReal = firped*calslope/pow(2.,m_nDropElement[iLayer][iElement]) - calslope*0.5;
          }
          pedsub = int(pedsubReal + round);
          L1CaloCoolChannelId idm = channelId(etam, phi, 0);
          m_pedValue.insert(std::map<unsigned int, double>::value_type(idm.id(),m_pedVal+pedvar));
          m_pedSub.insert(std::map<unsigned int, int>::value_type(idm.id(),pedsub));
          m_slope.insert(std::map<unsigned int, int>::value_type(idm.id(),calslope));
          m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(idm.id(),thresh));

          
        }
      }
      iLayer=0;
      iElement=24;
      for (int ieta = 0; ieta < 3; ++ieta) {
        iElement++;
        double etap = (float(ieta)+0.5)*0.2 + 2.5;
        double etam = -etap;
        float calslope = int(slope*m_CalibLUT[0][1][ieta]);
        // p.c 1.
        if (m_elementFir) calslope = int(m_slopeElement[iLayer][iElement]*m_CalibLUT[0][1][ieta]);
        int thresh = m_Thresholds[0][1][ieta];
        if (thresh <= 0) thresh = m_thresh[cal];
        for (int iphi = 0; iphi < 32; ++iphi) {
          double phi = (float(iphi)+0.5)*(M_PI/16.);

          double pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
          double firped = FIRsum*(m_pedVal+pedvar);
          // p.c. 2.
          if (m_elementFir) firped = m_FIRsumElement[iLayer][iElement]*(m_pedVal+pedvar);
          float pedsubReal = 0.;
          if (m_LUTStrategy == 0) {
            pedsubReal = firped/pow(2.,m_nDrop[cal]);
            // p.c. 3.
            if (m_elementFir) pedsubReal = firped/pow(2.,m_nDropElement[iLayer][iElement]);
          }
          else {
            pedsubReal = firped*calslope/pow(2.,m_nDrop[cal]) - calslope*0.5;
            // p.c. 4.
            if (m_elementFir) pedsubReal = firped*calslope/pow(2.,m_nDropElement[iLayer][iElement]) - calslope*0.5;
          }
          int pedsub = int(pedsubReal + round);
           L1CaloCoolChannelId idp = channelId(etap, phi, 0);
          m_pedValue.insert(std::map<unsigned int, double>::value_type(idp.id(),m_pedVal+pedvar));
          m_pedSub.insert(std::map<unsigned int, int>::value_type(idp.id(),pedsub));
          m_slope.insert(std::map<unsigned int, int>::value_type(idp.id(),calslope));
          m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(idp.id(),thresh));

          pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
          firped = FIRsum*(m_pedVal+pedvar);
          // p.c. 5.
          if (m_elementFir) firped = m_FIRsumElement[iLayer][iElement]*(m_pedVal+pedvar);
          pedsubReal = 0.;
          if (m_LUTStrategy == 0) {
            pedsubReal = firped/pow(2.,m_nDrop[cal]);
            // p.c. 6.
            if (m_elementFir) pedsubReal = firped/pow(2.,m_nDropElement[iLayer][iElement]);
          }
          else {
            pedsubReal = firped*calslope/pow(2.,m_nDrop[cal]) - calslope*0.5;
            // p.c 7.
            if (m_elementFir) pedsubReal = firped*calslope/pow(2.,m_nDropElement[iLayer][iElement]) - calslope*0.5;
          }
          pedsub = int(pedsubReal + round);
          L1CaloCoolChannelId idm = channelId(etam, phi, 0);
          m_pedValue.insert(std::map<unsigned int, double>::value_type(idm.id(),m_pedVal+pedvar));
          m_pedSub.insert(std::map<unsigned int, int>::value_type(idm.id(),pedsub));
          m_slope.insert(std::map<unsigned int, int>::value_type(idm.id(),calslope));
          m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(idm.id(),thresh));
        }
      }
      int ieta = 0;
      iLayer=0;
      iElement=28;
      double etap = 3.15;
      double etam = -3.15;
      float calslope = int(slope*m_CalibLUT[0][2][ieta]);
      // p.c. 1.
      if (m_elementFir) calslope = int(m_slopeElement[iLayer][iElement]*m_CalibLUT[0][2][ieta]);
      int thresh = m_Thresholds[0][2][ieta];
      if (thresh <= 0) thresh = m_thresh[cal];
      for (int iphi = 0; iphi < 32; ++iphi) {
          double phi = (float(iphi)+0.5)*(M_PI/16.);

          double pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
          double firped = FIRsum*(m_pedVal+pedvar);
          // p.c. 2.
          if (m_elementFir) firped = m_FIRsumElement[iLayer][iElement]*(m_pedVal+pedvar);
          float pedsubReal = 0.;
          if (m_LUTStrategy == 0) {
            pedsubReal = firped/pow(2.,m_nDrop[cal]);
            // p.c. 3.
            if (m_elementFir) pedsubReal = firped/pow(2.,m_nDropElement[iLayer][iElement]);
          }
          else {
            pedsubReal = firped*calslope/pow(2.,m_nDrop[cal]) - calslope*0.5;
            // p.c. 4.
            if (m_elementFir) pedsubReal = firped*calslope/pow(2.,m_nDropElement[iLayer][iElement]) - calslope*0.5;
          }
          int pedsub = int(pedsubReal + round);
           L1CaloCoolChannelId idp = channelId(etap, phi, 0);
          m_pedValue.insert(std::map<unsigned int, double>::value_type(idp.id(),m_pedVal+pedvar));
          m_pedSub.insert(std::map<unsigned int, int>::value_type(idp.id(),pedsub));
          m_slope.insert(std::map<unsigned int, int>::value_type(idp.id(),calslope));
          m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(idp.id(),thresh));

          pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
          firped = FIRsum*(m_pedVal+pedvar);
          // p.c. 5.
          if (m_elementFir) firped = m_FIRsumElement[iLayer][iElement]*(m_pedVal+pedvar);
          pedsubReal = 0.;
          if (m_LUTStrategy == 0) {
            pedsubReal = firped/pow(2.,m_nDrop[cal]);
            // p.c. 6.
            if (m_elementFir) pedsubReal = firped/pow(2.,m_nDropElement[iLayer][iElement]);
          }
          else {
            pedsubReal = firped*calslope/pow(2.,m_nDrop[cal]) - calslope*0.5;
            // p.c. 7.
            if (m_elementFir) pedsubReal = firped*calslope/pow(2.,m_nDropElement[iLayer][iElement]) - calslope*0.5;
          }
          pedsub = int(pedsubReal + round);
          L1CaloCoolChannelId idm = channelId(etam, phi, 0);
          m_pedValue.insert(std::map<unsigned int, double>::value_type(idm.id(),m_pedVal+pedvar));
          m_pedSub.insert(std::map<unsigned int, int>::value_type(idm.id(),pedsub));
          m_slope.insert(std::map<unsigned int, int>::value_type(idm.id(),calslope));
          m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(idm.id(),thresh));
      }
    }
    else if (cal == FcalE) {
      iLayer=0;
      iElement=28;
      for (int ieta = 0; ieta < 4; ++ieta) {
        iElement++;
        double etap = (float(ieta)+0.5)*0.425 + 3.2;
        double etam = -etap;
        float calslope = int(slope*m_CalibLUT[0][3][ieta]);
        // p.c. 1.
        if (m_elementFir) calslope = int(m_slopeElement[iLayer][iElement]*m_CalibLUT[0][3][ieta]);
        int thresh = m_Thresholds[0][3][ieta];
        if (thresh <= 0) thresh = m_thresh[cal];
        for (int iphi = 0; iphi < 16; ++iphi) {
          double phi = (float(iphi)+0.5)*(M_PI/8.);

          double pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
          double firped = FIRsum*(m_pedVal+pedvar);
          // p.c. 2.
          if (m_elementFir) firped = m_FIRsumElement[iLayer][iElement]*(m_pedVal+pedvar);
          float pedsubReal = 0.;
          if (m_LUTStrategy == 0) {
            pedsubReal = firped/pow(2.,m_nDrop[cal]);
            // p.c. 3.
            if (m_elementFir) pedsubReal = firped/pow(2.,m_nDropElement[iLayer][iElement]);
          }
          else {
            pedsubReal = firped*calslope/pow(2.,m_nDrop[cal]) - calslope*0.5;
            // p.c. 4.
            if (m_elementFir) pedsubReal = firped*calslope/pow(2.,m_nDropElement[iLayer][iElement]) - calslope*0.5;
          }
          int pedsub = int(pedsubReal + round);
           L1CaloCoolChannelId idp = channelId(etap, phi, 0);
          m_pedValue.insert(std::map<unsigned int, double>::value_type(idp.id(),m_pedVal+pedvar));
          m_pedSub.insert(std::map<unsigned int, int>::value_type(idp.id(),pedsub));
          m_slope.insert(std::map<unsigned int, int>::value_type(idp.id(),calslope));
          m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(idp.id(),thresh));

          pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
          firped = FIRsum*(m_pedVal+pedvar);
          // p.c. 5.
          if (m_elementFir) firped = m_FIRsumElement[iLayer][iElement]*(m_pedVal+pedvar);
          pedsubReal = 0.;
          if (m_LUTStrategy == 0) {
            pedsubReal = firped/pow(2.,m_nDrop[cal]);
            // p.c. 6.
            if (m_elementFir) pedsubReal = firped/pow(2.,m_nDropElement[iLayer][iElement]);
          }
          else {
            pedsubReal = firped*calslope/pow(2.,m_nDrop[cal]) - calslope*0.5;
            // p.c. 7.
            if (m_elementFir) pedsubReal = firped*calslope/pow(2.,m_nDropElement[iLayer][iElement]) - calslope*0.5;
          }
          pedsub = int(pedsubReal + round);
          L1CaloCoolChannelId idm = channelId(etam, phi, 0);
          m_pedValue.insert(std::map<unsigned int, double>::value_type(idm.id(),m_pedVal+pedvar));
          m_pedSub.insert(std::map<unsigned int, int>::value_type(idm.id(),pedsub));
          m_slope.insert(std::map<unsigned int, int>::value_type(idm.id(),calslope));
          m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(idm.id(),thresh));
        }
      }
    }
    else if (cal == Tile) {
      iLayer=1;
      iElement=-1;
      for (int ieta = 0; ieta < 15; ++ieta) {
        iElement++;
        double etap = (float(ieta)+0.5)*0.1;
        double etam = -etap;
        float calslope = int(slope*m_CalibLUT[1][0][ieta]);
        // p.c. 1.
        if (m_elementFir) calslope = int(m_slopeElement[iLayer][iElement]*m_CalibLUT[1][0][ieta]);
        int thresh = m_Thresholds[1][0][ieta];
        if (thresh <= 0) thresh = m_thresh[cal];
        for (int iphi = 0; iphi < 64; ++iphi) {
          double phi = (float(iphi)+0.5)*(M_PI/32.);

          double pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
          double firped = FIRsum*(m_pedVal+pedvar);
          // p.c. 2.
          if (m_elementFir) firped = m_FIRsumElement[iLayer][iElement]*(m_pedVal+pedvar);
          float pedsubReal = 0.;
          if (m_LUTStrategy == 0) {
            pedsubReal = firped/pow(2.,m_nDrop[cal]);
            // p.c. 3.
            if (m_elementFir) pedsubReal = firped/pow(2.,m_nDropElement[iLayer][iElement]);
          }
          else {
            pedsubReal = firped*calslope/pow(2.,m_nDrop[cal]) - calslope*0.5;
            // p.c. 4.
            if (m_elementFir) pedsubReal = firped*calslope/pow(2.,m_nDropElement[iLayer][iElement]) - calslope*0.5;
          }
          int pedsub = int(pedsubReal + round);
           L1CaloCoolChannelId idp = channelId(etap, phi, 1);
          m_pedValue.insert(std::map<unsigned int, double>::value_type(idp.id(),m_pedVal+pedvar));
          m_pedSub.insert(std::map<unsigned int, int>::value_type(idp.id(),pedsub));
          m_slope.insert(std::map<unsigned int, int>::value_type(idp.id(),calslope));
          m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(idp.id(),thresh));

          pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
          firped = FIRsum*(m_pedVal+pedvar);
          // p.c. 5.
          if (m_elementFir) firped = m_FIRsumElement[iLayer][iElement]*(m_pedVal+pedvar);
          pedsubReal = 0.;
          if (m_LUTStrategy == 0) {
            pedsubReal = firped/pow(2.,m_nDrop[cal]);
            // p.c. 6.
            if (m_elementFir) pedsubReal = firped/pow(2.,m_nDropElement[iLayer][iElement]);
          }
          else {
            pedsubReal = firped*calslope/pow(2.,m_nDrop[cal]) - calslope*0.5;
            // p.c. 7.
            if (m_elementFir) pedsubReal = firped*calslope/pow(2.,m_nDropElement[iLayer][iElement]) - calslope*0.5;
          }
          pedsub = int(pedsubReal + round);
          L1CaloCoolChannelId idm = channelId(etam, phi, 1);
          m_pedValue.insert(std::map<unsigned int, double>::value_type(idm.id(),m_pedVal+pedvar));
          m_pedSub.insert(std::map<unsigned int, int>::value_type(idm.id(),pedsub));
          m_slope.insert(std::map<unsigned int, int>::value_type(idm.id(),calslope));
          m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(idm.id(),thresh));
        }
      }
    }
    else if (cal == Hec) {
      iLayer=1;
      iElement=14;
      for (int ieta = 15; ieta < 25; ++ieta) {
        iElement++;
        double etap = (float(ieta)+0.5)*0.1;
        double etam = -etap;
        float calslope = int(slope*m_CalibLUT[1][0][ieta]);
        // p.c. 1.
        if (m_elementFir) calslope = int(m_slopeElement[iLayer][iElement]*m_CalibLUT[1][0][ieta]);
        int thresh = m_Thresholds[1][0][ieta];
        if (thresh <= 0) thresh = m_thresh[cal];
        for (int iphi = 0; iphi < 64; ++iphi) {
          double phi = (float(iphi)+0.5)*(M_PI/32.);

          double pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
          double firped = FIRsum*(m_pedVal+pedvar);
          // p.c. 2.
          if (m_elementFir) firped = m_FIRsumElement[iLayer][iElement]*(m_pedVal+pedvar);
          float pedsubReal = 0.;
          if (m_LUTStrategy == 0) {
            pedsubReal = firped/pow(2.,m_nDrop[cal]);
            // p.c. 3.
            if (m_elementFir) pedsubReal = firped/pow(2.,m_nDropElement[iLayer][iElement]);
          }
          else {
            pedsubReal = firped*calslope/pow(2.,m_nDrop[cal]) - calslope*0.5;
            // p.c. 4.
            if (m_elementFir) pedsubReal = firped*calslope/pow(2.,m_nDropElement[iLayer][iElement]) - calslope*0.5;
          }
          int pedsub = int(pedsubReal + round);
           L1CaloCoolChannelId idp = channelId(etap, phi, 1);
          m_pedValue.insert(std::map<unsigned int, double>::value_type(idp.id(),m_pedVal+pedvar));
          m_pedSub.insert(std::map<unsigned int, int>::value_type(idp.id(),pedsub));
          m_slope.insert(std::map<unsigned int, int>::value_type(idp.id(),calslope));
          m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(idp.id(),thresh));

          pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
          firped = FIRsum*(m_pedVal+pedvar);
          // p.c. 5.
          if (m_elementFir) firped = m_FIRsumElement[iLayer][iElement]*(m_pedVal+pedvar);
          pedsubReal = 0.;
          if (m_LUTStrategy == 0) {
            pedsubReal = firped/pow(2.,m_nDrop[cal]);
            // p.c. 6.
            if (m_elementFir) pedsubReal = firped/pow(2.,m_nDropElement[iLayer][iElement]);
          }
          else {
            pedsubReal = firped*calslope/pow(2.,m_nDrop[cal]) - calslope*0.5;
            // p.c. 7.
            if (m_elementFir) pedsubReal = firped*calslope/pow(2.,m_nDropElement[iLayer][iElement]) - calslope*0.5;
          }
          pedsub = int(pedsubReal + round);
          L1CaloCoolChannelId idm = channelId(etam, phi, 1);
          m_pedValue.insert(std::map<unsigned int, double>::value_type(idm.id(),m_pedVal+pedvar));
          m_pedSub.insert(std::map<unsigned int, int>::value_type(idm.id(),pedsub));
          m_slope.insert(std::map<unsigned int, int>::value_type(idm.id(),calslope));
          m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(idm.id(),thresh));
        }
      }
      iLayer=1;
      iElement=24;
      for (int ieta = 0; ieta < 3; ++ieta) {
        iElement++;
        double etap = (float(ieta)+0.5)*0.2 + 2.5;
        double etam = -etap;
        float calslope = int(slope*m_CalibLUT[1][1][ieta]);
        // p.c. 1.
        if (m_elementFir) calslope = int(m_slopeElement[iLayer][iElement]*m_CalibLUT[1][1][ieta]);
        int thresh = m_Thresholds[1][1][ieta];
        if (thresh <= 0) thresh = m_thresh[cal];
        for (int iphi = 0; iphi < 32; ++iphi) {
          double phi = (float(iphi)+0.5)*(M_PI/16.);

          double pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
          double firped = FIRsum*(m_pedVal+pedvar);
          // p.c. 2.
          if (m_elementFir) firped = m_FIRsumElement[iLayer][iElement]*(m_pedVal+pedvar);
          float pedsubReal = 0.;
          if (m_LUTStrategy == 0) {
            pedsubReal = firped/pow(2.,m_nDrop[cal]);
            // p.c. 3.
            if (m_elementFir) pedsubReal = firped/pow(2.,m_nDropElement[iLayer][iElement]);
          }
          else {
            pedsubReal = firped*calslope/pow(2.,m_nDrop[cal]) - calslope*0.5;
            // p.c. 4.
            if (m_elementFir) pedsubReal = firped*calslope/pow(2.,m_nDropElement[iLayer][iElement]) - calslope*0.5;
          }
          int pedsub = int(pedsubReal + round);
          L1CaloCoolChannelId idp = channelId(etap, phi, 1);
          m_pedValue.insert(std::map<unsigned int, double>::value_type(idp.id(),m_pedVal+pedvar));
          m_pedSub.insert(std::map<unsigned int, int>::value_type(idp.id(),pedsub));
          m_slope.insert(std::map<unsigned int, int>::value_type(idp.id(),calslope));
          m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(idp.id(),thresh));

          pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
          firped = FIRsum*(m_pedVal+pedvar);
          // p.c. 5.
          if (m_elementFir) firped = m_FIRsumElement[iLayer][iElement]*(m_pedVal+pedvar);
          pedsubReal = 0.;
          if (m_LUTStrategy == 0) {
            pedsubReal = firped/pow(2.,m_nDrop[cal]);
            // p.c. 6.
            if (m_elementFir) pedsubReal = firped/pow(2.,m_nDropElement[iLayer][iElement]);
          }
          else {
            pedsubReal = firped*calslope/pow(2.,m_nDrop[cal]) - calslope*0.5;
            // p.c. 7.
            if (m_elementFir) pedsubReal = firped*calslope/pow(2.,m_nDropElement[iLayer][iElement]) - calslope*0.5;
          }
          pedsub = int(pedsubReal + round);
          L1CaloCoolChannelId idm = channelId(etam, phi, 1);
          m_pedValue.insert(std::map<unsigned int, double>::value_type(idm.id(),m_pedVal+pedvar));
          m_pedSub.insert(std::map<unsigned int, int>::value_type(idm.id(),pedsub));
          m_slope.insert(std::map<unsigned int, int>::value_type(idm.id(),calslope));
          m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(idm.id(),thresh));
        }
      }
      int ieta = 0;
      iLayer=1;
      iElement=28;
      double etap = 3.15;
      double etam = -3.15;
      float calslope = int(slope*m_CalibLUT[1][2][ieta]);
      // p.c. 1.
      if (m_elementFir) calslope = int(m_slopeElement[iLayer][iElement]*m_CalibLUT[1][2][ieta]);
      int thresh = m_Thresholds[1][2][ieta];
      if (thresh <= 0) thresh = m_thresh[cal];
      for (int iphi = 0; iphi < 32; ++iphi) {
          double phi = (float(iphi)+0.5)*(M_PI/16.);

          double pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
          double firped = FIRsum*(m_pedVal+pedvar);
          // p.c. 2.
          if (m_elementFir) firped = m_FIRsumElement[iLayer][iElement]*(m_pedVal+pedvar);
          float pedsubReal = 0.;
          if (m_LUTStrategy == 0) {
            pedsubReal = firped/pow(2.,m_nDrop[cal]);
            // p.c. 3.
            if (m_elementFir) pedsubReal = firped/pow(2.,m_nDropElement[iLayer][iElement]);
          }
          else {
            pedsubReal = firped*calslope/pow(2.,m_nDrop[cal]) - calslope*0.5;
            // p.c. 4.
            if (m_elementFir) pedsubReal = firped*calslope/pow(2.,m_nDropElement[iLayer][iElement]) - calslope*0.5;
          }
          int pedsub = int(pedsubReal + round);
          L1CaloCoolChannelId idp = channelId(etap, phi, 1);
          m_pedValue.insert(std::map<unsigned int, double>::value_type(idp.id(),m_pedVal+pedvar));
          m_pedSub.insert(std::map<unsigned int, int>::value_type(idp.id(),pedsub));
          m_slope.insert(std::map<unsigned int, int>::value_type(idp.id(),calslope));
          m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(idp.id(),thresh));

          pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
          firped = FIRsum*(m_pedVal+pedvar);
          // p.c. 5.
          if (m_elementFir) firped = m_FIRsumElement[iLayer][iElement]*(m_pedVal+pedvar);
          pedsubReal = 0.;
          if (m_LUTStrategy == 0) {
            pedsubReal = firped/pow(2.,m_nDrop[cal]);
            // p.c. 6.
            if (m_elementFir) pedsubReal = firped/pow(2.,m_nDropElement[iLayer][iElement]);
          }
          else {
            pedsubReal = firped*calslope/pow(2.,m_nDrop[cal]) - calslope*0.5;
            // p.c. 7.
            if (m_elementFir) pedsubReal = firped*calslope/pow(2.,m_nDropElement[iLayer][iElement]) - calslope*0.5;
          }
          pedsub = int(pedsubReal + round);
          L1CaloCoolChannelId idm = channelId(etam, phi, 1);
          m_pedValue.insert(std::map<unsigned int, double>::value_type(idm.id(),m_pedVal+pedvar));
          m_pedSub.insert(std::map<unsigned int, int>::value_type(idm.id(),pedsub));
          m_slope.insert(std::map<unsigned int, int>::value_type(idm.id(),calslope));
          m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(idm.id(),thresh));
      }
    }
    else if (cal == FcalH) {
      iLayer=1;
      iElement=28;
      for (int ieta = 0; ieta < 4; ++ieta) {
        iElement++;
        double etap = (float(ieta)+0.5)*0.425 + 3.2;
        double etam = -etap;
        float calslope = int(slope*m_CalibLUT[1][3][ieta]);
        // p.c. 1.
        if (m_elementFir) calslope = int(m_slopeElement[iLayer][iElement]*m_CalibLUT[1][3][ieta]);
        int ietam = ieta + 1;
        if (ieta == 1 || ieta == 3) ietam = ieta - 1;
        int threshp = m_Thresholds[1][3][ieta];
        if (threshp <= 0) threshp = m_thresh[cal];
        int threshm = m_Thresholds[1][3][ietam];
        if (threshm <= 0) threshm = m_thresh[cal];
        for (int iphi = 0; iphi < 16; ++iphi) {
          double phi = (float(iphi)+0.5)*(M_PI/8.);

          double pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
          double firped = FIRsum*(m_pedVal+pedvar);
          // p.c. 2.
          if (m_elementFir) firped = m_FIRsumElement[iLayer][iElement]*(m_pedVal+pedvar);
          float pedsubReal = 0.;
          if (m_LUTStrategy == 0) {
            pedsubReal = firped/pow(2.,m_nDrop[cal]);
            // p.c. 3.
            if (m_elementFir) pedsubReal = firped/pow(2.,m_nDropElement[iLayer][iElement]);
          }
          else {
            pedsubReal = firped*calslope/pow(2.,m_nDrop[cal]) - calslope*0.5;
            // p.c. 4.
            if (m_elementFir) pedsubReal = firped*calslope/pow(2.,m_nDropElement[iLayer][iElement]) - calslope*0.5;
          }
          int pedsub = int(pedsubReal + round);
          L1CaloCoolChannelId idp = channelId(etap, phi, 1);
          m_pedValue.insert(std::map<unsigned int, double>::value_type(idp.id(),m_pedVal+pedvar));
          m_pedSub.insert(std::map<unsigned int, int>::value_type(idp.id(),pedsub));
          m_slope.insert(std::map<unsigned int, int>::value_type(idp.id(),calslope));
          m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(idp.id(),threshp));
          m_offsetElement[iLayer][iElement] = pedsub;  // for FCal23 mapping
          m_threshElement[iLayer][iElement] = threshp; // for FCal23 mapping

          pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
          firped = FIRsum*(m_pedVal+pedvar);
          // p.c. 5.
          if (m_elementFir) firped = m_FIRsumElement[iLayer][iElement]*(m_pedVal+pedvar);
          pedsubReal = 0.;
          if (m_LUTStrategy == 0) {
            pedsubReal = firped/pow(2.,m_nDrop[cal]);
            // p.c. 6.
            if (m_elementFir) pedsubReal = firped/pow(2.,m_nDropElement[iLayer][iElement]);
          }
          else {
            pedsubReal = firped*calslope/pow(2.,m_nDrop[cal]) - calslope*0.5;
            // p.c. 7.
            if (m_elementFir) pedsubReal = firped*calslope/pow(2.,m_nDropElement[iLayer][iElement]) - calslope*0.5;
          }
          pedsub = int(pedsubReal + round);
          L1CaloCoolChannelId idm = channelId(etam, phi, 1);
          m_pedValue.insert(std::map<unsigned int, double>::value_type(idm.id(),m_pedVal+pedvar));
          m_pedSub.insert(std::map<unsigned int, int>::value_type(idm.id(),pedsub));
          m_slope.insert(std::map<unsigned int, int>::value_type(idm.id(),calslope));
          m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(idm.id(),threshm));
          //m_nDropElement[iLayer][iElement] = pedsub;
        }
      }
    }
    else ATH_MSG_WARNING( "Unknown calorimeter module " << cal );
    
   }
   else ATH_MSG_ERROR( "No LUT setup for calorimeter type " << cal );
  }
  return;     
}
     
/** InitSatBCID method uses pulse profile and saturation threshold to compute
    thresholds for saturated pulse BCID */

void TriggerTowerMaker::initSatBCID(int cal, const std::vector<double>& Pulse)
{
  
  ATH_MSG_DEBUG( "Initialise SatBCID for pulse type " << cal );
  
  // If DecisionSource = FIR, need to convert ET ranges -> FIR values for tower type
  // Otherwise assume they have been set correctly in ADC counts
  
  if (!(m_DecisionSource&0x1)) {    // Use ADC for decision 
    m_EnergyLevelLow[cal] = int(m_EnergyLow*m_digitScale/m_adcStep + m_pedVal);
    m_EnergyLevelHigh[cal] = int(m_EnergyHigh*m_digitScale/m_adcStep + m_pedVal);
    ATH_MSG_DEBUG( "Decision ranges (ADC counts) = " << m_EnergyLevelLow[cal]
                     << "; " << m_EnergyLevelHigh[cal] );
  }
  else {       // Use FIR for decision
    if (m_EnergyLow > 0) {
      m_EnergyLevelLow[cal] = int(m_EnergyLow*m_FIRCal[cal] + m_FIRped[cal]);
    }
    else {
      m_EnergyLevelLow[cal] = 0;
    }
    
    if (m_EnergyHigh > 0) {
      m_EnergyLevelHigh[cal] = int(m_EnergyHigh*m_FIRCal[cal] + m_FIRped[cal]);
    }
    else {
      m_EnergyLevelHigh[cal] = 0;
    }
    
    ATH_MSG_DEBUG( "Decision ranges (FIR) = " << m_EnergyLevelLow[cal]
                     << "; " << m_EnergyLevelHigh[cal] );
  }
  
  // Now to set thresholds for decision logic:
  // Have thresholds already been set? If so, don't override.
  
  if (m_SatHigh[cal] > 0 || m_SatLow[cal] > 0) {
    ATH_MSG_DEBUG( "SatBCID thresholds already set: SatLow = " << m_SatLow[cal]
        << "; SatHigh = " << m_SatHigh[cal] );
    return;
  }
    
  // Otherwise, compute them from the pulse shape:
  
  double scale = m_SatLevel - m_pedVal;
  double margin = 10.*m_MeV/m_adcStep;         // 10 GeV margin for noise
  
  ATH_MSG_DEBUG( "SatBCID: SatLevel = " << m_SatLevel
                   << "; scale = " << scale 
                   << "; margin = " << margin 
       );
       
  // Algorithm needs 2 samples on rising edge
  
  int nSamples = Pulse.size();
  if (nSamples >= 5) {            
    
    // Assumes pulse peaks in centre of vector
    int peak = nSamples/2;      // works if odd number of samples, which there should be
  
    /* Calculate thresholds. When peak starts to saturate, peak-1 and peak-2 
       should pass their respective thresholds. However, we don't want it
       to be possible for noise fluctuations to pass SatLow (peak-2 threshold) */
  
    // Ideal amplitudes of preceeding samples when satuation begins
    double minusOne = Pulse[peak-1]*scale + m_pedVal;   
    double minusTwo = Pulse[peak-2]*scale + m_pedVal;   
     
    /* SatHigh setting: 
       Most important thing is that this one is passed when saturation begins.
       So set to amp of peak-1 when saturation starts - margin
       Should be OK as pulse shape should be undistorted to this point */
    
    m_SatHigh[cal] = int(minusOne - margin); 
    if (m_SatHigh[cal] <  m_pedVal+margin) {
       ATH_MSG_DEBUG( "m_SatHigh[" << cal << "] has lower level than I'd like" );
    }  
    
    /* SatLow setting:
       This one can cause problems either if it is ever failed by peak-2
       OR if noise causes it to be passed by peak-3.
       Expect the former to be the bigger threat, so set accordingly, but
       warn if the latter seems to be possible. */
    m_SatLow[cal] = int(minusTwo - margin);
    if (m_SatLow[cal] <  m_pedVal+margin) {
       ATH_MSG_DEBUG( "m_SatLow[" << cal << "] has lower level than I'd like" );
       if (m_SatLow[cal] < 0) m_SatLow[cal] = 0;
    }  
    
    // For info, print out the pulse shape and thresholds     
    for (int sample = peak-2; sample < peak+3; sample++) {
      ATH_MSG_DEBUG( "Pulse sample " << sample << " = " << Pulse[sample] );
    }
    ATH_MSG_DEBUG( "SatLow[" << cal << "]  = " << m_SatLow[cal] );
    ATH_MSG_DEBUG( "SatHigh[" << cal << "] = " << m_SatHigh[cal] );
  
  }
  else {  // insufficient samples in pulse!
    ATH_MSG_WARNING( "Only " << nSamples << " samples in pulse!! " );
    ATH_MSG_WARNING( "Saturated pulse BCID should not be used" );
    m_SatLow[cal] = int(m_pedVal);
    m_SatHigh[cal] = int(m_pedVal);
  }
  
  return;     
}

/** the finalise() method is called at the end of processing, so it is used
for deleting histograms and general tidying up*/

StatusCode TriggerTowerMaker::finalize()
{
  
  ATH_MSG_INFO( "Finalizing" );

  return StatusCode::SUCCESS ;
}

/**----------------------------------------------
   execute() method called once per event
   ----------------------------------------------

Checks that the Cell Type is supported (terminates with errors if not)
and calls relevant routine to look for the cells.
*/
StatusCode TriggerTowerMaker::execute( )
{
  //................................
  // make a message logging stream 
  
  ATH_MSG_VERBOSE( "Executing" );

  // Create a map to hold the internal towers
  m_IntTTContainer = new std::map<int, InternalTriggerTower*>;

  // Create a map to hold the output TriggerTowers
  m_TTContainer = new std::map<int, TriggerTower*>;

  // If running in multi-timeslice mode create a map to contain TriggerTowers
  // with extended ADC digits.  If there is only one slice can also be used
  // to provide non-zero-suppressed output for bytestream.

  if (m_timeslicesLUT) setupADCMap();
  
  ///m_eventNumber++;// keep a count of which event we're on. Needed for reading in test vectors

  StatusCode sc;
  
  switch (m_cellType){
  case CELL :
    {
      ATH_MSG_VERBOSE ( "Looking for CaloCells" );
      StatusCode temp=getCaloCells();
      if (temp==StatusCode::SUCCESS) {
         digitize(); // digitisation
         preProcess(); // FIR, BCID etc
      }
      break;
    }
  case TRIGGERTOWERS :
    {
      ATH_MSG_VERBOSE ( "Looking for TriggerTower input" );
      StatusCode temp=getTriggerTowers();
      if (temp==StatusCode::SUCCESS) {
         preProcess(); // FIR, BCID etc
      }
      break;
    }
    
  case TTL1 :
    {
      ATH_MSG_VERBOSE ( "Looking for calo towers" );
      StatusCode temp=getCaloTowers();
      if (temp==StatusCode::SUCCESS) {
        digitize(); // digitisation
        StatusCode scProceed = StatusCode::SUCCESS;
        if (m_doOverlay) scProceed = overlay();             // noise/mbias overlay
        if (scProceed == StatusCode::SUCCESS) preProcess(); // digitisation, FIR, BCID etc
      }
      break;
    }
  
  default :
    {
      ATH_MSG_ERROR( "Unsupported Cell Type!!!!!!" );
      break;
    }
  }//end switch

  // store them thar trigger towers
  sc=store();

  // Clean up at end of event
  delete m_IntTTContainer;
  delete m_TTContainer;
  if (m_timeslicesLUT) delete m_ADCContainer;
  return sc;
}//end execute

/** fetches LAr & Tile calorimeter cells */

StatusCode LVL1::TriggerTowerMaker::getCaloCells(){
  // Find  CaloCells in TES
  
  const CaloCellContainer* AllCells;
  
  // Give up now if the container doesn't exist
  if (!evtStore()->contains<CaloCellContainer>(m_CaloCellLocation)) {
    ATH_MSG_WARNING( "getGeantCells: no CaloCellContainer found in SG" );
    return StatusCode::FAILURE;
  }

  StatusCode sc0 = evtStore()->retrieve(AllCells, m_CaloCellLocation);
  ATH_MSG_DEBUG ( "Retrieve AllCalo, sc = " << sc0 );
        
  if (msgLvl(MSG::DEBUG)) {
    if (sc0==StatusCode::FAILURE) {
      ATH_MSG_DEBUG( "Can't find calo cells - stopping processing" );
    }else{
      //okay so we have some cells
      ATH_MSG_VERBOSE( "Found " << AllCells->size() << " calorimeter cells" );
    }// end else if (! cells)
  }
  
  if (sc0==StatusCode::SUCCESS) processCaloCells(AllCells);
 
  return StatusCode::SUCCESS;
}

/** Stores Trigger Towers in the TES, at a
location defined in m_outputLocation.

Returns FAILURE if the towers could not be saved.
 */
StatusCode LVL1::TriggerTowerMaker::store()
{
  // The TT map cannot be saved directly: there (currently) no
  // DataObject derived map container in GAUDI/Athena.
  // We must first write the data into an DataVector.

  t_TTCollection* VectorOfTTs = new  t_TTCollection;

  std::map<int, TriggerTower*>::iterator it;

  ATH_MSG_DEBUG("Storing TTs in DataVector");

  for( it=m_TTContainer->begin(); it!=m_TTContainer->end(); ++it ){
    VectorOfTTs->push_back(it->second);
    ATH_MSG_DEBUG("TT has coords ("<<it->second->phi()<<", "<<it->second->eta()
        << " and energies : "<<it->second->emEnergy()<<", "<<it->second->hadEnergy()<<" (Em,Had)");
  }

  ATH_MSG_VERBOSE( VectorOfTTs->size()<<" TTs have been generated");
  ATH_CHECK( evtStore()->overwrite(VectorOfTTs, m_outputLocation, true) );
  VectorOfTTs=0;
  ATH_MSG_DEBUG ( "Stored TTs in TES at "<< m_outputLocation );

  // For multi-slice mode save extended ADC digits on first slice

  if (m_timeslicesLUT) {
    if (m_sliceLUT == 0) {
      ConstDataVector<t_TTCollection>* VectorOfFullADCs = new ConstDataVector<t_TTCollection>;
      TriggerTowerMap_t::iterator pos  = m_ADCContainer->begin();
      TriggerTowerMap_t::iterator pose = m_ADCContainer->end();
      for (; pos != pose; ++pos) VectorOfFullADCs->push_back(pos->second);

      ATH_CHECK( evtStore()->overwrite(VectorOfFullADCs, m_fullADCLocation, true) );
      ATH_MSG_DEBUG ( "Stored multi-slice ADC digits in TDS, key "
                      << m_fullADCLocation );
    }
  }

   return StatusCode::SUCCESS;
} // end of LVL1::TriggerTowerMaker::store(){

/** steps over Calo cells and creates/fills trigger towers. */

void LVL1::TriggerTowerMaker::processCaloCells(const CaloCellContainer * cells)
{
  static bool showCalibVal = false;
  
  //int outputLevel = msgSvc()->outputLevel( name() );
  
  if( showCalibVal) {
    
    for (uint i = 0; i<2; i++) {
      for (uint j = 0; j <  m_Calib[i].size(); j++) {
        for (uint k = 0; k <  m_Calib[i][j].size(); k++) {
          ATH_MSG_INFO( " processCaloCells m_Calib[" << i << "][" << j << "][" << k << "] = " <<  m_Calib[i][j][k] );
        }
      } 
    }
    
  }

  // Algorithm relies on valid hash table. Abort if this is not present
  if (m_sinThetaHash == 0x0) {
    ATH_MSG_ERROR( "No hash table present. Abort processing" );
    return;
  }
  
  // the TriggerTowerKey object provides the key for each trigger tower,
  // depending on its eta,phi coords. The key is an integer number that uniquely
  // identifies each tower.
  TriggerTowerKey testKey(0.0, 0.0);

  // Initialise cell->tt tool
  m_cells2tt->initCaloCellsTriggerTowers(*cells);

  /*
    Procedure: we need to create towers at all possible locations, since even
    if there is no cell ET in a tower there will be noise from the summing circuits.
    Hence we loop over all possible tower positions, compute the identifiers,
    and then use the tool to get the ET from the cells matching that tower.
    Conversion to ET, noise addition and calibration adjustments are all performed here.
  */
  // Central region, |eta|<2.5
  for (int ieta = 0; ieta < 25; ieta++) {   // loop over one side in eta
    double temp_eta = (ieta + 0.5) * 0.1;
    double emScale = calib(temp_eta, EmB);
    double hdScale = calib(temp_eta, Tile);
    
    for (int iphi = 0; iphi < 64; iphi++) {
      double temp_phi = (iphi + 0.5)*(M_PI/32.);

      // Get corresponding tower key and coordinates
      unsigned int key = testKey.ttKey(temp_phi,temp_eta);
      double tt_phi=testKey.phi();
      double tt_eta=testKey.eta();
      int Ieta = (int)(tt_eta*10.);
      // compute identifier
      Identifier emId  = m_TTtool->identifier(tt_eta, tt_phi, 0);
      Identifier hadId = m_TTtool->identifier(tt_eta, tt_phi, 1);
      // get cell energy sums for this TT
      double emTTCellsEnergy = m_cells2tt->energy(emId);
      double hadTTCellsEnergy = m_cells2tt->energy(hadId);
      // add noise
      if (m_towerNoise) emTTCellsEnergy  += CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,emNoise(tt_eta));
      if (m_towerNoise) hadTTCellsEnergy += CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,hadNoise(tt_eta));
      // create and fill tower
      InternalTriggerTower* TT1 = findTriggerTower(tt_phi, tt_eta, key);
      TT1->addEMPeak(emTTCellsEnergy*emScale*m_sinThetaHash[Ieta]);
      TT1->addHadPeak(hadTTCellsEnergy*hdScale*m_sinThetaHash[Ieta]);

      // repeat for -eta
      key = testKey.ttKey(temp_phi,-temp_eta);
      tt_phi=testKey.phi();
      tt_eta=testKey.eta();
      // compute identifier
      emId  = m_TTtool->identifier(tt_eta, tt_phi, 0);
      hadId = m_TTtool->identifier(tt_eta, tt_phi, 1);
      // get cell energy sums for this TT
      emTTCellsEnergy = m_cells2tt->energy(emId);
      hadTTCellsEnergy = m_cells2tt->energy(hadId);
      // add noise
      if (m_towerNoise) emTTCellsEnergy  += CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,emNoise(tt_eta));
      if (m_towerNoise) hadTTCellsEnergy += CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,hadNoise(tt_eta));
      // create and fill tower
      InternalTriggerTower* TT2 = findTriggerTower(tt_phi, tt_eta, key);
      TT2->addEMPeak(emTTCellsEnergy*emScale*m_sinThetaHash[Ieta]);
      TT2->addHadPeak(hadTTCellsEnergy*hdScale*m_sinThetaHash[Ieta]);
    
    }  // end phi loop
  }  // end eta loop
        
  // Outer region, 2.5 < |eta| <3.2
  for (int ieta = 0; ieta < 4; ieta++) {   // loop over one side in eta
    double temp_eta = 2.55 + (ieta * 0.2);
    double emScale = calib(temp_eta, EmB);
    double hdScale = calib(temp_eta, Tile);
    
    for (int iphi = 0; iphi < 32; iphi++) {
      double temp_phi = (iphi + 0.5)*(M_PI/16.);

      // Get corresponding tower key and coordinates
      unsigned int key = testKey.ttKey(temp_phi,temp_eta);
      double tt_phi=testKey.phi();
      double tt_eta=testKey.eta();
      int Ieta = (int)(tt_eta*10.);
      // compute identifier
      Identifier emId  = m_TTtool->identifier(tt_eta, tt_phi, 0);
      Identifier hadId = m_TTtool->identifier(tt_eta, tt_phi, 1);
      // get cell energy sums for this TT
      double emTTCellsEnergy = m_cells2tt->energy(emId);
      double hadTTCellsEnergy = m_cells2tt->energy(hadId);
      // add noise
      if (m_towerNoise) emTTCellsEnergy  += CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,emNoise(tt_eta));
      if (m_towerNoise) hadTTCellsEnergy += CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,hadNoise(tt_eta));
      // create and fill tower
      InternalTriggerTower* TT1 = findTriggerTower(tt_phi, tt_eta, key);
      TT1->addEMPeak(emTTCellsEnergy*emScale*m_sinThetaHash[Ieta]);
      TT1->addHadPeak(hadTTCellsEnergy*hdScale*m_sinThetaHash[Ieta]);
      
      // repeat for -eta
      key = testKey.ttKey(temp_phi,-temp_eta);
      tt_phi=testKey.phi();
      tt_eta=testKey.eta();
      // compute identifier
      emId  = m_TTtool->identifier(tt_eta, tt_phi, 0);
      hadId = m_TTtool->identifier(tt_eta, tt_phi, 1);
      // get cell energy sums for this TT
      emTTCellsEnergy = m_cells2tt->energy(emId);
      hadTTCellsEnergy = m_cells2tt->energy(hadId);
      // add noise
      if (m_towerNoise) emTTCellsEnergy  += CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,emNoise(tt_eta));
      if (m_towerNoise) hadTTCellsEnergy += CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,hadNoise(tt_eta));
      // create and fill tower
      InternalTriggerTower* TT2 = findTriggerTower(tt_phi, tt_eta, key);
      TT2->addEMPeak(emTTCellsEnergy*emScale*m_sinThetaHash[Ieta]);
      TT2->addHadPeak(hadTTCellsEnergy*hdScale*m_sinThetaHash[Ieta]);
    
    }  // end phi loop
  }  // end eta loop
        
        
  // FCAL, |eta| > 3.2
  for (int ieta = 0; ieta < 4; ieta++) {   // loop over one side in eta
    double temp_eta = 3.4125 + (ieta * 0.425);
    double emScale = calib(temp_eta, EmB);
    double hdScale = calib(temp_eta, Tile);
    
    for (int iphi = 0; iphi < 16; iphi++) {
      double temp_phi = (iphi + 0.5)*(M_PI/8.);

      // Get corresponding tower key and coordinates
      unsigned int key = testKey.ttKey(temp_phi,temp_eta);
      double tt_phi=testKey.phi();
      double tt_eta=testKey.eta();
      int Ieta = (int)(tt_eta*10.);
      // compute identifier
      Identifier emId  = m_TTtool->identifier(tt_eta, tt_phi, 0);
      Identifier hadId = m_TTtool->identifier(tt_eta, tt_phi, 1);
      // get cell energy sums for this TT
      double emTTCellsEnergy = m_cells2tt->energy(emId);
      double hadTTCellsEnergy = m_cells2tt->energy(hadId);
      // add noise
      if (m_towerNoise) emTTCellsEnergy  += CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,emNoise(tt_eta));
      if (m_towerNoise) hadTTCellsEnergy += CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,hadNoise(tt_eta));
      // create and fill tower
      InternalTriggerTower* TT1 = findTriggerTower(tt_phi, tt_eta, key);
      TT1->addEMPeak(emTTCellsEnergy*emScale*m_sinThetaHash[Ieta]);
      TT1->addHadPeak(hadTTCellsEnergy*hdScale*m_sinThetaHash[Ieta]);
      
      // repeat for -eta
      key = testKey.ttKey(temp_phi,-temp_eta);
      tt_phi=testKey.phi();
      tt_eta=testKey.eta();
      // compute identifier
      emId  = m_TTtool->identifier(tt_eta, tt_phi, 0);
      hadId = m_TTtool->identifier(tt_eta, tt_phi, 1);
      // get cell energy sums for this TT
      emTTCellsEnergy = m_cells2tt->energy(emId);
      hadTTCellsEnergy = m_cells2tt->energy(hadId);
      // add noise
      if (m_towerNoise) emTTCellsEnergy  += CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,emNoise(tt_eta));
      if (m_towerNoise) hadTTCellsEnergy += CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,hadNoise(tt_eta));
      // create and fill tower
      InternalTriggerTower* TT2 = findTriggerTower(tt_phi, tt_eta, key);
      TT2->addEMPeak(emTTCellsEnergy*emScale*m_sinThetaHash[Ieta]);
      TT2->addHadPeak(hadTTCellsEnergy*hdScale*m_sinThetaHash[Ieta]);
    
    }  // end phi loop
  }  // end eta loop
}

/** gets TriggerTowers from input collection and copies ADC digits into internaTriggerTowers
    for reprocessing */

StatusCode LVL1::TriggerTowerMaker::getTriggerTowers()
{
  /// Find  TriggerTowers in TES
  const t_TTCollection* inputTTs;
  
  // Give up now if the container doesn't exist
  if (!evtStore()->contains<t_TTCollection>(m_inputTTLocation)) {
    ATH_MSG_WARNING
      ( "getTriggerTowers: no input TriggerTowerContainer found in SG at "
        << m_inputTTLocation );
    return StatusCode::FAILURE;
  }

  StatusCode sc = evtStore()->retrieve(inputTTs, m_inputTTLocation);
  ATH_MSG_DEBUG ( "Retrieve input TriggerTowers, sc = " << sc );
        
  if (msgLvl(MSG::DEBUG)) {
    if (sc==StatusCode::FAILURE) {
      ATH_MSG_DEBUG( "Can't find input TriggerTowers - stopping processing" );
    }else{
      //okay so we have some towers
      ATH_MSG_VERBOSE( "Found " << inputTTs->size() << " input TriggerTowers" );
    }
  }
  
  t_TTCollection::const_iterator it = inputTTs->begin();
  for ( ; it != inputTTs->end(); ++it) {
    unsigned int key = (*it)->key();
    double eta = (*it)->eta();
    double phi = (*it)->phi();
    std::vector<int> emADC = (*it)->emADC();
    std::vector<int> hadADC = (*it)->hadADC();

    InternalTriggerTower* TT = findTriggerTower(phi, eta, key);
    TT->addEMADC(emADC);
    TT->addHadADC(hadADC);        
  }
  
  return StatusCode::SUCCESS;
} // end of getTriggerTowers()

/** fetches LAr & Tile calorimeter towers */

StatusCode LVL1::TriggerTowerMaker::getCaloTowers()
{
  // Find LAr towers in TES
  StatusCode sc1;
  const DataHandle<LArTTL1Container> EMTowers;
  if (!evtStore()->contains<LArTTL1Container>(m_EmTTL1ContainerName)) {
    ATH_MSG_WARNING ( "EM LArTTL1Container not found" );
    sc1 = StatusCode::FAILURE;
  }
  else {
    sc1 = evtStore()->retrieve(EMTowers, m_EmTTL1ContainerName);
  }
    
  StatusCode sc2;
  const DataHandle<LArTTL1Container> HECTowers;
  if (!evtStore()->contains<LArTTL1Container>(m_HadTTL1ContainerName)) {
    ATH_MSG_WARNING ( "Had LArTTL1Container not found" );
    sc2 = StatusCode::FAILURE;
  }
  else {
    sc2 = evtStore()->retrieve(HECTowers, m_HadTTL1ContainerName);
  }

  // Find Tile towers in TES
  StatusCode sc3;
  const DataHandle<TileTTL1Container> TileTowers;
  if (!evtStore()->contains<TileTTL1Container>(m_TileTTL1ContainerName)) {
    ATH_MSG_WARNING ( "Tile TTL1Container not found" );
    sc3 = StatusCode::FAILURE;
  }
  else {
    sc3 = evtStore()->retrieve(TileTowers, m_TileTTL1ContainerName);
  }
  
  if ( m_requireAllCalos && ( (sc1==StatusCode::FAILURE) || 
                              (sc2==StatusCode::FAILURE) || 
                              (sc3==StatusCode::FAILURE) ) ){
    ATH_MSG_WARNING( "Can't find calo towers - stopping processing"<< endmsg
        << "Found Em  LArTTL1 : "<<sc1<< endmsg
        << "Found Had LArTTL1 : "<<sc2<< endmsg
        << "Found TileTTL1    : "<<sc3<< endmsg);
    return StatusCode::FAILURE;
    
  } else {
  
    // lets now try to create some trigger towers
    if (sc1 == StatusCode::SUCCESS) {
       ATH_MSG_DEBUG ( "Found " << EMTowers->size() << " EM towers " );
       processLArTowers( EMTowers );
    }
    if (sc2 == StatusCode::SUCCESS) {
       ATH_MSG_DEBUG ( "Found "<< HECTowers->size() << " HEC Towers" ) ;
       processLArTowers( HECTowers );
    }
    if (sc3 == StatusCode::SUCCESS) {
       ATH_MSG_DEBUG ( "Found "<< TileTowers->size() << " Tile Towers" ) ;
       processTileTowers( TileTowers );
    }
        
  }// end else if (! cells)
  
  return StatusCode::SUCCESS;
}

/** steps over Calo towers and creates/fills trigger towers */

void LVL1::TriggerTowerMaker::processLArTowers(const LArTTL1Container * towers)
{
  // the TriggerTowerKey object provides the key for each trigger tower,
  // depending on its eta,phi coords. The key is an integer number that uniquely
  // identifies each tower.
  
  TriggerTowerKey testKey(0.0, 0.0);
  // Setup done. Now we step through the towers
  
  LArTTL1Container::const_iterator tower ;
  int towerNumber=0;
  for( tower  = towers->begin(); tower != towers->end(); ++tower ){
    
    ATH_MSG_VERBOSE
      ( "Looking at retrieved tower number "<<towerNumber++<<" ***********" ) ;
    
    // Obtain identifier 
    Identifier id = (*tower)->ttOfflineID();
    double cal = calib(id, m_caloId);
 
    // Tower amplitudes and type
    int nsamples = (*tower)->nsamples();
    std::vector<float> tower_amps = (*tower)->samples();
    
    // Now calibrate tower_amps then fill TriggerTower amplitude vector
    // TTL1 should have 7 samples, but this little kludge handles other
    // eventualities, provided peak is in centre of the pulse
    
    int offset = (nsamples - (m_FirLength+2))/2;
    std::vector<double> amps(m_FirLength+2);
    
    ATH_MSG_VERBOSE ( "nsamples = " 
                      << nsamples << " offset = " << offset );
        
    for (int i = 0; i < m_FirLength+2; i++) {
      int j = i + offset;
      if (j >= 0 && j < nsamples) {
        amps[i] = tower_amps[j]*cal; // scale amps to uniform MeV calibration
      }
      else {
        amps[i] = 0.;
      }      
      ATH_MSG_VERBOSE( "amps[" << i << "] = " << amps[i] );
    }
    
    // Create TriggerTower
    double tower_eta = IDeta(id, m_caloId);
    double tower_phi = IDphi(id, m_caloId); 
    unsigned int key = testKey.ttKey(tower_phi,tower_eta);
    double tt_phi=testKey.phi();
    double tt_eta=testKey.eta();  
    InternalTriggerTower* TT = findTriggerTower(tt_phi, tt_eta, key);

    // Fill TT amplitude vector
    if (m_caloId->sampling(id)==0) {
       TT->addEMAmps(amps);
       
       ATH_MSG_VERBOSE ( "Add to EM tower at (" << tt_eta 
                         << ", " << tt_phi << "), key = " << key );
    } else {
       TT->addHadAmps(amps);
   
       ATH_MSG_VERBOSE ( "Add to HAD tower at (" << tt_eta 
                         << ", " << tt_phi << "), key = " << key );
    }
        
  } // end for loop
              
  return;
}                             
    
void LVL1::TriggerTowerMaker::processTileTowers(const TileTTL1Container * towers)
{
  // the TriggerTowerKey object provides the key for each trigger tower,
  // depending on its eta,phi coords. The key is an integer number that uniquely
  // identifies each tower.
  
  TriggerTowerKey testKey(0.0, 0.0);

  // Algorithm relies on valid hash table. Abort if this is not present
  if (m_sinThetaHash == 0x0) {
    ATH_MSG_ERROR( "No hash table present. Abort processing" );
    return;
  }

  // Step over all towers
  
  TileTTL1Container::const_iterator tower ;
  int towerNumber=0;
  for( tower  = towers->begin(); tower != towers->end(); ++tower ){
    ATH_MSG_VERBOSE
      ( "Looking at retrieved tower number "<<towerNumber++<<" ***********"  ) ;

    // Obtain identifier     
    Identifier id = (*tower)->TTL1_ID();
    double cal = calib(id, m_caloId)*m_TileToMeV;
    
    // Check this tower is used by the trigger
    if (TTL1type(id, m_caloId) == normal) {      // don't use gap or mbias scinitllators
    
      // need to convert tower energy to E_t
   
      double tower_eta = IDeta(id, m_caloId);
      int Ieta = (int)( fabs(tower_eta)*10.0 );
      if( Ieta >= (int)m_maxIetaBins){
        ATH_MSG_WARNING( "TileTTL1 with invalid index for m_sinThetaHash: Ieta = " << Ieta );
        Ieta = 0;
      }
 
      /* Extract amplitudes and rescale according to tower eta */
      int nsamples = (*tower)->nsamples();
      std::vector<float> tower_amps = (*tower)->fsamples();
    
      /* Debug message */
      ATH_MSG_VERBOSE ( " nsamples = " << nsamples );
    
      // Want 7 samples, but this little kludge allows us to accept other 
      // numbers, provided peak is in centre of the pulse
    
      int offset = (nsamples - (m_FirLength+2))/2;
      std::vector<double> amps(m_FirLength+2);
      for (int i = 0; i < 7; i++) {
        int j = i + offset;
        if (j >= 0 && j < nsamples) {
          amps[i] = (tower_amps[j]-m_TileTTL1Ped)*m_sinThetaHash[Ieta]*cal; // rescale to MeV
        }
        else {
          amps[i] = 0.;
        } 
      /* Debug message */
        ATH_MSG_VERBOSE ( "amps[" << i << "] = " << amps[i] );
      }
    
      // Create TriggerTower
      double tower_phi = IDphi(id, m_caloId); 
      unsigned int key = testKey.ttKey(tower_phi,tower_eta);
      double tt_phi=testKey.phi();
      double tt_eta=testKey.eta();
      InternalTriggerTower* TT = findTriggerTower(tt_phi, tt_eta, key);
    
      // Fill TT amplitude vector
      TT->addHadAmps(amps);
    
    } // end check on whether tower is used
  } // end for loop
              
  return;
}

/**Retrieve overlay digit container and merge with InternalTriggerTowers */

StatusCode LVL1::TriggerTowerMaker::overlay()
{
  ATH_MSG_VERBOSE ( "LVL1::TriggerTowerMaker::overlay()" ) ;

  // Get overlay container
  typedef PileUpMergeSvc::TimedList<t_TTCollection>::type OverlayDigitContList ;
  
  OverlayDigitContList digitContList;
  if (!(m_mergeSvc->retrieveSubEvtsData("TriggerTowers",digitContList).isSuccess())) {
       ATH_MSG_ERROR( " Cannot retrieve TriggerTowerContainer for random event overlay" );
       return StatusCode::FAILURE ;
  }
  ATH_MSG_DEBUG ( "Retrieved TriggerTowerContainer for random event overlay." ) ;
  if(0==digitContList.size())
    {
      ATH_MSG_WARNING( "digitContList.size()=" << digitContList.size() << ". Therefore cannot loop over overlay collection. Skipping this method!" );
      return StatusCode::SUCCESS;
    }
  OverlayDigitContList::iterator iDigitCont(digitContList.begin()) ;
  const t_TTCollection* overlayTowers = (iDigitCont->second);

  ATH_MSG_VERBOSE ( "Found " << overlayTowers->size() << " towers in overlay collection" ) ;
  ATH_MSG_VERBOSE ( "Initial number of InternalTriggerTowers = "
                    << m_IntTTContainer->size() );
  // Now do the overlay
  TriggerTowerKey testKey(0.0, 0.0);

  t_TTCollection::const_iterator tt = overlayTowers->begin();

  for ( ; tt != overlayTowers->end(); ++tt) {
    double ttEta = (*tt)->eta();
    double ttPhi = (*tt)->phi();
    int key = testKey.ttKey(ttPhi,ttEta);
    std::map<int, InternalTriggerTower*>::iterator test=m_IntTTContainer->find( key );
    if (test != m_IntTTContainer->end()) {
      ATH_MSG_VERBOSE ( "Add overlay to existing tower" );
      // Add overlay digits to simulated
      // Start with EM
      std::vector<int> emADC = test->second->EmADC();
      std::vector<int> emOverlay = (*tt)->emADC();
      int emOff = (emADC.size() - emOverlay.size())/2;
      for (int i = 0; i < (int)emOverlay.size(); ++i) {
        int j = i + emOff;
        if (j >= 0 && j < (int)emADC.size()) emADC[j] += int(emOverlay[i] - m_overlayPed);
      }
      // repeat for hadronic
      std::vector<int> hadADC = test->second->HadADC();
      std::vector<int> hadOverlay = (*tt)->hadADC();
      int hadOff = (hadADC.size() - hadOverlay.size())/2;
      for (int i = 0; i < (int)hadOverlay.size(); ++i) {
        int j = i + hadOff;
        if (j >= 0 && j < (int)hadADC.size()) hadADC[j] += int(hadOverlay[i] - m_overlayPed);
      }
      // Replace amplitudes in ITT
      test->second->addEMADC(emADC);
      test->second->addHadADC(hadADC);
    }
    else {
      ATH_MSG_VERBOSE ( "New tower from overlay" );
      // Create new ITT and add overlay data
      InternalTriggerTower* TT=new InternalTriggerTower(ttPhi,ttEta, key);
      // Fill em amplitude vector from overlay digits
      std::vector<int> emADC(7);
      std::vector<int> emDig = (*tt)->emADC();
      int emOff = (emADC.size() - emDig.size())/2;
      for (int i = 0; i < (int)emDig.size(); ++i) {
        int j = i + emOff;
        if (j >= 0 && j < (int)emADC.size()) emADC[j] += int(emDig[i] - m_overlayPed + m_pedVal);
      }
      // repeat for hadronic
      std::vector<int> hadADC(7);
      std::vector<int> hadDig = (*tt)->hadADC();
      int hadOff = (hadADC.size() - hadDig.size())/2;
      for (int i = 0; i < (int)hadDig.size(); ++i) {
        int j = i + hadOff;
        if (j >= 0 && j < (int)hadADC.size()) hadADC[j] += int(hadDig[i] - m_overlayPed + m_pedVal);
      }
      // add amplitudes
      TT->addEMADC(emADC);
      TT->addHadADC(hadADC);
      // add new ITT to container
      m_IntTTContainer->insert(std::map<int, InternalTriggerTower*>::value_type(key,TT)); 
    }
  }
  ATH_MSG_VERBOSE ( "Final number of InternalTriggerTowers = "
                    << m_IntTTContainer->size() );
  
  return StatusCode::SUCCESS;
}

/** Digitize pulses and store results back in InternalTriggerTowers */

void LVL1::TriggerTowerMaker::digitize()
{
  
  
  // Iterator for the InternalTriggerTower map
  std::map<int, InternalTriggerTower*>::iterator it;

  // Loop over all existing internal towers and digitize pulses
  
  for( it=m_IntTTContainer->begin(); it!=m_IntTTContainer->end(); ++it ){
    // First process EM layer 
    L1CaloCoolChannelId emId = channelId(it->second->eta(), it->second->phi(), 0); 
    std::vector<int> emDigits = ADC(emId, it->second->EmAmps()); // ADC simulation
    // Then the hadronic    
    L1CaloCoolChannelId hadId = channelId(it->second->eta(), it->second->phi(), 1);
    std::vector<int> hadDigits = ADC(hadId, it->second->HadAmps()); // ADC simulation
    // and put the results back into the ITT
    it->second->addEMADC(emDigits);
    it->second->addHadADC(hadDigits);
  }
  
  return;
}

/** Emulate FIR filter, bunch-crossing identification & LUT, and create & fill TriggerTowers. */

void LVL1::TriggerTowerMaker::preProcess()
{
  // Pedestal Correction: Get the BCID number
  unsigned int eventBCID=0;
  const EventIDBase* evid = EventIDFromStore( evtStore() );
  if( evid ) {
    eventBCID = evid->bunch_crossing_id();
  }else{
    ATH_MSG_ERROR(" Unable to retrieve EventInfo from StoreGate ");
  }
 
  // Iterator for the InternalTriggerTower map
  std::map<int, InternalTriggerTower*>::iterator it;

  // Loop over all existing internal towers and simulate preprocessor functions
  
  for( it=m_IntTTContainer->begin(); it!=m_IntTTContainer->end(); ++it ){
  
    unsigned int key = it->second->key();
    // First process EM layer
  
    // Use eta coordinate of tower to deduce type
    double absEta = fabs(it->second->eta());

    int towerType;
    if (absEta<1.5) {
      towerType = EmB;
    }
    else if (absEta<3.2) {
      towerType = EmEC;
    }
    else {
      towerType = FcalE;
    }
    
    /// vectors to store intermediate results of preprocessing
    std::vector<int> fir;
    std::vector<int> lutIn;
    std::vector<int> lutOut;
    std::vector<int> BCIDOut;

    /// vectors to store EM results
    std::vector<int> emEt;
    std::vector<int> emBCID;
    
    /// retrieve digits
    std::vector<int> emDigits = (m_timeslicesLUT)
                              ? multiSliceDigits(it->second, towerType)
                              : it->second->EmADC();       // ADC counts
    /// disabled tower?
    bool disabled = (m_badEMTowers.find( key ) != m_badEMTowers.end());

    /// get LUT slope for this channel
    L1CaloCoolChannelId emId = channelId(it->second->eta(), it->second->phi(), 0);
    int coolId = emId.id();
    int slope = 1;
    std::map<unsigned int, int>::iterator itSlope=m_slope.find( coolId );
    if (itSlope != m_slope.end()) slope = itSlope->second;
    else ATH_MSG_ERROR( "No slope entry for channel " << coolId );
    
    int pedsub = int(m_pedVal);
    std::map<unsigned int, int>::iterator itPedSub=m_pedSub.find( coolId );
    if (itPedSub != m_pedSub.end()) pedsub = itPedSub->second;
    else ATH_MSG_ERROR( "No pedSub entry for channel " << coolId );
    
    int thresh = int(m_emThresh);
    std::map<unsigned int, int>::iterator itThresh=m_channelNoiseCuts.find( coolId );
    if (itThresh != m_channelNoiseCuts.end()) thresh = itThresh->second;
    else ATH_MSG_ERROR( "No threshold entry for channel " << coolId );
      
    // Pedestal Correction
    int element=-1; // for both: em, had
    int firPedEm=0;
    if (m_elementFir) {
      /// Get integer eta bin
      float shiftedEta = it->second->eta() + 4.9;
      uint iEta = (uint)floor(shiftedEta*10.0);
      if (fabs(shiftedEta*10.0 - (uint)(iEta+1)) < 0.01) iEta++;
      if (fabs(shiftedEta) < 0.01) iEta = 0;
      /// Map integer eta bin to element
      element = IetaToElement(iEta, 0); // element range (0-32)
      if (element == -1) continue;
      element = m_nElement-element-1;

      m_nDrop[towerType] = m_nDropElement[0][element];
      slope = m_slopeElement[0][element];
      m_FIRCoeff[towerType][0] =  m_FIRCoeffElement[0][element][0];
      m_FIRCoeff[towerType][1] =  m_FIRCoeffElement[0][element][1];
      m_FIRCoeff[towerType][2] =  m_FIRCoeffElement[0][element][2];
      m_FIRCoeff[towerType][3] =  m_FIRCoeffElement[0][element][3];
      m_FIRCoeff[towerType][4] =  m_FIRCoeffElement[0][element][4];

      double pedvalue = m_pedVal;
      std::map<unsigned int, double>::iterator itPedValue=m_pedValue.find( coolId );
      if (itPedValue != m_pedValue.end()) pedvalue = itPedValue->second;
      else ATH_MSG_ERROR( "No pedValue entry for channel " << coolId );
      firPedEm = std::accumulate(m_FIRCoeff[towerType].begin(), m_FIRCoeff[towerType].end(), 0) * floor(pedvalue + 0.5);  
    }
               
    /// process channel
    m_TTtool->fir(emDigits, m_FIRCoeff[towerType], fir);
    if(m_correctFir && m_elementFir) {
      std::vector<int_least16_t> correction;
      m_TTtool->pedestalCorrection(fir, firPedEm, element, 0, eventBCID,
                                   m_lumiBlockMuTool->actualInteractionsPerCrossing(), correction);
    }
    m_TTtool->dropBits(fir, m_nDrop[towerType], lutIn);
    m_TTtool->lut(lutIn, slope, pedsub, thresh, int(m_pedVal), m_LUTStrategy, disabled, lutOut);
    m_TTtool->bcid(fir, emDigits, m_PeakFinderCond, m_SatLow[towerType], m_SatHigh[towerType], m_SatLevel, BCIDOut);

    int emPeak = lutOut.size()/2;   
    
    emEt.push_back(lutOut[emPeak]);                       // calibration LUT
    emBCID.push_back(BCIDOut[emPeak]);                    // BCID result
   
    int range;
    if (!(m_DecisionSource&0x1)) {                         // select energy range
      range = EtRange(emDigits[3], towerType);
    } 
    else {
      range = EtRange(fir[emPeak], towerType);
    }

    if (m_BcidDecision[range]&(0x1<<BCIDOut[emPeak])) {  // passed appropriate BCID
      if (m_SatOverride[range]&0x1) {                  // return LUT or saturation ET
        emEt[0] = m_SaturationValue;
      }
    }
    else {                                            // zero if fail BCID
      emEt[0] = 0;
    }
        
    /// then process hadronic layer
    
    if ( absEta < 1.5) {
      towerType = Tile;
    } else if ( absEta < 3.2 ) {
      towerType = Hec;
    } else {
      towerType = FcalH;
    }

    /// vectors to store hadronic results
    std::vector<int> hadEt;
    std::vector<int> hadBCID;

    /// retrieve hadronic digits
    std::vector<int> hadDigits = (m_timeslicesLUT)
                               ? multiSliceDigits(it->second, towerType)
                               : it->second->HadADC();       // ADC counts
                               
    /// disabled tower?
    disabled = (m_badHadTowers.find( key ) != m_badHadTowers.end());
    
    /// lut slope
    L1CaloCoolChannelId hadId = channelId(it->second->eta(), it->second->phi(), 1);
    coolId = hadId.id();
    slope = 1;
    itSlope=m_slope.find( coolId );
    if (itSlope != m_slope.end()) slope = itSlope->second;
    else ATH_MSG_ERROR( "No slope entry for channel " << coolId );
    
    pedsub = int(m_pedVal);
    itPedSub=m_pedSub.find( coolId );
    if (itPedSub != m_pedSub.end()) pedsub = itPedSub->second;
    else ATH_MSG_ERROR( "No pedSub entry for channel " << coolId );
    
    thresh = int(m_hadThresh);
    itThresh=m_channelNoiseCuts.find( coolId );
    if (itThresh != m_channelNoiseCuts.end()) thresh = itThresh->second;
    else ATH_MSG_ERROR( "No threshold entry for channel " << coolId );
      
    int firPedHad=0;
    if (m_elementFir) {
      /// Get integer eta bin
      float shiftedEta = it->second->eta() + 4.9;
      uint iEta = (uint)floor(shiftedEta*10.0);
      if (fabs(shiftedEta*10.0 - (uint)(iEta+1)) < 0.01) iEta++;
      if (fabs(shiftedEta) < 0.01) iEta = 0;
      /// Map integer eta bin to element
      element = IetaToElement(iEta, 1); // element range (0-32)
      element = m_nElement-element-1;

      // silence Coverity ...
      if(element < 0 || element > 32) {
	  throw std::out_of_range("Element out-of-range");
      }

      m_nDrop[towerType] = m_nDropElement[1][element];
      slope = m_slopeElement[1][element];
      if (element>28 && element<33) { // FCal23 mapping
         pedsub = m_offsetElement[1][element];
         thresh = m_threshElement[1][element];
      }      
      m_FIRCoeff[towerType][0] =  m_FIRCoeffElement[1][element][0];
      m_FIRCoeff[towerType][1] =  m_FIRCoeffElement[1][element][1];
      m_FIRCoeff[towerType][2] =  m_FIRCoeffElement[1][element][2];
      m_FIRCoeff[towerType][3] =  m_FIRCoeffElement[1][element][3];
      m_FIRCoeff[towerType][4] =  m_FIRCoeffElement[1][element][4];

      double pedvalue = m_pedVal;
      std::map<unsigned int, double>::iterator itPedValue=m_pedValue.find( coolId );
      if (itPedValue != m_pedValue.end()) pedvalue = itPedValue->second;
      else ATH_MSG_ERROR( "No pedValue entry for channel " << coolId );
      firPedHad = std::accumulate(m_FIRCoeff[towerType].begin(), m_FIRCoeff[towerType].end(), 0) * floor(pedvalue + 0.5);
    }
                   
    /// process tower
    m_TTtool->fir(hadDigits, m_FIRCoeff[towerType], fir);
    if(m_correctFir && m_elementFir) {
      std::vector<int_least16_t> correction;
      m_TTtool->pedestalCorrection(fir, firPedHad, element, 1, eventBCID,
                                   m_lumiBlockMuTool->actualInteractionsPerCrossing(), correction);
    } else {
    }
    m_TTtool->dropBits(fir, m_nDrop[towerType], lutIn);
    m_TTtool->lut(lutIn, slope, pedsub, thresh, int(m_pedVal), m_LUTStrategy, disabled, lutOut);
    m_TTtool->bcid(fir, hadDigits, m_PeakFinderCond, m_SatLow[towerType], m_SatHigh[towerType], m_SatLevel, BCIDOut);
    
    int hadPeak = lutOut.size()/2;
    
    hadEt.push_back(lutOut[hadPeak]);                       // calibration LUT
    hadBCID.push_back(BCIDOut[hadPeak]);
  
    if (!(m_DecisionSource&0x1)) {                            // identify BCID range
      range = EtRange(hadDigits[3],towerType);
    } 
    else {
      range = EtRange(fir[hadPeak],towerType);
    }
    
    if (m_BcidDecision[range]&(0x1<<BCIDOut[hadPeak])) {      // correct BCID for this range?
      if (m_SatOverride[range]&0x1) {                  
        hadEt[0] = m_SaturationValue;                         // return saturation if set
      }
    }
    else {                                              
      hadEt[0] = 0;                                           // zero if fail BCID
    }
  
    /// If either sample has passed BCID and threshold, create TriggerTower
    /// and add it to the map
    if (emEt[0] > 0 || hadEt[0] > 0 || (!m_ZeroSuppress)) {
      double eta = it->second->eta();
      double phi = it->second->phi();
      /// In simulation external BCID is always zero, but for consistency with
      /// data we need to add it to the TriggerTower
      std::vector<int> emExt;
      for (unsigned int i = 0; i < emDigits.size(); i++) { emExt.push_back(0); }
      std::vector<int> hadExt;
      for (unsigned int i = 0; i < hadDigits.size(); i++) { hadExt.push_back(0); }
      
      int peak = 0;
      int emADCPeak = emDigits.size()/2;
      int hadADCPeak = hadDigits.size()/2;

      /// Create a new TriggerTower and add it to the output map
      LVL1::TriggerTower* tower = new TriggerTower(phi, eta, key,
                             emDigits,  emEt,  emExt,  emBCID, 0, peak, emADCPeak,
                            hadDigits, hadEt, hadExt, hadBCID, 0, peak, hadADCPeak);
      //LVL1::TriggerTower* tower = new TriggerTower(phi, eta,
      //                       key, emChannel.id(), hadChannel.id(),
      //                       emDigits,  emEt,  emExt,  emBCID, 0, peak, emADCPeak,
      //                      hadDigits, hadEt, hadExt, hadBCID, 0, peak, hadADCPeak);
      m_TTContainer->insert(std::map<int, TriggerTower*>::value_type(key,tower)); 
    }

    // Finished with this InternalTriggerTower now:

    delete it->second;
        
  }
  return;
}

} // end of namespace bracket

/** returns a pointer to a Trigger Tower, an existing one if it can be found,
otherwise create one and return the pointer to that. */

LVL1::InternalTriggerTower* LVL1::TriggerTowerMaker::findTriggerTower(double tt_phi, double tt_eta, unsigned int key)
{
  std::map<int, InternalTriggerTower*>::iterator it=m_IntTTContainer->find( key );
  LVL1::InternalTriggerTower* TT=0;
  if (it == m_IntTTContainer->end()){
    // no TT yet. Create it!
    ATH_MSG_VERBOSE ( "Creating Internal Trigger Tower at ("
                      << tt_phi << " , " << tt_eta << ")" ) ;
    TT=new InternalTriggerTower(tt_phi,tt_eta, key);
    m_IntTTContainer->insert(std::map<int, InternalTriggerTower*>::value_type(key,TT)); //and put it in the map.
  }else{
    ATH_MSG_VERBOSE ( "Internal Trigger Tower already exists! " ) ;
    TT=(it->second);
  } // end else
  return TT;
}

/** Step over all tower locations and add noise. If tower does not exist, create it */

void LVL1::TriggerTowerMaker::addNoise()
{
  // the TriggerTowerKey object provides the key for each trigger tower,
  // depending on its eta,phi coords. The key is an integer number that uniquely
  // identifies each tower.
  TriggerTowerKey testKey(0.0, 0.0);

  // Step over all tower locations
  // Central region (50*64 towers)
  for (int ieta = 0; ieta < 25; ieta++) {   // loop over one side in eta
    double temp_eta = (ieta + 0.5) * 0.1;
    double emScale = calib(temp_eta, EmB);
    double hdScale = calib(temp_eta, Tile);
    
    for (int iphi = 0; iphi < 64; iphi++) {
      double temp_phi = (iphi + 0.5)*(M_PI/32.);

      // Get corresponding tower key and coordinates
      unsigned int key = testKey.ttKey(temp_phi,temp_eta);
      double tt_phi=testKey.phi();
      double tt_eta=testKey.eta();

      // Get matching tower
      InternalTriggerTower* TTr = findTriggerTower(tt_phi, tt_eta, key);
      
      // add noise      
      double emnoise = CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,emNoise(tt_eta));
      TTr->addEMPeak(emnoise*emScale*m_sinThetaHash[ieta]);
      
      double hdnoise = CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,hadNoise(tt_eta));     
      TTr->addHadPeak(hdnoise*hdScale*m_sinThetaHash[ieta]);
      
      // now corresponding tower at -ive eta
      key = testKey.ttKey(temp_phi,-temp_eta);
      tt_phi=testKey.phi();
      tt_eta=testKey.eta();

      InternalTriggerTower* TTl = findTriggerTower(tt_phi, tt_eta, key);
  
      emnoise = CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,emNoise(tt_eta));
      TTl->addEMPeak(emnoise*emScale*m_sinThetaHash[ieta]);
      
      hdnoise = CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,hadNoise(tt_eta));  
      TTl->addHadPeak(hdnoise*hdScale*m_sinThetaHash[ieta]);

    }  // end phi loop
  }  // end eta loop
  
  // outer region (4*32 towers each end)
  for (int ieta = 0; ieta < 4; ieta++) {
    double temp_eta = 2.55 + (ieta * 0.2);
    double emScale = calib(temp_eta, EmB);
    double hdScale = calib(temp_eta, Tile);
    
    for (int iphi = 0; iphi < 32; iphi++) {
      double temp_phi = (iphi + 0.5)*(M_PI/16.);

// Get corresponding tower key and coordinates
      unsigned int key = testKey.ttKey(temp_phi,temp_eta);
      double tt_phi=testKey.phi();
      double tt_eta=testKey.eta();
      int Ieta = (int)( tt_eta*10.0);

// Get matching tower
      InternalTriggerTower* TTr = findTriggerTower(tt_phi, tt_eta, key);

// add noise      
      double emnoise = CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,emNoise(tt_eta));
      TTr->addEMPeak(emnoise*emScale*m_sinThetaHash[Ieta]);
      
      double hdnoise = CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,hadNoise(tt_eta));     
      TTr->addHadPeak(hdnoise*hdScale*m_sinThetaHash[Ieta]);
      
// now corresponding tower at -ive eta
      key = testKey.ttKey(temp_phi,-temp_eta);
      tt_phi=testKey.phi();
      tt_eta=testKey.eta();

      InternalTriggerTower* TTl = findTriggerTower(tt_phi, tt_eta, key);

      emnoise = CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,emNoise(tt_eta));
      TTl->addEMPeak(emnoise*emScale*m_sinThetaHash[Ieta]);
      hdnoise = CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,hadNoise(tt_eta));  
      TTl->addHadPeak(hdnoise*hdScale*m_sinThetaHash[Ieta]);


    }  // end phi loop
  }  // end eta loop

  // FCAL region (4*16 towers each end)
  for (int ieta = 0; ieta < 4; ieta++) {
    double temp_eta = 3.4125 + (ieta * 0.425);
    double emScale = calib(temp_eta, EmB);
    double hdScale = calib(temp_eta, Tile);
    
    for (int iphi = 0; iphi < 16; iphi++) {
      double temp_phi = (iphi + 0.5)*(M_PI/8.);

// Get corresponding tower key and coordinates
      unsigned int key = testKey.ttKey(temp_phi,temp_eta);
      double tt_phi=testKey.phi();
      double tt_eta=testKey.eta();
      int Ieta = (int)( tt_eta*10.0);

// Get matching tower
      InternalTriggerTower* TTr = findTriggerTower(tt_phi, tt_eta, key);

// add noise      
      double emnoise = CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,emNoise(tt_eta));
      TTr->addEMPeak(emnoise*emScale*m_sinThetaHash[Ieta]);
      
      double hdnoise = CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,hadNoise(tt_eta));     
      TTr->addHadPeak(hdnoise*hdScale*m_sinThetaHash[Ieta]);
      
// now corresponding tower at -ive eta
      key = testKey.ttKey(temp_phi,-temp_eta);
      tt_phi=testKey.phi();
      tt_eta=testKey.eta();

      InternalTriggerTower* TTl = findTriggerTower(tt_phi, tt_eta, key);

      emnoise = CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,emNoise(tt_eta));
      TTl->addEMPeak(emnoise*emScale*m_sinThetaHash[Ieta]);
      hdnoise = CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,hadNoise(tt_eta));  
      TTl->addHadPeak(hdnoise*hdScale*m_sinThetaHash[Ieta]);


    }  // end phi loop
  }  // end eta loop
}

double LVL1::TriggerTowerMaker::emNoise(double eta)
{
  double noise = 0.;
  
  std::vector<double>::iterator EmRegIter = m_EtaEmNoise.begin();
  std::vector<double>::iterator EmRMSIter = m_EmNoise.begin();
  
  for (; EmRegIter!=m_EtaEmNoise.end(); EmRegIter++, EmRMSIter++) {
    if (fabs(eta)>*EmRegIter) noise = *EmRMSIter;
  }  
  return noise;
}

double LVL1::TriggerTowerMaker::hadNoise(double eta)
{
  double noise = 0.;
  
  std::vector<double>::iterator HadRegIter = m_EtaHadNoise.begin();
  std::vector<double>::iterator HadRMSIter = m_HadNoise.begin();
  
  for (; HadRegIter!=m_EtaHadNoise.end(); HadRegIter++, HadRMSIter++) {
    if (fabs(eta)>*HadRegIter) noise = *HadRMSIter;
  }  
  return noise;
}

std::vector<int> LVL1::TriggerTowerMaker::ADC(L1CaloCoolChannelId channel, const std::vector<double>& amps)
{     
  double adcCal = CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,1.,m_gainCorr);
  double ped = m_pedVal; // Give it a default in case of trouble

  int coolId = channel.id();
  std::map<unsigned int, double>::iterator itPed=m_pedValue.find( coolId );
  if (itPed != m_pedValue.end()) ped = itPed->second;

  std::vector<int> digits;

  int nSamples = amps.size();   
  for (int i=0; i<nSamples; i++) {
    double adcNoise = CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,m_adcVar);
    int digit = int( (amps[i]*adcCal/m_adcStep) + ped + adcNoise);
    if (digit > m_adcMax) digit = m_adcMax;
    if (digit < 0)        digit = 0;
    digits.push_back(digit);
  }
  
  return digits;
  
}

int LVL1::TriggerTowerMaker::EtRange(int et, int type)
{
  if (et < m_EnergyLevelLow[type])  return 0;
  if (et < m_EnergyLevelHigh[type]) return 1;
  return 2;
}

double LVL1::TriggerTowerMaker::IDeta(const Identifier& id, const CaloLVL1_ID* l1id)
{
  int region = l1id->region(id);
  int ieta = l1id->eta(id);
  int sign = l1id->pos_neg_z(id);
  
  double gran[4] = {0.1, 0.2, 0.1, 0.425};
  double offset[4] = {0., 2.5, 3.1, 3.2};
  double eta;
  
  if (region>=0 && region<=3) {
    eta = sign* ( ( (ieta+0.5) * gran[region] ) + offset[region] );
  }
  else {
    eta = 0.;
  }
  
  return eta;
}


double LVL1::TriggerTowerMaker::IDphi(const Identifier& id, const CaloLVL1_ID* l1id)
{
  Identifier regId = l1id->region_id(id);
  
  double phiMax = l1id->phi_max(regId);
  int iphi = l1id->phi(id);
      
  double phi = (iphi+0.5)*2*M_PI/(phiMax+1);
  
  return phi;
}


double LVL1::TriggerTowerMaker::calib(const Identifier& id, const CaloLVL1_ID* l1id)
{
  if (m_DoNotCalibrate) return 1.; // If calibration tuning is disabled
  
  int sample = l1id->sampling(id);
  int region = l1id->region(id);
  int ieta = l1id->eta(id);
 
  if ( sample < (int)m_Calib.size() && region < (int)m_Calib[sample].size()
       && ieta < (int)m_Calib[sample][region].size() ) {
    return m_Calib[sample][region][ieta]; 
  }
  else {
    return 0.; // error condition
  }
}

double LVL1::TriggerTowerMaker::calib(const double tt_eta, const TowerTypes TTType )
{ 
  if (m_DoNotCalibrate) return 1.; // If calibration tuning is disabled
  
  double abseta = fabs(tt_eta);
  uint ieta = 0;
  uint myInd = 0;
  if( TTType == Tile || TTType == Hec || TTType == FcalH) myInd = 1;
  /* I know, hardcoded numbers are not nice.
     But this function will be removed asap.
     So, I will do so.
  */
  if (abseta >= 0.0 && abseta < 2.5) {
    ieta = static_cast<uint>( abseta*(1./0.1) );
    if ( ieta >= 25) {
      ATH_MSG_WARNING( "calib: calculated index for  eta value: " << abseta
                       << " invalid: " << ieta  );
      return 1.0;
    }
    return m_Calib[myInd][0][ieta];
  } else if (abseta >= 2.5 && abseta < 3.1 ) {
    abseta -= 2.5;
    ieta =  static_cast<uint>( abseta*(1./0.2) );
    if (ieta >= 3) {
      ATH_MSG_WARNING( "calib: calculated index for  eta value: " << abseta
                       << " invalid: " << ieta  );
      return 1.0;
    }
    return m_Calib[myInd][1][ieta];
  } else if (abseta >= 3.1 && abseta < 3.2) {
    return m_Calib[myInd][2][0]; // we have only one TT in this region
  } else if (abseta >= 3.2) {
    abseta -= 3.2;
    ieta =  static_cast<uint>( abseta*(1./0.425) );
    if (ieta >= 4) {
      ATH_MSG_WARNING( "calib: calculated index for  eta value: " << abseta
                       << "invalid: " << ieta  );
      return 1.0;
    }
    return m_Calib[myInd][3][ieta];
  } else {
    ATH_MSG_WARNING( "calib (for calo cells): eta region (" 
                     << tt_eta << ") couldn't recognise. Skip calibration for this TT. " );
    return 1.0;
  }
  // cannot be reached
}

LVL1::TriggerTowerMaker::towerType LVL1::TriggerTowerMaker::TTL1type(const Identifier& id, const CaloLVL1_ID* l1id) const {
 
  if (l1id->is_tile(id)) { // do not use gap or mbias scintillators
    return normal; 
  }
  else {
    return gap_scint; 
  }
}

std::vector<int> LVL1::TriggerTowerMaker::multiSliceDigits(
                          const InternalTriggerTower* itt, int type)
{
  TriggerTowerMap_t::const_iterator pos = m_ADCContainer->find(itt->key());

  // Create full set of digits for all slices
  if (m_sliceLUT == 0 && pos == m_ADCContainer->end()) {
    int slicesEmADC  = (itt->EmAmps()).size();
    int slicesHadADC = (itt->HadAmps()).size();
    int fullEmSlices  = slicesEmADC + m_timeslicesLUT - 1;
    int fullHadSlices = slicesHadADC + m_timeslicesLUT - 1;
    std::vector<double> fullEmAmps(fullEmSlices);
    std::vector<double> fullHadAmps(fullHadSlices);
    if (m_towerNoise) sliceNoise(fullEmAmps, fullHadAmps, itt->eta());
    int offset = m_timeslicesLUT / 2;
    for (int sl = 0; sl < slicesEmADC; ++ sl) {
      fullEmAmps[sl + offset] = itt->EmAmps()[sl];
    }
    for (int sl = 0; sl < slicesHadADC; ++ sl) {
      fullHadAmps[sl + offset] = itt->HadAmps()[sl];
    }
    L1CaloCoolChannelId emId  = channelId(itt->eta(), itt->phi(), 0); 
    L1CaloCoolChannelId hadId = channelId(itt->eta(), itt->phi(), 1); 
    std::vector<int> fullEmDigits  = ADC(emId,fullEmAmps); // ADC simulation
    std::vector<int> fullHadDigits = ADC(hadId,fullHadAmps);
    // Save for later iterations/slices
    std::vector<int> dummyA(fullEmSlices);
    std::vector<int> dummyB(fullHadSlices);
    std::vector<int> dummyC(1);
    TriggerTower* tt = new TriggerTower(itt->phi(), itt->eta(), itt->key(),
                fullEmDigits, dummyC, dummyA, dummyC, 0, 0, fullEmSlices/2,
    fullHadDigits, dummyC, dummyB, dummyC, 0, 0, fullHadSlices/2);
    m_ADCContainer->insert(std::map<unsigned int, TriggerTower*>
                                    ::value_type(itt->key(), tt));
    pos = m_ADCContainer->find(itt->key());
  }

  // Select digits for this slice
  bool em = type == EmB || type == EmEC || type == FcalE;
  int slicesADC = (em) ? (itt->EmAmps()).size() : (itt->HadAmps()).size();
  std::vector<int> digits(slicesADC);
  if (pos != m_ADCContainer->end()) {
    const TriggerTower* tt = pos->second;
    int maxSlices = (em) ? (tt->emADC()).size() : (tt->hadADC()).size();
    for (int sl = 0; sl < slicesADC; ++ sl) {
      if (sl + m_sliceLUT < maxSlices) {
        digits[sl] = (em) ? tt->emADC()[sl + m_sliceLUT]
                    : tt->hadADC()[sl + m_sliceLUT];
      }
    }
  }
  return digits;
}

/** Function to compute L1CaloCoolChannelId from eta/phi/layer.
    Unlike L1TriggerTowerTool::channelID this function can cope with old geometries (hence
    the hard-coded numbers). So don't remove this until all ATLAS-CSC datasets are irrevokably
    deprecated */

L1CaloCoolChannelId LVL1::TriggerTowerMaker::channelId(double eta, double phi, int layer)
{
  int crate, module, channel;
  m_mappingTool->mapping(eta, phi, layer, crate, module, channel);
  int slot = module + 5;
  int pin = channel % 16;
  int asic = channel / 16;
  return L1CaloCoolChannelId(crate, L1CaloModuleType::Ppm, slot, pin, asic, false);
}

void LVL1::TriggerTowerMaker::setupADCMap()
{
  ATH_MSG_DEBUG ( "Processing LUT slice " << m_sliceLUT
        << " of 0-" << m_timeslicesLUT-1 << " slices" );

  m_ADCContainer = new TriggerTowerMap_t;

  if (m_sliceLUT) {
    const t_TTCollection* fullTTCol = 0;
    StatusCode sc = evtStore()->retrieve(fullTTCol, m_fullADCLocation);
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR("Unable to retrieve multi-slice ADC digits, key "
                    << m_fullADCLocation );
    } else {
      ATH_MSG_DEBUG ( "Retrieved multi-slice ADC digits, key "
                      << m_fullADCLocation );
      t_TTCollection::const_iterator pos  = fullTTCol->begin();
      t_TTCollection::const_iterator pose = fullTTCol->end();
      for (; pos != pose; ++pos) {
        m_ADCContainer->insert(TriggerTowerMap_t
                                  ::value_type((*pos)->key(), *pos));
      }
    }
  }
}

void LVL1::TriggerTowerMaker::sliceNoise(std::vector<double>& emAmps,
                                         std::vector<double>& hadAmps,
                                                     double eta)
{
  double emScale = calib(eta, EmB);
  double hdScale = calib(eta, Tile);
  double theta = 2.*atan(exp(-fabs(eta)));
  double sintheta = sin(theta);
// add noise      
  std::vector<double>::iterator pos  = emAmps.begin();
  std::vector<double>::iterator pose = emAmps.end();
  for (; pos != pose; ++pos) {
    double emnoise = CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,emNoise(eta));
    *pos += emnoise*emScale*sintheta;
  }
  pos  = hadAmps.begin();
  pose = hadAmps.end();
  for (; pos != pose; ++pos) {
    double hdnoise = CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,hadNoise(eta));     
    *pos += hdnoise*hdScale*sintheta;
  }
  return;
}

// Pedestal Correction
int LVL1::TriggerTowerMaker::IetaToElement(int eta, int layer)
{
  const int nBins = 16;
  int map[nBins] = {2,6,10,14,17,19,21,23,75,77,79,80,83,87,91,95};
  int element = -1;
  if (eta > 23 && eta < 74) {
    element = eta - 16;
  } else {
    for (int i = 0; i < nBins; i++) {
      if (eta == map[i]) {
        if (i < 8) element = i;
        else element = i + 50;
        break;
      }
    }
  }
  if      (layer == 1 && (element == 0 || element == 64)) element = 2; // FCal2-2
  else if (layer == 1 && (element == 1 || element == 65)) element = 0; // FCal3-2
  else if (layer == 1 && (element == 2 || element == 62)) element = 3; // FCal2-1
  else if (layer == 1 && (element == 3 || element == 63)) element = 1; // FCal3-1
  else if (element > 32) element = 65-element;

  return element;
}

/** InitLUTs method uses FIR coefficients and pulse profile to compute
    LUT calibration parameters:
    m_slopeElement 
    m_FIRsumElement
    m_nDropElement
    m_FIRCalElement
    m_FIRpedElement
    */

void LVL1::TriggerTowerMaker::initLUTsPrepare(const std::vector< std::vector< std::vector<double> > >& PulsesElement)
{
  

  // How to handle pedestal when truncating to 10 bit LUT range
  // float round = ( m_RoundPed ? 0.5 : 0. );

  // Loop over layers (em, had) and elements (eta bins) and compute parameters
  for (int layer = 0; layer < m_nLayer; layer++) {
    for (int element =0; element < m_nElement; element++) {

      /** Calculate calibration and pedestal subtraction after FIR
          Will redo pedestal channel-by-channel, but use central value here for
          LUT range check */
      double FIRcal = 0.;
      int FIRsum = 0;
      int FIRsat = 0;
      int slope  = 1;

      for (int sample = 0; sample < m_FirLength; sample++) {
        FIRsum += m_FIRCoeffElement[layer][element][sample];
        FIRcal += m_FIRCoeffElement[layer][element][sample]*PulsesElement[layer][element][sample]*m_MeV/m_adcStep;
        FIRsat += int( m_FIRCoeffElement[layer][element][sample]*( (PulsesElement[layer][element][sample]*(1023-m_pedVal)) + m_pedVal ) );

        ATH_MSG_INFO( " FIRco[" << sample << "] = "
            << m_FIRCoeffElement[layer][element]
            << "  ideal pulse amplitude = " << PulsesElement[layer][element][sample]
            );
      }
      double FIRped = FIRsum*m_pedVal;

      ATH_MSG_DEBUG( "Full precision: FIRcal = " << FIRcal
          << " FIRsat = " << FIRsat
          << " FIRped = " << FIRped );

      // Identify 10 bit range to use in LUT

      int bitMax = 0;
      for (int i = 0; i < 16; i++) {
        if ( FIRsat > (1<<i) ) bitMax++;
      }
      m_nDropElement[layer][element] = bitMax - 10; // keep a 10 bit range below 2**bitMAx
      if (m_nDropElement[layer][element] < 0) m_nDropElement[layer][element] = 0; // should not be possible anyway

      ATH_MSG_DEBUG( "Highest bit used = " << bitMax
          << " bits to drop = " << m_nDropElement[layer][element] );

      // Save the unshifted values for FIR energy range calibration
      m_FIRCalElement[layer][element] = int(FIRcal+0.5);
      m_FIRpedElement[layer][element] = int(FIRped+0.5);

      // for LUT operations on selected 10 bits

      FIRcal /= pow(2.,m_nDropElement[layer][element]);

      if (m_AutoCalibrateLUT) slope = int((4096./FIRcal)*(m_MeV/m_digitScale));

      m_slopeElement[layer][element] = slope;
      m_FIRsumElement[layer][element] = FIRsum;
      m_offsetElement[layer][element] = 0; // for FCal23 mapping
      m_threshElement[layer][element] = 0; // for FCal23 mapping
    }
  }
  return;
}
