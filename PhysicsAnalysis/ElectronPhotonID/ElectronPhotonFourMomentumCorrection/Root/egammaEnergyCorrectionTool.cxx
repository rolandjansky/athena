/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
///
/// Implementation of energy scale uncertainties, for 2010 data and beyond
///

#include <cassert>

#include "ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionTool.h"
#include "ElectronPhotonFourMomentumCorrection/GainTool.h"
#include "ElectronPhotonFourMomentumCorrection/eg_resolution.h"
#include "ElectronPhotonFourMomentumCorrection/get_MaterialResolutionEffect.h"
#include "ElectronPhotonFourMomentumCorrection/e1hg_systematics.h"

#include "egammaMVACalib/egammaMVACalib.h"

#include "egammaLayerRecalibTool/egammaLayerRecalibTool.h"

#ifndef ROOTCORE
#include "PathResolver/PathResolver.h"
#endif


namespace AtlasRoot {
  
  using std::string;
  
  egammaEnergyCorrectionTool::egammaEnergyCorrectionTool() 
      : m_calibInputs(0),
	m_esmodel(egEnergyCorr::UNDEFINED)
  {

    m_rootFile = 0;
#ifdef ROOTCORE
    m_rootFileName = "$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionData.root";
#else
    m_rootFileName = PathResolver::find_file ("ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionData.root", "XMLPATH");
#endif

    m_begRunNumber = 0;
    m_endRunNumber = 0;

    // corrections and systematics

    m_aPSNom = 0;
    m_daPSCor = 0;
    m_aS12Nom = 0;
    m_daS12Cor = 0;
    m_zeeNom = 0; 
    m_zeeSyst = 0;

    m_resNom = 0;
    m_resSyst = 0;
    
    m_trkSyst = 0;

    m_peakResData = 0;
    m_peakResMC = 0;

    m_dX_ID_Nom = 0;

    m_dX_IPPS_Nom = 0;
    m_dX_IPPS_LAr = 0;

    m_dX_IPAcc_Nom = 0;
    m_dX_IPAcc_LAr = 0;
    m_dX_IPAcc_G4 = 0;
    m_dX_IPAcc_GL1 = 0;

    m_dX_PSAcc_Nom = 0;
    m_dX_PSAcc_LAr = 0;
    m_dX_PSAcc_G4 = 0;

    m_psElectronEtaBins = 0;
    m_psElectronGraphs = 0;
    m_psUnconvertedEtaBins = 0;
    m_psUnconvertedGraphs = 0;
    m_psConvertedEtaBins = 0;
    m_psConvertedGraphs = 0;

    m_s12ElectronEtaBins = 0;
    m_s12ElectronGraphs = 0;
    m_s12UnconvertedEtaBins = 0;
    m_s12UnconvertedGraphs = 0;
    m_s12ConvertedEtaBins = 0;
    m_s12ConvertedGraphs = 0;

    m_pedestalL0 = 0;
    m_pedestalL1 = 0;
    m_pedestalL2 = 0;
    m_pedestalL3 = 0;

    m_convRadius = 0;
    m_convFakeRate = 0;
    m_convRecoEfficiency = 0;

    m_leakageConverted = 0;
    m_leakageUnconverted = 0;

    m_zeeES2Profile = 0;

    m_matElectronEtaBins = 0;

    m_G4OverAFII = 0;
    m_G4OverFrSh = 0;

    // tools

    m_mva_photon_tool = 0;
    m_mva_electron_tool = 0;
    m_layer_recalibration_tool = 0;
    m_gain_tool = 0;
    m_resolution_tool = 0;
    m_getMaterialDelta = 0;
    m_e1hg_tool = 0;

    // switches

    m_use_intermodule_correction = true;
    m_use_phi_unif_correction = true;
    m_use_layer2_recalibration = true;
    m_use_gain_correction = false;
    m_use_layer_correction = true;
    m_use_MVA_calibration = true;

    m_applyPSCorrection = true;
    m_applyS12Correction = true;

    m_initialized = false;
    m_debug = false;

  }
  

  egammaEnergyCorrectionTool::~egammaEnergyCorrectionTool() {    

    // Clean up

    if ( m_rootFile )                   delete m_rootFile;

    if ( m_mva_electron_tool )          delete m_mva_electron_tool;
    if ( m_mva_photon_tool )            delete m_mva_photon_tool;
    if ( m_calibInputs )                delete m_calibInputs;
    if ( m_gain_tool )                  delete m_gain_tool;
    if ( m_e1hg_tool )                  delete m_e1hg_tool;
    if ( m_resolution_tool )            delete m_resolution_tool;
    if ( m_getMaterialDelta)            delete m_getMaterialDelta;
    if ( m_layer_recalibration_tool )   delete m_layer_recalibration_tool;

    if ( m_aPSNom )                     delete m_aPSNom;
    if ( m_daPSCor )                    delete m_daPSCor;
    if ( m_aS12Nom )                    delete m_aS12Nom;
    if ( m_daS12Cor )                   delete m_daS12Cor;
    if ( m_zeeNom )                     delete m_zeeNom;
    if ( m_zeeSyst )                    delete m_zeeSyst;
    if ( m_resNom )                     delete m_resNom;
    if ( m_resSyst )                    delete m_resSyst;
    if ( m_peakResData )                delete m_peakResData;
    if ( m_peakResMC )                  delete m_peakResMC;
    if ( m_dX_ID_Nom )                  delete m_dX_ID_Nom;
    if ( m_dX_IPPS_Nom )                delete m_dX_IPPS_Nom;
    if ( m_dX_IPPS_LAr )                delete m_dX_IPPS_LAr;
    if ( m_dX_IPAcc_Nom )               delete m_dX_IPAcc_Nom;
    if ( m_dX_IPAcc_LAr )               delete m_dX_IPAcc_LAr;
    if ( m_dX_IPAcc_G4 )                delete m_dX_IPAcc_G4;
    if ( m_dX_IPAcc_GL1 )               delete m_dX_IPAcc_GL1;
    if ( m_dX_PSAcc_Nom )               delete m_dX_PSAcc_Nom;
    if ( m_dX_PSAcc_LAr )               delete m_dX_PSAcc_LAr;
    if ( m_dX_PSAcc_G4 )                delete m_dX_PSAcc_G4;
    if ( m_G4OverAFII )                 delete m_G4OverAFII;
    if ( m_G4OverFrSh )                 delete m_G4OverFrSh;
    if ( m_matElectronEtaBins )         delete m_matElectronEtaBins;

  }


  int egammaEnergyCorrectionTool::initialize() {

    // Test if we already called initialize
    //comment this for now, because of a double initialization issue when calling the tool inside ATHENA
    /*if ( m_initialized ) {
      std::cerr << "ERROR " << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		<< "! tool initialized twice!" << std::endl;
      return 0;
    }
    */
    
    // Load the ROOT file
    const char* fname(gSystem->ExpandPathName( m_rootFileName.c_str() ));
    m_rootFile = TFile::Open( fname, "READ" );	
    
    if ( !m_rootFile ) {
      std::cerr << "ERROR  " << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		<< "! No ROOT file found here: " << m_rootFileName << std::endl;
    delete fname;
    return 0;      
    }

    // instantiate the resolution parametriaton    

    if( !m_resolution_tool )
      m_resolution_tool = new eg_resolution();

    if (!m_getMaterialDelta)
       m_getMaterialDelta = new get_MaterialResolutionEffect();

    
    // Energy corrections and systematic uncertainties
    //////////////////////////////////////////////////
    

    // Legacy numbers for 2010
    
    if ( m_esmodel==egEnergyCorr::es2010 ) {

      m_aPSNom      = (TH1D*) m_rootFile->Get("Scales/es2010/alphaPS_errTot");           m_aPSNom->SetDirectory(0);
      m_aS12Nom     = (TH1D*) m_rootFile->Get("Scales/es2010/alphaS12_errTot");          m_aS12Nom->SetDirectory(0);
 
      m_zeeNom      = (TH1D*) m_rootFile->Get("Scales/es2010/alphaZee_errStat");         m_zeeNom->SetDirectory(0);      
      m_zeeSyst     = (TH1D*) m_rootFile->Get("Scales/es2010/alphaZee_errSyst");         m_zeeSyst->SetDirectory(0);

      m_resNom      = (TH1D*) m_rootFile->Get("Resolution/es2010/ctZee_errStat");        m_resNom->SetDirectory(0);
      m_resSyst     = (TH1D*) m_rootFile->Get("Resolution/es2010/ctZee_errSyst");        m_resSyst->SetDirectory(0);
      m_peakResData = (TH1D*) m_rootFile->Get("Resolution/es2010/resZee_Data");          m_peakResData->SetDirectory(0);
      m_peakResMC   = (TH1D*) m_rootFile->Get("Resolution/es2010/resZee_MC");            m_peakResMC->SetDirectory(0);

      m_begRunNumber = 152166;
      m_endRunNumber = 170482;
      
      m_layer_recalibration_tool = new egammaLayerRecalibTool("2010_with_layer2");

      m_use_layer_correction = false;
      m_use_MVA_calibration = false;
      m_use_gain_correction = false;
      m_use_intermodule_correction = false;
      m_use_phi_unif_correction = false;
      
    // mc11c : faulty electron multiple scattering in G4; old geometry
    // Precise Z scales, systematics otherwise as in 2010

    } else if ( m_esmodel==egEnergyCorr::es2011c ) {
      
      m_aPSNom      = (TH1D*) m_rootFile->Get("Scales/es2011c/alphaPS_errTot");          m_aPSNom->SetDirectory(0);
      m_aS12Nom     = (TH1D*) m_rootFile->Get("Scales/es2011c/alphaS12_errTot");         m_aS12Nom->SetDirectory(0);
 
      m_zeeNom      = (TH1D*) m_rootFile->Get("Scales/es2011c/alphaZee_errStat");        m_zeeNom->SetDirectory(0);
      m_zeeSyst     = (TH1D*) m_rootFile->Get("Scales/es2011c/alphaZee_errSyst");        m_zeeSyst->SetDirectory(0);

      m_resNom      = (TH1D*) m_rootFile->Get("Resolution/es2011c/ctZee_errStat");       m_resNom->SetDirectory(0);
      m_resSyst     = (TH1D*) m_rootFile->Get("Resolution/es2011c/ctZee_errSyst");       m_resSyst->SetDirectory(0);
      m_peakResData = (TH1D*) m_rootFile->Get("Resolution/es2011c/resZee_Data");         m_peakResData->SetDirectory(0);
      m_peakResMC   = (TH1D*) m_rootFile->Get("Resolution/es2011c/resZee_MC");           m_peakResMC->SetDirectory(0);

      m_begRunNumber = 177531;
      m_endRunNumber = 194382;
      
      m_layer_recalibration_tool = new egammaLayerRecalibTool("2010_with_layer2");
      
      m_use_layer_correction = false;
      m_use_MVA_calibration = false;
      m_use_gain_correction = false;
      m_use_intermodule_correction = false;
      m_use_phi_unif_correction = false;
      

    // mc11d : correct MSc in G4; new geometry
    // Final Run1 calibration scheme

    } else if ( m_esmodel==egEnergyCorr::es2011d ) {
      
      m_aPSNom       = (TH1D*) m_rootFile->Get("Scales/es2011d/alphaPS_uncor");                 m_aPSNom->SetDirectory(0);
      m_daPSCor      = (TH1D*) m_rootFile->Get("Scales/es2011d/dalphaPS_cor");                  m_daPSCor->SetDirectory(0);
      m_aS12Nom      = (TH1D*) m_rootFile->Get("Scales/es2011d/alphaS12_uncor");                m_aS12Nom->SetDirectory(0);
      m_daS12Cor     = (TH1D*) m_rootFile->Get("Scales/es2011d/dalphaS12_cor");                 m_daS12Cor->SetDirectory(0);

      m_trkSyst      = (TH1D*) m_rootFile->Get("Scales/es2011d/momentum_errSyst");              m_trkSyst->SetDirectory(0);

      m_zeeNom       = (TH1D*) m_rootFile->Get("Scales/es2011d/alphaZee_errStat");              m_zeeNom->SetDirectory(0);      	
      m_zeeSyst      = (TH1D*) m_rootFile->Get("Scales/es2011d/alphaZee_errSyst");              m_zeeSyst->SetDirectory(0);

      m_resNom       = (TH1D*) m_rootFile->Get("Resolution/es2011d/ctZee_errStat");             m_resNom->SetDirectory(0); 
      m_resSyst      = (TH1D*) m_rootFile->Get("Resolution/es2011d/ctZee_errSyst");             m_resSyst->SetDirectory(0);

      m_pedestalL0   = (TH1D*) m_rootFile->Get("Pedestals/es2011d/pedestals_l0");               m_pedestalL0->SetDirectory(0);
      m_pedestalL1   = (TH1D*) m_rootFile->Get("Pedestals/es2011d/pedestals_l1");               m_pedestalL1->SetDirectory(0);
      m_pedestalL2   = (TH1D*) m_rootFile->Get("Pedestals/es2011d/pedestals_l2");               m_pedestalL2->SetDirectory(0);
      m_pedestalL3   = (TH1D*) m_rootFile->Get("Pedestals/es2011d/pedestals_l3");               m_pedestalL3->SetDirectory(0);

      m_dX_ID_Nom    = (TH1D*) m_rootFile->Get("Material/DX0_ConfigA");                         m_dX_ID_Nom->SetDirectory(0);

      m_dX_IPPS_Nom  = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errUncor");   m_dX_IPPS_Nom->SetDirectory(0);
      m_dX_IPPS_LAr  = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errLAr");     m_dX_IPPS_LAr->SetDirectory(0);

      m_dX_IPAcc_Nom = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errUncor");  m_dX_IPAcc_Nom->SetDirectory(0);
      m_dX_IPAcc_LAr = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errLAr");    m_dX_IPAcc_LAr->SetDirectory(0);
      m_dX_IPAcc_G4  = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errG4");     m_dX_IPAcc_G4->SetDirectory(0);
      m_dX_IPAcc_GL1  = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errGL1");    m_dX_IPAcc_GL1->SetDirectory(0);

      m_dX_PSAcc_Nom = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errUncor");  m_dX_PSAcc_Nom->SetDirectory(0);
      m_dX_PSAcc_LAr = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errLAr");    m_dX_PSAcc_LAr->SetDirectory(0);
      m_dX_PSAcc_G4  = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errG4");     m_dX_PSAcc_G4->SetDirectory(0);

      m_convRadius            = (TH1D*) m_rootFile->Get("Conversions/es2011d/convRadiusMigrations");  m_convRadius->SetDirectory(0);
      m_convFakeRate          = (TH1D*) m_rootFile->Get("Conversions/es2011d/convFakeRate");          m_convFakeRate->SetDirectory(0);
      m_convRecoEfficiency    = (TH1D*) m_rootFile->Get("Conversions/es2011d/convRecoEfficiency");    m_convRecoEfficiency->SetDirectory(0);

      m_begRunNumber = 177531;
      m_endRunNumber = 194382;
      
      m_layer_recalibration_tool = new egammaLayerRecalibTool("2011_alt_with_layer2");
      
      m_use_layer_correction = true;
      m_use_MVA_calibration = true;
      m_use_gain_correction = true;
      m_use_intermodule_correction = true;
      m_use_phi_unif_correction = true;
      
#ifdef ROOTCORE
      const std::string gain_filename1 = "$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/FunctionsTO.root";
      const std::string gain_filename2 = "$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/FunctionsG_all.root";
#else
      const std::string gain_filename1 = PathResolver::find_file ("ElectronPhotonFourMomentumCorrection/FunctionsTO.root", "XMLPATH");
      const std::string gain_filename2 = PathResolver::find_file ("ElectronPhotonFourMomentumCorrection/FunctionsG_all.root", "XMLPATH");
#endif
      m_gain_tool = new egGain::GainTool(gain_filename1, gain_filename2);
      
      m_e1hg_tool = new e1hg_systematics();
      
    // mc12a : crude MSc fix in G4; old geometry
    // All systematics as in 2010.

    } else if ( m_esmodel==egEnergyCorr::es2012a ) {
      
      m_aPSNom      = (TH1D*) m_rootFile->Get("Scales/es2012a/alphaPS_errTot");          m_aPSNom->SetDirectory(0);
      m_aS12Nom     = (TH1D*) m_rootFile->Get("Scales/es2012a/alphaS12_errTot");         m_aS12Nom->SetDirectory(0);
 
      m_zeeNom      = (TH1D*) m_rootFile->Get("Scales/es2012a/alphaZee_errStat");        m_zeeNom->SetDirectory(0);
      m_zeeSyst     = (TH1D*) m_rootFile->Get("Scales/es2012a/alphaZee_errSyst");        m_zeeSyst->SetDirectory(0);
 
      m_resNom      = (TH1D*) m_rootFile->Get("Resolution/es2012a/ctZee_errStat");       m_resNom->SetDirectory(0);
      m_resSyst     = (TH1D*) m_rootFile->Get("Resolution/es2012a/ctZee_errSyst");       m_resSyst->SetDirectory(0);
      m_peakResData = (TH1D*) m_rootFile->Get("Resolution/es2012a/resZee_Data");         m_peakResData->SetDirectory(0);
      m_peakResMC   = (TH1D*) m_rootFile->Get("Resolution/es2012a/resZee_MC");           m_peakResMC->SetDirectory(0);
      
      m_begRunNumber = 195847;
      m_endRunNumber = 219365;
      
      m_layer_recalibration_tool = new egammaLayerRecalibTool("2010_with_layer2");
      
      m_use_layer_correction = false;
      m_use_MVA_calibration = false;
      m_use_gain_correction = false;
      m_use_intermodule_correction = false;
      m_use_phi_unif_correction = false;

      
    // mc12c : correct MSc in G4; new geometry
    // Final Run1 calibration scheme
      
    } else if ( m_esmodel==egEnergyCorr::es2012c ) {

      m_aPSNom       = (TH1D*) m_rootFile->Get("Scales/es2012c/alphaPS_uncor");                 m_aPSNom->SetDirectory(0);
      m_daPSCor      = (TH1D*) m_rootFile->Get("Scales/es2012c/dalphaPS_cor");                  m_daPSCor->SetDirectory(0);
      m_aS12Nom      = (TH1D*) m_rootFile->Get("Scales/es2012c/alphaS12_uncor");                m_aS12Nom->SetDirectory(0);
      m_daS12Cor     = (TH1D*) m_rootFile->Get("Scales/es2012c/dalphaS12_cor");                 m_daS12Cor->SetDirectory(0);

      m_trkSyst      = (TH1D*) m_rootFile->Get("Scales/es2012c/momentum_errSyst");              m_trkSyst->SetDirectory(0);
      
      m_zeeNom       = (TH1D*) m_rootFile->Get("Scales/es2012c/alphaZee_errStat");              m_zeeNom->SetDirectory(0);      	
      m_zeeSyst      = (TH1D*) m_rootFile->Get("Scales/es2012c/alphaZee_errSyst");              m_zeeSyst->SetDirectory(0);

      m_resNom       = (TH1D*) m_rootFile->Get("Resolution/es2012c/ctZee_errStat");             m_resNom->SetDirectory(0); 
      m_resSyst      = (TH1D*) m_rootFile->Get("Resolution/es2012c/ctZee_errSyst");             m_resSyst->SetDirectory(0);

      m_pedestalL0   = (TH1D*) m_rootFile->Get("Pedestals/es2012c/pedestals_l0");               m_pedestalL0->SetDirectory(0);
      m_pedestalL1   = (TH1D*) m_rootFile->Get("Pedestals/es2012c/pedestals_l1");               m_pedestalL1->SetDirectory(0);
      m_pedestalL2   = (TH1D*) m_rootFile->Get("Pedestals/es2012c/pedestals_l2");               m_pedestalL2->SetDirectory(0);
      m_pedestalL3   = (TH1D*) m_rootFile->Get("Pedestals/es2012c/pedestals_l3");               m_pedestalL3->SetDirectory(0);

      m_dX_ID_Nom    = (TH1D*) m_rootFile->Get("Material/DX0_ConfigA");                         m_dX_ID_Nom->SetDirectory(0);

      m_dX_IPPS_Nom  = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errUncor");   m_dX_IPPS_Nom->SetDirectory(0);
      m_dX_IPPS_LAr  = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errLAr");     m_dX_IPPS_LAr->SetDirectory(0);

      m_dX_IPAcc_Nom = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errUncor");  m_dX_IPAcc_Nom->SetDirectory(0);
      m_dX_IPAcc_LAr = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errLAr");    m_dX_IPAcc_LAr->SetDirectory(0);
      m_dX_IPAcc_G4  = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errG4");     m_dX_IPAcc_G4->SetDirectory(0);
      m_dX_IPAcc_GL1  = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errGL1");    m_dX_IPAcc_GL1->SetDirectory(0);

      m_dX_PSAcc_Nom = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errUncor");  m_dX_PSAcc_Nom->SetDirectory(0);
      m_dX_PSAcc_LAr = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errLAr");    m_dX_PSAcc_LAr->SetDirectory(0);
      m_dX_PSAcc_G4  = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errG4");     m_dX_PSAcc_G4->SetDirectory(0);

      m_convRadius            = (TH1D*) m_rootFile->Get("Conversions/es2012c/convRadiusMigrations");  m_convRadius->SetDirectory(0);
      m_convFakeRate          = (TH1D*) m_rootFile->Get("Conversions/es2012c/convFakeRate");          m_convFakeRate->SetDirectory(0);
      m_convRecoEfficiency    = (TH1D*) m_rootFile->Get("Conversions/es2012c/convRecoEfficiency");    m_convRecoEfficiency->SetDirectory(0);

      m_begRunNumber = 195847;
      m_endRunNumber = 219365;
      
      m_layer_recalibration_tool = new egammaLayerRecalibTool("2012_alt_with_layer2");
      
      m_use_layer_correction = true;
      m_use_MVA_calibration = true;
      m_use_gain_correction = true;
      m_use_intermodule_correction = true;
      m_use_phi_unif_correction = true;

#ifdef ROOTCORE
      const std::string gain_filename1 = "$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/FunctionsTO.root";
      const std::string gain_filename2 = "$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/FunctionsG_all.root";
#else
      const std::string gain_filename1 = PathResolver::find_file ("ElectronPhotonFourMomentumCorrection/FunctionsTO.root", "XMLPATH");
      const std::string gain_filename2 = PathResolver::find_file ("ElectronPhotonFourMomentumCorrection/FunctionsG_all.root", "XMLPATH");
#endif
      m_gain_tool = new egGain::GainTool(gain_filename1, gain_filename2);

      m_e1hg_tool = new e1hg_systematics();

    // mc12c : correct MSc in G4; new geometry
    // Final Run1 calibration scheme
      
    } else if ( m_esmodel==egEnergyCorr::es2015_day0_3percent ) {

			m_aPSNom       = (TH1D*) m_rootFile->Get("Scales/es2015_day0/alphaPS_uncor");                 m_aPSNom->SetDirectory(0);         // old one
      m_daPSCor      = (TH1D*) m_rootFile->Get("Scales/es2015_day0/dalphaPS_cor");                  m_daPSCor->SetDirectory(0);        // old one
      m_aS12Nom      = (TH1D*) m_rootFile->Get("Scales/es2015_day0/alphaS12_uncor");                m_aS12Nom->SetDirectory(0);        // old one
      m_daS12Cor     = (TH1D*) m_rootFile->Get("Scales/es2015_day0/dalphaS12_cor");                 m_daS12Cor->SetDirectory(0);       // old one

      m_trkSyst      = (TH1D*) m_rootFile->Get("Scales/es2015_day0/momentum_errSyst");              m_trkSyst->SetDirectory(0);        // old one
      
      m_zeeNom       = (TH1D*) m_rootFile->Get("Scales/es2015_day0/alphaZee_errStat");              m_zeeNom->SetDirectory(0);      	 // old one
      m_zeeSyst      = (TH1D*) m_rootFile->Get("Scales/es2015_day0/alphaZee_errSyst");              m_zeeSyst->SetDirectory(0);        // old one

      m_resNom       = (TH1D*) m_rootFile->Get("Resolution/es2012c/ctZee_errStat");             m_resNom->SetDirectory(0);         // old one
      m_resSyst      = (TH1D*) m_rootFile->Get("Resolution/es2012c/ctZee_errSyst");             m_resSyst->SetDirectory(0);        // old one

      m_pedestalL0   = (TH1D*) m_rootFile->Get("Pedestals/es2012c/pedestals_l0");               m_pedestalL0->SetDirectory(0);     // old one
      m_pedestalL1   = (TH1D*) m_rootFile->Get("Pedestals/es2012c/pedestals_l1");               m_pedestalL1->SetDirectory(0);     // old one
      m_pedestalL2   = (TH1D*) m_rootFile->Get("Pedestals/es2012c/pedestals_l2");               m_pedestalL2->SetDirectory(0);     // old one
      m_pedestalL3   = (TH1D*) m_rootFile->Get("Pedestals/es2012c/pedestals_l3");               m_pedestalL3->SetDirectory(0);     // old one

      m_dX_ID_Nom    = (TH1D*) m_rootFile->Get("Material/DX0_ConfigA");                         m_dX_ID_Nom->SetDirectory(0);      // old one

      m_dX_IPPS_Nom  = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errUncor");   m_dX_IPPS_Nom->SetDirectory(0);    // old one
      m_dX_IPPS_LAr  = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errLAr");     m_dX_IPPS_LAr->SetDirectory(0);    // old one

      m_dX_IPAcc_Nom = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errUncor");  m_dX_IPAcc_Nom->SetDirectory(0);   // old one
      m_dX_IPAcc_LAr = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errLAr");    m_dX_IPAcc_LAr->SetDirectory(0);   // old one
      m_dX_IPAcc_G4  = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errG4");     m_dX_IPAcc_G4->SetDirectory(0);    // old one
      m_dX_IPAcc_GL1  = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errGL1");    m_dX_IPAcc_GL1->SetDirectory(0);  // old one

      m_dX_PSAcc_Nom = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errUncor");  m_dX_PSAcc_Nom->SetDirectory(0);   // old one
      m_dX_PSAcc_LAr = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errLAr");    m_dX_PSAcc_LAr->SetDirectory(0);   // old one
      m_dX_PSAcc_G4  = (TH1D*) m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errG4");     m_dX_PSAcc_G4->SetDirectory(0);    // old one

      m_convRadius            = (TH1D*) m_rootFile->Get("Conversions/es2012c/convRadiusMigrations");  m_convRadius->SetDirectory(0);          // old one
      m_convFakeRate          = (TH1D*) m_rootFile->Get("Conversions/es2012c/convFakeRate");          m_convFakeRate->SetDirectory(0);        // old one
      m_convRecoEfficiency    = (TH1D*) m_rootFile->Get("Conversions/es2012c/convRecoEfficiency");    m_convRecoEfficiency->SetDirectory(0);  // old one

      m_begRunNumber = 195847;
      m_endRunNumber = 219365;
      
      m_layer_recalibration_tool = new egammaLayerRecalibTool("2012_alt_with_layer2");                                             // old one
      
      m_use_layer_correction = true;
      m_use_MVA_calibration = true;
      m_use_gain_correction = true;
      m_use_intermodule_correction = true;
      m_use_phi_unif_correction = true;

#ifdef ROOTCORE
      const std::string gain_filename1 = "$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/FunctionsTO.root";
      const std::string gain_filename2 = "$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/FunctionsG_all.root";
#else
      const std::string gain_filename1 = PathResolver::find_file ("ElectronPhotonFourMomentumCorrection/FunctionsTO.root", "XMLPATH");
      const std::string gain_filename2 = PathResolver::find_file ("ElectronPhotonFourMomentumCorrection/FunctionsG_all.root", "XMLPATH");
#endif
      m_gain_tool = new egGain::GainTool(gain_filename1, gain_filename2);

      m_e1hg_tool = new e1hg_systematics();

      
    // If we are here, fail      :

    } else if ( m_esmodel == egEnergyCorr::UNDEFINED) {
      
      std::cout << "FATAL "
		<<  "(file: " << __FILE__ << ", line: " << __LINE__ << ") "
		<< "egammaEnergyCorrectionTool : ES model not initialized - Initialization fails" << std::endl;
      return 0;	
      
    } else {
      
      std::cout << "FATAL "
		<<  "(file: " << __FILE__ << ", line: " << __LINE__ << ") "
		<< "egammaEnergyCorrectionTool : ES model not recognized - Initialization fails" << std::endl;
      return 0;
      
    }
    

    // further inputs do not depend on year

    // ... PS and S12 recalibration curves

    m_psElectronEtaBins =          (TAxis*) m_rootFile->Get("PSRecalibration/ElectronAxis");	
    m_psElectronGraphs =           (TList*) m_rootFile->Get("PSRecalibration/ElectronBiasPS");
    m_psUnconvertedEtaBins =       (TAxis*) m_rootFile->Get("PSRecalibration/UnconvertedAxis");
    m_psUnconvertedGraphs =        (TList*) m_rootFile->Get("PSRecalibration/UnconvertedBiasPS");
    m_psConvertedEtaBins =         (TAxis*) m_rootFile->Get("PSRecalibration/ConvertedAxis");
    m_psConvertedGraphs =          (TList*) m_rootFile->Get("PSRecalibration/ConvertedBiasPS");
    
    m_s12ElectronEtaBins =         (TAxis*) m_rootFile->Get("S1Recalibration/ElectronAxis");	
    m_s12ElectronGraphs =          (TList*) m_rootFile->Get("S1Recalibration/ElectronBiasS1");
    m_s12UnconvertedEtaBins =      (TAxis*) m_rootFile->Get("S1Recalibration/UnconvertedAxis");
    m_s12UnconvertedGraphs =       (TList*) m_rootFile->Get("S1Recalibration/UnconvertedBiasS1");
    m_s12ConvertedEtaBins =        (TAxis*) m_rootFile->Get("S1Recalibration/ConvertedAxis");
    m_s12ConvertedGraphs =         (TList*) m_rootFile->Get("S1Recalibration/ConvertedBiasS1");

    // ... material distortions
        
    m_matUnconvertedScale.push_back( (TH1D*) m_rootFile->Get("Material/unconvertedBiasSubtracted_ConfigA") );
    m_matUnconvertedScale.push_back( (TH1D*) m_rootFile->Get("Material/unconvertedBiasSubtracted_ConfigCpDp") );
    m_matUnconvertedScale.push_back( (TH1D*) m_rootFile->Get("Material/unconvertedBiasSubtracted_ConfigEpLp") );
    m_matUnconvertedScale.push_back( (TH1D*) m_rootFile->Get("Material/unconvertedBiasSubtracted_ConfigFpMX") );
    m_matUnconvertedScale.push_back( (TH1D*) m_rootFile->Get("Material/unconvertedBiasSubtracted_ConfigGp") );
    
    m_matConvertedScale.push_back( (TH1D*) m_rootFile->Get("Material/convertedBiasSubtracted_ConfigA") );
    m_matConvertedScale.push_back( (TH1D*) m_rootFile->Get("Material/convertedBiasSubtracted_ConfigCpDp") );
    m_matConvertedScale.push_back( (TH1D*) m_rootFile->Get("Material/convertedBiasSubtracted_ConfigEpLp") );
    m_matConvertedScale.push_back( (TH1D*) m_rootFile->Get("Material/convertedBiasSubtracted_ConfigFpMX") );
    m_matConvertedScale.push_back( (TH1D*) m_rootFile->Get("Material/convertedBiasSubtracted_ConfigGp") );

    m_matElectronCstTerm.push_back( (TH1D*) m_rootFile->Get("Material/electronCstTerm_ConfigA") );
    m_matElectronCstTerm.push_back( (TH1D*) m_rootFile->Get("Material/electronCstTerm_ConfigCpDp") );
    m_matElectronCstTerm.push_back( (TH1D*) m_rootFile->Get("Material/electronCstTerm_ConfigEpLp") );
    m_matElectronCstTerm.push_back( (TH1D*) m_rootFile->Get("Material/electronCstTerm_ConfigFpMX") );
    m_matElectronCstTerm.push_back( (TH1D*) m_rootFile->Get("Material/electronCstTerm_ConfigGp") );

    m_matX0Additions.push_back( (TH1D*) m_rootFile->Get("Material/DX0_ConfigA") );
    m_matX0Additions.push_back( (TH1D*) m_rootFile->Get("Material/DX0_ConfigCpDp") );
    m_matX0Additions.push_back( (TH1D*) m_rootFile->Get("Material/DX0_ConfigEpLp") );
    m_matX0Additions.push_back( (TH1D*) m_rootFile->Get("Material/DX0_ConfigFpMX") );
    m_matX0Additions.push_back( (TH1D*) m_rootFile->Get("Material/DX0_ConfigGp") );

    m_matElectronEtaBins =         (TAxis*) m_rootFile->Get("Material/LinearityEtaBins");
    m_matElectronGraphs.push_back( (TList*) m_rootFile->Get("Material/Linearity_Cluster_ConfigA") );
    m_matElectronGraphs.push_back( (TList*) m_rootFile->Get("Material/Linearity_Cluster_ConfigCpDp") );
    m_matElectronGraphs.push_back( (TList*) m_rootFile->Get("Material/Linearity_Cluster_ConfigEpLp") );
    m_matElectronGraphs.push_back( (TList*) m_rootFile->Get("Material/Linearity_Cluster_ConfigFpMX") );
    m_matElectronGraphs.push_back( (TList*) m_rootFile->Get("Material/Linearity_Cluster_ConfigGp") );

    // ... Fastsim to Fullsim corrections
    
    m_G4OverAFII            = (TH1D*) m_rootFile->Get("FastSim/hG4OverAF");                 m_G4OverAFII->SetDirectory(0);
    m_G4OverFrSh            = (TH1D*) m_rootFile->Get("FastSim/hG4OverFS");                 m_G4OverFrSh->SetDirectory(0);
    
    // ... Leakage systematics

    m_leakageConverted      = (TH1D*) m_rootFile->Get("Leakage/LeakageDiffConverted");      m_leakageConverted->SetDirectory(0);
    m_leakageUnconverted    = (TH1D*) m_rootFile->Get("Leakage/LeakageDiffUnconverted");    m_leakageUnconverted->SetDirectory(0);

    // ... Zee S2 profile (needed for gain switch syst).

    m_zeeES2Profile         = (TH1D*) m_rootFile->Get("ZeeEnergyProfiles/p2MC");    m_zeeES2Profile->SetDirectory(0);

    // OK, now we are all initialized and everything went fine

    m_calibInputs = new StdCalibrationInputs();

    m_initialized = true;
    return 1;

  }


  // just for test
  float egammaEnergyCorrectionTool::getCalibInputs(int i) {
    
    switch (i) {
    case 0: 
      return m_calibInputs->E0raw;
    case 1: 
      return m_calibInputs->E1raw;
    case 2: 
      return m_calibInputs->E2raw;
    case 3: 
      return m_calibInputs->E3raw;
    default: 
      return 0.;
    }

  }
  

  // User interface
  // universal compact interface to getCorrectedEnergy(...)

  double egammaEnergyCorrectionTool::getCorrectedMomentum( PATCore::ParticleDataType::DataType dataType,
							   PATCore::ParticleType::Type ptype,
							   double momentum,
							   double trk_eta,
							   egEnergyCorr::Scale::Variation scaleVar,
							   double varSF ) const {
    
    double correctedMomentum = momentum;
    double aeta = fabs(trk_eta);

    if ( ptype == PATCore::ParticleType::Electron && dataType != PATCore::ParticleDataType::Data ) {
      
      double corr = 0;
      if( scaleVar==egEnergyCorr::Scale::MomentumUp )
	corr = m_trkSyst->GetBinContent( m_trkSyst->FindBin(aeta) );
      else if( scaleVar==egEnergyCorr::Scale::MomentumDown )
	corr = -m_trkSyst->GetBinContent( m_trkSyst->FindBin(aeta) );
      
      correctedMomentum *= 1. + corr*varSF;
      
    }      
    
    return correctedMomentum;

  }
  
  double egammaEnergyCorrectionTool::getCorrectedEnergy( unsigned int runnumber,
							 PATCore::ParticleDataType::DataType dataType,
							 const ParticleInformation & particle_info,
							 egEnergyCorr::Scale::Variation scaleVar,
							 egEnergyCorr::Resolution::Variation resVar,
                                                         egEnergyCorr::Resolution::resolutionType resType,
							 double varSF ) const {
    
    return getCorrectedEnergy(runnumber,
			      dataType,
			      particle_info.ptype,
			      particle_info.rawcl_Es0,
			      particle_info.rawcl_Es1,
			      particle_info.rawcl_Es2,
			      particle_info.rawcl_Es3,
			      particle_info.cl_eta,
			      particle_info.cl_phi,
			      particle_info.trk_eta,
			      particle_info.cl_E,
			      particle_info.cl_etaCalo,
			      particle_info.cl_phiCalo,
			      particle_info.ptconv,
			      particle_info.pt1conv,
			      particle_info.pt2conv,
			      particle_info.convtrk1nPixHits,
			      particle_info.convtrk1nSCTHits,
			      particle_info.convtrk2nPixHits,
			      particle_info.convtrk2nSCTHits,
			      particle_info.Rconv,
			      scaleVar,
			      resVar,
                              resType,
			      varSF );
  }


  // User interface
  // Photon interface to getCorrectedEnergy(...)

  double egammaEnergyCorrectionTool::getCorrectedEnergyPhoton( unsigned int runnumber,
							       PATCore::ParticleDataType::DataType dataType,
							       float rawcl_Es0,
							       float rawcl_Es1,
							       float rawcl_Es2,
							       float rawcl_Es3,
							       float cl_eta,
							       float cl_phi,
							       float cl_E,
							       float cl_etaCalo,
							       float cl_phiCalo,
							       float ptconv,
							       float pt1conv,
							       float pt2conv,
							       int convtrk1nPixHits,
							       int convtrk1nSCTHits,
							       int convtrk2nPixHits,
							       int convtrk2nSCTHits,
							       float Rconv,
							       egEnergyCorr::Scale::Variation scaleVar,
							       egEnergyCorr::Resolution::Variation resVar,
                                                               egEnergyCorr::Resolution::resolutionType resType,
							       double varSF ) const {

    const PATCore::ParticleType::Type ptype = (Rconv > 0 and Rconv < 800) ? 
      PATCore::ParticleType::ConvertedPhoton : PATCore::ParticleType::UnconvertedPhoton;

    return getCorrectedEnergy(runnumber,
			      dataType,
			      ptype,
			      rawcl_Es0,
			      rawcl_Es1,
			      rawcl_Es2,
			      rawcl_Es3,
			      cl_eta,
			      cl_phi,
			      cl_eta,   // cl_eta -> trk_eta
			      cl_E,
			      cl_etaCalo,
			      cl_phiCalo,
			      ptconv,
			      pt1conv,
			      pt2conv,
			      convtrk1nPixHits,
			      convtrk1nSCTHits,
			      convtrk2nPixHits,
			      convtrk2nSCTHits,
			      Rconv,
			      scaleVar,
			      resVar,
                              resType,
			      varSF );
  }


  // User interface
  // Electron interface to getCorrectedEnergy(...)

  double egammaEnergyCorrectionTool::getCorrectedEnergyElectron( unsigned int runnumber,
								   PATCore::ParticleDataType::DataType dataType,
								   float rawcl_Es0,
								   float rawcl_Es1,
								   float rawcl_Es2,
								   float rawcl_Es3,
								   float cl_eta,
								   float cl_phi,
								   float trk_eta,
								   float cl_E,
								   float cl_etaCalo,
								   float cl_phiCalo,
								   egEnergyCorr::Scale::Variation scaleVar,
								   egEnergyCorr::Resolution::Variation resVar,
                                                                   egEnergyCorr::Resolution::resolutionType resType,
								   double varSF ) const {

    return getCorrectedEnergy(runnumber,
			      dataType,
			      PATCore::ParticleType::Electron,
			      rawcl_Es0,
			      rawcl_Es1,
			      rawcl_Es2,
			      rawcl_Es3,
			      cl_eta,
			      cl_phi,
			      trk_eta,
			      cl_E,
			      cl_etaCalo,
			      cl_phiCalo,
			      -999,
			      -999,
			      -999,
			      -999,
			      -999,
			      -999,
			      -999,
			      -999,
			      scaleVar,
			      resVar,
                              resType,
			      varSF );

  }


  // Main method for all corrections. Called by user interfaces
  // Pre-corrections applied here (layer calibration; MVA; uniformity)

  // TODO : 
  // -> change name?
  // -> move to private?

  double egammaEnergyCorrectionTool::getCorrectedEnergy( unsigned int runnumber,
                                                         PATCore::ParticleDataType::DataType dataType,
                                                         PATCore::ParticleType::Type ptype,
							 float rawcl_Es0,
							 float rawcl_Es1,
							 float rawcl_Es2,
							 float rawcl_Es3,
							 float cl_eta,
							 float cl_phi,
							 float trk_eta,
							 float cl_E,
							 float cl_etaCalo,
							 float cl_phiCalo,
							 float ptconv,
							 float pt1conv,
							 float pt2conv,
							 int convtrk1nPixHits,
							 int convtrk1nSCTHits,
							 int convtrk2nPixHits,
							 int convtrk2nSCTHits,
							 float Rconv,
                                                         egEnergyCorr::Scale::Variation scaleVar,
                                                         egEnergyCorr::Resolution::Variation resVar,
                                                         egEnergyCorr::Resolution::resolutionType resType,
                                                         double varSF ) const {

    if (ptype == PATCore::ParticleType::Photon)
      ptype = (Rconv > 0 and Rconv < 800) ? PATCore::ParticleType::ConvertedPhoton : PATCore::ParticleType::UnconvertedPhoton;

    double result = cl_E;
    
    m_calibInputs->RunNumber = (int)runnumber;
    m_calibInputs->eta = cl_eta;
    m_calibInputs->phi = cl_phi;
    m_calibInputs->E0raw = rawcl_Es0;
    m_calibInputs->E1raw = rawcl_Es1;
    m_calibInputs->E2raw = rawcl_Es2;
    m_calibInputs->E3raw = rawcl_Es3;
    m_calibInputs->Rconv = Rconv;


    // layer recalibration
    //////////////////////

    if( dataType == PATCore::ParticleDataType::Data && m_use_layer_correction ) {
      
      m_layer_recalibration_tool->scale_inputs(*m_calibInputs);

      if( m_applyPSCorrection ) rawcl_Es0 = m_calibInputs->E0raw;
      if( m_applyS12Correction )  {
	rawcl_Es1 = m_calibInputs->E1raw;
	rawcl_Es2 = m_calibInputs->E2raw;
      }
      rawcl_Es3 = m_calibInputs->E3raw;
      Rconv     = m_calibInputs->Rconv;

    }
    
    if (rawcl_Es2 == 0 and rawcl_Es1 == 0 and rawcl_Es3 == 0 and rawcl_Es0 == 0)
      if ( (ptype == PATCore::ParticleType::Electron and (fabs(cl_eta) < 1.37 or (fabs(cl_eta) > 1.55 and fabs(cl_eta) <  2.47))) or
	   (ptype != PATCore::ParticleType::Electron and (fabs(cl_eta) < 1.37 or (fabs(cl_eta) > 1.55 and fabs(cl_eta) <  2.47))) )
	std::cerr << "WARNING: all layer energies are zero, MVA result will be wrong" << std::endl;


    // apply MVA calibration
    ////////////////////////
    
    if (m_use_MVA_calibration) {
      
      if ( ptype == PATCore::ParticleType::Electron ) {
	
	if (!m_mva_electron_tool) {  // initialize the tool on first use
	  // TODO(ruggero): make last argument configurable with property
	  m_mva_electron_tool = new egammaMVACalib(1, true, "egammaMVACalib/v1");
	  m_mva_electron_tool->InitTree(0);
	}
	
	result = m_mva_electron_tool->getMVAEnergyElectron( rawcl_Es0,
							    rawcl_Es1,
							    rawcl_Es2,
							    rawcl_Es3,
							    cl_eta,
							    cl_E,
							    cl_etaCalo,
							    cl_phiCalo);
	
      } else if ( ptype == PATCore::ParticleType::ConvertedPhoton or ptype == PATCore::ParticleType::UnconvertedPhoton ) {
	
	if (!m_mva_photon_tool) {
	  // TODO(ruggero): make last argument configurable with property
	  m_mva_photon_tool = new egammaMVACalib(0, true, "egammaMVACalib/v1");
	  m_mva_photon_tool->InitTree(0);
	}

	result = m_mva_photon_tool->getMVAEnergyPhoton( rawcl_Es0,
							rawcl_Es1,
							rawcl_Es2,
							rawcl_Es3,
							cl_eta,
							cl_E,
							cl_etaCalo,
							cl_phiCalo,
							ptconv,
							pt1conv,
							pt2conv,
							convtrk1nPixHits,
							convtrk1nSCTHits,
							convtrk2nPixHits,
							convtrk2nSCTHits,
							Rconv );
	
      } else {
	
	std::cerr << "ERROR " << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		  << "! particle type not valid!" << std::endl;
	return 0;
	
      }
      
    }
    
    
    // Crack calibation correction for es2011c (calibration hits calibration)
    /////////////////////////////////////////////////////////////////////////

    if ( m_esmodel==egEnergyCorr::es2011c ) {
      assert(not m_use_MVA_calibration);
      result *= applyMCCalibration( cl_eta, result/cosh(trk_eta), ptype);
    }


    // Uniformity corrections
    /////////////////////////

    if ( dataType == PATCore::ParticleDataType::Data and m_use_intermodule_correction )
      result = IntermoduleCorrectionTool(result, cl_phi, cl_eta); // cl_eta, cl_phi ok
    
    if ( dataType == PATCore::ParticleDataType::Data and m_use_phi_unif_correction )
      result *= CorrectionPhiUnif(cl_etaCalo, cl_phiCalo); // cl_etaCalo, cl_phiCalo ok


    // Gain corection
    /////////////////

    if ( dataType == PATCore::ParticleDataType::Data && m_use_gain_correction )
      if (!(fabs(cl_eta) < 1.52 and fabs(cl_eta) > 1.37) and fabs(cl_eta) < 2.4)
	result = m_gain_tool->CorrectionGainTool(cl_eta, result/GeV, rawcl_Es2/GeV, ptype); // cl_eta ok, TODO: check corrected E2


    // overall energy corrections
    /////////////////////////////
    
    result = getCorrectedEnergy( runnumber,
				 dataType,
				 ptype,
				 cl_eta,
				 trk_eta, 
				 result,
				 rawcl_Es2,
				 scaleVar,
				 resVar,
                                 resType,
				 varSF );

    return result;
  }



  // This method handles the main switches between data and the various MC flavours.
  // Called internally by getCorrectedEnergy(...)

  // TODO :
  // -> change name?
  // -> move to private?

  double egammaEnergyCorrectionTool::getCorrectedEnergy( unsigned int runnumber,
                                                         PATCore::ParticleDataType::DataType dataType,
                                                         PATCore::ParticleType::Type ptype,
                                                         double cl_eta,
                                                         double trk_eta, 
                                                         double energy,
							 double energyS2, 
                                                         egEnergyCorr::Scale::Variation scaleVar,
                                                         egEnergyCorr::Resolution::Variation resVar,
                                                         egEnergyCorr::Resolution::resolutionType resType,
                                                         double varSF ) const {
    (void) runnumber; // not used
    (void) trk_eta; // not used 

    // Return value

    double fullyCorrectedEnergy = energy;


    // If nothing is to be done

    if ( scaleVar == egEnergyCorr::Scale::None && resVar == egEnergyCorr::Resolution::None )
      return fullyCorrectedEnergy;


    // Correct fast sim flavours

    else if ( dataType == PATCore::ParticleDataType::FastShower ) // Frozen shower sim
        fullyCorrectedEnergy = energy * this->applyFStoG4(cl_eta);

    else if ( dataType == PATCore::ParticleDataType::Fast ) // AtlFast2 sim
        fullyCorrectedEnergy = energy * this->applyAFtoG4(cl_eta);
      

    // main E-scale corrections
    
    if ( dataType == PATCore::ParticleDataType::Data ) { // ... Data

      if( scaleVar == egEnergyCorr::Scale::Nominal ) {
      
	double alpha = getAlphaValue( cl_eta, fullyCorrectedEnergy, energyS2, ptype, scaleVar, varSF );
	fullyCorrectedEnergy /= (1 + alpha);

      }

    } else { // ... MC
      
      // Do the energy scale correction (for systematic variations)
      
      if ( scaleVar != egEnergyCorr::Scale::None && scaleVar != egEnergyCorr::Scale::Nominal ) {	        

	double deltaAlpha = getAlphaUncertainty( cl_eta, fullyCorrectedEnergy, energyS2, ptype, scaleVar, varSF );
	fullyCorrectedEnergy *= ( 1 + deltaAlpha );
	
      }
      
      // Do the resolution correction

      if ( resVar != egEnergyCorr::Resolution::None )
	
	fullyCorrectedEnergy *= getSmearingCorrection( cl_eta, fullyCorrectedEnergy, ptype, resVar ,resType);
        
    }
    
    return fullyCorrectedEnergy;

  }





  // This method applied the overall energy corrections and systematic variations.
  // Called internally by getCorrectedEnergy(...).
  // convention is Edata = (1+alpha)*Emc, hence Edata -> Edata/(1+alpha) to match the MC
  // note : all energies in MeV

  // returns alpha_var. var = egEnergyCorr::Scale::Nominal or any systematic variation

  double egammaEnergyCorrectionTool::getAlphaValue( double cl_eta, 
						    double energy,      // input energy (not ET!!)
						    double energyS2,    // raw energy in S2
						    PATCore::ParticleType::Type ptype,
						    egEnergyCorr::Scale::Variation var, 
						    double varSF ) const { 

    double meanE = getZeeMeanET(cl_eta)*cosh(cl_eta);    

    if ( m_debug ) {      
      std::cout << std::endl << "egammaEnergyCorrectionTool::getAlphaValue" << std::endl;
      std::cout << " cl_eta = " << cl_eta<< std::endl;
      std::cout << " energy = " << energy<< std::endl;
      std::cout << " ptype = "  << ptype<< std::endl;
      std::cout << " meanET*cosh(cl_eta) = " << meanE << std::endl;
      std::cout << " PATCore::ParticleType::Electron = "  << PATCore::ParticleType::Electron << std::endl;
    }
    

    // Main Scale factor
    
    double alphaZee = getAlphaZee( cl_eta, var, varSF );
    

    // Sampling recalibration
    
    double daPS, daS12, linPS, linS12;
    daPS = daS12 = linPS = linS12 = 0.;
    
    // ... Presampler contribution    

    if( var==egEnergyCorr::Scale::PSUp || var==egEnergyCorr::Scale::PSDown ||
	var==egEnergyCorr::Scale::LArElecUnconvUp || var==egEnergyCorr::Scale::LArElecUnconvDown ) {      

      daPS  = getLayerUncertainty(  0, cl_eta, var, varSF );
      linPS = getLayerNonLinearity( 0, cl_eta, energy, ptype ) 
	    - getLayerNonLinearity( 0, cl_eta, meanE,  PATCore::ParticleType::Electron );	   
      
    }

    // ... S1 / S2 contribution

    if( var==egEnergyCorr::Scale::S12Up || var==egEnergyCorr::Scale::S12Down ||
	var==egEnergyCorr::Scale::LArCalibUp || var==egEnergyCorr::Scale::LArCalibDown ) {

      daS12  = getLayerUncertainty(  1, cl_eta, var, varSF );
      linS12 = getLayerNonLinearity( 1, cl_eta, energy, ptype ) 
	     - getLayerNonLinearity( 1, cl_eta, meanE,  PATCore::ParticleType::Electron );

    }
    

    // Material contribution

    double daMatID, daMatCryo, daMatCalo;
    daMatID = daMatCryo = daMatCalo = 0;

    if( ptype!=PATCore::ParticleType::Electron ) {

      daMatID   = getAlphaMaterial( cl_eta, egEnergyCorr::MatID,   ptype, var, varSF );
      daMatCryo = getAlphaMaterial( cl_eta, egEnergyCorr::MatCryo, ptype, var, varSF );
      daMatCalo = getAlphaMaterial( cl_eta, egEnergyCorr::MatCalo, ptype, var, varSF );

    } else {

      daMatID   = getMaterialNonLinearity( cl_eta, energy, egEnergyCorr::MatID,   ptype, var, varSF ) 
	        - getMaterialNonLinearity( cl_eta, meanE,  egEnergyCorr::MatID,   ptype, var, varSF );
      daMatCryo = getMaterialNonLinearity( cl_eta, energy, egEnergyCorr::MatCryo, ptype, var, varSF ) 
	        - getMaterialNonLinearity( cl_eta, meanE,  egEnergyCorr::MatCryo, ptype, var, varSF );
      daMatCalo = getMaterialNonLinearity( cl_eta, energy, egEnergyCorr::MatCalo, ptype, var, varSF ) 
	        - getMaterialNonLinearity( cl_eta, meanE,  egEnergyCorr::MatCalo, ptype, var, varSF );

    }
        
    // Pedestal subtraction

    double daPedestal = getAlphaPedestal(cl_eta, energy, ptype, false, var, varSF)
                      - getAlphaPedestal(cl_eta, meanE, PATCore::ParticleType::Electron , true, var, varSF);


    // Leakage contribution (electron-photon difference)

    double daLeakage = getAlphaLeakage(cl_eta, ptype, var, varSF);


    // L1 Gain switch contribution

    double daL1GainSwitch = 0.;

    if ( var==egEnergyCorr::Scale::L1GainUp || var==egEnergyCorr::Scale::L1GainDown ) {

      int eg_e1hg_ptype;
      if( ptype==PATCore::ParticleType::Electron )
	eg_e1hg_ptype = 0;
      else if( ptype==PATCore::ParticleType::UnconvertedPhoton )
	eg_e1hg_ptype = 1;
      else if( ptype==PATCore::ParticleType::ConvertedPhoton )
	eg_e1hg_ptype = 2;
      else
	return -1;

      daL1GainSwitch = m_e1hg_tool->getAlpha(eg_e1hg_ptype, energy, cl_eta, true);
      if( var==egEnergyCorr::Scale::L1GainDown )
	daL1GainSwitch = -daL1GainSwitch;

    }


    // L2 Gain switch contribution

    double daL2GainSwitch = 0.;
    
    if ( var==egEnergyCorr::Scale::L2GainUp || var==egEnergyCorr::Scale::L2GainDown )
      if (!(fabs(cl_eta) < 1.52 and fabs(cl_eta) > 1.37) and fabs(cl_eta) < 2.4) {

	double evar = m_gain_tool->CorrectionGainTool(cl_eta, energy/GeV, energyS2/GeV, ptype);

	double meanES2 = m_zeeES2Profile->GetBinContent(m_zeeES2Profile->FindBin(cl_eta)); // in GeV already
	double eref = m_gain_tool->CorrectionGainTool(cl_eta, meanE/GeV, meanES2, PATCore::ParticleType::Electron );

	daL2GainSwitch = evar/energy - eref/meanE;
	if( var==egEnergyCorr::Scale::L2GainDown )
	  daL2GainSwitch = -daL2GainSwitch;

      }


    // Conversion systematics

    double daConvSyst = getAlphaConvSyst(cl_eta, energy, ptype, var, varSF);


    // debug
    
    if ( m_debug ) {      
      std::cout << std::endl << "egammaEnergyCorrectionTool::getAlphaValue" << std::endl;
      std::cout << "Type / variation : " << ptype << " / " << variationName(var) << std::endl;
      std::cout << "Inputs :           " << cl_eta << " / " << energy << " / " << energyS2 << std::endl;
      std::cout << "  alphaZee       = " <<  alphaZee << std::endl;
      std::cout << "  daPS           = " << daPS << " linPS = " << linPS << std::endl;
      std::cout << "  daS12          = " << daS12 << " linS12 = " << linS12 << std::endl;
      std::cout << "  daMatID        = " << daMatID   << std::endl;
      std::cout << "  daMatCryo      = " << daMatCryo << std::endl;
      std::cout << "  daMatCalo      = " << daMatCalo << std::endl;
      std::cout << "  daLeakage      = " << daLeakage << std::endl;
      std::cout << "  daL1GainSwitch = " << daL1GainSwitch << std::endl;
      std::cout << "  daL2GainSwitch = " << daL2GainSwitch << std::endl;
      std::cout << "  daConvSyst     = " << daConvSyst    << std::endl;
      std::cout << "  daPedestal     = " << daPedestal    << std::endl;
    }


    // Total

    double alphaTot = alphaZee;
    alphaTot += daPS*linPS;
    alphaTot += daS12*linS12;
    alphaTot += daMatID + daMatCryo + daMatCalo;
    alphaTot += daLeakage;
    alphaTot += daL1GainSwitch;
    alphaTot += daL2GainSwitch;
    alphaTot += daConvSyst;
    alphaTot += daPedestal;
 
    return alphaTot;

  } 


  // returns alpha_var - alpha_nom, for systematic variations.

  double egammaEnergyCorrectionTool::getAlphaUncertainty( double cl_eta, 
							  double energy, 
							  double energyS2,
                                                          PATCore::ParticleType::Type ptype,
                                                          egEnergyCorr::Scale::Variation var, double varSF ) const { 
    
    double alphaNom = getAlphaValue( cl_eta, energy, energyS2, ptype, egEnergyCorr::Scale::Nominal );
    double alphaVar = 0.;
    
    if( var != egEnergyCorr::Scale::AllUp && var != egEnergyCorr::Scale::AllDown )
      
      alphaVar = getAlphaValue( cl_eta, energy, energyS2, ptype, var, varSF ) - alphaNom;

    else if( var == egEnergyCorr::Scale::AllUp ) {
	
	for (egEnergyCorr::Scale::Variation ivar = egEnergyCorr::Scale::ZeeStatUp;
	     ivar < egEnergyCorr::Scale::AllUp;
	     ivar = egEnergyCorr::Scale::Variation(ivar+2)) {
	    if( ivar==egEnergyCorr::Scale::ZeeAllUp )
		continue;
	    alphaVar += pow( getAlphaValue( cl_eta, energy, energyS2, ptype, ivar, varSF) - alphaNom, 2 );
	}
	
	alphaVar = sqrt(alphaVar);
      
    } else if( var == egEnergyCorr::Scale::AllDown ) {
	
	for (egEnergyCorr::Scale::Variation v = egEnergyCorr::Scale::ZeeStatDown; v<egEnergyCorr::Scale::AllDown; v = egEnergyCorr::Scale::Variation(v+2)) {
	    if( v==egEnergyCorr::Scale::ZeeAllDown )
		continue;
	    alphaVar += pow( getAlphaValue( cl_eta, energy, energyS2, ptype, v, varSF) - alphaNom, 2 );
	}
	
	alphaVar = -sqrt(alphaVar);
	
    } 

    return alphaVar;
    
  }
  
  
  // returns mean electron ET at given eta
  double egammaEnergyCorrectionTool::getZeeMeanET(double /* cl_eta */) const {
  
    return 40000.; // to be replaced by histogram look-up at some point

  }





  // RESOLUTION FUNCTIONS START HERE (MB)


  // sampling term inMC, parametrization from Iro Koletsou

  double egammaEnergyCorrectionTool::mcSamplingTerm(double cl_eta) const {

    double aeta = fabs( cl_eta );
    double sampling = 0.;

    if ( aeta<0.8 )
      sampling = 0.091;

    else if ( aeta<1.37 )
      sampling = 0.036 + 0.130 * aeta;

    else if ( aeta<1.52 )
      sampling = 0.27;

    else if ( aeta<2.0 )
      sampling = 0.85 - 0.36 * aeta;

    else if ( aeta<2.3 )
      sampling = 0.16;
  
    else if ( aeta<2.5 )
      sampling = -1.05 + 0.52 * aeta;

    return sampling;

  }


  // sampling term uncertainty

  double egammaEnergyCorrectionTool::mcSamplingTermRelError( double cl_eta ) const {

    (void) cl_eta; // not used
    return 0.1; // when will this be improved?

  }


  // noise term in MC (from Iro)

  double egammaEnergyCorrectionTool::mcNoiseTerm( double cl_eta ) const {

    double aeta = fabs( cl_eta );
    double noise = 0.;

    double noise37[25]={ 0.27, 0.27, 0.27, 0.27, 0.27, 
			 0.26, 0.25, 0.23, 0.21, 0.19, 
			 0.17, 0.16, 0.15, 0.14, 0.27, 
			 0.23, 0.17, 0.15, 0.13, 0.10, 
			 0.07, 0.06, 0.05, 0.04, 0.03 };  
    
    int ieta = (int) (aeta/0.1);

    if ( ieta >= 0 && ieta<25 )
      noise =  noise37[ieta] * cosh(cl_eta); // the above parametrization is vs ET

    return noise;

  }


  // constant term in MC (local)

  double egammaEnergyCorrectionTool::mcConstantTerm( double cl_eta ) const {

    double aeta = fabs( cl_eta );
    double cst = 0.;

    if ( aeta<0.6 )
      cst = 0.005;

    else if ( aeta<1.75 )
      cst = 0.003;

    else if ( aeta<2.5 )
      cst = 0.0055 * (2.69 - aeta);

    //cst = 0.005;

    return cst;

  }


  // constant term fitted in data (long range)

  double egammaEnergyCorrectionTool::dataConstantTerm( double cl_eta ) const {

    return std::max(0., m_resNom->GetBinContent( m_resNom->FindBin(cl_eta) ));

  }


  double egammaEnergyCorrectionTool::dataConstantTermError( double cl_eta ) const {

    return m_resSyst->GetBinContent( m_resSyst->FindBin(cl_eta) );

  }


  // fitted Z peak resolution, data, in GeV

  double egammaEnergyCorrectionTool::dataZPeakResolution( double cl_eta ) const {

    return m_peakResData->GetBinContent( m_peakResData->GetXaxis()->FindBin(cl_eta) );

  }


  // fitted Z peak resolution, MC, in GeV

  double egammaEnergyCorrectionTool::mcZPeakResolution( double cl_eta ) const {

    return m_peakResMC->GetBinContent( m_peakResMC->GetXaxis()->FindBin(cl_eta) );

  }


  // correlated part of constant term uncertainty, in data (approx.)

  double egammaEnergyCorrectionTool::dataConstantTermCorError( double cl_eta ) const {

    double mz = 91.2;
  
    double resData = dataZPeakResolution( cl_eta );
    double resMC   = mcZPeakResolution( cl_eta );
    double cmc     = mcConstantTerm( cl_eta );

    double smpup = 1. + mcSamplingTermRelError( cl_eta );
    double smpdo = 1. - mcSamplingTermRelError( cl_eta );

    double central = sqrt( 2*(resData*resData - resMC*resMC)/mz/mz + cmc*cmc );
    double vardown = sqrt( 2*(resData*resData - resMC*resMC*smpup*smpup)/mz/mz + cmc*cmc );
    double varup   = sqrt( 2*(resData*resData - resMC*resMC*smpdo*smpdo)/mz/mz + cmc*cmc );

    double errdown = fabs( central - vardown );
    double errup   = fabs( central - varup );

    return .5*( errup + errdown );

  }

  // get fractional uncertainty on resolution

  double egammaEnergyCorrectionTool::getResolutionError(double energy, double eta,PATCore::ParticleType::Type ptype,egEnergyCorr::Resolution::Variation value,
                                                        egEnergyCorr::Resolution::resolutionType resType ) const

  {

    int eg_resolution_ptype;
    if( ptype==PATCore::ParticleType::Electron )
      eg_resolution_ptype = 0;
    else if( ptype==PATCore::ParticleType::UnconvertedPhoton )
      eg_resolution_ptype = 1;
    else if( ptype==PATCore::ParticleType::ConvertedPhoton )
      eg_resolution_ptype = 2;
    else    
      return -1;

    int isys=0;
    double sign=1.;
    if (value==egEnergyCorr::Resolution::AllUp || value==egEnergyCorr::Resolution::AllDown) {
       isys=0xFFFF;
    }
    if (value==egEnergyCorr::Resolution::ZSmearingUp || value==egEnergyCorr::Resolution::ZSmearingDown) {
       isys=0x1;
    }
    if (value==egEnergyCorr::Resolution::SamplingTermUp || value==egEnergyCorr::Resolution::SamplingTermDown) {
       isys=0x2;
    }
    if (value==egEnergyCorr::Resolution::MaterialIDUp || value==egEnergyCorr::Resolution::MaterialIDDown) {
       isys=0x4;
    }
    if (value==egEnergyCorr::Resolution::MaterialCaloUp || value==egEnergyCorr::Resolution::MaterialCaloDown) {
       isys=0x8;
    }
    if (value==egEnergyCorr::Resolution::MaterialGapUp || value==egEnergyCorr::Resolution::MaterialGapDown) {
       isys=0x10;
    }
    if (value==egEnergyCorr::Resolution::MaterialCryoUp || value==egEnergyCorr::Resolution::MaterialCryoDown) {
       isys=0x20;
    }
    if (value==egEnergyCorr::Resolution::PileUpUp || value==egEnergyCorr::Resolution::PileUpDown) {
       isys=0x40;  
    }
    if (value==egEnergyCorr::Resolution::AllDown ||  value==egEnergyCorr::Resolution::ZSmearingDown ||
        value==egEnergyCorr::Resolution::SamplingTermDown ||  value==egEnergyCorr::Resolution::MaterialIDDown ||
        value==egEnergyCorr::Resolution::MaterialGapDown || value==egEnergyCorr::Resolution::MaterialCaloDown ||
        value==egEnergyCorr::Resolution::MaterialCryoDown ||
        value==egEnergyCorr::Resolution::PileUpDown)  sign=-1.;

    double resolution;
    double resolution_error;
    double resolution_error_up;
    double resolution_error_down;

    getResolution_systematics(eg_resolution_ptype, energy,  eta, isys,  resolution, resolution_error, resolution_error_up, resolution_error_down, resType);
    
    // total resolution uncertainty
    if (value==egEnergyCorr::Resolution::AllUp || value==egEnergyCorr::Resolution::AllDown) {
      resolution_error = resolution_error / resolution * sign; 
    } else {
      if (sign==1) resolution_error= resolution_error_up/resolution;
      else         resolution_error=resolution_error_down/resolution;
    }

    return resolution_error;

  }

  // total resolution uncertainty (fractional)  (OLD model)

  void egammaEnergyCorrectionTool::resolutionError( double energy /* GeV! */, double cl_eta, double& errUp, double& errDown ) const {

    double Cdata     = dataConstantTerm( cl_eta );
    double Cdata_cor = dataConstantTermCorError( cl_eta );
    double Cdata_err = dataConstantTermError( cl_eta );
  
    double Cdata_unc = 0.;
    if( Cdata_err > Cdata_cor )
      Cdata_unc = sqrt( Cdata_err*Cdata_err - Cdata_cor*Cdata_cor );
    if( Cdata_unc<0.001 )
      Cdata_unc = 0.001; // preserve at least the stat error

    double Smc       = mcSamplingTerm( cl_eta );
    double Smc_err   = mcSamplingTermRelError( cl_eta );

    double central = fcn_sigma( energy,  Cdata,  0.,  Smc,  0.);

    double err1 = fcn_sigma( energy, Cdata,  Cdata_unc, Smc,  0.    ) - central; 
    double err2 = fcn_sigma( energy, Cdata, -Cdata_unc, Smc,  0.    ) - central; 
    double err3 = fcn_sigma( energy, Cdata, -Cdata_cor, Smc, Smc_err) - central;
    double err4 = -err3;

    errUp = 0;
    if( err1>0 ) errUp = sqrt( errUp*errUp + err1*err1);
    if( err2>0 ) errUp = sqrt( errUp*errUp + err2*err2);
    if( err3>0 ) errUp = sqrt( errUp*errUp + err3*err3);
    if( err4>0 ) errUp = sqrt( errUp*errUp + err4*err4);

    errDown   = -errUp;
  }


  // total resolution (fractional)

  double egammaEnergyCorrectionTool::resolution( double energy, double cl_eta, 
						 PATCore::ParticleType::Type ptype,
						 bool withCT,
                                                 egEnergyCorr::Resolution::resolutionType resType ) const {

    int eg_resolution_ptype;
    if( ptype==PATCore::ParticleType::Electron )
      eg_resolution_ptype = 0;
    else if( ptype==PATCore::ParticleType::UnconvertedPhoton )
      eg_resolution_ptype = 1;
    else if( ptype==PATCore::ParticleType::ConvertedPhoton )
      eg_resolution_ptype = 2;
    else
      return -1;

    //double sig2 = pow(m_resolution_tool->getResolution(eg_resolution_ptype, energy, cl_eta), 2);
    //if( withCT )
    //sig2 += pow(dataConstantTerm( cl_eta ), 2);
    
    double sig2 =0;
    if ( m_esmodel==egEnergyCorr::es2012c || m_esmodel==egEnergyCorr::es2011d ){
      sig2=pow(m_resolution_tool->getResolution(eg_resolution_ptype, energy, cl_eta, resType), 2);
      double et = energy/cosh(cl_eta);
      sig2 += pow(pileUpTerm(cl_eta,eg_resolution_ptype)/et,2);
    } else {
      double energyGeV = energy/GeV;
      double a = mcSamplingTerm( cl_eta );
      double b = mcNoiseTerm( cl_eta );
      double c = mcConstantTerm( cl_eta );
      sig2 = a*a/energyGeV + b*b/energyGeV/energyGeV + c*c;
    }
    if( withCT )
      sig2 += pow(dataConstantTerm( cl_eta ), 2);
    
    /*
      double sig2_bis=0;
      double a = mcSamplingTerm( cl_eta );
      double b = mcNoiseTerm( cl_eta );
      double c = mcConstantTerm( cl_eta );
      sig2_bis = a*a/energy + b*b/energy/energy + c*c;
      if( withCT )
      sig2_bis += pow(dataConstantTerm( cl_eta ), 2);
      //std::cout << std::endl << "MC12ab sig2 = " << sig2_bis << std::endl;
      
      std::cout << std::endl << " sig2 relative diff (%) = " << (fabs(sig2_bis-sig2)/sig2)*100 << std::endl;
    */


    if( m_debug ) {
      std::cout << std::endl << "egammaEnergyCorrectionTool::resolution" << std::endl;
      std::cout << " - inputs  : " << energy << " / " << cl_eta << " / " << ptype << " / " << withCT << std::endl;
      std::cout << " - outputs : " << sqrt(sig2) << std::endl;
      std::cout << std::endl;
    }

    return sqrt(sig2);

  }


  // internal use only

  double egammaEnergyCorrectionTool::fcn_sigma(double energy, double Cdata, double Cdata_er, double S, double S_er) const {

    double sigma2 = std::pow((Cdata+Cdata_er)*energy,2) + std::pow(S*(1+S_er)*std::sqrt(energy),2);
  
    double sigma=0; 
    if (sigma2>0) 
      sigma=sqrt(sigma2);
  
    return sigma/energy;

  }


  // derive smearing correction

  double egammaEnergyCorrectionTool::getSmearingCorrection( double eta, double energy,
                                                            PATCore::ParticleType::Type ptype,
							    egEnergyCorr::Resolution::Variation value, 
                                                            egEnergyCorr::Resolution::resolutionType resType) const {

    if ( value == egEnergyCorr::Resolution::None || energy<=0. )
      return 1.0;

    double energyGeV = energy/GeV;

    double resMC, resData, errUp, errDown;
    resMC   = resolution( energy, eta, ptype, false ,resType);
    resData = resolution( energy, eta, ptype, true  ,resType);

    if ( m_esmodel==egEnergyCorr::es2012c || m_esmodel==egEnergyCorr::es2011d ) {

       resData *= 1 + getResolutionError(energy,eta,ptype,value,resType);

    } else { // OLD model

      resolutionError( energyGeV, eta, errUp, errDown );
      if( value == egEnergyCorr::Resolution::AllDown )
       resData += errDown;
      else if( value == egEnergyCorr::Resolution::AllUp )
       resData += errUp;
      else if( value != egEnergyCorr::Resolution::Nominal ) {
       //      std::cout << "getSmearingCorrection : wrong value, return 1" << std::endl;
       return 1.0;
      }

    }
  
    double sigma2 = std::pow( resData*energyGeV, 2 ) - std::pow( resMC*energyGeV, 2 );
  
    if (sigma2<=0) return 1;
  
    double sigma = sqrt(sigma2);
    double DeltaE0 = m_random3.Gaus(0,sigma);

    double cor0=(energyGeV+DeltaE0)/energyGeV;
  
    return cor0;
  
  }


  // a calibration correction for crack electrons, to be applied to both data11 and MC11
  // not to be used in data12 / MC12
  
  double egammaEnergyCorrectionTool::applyMCCalibration( double eta, double ET, PATCore::ParticleType::Type ptype) const {
    
    if( ptype != PATCore::ParticleType::Electron || m_esmodel != egEnergyCorr::es2011c )
      return 1.;
    
    double aeta = fabs(eta);
    
    if( aeta<1.42 || aeta>1.55 )
      return 1.;
    
    const int nBoundaries = 18;
    double ETBoundaries[nBoundaries]   = { 0., 5.4, 8.5, 12.9, 16., 20., 
                                           25., 30., 35., 40., 45., 50., 
                                           55., 60., 65., 70., 75., 99999. };
    
    double CalibFactors[nBoundaries-1] = { 0.884845, 0.898526, 0.902439, 0.91899, 0.925868, 0.929440, 
                                           0.948080, 0.943788, 0.96026, 0.955709, 0.964285, 0.95762, 
                                           0.970385, 0.963489, 0.968149, 0.970799, 0.961617 };
    
    int i0 = -1;
    for ( int i=0; i<nBoundaries-1; i++)
      if ( ET/GeV>ETBoundaries[i] && ET/GeV<=ETBoundaries[i+1] )
        i0 = i;
    
    if( i0>=0 && i0<nBoundaries-1 )
      return 1./CalibFactors[i0];

    return 1.;
	
  }

	
  // AF -> G4 correction

  double egammaEnergyCorrectionTool::applyAFtoG4(double eta) const {

    double aeta = fabs(eta);
    if( aeta>2.47 )
      return 1.;
 
    return m_G4OverAFII->GetBinContent( m_G4OverAFII->GetXaxis()->FindBin(eta) );

  }  


  // Frozen Showers -> G4 correction

  double egammaEnergyCorrectionTool::applyFStoG4(double eta) const {

    double aeta = fabs(eta);
    if( aeta<3.3 || aeta>4.9 )
      return 1.;
    
    return m_G4OverFrSh->GetBinContent( m_G4OverFrSh->GetXaxis()->FindBin(aeta) );

  }  

  // functions for energy scale corrections

  double egammaEnergyCorrectionTool::getAlphaZee( double cl_eta, egEnergyCorr::Scale::Variation var, double varSF) const {

    if ( m_debug )
      std::cout << std::endl << "egammaEnergyCorrectionTool::getAlphaZee" << std::endl;

    if (!m_zeeNom) { 
      std::cerr << m_esmodel << std::endl; 
      assert(m_zeeNom);
      return -999.0;
    }
    
    int ieta = m_zeeNom->GetXaxis()->FindBin( cl_eta );
    double value = m_zeeNom->GetBinContent(ieta);
    
    if( var==egEnergyCorr::Scale::ZeeStatUp ) 
      
      value += m_zeeNom->GetBinError(ieta) * varSF;
    
    else if( var==egEnergyCorr::Scale::ZeeStatDown )

      value -= m_zeeNom->GetBinError(ieta) * varSF;

    else if( var==egEnergyCorr::Scale::ZeeSystUp && m_zeeSyst ) {

      ieta = m_zeeSyst->GetXaxis()->FindBin( cl_eta );
      value += m_zeeSyst->GetBinContent(ieta) * varSF;

      if( m_debug )
        std::cout << "  ZeeSystUp : " << m_zeeSyst->GetBinContent(ieta) << std::endl;

    } else if( var==egEnergyCorr::Scale::ZeeSystDown && m_zeeSyst ) {

      ieta = m_zeeSyst->GetXaxis()->FindBin( cl_eta );
      value -= m_zeeSyst->GetBinContent(ieta) * varSF;

      if( m_debug )
        std::cout << "  ZeeSystDown : " << m_zeeSyst->GetBinContent(ieta) << std::endl;
    
    } else if( var==egEnergyCorr::Scale::ZeeAllDown || var==egEnergyCorr::Scale::ZeeAllUp ) {
      
      ieta = m_zeeNom->GetXaxis()->FindBin( cl_eta );
      double diff = pow(m_zeeNom->GetBinError(ieta) * varSF, 2);

      if( m_zeeSyst ) {
        ieta = m_zeeSyst->GetXaxis()->FindBin( cl_eta );
        diff += pow(m_zeeSyst->GetBinContent(ieta) * varSF, 2);
      }

      if( var==egEnergyCorr::Scale::ZeeAllUp )
        value += sqrt(diff);
      else if( var==egEnergyCorr::Scale::ZeeAllDown )
        value -= sqrt(diff);

    }

    return value;

  }


  // Layer scale uncertainties and induced non-linearity
  //////////////////////////////////////////////////////
    
  double egammaEnergyCorrectionTool::getLayerUncertainty( int iLayer, 
							  double cl_eta, 
							  egEnergyCorr::Scale::Variation var,
                                                          double varSF) const {
    
    double value = 0.;
    
    if( var==egEnergyCorr::Scale::Nominal )
      return value;

    // nearest eta outside of crack (for PS scale values and uncertainties)
    double nearestEta = nearestEtaBEC(cl_eta);
  
    if( iLayer==0 ) { // use nearestEta

      if( var==egEnergyCorr::Scale::PSUp && m_aPSNom ) 
	value =  m_aPSNom->GetBinError( m_aPSNom->FindBin(nearestEta) );

      else if( var==egEnergyCorr::Scale::PSDown && m_aPSNom )
	value = -m_aPSNom->GetBinError( m_aPSNom->FindBin(nearestEta) );

      else if( var==egEnergyCorr::Scale::LArElecUnconvUp && m_daPSCor )
	value =  m_daPSCor->GetBinContent( m_daPSCor->FindBin(nearestEta) );

      else if( var==egEnergyCorr::Scale::LArElecUnconvDown && m_daPSCor )
	value = -m_daPSCor->GetBinContent( m_daPSCor->FindBin(nearestEta) );

    } else if( iLayer==1 ) { // use cl_eta

      if( var==egEnergyCorr::Scale::S12Up && m_aS12Nom )
	value =  m_aS12Nom->GetBinError( m_aS12Nom->FindBin(cl_eta) );

      else if( var==egEnergyCorr::Scale::S12Down && m_aS12Nom )
	value = -m_aS12Nom->GetBinError( m_aS12Nom->FindBin(cl_eta) );

      else if( var==egEnergyCorr::Scale::LArCalibUp && m_daS12Cor )
	value =  m_daS12Cor->GetBinContent( m_daS12Cor->FindBin(cl_eta) );

      else if( var==egEnergyCorr::Scale::LArCalibDown && m_daS12Cor )
	value = -m_daS12Cor->GetBinContent( m_daS12Cor->FindBin(cl_eta) );

    }

    if( m_debug ) {
      std::cout << std::endl << "egammaEnergyCorrectionTool::getLayerUncertainty" << std::endl;
      std::cout << "  Eta = " << cl_eta << " / " << nearestEta << std::endl;
      std::cout << "  Var = " << variationName(var) << std::endl;
      std::cout << "  Layer = " << iLayer << std::endl;
      std::cout << "  Scale = " << value << " * " << varSF << std::endl;
    }

    return value * varSF;

  }


  double egammaEnergyCorrectionTool::getLayerNonLinearity( int iLayer, 
							   double cl_eta, 
							   double energy, 
							   PATCore::ParticleType::Type ptype ) const {
    
    double value = 0;
    double aeta = fabs(cl_eta);
    double ET = energy/cosh(cl_eta);
    
    // move out of crack
    aeta = nearestEtaBEC(aeta);
    
    // argument ET is transverse energy in MeV
    
    if( iLayer==0 && aeta>=1.82 )
      return value;  
    
    if( ptype==PATCore::ParticleType::Electron ) {
      
      if( iLayer==0 ) {

	const int ieta = m_psElectronEtaBins->FindFixBin( aeta ) - 1;
	if (ieta >= 0 and ieta < m_psElectronGraphs->GetSize()) {
	    value = ((TF1*)m_psElectronGraphs->At(ieta))->Eval( ET );
	}
      } else if( iLayer==1 ) {

	const int ieta = m_s12ElectronEtaBins->FindFixBin( aeta ) - 1;
	if (ieta >= 0 and ieta < m_s12ElectronGraphs->GetSize()) {
	    value = ((TF1*)m_s12ElectronGraphs->At(ieta))->Eval( ET );
	}
      }
      
    } else if( ptype==PATCore::ParticleType::UnconvertedPhoton ) {
      
      if( iLayer==0 ) {

	const int ieta = m_psUnconvertedEtaBins->FindBin( aeta ) - 1;
	if (ieta >= 0 and ieta < m_psUnconvertedGraphs->GetSize()) {
	    value = ((TF1*)m_psUnconvertedGraphs->At(ieta))->Eval( ET );
	}

      } else if( iLayer==1 ) {

	const int ieta = m_s12UnconvertedEtaBins->FindBin( aeta ) - 1;
	if (ieta >= 0 and ieta < m_s12UnconvertedGraphs->GetSize()) {
	    value = ((TF1*)m_s12UnconvertedGraphs->At(ieta))->Eval( ET );
	}
      }
      
    } else if( ptype==PATCore::ParticleType::ConvertedPhoton ) {
      
      if( iLayer==0 ) {

	const int ieta = m_psConvertedEtaBins->FindBin( aeta ) - 1;
	if (ieta >= 0 and ieta < m_psConvertedGraphs->GetSize()) {
	    value = ((TF1*)m_psConvertedGraphs->At(ieta))->Eval( ET );
	}

      } else if( iLayer==1 ) {

	const int ieta = m_s12ConvertedEtaBins->FindBin( aeta ) - 1;
	if (ieta >= 0 and ieta < m_s12ConvertedGraphs->GetSize()) {
	    value = ((TF1*)m_s12ConvertedGraphs->At(ieta))->Eval( ET );
	}
      }
      
    }
   
    if( m_debug ) {
      std::cout << std::endl << "egammaEnergyCorrectionTool::getLayerNonLinearity" << std::endl;
      std::cout << "  Eta = " << cl_eta << " / " << aeta << std::endl;
      std::cout << "  Layer = " << iLayer << std::endl;
      std::cout << "  Value = " << value << std::endl;
    }

    if (value < 0) {
	if (m_debug) { std::cout << "  Value is negative -> set to 0" << std::endl; }
	value = 0;
    }

    return value;
    
  }


  // passive material correction
  //////////////////////////////

  // ... material look-up function, called internally by getAlphaMaterial() and getMaterialNonLinearity()

  double egammaEnergyCorrectionTool::getDeltaX( double cl_eta, 
						egEnergyCorr::MaterialCategory imat, 
						egEnergyCorr::Scale::Variation var ) const {

    double value = 0.;
    double aeta = fabs(cl_eta);

    // "ID" : inner detector material; bottom-up (from construction/simulation accuracy : ConfigA)

    if( imat==egEnergyCorr::MatID && m_dX_ID_Nom ) {

      // ... NOTE : watch out here : this histo does not follow the usual value/error look-up convention

      if( var==egEnergyCorr::Scale::MatIDUp )
	value += m_dX_ID_Nom->GetBinContent( m_dX_ID_Nom->FindBin(aeta) );
      else if( var==egEnergyCorr::Scale::MatIDDown )
	value -= m_dX_ID_Nom->GetBinContent( m_dX_ID_Nom->FindBin(aeta) );

    // "Cryo" : integral from IP to PS or Acc, depending on eta

    } else if( imat==egEnergyCorr::MatCryo && aeta<1.82 && m_dX_IPPS_Nom ) { // Integral between IP and PS

      value = m_dX_IPPS_Nom->GetBinContent( m_dX_IPPS_Nom->FindBin(aeta) );
      
      if( var==egEnergyCorr::Scale::MatCryoUp )
	value += m_dX_IPPS_Nom->GetBinError( m_dX_IPPS_Nom->FindBin(aeta) );
      else if( var==egEnergyCorr::Scale::MatCryoDown )
	value -= m_dX_IPPS_Nom->GetBinError( m_dX_IPPS_Nom->FindBin(aeta) );

      // ... careful : sign below should be opposite to the effect of this source on the PS scale!
      if( var==egEnergyCorr::Scale::LArElecUnconvUp && m_dX_IPPS_LAr )
	value -= m_dX_IPPS_LAr->GetBinContent( m_dX_IPPS_LAr->FindBin(aeta) ); 
      else if( var==egEnergyCorr::Scale::LArElecUnconvDown && m_dX_IPPS_LAr )
	value += m_dX_IPPS_LAr->GetBinContent( m_dX_IPPS_LAr->FindBin(aeta) );

    } else if( imat==egEnergyCorr::MatCryo && aeta>1.82 && m_dX_IPAcc_Nom ) { // Integral between IP and Accordion

      value = m_dX_IPAcc_Nom->GetBinContent( m_dX_IPAcc_Nom->FindBin(aeta) );
      
      if( var==egEnergyCorr::Scale::MatCryoUp )
	value += m_dX_IPAcc_Nom->GetBinError( m_dX_IPAcc_Nom->FindBin(aeta) );
      else if( var==egEnergyCorr::Scale::MatCryoDown )
	value -= m_dX_IPAcc_Nom->GetBinError( m_dX_IPAcc_Nom->FindBin(aeta) );

      if( var==egEnergyCorr::Scale::LArElecCalibUp && m_dX_IPAcc_LAr )
	value += m_dX_IPAcc_LAr->GetBinContent( m_dX_IPAcc_LAr->FindBin(aeta) );
      else if( var==egEnergyCorr::Scale::LArElecCalibDown && m_dX_IPAcc_LAr )
	value -= m_dX_IPAcc_LAr->GetBinContent( m_dX_IPAcc_LAr->FindBin(aeta) );

      if( var==egEnergyCorr::Scale::G4Up && m_dX_IPAcc_G4 )
	value += m_dX_IPAcc_G4->GetBinContent( m_dX_IPAcc_G4->FindBin(aeta) );
      else if( var==egEnergyCorr::Scale::G4Down && m_dX_IPAcc_G4 )
	value -= m_dX_IPAcc_G4->GetBinContent( m_dX_IPAcc_G4->FindBin(aeta) );

      if( var==egEnergyCorr::Scale::L1GainUp && m_dX_IPAcc_GL1 )
      	value += m_dX_IPAcc_GL1->GetBinContent( m_dX_IPAcc_GL1->FindBin(aeta) );
      else if( var==egEnergyCorr::Scale::L1GainDown && m_dX_IPAcc_GL1 )
      	value -= m_dX_IPAcc_GL1->GetBinContent( m_dX_IPAcc_GL1->FindBin(aeta) );

    // "Calo" : between PS and Strips

    } else if( imat==egEnergyCorr::MatCalo && aeta<1.82 && m_dX_PSAcc_Nom ) {

      value = m_dX_PSAcc_Nom->GetBinContent( m_dX_PSAcc_Nom->FindBin(aeta) );
      
      if( var==egEnergyCorr::Scale::MatCaloUp )
	value += m_dX_PSAcc_Nom->GetBinError( m_dX_PSAcc_Nom->FindBin(aeta) );
      else if( var==egEnergyCorr::Scale::MatCaloDown )
	value -= m_dX_PSAcc_Nom->GetBinError( m_dX_PSAcc_Nom->FindBin(aeta) );

      if( var==egEnergyCorr::Scale::LArUnconvCalibUp && m_dX_PSAcc_LAr )
	value += m_dX_PSAcc_LAr->GetBinContent( m_dX_PSAcc_LAr->FindBin(aeta) );
      else if( var==egEnergyCorr::Scale::LArUnconvCalibDown && m_dX_PSAcc_LAr )
	value -= m_dX_PSAcc_LAr->GetBinContent( m_dX_PSAcc_LAr->FindBin(aeta) );

      if( var==egEnergyCorr::Scale::G4Up && m_dX_PSAcc_G4 )
	value += m_dX_PSAcc_G4->GetBinContent( m_dX_PSAcc_G4->FindBin(aeta) );
      else if( var==egEnergyCorr::Scale::G4Down && m_dX_PSAcc_G4 )
	value -= m_dX_PSAcc_G4->GetBinContent( m_dX_PSAcc_G4->FindBin(aeta) );

    }

    // debug

    if( m_debug ) {
      std::cout << std::endl << "egammaEnergyCorrectionTool::getDeltaX" << std::endl;
      std::cout << " - inputs  : " << cl_eta << " / " << imat << " / " << variationName(var) << std::endl;
      std::cout << " - outputs : " << value << std::endl;
      std::cout << std::endl;
    }

    // done

    return value;

  }


  // returns the impact of material variation on response.
  // non-zero for photons only (the average effect is absorbed by the effective Z scales for electrons).

  // Strategy for material before the PS : 
  // - consider ID material and uncertainty fixed to ConfigA
  // - attribute measured material to ConfigL, with uncertainty from difference between measurement and ConfigA
  //   I.e : DX_A = 0 +- dA ; DX_L = DX_Meas +- (dMeas ++ dA)

  // Strategy for material after the PS : 
  // - direct measurement
  //   I.e : DX_M = DX_Meas +- dMeas

  // Then calculate the impact on the scale accordingly.

  double egammaEnergyCorrectionTool::getAlphaMaterial( double cl_eta, 
						       egEnergyCorr::MaterialCategory imat, 
						       PATCore::ParticleType::Type ptype,
						       egEnergyCorr::Scale::Variation var, double varSF ) const {
    
    double value=0.;
    if( ptype==PATCore::ParticleType::Electron )
      return value;
    if( var==egEnergyCorr::Scale::Nominal )
      return value;

    egEnergyCorr::Geometry geoID, geoCryo, geoCalo, geoGp;
    geoID = egEnergyCorr::ConfigA;
    if( fabs(cl_eta)<2. ) 
      geoCryo = egEnergyCorr::ConfigEL;
    else
      geoCryo = egEnergyCorr::ConfigFMX;
    geoCalo = egEnergyCorr::ConfigFMX;
    geoGp = egEnergyCorr::ConfigGp;

    // look up material bias

    double DeltaX = getDeltaX(cl_eta, imat, var) - getDeltaX(cl_eta, imat, egEnergyCorr::Scale::Nominal);

    // calculate scale change per unit added material      
    
    double DAlphaDXID, DAlphaDXCryo, DAlphaDXCalo, DAlphaDXGp;
    DAlphaDXID = DAlphaDXCryo = DAlphaDXCalo = DAlphaDXGp = 0;
    if( ptype==PATCore::ParticleType::UnconvertedPhoton ) {        
      DAlphaDXGp   = m_matUnconvertedScale[geoGp]->GetBinContent( m_matUnconvertedScale[geoGp]->FindBin(cl_eta) );
      DAlphaDXID   = m_matUnconvertedScale[geoID]->GetBinContent( m_matUnconvertedScale[geoID]->FindBin(cl_eta) );
      DAlphaDXCryo = m_matUnconvertedScale[geoCryo]->GetBinContent( m_matUnconvertedScale[geoCryo]->FindBin(cl_eta) );
      DAlphaDXCalo = m_matUnconvertedScale[geoCalo]->GetBinContent( m_matUnconvertedScale[geoCalo]->FindBin(cl_eta) );
    } else if( ptype==PATCore::ParticleType::ConvertedPhoton ) {
      DAlphaDXGp   = m_matConvertedScale[geoGp]->GetBinContent( m_matConvertedScale[geoGp]->FindBin(cl_eta) );
      DAlphaDXID   = m_matConvertedScale[geoID]->GetBinContent( m_matConvertedScale[geoID]->FindBin(cl_eta) );
      DAlphaDXCryo = m_matConvertedScale[geoCryo]->GetBinContent( m_matConvertedScale[geoCryo]->FindBin(cl_eta) );
      DAlphaDXCalo = m_matConvertedScale[geoCalo]->GetBinContent( m_matConvertedScale[geoCalo]->FindBin(cl_eta) );
    }

    // when in crack, use G', exit

    if( isInCrack(cl_eta) ) {
      if( imat==egEnergyCorr::MatID && var==egEnergyCorr::Scale::MatIDUp )
	value = DAlphaDXGp;
      else if( imat==egEnergyCorr::MatID && var==egEnergyCorr::Scale::MatIDDown )
	value = -DAlphaDXGp;
      return value; 
    }

    // normal case

    int idx = m_matX0Additions[geoID]->FindBin( fabs(cl_eta) );
    if(idx<1 || idx>m_matX0Additions[geoID]->GetNbinsX())
      DAlphaDXID = 0;
    else
      DAlphaDXID /= m_matX0Additions[geoID]->GetBinContent(idx);

    idx = m_matX0Additions[geoCryo]->FindBin( fabs(cl_eta) );
    if(idx<1 || idx>m_matX0Additions[geoCryo]->GetNbinsX())
      DAlphaDXCryo = 0;
    else
      DAlphaDXCryo /= m_matX0Additions[geoCryo]->GetBinContent(idx);

    idx = m_matX0Additions[geoCalo]->FindBin( fabs(cl_eta) );
    if(idx<1 || idx>m_matX0Additions[geoCalo]->GetNbinsX())
      DAlphaDXCalo = 0;
    else
      DAlphaDXCalo /= m_matX0Additions[geoCalo]->GetBinContent(idx);

    // final value

    if( imat==egEnergyCorr::MatID )       
      value = DeltaX * (DAlphaDXID - DAlphaDXCryo);
    else if( imat==egEnergyCorr::MatCryo )
      value = DeltaX * DAlphaDXCryo;
    else if( imat==egEnergyCorr::MatCalo ) 
      value = DeltaX * DAlphaDXCalo;

    // debug

    if( m_debug ) {
      std::cout << std::endl << "egammaEnergyCorrectionTool::getAlphaMaterial" << std::endl;
      std::cout << " - inputs    : " << cl_eta << " / " << imat << " / " << ptype << " / " << variationName(var) << std::endl;
      std::cout << " - outputs-1 : " << DAlphaDXID << " / " << DAlphaDXCryo << " / " << DAlphaDXCalo << std::endl;
      std::cout << " - outputs-2 : " << DeltaX << " / " << value  << std::endl;
      std::cout << std::endl;
    }

    // done
	
    return value * varSF;
    
  }
  
  
  // returns the energy dependence of the above (non-zero for electrons only).
  
  double egammaEnergyCorrectionTool::getMaterialNonLinearity( double cl_eta, double energy, 
							      egEnergyCorr::MaterialCategory imat, 
							      PATCore::ParticleType::Type ptype,
							      egEnergyCorr::Scale::Variation var, double varSF ) const {
    
    double value = 0;
    double ET = energy/cosh(cl_eta)/GeV;

    if( ptype!=PATCore::ParticleType::Electron || var==egEnergyCorr::Scale::Nominal )
      return value;

    egEnergyCorr::Geometry geoID, geoCryo, geoCalo, geoGp;
    geoID = egEnergyCorr::ConfigA;
    if( fabs(cl_eta)<2. ) 
      geoCryo = egEnergyCorr::ConfigEL;
    else
      geoCryo = egEnergyCorr::ConfigFMX;
    geoCalo = egEnergyCorr::ConfigFMX;
    geoGp = egEnergyCorr::ConfigGp;
    
    // look up material bias
    
    double DeltaX = getDeltaX(cl_eta, imat, var) - getDeltaX(cl_eta, imat, egEnergyCorr::Scale::Nominal);
    
    // calculate scale change per unit added material      

    int ialpha = m_matElectronEtaBins->FindBin( fabs(cl_eta) ) - 1;
    if (ialpha<0 || ialpha>=m_matElectronGraphs[geoGp]->GetSize())
      return 0.;
    
    double DAlphaDXID, DAlphaDXCryo, DAlphaDXCalo, DAlphaDXGp;
    DAlphaDXID = DAlphaDXCryo = DAlphaDXCalo = DAlphaDXGp = 0;
    
    DAlphaDXGp   = ((TGraphErrors*)m_matElectronGraphs[geoGp]->At(ialpha))->GetFunction("fNonLin")->Eval( ET );
    DAlphaDXID   = ((TGraphErrors*)m_matElectronGraphs[geoID]->At(ialpha))->GetFunction("fNonLin")->Eval( ET );
    DAlphaDXCryo = ((TGraphErrors*)m_matElectronGraphs[geoCryo]->At(ialpha))->GetFunction("fNonLin")->Eval( ET );
    DAlphaDXCalo = ((TGraphErrors*)m_matElectronGraphs[geoCalo]->At(ialpha))->GetFunction("fNonLin")->Eval( ET );

    // when in crack, use G', exit

    if( isInCrack(cl_eta) ) {
      if( imat==egEnergyCorr::MatID && var==egEnergyCorr::Scale::MatIDUp )
	value = DAlphaDXGp;
      else if( imat==egEnergyCorr::MatID && var==egEnergyCorr::Scale::MatIDDown )
	value = -DAlphaDXGp;
      return value; 
    }
    
    int idx = m_matX0Additions[geoID]->FindBin( fabs(cl_eta) );
    if(idx<1 || idx>m_matX0Additions[geoID]->GetNbinsX())
      DAlphaDXID = 0;
    else
      DAlphaDXID /= m_matX0Additions[geoID]->GetBinContent(idx);

    idx = m_matX0Additions[geoCryo]->FindBin( fabs(cl_eta) );
    if(idx<1 || idx>m_matX0Additions[geoCryo]->GetNbinsX())
      DAlphaDXCryo = 0;
    else
      DAlphaDXCryo /= m_matX0Additions[geoCryo]->GetBinContent(idx);

    idx = m_matX0Additions[geoCalo]->FindBin( fabs(cl_eta) );
    if(idx<1 || idx>m_matX0Additions[geoCalo]->GetNbinsX())
      DAlphaDXCalo = 0;
    else
      DAlphaDXCalo /= m_matX0Additions[geoCalo]->GetBinContent(idx);

    // final value

    if( imat==egEnergyCorr::MatID )       
      value = DeltaX * (DAlphaDXID - DAlphaDXCryo);
    else if( imat==egEnergyCorr::MatCryo )
      value = DeltaX * DAlphaDXCryo;
    else if( imat==egEnergyCorr::MatCalo ) 
      value = DeltaX * DAlphaDXCalo;

    // debug
    
    if( m_debug ) {
      std::cout << std::endl << "egammaEnergyCorrectionTool::getMaterialNonLinearity" << std::endl;
      std::cout << " - inputs  : " << cl_eta << " / " << imat << " / " << ptype << " / " << ET << " / " << variationName(var) << std::endl;
      std::cout << " - outputs-1 : " << DAlphaDXID << " / " << DAlphaDXCryo << " / " << DAlphaDXCalo << std::endl;
      std::cout << " - outputs-2 : " << DeltaX << " / " << value << " / " << isInCrack(cl_eta) << " / " << value << std::endl;
      std::cout << std::endl;
    }

    // done
	
    return value * varSF;
    
  }



  double egammaEnergyCorrectionTool::getAlphaLeakage(double cl_eta, PATCore::ParticleType::Type ptype,
						     egEnergyCorr::Scale::Variation var, double varSF) const {
    
    double alpha = 0.;
    double aeta = fabs(cl_eta);
    
    if( var==egEnergyCorr::Scale::Nominal || ptype==PATCore::ParticleType::Electron )
      return alpha;
    
    if( ptype==PATCore::ParticleType::UnconvertedPhoton ) {
      
      if( var==egEnergyCorr::Scale::LeakageUnconvUp ) {
	alpha =  m_leakageUnconverted->GetBinContent( m_leakageUnconverted->FindBin(aeta) );
      } else if( var==egEnergyCorr::Scale::LeakageUnconvDown ) {
	alpha = -m_leakageUnconverted->GetBinContent( m_leakageUnconverted->FindBin(aeta) );
      }
      
    } else if( ptype==PATCore::ParticleType::ConvertedPhoton ) {
      
      if( var==egEnergyCorr::Scale::LeakageConvUp ) {
	alpha =  m_leakageConverted->GetBinContent( m_leakageConverted->FindBin(aeta) );
      } else if( var==egEnergyCorr::Scale::LeakageConvDown ) {
	alpha = -m_leakageConverted->GetBinContent( m_leakageConverted->FindBin(aeta) );
      }

    }

    return alpha*varSF;

  }
  
  double egammaEnergyCorrectionTool::getAlphaConvSyst(double cl_eta, double energy, PATCore::ParticleType::Type ptype,
						      egEnergyCorr::Scale::Variation var, double varSF) const {

    double alpha = 0.;
    double aeta = fabs(cl_eta);
    double ET = energy/cosh(cl_eta)/GeV;

    if( var==egEnergyCorr::Scale::Nominal || ptype==PATCore::ParticleType::Electron )
      return alpha;
    
    if( ptype==PATCore::ParticleType::UnconvertedPhoton ) {

      if( var==egEnergyCorr::Scale::ConvEfficiencyUp )
	alpha =  m_convRecoEfficiency->GetBinContent( m_convRecoEfficiency->FindBin(aeta) );
      else if( var==egEnergyCorr::Scale::ConvEfficiencyDown )
	alpha = -m_convRecoEfficiency->GetBinContent( m_convRecoEfficiency->FindBin(aeta) );

    } else if( ptype==PATCore::ParticleType::ConvertedPhoton ) {

      if( var==egEnergyCorr::Scale::ConvFakeRateUp )
	alpha =  m_convFakeRate->GetBinContent( m_convFakeRate->FindBin(aeta) );
      else if( var==egEnergyCorr::Scale::ConvFakeRateDown )
	alpha = -m_convFakeRate->GetBinContent( m_convFakeRate->FindBin(aeta) );
      else if( var==egEnergyCorr::Scale::ConvRadiusUp )
	alpha =  m_convRadius->GetBinContent( m_convRadius->FindBin(aeta, ET) );
      else if( var==egEnergyCorr::Scale::ConvRadiusDown )
	alpha = -m_convRadius->GetBinContent( m_convRadius->FindBin(aeta, ET) );

    }

    return alpha*varSF;

  }
  
  double egammaEnergyCorrectionTool::getAlphaPedestal(double cl_eta, double energy, PATCore::ParticleType::Type ptype, bool isRef, 
						      egEnergyCorr::Scale::Variation var, double varSF) const {

    // observed pedestal corrected as a systematic on MC for now.
    // TODO : correct for it in the data

    double alpha = 0.;
    
    if( var==egEnergyCorr::Scale::PedestalUp || var==egEnergyCorr::Scale::PedestalDown ) {
      
      double pedestal = getLayerPedestal(cl_eta, ptype, 0, var, varSF) +
	                getLayerPedestal(cl_eta, ptype, 1, var, varSF) +
	                getLayerPedestal(cl_eta, ptype, 2, var, varSF) +
	                getLayerPedestal(cl_eta, ptype, 3, var, varSF);

      double eraw = m_calibInputs->E0raw + m_calibInputs->E1raw + m_calibInputs->E2raw + m_calibInputs->E3raw;
      
      if( isRef )
	alpha = pedestal/energy*1.06; // approximate average ratio between calibrated and raw
      else
	alpha = pedestal/eraw;

    }
    
    return alpha*varSF;

  }


  double egammaEnergyCorrectionTool::getLayerPedestal(double cl_eta, PATCore::ParticleType::Type ptype, int iLayer,
			  egEnergyCorr::Scale::Variation var, double varSF) const {

    double pedestal = 0.;
    double aeta = fabs(cl_eta);

    if( var==egEnergyCorr::Scale::PedestalUp || var==egEnergyCorr::Scale::PedestalDown ) {
      
      if( iLayer==0 ) 
	pedestal = m_pedestalL0->GetBinContent( m_pedestalL0->FindBin(aeta) );
      else if( iLayer==1 ) 
	pedestal = m_pedestalL1->GetBinContent( m_pedestalL1->FindBin(aeta) );
      else if( iLayer==2 ) 
	pedestal = m_pedestalL2->GetBinContent( m_pedestalL2->FindBin(aeta) );
      else if( iLayer==3 ) 
	pedestal = m_pedestalL3->GetBinContent( m_pedestalL3->FindBin(aeta) );
      
      if( ptype==PATCore::ParticleType::UnconvertedPhoton && aeta<1.4 ) {
	if( iLayer<=1 )
	  pedestal /= 1.5;
	else if( iLayer==2 )
	  pedestal *= 15./21.;
      } 
      
      if( var==egEnergyCorr::Scale::PedestalDown ) 
	pedestal *= -1.;
      
    }
    
    return pedestal*varSF;
    
  }








  double egammaEnergyCorrectionTool::IntermoduleCorrectionTool( double Ecl, 
								double phi, 
								double eta) {

    double Ecl_corr = 0.;
    double phi_mod = 0.;
    int DivInt = 0;
    double pi = 3.1415926535897932384626433832795 ;
  
    //  Definitions of module folding into four quarters (top, left, bottom and right)

    DivInt =  (int) (phi / ((2 * pi) / 16.));
    phi_mod = phi - DivInt * (2 * pi / 16.);

  
    //  Centring on the intermodule --> phi_mod will now be in [0,0.4]
    if (phi_mod < 0) phi_mod += pi / 8.;

    //  The correction concerns only the barrel 
    if(fabs(eta) <= 1.4){

      //  Top quarter
      if(phi < (3 * pi) / 4. && phi >= pi / 4.){
	Ecl_corr = Ecl / (1-0.131 * ((1 / (1 + exp((phi_mod-0.2) * 199.08))) * (1 / (1 + exp((phi_mod-0.2) * (-130.36))))));
      }	  

      //  Right quarter
      if(phi < pi / 4. && phi >= -pi / 4.){
	Ecl_corr = Ecl / (1-0.0879 * ((1 / (1 + exp((phi_mod-0.2) * 221.01))) * (1 / (1 + exp((phi_mod-0.2) * (-149.51))))));
      }
      //  Bottom quarter
      if(phi < -pi / 4. && phi >= (-3 * pi) / 4.){
	Ecl_corr = Ecl / (1-0.0605 * ((1 / (1 + exp((phi_mod-0.2) * 281.37))) * (1 / (1 + exp((phi_mod-0.2) * (-170.29))))));
      }
      //  Left quarter
      if((phi < (-3 * pi) / 4.) || (phi >= (3 * pi) / 4.)){
	Ecl_corr = Ecl / (1-0.102 * ((1 / (1 + exp((phi_mod-0.2) * 235.37))) * (1 / (1 + exp((phi_mod-0.2) * (-219.04))))));	
      }
    }

    //  No correction for the EC
    else{
      Ecl_corr = Ecl;
    }

    return Ecl_corr;
    
  }



  double egammaEnergyCorrectionTool::CorrectionPhiUnif( double eta, 
							double phi) {

    double PI = 3.141592653589793;
    double Fcorr = 1.0; 
 
    if (eta < -0.4 && eta > -0.6) { 
 
	if (phi < (14 * PI / 32.) && phi > (13 * PI / 32.)) 
	    Fcorr += 0.035; 
	else if (phi < (13 * PI / 32.) && phi > (12 * PI / 32.))
	    Fcorr -= 0.035; 
 
    } 
 
    if (eta < 0.6 && eta > 0.4) { 
 
	if (phi < 0 && phi > (-2 * PI / 32.)) 
	    Fcorr = 1.028; 
	else if (phi < (-4 * 2 * PI / 32.) && phi > (-5 * 2 * PI / 32.))
	    Fcorr = 1.044; 
 
    } 

    else if (eta < 0.8 && eta > 0.6) {

	if (phi < (7 * 2 * PI / 32.) && phi > (6 * 2 * PI / 32.))
	    Fcorr = 1.022;

    }

    else if (eta < 1.4 && eta > 1.2) {

	if (phi < (-11 * 2 * PI / 32.) && phi > (-12 * 2 * PI / 32.))
	    Fcorr = 1.038;

    }

    
    else if (eta < 2.0 && eta > 1.9 ) {

	if (phi < (10 * 2 * PI / 32.) && phi > (9 * 2 * PI / 32.))
	    Fcorr = 1.029;
 
    }

    else if(eta < -1.2 && eta > -1.4) {

	if (phi < (-4 * 2 * PI / 32.) && phi > (-5 * 2 * PI / 32.))
	    Fcorr = 1.048;
	else if (phi < (-6 * 2 * PI / 32.) && phi > (-7 * 2 * PI / 32.))
	    Fcorr = 1.048;

    }
 
 
    else if (eta < -1.6 && eta > -1.8 ) {

	if (phi < (9 * 2 * PI / 32.) && phi > (8 * 2 * PI / 32.))
	    Fcorr = 1.024;

    }

    else if(eta < -2.3 && eta > -2.5) {

	if (phi < (-8 * 2 * PI / 32.) && phi > (-9 * 2 * PI / 32.))
	    Fcorr = 1.037;
	else if (phi < (5 * 2 * PI / 32.) && phi > (4 * 2 * PI / 32.))
	    Fcorr = 1.031;
	else if (phi < (9 * 2 * PI / 32.) && phi > (8 * 2 * PI / 32.))
	    Fcorr = 1.040;
	else if (phi < (10 * 2 * PI / 32.) && phi > (9 * 2 * PI / 32.))
	    Fcorr = 1.030;
	else if (phi < (11 * 2 * PI / 32.) && phi > (10 * 2 * PI / 32.))
	    Fcorr = 1.020;

    }

    return Fcorr;
} 





  bool egammaEnergyCorrectionTool::isInCrack( double cl_eta ) const {
    
    if( fabs(cl_eta)>=1.35 && fabs(cl_eta)<=1.55)
      return true;
    
    return false;

  }
    
  double egammaEnergyCorrectionTool::nearestEtaBEC( double cl_eta ) const {
    
    double newEta = cl_eta;
    
    if( !isInCrack(newEta) )
      return newEta;

    if( newEta>=1.35  && newEta<=1.45) newEta = 1.349;
    if( newEta>=1.45  && newEta<=1.55) newEta = 1.551;

    if( newEta>=-1.55 && newEta<=-1.45) newEta = -1.551;
    if( newEta>=-1.45 && newEta<=-1.35) newEta = -1.349;

    return newEta;

  }


  double egammaEnergyCorrectionTool::pileUpTerm(double eta, int particle_type) const {

    // approximate pileup noise addition to the total noise in MeV   for <mu_data> (2012) = 20
    // converted photons and electrons
    double pileupNoise=240.;
   // unconverted photons, different values in barrel and end-cap
    if (particle_type==1) {
       if (fabs(eta)<1.4) pileupNoise=200.;
    }
    return pileupNoise;

  }
  
  
  void egammaEnergyCorrectionTool::getResolution_systematics(int particle_type, 
							     double energy, 
							     double eta, 
							     int syst_mask, 
							     double& resolution, 
							     double& resolution_error,
                                                             double& resolution_error_up,
                                                             double& resolution_error_down,
							     int resol_type) const {
    
        double pileupNoise =  pileUpTerm(eta, particle_type);
        double et = energy/cosh(eta);
	
	resolution = m_resolution_tool->getResolution(particle_type,energy,eta,resol_type);
        //std::cout << " resolution from tool " << resolution << std::endl;
	double smearingZ=dataConstantTerm(eta);
        double esmearingZ=dataConstantTermError(eta);
	double resolution2=resolution*resolution+smearingZ*smearingZ + (pileupNoise*pileupNoise)/(et*et);
        resolution=sqrt(resolution2);


	double_t sum_sigma_resolution2=0.;
        double sum_deltaDown=0.;
        double sum_deltaUp=0.;


	for (int isys=0;isys<7;isys++) {

	    if (syst_mask & (1<<isys) ) {

                double sigma2=0.;
                double sigma2up=0.;
                double sigma2down=0.;

                // systematics on Z smearing measurement
                if (isys==0) {
                  double d1 = (smearingZ+esmearingZ)*(smearingZ+esmearingZ) - smearingZ*smearingZ;
                  double d2 = smearingZ*smearingZ- (smearingZ-esmearingZ)*(smearingZ-esmearingZ);
                  double d = 0.5*(d1+d2);
                  sigma2up = d1;
                  sigma2down = -d2;
                  sigma2 = d;
                }

                // systematics on intrinsic resolution
                if (isys==1) {
                  double resolutionZ=m_resolution_tool->getResolution(3,40000.*cosh(eta),eta,resol_type);
                  double deltaSigma2 = (1.1*resolutionZ)*(1.1*resolutionZ)-resolutionZ*resolutionZ;
                  double resolution1 = m_resolution_tool->getResolution(3,energy,eta,resol_type);
                  sigma2up = (1.1*resolution1)*(1.1*resolution1)-resolution1*resolution1 - deltaSigma2;
                  deltaSigma2 = (0.9*resolutionZ)*(0.9*resolutionZ)-resolutionZ*resolutionZ;
                  sigma2down= (0.9*resolution1)*(0.9*resolution1)-resolution1*resolution1-deltaSigma2;
                  sigma2 = 0.5*(sigma2up-sigma2down);
                }

                // systematics from configA ID material
                if (isys==2) {
                  double sigmaA= m_getMaterialDelta->getDelta(particle_type,energy,eta,1,resol_type);
                  sigma2 = sigmaA*sigmaA;
                  sigma2up = sigma2;
                  sigma2down = -1.*sigma2;
                }

                // systematics from material presampler-layer 1 in barrel (based on half config M )
                if (isys==3 ) {
                    if( fabs(eta)<1.45) {
                        double sigmaM = m_getMaterialDelta->getDelta(particle_type,energy,eta,1,3);
                        sigma2 = 0.5*sigmaM*sigmaM;
                    } else sigma2=0.;
                    sigma2up = sigma2;
                    sigma2down = -1.*sigma2;
                } 

                // systematic from material in barrel-endcap gap (using full config X for now)
                if (isys==4) {
                  if (fabs(eta)>1.52 && fabs(eta)<1.82) {
                     double sigmaX =  m_getMaterialDelta->getDelta(particle_type,energy,eta,1,3);
                     sigma2 = sigmaX*sigmaX;
                  } else sigma2=0.;
                  sigma2up = sigma2;
                  sigma2down = -1.*sigma2;

                }

                // systematics from material in cryostat area (using half config EL, FIXME: could use clever eta dependent scaling)
                if (isys==5) {
                    double sigmaEL = m_getMaterialDelta->getDelta(particle_type,energy,eta,1,2);
                    sigma2 = 0.5*sigmaEL*sigmaEL;
                    sigma2up = sigma2;
                    sigma2down = -1.*sigma2;
                }

                // systematics from pileup noise  on total noise (200 MeV in quadrature, somewhat conservative)
                if (isys==6) {
                    double et = energy/cosh(eta);
                    double deltaPileupNoise=100.; // MeV
                    if (abs(eta)>=1.4 && abs(eta)<1.8) deltaPileupNoise=200.; // larger systematic in this eta bin
                    double scaleNcells=1;
                    if (particle_type==1 && abs(eta)<1.4) scaleNcells = sqrt(3./5.);   // cluster=3X5 instead of 3x7, rms scales with cluster area
                    double sigmaPileUp = deltaPileupNoise*scaleNcells/et;
                    double sigmaZ = deltaPileupNoise/(40000.); // effect for Z->ee at Et=40 GeV
                    sigma2=sigmaPileUp*sigmaPileUp-sigmaZ*sigmaZ;
                    sigma2up = sigma2;
                    sigma2down = -1.*sigma2;
                }

               double rr1 = sqrt(resolution2+sigma2);
               double rr2=0.;
               if((resolution2-sigma2) > 0.) rr2 = sqrt(resolution2-sigma2);
               double deltaSigma_sys;
               if ((rr1-resolution) > (resolution-rr2) ) deltaSigma_sys = rr1-resolution;
               else deltaSigma_sys = resolution-rr2;
               deltaSigma_sys = deltaSigma_sys / resolution;

               sum_sigma_resolution2 += deltaSigma_sys*deltaSigma_sys;

               if ((resolution2+sigma2up)>0.) rr1=sqrt(resolution2+sigma2up);
               else rr1=0.;
               double deltaSigmaUp = (rr1-resolution)/resolution;
               //std::cout << " relative resolution change Up " << deltaSigmaUp << std::endl;

               if ((resolution2+sigma2down)>0.) rr2=sqrt(resolution2+sigma2down);
               else rr2=0.;
               double deltaSigmaDown = (rr2-resolution)/resolution;

               //std::cout << " relative resolution change Down " << deltaSigmaDown << std::endl;
               sum_deltaUp += deltaSigmaUp;
               sum_deltaDown += deltaSigmaDown;

            }

        }

        resolution = resolution *energy;  // to return final resolution in MeV
        resolution_error = sqrt(sum_sigma_resolution2)*resolution;   // to return resolution uncertainty in MeV

        resolution_error_up = sum_deltaUp*resolution;
        resolution_error_down = sum_deltaDown*resolution;


        //std::cout << " Resolution (MeV): " << resolution << "   Resolution Error (MeV): " << resolution_error
        //  << " Z smearing " << smearingZ << " +- " << esmearingZ << std::endl;

  }

  string egammaEnergyCorrectionTool::variationName(egEnergyCorr::Scale::Variation& var) const {
    
    std::string name = "Unknown";

    switch(var) {
    case egEnergyCorr::Scale::None:
      name = "None";
      break;
    case egEnergyCorr::Scale::Nominal:
      name = "Nominal";
      break;
    case egEnergyCorr::Scale::MomentumUp:
      name = "MomentumUp";
      break;
    case egEnergyCorr::Scale::MomentumDown:
      name = "MomentumDown";
      break;
    case egEnergyCorr::Scale::ZeeStatUp:
      name = "ZeeStatUp";
      break;
    case egEnergyCorr::Scale::ZeeStatDown:
      name = "ZeeStatDown";
      break;
    case egEnergyCorr::Scale::ZeeSystUp:
      name = "ZeeSystUp";
      break;
    case egEnergyCorr::Scale::ZeeSystDown:
      name = "ZeeSystDown";
      break;
    case egEnergyCorr::Scale::ZeeAllUp:
      name = "ZeeAllUp";
      break;
    case egEnergyCorr::Scale::ZeeAllDown:
      name = "ZeeAllDown";
      break;
    case egEnergyCorr::Scale::LArCalibUp:
      name = "LArCalibUp";
      break;
    case egEnergyCorr::Scale::LArCalibDown:
      name = "LArCalibDown";
      break;
    case egEnergyCorr::Scale::LArUnconvCalibUp:
      name = "LArUnconvCalibUp";
      break;
    case egEnergyCorr::Scale::LArUnconvCalibDown:
      name = "LArUnconvCalibDown";
      break;
    case egEnergyCorr::Scale::LArElecCalibUp:
      name = "LArElecCalibUp";
      break;
    case egEnergyCorr::Scale::LArElecCalibDown:
      name = "LArElecCalibDown";
      break;
    case egEnergyCorr::Scale::LArElecUnconvUp:
      name = "LArElecUnconvUp";
      break;
    case egEnergyCorr::Scale::LArElecUnconvDown:
      name = "LArElecUnconvDown";
      break;
    case egEnergyCorr::Scale::G4Up:
      name = "G4Up";
      break;
    case egEnergyCorr::Scale::G4Down:
      name = "G4Down";
      break;
    case egEnergyCorr::Scale::PSUp:
      name = "PSUp";
      break;
    case egEnergyCorr::Scale::PSDown:
      name = "PSDown";
      break;
    case egEnergyCorr::Scale::S12Up:
      name = "S12Up";
      break;
    case egEnergyCorr::Scale::S12Down:
      name = "S12Down";
      break;
    case egEnergyCorr::Scale::MatIDUp:
      name = "MatIDUp";
      break;
    case egEnergyCorr::Scale::MatIDDown:
      name = "MatIDDown";
      break;
    case egEnergyCorr::Scale::MatCryoUp:
      name = "MatCryoUp";
      break;
    case egEnergyCorr::Scale::MatCryoDown:
      name = "MatCryoDown";
      break;
    case egEnergyCorr::Scale::MatCaloUp:
      name = "MatCaloUp";
      break;
    case egEnergyCorr::Scale::MatCaloDown:
      name = "MatCaloDown";
      break;
    case egEnergyCorr::Scale::L1GainUp:
      name = "L1GainUp";
      break;
    case egEnergyCorr::Scale::L1GainDown:
      name = "L1GainDown";
      break;
    case egEnergyCorr::Scale::L2GainUp:
      name = "L2GainUp";
      break;
    case egEnergyCorr::Scale::L2GainDown:
      name = "L2GainDown";
      break;
    case egEnergyCorr::Scale::LeakageUnconvUp:
      name = "LeakageUnconvUp";
      break;
    case egEnergyCorr::Scale::LeakageUnconvDown:
      name = "LeakageUnconvDown";
      break;
    case egEnergyCorr::Scale::LeakageConvUp:
      name = "LeakageConvUp";
      break;
    case egEnergyCorr::Scale::LeakageConvDown:
      name = "LeakageConvDown";
      break;
    case egEnergyCorr::Scale::ConvEfficiencyUp:
      name = "ConvEfficiencyUp";
      break;
    case egEnergyCorr::Scale::ConvEfficiencyDown:
      name = "ConvEfficiencyDown";
      break;
    case egEnergyCorr::Scale::ConvFakeRateUp:
      name = "ConvFakeRateUp";
      break;
    case egEnergyCorr::Scale::ConvFakeRateDown:
      name = "ConvFakeRateDown";
      break;
    case egEnergyCorr::Scale::ConvRadiusUp:
      name = "ConvRadiusUp";
      break;
    case egEnergyCorr::Scale::ConvRadiusDown:
      name = "ConvRadiusDown";
      break;
    case egEnergyCorr::Scale::PedestalUp:
      name = "PedestalUp";
      break;
    case egEnergyCorr::Scale::PedestalDown:
      name = "PedestalDown";
      break;
    case egEnergyCorr::Scale::AllUp:
      name = "AllUp";
      break;
    case egEnergyCorr::Scale::AllDown:
      name = "AllDown";
      break;
    case egEnergyCorr::Scale::LastScaleVariation:
      name = "LastScaleVariation";
      break;
    }

    return name;

  }

  string egammaEnergyCorrectionTool::variationName(egEnergyCorr::Resolution::Variation& var) const {

    std::string name = "Resolution::Unknown";

    switch(var) {
    case egEnergyCorr::Resolution::None:
      name = "Resolution::None";
      break;
    case egEnergyCorr::Resolution::Nominal:
      name = "Resolution::Nominal";
      break;
    case egEnergyCorr::Resolution::AllDown:
      name = "Resolution::AllDown";
      break;
    case egEnergyCorr::Resolution::AllUp:
      name = "Resolution::AllUp";
      break;
    case egEnergyCorr::Resolution::ZSmearingUp:
      name = "Resolution::ZSmearingUp";
      break;
    case egEnergyCorr::Resolution::ZSmearingDown: 
      name = "Resolution::ZSmearingDown";
      break;
    case  egEnergyCorr::Resolution::SamplingTermUp:
      name = "Resolution::SamplingTermUp";
      break;
    case  egEnergyCorr::Resolution::SamplingTermDown:
      name = "Resolution::SamplingTermDown";
      break;
    case  egEnergyCorr::Resolution::MaterialIDUp:
      name = "Resolution::MaterialUp";
      break;
    case  egEnergyCorr::Resolution::MaterialIDDown:
      name = "Resolution::MaterialDown";
      break;
    case  egEnergyCorr::Resolution::MaterialCaloUp:
      name = "Resolution::MaterialUp";
      break;
    case  egEnergyCorr::Resolution::MaterialCaloDown:
      name = "Resolution::MaterialDown";
      break;
    case  egEnergyCorr::Resolution::MaterialCryoUp:
      name = "Resolution::MaterialUp";
      break;
    case  egEnergyCorr::Resolution::MaterialCryoDown:
      name = "Resolution::MaterialDown";
      break;
    case  egEnergyCorr::Resolution::MaterialGapUp:
      name = "Resolution::MaterialUp";
      break;
    case  egEnergyCorr::Resolution::MaterialGapDown:
      name = "Resolution::MaterialDown";
      break;
    case egEnergyCorr::Resolution::PileUpUp:
      name = "Resolution::PileUpUp";
      break;
    case egEnergyCorr::Resolution::PileUpDown:
       name = "Resolution::PileUpDown"; 
       break;
    case egEnergyCorr::Resolution::LastResolutionVariation:
      name = "LastResolutionVariation";
      break;
    }

    return name;

  }





  egEnergyCorr::Scale::Variation egammaEnergyCorrectionTool::ScaleVariationFromString(string& var) {
    
    egEnergyCorr::Scale::Variation TheVar = egEnergyCorr::Scale::None;
    
    if( var == "None" )                        TheVar = egEnergyCorr::Scale::None;
    else if( var == "Nominal" )                TheVar = egEnergyCorr::Scale::Nominal;
    else if( var == "MomentumUp" )             TheVar = egEnergyCorr::Scale::MomentumUp;
    else if( var == "MomentumDown" )           TheVar = egEnergyCorr::Scale::MomentumDown;
    else if( var == "ZeeStatUp" )              TheVar = egEnergyCorr::Scale::ZeeStatUp;
    else if( var == "ZeeStatDown" )            TheVar = egEnergyCorr::Scale::ZeeStatDown;
    else if( var == "ZeeSystUp" )              TheVar = egEnergyCorr::Scale::ZeeSystUp;
    else if( var == "ZeeSystDown" )            TheVar = egEnergyCorr::Scale::ZeeSystDown;
    else if( var == "ZeeAllUp" )               TheVar = egEnergyCorr::Scale::ZeeAllUp;
    else if( var == "ZeeAllDown" )             TheVar = egEnergyCorr::Scale::ZeeAllDown;
    else if( var == "LArCalibUp" )             TheVar = egEnergyCorr::Scale::LArCalibUp;
    else if( var == "LArCalibDown" )           TheVar = egEnergyCorr::Scale::LArCalibDown;
    else if( var == "LArUnconvCalibUp" )       TheVar = egEnergyCorr::Scale::LArUnconvCalibUp;
    else if( var == "LArUnconvCalibDown" )     TheVar = egEnergyCorr::Scale::LArUnconvCalibDown;
    else if( var == "LArElecCalibUp" )         TheVar = egEnergyCorr::Scale::LArElecCalibUp;
    else if( var == "LArElecCalibDown" )       TheVar = egEnergyCorr::Scale::LArElecCalibDown;
    else if( var == "LArElecUnconvUp" )        TheVar = egEnergyCorr::Scale::LArElecUnconvUp;
    else if( var == "LArElecUnconvDown" )      TheVar = egEnergyCorr::Scale::LArElecUnconvDown;
    else if( var == "G4Up" )                   TheVar = egEnergyCorr::Scale::G4Up;
    else if( var == "G4Down" )                 TheVar = egEnergyCorr::Scale::G4Down;
    else if( var == "PSUp" )                   TheVar = egEnergyCorr::Scale::PSUp;
    else if( var == "PSDown" )                 TheVar = egEnergyCorr::Scale::PSDown;
    else if( var == "S12Up" )                  TheVar = egEnergyCorr::Scale::S12Up;
    else if( var == "S12Down" )                TheVar = egEnergyCorr::Scale::S12Down;
    else if( var == "MatIDUp" )                TheVar = egEnergyCorr::Scale::MatIDUp;
    else if( var == "MatIDDown" )              TheVar = egEnergyCorr::Scale::MatIDDown;
    else if( var == "MatCryoUp" )              TheVar = egEnergyCorr::Scale::MatCryoUp;
    else if( var == "MatCryoDown" )            TheVar = egEnergyCorr::Scale::MatCryoDown;
    else if( var == "MatCaloUp" )              TheVar = egEnergyCorr::Scale::MatCaloUp;
    else if( var == "MatCaloDown" )            TheVar = egEnergyCorr::Scale::MatCaloDown;
    else if( var == "L1GainUp" )               TheVar = egEnergyCorr::Scale::L1GainUp;
    else if( var == "L1GainDown" )             TheVar = egEnergyCorr::Scale::L1GainDown;
    else if( var == "L2GainUp" )               TheVar = egEnergyCorr::Scale::L2GainUp;
    else if( var == "L2GainDown" )             TheVar = egEnergyCorr::Scale::L2GainDown;
    else if( var == "LeakageConvUp" )          TheVar = egEnergyCorr::Scale::LeakageConvUp;
    else if( var == "LeakageConvDown" )        TheVar = egEnergyCorr::Scale::LeakageConvDown;
    else if( var == "LeakageUnconvUp" )        TheVar = egEnergyCorr::Scale::LeakageUnconvUp;
    else if( var == "LeakageUnconvDown" )      TheVar = egEnergyCorr::Scale::LeakageUnconvDown;
    else if( var == "ConvEfficiencyUp" )       TheVar = egEnergyCorr::Scale::ConvEfficiencyUp;
    else if( var == "ConvEfficiencyDown" )     TheVar = egEnergyCorr::Scale::ConvEfficiencyDown;
    else if( var == "ConvFakeRateUp" )         TheVar = egEnergyCorr::Scale::ConvFakeRateUp;
    else if( var == "ConvFakeRateDown" )       TheVar = egEnergyCorr::Scale::ConvFakeRateDown;
    else if( var == "ConvRadiusUp" )           TheVar = egEnergyCorr::Scale::ConvRadiusUp;
    else if( var == "ConvRadiusDown" )         TheVar = egEnergyCorr::Scale::ConvRadiusDown;
    else if( var == "PedestalUp" )             TheVar = egEnergyCorr::Scale::PedestalUp;
    else if( var == "PedestalDown" )           TheVar = egEnergyCorr::Scale::PedestalDown;
    else if( var == "AllUp" )                  TheVar = egEnergyCorr::Scale::AllUp;
    else if( var == "AllDown" )                TheVar = egEnergyCorr::Scale::AllDown;
    else if( var == "LastScaleVariation" )     TheVar = egEnergyCorr::Scale::LastScaleVariation;

    return TheVar;

  }


  egEnergyCorr::Resolution::Variation egammaEnergyCorrectionTool::ResolutionVariationFromString(string& var) {
    
    egEnergyCorr::Resolution::Variation TheVar  = egEnergyCorr::Resolution::None;
    
    if ( var == "None") TheVar = egEnergyCorr::Resolution::None;
    else if ( var == "Nominal")                   TheVar = egEnergyCorr::Resolution::Nominal;
    else if ( var == "AllDown")                   TheVar = egEnergyCorr::Resolution::AllDown;
    else if ( var == "AllUp")                     TheVar = egEnergyCorr::Resolution::AllUp;
    else if ( var == "ZSmearingUp")               TheVar = egEnergyCorr::Resolution::ZSmearingUp;
    else if ( var == "ZSmearingDown")             TheVar = egEnergyCorr::Resolution::ZSmearingDown;
    else if ( var == "SamplingTermUp")            TheVar = egEnergyCorr::Resolution::SamplingTermUp;
    else if ( var == "SamplingTermDown")          TheVar = egEnergyCorr::Resolution::SamplingTermDown;
    else if ( var == "MaterialIDUp")              TheVar = egEnergyCorr::Resolution::MaterialIDUp;
    else if ( var == "MaterialIDDown")            TheVar = egEnergyCorr::Resolution::MaterialIDDown;
    else if ( var == "MaterialCaloUp")            TheVar = egEnergyCorr::Resolution::MaterialCaloUp;
    else if ( var == "MaterialCaloDown")          TheVar = egEnergyCorr::Resolution::MaterialCaloDown;
    else if ( var == "MaterialCryoUp")            TheVar = egEnergyCorr::Resolution::MaterialCryoUp;
    else if ( var == "MaterialCryoDown")          TheVar = egEnergyCorr::Resolution::MaterialCryoDown;
    else if ( var == "MaterialGapUp")             TheVar = egEnergyCorr::Resolution::MaterialGapUp;
    else if ( var == "MaterialGapDown")           TheVar = egEnergyCorr::Resolution::MaterialGapDown;
    else if ( var == "PileUpUp")                  TheVar = egEnergyCorr::Resolution::PileUpUp;
    else if ( var == "PileUpDown")                TheVar = egEnergyCorr::Resolution::PileUpDown;
    else if ( var == "LastResolutionVariation")   TheVar = egEnergyCorr::Resolution::LastResolutionVariation;

    return TheVar;

  }





} // egRescaler






