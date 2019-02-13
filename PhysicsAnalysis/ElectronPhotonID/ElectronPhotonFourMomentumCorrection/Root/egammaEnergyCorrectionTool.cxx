/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
///
/// Implementation of energy scale uncertainties, for 2010 data and beyond
///
#include <iostream>
#include <cassert>
#include <exception>
#include <iomanip>
#include <ios>

#include <boost/format.hpp>

#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TFile.h"
#include "TList.h"
#include "TGraphErrors.h"

#include "ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionTool.h"
#include "ElectronPhotonFourMomentumCorrection/GainTool.h"
#include "ElectronPhotonFourMomentumCorrection/eg_resolution.h"
#include "ElectronPhotonFourMomentumCorrection/get_MaterialResolutionEffect.h"
#include "ElectronPhotonFourMomentumCorrection/e1hg_systematics.h"
#include "ElectronPhotonFourMomentumCorrection/GainUncertainty.h"


#include "PathResolver/PathResolver.h"

inline double qsum(double x, double y) { return std::hypot(x, y); }

template<typename T>
T* get_object(TFile& file, const std::string& name)
{
  T* obj = dynamic_cast<T*>(file.Get(name.c_str()));
  if (not obj) { throw std::runtime_error("object " + name + " not found in file " + std::string(file.GetName())); }
  return obj;
}

inline double getValueHistoAt(const TH1& histo, double xvalue,
			      bool use_lastbin_overflow=false,
                              bool use_firstbin_underflow=false)
{
  int bin = histo.FindFixBin(xvalue);
  if (use_lastbin_overflow and histo.IsBinOverflow(bin)) { bin = histo.GetNbinsX(); }
  if (use_firstbin_underflow and histo.IsBinUnderflow(bin)) { bin = 1; }
  return histo.GetBinContent(bin);
}

inline double getValueHistAt(const TH2& histo, double xvalue, double yvalue,
			     bool use_lastbin_x_overflow=false,
                             bool use_lastbin_y_overflow=false,
                             bool use_fistbin_x_underflow=false,
                             bool use_firstbin_y_underflow=false)
{
  int xbin = histo.GetXaxis()->FindFixBin(xvalue);
  if (use_lastbin_x_overflow and xbin == histo.GetXaxis()->GetNbins() + 1) { xbin = histo.GetXaxis()->GetNbins(); }
  if (use_fistbin_x_underflow and xbin == 0) { xbin = 1; }
  int ybin = histo.GetYaxis()->FindFixBin(yvalue);
  if (use_lastbin_y_overflow and ybin == histo.GetYaxis()->GetNbins() + 1) { ybin = histo.GetYaxis()->GetNbins(); }
  if (use_firstbin_y_underflow and ybin == 0) { ybin = 1; }
  return histo.GetBinContent(xbin, ybin);
}

namespace AtlasRoot {

  using std::string;

  egammaEnergyCorrectionTool::egammaEnergyCorrectionTool()
    :
    asg::AsgMessaging("egammaEnergyCorrectionTool"),
    m_esmodel(egEnergyCorr::UNDEFINED)
  {


    m_rootFileName = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v20/egammaEnergyCorrectionData.root");
    
    if (m_rootFileName.empty()) {
      ATH_MSG_FATAL("cannot find configuration file");
      throw std::runtime_error("cannot find file");
    }
    
    m_begRunNumber = 0;
    m_endRunNumber = 0;


    /* 
     * All histogram vectors start empty
     */

    m_use_new_resolution_model = false;

    // tools

    m_resolution_tool = nullptr;
    //m_getMaterialDelta = nullptr;
    m_e1hg_tool = nullptr;

    // switches

    m_use_etaCalo_scales = false;

    m_applyPSCorrection = true;
    m_applyS12Correction = true;

    // special for es2015PRE
    m_use_temp_correction201215 = true;
    m_use_uA2MeV_2015_first2weeks_correction = false;

    // special for es2016PRE
    m_use_temp_correction201516 = true;

    m_use_stat_error_scaling = false;
    m_initialized = false;
  }


  egammaEnergyCorrectionTool::~egammaEnergyCorrectionTool() {

    // Clean up

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

    ATH_MSG_DEBUG("initialize internal tool");

    // Load the ROOT filea

    const std::unique_ptr<char[]> fname(gSystem->ExpandPathName(m_rootFileName.c_str()));
    m_rootFile.reset(TFile::Open( fname.get(), "READ" ));

    if ( !m_rootFile ) {
      ATH_MSG_ERROR("no root file found");
      return 0;
    }

    // instantiate the resolution parametriaton

    //`:if (!m_getMaterialDelta)
    m_getMaterialDelta.reset( new get_MaterialResolutionEffect());


    // Energy corrections and systematic uncertainties
    //////////////////////////////////////////////////


    // Legacy numbers for 2010

    if ( m_esmodel==egEnergyCorr::es2010 ) {
      m_use_new_resolution_model = false;
      m_aPSNom.reset( dynamic_cast< TH1* >( m_rootFile->Get( "Scales/es2010/alphaPS_errTot" ) ) );
      m_aPSNom->SetDirectory(nullptr);
      m_aS12Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2010/alphaS12_errTot")));          m_aS12Nom->SetDirectory(nullptr);

      m_zeeNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2010/alphaZee_errStat")));         m_zeeNom->SetDirectory(nullptr);
      m_zeeSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2010/alphaZee_errSyst")));         m_zeeSyst->SetDirectory(nullptr);

      m_resNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2010/ctZee_errStat")));        m_resNom->SetDirectory(nullptr);
      m_resSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2010/ctZee_errSyst")));        m_resSyst->SetDirectory(nullptr);
      m_peakResData.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2010/resZee_Data")));          m_peakResData->SetDirectory(nullptr);
      m_peakResMC.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2010/resZee_MC")));            m_peakResMC->SetDirectory(nullptr);

      m_begRunNumber = 152166;
      m_endRunNumber = 170482;


      // mc11c : faulty electron multiple scattering in G4; old geometry
      // Precise Z scales, systematics otherwise as in 2010

    } else if ( m_esmodel==egEnergyCorr::es2011c ) {
      m_use_new_resolution_model = false;
      m_aPSNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2011c/alphaPS_errTot")));          m_aPSNom->SetDirectory(nullptr);
      m_aS12Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2011c/alphaS12_errTot")));         m_aS12Nom->SetDirectory(nullptr);

      m_zeeNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2011c/alphaZee_errStat")));        m_zeeNom->SetDirectory(nullptr);
      m_zeeSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2011c/alphaZee_errSyst")));        m_zeeSyst->SetDirectory(nullptr);

      m_resNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2011c/ctZee_errStat")));       m_resNom->SetDirectory(nullptr);
      m_resSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2011c/ctZee_errSyst")));       m_resSyst->SetDirectory(nullptr);
      m_peakResData.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2011c/resZee_Data")));         m_peakResData->SetDirectory(nullptr);
      m_peakResMC.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2011c/resZee_MC")));           m_peakResMC->SetDirectory(nullptr);

      m_begRunNumber = 177531;
      m_endRunNumber = 194382;



      // mc11d : correct MSc in G4; new geometry
      // Final Run1 calibration scheme

    } else if ( m_esmodel==egEnergyCorr::es2011d || m_esmodel==egEnergyCorr::es2011dMedium || m_esmodel==egEnergyCorr::es2011dTight ) {
      m_use_new_resolution_model = true;
      m_resolution_tool.reset(new eg_resolution("run1"));
      m_aPSNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2011d/alphaPS_uncor")));                 m_aPSNom->SetDirectory(nullptr);
      m_daPSCor.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2011d/dalphaPS_cor")));                  m_daPSCor->SetDirectory(nullptr);
      m_aS12Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2011d/alphaS12_uncor")));                m_aS12Nom->SetDirectory(nullptr);
      m_daS12Cor.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2011d/dalphaS12_cor")));                 m_daS12Cor->SetDirectory(nullptr);

      m_trkSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2011d/momentum_errSyst")));              m_trkSyst->SetDirectory(nullptr);

      if( m_esmodel==egEnergyCorr::es2011d ) {

        m_zeeNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2011d/alphaZee_errStat")));              m_zeeNom->SetDirectory(nullptr);
        m_zeeSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2011d/alphaZee_errSyst")));              m_zeeSyst->SetDirectory(nullptr);
        m_resNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2011d/ctZee_errStat")));             m_resNom->SetDirectory(nullptr);
        m_resSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2011d/ctZee_errSyst")));             m_resSyst->SetDirectory(nullptr);

      } else if( m_esmodel==egEnergyCorr::es2011dMedium ) {

        m_zeeNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2011dMedium/alphaZee_errStat")));        m_zeeNom->SetDirectory(nullptr);
        m_zeeSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2011dMedium/alphaZee_errSyst")));        m_zeeSyst->SetDirectory(nullptr);
        m_zeePhys.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2011dMedium/alphaZee_errPhys")));        m_zeePhys->SetDirectory(nullptr);
        m_resNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2011dMedium/ctZee_errStat")));       m_resNom->SetDirectory(nullptr);
        m_resSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2011dMedium/ctZee_errSyst")));       m_resSyst->SetDirectory(nullptr);

      } else if( m_esmodel==egEnergyCorr::es2011dTight ) {

        m_zeeNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2011dTight/alphaZee_errStat")));         m_zeeNom->SetDirectory(nullptr);
        m_zeeSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2011dTight/alphaZee_errSyst")));         m_zeeSyst->SetDirectory(nullptr);
        m_zeePhys.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2011dTight/alphaZee_errPhys")));         m_zeePhys->SetDirectory(nullptr);
        m_resNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2011dTight/ctZee_errStat")));        m_resNom->SetDirectory(nullptr);
        m_resSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2011dTight/ctZee_errSyst")));        m_resSyst->SetDirectory(nullptr);

      }

      m_pedestalL0.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2011d/pedestals_l0")));               m_pedestalL0->SetDirectory(nullptr);
      m_pedestalL1.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2011d/pedestals_l1")));               m_pedestalL1->SetDirectory(nullptr);
      m_pedestalL2.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2011d/pedestals_l2")));               m_pedestalL2->SetDirectory(nullptr);
      m_pedestalL3.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2011d/pedestals_l3")));               m_pedestalL3->SetDirectory(nullptr);

      m_dX_ID_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/DX0_ConfigA")));                         m_dX_ID_Nom->SetDirectory(nullptr);

      m_dX_IPPS_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errUncor")));   m_dX_IPPS_Nom->SetDirectory(nullptr);
      m_dX_IPPS_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errLAr")));     m_dX_IPPS_LAr->SetDirectory(nullptr);

      m_dX_IPAcc_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errUncor")));  m_dX_IPAcc_Nom->SetDirectory(nullptr);
      m_dX_IPAcc_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errLAr")));    m_dX_IPAcc_LAr->SetDirectory(nullptr);
      m_dX_IPAcc_G4.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errG4")));     m_dX_IPAcc_G4->SetDirectory(nullptr);
      m_dX_IPAcc_GL1.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errGL1")));    m_dX_IPAcc_GL1->SetDirectory(nullptr);

      m_dX_PSAcc_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errUncor")));  m_dX_PSAcc_Nom->SetDirectory(nullptr);
      m_dX_PSAcc_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errLAr")));    m_dX_PSAcc_LAr->SetDirectory(nullptr);
      m_dX_PSAcc_G4.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errG4")));     m_dX_PSAcc_G4->SetDirectory(nullptr);

      m_convRadius.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2011d/convRadiusMigrations")));  m_convRadius->SetDirectory(nullptr);
      m_convFakeRate.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2011d/convFakeRate")));          m_convFakeRate->SetDirectory(nullptr);
      m_convRecoEfficiency.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2011d/convRecoEfficiency")));    m_convRecoEfficiency->SetDirectory(nullptr);

      m_begRunNumber = 177531;
      m_endRunNumber = 194382;


      const std::string gain_filename1 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/FunctionsTO.root");
      const std::string gain_filename2 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/FunctionsG_all.root");
      m_gain_tool.reset(new egGain::GainTool(gain_filename1, gain_filename2));


      m_e1hg_tool.reset(new e1hg_systematics());

      // mc12a : crude MSc fix in G4; old geometry
      // All systematics as in 2010.

    } else if ( m_esmodel==egEnergyCorr::es2012a ) {
      m_use_new_resolution_model = false;
      m_aPSNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012a/alphaPS_errTot")));          m_aPSNom->SetDirectory(nullptr);
      m_aS12Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012a/alphaS12_errTot")));         m_aS12Nom->SetDirectory(nullptr);

      m_zeeNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012a/alphaZee_errStat")));        m_zeeNom->SetDirectory(nullptr);
      m_zeeSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012a/alphaZee_errSyst")));        m_zeeSyst->SetDirectory(nullptr);

      m_resNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2012a/ctZee_errStat")));       m_resNom->SetDirectory(nullptr);
      m_resSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2012a/ctZee_errSyst")));       m_resSyst->SetDirectory(nullptr);
      m_peakResData.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2012a/resZee_Data")));         m_peakResData->SetDirectory(nullptr);
      m_peakResMC.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2012a/resZee_MC")));           m_peakResMC->SetDirectory(nullptr);

      m_begRunNumber = 195847;
      m_endRunNumber = 219365;

      // mc12c : correct MSc in G4; new geometry
      // Final Run1 calibration scheme

    } else if (m_esmodel == egEnergyCorr::es2012c) {
      m_use_new_resolution_model = true;
      m_resolution_tool.reset(new eg_resolution("run1"));

      m_aPSNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/alphaPS_uncor")));                 m_aPSNom->SetDirectory(nullptr);
      m_daPSCor.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/dalphaPS_cor")));                  m_daPSCor->SetDirectory(nullptr);
      m_aS12Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/alphaS12_uncor")));                m_aS12Nom->SetDirectory(nullptr);
      m_daS12Cor.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/dalphaS12_cor")));                 m_daS12Cor->SetDirectory(nullptr);

      m_trkSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/momentum_errSyst")));              m_trkSyst->SetDirectory(nullptr);

      m_zeeNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/alphaZee_errStat")));              m_zeeNom->SetDirectory(nullptr);
      m_zeeSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/alphaZee_errSyst")));              m_zeeSyst->SetDirectory(nullptr);

      m_resNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2012c/ctZee_errStat")));             m_resNom->SetDirectory(nullptr);
      m_resSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2012c/ctZee_errSyst")));             m_resSyst->SetDirectory(nullptr);

      m_pedestalL0.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l0")));               m_pedestalL0->SetDirectory(nullptr);
      m_pedestalL1.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l1")));               m_pedestalL1->SetDirectory(nullptr);
      m_pedestalL2.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l2")));               m_pedestalL2->SetDirectory(nullptr);
      m_pedestalL3.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l3")));               m_pedestalL3->SetDirectory(nullptr);

      m_dX_ID_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/DX0_ConfigA")));                         m_dX_ID_Nom->SetDirectory(nullptr);

      m_dX_IPPS_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errUncor")));   m_dX_IPPS_Nom->SetDirectory(nullptr);
      m_dX_IPPS_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errLAr")));     m_dX_IPPS_LAr->SetDirectory(nullptr);

      m_dX_IPAcc_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errUncor")));  m_dX_IPAcc_Nom->SetDirectory(nullptr);
      m_dX_IPAcc_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errLAr")));    m_dX_IPAcc_LAr->SetDirectory(nullptr);
      m_dX_IPAcc_G4.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errG4")));     m_dX_IPAcc_G4->SetDirectory(nullptr);
      m_dX_IPAcc_GL1.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errGL1")));    m_dX_IPAcc_GL1->SetDirectory(nullptr);

      m_dX_PSAcc_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errUncor")));  m_dX_PSAcc_Nom->SetDirectory(nullptr);
      m_dX_PSAcc_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errLAr")));    m_dX_PSAcc_LAr->SetDirectory(nullptr);
      m_dX_PSAcc_G4.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errG4")));     m_dX_PSAcc_G4->SetDirectory(nullptr);

      m_convRadius.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convRadiusMigrations")));  m_convRadius->SetDirectory(nullptr);
      m_convFakeRate.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convFakeRate")));          m_convFakeRate->SetDirectory(nullptr);
      m_convRecoEfficiency.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convRecoEfficiency")));    m_convRecoEfficiency->SetDirectory(nullptr);

      m_begRunNumber = 195847;
      m_endRunNumber = 219365;

      const std::string gain_filename1 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/FunctionsTO.root");
      const std::string gain_filename2 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/FunctionsG_all.root");
      m_gain_tool.reset(new egGain::GainTool(gain_filename1, gain_filename2));

      m_e1hg_tool.reset(new e1hg_systematics());
    }
    else if (m_esmodel == egEnergyCorr::es2012XX) {
      m_use_etaCalo_scales = true;
      m_use_new_resolution_model = true;
      m_resolution_tool.reset(new eg_resolution("run1"));

      m_aPSNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/alphaPS_uncor")));                 m_aPSNom->SetDirectory(nullptr);
      m_daPSCor.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/dalphaPS_cor")));                  m_daPSCor->SetDirectory(nullptr);
      m_aS12Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/alphaS12_uncor")));                m_aS12Nom->SetDirectory(nullptr);
      m_daS12Cor.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/dalphaS12_cor")));                 m_daS12Cor->SetDirectory(nullptr);

      m_trkSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/momentum_errSyst")));              m_trkSyst->SetDirectory(nullptr);

      m_zeeNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015PRE/alphaZee_errStat")));        m_zeeNom->SetDirectory(nullptr);
      m_zeeSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/alphaZee_errSyst")));              m_zeeSyst->SetDirectory(nullptr);

      m_resNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2015PRE/ctZee_errStat")));       m_resNom->SetDirectory(nullptr);
      m_resSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2012c/ctZee_errSyst")));             m_resSyst->SetDirectory(nullptr);

      m_pedestalL0.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l0")));               m_pedestalL0->SetDirectory(nullptr);
      m_pedestalL1.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l1")));               m_pedestalL1->SetDirectory(nullptr);
      m_pedestalL2.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l2")));               m_pedestalL2->SetDirectory(nullptr);
      m_pedestalL3.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l3")));               m_pedestalL3->SetDirectory(nullptr);

      m_dX_ID_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/DX0_ConfigA")));                         m_dX_ID_Nom->SetDirectory(nullptr);

      m_dX_IPPS_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errUncor")));   m_dX_IPPS_Nom->SetDirectory(nullptr);
      m_dX_IPPS_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errLAr")));     m_dX_IPPS_LAr->SetDirectory(nullptr);

      m_dX_IPAcc_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errUncor")));  m_dX_IPAcc_Nom->SetDirectory(nullptr);
      m_dX_IPAcc_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errLAr")));    m_dX_IPAcc_LAr->SetDirectory(nullptr);
      m_dX_IPAcc_G4.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errG4")));     m_dX_IPAcc_G4->SetDirectory(nullptr);
      m_dX_IPAcc_GL1.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errGL1")));    m_dX_IPAcc_GL1->SetDirectory(nullptr);

      m_dX_PSAcc_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errUncor")));  m_dX_PSAcc_Nom->SetDirectory(nullptr);
      m_dX_PSAcc_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errLAr")));    m_dX_PSAcc_LAr->SetDirectory(nullptr);
      m_dX_PSAcc_G4.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errG4")));     m_dX_PSAcc_G4->SetDirectory(nullptr);

      m_convRadius.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convRadiusMigrations")));  m_convRadius->SetDirectory(nullptr);
      m_convFakeRate.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convFakeRate")));          m_convFakeRate->SetDirectory(nullptr);
      m_convRecoEfficiency.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convRecoEfficiency")));    m_convRecoEfficiency->SetDirectory(nullptr);

      m_begRunNumber = 195847;
      m_endRunNumber = 219365;


      const std::string gain_filename1 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/FunctionsTO.root");
      const std::string gain_filename2 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/FunctionsG_all.root");
      m_gain_tool.reset(new egGain::GainTool(gain_filename1, gain_filename2));

      m_e1hg_tool.reset(new e1hg_systematics());
    }
    else if (m_esmodel == egEnergyCorr::es2015PRE or m_esmodel == egEnergyCorr::es2015cPRE) {
      m_use_etaCalo_scales = true;
      m_use_new_resolution_model = true;
      m_resolution_tool.reset( new eg_resolution("run2_pre")) ;

      m_aPSNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/alphaPS_uncor")));                 m_aPSNom->SetDirectory(nullptr);
      m_daPSCor.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/dalphaPS_cor")));                  m_daPSCor->SetDirectory(nullptr);
      m_aS12Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/alphaS12_uncor")));                m_aS12Nom->SetDirectory(nullptr);
      m_daS12Cor.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/dalphaS12_cor")));                 m_daS12Cor->SetDirectory(nullptr);

      m_trkSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/momentum_errSyst")));              m_trkSyst->SetDirectory(nullptr);

      m_zeeNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015PRE/alphaZee_errStat")));        m_zeeNom->SetDirectory(nullptr);
      m_zeeSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015PRE/alphaZee_errSyst")));              m_zeeSyst->SetDirectory(nullptr);
      m_uA2MeV_2015_first2weeks_correction.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015PRE/histo_uA2MeV_week12"))); m_uA2MeV_2015_first2weeks_correction->SetDirectory(nullptr);

      m_resNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2015PRE/ctZee_errStat")));       m_resNom->SetDirectory(nullptr);
      m_resSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2015PRE/ctZee_errSyst")));             m_resSyst->SetDirectory(nullptr);

      m_pedestalL0.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l0")));               m_pedestalL0->SetDirectory(nullptr);
      m_pedestalL1.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l1")));               m_pedestalL1->SetDirectory(nullptr);
      m_pedestalL2.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l2")));               m_pedestalL2->SetDirectory(nullptr);
      m_pedestalL3.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l3")));               m_pedestalL3->SetDirectory(nullptr);

      m_dX_ID_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/DX0_ConfigA")));                         m_dX_ID_Nom->SetDirectory(nullptr);

      m_dX_IPPS_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errUncor")));   m_dX_IPPS_Nom->SetDirectory(nullptr);
      m_dX_IPPS_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errLAr")));     m_dX_IPPS_LAr->SetDirectory(nullptr);

      m_dX_IPAcc_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errUncor")));  m_dX_IPAcc_Nom->SetDirectory(nullptr);
      m_dX_IPAcc_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errLAr")));    m_dX_IPAcc_LAr->SetDirectory(nullptr);
      m_dX_IPAcc_G4.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errG4")));     m_dX_IPAcc_G4->SetDirectory(nullptr);
      m_dX_IPAcc_GL1.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errGL1")));    m_dX_IPAcc_GL1->SetDirectory(nullptr);

      m_dX_PSAcc_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errUncor")));  m_dX_PSAcc_Nom->SetDirectory(nullptr);
      m_dX_PSAcc_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errLAr")));    m_dX_PSAcc_LAr->SetDirectory(nullptr);
      m_dX_PSAcc_G4.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errG4")));     m_dX_PSAcc_G4->SetDirectory(nullptr);

      m_convRadius.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convRadiusMigrations")));  m_convRadius->SetDirectory(nullptr);
      m_convFakeRate.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convFakeRate")));          m_convFakeRate->SetDirectory(nullptr);
      m_convRecoEfficiency.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convRecoEfficiency")));    m_convRecoEfficiency->SetDirectory(nullptr);

      m_begRunNumber = 195847;
      m_endRunNumber = 219365;

      m_G4OverAFII_resolution_electron.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2015/el_full_fast_resolution")));
      m_G4OverAFII_resolution_unconverted.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2015/ph_unconv_full_fast_resolution")));
      m_G4OverAFII_resolution_converted.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2015/ph_conv_full_fast_resolution")));

      assert(m_G4OverAFII_resolution_electron);
      assert(m_G4OverAFII_resolution_unconverted);
      assert(m_G4OverAFII_resolution_converted);

      m_G4OverAFII_resolution_electron->SetDirectory(nullptr);
      m_G4OverAFII_resolution_unconverted->SetDirectory(nullptr);
      m_G4OverAFII_resolution_converted->SetDirectory(nullptr);

      const std::string gain_filename1 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/FunctionsTO.root");
      const std::string gain_filename2 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/FunctionsG_all.root");
      m_gain_tool.reset(new egGain::GainTool(gain_filename1, gain_filename2));

      m_e1hg_tool.reset(new e1hg_systematics());
    }

    else if (m_esmodel == egEnergyCorr::es2015PRE_res_improved or m_esmodel == egEnergyCorr::es2015cPRE_res_improved) {
      m_use_etaCalo_scales = true;
      m_use_new_resolution_model = true;
      m_resolution_tool.reset(new eg_resolution("run2_pre"));

      m_aPSNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/alphaPS_uncor")));                 m_aPSNom->SetDirectory(nullptr);
      m_daPSCor.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/dalphaPS_cor")));                  m_daPSCor->SetDirectory(nullptr);
      m_aS12Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/alphaS12_uncor")));                m_aS12Nom->SetDirectory(nullptr);
      m_daS12Cor.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/dalphaS12_cor")));                 m_daS12Cor->SetDirectory(nullptr);

      m_trkSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/momentum_errSyst")));              m_trkSyst->SetDirectory(nullptr);

      m_zeeNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015PRE/alphaZee_errStat")));        m_zeeNom->SetDirectory(nullptr);
      m_zeeSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015PRE/alphaZee_errSyst")));              m_zeeSyst->SetDirectory(nullptr);
      m_uA2MeV_2015_first2weeks_correction.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015PRE/histo_uA2MeV_week12"))); m_uA2MeV_2015_first2weeks_correction->SetDirectory(nullptr);

      m_resNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2015PRE/ctZee_errStat")));       m_resNom->SetDirectory(nullptr);
      m_resSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2015PRE_res_improved/ctZee_errSyst")));             m_resSyst->SetDirectory(nullptr);

      m_pedestalL0.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l0")));               m_pedestalL0->SetDirectory(nullptr);
      m_pedestalL1.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l1")));               m_pedestalL1->SetDirectory(nullptr);
      m_pedestalL2.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l2")));               m_pedestalL2->SetDirectory(nullptr);
      m_pedestalL3.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l3")));               m_pedestalL3->SetDirectory(nullptr);

      m_dX_ID_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/DX0_ConfigA")));                         m_dX_ID_Nom->SetDirectory(nullptr);

      m_dX_IPPS_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errUncor")));   m_dX_IPPS_Nom->SetDirectory(nullptr);
      m_dX_IPPS_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errLAr")));     m_dX_IPPS_LAr->SetDirectory(nullptr);

      m_dX_IPAcc_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errUncor")));  m_dX_IPAcc_Nom->SetDirectory(nullptr);
      m_dX_IPAcc_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errLAr")));    m_dX_IPAcc_LAr->SetDirectory(nullptr);
      m_dX_IPAcc_G4.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errG4")));     m_dX_IPAcc_G4->SetDirectory(nullptr);
      m_dX_IPAcc_GL1.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errGL1")));    m_dX_IPAcc_GL1->SetDirectory(nullptr);

      m_dX_PSAcc_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errUncor")));  m_dX_PSAcc_Nom->SetDirectory(nullptr);
      m_dX_PSAcc_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errLAr")));    m_dX_PSAcc_LAr->SetDirectory(nullptr);
      m_dX_PSAcc_G4.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errG4")));     m_dX_PSAcc_G4->SetDirectory(nullptr);

      m_convRadius.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convRadiusMigrations")));  m_convRadius->SetDirectory(nullptr);
      m_convFakeRate.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convFakeRate")));          m_convFakeRate->SetDirectory(nullptr);
      m_convRecoEfficiency.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convRecoEfficiency")));    m_convRecoEfficiency->SetDirectory(nullptr);

      m_begRunNumber = 195847;
      m_endRunNumber = 219365;

      m_G4OverAFII_resolution_electron.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2015/el_full_fast_resolution")));
      m_G4OverAFII_resolution_unconverted.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2015/ph_unconv_full_fast_resolution")));
      m_G4OverAFII_resolution_converted.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2015/ph_conv_full_fast_resolution")));

      assert(m_G4OverAFII_resolution_electron);
      assert(m_G4OverAFII_resolution_unconverted);
      assert(m_G4OverAFII_resolution_converted);

      m_G4OverAFII_resolution_electron->SetDirectory(nullptr);
      m_G4OverAFII_resolution_unconverted->SetDirectory(nullptr);
      m_G4OverAFII_resolution_converted->SetDirectory(nullptr);

      const std::string gain_filename1 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/FunctionsTO.root");
      const std::string gain_filename2 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/FunctionsG_all.root");
      m_gain_tool.reset(new egGain::GainTool(gain_filename1, gain_filename2));

      m_e1hg_tool.reset(new e1hg_systematics());
    }

    else if (m_esmodel == egEnergyCorr::es2015c_summer) {
      m_use_etaCalo_scales = true;
      m_use_new_resolution_model = true;
      m_resolution_tool.reset(new eg_resolution("run2_pre"));

      m_aPSNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/alphaPS_uncor")));                 m_aPSNom->SetDirectory(nullptr);
      m_daPSCor.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/dalphaPS_cor")));                  m_daPSCor->SetDirectory(nullptr);
      m_aS12Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/alphaS12_uncor")));                m_aS12Nom->SetDirectory(nullptr);
      m_daS12Cor.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/dalphaS12_cor")));                 m_daS12Cor->SetDirectory(nullptr);

      m_trkSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/momentum_errSyst")));              m_trkSyst->SetDirectory(nullptr);

      m_zeeNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015Summer/alphaZee_errStat")));        m_zeeNom->SetDirectory(nullptr);
      m_zeeSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015Summer/alphaZee_errSyst")));              m_zeeSyst->SetDirectory(nullptr);
      m_uA2MeV_2015_first2weeks_correction = nullptr;

      m_resNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2015Summer/ctZee_errStat")));       m_resNom->SetDirectory(nullptr);
      m_resSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2015Summer/ctZee_errSyst")));       m_resSyst->SetDirectory(nullptr);

      m_pedestalL0.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l0")));               m_pedestalL0->SetDirectory(nullptr);
      m_pedestalL1.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l1")));               m_pedestalL1->SetDirectory(nullptr);
      m_pedestalL2.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l2")));               m_pedestalL2->SetDirectory(nullptr);
      m_pedestalL3.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l3")));               m_pedestalL3->SetDirectory(nullptr);

      m_dX_ID_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/DX0_ConfigA")));                         m_dX_ID_Nom->SetDirectory(nullptr);

      m_dX_IPPS_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errUncor")));   m_dX_IPPS_Nom->SetDirectory(nullptr);
      m_dX_IPPS_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errLAr")));     m_dX_IPPS_LAr->SetDirectory(nullptr);

      m_dX_IPAcc_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errUncor")));  m_dX_IPAcc_Nom->SetDirectory(nullptr);
      m_dX_IPAcc_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errLAr")));    m_dX_IPAcc_LAr->SetDirectory(nullptr);
      m_dX_IPAcc_G4.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errG4")));     m_dX_IPAcc_G4->SetDirectory(nullptr);
      m_dX_IPAcc_GL1.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errGL1")));    m_dX_IPAcc_GL1->SetDirectory(nullptr);

      m_dX_PSAcc_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errUncor")));  m_dX_PSAcc_Nom->SetDirectory(nullptr);
      m_dX_PSAcc_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errLAr")));    m_dX_PSAcc_LAr->SetDirectory(nullptr);
      m_dX_PSAcc_G4.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errG4")));     m_dX_PSAcc_G4->SetDirectory(nullptr);

      m_convRadius.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convRadiusMigrations")));  m_convRadius->SetDirectory(nullptr);
      m_convFakeRate.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convFakeRate")));          m_convFakeRate->SetDirectory(nullptr);
      m_convRecoEfficiency.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convRecoEfficiency")));    m_convRecoEfficiency->SetDirectory(nullptr);

      m_begRunNumber = 195847;
      m_endRunNumber = 219365;

      m_G4OverAFII_resolution_electron.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2015/el_full_fast_resolution")));
      m_G4OverAFII_resolution_unconverted.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2015/ph_unconv_full_fast_resolution")));
      m_G4OverAFII_resolution_converted.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2015/ph_conv_full_fast_resolution")));

      assert(m_G4OverAFII_resolution_electron);
      assert(m_G4OverAFII_resolution_unconverted);
      assert(m_G4OverAFII_resolution_converted);

      m_G4OverAFII_resolution_electron->SetDirectory(nullptr);
      m_G4OverAFII_resolution_unconverted->SetDirectory(nullptr);
      m_G4OverAFII_resolution_converted->SetDirectory(nullptr);

      const std::string gain_filename1 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/FunctionsTO.root");
      const std::string gain_filename2 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/FunctionsG_all.root");
      m_gain_tool.reset(new egGain::GainTool(gain_filename1, gain_filename2));

      m_e1hg_tool.reset( new e1hg_systematics());
      m_use_temp_correction201215 = true;  // for eta > 2.5
      m_use_temp_correction201516 = false;
    }

    else if (m_esmodel == egEnergyCorr::es2016PRE) {
      m_use_etaCalo_scales = true;
      m_use_new_resolution_model = true;
      m_resolution_tool.reset( new eg_resolution("run2_pre"));

      m_aPSNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/alphaPS_uncor")));                 m_aPSNom->SetDirectory(nullptr);
      m_daPSCor.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/dalphaPS_cor")));                  m_daPSCor->SetDirectory(nullptr);
      m_aS12Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/alphaS12_uncor")));                m_aS12Nom->SetDirectory(nullptr);
      m_daS12Cor.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/dalphaS12_cor")));                 m_daS12Cor->SetDirectory(nullptr);

      m_trkSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/momentum_errSyst")));              m_trkSyst->SetDirectory(nullptr);

      m_zeeNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015Summer/alphaZee_errStat")));         m_zeeNom->SetDirectory(nullptr);
      m_zeeSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015Summer/alphaZee_errSyst")));         m_zeeSyst->SetDirectory(nullptr);

      m_resNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2015Summer/ctZee_errStat")));        m_resNom->SetDirectory(nullptr);
      m_resSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2015Summer/ctZee_errSyst")));        m_resSyst->SetDirectory(nullptr);

      m_pedestalL0.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l0")));               m_pedestalL0->SetDirectory(nullptr);
      m_pedestalL1.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l1")));               m_pedestalL1->SetDirectory(nullptr);
      m_pedestalL2.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l2")));               m_pedestalL2->SetDirectory(nullptr);
      m_pedestalL3.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l3")));               m_pedestalL3->SetDirectory(nullptr);

      m_dX_ID_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/DX0_ConfigA")));                         m_dX_ID_Nom->SetDirectory(nullptr);

      m_dX_IPPS_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errUncor")));   m_dX_IPPS_Nom->SetDirectory(nullptr);
      m_dX_IPPS_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errLAr")));     m_dX_IPPS_LAr->SetDirectory(nullptr);

      m_dX_IPAcc_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errUncor")));  m_dX_IPAcc_Nom->SetDirectory(nullptr);
      m_dX_IPAcc_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errLAr")));    m_dX_IPAcc_LAr->SetDirectory(nullptr);
      m_dX_IPAcc_G4.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errG4")));     m_dX_IPAcc_G4->SetDirectory(nullptr);
      m_dX_IPAcc_GL1.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errGL1")));    m_dX_IPAcc_GL1->SetDirectory(nullptr);

      m_dX_PSAcc_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errUncor")));  m_dX_PSAcc_Nom->SetDirectory(nullptr);
      m_dX_PSAcc_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errLAr")));    m_dX_PSAcc_LAr->SetDirectory(nullptr);
      m_dX_PSAcc_G4.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errG4")));     m_dX_PSAcc_G4->SetDirectory(nullptr);

      m_convRadius.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convRadiusMigrations")));  m_convRadius->SetDirectory(nullptr);
      m_convFakeRate.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convFakeRate")));          m_convFakeRate->SetDirectory(nullptr);
      m_convRecoEfficiency.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convRecoEfficiency")));    m_convRecoEfficiency->SetDirectory(nullptr);

      m_begRunNumber = 195847;
      m_endRunNumber = 219365;

      m_G4OverAFII_resolution_electron.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2015/el_full_fast_resolution")));
      m_G4OverAFII_resolution_unconverted.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2015/ph_unconv_full_fast_resolution")));
      m_G4OverAFII_resolution_converted.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2015/ph_conv_full_fast_resolution")));

      assert(m_G4OverAFII_resolution_electron);
      assert(m_G4OverAFII_resolution_unconverted);
      assert(m_G4OverAFII_resolution_converted);

      m_G4OverAFII_resolution_electron->SetDirectory(nullptr);
      m_G4OverAFII_resolution_unconverted->SetDirectory(nullptr);
      m_G4OverAFII_resolution_converted->SetDirectory(nullptr);

      const std::string gain_filename1 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/FunctionsTO.root");
      const std::string gain_filename2 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/FunctionsG_all.root");
      m_gain_tool.reset(new egGain::GainTool(gain_filename1, gain_filename2));

      m_e1hg_tool.reset( new e1hg_systematics());
      m_use_temp_correction201215 = true;  // for eta > 2.5
      m_use_temp_correction201516 = true;
    }

    else if (m_esmodel == egEnergyCorr::es2017 or m_esmodel == egEnergyCorr::es2017_summer or m_esmodel == egEnergyCorr::es2017_summer_improved or m_esmodel == egEnergyCorr::es2017_summer_final or m_esmodel == egEnergyCorr::es2015_5TeV or m_esmodel == egEnergyCorr::es2017_R21_PRE or m_esmodel == egEnergyCorr::es2017_R21_v0 or m_esmodel == egEnergyCorr::es2017_R21_v1) {//add release 21 here for now
      m_use_etaCalo_scales = true;
      m_use_new_resolution_model = true;
      if (m_esmodel == egEnergyCorr::es2017_R21_v1)
	m_resolution_tool.reset(new eg_resolution("run2_R21_v1"));
      else 
	m_resolution_tool.reset(new eg_resolution("run2_pre"));
      
      if(m_esmodel == egEnergyCorr::es2017_summer_final or m_esmodel == egEnergyCorr::es2017_R21_v0 or m_esmodel == egEnergyCorr::es2017_R21_v1){
	m_aPSNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_summer_final/alphaPS_uncor")));       m_aPSNom->SetDirectory(nullptr); 
	m_daPSb12.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_summer_final/dalphaPS_b12")));        m_daPSb12->SetDirectory(nullptr); 
	m_daPSCor.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/dalphaPS_cor")));                    m_daPSCor->SetDirectory(nullptr);
	m_aS12Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_summer_final/alphaS12_uncor")));      m_aS12Nom->SetDirectory(nullptr); 
	m_daS12Cor.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/dalphaS12_cor")));                   m_daS12Cor->SetDirectory(nullptr);
      }
      else{
      m_aPSNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/alphaPS_uncor")));                 m_aPSNom->SetDirectory(nullptr);
      m_daPSCor.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/dalphaPS_cor")));                  m_daPSCor->SetDirectory(nullptr);
      m_aS12Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/alphaS12_uncor")));                m_aS12Nom->SetDirectory(nullptr);
      m_daS12Cor.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/dalphaS12_cor")));                 m_daS12Cor->SetDirectory(nullptr);
      }
      m_trkSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2012c/momentum_errSyst")));              m_trkSyst->SetDirectory(nullptr);
      
      if(m_esmodel == egEnergyCorr::es2017){
      m_zeeNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017/alphaZee_errStat_period_2016")));         m_zeeNom->SetDirectory(nullptr);
      m_zeeNom_data2015.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017/alphaZee_errStat_period_2015")));      m_zeeNom_data2015->SetDirectory(nullptr);
      }
      else if(m_esmodel == egEnergyCorr::es2017_summer or m_esmodel == egEnergyCorr::es2017_summer_improved){
	m_zeeNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_summer/alphaZee_errStat_period_2016")));         m_zeeNom->SetDirectory(nullptr); 
	m_zeeNom_data2015.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_summer/alphaZee_errStat_period_2015")));      m_zeeNom_data2015->SetDirectory(nullptr);
      }
      else if(m_esmodel == egEnergyCorr::es2017_summer_final){ 
        m_zeeNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_summer_final/alphaZee_errStat_period_2016")));         m_zeeNom->SetDirectory(nullptr);  
        m_zeeNom_data2015.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_summer_final/alphaZee_errStat_period_2015")));      m_zeeNom_data2015->SetDirectory(nullptr); 
      }
      else if(m_esmodel == egEnergyCorr::es2015_5TeV){
	m_zeeNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015_5TeV/alphaZee_errStat_period_2015")));         m_zeeNom->SetDirectory(nullptr);   
        //Same histogram added twice for simplicity
        m_zeeNom_data2015.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015_5TeV/alphaZee_errStat_period_2015")));      m_zeeNom_data2015->SetDirectory(nullptr);
      }
      else if (m_esmodel==egEnergyCorr::es2017_R21_v0) {
        m_zeeNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_R21_v0/alphaZee_errStat_period_2017")));  m_zeeNom->SetDirectory(nullptr);
        m_zeeNom_data2016.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_R21_v0/alphaZee_errStat_period_2016")));  m_zeeNom_data2016->SetDirectory(nullptr);
        m_zeeNom_data2015.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_R21_v0/alphaZee_errStat_period_2015")));  m_zeeNom_data2015->SetDirectory(nullptr);
      }
      else if (m_esmodel==egEnergyCorr::es2017_R21_v1) {
        m_zeeNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_R21_v1/alphaZee_errStat_period_2017")));  m_zeeNom->SetDirectory(nullptr);
        m_zeeNom_data2016.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_R21_v1/alphaZee_errStat_period_2016")));  m_zeeNom_data2016->SetDirectory(nullptr);
        m_zeeNom_data2015.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_R21_v1/alphaZee_errStat_period_2015")));  m_zeeNom_data2015->SetDirectory(nullptr);
	m_zeeFwdk.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_R21_v1/alphaFwd_Finalk"))); m_zeeFwdk->SetDirectory(nullptr);
	m_zeeFwdb.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_R21_v1/alphaFwd_Finalb"))); m_zeeFwdb->SetDirectory(nullptr);
      }
      else{
	m_zeeNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_R21_PRE/alphaZee_errStat_period_2016")));         m_zeeNom->SetDirectory(nullptr);  
        //SAME HISTO FOR 2015 FOR NOW
	m_zeeNom_data2015.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_R21_PRE/alphaZee_errStat_period_2016")));      m_zeeNom_data2015->SetDirectory(nullptr);
      }
      if(m_esmodel == egEnergyCorr::es2017){
	m_zeeSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017/alphaZee_errSyst")));         m_zeeSyst->SetDirectory(nullptr);
      }
      else if(m_esmodel == egEnergyCorr::es2017_summer_final){
	m_zeeSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_summer_final/alphaZee_errSyst")));         m_zeeSyst->SetDirectory(nullptr);
      }
      else if(m_esmodel == egEnergyCorr::es2015_5TeV){
	m_zeeSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015_5TeV/alphaZee_errSyst")));         m_zeeSyst->SetDirectory(nullptr);
      }
      else if (m_esmodel == egEnergyCorr::es2017_R21_v0) {
        m_zeeSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_summer_final/alphaZee_errSyst")));         m_zeeSyst->SetDirectory(nullptr);
      }
      else if (m_esmodel == egEnergyCorr::es2017_R21_v1) {
        m_zeeSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_R21_v1/alphaZee_errSyst")));         m_zeeSyst->SetDirectory(nullptr);
      }
      else{
	m_zeeSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2017_summer/alphaZee_errSyst")));         m_zeeSyst->SetDirectory(nullptr);
      }
      
      m_uA2MeV_2015_first2weeks_correction = nullptr;
      if(m_esmodel == egEnergyCorr::es2017){
	m_resNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2017/ctZee_errStat")));        m_resNom->SetDirectory(nullptr);
      }
      else if(m_esmodel == egEnergyCorr::es2017_summer or m_esmodel == egEnergyCorr::es2017_summer_improved or m_esmodel == egEnergyCorr::es2015_5TeV){
	m_resNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2017_summer/ctZee_errStat")));        m_resNom->SetDirectory(nullptr);
      }
      else if(m_esmodel == egEnergyCorr::es2017_summer_final) {
	m_resNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2017_summer_final/ctZee_errStat")));        m_resNom->SetDirectory(nullptr);}
      else if (m_esmodel==egEnergyCorr::es2017_R21_v0) {
         m_resNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2017_R21_v0/ctZee_errStat")));        m_resNom->SetDirectory(nullptr);}
      else if (m_esmodel==egEnergyCorr::es2017_R21_v1) {
	m_resNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2017_R21_v1/ctZee_errStat")));        m_resNom->SetDirectory(nullptr);}
      else{
	m_resNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2017_R21_PRE/ctZee_errStat")));        m_resNom->SetDirectory(nullptr); 
      }
      
      if(m_esmodel == egEnergyCorr::es2017){
	m_resSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2017/ctZee_errSyst")));        m_resSyst->SetDirectory(nullptr);
      }
      else if(m_esmodel == egEnergyCorr::es2017_summer_final){
	m_resSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2017_summer_final/ctZee_errSyst")));        m_resSyst->SetDirectory(nullptr);
      }
      else if(m_esmodel == egEnergyCorr::es2015_5TeV){
	m_resSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2015_5TeV/ctZee_errSyst")));        m_resSyst->SetDirectory(nullptr); 
      }
      else if (m_esmodel == egEnergyCorr::es2017_R21_v0) {
         m_resSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2017_summer_final/ctZee_errSyst")));        m_resSyst->SetDirectory(nullptr);
      }
      else if (m_esmodel == egEnergyCorr::es2017_R21_v1) {
	m_resSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2017_R21_v1/ctZee_errSyst")));        m_resSyst->SetDirectory(nullptr);
      }
      else{ 
	m_resSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2017_summer/ctZee_errSyst")));        m_resSyst->SetDirectory(nullptr);
      }
      //else{
      //m_resSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2017_summer_improved/ctZee_errSyst")));        m_resSyst->SetDirectory(nullptr);
      //}
      
      m_pedestals_es2017.reset( dynamic_cast< TH1* >(m_rootFile->Get("Pedestals/es2017/pedestals")));          m_pedestals_es2017->SetDirectory(nullptr);

      m_dX_ID_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/DX0_ConfigA")));                         m_dX_ID_Nom->SetDirectory(nullptr);

      m_dX_IPPS_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errUncor")));   m_dX_IPPS_Nom->SetDirectory(nullptr);
      m_dX_IPPS_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errLAr")));     m_dX_IPPS_LAr->SetDirectory(nullptr);

      m_dX_IPAcc_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errUncor")));  m_dX_IPAcc_Nom->SetDirectory(nullptr);
      m_dX_IPAcc_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errLAr")));    m_dX_IPAcc_LAr->SetDirectory(nullptr);
      m_dX_IPAcc_G4.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errG4")));     m_dX_IPAcc_G4->SetDirectory(nullptr);
      m_dX_IPAcc_GL1.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errGL1")));    m_dX_IPAcc_GL1->SetDirectory(nullptr);

      m_dX_PSAcc_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errUncor")));  m_dX_PSAcc_Nom->SetDirectory(nullptr);
      m_dX_PSAcc_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errLAr")));    m_dX_PSAcc_LAr->SetDirectory(nullptr);
      m_dX_PSAcc_G4.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errG4")));     m_dX_PSAcc_G4->SetDirectory(nullptr);

      m_convRadius.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convRadiusMigrations")));  m_convRadius->SetDirectory(nullptr);
      if(m_esmodel == egEnergyCorr::es2017){
	m_convFakeRate.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convFakeRate")));          m_convFakeRate->SetDirectory(nullptr);
	m_convRecoEfficiency.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convRecoEfficiency")));    m_convRecoEfficiency->SetDirectory(nullptr);
      }
      else{
	m_convFakeRate.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2017_summer/convFakeRate")));          m_convFakeRate->SetDirectory(nullptr); 
        m_convRecoEfficiency.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2017_summer/convRecoEfficiency")));    m_convRecoEfficiency->SetDirectory(nullptr);
      }
      
      // TODO: change path when moving to calibarea
      // TODO: better package this somewhere

      const std::string filename_pp0 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/PP0sys.root");
      
      TFile file_pp0(filename_pp0.c_str());
      m_pp0_elec.reset( dynamic_cast< TH2* >( file_pp0.Get("elec")));
      m_pp0_conv.reset( dynamic_cast< TH2* >( file_pp0.Get("conv")));  
      m_pp0_unconv.reset( dynamic_cast< TH2* >( file_pp0.Get("unco"))); 

      m_pp0_elec->SetDirectory(nullptr);
      m_pp0_unconv->SetDirectory(nullptr);
      m_pp0_conv->SetDirectory(nullptr);

      //similar case for wtots1
      const std::string filename_wstot = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/wstot_related_syst.root");
      

      TFile file_wstot(filename_wstot.c_str());
      m_wstot_slope_A_data.reset(  dynamic_cast< TH1* >( file_wstot.Get("A_data"))); m_wstot_slope_A_data->SetDirectory(nullptr);
      m_wstot_slope_B_MC.reset(  dynamic_cast< TH1* >( file_wstot.Get("B_mc"))); m_wstot_slope_B_MC->SetDirectory(nullptr);
      m_wstot_40GeV_data.reset(  dynamic_cast< TH1* >( file_wstot.Get("wstot40_data"))); m_wstot_40GeV_data->SetDirectory(nullptr);
      m_wstot_40GeV_MC.reset(  dynamic_cast< TH1* >( file_wstot.Get("wstot40_mc"))); m_wstot_40GeV_MC->SetDirectory(nullptr);
      m_wstot_pT_data_p0_electrons.reset(  dynamic_cast< TH1* >( file_wstot.Get("wstot_pT_el_data_p0"))); m_wstot_pT_data_p0_electrons->SetDirectory(nullptr);
      m_wstot_pT_data_p1_electrons.reset(  dynamic_cast< TH1* >( file_wstot.Get("wstot_pT_el_data_p1"))); m_wstot_pT_data_p1_electrons->SetDirectory(nullptr);
      m_wstot_pT_data_p0_unconverted_photons.reset(  dynamic_cast< TH1* >( file_wstot.Get("wstot_pT_uc_data_p0"))); m_wstot_pT_data_p0_unconverted_photons->SetDirectory(nullptr);
      m_wstot_pT_data_p1_unconverted_photons.reset(  dynamic_cast< TH1* >( file_wstot.Get("wstot_pT_uc_data_p1"))); m_wstot_pT_data_p1_unconverted_photons->SetDirectory(nullptr);
      m_wstot_pT_data_p0_converted_photons.reset(  dynamic_cast< TH1* >( file_wstot.Get("wstot_pT_c_data_p0"))); m_wstot_pT_data_p0_converted_photons->SetDirectory(nullptr);
      m_wstot_pT_data_p1_converted_photons.reset(  dynamic_cast< TH1* >( file_wstot.Get("wstot_pT_c_data_p1"))); m_wstot_pT_data_p1_converted_photons->SetDirectory(nullptr);
      m_wstot_pT_MC_p0_electrons.reset(  dynamic_cast< TH1* >( file_wstot.Get("wstot_pT_el_mc_p0"))); m_wstot_pT_MC_p0_electrons->SetDirectory(nullptr);
      m_wstot_pT_MC_p1_electrons.reset(  dynamic_cast< TH1* >( file_wstot.Get("wstot_pT_el_mc_p1"))); m_wstot_pT_MC_p1_electrons->SetDirectory(nullptr);
      m_wstot_pT_MC_p0_unconverted_photons.reset(  dynamic_cast< TH1* >( file_wstot.Get("wstot_pT_ph_uc_mc_p0"))); m_wstot_pT_MC_p0_unconverted_photons->SetDirectory(nullptr);
      m_wstot_pT_MC_p1_unconverted_photons.reset(  dynamic_cast< TH1* >( file_wstot.Get("wstot_pT_ph_uc_mc_p1"))); m_wstot_pT_MC_p1_unconverted_photons->SetDirectory(nullptr);
      m_wstot_pT_MC_p0_converted_photons.reset(  dynamic_cast< TH1* >( file_wstot.Get("wstot_pT_ph_c_mc_p0"))); m_wstot_pT_MC_p0_converted_photons->SetDirectory(nullptr);
      m_wstot_pT_MC_p1_converted_photons.reset(  dynamic_cast< TH1* >( file_wstot.Get("wstot_pT_ph_c_mc_p1"))); m_wstot_pT_MC_p1_converted_photons->SetDirectory(nullptr);

      m_begRunNumber = 252604;
      m_endRunNumber = 314199;

      if (m_esmodel == egEnergyCorr::es2017_R21_v1) {
         m_G4OverAFII_resolution_electron.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2017_v1/resol_Af2ToG4_elec_rel21")));
         m_G4OverAFII_resolution_unconverted.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2017_v1/resol_Af2ToG4_unco_rel21")));
         m_G4OverAFII_resolution_converted.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2017_v1/resol_Af2ToG4_conv_rel21")));
      }
      else {
        m_G4OverAFII_resolution_electron.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2017/el_full_fast_resolution")));
        m_G4OverAFII_resolution_unconverted.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2017/ph_unconv_full_fast_resolution")));
        m_G4OverAFII_resolution_converted.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2017/ph_conv_full_fast_resolution")));
      }
      assert(m_G4OverAFII_resolution_electron);
      assert(m_G4OverAFII_resolution_unconverted);
      assert(m_G4OverAFII_resolution_converted);

      m_G4OverAFII_resolution_electron->SetDirectory(nullptr);
      m_G4OverAFII_resolution_unconverted->SetDirectory(nullptr);
      m_G4OverAFII_resolution_converted->SetDirectory(nullptr);

      const std::string gain_filename1 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/FunctionsTO.root");
      const std::string gain_filename2 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/FunctionsG_all.root");
      m_gain_tool = nullptr;

      std::string gain_tool_run_2_filename;
      if(m_esmodel == egEnergyCorr::es2017 or m_esmodel == egEnergyCorr::es2017_summer or m_esmodel == egEnergyCorr::es2017_R21_PRE or m_esmodel == egEnergyCorr::es2015_5TeV){
	gain_tool_run_2_filename = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v11/gain_uncertainty_specialRun.root"); 
      }
      else{
	gain_tool_run_2_filename = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v14/gain_uncertainty_specialRun.root");
      }
      m_gain_tool_run2.reset( new egGain::GainUncertainty(gain_tool_run_2_filename));
      
      m_e1hg_tool.reset( new e1hg_systematics());
      m_use_temp_correction201215 = false;
      m_use_temp_correction201516 = false;
    }

    else if ( m_esmodel==egEnergyCorr::es2015_day0_3percent ) {
      m_use_new_resolution_model = true;
      m_resolution_tool.reset(new eg_resolution("run2_pre"));

      m_aPSNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015_day0/alphaPS_uncor")));             m_aPSNom->SetDirectory(nullptr);         // old one
      m_daPSCor.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015_day0/dalphaPS_cor")));              m_daPSCor->SetDirectory(nullptr);        // old one
      m_aS12Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015_day0/alphaS12_uncor")));            m_aS12Nom->SetDirectory(nullptr);        // old one
      m_daS12Cor.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015_day0/dalphaS12_cor")));             m_daS12Cor->SetDirectory(nullptr);       // old one

      m_trkSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015_day0/momentum_errSyst")));          m_trkSyst->SetDirectory(nullptr);        // old one

      m_zeeNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015_day0/alphaZee_errStat")));          m_zeeNom->SetDirectory(nullptr);         // old one
      m_zeeSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Scales/es2015_day0/alphaZee_errSyst")));          m_zeeSyst->SetDirectory(nullptr);        // old one

      m_resNom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2012c/ctZee_errStat")));             m_resNom->SetDirectory(nullptr);         // old one
      m_resSyst.reset( dynamic_cast< TH1* >( m_rootFile->Get("Resolution/es2012c/ctZee_errSyst")));             m_resSyst->SetDirectory(nullptr);        // old one

      m_pedestalL0.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l0")));               m_pedestalL0->SetDirectory(nullptr);     // old one
      m_pedestalL1.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l1")));               m_pedestalL1->SetDirectory(nullptr);     // old one
      m_pedestalL2.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l2")));               m_pedestalL2->SetDirectory(nullptr);     // old one
      m_pedestalL3.reset( dynamic_cast< TH1* >( m_rootFile->Get("Pedestals/es2012c/pedestals_l3")));               m_pedestalL3->SetDirectory(nullptr);     // old one

      m_dX_ID_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/DX0_ConfigA")));                         m_dX_ID_Nom->SetDirectory(nullptr);      // old one

      m_dX_IPPS_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errUncor")));   m_dX_IPPS_Nom->SetDirectory(nullptr);    // old one
      m_dX_IPPS_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPPS_NewG_errLAr")));     m_dX_IPPS_LAr->SetDirectory(nullptr);    // old one

      m_dX_IPAcc_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errUncor")));  m_dX_IPAcc_Nom->SetDirectory(nullptr);   // old one
      m_dX_IPAcc_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errLAr")));    m_dX_IPAcc_LAr->SetDirectory(nullptr);   // old one
      m_dX_IPAcc_G4.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errG4")));     m_dX_IPAcc_G4->SetDirectory(nullptr);    // old one
      m_dX_IPAcc_GL1.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_IPAcc_NewG_errGL1")));    m_dX_IPAcc_GL1->SetDirectory(nullptr);  // old one

      m_dX_PSAcc_Nom.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errUncor")));  m_dX_PSAcc_Nom->SetDirectory(nullptr);   // old one
      m_dX_PSAcc_LAr.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errLAr")));    m_dX_PSAcc_LAr->SetDirectory(nullptr);   // old one
      m_dX_PSAcc_G4.reset( dynamic_cast< TH1* >( m_rootFile->Get("Material/Measured/DXerr_PSAcc_NewG_errG4")));     m_dX_PSAcc_G4->SetDirectory(nullptr);    // old one

      m_convRadius.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convRadiusMigrations")));  m_convRadius->SetDirectory(nullptr);          // old one
      m_convFakeRate.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convFakeRate")));          m_convFakeRate->SetDirectory(nullptr);        // old one
      m_convRecoEfficiency.reset( dynamic_cast< TH1* >( m_rootFile->Get("Conversions/es2012c/convRecoEfficiency")));    m_convRecoEfficiency->SetDirectory(nullptr);  // old one

      m_begRunNumber = 195847;
      m_endRunNumber = 219365;

      const std::string gain_filename1 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/FunctionsTO.root");
      const std::string gain_filename2 = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/FunctionsG_all.root");
      m_gain_tool.reset( new egGain::GainTool(gain_filename1, gain_filename2));

      m_e1hg_tool.reset( new e1hg_systematics());


      // If we are here, fail      :

    } else if ( m_esmodel == egEnergyCorr::UNDEFINED) {
      ATH_MSG_FATAL("ES model not initialized - Initialization fails");
      return 0;
    } else {
      ATH_MSG_FATAL("ES model not recognized - Initialization fails");
      return 0;

    }

    if (m_esmodel == egEnergyCorr::es2015cPRE or m_esmodel == egEnergyCorr::es2015cPRE_res_improved or
        m_esmodel == egEnergyCorr::es2015c_summer or m_esmodel == egEnergyCorr::es2016PRE or m_esmodel == egEnergyCorr::es2017
	or m_esmodel == egEnergyCorr::es2017_summer or m_esmodel == egEnergyCorr::es2017_summer_improved or m_esmodel == egEnergyCorr::es2017_summer_final
        or m_esmodel == egEnergyCorr::es2017_R21_v0 or m_esmodel == egEnergyCorr::es2017_R21_v1 
	or m_esmodel == egEnergyCorr::es2017_R21_PRE or m_esmodel == egEnergyCorr::es2015_5TeV)
    {
      // E4 systematics
      m_E4ElectronEtaBins.reset( dynamic_cast< TAxis* >( m_rootFile->Get("E4Recalibration/v4/electron_eta_axis")));
      m_E4ElectronGraphs.reset( dynamic_cast< TList* >( m_rootFile->Get("E4Recalibration/v4/electron")));m_E4ElectronGraphs->SetOwner();
      // for photons use the same as electrons
      m_E4UnconvertedEtaBins.reset( dynamic_cast< TAxis* >( m_rootFile->Get("E4Recalibration/v4/electron_eta_axis")));
      m_E4UnconvertedGraphs.reset( dynamic_cast< TList* >( m_rootFile->Get("E4Recalibration/v4/electron")));m_E4UnconvertedGraphs->SetOwner();
      m_E4ConvertedEtaBins.reset( dynamic_cast< TAxis* >( m_rootFile->Get("E4Recalibration/v4/electron_eta_axis")));
      m_E4ConvertedGraphs.reset( dynamic_cast< TList* >( m_rootFile->Get("E4Recalibration/v4/electron")));m_E4ConvertedGraphs->SetOwner();
    }

    // ... PS and S12 recalibration curves
    if (m_esmodel == egEnergyCorr::es2015PRE or m_esmodel == egEnergyCorr::es2015PRE_res_improved or
        m_esmodel == egEnergyCorr::es2015cPRE or m_esmodel == egEnergyCorr::es2015cPRE_res_improved or
        m_esmodel == egEnergyCorr::es2015c_summer or m_esmodel == egEnergyCorr::es2016PRE or m_esmodel == egEnergyCorr::es2017
	or m_esmodel == egEnergyCorr::es2017_summer or m_esmodel == egEnergyCorr::es2017_summer_improved or m_esmodel == egEnergyCorr::es2017_summer_final
        or m_esmodel == egEnergyCorr::es2017_R21_v0 or m_esmodel == egEnergyCorr::es2017_R21_v1 
	or m_esmodel == egEnergyCorr::es2017_R21_PRE or m_esmodel == egEnergyCorr::es2015_5TeV) {
      
      m_psElectronEtaBins.reset( dynamic_cast< TAxis* >( m_rootFile->Get("PSRecalibration/es2015PRE/ElectronAxis")));
      m_psElectronGraphs.reset( dynamic_cast< TList* >( m_rootFile->Get("PSRecalibration/es2015PRE/ElectronBiasPS")));m_psElectronGraphs->SetOwner();
      m_psUnconvertedEtaBins.reset( dynamic_cast< TAxis* >( m_rootFile->Get("PSRecalibration/es2015PRE/UnconvertedAxis")));
      m_psUnconvertedGraphs.reset( dynamic_cast< TList* >( m_rootFile->Get("PSRecalibration/es2015PRE/UnconvertedBiasPS")));m_psUnconvertedGraphs->SetOwner();
      m_psConvertedEtaBins.reset( dynamic_cast< TAxis* >( m_rootFile->Get("PSRecalibration/es2015PRE/ConvertedAxis")));
      m_psConvertedGraphs.reset( dynamic_cast< TList* >( m_rootFile->Get("PSRecalibration/es2015PRE/ConvertedBiasPS")));m_psConvertedGraphs->SetOwner();

      m_s12ElectronEtaBins.reset( dynamic_cast< TAxis* >( m_rootFile->Get("S1Recalibration/es2015PRE/ElectronAxis")));
      m_s12ElectronGraphs.reset( dynamic_cast< TList* >( m_rootFile->Get("S1Recalibration/es2015PRE/ElectronBiasS1")));m_s12ElectronGraphs->SetOwner();
      m_s12UnconvertedEtaBins.reset( dynamic_cast< TAxis* >( m_rootFile->Get("S1Recalibration/es2015PRE/UnconvertedAxis")));
      m_s12UnconvertedGraphs.reset( dynamic_cast< TList* >( m_rootFile->Get("S1Recalibration/es2015PRE/UnconvertedBiasS1")));m_s12UnconvertedGraphs->SetOwner();
      m_s12ConvertedEtaBins.reset( dynamic_cast< TAxis* >( m_rootFile->Get("S1Recalibration/es2015PRE/ConvertedAxis")));
      m_s12ConvertedGraphs.reset( dynamic_cast< TList* >( m_rootFile->Get("S1Recalibration/es2015PRE/ConvertedBiasS1")));m_s12ConvertedGraphs->SetOwner();
    }
    else // run1
    {
      m_psElectronEtaBins.reset( dynamic_cast< TAxis* >( m_rootFile->Get("PSRecalibration/ElectronAxis")));
      m_psElectronGraphs.reset( dynamic_cast< TList* >( m_rootFile->Get("PSRecalibration/ElectronBiasPS")));m_psElectronGraphs->SetOwner();
      m_psUnconvertedEtaBins.reset( dynamic_cast< TAxis* >( m_rootFile->Get("PSRecalibration/UnconvertedAxis")));
      m_psUnconvertedGraphs.reset( dynamic_cast< TList* >( m_rootFile->Get("PSRecalibration/UnconvertedBiasPS")));m_psUnconvertedGraphs->SetOwner();
      m_psConvertedEtaBins.reset( dynamic_cast< TAxis* >( m_rootFile->Get("PSRecalibration/ConvertedAxis")));
      m_psConvertedGraphs.reset( dynamic_cast< TList* >( m_rootFile->Get("PSRecalibration/ConvertedBiasPS")));m_psConvertedGraphs->SetOwner();

      m_s12ElectronEtaBins.reset( dynamic_cast< TAxis* >( m_rootFile->Get("S1Recalibration/ElectronAxis")));
      m_s12ElectronGraphs.reset( dynamic_cast< TList* >( m_rootFile->Get("S1Recalibration/ElectronBiasS1")));m_s12ElectronGraphs->SetOwner();
      m_s12UnconvertedEtaBins.reset( dynamic_cast< TAxis* >( m_rootFile->Get("S1Recalibration/UnconvertedAxis")));
      m_s12UnconvertedGraphs.reset( dynamic_cast< TList* >( m_rootFile->Get("S1Recalibration/UnconvertedBiasS1")));m_s12UnconvertedGraphs->SetOwner();
      m_s12ConvertedEtaBins.reset( dynamic_cast< TAxis* >( m_rootFile->Get("S1Recalibration/ConvertedAxis")));
      m_s12ConvertedGraphs.reset( dynamic_cast< TList* >( m_rootFile->Get("S1Recalibration/ConvertedBiasS1")));m_s12ConvertedGraphs->SetOwner();
    }

    // further inputs do not depend on year

    // ... material distortions

    m_matUnconvertedScale.emplace_back(std::move(std::unique_ptr<TH1>( (TH1*) m_rootFile->Get("Material/unconvertedBiasSubtracted_ConfigA") )));
    m_matUnconvertedScale.back()->SetDirectory(nullptr);
    m_matUnconvertedScale.emplace_back(std::move(std::unique_ptr<TH1>( (TH1*) m_rootFile->Get("Material/unconvertedBiasSubtracted_ConfigCpDp") )));
    m_matUnconvertedScale.back()->SetDirectory(nullptr);
    m_matUnconvertedScale.emplace_back(std::move(std::unique_ptr<TH1>( (TH1*) m_rootFile->Get("Material/unconvertedBiasSubtracted_ConfigEpLp") )));
    m_matUnconvertedScale.back()->SetDirectory(nullptr);
    m_matUnconvertedScale.emplace_back(std::move(std::unique_ptr<TH1>( (TH1*) m_rootFile->Get("Material/unconvertedBiasSubtracted_ConfigFpMX") )));
    m_matUnconvertedScale.back()->SetDirectory(nullptr);
    m_matUnconvertedScale.emplace_back(std::move(std::unique_ptr<TH1>( (TH1*) m_rootFile->Get("Material/unconvertedBiasSubtracted_ConfigGp") )));
    m_matUnconvertedScale.back()->SetDirectory(nullptr);
  
    m_matConvertedScale.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material/convertedBiasSubtracted_ConfigA") )));
    m_matConvertedScale.back()->SetDirectory(nullptr);
    m_matConvertedScale.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material/convertedBiasSubtracted_ConfigCpDp") )));
    m_matConvertedScale.back()->SetDirectory(nullptr);
    m_matConvertedScale.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material/convertedBiasSubtracted_ConfigEpLp") )));
    m_matConvertedScale.back()->SetDirectory(nullptr);
    m_matConvertedScale.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material/convertedBiasSubtracted_ConfigFpMX") )));
    m_matConvertedScale.back()->SetDirectory(nullptr);
    m_matConvertedScale.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material/convertedBiasSubtracted_ConfigGp") )));
    m_matConvertedScale.back()->SetDirectory(nullptr);
    
    m_matElectronCstTerm.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material/electronCstTerm_ConfigA") )));
    m_matElectronCstTerm.back()->SetDirectory(nullptr);
    m_matElectronCstTerm.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material/electronCstTerm_ConfigCpDp") )));
    m_matElectronCstTerm.back()->SetDirectory(nullptr);
    m_matElectronCstTerm.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material/electronCstTerm_ConfigEpLp") )));
    m_matElectronCstTerm.back()->SetDirectory(nullptr);
    m_matElectronCstTerm.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material/electronCstTerm_ConfigFpMX") )));
    m_matElectronCstTerm.back()->SetDirectory(nullptr);
    m_matElectronCstTerm.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material/electronCstTerm_ConfigGp") )));
    m_matElectronCstTerm.back()->SetDirectory(nullptr);
    
    if (m_esmodel==egEnergyCorr::es2017_R21_v1) {
     // update dX0 plots for distorted geometry for case A, EL, FMX and N
      m_matX0Additions.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material_rel21/DX0_ConfigA") ))); 
      m_matX0Additions.back()->SetDirectory(nullptr);
      m_matX0Additions.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material/DX0_ConfigCpDp") )));
      m_matX0Additions.back()->SetDirectory(nullptr);
      m_matX0Additions.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material_rel21/DX0_ConfigEpLp") )));
      m_matX0Additions.back()->SetDirectory(nullptr);
      m_matX0Additions.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material_rel21/DX0_ConfigFpMX") )));
      m_matX0Additions.back()->SetDirectory(nullptr);
      m_matX0Additions.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material/DX0_ConfigGp") )));
      m_matX0Additions.back()->SetDirectory(nullptr);
      m_matX0Additions.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material_rel21/DX0_ConfigN") )));
      m_matX0Additions.back()->SetDirectory(nullptr);
    }
    else {
      m_matX0Additions.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material/DX0_ConfigA") )));
      m_matX0Additions.back()->SetDirectory(nullptr);
      m_matX0Additions.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material/DX0_ConfigCpDp") )));
      m_matX0Additions.back()->SetDirectory(nullptr);
      m_matX0Additions.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material/DX0_ConfigEpLp") )));
      m_matX0Additions.back()->SetDirectory(nullptr);
      m_matX0Additions.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material/DX0_ConfigFpMX") )));
      m_matX0Additions.back()->SetDirectory(nullptr);
      m_matX0Additions.emplace_back(std::move(std::unique_ptr<TH1>(  (TH1*) m_rootFile->Get("Material/DX0_ConfigGp") )));
      m_matX0Additions.back()->SetDirectory(nullptr);
    }

    
    m_matElectronEtaBins.reset( dynamic_cast<TAxis*>(m_rootFile->Get("Material/LinearityEtaBins")));
    m_matElectronGraphs.emplace_back( std::move(std::unique_ptr<TList>( (TList*) m_rootFile->Get("Material/Linearity_Cluster_ConfigA")) ));
    m_matElectronGraphs.emplace_back( std::move(std::unique_ptr<TList>( (TList*) m_rootFile->Get("Material/Linearity_Cluster_ConfigCpDp")) ));
    m_matElectronGraphs.emplace_back( std::move(std::unique_ptr<TList>( (TList*) m_rootFile->Get("Material/Linearity_Cluster_ConfigEpLp")) ));
    m_matElectronGraphs.emplace_back( std::move(std::unique_ptr<TList>( (TList*) m_rootFile->Get("Material/Linearity_Cluster_ConfigFpMX")) ));
    m_matElectronGraphs.emplace_back( std::move(std::unique_ptr<TList>( (TList*) m_rootFile->Get("Material/Linearity_Cluster_ConfigGp")) ));


    for (auto &mat: m_matElectronGraphs) {
      mat->SetOwner();
    }


    // ... new material distortions from release 21 parameterizations

    if (m_esmodel==egEnergyCorr::es2017_R21_v1) {
      m_electronBias_ConfigA.reset( dynamic_cast< TH2* >( m_rootFile->Get("Material_rel21/electronBias_ConfigA")));            m_electronBias_ConfigA->SetDirectory(nullptr);
      m_electronBias_ConfigEpLp.reset( dynamic_cast< TH2* >( m_rootFile->Get("Material_rel21/electronBias_ConfigEpLp")));      m_electronBias_ConfigEpLp->SetDirectory(nullptr);
      m_electronBias_ConfigFpMX.reset( dynamic_cast< TH2* >( m_rootFile->Get("Material_rel21/electronBias_ConfigFpMX")));      m_electronBias_ConfigFpMX->SetDirectory(nullptr);
      m_electronBias_ConfigN.reset( dynamic_cast< TH2* >( m_rootFile->Get("Material_rel21/electronBias_ConfigN")));            m_electronBias_ConfigN->SetDirectory(nullptr);
      m_electronBias_ConfigIBL.reset( dynamic_cast< TH2* >( m_rootFile->Get("Material_rel21/electronBias_ConfigIBL")));        m_electronBias_ConfigIBL->SetDirectory(nullptr);
      m_electronBias_ConfigPP0.reset( dynamic_cast< TH2* >( m_rootFile->Get("Material_rel21/electronBias_ConfigPP0")));        m_electronBias_ConfigPP0->SetDirectory(nullptr);
      m_unconvertedBias_ConfigA.reset( dynamic_cast< TH2* >( m_rootFile->Get("Material_rel21/unconvertedBias_ConfigA")));      m_unconvertedBias_ConfigA->SetDirectory(nullptr);
      m_unconvertedBias_ConfigEpLp.reset( dynamic_cast< TH2* >( m_rootFile->Get("Material_rel21/unconvertedBias_ConfigEpLp"))); m_unconvertedBias_ConfigEpLp->SetDirectory(nullptr);
      m_unconvertedBias_ConfigFpMX.reset( dynamic_cast< TH2* >( m_rootFile->Get("Material_rel21/unconvertedBias_ConfigFpMX")));m_unconvertedBias_ConfigFpMX->SetDirectory(nullptr);
      m_unconvertedBias_ConfigN.reset( dynamic_cast< TH2* >( m_rootFile->Get("Material_rel21/unconvertedBias_ConfigN")));      m_unconvertedBias_ConfigN->SetDirectory(nullptr);
      m_unconvertedBias_ConfigIBL.reset( dynamic_cast< TH2* >( m_rootFile->Get("Material_rel21/unconvertedBias_ConfigIBL")));  m_unconvertedBias_ConfigIBL->SetDirectory(nullptr);
      m_unconvertedBias_ConfigPP0.reset( dynamic_cast< TH2* >( m_rootFile->Get("Material_rel21/unconvertedBias_ConfigPP0")));  m_unconvertedBias_ConfigPP0->SetDirectory(nullptr);
      m_convertedBias_ConfigA.reset( dynamic_cast< TH2* >( m_rootFile->Get("Material_rel21/convertedBias_ConfigA")));          m_convertedBias_ConfigA->SetDirectory(nullptr);
      m_convertedBias_ConfigEpLp.reset( dynamic_cast< TH2* >( m_rootFile->Get("Material_rel21/convertedBias_ConfigEpLp")));    m_convertedBias_ConfigEpLp->SetDirectory(nullptr);
      m_convertedBias_ConfigFpMX.reset( dynamic_cast< TH2* >( m_rootFile->Get("Material_rel21/convertedBias_ConfigFpMX")));    m_convertedBias_ConfigFpMX->SetDirectory(nullptr);
      m_convertedBias_ConfigN.reset( dynamic_cast< TH2* >( m_rootFile->Get("Material_rel21/convertedBias_ConfigN")));          m_convertedBias_ConfigN->SetDirectory(nullptr);
      m_convertedBias_ConfigIBL.reset( dynamic_cast< TH2* >( m_rootFile->Get("Material_rel21/convertedBias_ConfigIBL")));      m_convertedBias_ConfigIBL->SetDirectory(nullptr);
      m_convertedBias_ConfigPP0.reset( dynamic_cast< TH2* >( m_rootFile->Get("Material_rel21/convertedBias_ConfigPP0")));      m_convertedBias_ConfigPP0->SetDirectory(nullptr);
    }

       

    // ... Fastsim to Fullsim corrections

    if (m_esmodel == egEnergyCorr::es2015PRE or m_esmodel == egEnergyCorr::es2015PRE_res_improved or
        m_esmodel == egEnergyCorr::es2015cPRE or m_esmodel == egEnergyCorr::es2015cPRE_res_improved or
        m_esmodel == egEnergyCorr::es2015c_summer or m_esmodel == egEnergyCorr::es2016PRE) {

      m_G4OverAFII_electron.reset( dynamic_cast< TH1* >( m_rootFile->Get("FastSim/es2015/el_scale_full_fast_peak_gaussian"))); m_G4OverAFII_electron->SetDirectory(nullptr);
      m_G4OverAFII_unconverted.reset( dynamic_cast< TH1* >( m_rootFile->Get("FastSim/es2015/ph_unconv_scale_full_fast_peak_gaussian"))); m_G4OverAFII_unconverted->SetDirectory(nullptr);
      m_G4OverAFII_converted.reset( dynamic_cast< TH1* >( m_rootFile->Get("FastSim/es2015/ph_conv_scale_full_fast_peak_gaussian"))); m_G4OverAFII_converted->SetDirectory(nullptr);
    }
    else if (m_esmodel == egEnergyCorr::es2017 or m_esmodel == egEnergyCorr::es2017_summer 
             or m_esmodel == egEnergyCorr::es2017_summer_improved or m_esmodel == egEnergyCorr::es2017_summer_final 
             or m_esmodel == egEnergyCorr::es2017_R21_PRE or m_esmodel == egEnergyCorr::es2015_5TeV 
             or m_esmodel == egEnergyCorr::es2017_R21_v0 ) {
      m_G4OverAFII_electron.reset( dynamic_cast< TH1* >( m_rootFile->Get("FastSim/es2017/el_scale_full_fast_peak_gaussian"))); m_G4OverAFII_electron->SetDirectory(nullptr);
      m_G4OverAFII_unconverted.reset( dynamic_cast< TH1* >( m_rootFile->Get("FastSim/es2017/ph_unconv_scale_full_fast_peak_gaussian"))); m_G4OverAFII_unconverted->SetDirectory(nullptr);
      m_G4OverAFII_converted.reset( dynamic_cast< TH1* >( m_rootFile->Get("FastSim/es2017/ph_conv_scale_full_fast_peak_gaussian"))); m_G4OverAFII_converted->SetDirectory(nullptr);
    }
    else if (m_esmodel == egEnergyCorr::es2017_R21_v1) {
      m_G4OverAFII_electron_2D.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2017_v1/scale_Af2ToG4_elec_rel21"))); m_G4OverAFII_electron_2D->SetDirectory(nullptr);
      m_G4OverAFII_unconverted_2D.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2017_v1/scale_Af2ToG4_unco_rel21"))); m_G4OverAFII_unconverted_2D->SetDirectory(nullptr);
      m_G4OverAFII_converted_2D.reset( dynamic_cast< TH2* >( m_rootFile->Get("FastSim/es2017_v1/scale_Af2ToG4_conv_rel21"))); m_G4OverAFII_converted_2D->SetDirectory(nullptr);

    }
    else { // run 1
      m_G4OverAFII_electron.reset( dynamic_cast< TH1* >( m_rootFile->Get("FastSim/hG4OverAF")));                 m_G4OverAFII_electron->SetDirectory(nullptr);
    }
    
    m_G4OverFrSh.reset( dynamic_cast< TH1* >( m_rootFile->Get("FastSim/hG4OverFS")));                 m_G4OverFrSh->SetDirectory(nullptr);

    // ... Leakage systematics
    
    if(m_esmodel != egEnergyCorr::es2017_summer and m_esmodel != egEnergyCorr::es2017_summer_improved 
       and m_esmodel != egEnergyCorr::es2017_summer_final and m_esmodel != egEnergyCorr::es2017_R21_PRE 
       and m_esmodel != egEnergyCorr::es2015_5TeV and m_esmodel != egEnergyCorr::es2017_R21_v0 
       and m_esmodel != egEnergyCorr::es2017_R21_v1){
      m_leakageConverted.reset( dynamic_cast< TH1* >( m_rootFile->Get("Leakage/LeakageDiffConverted")));      m_leakageConverted->SetDirectory(nullptr);
      m_leakageUnconverted.reset( dynamic_cast< TH1* >( m_rootFile->Get("Leakage/LeakageDiffUnconverted")));    m_leakageUnconverted->SetDirectory(nullptr);
    }
    else{
      m_leakageConverted.reset( dynamic_cast< TH1* >( m_rootFile->Get("Leakage/es2017_summer/LeakageDiffConverted")));      m_leakageConverted->SetDirectory(nullptr);
      m_leakageUnconverted.reset( dynamic_cast< TH1* >( m_rootFile->Get("Leakage/es2017_summer/LeakageDiffUnconverted")));    m_leakageUnconverted->SetDirectory(nullptr);
    }
    
    // ... Zee S2 profile (needed for gain switch syst).

    m_zeeES2Profile.reset( dynamic_cast< TH1* >( m_rootFile->Get("ZeeEnergyProfiles/p2MC")));    m_zeeES2Profile->SetDirectory(nullptr);

    // OK, now we are all initialized and everything went fine

    m_initialized = true;
    return 1;

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
      if( scaleVar==egEnergyCorr::Scale::MomentumUp )  corr = m_trkSyst->GetBinContent( m_trkSyst->FindBin(aeta) );
      else if( scaleVar==egEnergyCorr::Scale::MomentumDown )corr = -m_trkSyst->GetBinContent( m_trkSyst->FindBin(aeta) );

      correctedMomentum *= 1. + corr*varSF;
    }

    return correctedMomentum;
  }


  // This method handles the main switches between data and the various MC flavours.
  // Called internally by getCorrectedEnergy(...)
  double egammaEnergyCorrectionTool::getCorrectedEnergy( unsigned int runnumber,
                                                         PATCore::ParticleDataType::DataType dataType,
                                                         PATCore::ParticleType::Type ptype,
                                                         double cl_eta,
                                                         double cl_etaCalo,
                                                         double energy,
                                                         double energyS2,
                                                         double eraw,
							 egEnergyCorr::Scale::Variation scaleVar,
                                                         egEnergyCorr::Resolution::Variation resVar,
                                                         egEnergyCorr::Resolution::resolutionType resType,
                                                         double varSF ) const {
    double fullyCorrectedEnergy = energy;

    // Correct fast sim flavours

    if ( dataType == PATCore::ParticleDataType::FastShower ) // Frozen shower sim
      fullyCorrectedEnergy = energy * this->applyFStoG4(cl_eta);
    else if ( dataType == PATCore::ParticleDataType::Fast ) // AtlFast2 sim
    {
      fullyCorrectedEnergy = energy * this->applyAFtoG4(cl_eta, 0.001*energy/cosh(cl_eta),ptype);
    }

    // If nothing is to be done

    if ( scaleVar == egEnergyCorr::Scale::None && resVar == egEnergyCorr::Resolution::None )
      return fullyCorrectedEnergy;

    ATH_MSG_DEBUG("after sim fl = " << boost::format("%.2f") % fullyCorrectedEnergy);

    // main E-scale corrections

    if ( dataType == PATCore::ParticleDataType::Data ) { // ... Data

      if( scaleVar == egEnergyCorr::Scale::Nominal ) {
        double alpha = getAlphaValue(runnumber, cl_eta, cl_etaCalo, fullyCorrectedEnergy, energyS2, eraw, ptype, scaleVar, varSF);
	fullyCorrectedEnergy /= (1 + alpha);
        // apply additional k.E+b corrections if histograms exist (like in es2017_R21_v1)
	if (m_zeeFwdk &&  m_zeeFwdb && fabs(cl_eta)>2.5){ //  calo eta?
	  int ieta_k = m_zeeFwdk->GetXaxis()->FindBin(cl_eta);
	  double value_k = m_zeeFwdk->GetBinContent(ieta_k);
	  int ieta_b = m_zeeFwdb->GetXaxis()->FindBin(cl_eta);
          double value_b = m_zeeFwdb->GetBinContent(ieta_b);
	  fullyCorrectedEnergy = value_k*fullyCorrectedEnergy + value_b*GeV;   // value is stored in GeV in the histogram file
	}
        ATH_MSG_DEBUG("after alpha = " << boost::format("%.2f") % fullyCorrectedEnergy);
      }

    } else { // ... MC

      // Do the energy scale correction (for systematic variations)

      if ( scaleVar != egEnergyCorr::Scale::None && scaleVar != egEnergyCorr::Scale::Nominal ) {
        double deltaAlpha = getAlphaUncertainty(runnumber, cl_eta, cl_etaCalo, fullyCorrectedEnergy, energyS2, eraw, ptype, scaleVar, varSF );
        ATH_MSG_DEBUG("alpha sys " << variationName(scaleVar) << " = " << deltaAlpha);
        fullyCorrectedEnergy *= ( 1 + deltaAlpha );
        ATH_MSG_DEBUG("after mc alpha = " << boost::format("%.2f") % fullyCorrectedEnergy);
      }

       // AF2 systematics  (this will not be in the sum of all other NP in the 1 NP model)
      if (dataType == PATCore::ParticleDataType::Fast && (m_esmodel== egEnergyCorr::es2017_R21_v0 || m_esmodel== egEnergyCorr::es2017_R21_v1) ) {
        if (scaleVar==egEnergyCorr::Scale::af2Up or scaleVar==egEnergyCorr::Scale::af2Down) {
           double daAF2=0.;
           if (m_esmodel== egEnergyCorr::es2017_R21_v0) {
              if (scaleVar==egEnergyCorr::Scale::af2Up) daAF2 = 0.005;
              if (scaleVar==egEnergyCorr::Scale::af2Down) daAF2 = -0.005;
           }
           if (m_esmodel== egEnergyCorr::es2017_R21_v1) {
              if (scaleVar==egEnergyCorr::Scale::af2Up) daAF2 = 0.001;
              if (scaleVar==egEnergyCorr::Scale::af2Down) daAF2 = -0.001;
           }
           fullyCorrectedEnergy *= ( 1 + daAF2);
        }
      }


      // Do the resolution correction
      if ( resVar != egEnergyCorr::Resolution::None )
      fullyCorrectedEnergy *= getSmearingCorrection(cl_eta, cl_etaCalo, fullyCorrectedEnergy, ptype, dataType, resVar, resType);

      ATH_MSG_DEBUG("after resolution correction = " << boost::format("%.2f") % fullyCorrectedEnergy);
    }

    return fullyCorrectedEnergy;
  }


  // This method applied the overall energy corrections and systematic variations.
  // Called internally by getCorrectedEnergy(...).
  // convention is Edata = (1+alpha)*Emc, hence Edata -> Edata/(1+alpha) to match the MC
  // note : all energies in MeV

  // returns alpha_var. var = egEnergyCorr::Scale::Nominal or any systematic variation

  double egammaEnergyCorrectionTool::getAlphaValue(
						   long int runnumber,
						   double cl_eta,
						   double cl_etaCalo,
						   double energy,      // input energy (not ET!!)
						   double energyS2,    // raw energy in S2
						   double eraw,
						   PATCore::ParticleType::Type ptype,
						   egEnergyCorr::Scale::Variation var,
						   double varSF ) const {

    double meanE = getZeeMeanET(cl_eta)*cosh(cl_eta);

    // Main Scale factor

    double alphaZee = getAlphaZee(runnumber, m_use_etaCalo_scales ? cl_etaCalo : cl_eta, var, varSF );


    // Sampling recalibration

    double daPS, daS12, linPS, linS12;
    daPS = daS12 = linPS = linS12 = 0.;

    double daE4 = 0., linE4 = 0.;
    // E4 contribution
    if ((m_esmodel == egEnergyCorr::es2015c_summer or m_esmodel == egEnergyCorr::es2016PRE or m_esmodel == egEnergyCorr::es2017 or m_esmodel == egEnergyCorr::es2017_summer or m_esmodel == egEnergyCorr::es2017_summer_improved or m_esmodel == egEnergyCorr::es2017_summer_final or m_esmodel == egEnergyCorr::es2017_R21_PRE or m_esmodel == egEnergyCorr::es2015_5TeV or m_esmodel == egEnergyCorr::es2017_R21_v0 or m_esmodel == egEnergyCorr::es2017_R21_v1 ) and
        (var == egEnergyCorr::Scale::E4ScintillatorUp or var == egEnergyCorr::Scale::E4ScintillatorDown))
    {
      daE4 = getE4Uncertainty(cl_eta);
      if (var == egEnergyCorr::Scale::E4ScintillatorDown) daE4 *= -1;
      linE4 = getE4NonLinearity(cl_eta, energy, ptype) - getE4NonLinearity(cl_eta, meanE, PATCore::ParticleType::Electron);
    }

    //wtots1 contribution
    double daWtots1 = 0.;
    if ((m_esmodel==egEnergyCorr::es2017 or m_esmodel == egEnergyCorr::es2017_summer or m_esmodel == egEnergyCorr::es2017_summer_improved or m_esmodel == egEnergyCorr::es2017_summer_final or m_esmodel == egEnergyCorr::es2017_R21_PRE or m_esmodel == egEnergyCorr::es2015_5TeV or m_esmodel == egEnergyCorr::es2017_R21_v0 or m_esmodel == egEnergyCorr::es2017_R21_v1) and (var == egEnergyCorr::Scale::Wtots1Up or var == egEnergyCorr::Scale::Wtots1Down)){
      daWtots1 = getWtots1Uncertainty(cl_eta, energy, ptype);
      if(var == egEnergyCorr::Scale::Wtots1Down)
	daWtots1 = -daWtots1;
    }

    // ... Presampler contribution

    if( var==egEnergyCorr::Scale::PSUp || var==egEnergyCorr::Scale::PSDown ||
	var==egEnergyCorr::Scale::PSb12Up || var==egEnergyCorr::Scale::PSb12Down ||
	var==egEnergyCorr::Scale::LArElecUnconvUp || var==egEnergyCorr::Scale::LArElecUnconvDown ) {

      daPS  = getLayerUncertainty(  0, cl_eta, var, varSF );
      linPS = getLayerNonLinearity( 0, cl_eta, energy, ptype )
	- getLayerNonLinearity( 0, cl_eta, meanE,  PATCore::ParticleType::Electron );

    }

    // ... S1 / S2 contribution

    if (var == egEnergyCorr::Scale::S12Up or var==egEnergyCorr::Scale::S12Down or
        var == egEnergyCorr::Scale::LArCalibUp or var==egEnergyCorr::Scale::LArCalibDown or
        var == egEnergyCorr::Scale::LArCalibExtra2015PreUp or var == egEnergyCorr::Scale::LArCalibExtra2015PreDown or
        var == egEnergyCorr::Scale::S12ExtraLastEtaBinRun2Up or var == egEnergyCorr::Scale::S12ExtraLastEtaBinRun2Down)
      {
        daS12  = getLayerUncertainty(1, cl_eta, var, varSF);
	linS12 = getLayerNonLinearity( 1, cl_eta, energy, ptype ) - getLayerNonLinearity( 1, cl_eta, meanE,  PATCore::ParticleType::Electron );
      }

    // Material contribution

    double daMatID, daMatCryo, daMatCalo;
    daMatID = daMatCryo = daMatCalo = 0;

    // for release 21 sensitivity use the same getMaterialNonLinearity for all particles
    // while in sensitivities derived from run 1 this is only used for electrons

    if( ptype!=PATCore::ParticleType::Electron  && m_esmodel != egEnergyCorr::es2017_R21_v1) {

      daMatID   = getAlphaMaterial( cl_eta, egEnergyCorr::MatID,   ptype, var, varSF );
      daMatCryo = getAlphaMaterial( cl_eta, egEnergyCorr::MatCryo, ptype, var, varSF );
      daMatCalo = getAlphaMaterial( cl_eta, egEnergyCorr::MatCalo, ptype, var, varSF );

    } else {

      daMatID   = getMaterialNonLinearity( cl_eta, energy, egEnergyCorr::MatID,   ptype, var, varSF )
	- getMaterialNonLinearity( cl_eta, meanE,  egEnergyCorr::MatID,   PATCore::ParticleType::Electron, var, varSF );
      daMatCryo = getMaterialNonLinearity( cl_eta, energy, egEnergyCorr::MatCryo, ptype, var, varSF )
	- getMaterialNonLinearity( cl_eta, meanE,  egEnergyCorr::MatCryo, PATCore::ParticleType::Electron, var, varSF );
      daMatCalo = getMaterialNonLinearity( cl_eta, energy, egEnergyCorr::MatCalo, ptype, var, varSF )
	- getMaterialNonLinearity( cl_eta, meanE,  egEnergyCorr::MatCalo, PATCore::ParticleType::Electron, var, varSF );

    }

    // Pedestal subtraction

    double daPedestal = getAlphaPedestal(cl_eta, energy, eraw, ptype, false, var, varSF)
      - getAlphaPedestal(cl_eta, meanE, eraw, PATCore::ParticleType::Electron , true, var, varSF);

    // double pedestal systematics for 2016, Guillaume 12/05/16
    if (m_esmodel == egEnergyCorr::es2016PRE) {
       daPedestal *= 2;
    }

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

    if ( var==egEnergyCorr::Scale::L2GainUp || var==egEnergyCorr::Scale::L2GainDown ) {
      if (m_gain_tool) { // recipe for run1
        if (!(fabs(cl_eta) < 1.52 and fabs(cl_eta) > 1.37) and fabs(cl_eta) < 2.4) {
  	      double evar = m_gain_tool->CorrectionGainTool(cl_eta, energy/GeV, energyS2/GeV, ptype);
  	      double meanES2 = m_zeeES2Profile->GetBinContent(m_zeeES2Profile->FindBin(cl_eta)); // in GeV already
  	      double eref = m_gain_tool->CorrectionGainTool(cl_eta, meanE/GeV, meanES2, PATCore::ParticleType::Electron );
  	      daL2GainSwitch = evar/energy - eref/meanE;
  	      if( var==egEnergyCorr::Scale::L2GainDown )
  	        daL2GainSwitch = -daL2GainSwitch;
        }
      }
      else if (m_gain_tool_run2) { // recipe for run 2, see ATLASEG-44
        daL2GainSwitch = m_gain_tool_run2->getUncertainty(cl_etaCalo, energy / cosh(cl_eta), ptype);
        if (var == egEnergyCorr::Scale::L2GainDown) daL2GainSwitch *= -1;
      }
      else {
        ATH_MSG_ERROR("trying to compute gain systematic, but no tool for doing it has been instantiated, setting sys to 0");
        daL2GainSwitch = 0.;
      }
    }

    // pp0 (and IBL)
    double dapp0 = 0.;
    // values from the histogram already are 0 for the Z->ee electrons
    if (var == egEnergyCorr::Scale::MatPP0Up or var == egEnergyCorr::Scale::MatPP0Down) {

// new parameterization for release 21 reconstruction with mc16 geometries + distortions
      if (m_esmodel == egEnergyCorr::es2017_R21_v1) {

          if (fabs(cl_eta)<1.5)
           dapp0 = getMaterialEffect(egEnergyCorr::ConfigIBL,ptype ,cl_eta,energy / GeV / cosh(cl_eta))
                  -getMaterialEffect(egEnergyCorr::ConfigIBL,PATCore::ParticleType::Electron, cl_eta,getZeeMeanET(cl_eta)/GeV);
          else 
           dapp0 = getMaterialEffect(egEnergyCorr::ConfigPP0,ptype ,cl_eta,energy / GeV / cosh(cl_eta))
                  -getMaterialEffect(egEnergyCorr::ConfigPP0,PATCore::ParticleType::Electron, cl_eta,getZeeMeanET(cl_eta)/GeV);

          if (var == egEnergyCorr::Scale::MatPP0Down) { dapp0 = -dapp0; }
      }

      // release 20 run 2 systematics for mc15 like geometries
      else {
        //Just pick the owned one from a unique_ptr per case
        TH2* histo = nullptr;
        if (ptype == PATCore::ParticleType::Electron && m_pp0_elec) histo = dynamic_cast<TH2*>(m_pp0_elec.get());
        else if (ptype == PATCore::ParticleType::ConvertedPhoton && m_pp0_conv ) histo = dynamic_cast<TH2*>(m_pp0_conv.get());
        else if (ptype == PATCore::ParticleType::UnconvertedPhoton && m_pp0_unconv) histo = dynamic_cast<TH2*>(m_pp0_unconv.get());

        if (histo) {
          const double aeta = std::abs(cl_eta);
          dapp0 = getValueHistAt(*histo, aeta, energy / GeV / cosh(cl_eta), false, true, false, true);
          if (var == egEnergyCorr::Scale::MatPP0Down) { dapp0 = -dapp0; }

          // normalize to pp0 systematics
          if (aeta > 1.5 and aeta < 2.0) { dapp0 *= 2.6; }
          else if (aeta >= 2.0 and aeta <= 2.5) { dapp0 *= 2.3; }
        }
     }
    }

    // Conversion systematics

    double daConvSyst = getAlphaConvSyst(cl_eta, energy, ptype, var, varSF);

    // topo cluster threshold systematics for release 21
    double daTopoCluster=0;
    if ((var==egEnergyCorr::Scale::topoClusterThresUp || var==egEnergyCorr::Scale::topoClusterThresDown) && (m_esmodel== egEnergyCorr::es2017_R21_v0 || m_esmodel== egEnergyCorr::es2017_R21_v1)) {
       double Et = energy/cosh(cl_eta);
       double Et0=10000.;
       //  Effect taken as 10**-3/(Et/10GeV) - order of magniture from https://indico.cern.ch/event/669895/contributions/2745266/attachments/1535612/2405452/slides.pdf
       if (var==egEnergyCorr::Scale::topoClusterThresUp)      daTopoCluster = 1e-3*(1./(Et/Et0)-1./(getZeeMeanET(cl_eta)/Et0));
       if (var==egEnergyCorr::Scale::topoClusterThresUp)      daTopoCluster = -1e-3*(1./(Et/Et0)-1./(getZeeMeanET(cl_eta)/Et0));
    }

    // Total

    double alphaTot = alphaZee;
    alphaTot += daE4 * linE4;
    alphaTot += daPS*linPS;
    alphaTot += daS12*linS12;
    alphaTot += daMatID + daMatCryo + daMatCalo;
    alphaTot += daLeakage;
    alphaTot += daL1GainSwitch;
    alphaTot += daL2GainSwitch;
    alphaTot += daConvSyst;
    alphaTot += daPedestal;
    alphaTot += daWtots1;
    alphaTot += dapp0;
    alphaTot += daTopoCluster;

    ATH_MSG_DEBUG("alpha value for " << variationName(var) << " = " << alphaTot);

    return alphaTot;

  }


  // returns alpha_var - alpha_nom, for systematic variations.

  double egammaEnergyCorrectionTool::getAlphaUncertainty(long int runnumber,
							 double cl_eta,
							 double cl_etaCalo,
							 double energy,
							 double energyS2,
							 double eraw,
							 PATCore::ParticleType::Type ptype,
							 egEnergyCorr::Scale::Variation var, double varSF ) const {

    double alphaNom = getAlphaValue(runnumber, cl_eta, cl_etaCalo, energy, energyS2, eraw, ptype, egEnergyCorr::Scale::Nominal );
    double alphaVar = 0.;

    if (var != egEnergyCorr::Scale::AllUp && var != egEnergyCorr::Scale::AllDown and
        var != egEnergyCorr::Scale::AllCorrelatedUp and var != egEnergyCorr::Scale::AllCorrelatedDown)  {
      // not an ALLUP
      alphaVar = getAlphaValue(runnumber, cl_eta, cl_etaCalo, energy, energyS2, eraw, ptype, var, varSF ) - alphaNom;
    }
    else if (var == egEnergyCorr::Scale::AllUp) {
      for (egEnergyCorr::Scale::Variation ivar = egEnergyCorr::Scale::ZeeStatUp;
           ivar < egEnergyCorr::Scale::AllUp;
           ivar = egEnergyCorr::Scale::Variation(ivar + 2)) {
        if (ivar == egEnergyCorr::Scale::ZeeAllUp) continue;
        const double v = getAlphaValue(runnumber, cl_eta, cl_etaCalo, energy, energyS2, eraw, ptype, ivar, varSF) - alphaNom;
        ATH_MSG_DEBUG("computing ALLUP, adding " << variationName(ivar) << ": " << v);
        alphaVar += pow(v, 2);
      }
      alphaVar = sqrt(alphaVar);
    }
    else if (var == egEnergyCorr::Scale::AllDown) {
      for (egEnergyCorr::Scale::Variation ivar = egEnergyCorr::Scale::ZeeStatDown;
           ivar < egEnergyCorr::Scale::AllDown;
           ivar = egEnergyCorr::Scale::Variation(ivar + 2)) {
	if (ivar == egEnergyCorr::Scale::ZeeAllDown) continue;
        const double v = getAlphaValue(runnumber, cl_eta, cl_etaCalo, energy, energyS2, eraw, ptype, ivar, varSF) - alphaNom;
        ATH_MSG_DEBUG("computing ALLDOWN, adding " << variationName(ivar) << ": " << v);
	alphaVar += pow(v, 2);
      }
      alphaVar = -sqrt(alphaVar);
    }
    else if (var == egEnergyCorr::Scale::AllCorrelatedUp) {
      for (egEnergyCorr::Scale::Variation ivar = egEnergyCorr::Scale::ZeeStatUp;
           ivar < egEnergyCorr::Scale::AllUp;
	   ivar = egEnergyCorr::Scale::Variation(ivar+2)) {
        if (ivar == egEnergyCorr::Scale::ZeeAllUp or
            ivar == egEnergyCorr::Scale::LArCalibExtra2015PreUp or
            ivar == egEnergyCorr::Scale::LArTemperature2015PreUp or
            ivar == egEnergyCorr::Scale::LArTemperature2016PreUp or
            ivar == egEnergyCorr::Scale::E4ScintillatorUp) continue;
        const double v = getAlphaValue(runnumber, cl_eta, cl_etaCalo, energy, energyS2, eraw, ptype, ivar, varSF) - alphaNom;
        alphaVar += pow(v, 2);
      }
      alphaVar = sqrt(alphaVar);
    }
    else if (var == egEnergyCorr::Scale::AllCorrelatedDown) {
      for (egEnergyCorr::Scale::Variation ivar = egEnergyCorr::Scale::ZeeStatDown;
           ivar < egEnergyCorr::Scale::AllDown;
           ivar = egEnergyCorr::Scale::Variation(ivar+2)) {
        if (ivar == egEnergyCorr::Scale::ZeeAllDown or
            ivar == egEnergyCorr::Scale::LArCalibExtra2015PreDown or
            ivar == egEnergyCorr::Scale::LArTemperature2015PreDown or
            ivar == egEnergyCorr::Scale::LArTemperature2016PreDown or
            ivar == egEnergyCorr::Scale::E4ScintillatorDown) continue;
        const double v = getAlphaValue(runnumber, cl_eta, cl_etaCalo, energy, energyS2, eraw, ptype, ivar, varSF) - alphaNom;
        alphaVar += pow(v, 2);
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

  double egammaEnergyCorrectionTool::dataConstantTerm( double eta ) const {
    return std::max(0., m_resNom->GetBinContent(m_resNom->FindBin(eta)));
  }

  double egammaEnergyCorrectionTool::dataConstantTermError( double eta ) const {
    return m_resSyst->GetBinContent(m_resSyst->FindBin(eta));
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

  double egammaEnergyCorrectionTool::getResolutionError(PATCore::ParticleDataType::DataType dataType,double energy, double eta, double etaCalo, 
                                                        PATCore::ParticleType::Type ptype,egEnergyCorr::Resolution::Variation value,
                                                       egEnergyCorr::Resolution::resolutionType resType) const

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

    if (value==egEnergyCorr::Resolution::MaterialIBLUp || value==egEnergyCorr::Resolution::MaterialIBLDown) {
      isys=0x80;
    }
    if (value==egEnergyCorr::Resolution::MaterialPP0Up || value==egEnergyCorr::Resolution::MaterialPP0Down) {
      isys=0x100;
    }
    if (value==egEnergyCorr::Resolution::af2Up || value==egEnergyCorr::Resolution::af2Down) {
      isys=0x200;
    }

    double sign = 1.;
    if (value==egEnergyCorr::Resolution::AllDown ||  value==egEnergyCorr::Resolution::ZSmearingDown ||
        value==egEnergyCorr::Resolution::SamplingTermDown ||  value==egEnergyCorr::Resolution::MaterialIDDown ||
        value==egEnergyCorr::Resolution::MaterialGapDown || value==egEnergyCorr::Resolution::MaterialCaloDown ||
        value==egEnergyCorr::Resolution::MaterialCryoDown || value==egEnergyCorr::Resolution::PileUpDown ||
	value==egEnergyCorr::Resolution::MaterialIBLDown || value==egEnergyCorr::Resolution::MaterialPP0Down ||
        value==egEnergyCorr::Resolution::af2Down) sign=-1.;

    double resolution;
    double resolution_error;
    double resolution_error_up;
    double resolution_error_down;


    getResolution_systematics(eg_resolution_ptype, energy,  eta, etaCalo, isys,  resolution, resolution_error, resolution_error_up, resolution_error_down, resType, 
          dataType == PATCore::ParticleDataType::Fast);

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

  double egammaEnergyCorrectionTool::resolution(double energy, double cl_eta, double cl_etaCalo,
						                                    PATCore::ParticleType::Type ptype,
						                                    bool withCT,
						                                    bool fast,
                                                egEnergyCorr::Resolution::resolutionType resType) const
{
    int eg_resolution_ptype;
    if (ptype == PATCore::ParticleType::Electron) eg_resolution_ptype = 0;
    else if (ptype==PATCore::ParticleType::UnconvertedPhoton) eg_resolution_ptype = 1;
    else if (ptype==PATCore::ParticleType::ConvertedPhoton) eg_resolution_ptype = 2;
    else {
      ATH_MSG_FATAL("cannot understand particle type");
      return -1;
    }

    double sig2 = 0.;

    if (m_use_new_resolution_model) {
      sig2 = pow(m_resolution_tool->getResolution(eg_resolution_ptype, energy, cl_eta, resType), 2);
      const double et = energy / cosh(cl_eta);
      sig2 += pow(pileUpTerm(energy, cl_eta, eg_resolution_ptype) / et, 2);  // TODO: why et and not E?
    } else { // OLD model

      double energyGeV = energy/GeV;
      double a = mcSamplingTerm( cl_eta );
      double b = mcNoiseTerm( cl_eta );
      double c = mcConstantTerm( cl_eta );
      sig2 = a*a/energyGeV + b*b/energyGeV/energyGeV + c*c;

    }

    if (withCT and fast) {
      throw std::runtime_error("It doesn't make sense to ask resolution fast sim + additional CT."
			                         " The resolution on data is FULL sim resolution + CT");
    }

    if (fast and std::abs(cl_eta) < 2.5) {
      if (m_esmodel == egEnergyCorr::es2017 or m_esmodel == egEnergyCorr::es2017_summer or m_esmodel == egEnergyCorr::es2017_summer_improved or m_esmodel == egEnergyCorr::es2017_summer_final or m_esmodel == egEnergyCorr::es2017_R21_PRE or m_esmodel == egEnergyCorr::es2015_5TeV or m_esmodel == egEnergyCorr::es2015PRE or m_esmodel == egEnergyCorr::es2015PRE_res_improved or m_esmodel == egEnergyCorr::es2015cPRE or m_esmodel == egEnergyCorr::es2015cPRE_res_improved or m_esmodel == egEnergyCorr::es2015c_summer or m_esmodel == egEnergyCorr::es2016PRE or m_esmodel == egEnergyCorr::es2017_R21_v0 or m_esmodel == egEnergyCorr::es2017_R21_v1) {

        double ratio_IQR_full_fast = 1.;
        const double ptGeV = energy / cosh(cl_eta) / 1E3;

        if  ( m_esmodel == egEnergyCorr::es2017_R21_v1) {
//
// for es2017_R21_v1, histograms contain directly values of deltaSigma**2 of relative energy resolution (FastSIm-FulSim) so need to subtract this value to get the sigma**2 of FastSim

           if(ptype == PATCore::ParticleType::Electron) sig2 -=   getValueHistAt(*m_G4OverAFII_resolution_electron,cl_eta,ptGeV,true,true,true,true);
           if (ptype == PATCore::ParticleType::UnconvertedPhoton) sig2 -=   getValueHistAt(*m_G4OverAFII_resolution_unconverted,cl_eta,ptGeV,true,true,true,true);
           if (ptype == PATCore::ParticleType::ConvertedPhoton) sig2 -=   getValueHistAt(*m_G4OverAFII_resolution_converted,cl_eta,ptGeV,true,true,true,true);
           if (sig2<0.) sig2=0.;
        }
        else {
          if (ptype == PATCore::ParticleType::Electron) { ratio_IQR_full_fast = getValueHistAt(*m_G4OverAFII_resolution_electron, ptGeV, cl_eta, true, false); }
          else if (ptype == PATCore::ParticleType::UnconvertedPhoton) { ratio_IQR_full_fast = getValueHistAt(*m_G4OverAFII_resolution_unconverted, ptGeV, cl_eta, true, false); }
          else if (ptype == PATCore::ParticleType::ConvertedPhoton) {  ratio_IQR_full_fast = getValueHistAt(*m_G4OverAFII_resolution_converted, ptGeV, cl_eta, true, false); }

          sig2 /= ratio_IQR_full_fast * ratio_IQR_full_fast;
        }
      }
    }

    // add the additional constant term from the Zee data/MC measurement
    if (withCT) sig2 += pow(dataConstantTerm(m_use_etaCalo_scales ? cl_etaCalo : cl_eta), 2);  // TODO: is it correct? Or should be += -c**2 + (c + deltac) ** 2 ?

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

  double egammaEnergyCorrectionTool::getSmearingCorrection(double cl_eta, double cl_etaCalo, double energy,
                                                           PATCore::ParticleType::Type ptype,
                                                           PATCore::ParticleDataType::DataType dataType,
                                                           egEnergyCorr::Resolution::Variation value,
                                                           egEnergyCorr::Resolution::resolutionType resType) const {

    if (dataType == PATCore::ParticleDataType::Data) { ATH_MSG_FATAL("Trying to compute smearing correction on data"); }

    if (value == egEnergyCorr::Resolution::None || energy <= 0.) return 1.0;

    const double energyGeV = energy / GeV;

    // relative resolutions
    const double resMC = resolution(energy, cl_eta, cl_etaCalo, ptype, false, // no additional CT
			                              dataType == PATCore::ParticleDataType::Fast, resType);
    double resData = resolution(energy, cl_eta, cl_etaCalo, ptype, true,  // with additional CT
		                            false, resType);  // on top of Full simulation

    ATH_MSG_DEBUG("resolution in data: " << resData << " in MC: " << resMC);

    if (m_use_new_resolution_model) {
      resData *= 1 + getResolutionError(dataType,energy, cl_eta, cl_etaCalo, ptype, value, resType);
    } else { // OLD model
      double errUp, errDown;
      resolutionError( energyGeV, cl_eta, errUp, errDown );
      if( value == egEnergyCorr::Resolution::AllDown ) resData += errDown;
      else if( value == egEnergyCorr::Resolution::AllUp )	resData += errUp;
      else if( value != egEnergyCorr::Resolution::Nominal ) {
        //      std::cout << "getSmearingCorrection : wrong value, return 1" << std::endl;
        return 1.0;
      }
    }

    ATH_MSG_DEBUG("resolution in data after systematics: " << resData);

    const double sigma2 = std::pow(resData * energyGeV, 2 ) - std::pow(resMC * energyGeV, 2);

    // TODO: for nominal case it can be simplified to:
    // const double sigma = dataConstantTerm(m_use_etaCalo_scales ? cl_etaCalo : cl_eta) * energyGeV;
    // which is just the additional constant term
    if (sigma2 <= 0) { return 1; }

    const double sigma = sqrt(sigma2);

    ATH_MSG_DEBUG("seed before = " << m_random3.GetSeed());

    const double DeltaE0 = m_random3.Gaus(0, sigma);
    const double cor0 = (energyGeV + DeltaE0) / energyGeV;

    ATH_MSG_DEBUG("sigma|DeltaE0|cor0|seed = " << sigma << "|" << DeltaE0 << "|" << cor0 << "|" << m_random3.GetSeed());

    return cor0;  // TODO: why not returning DeltaE0 and apply E -> E + DeltaE0 ?

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

  double egammaEnergyCorrectionTool::applyAFtoG4(double eta, double ptGeV,
						 PATCore::ParticleType::Type ptype) const {
    const double aeta = fabs(eta);
    if (aeta > 2.47) return 1.;

    if (m_esmodel == egEnergyCorr::es2015PRE or m_esmodel == egEnergyCorr::es2015PRE_res_improved or
        m_esmodel == egEnergyCorr::es2015cPRE or m_esmodel == egEnergyCorr::es2015cPRE_res_improved or
        m_esmodel == egEnergyCorr::es2015c_summer or m_esmodel == egEnergyCorr::es2016PRE or m_esmodel == egEnergyCorr::es2017
	or m_esmodel == egEnergyCorr::es2017_summer or m_esmodel == egEnergyCorr::es2017_summer_improved or m_esmodel == egEnergyCorr::es2017_summer_final
	or m_esmodel == egEnergyCorr::es2017_R21_PRE or m_esmodel == egEnergyCorr::es2017_R21_v0 or m_esmodel == egEnergyCorr::es2017_R21_v1) {
     
      if (m_esmodel == egEnergyCorr::es2017_R21_v1) {
//
// in es02017_R21_v1 : AF2 to FullSim correction is in a 2D eta-Pt histogram

        if (ptype == PATCore::ParticleType::Electron) { return (1.+getValueHistAt(*m_G4OverAFII_electron_2D, aeta,ptGeV,true,true,true,true)); }
        else if (ptype == PATCore::ParticleType::ConvertedPhoton) { return (1.+getValueHistAt(*m_G4OverAFII_converted_2D, aeta,ptGeV,true,true,true,true)); }
        else if (ptype == PATCore::ParticleType::UnconvertedPhoton) { return (1.+getValueHistAt(*m_G4OverAFII_unconverted_2D, aeta,ptGeV,true,true,true,true)); }
        else { throw std::runtime_error("particle not valid"); }
      }
      else {
        if (ptype == PATCore::ParticleType::Electron) { return getValueHistoAt(*m_G4OverAFII_electron, aeta); }
        else if (ptype == PATCore::ParticleType::ConvertedPhoton) { return getValueHistoAt(*m_G4OverAFII_converted, aeta); }
        else if (ptype == PATCore::ParticleType::UnconvertedPhoton) { return getValueHistoAt(*m_G4OverAFII_unconverted, aeta); }
        else { throw std::runtime_error("particle not valid"); }
      }
    }
    else {
      // run 1
      return m_G4OverAFII_electron->GetBinContent( m_G4OverAFII_electron->GetXaxis()->FindBin(eta) );
    }
  }


  // Frozen Showers -> G4 correction

  double egammaEnergyCorrectionTool::applyFStoG4(double eta) const {

    double aeta = fabs(eta);
    if( aeta<3.3 || aeta>4.9 )
      return 1.;

    return m_G4OverFrSh->GetBinContent( m_G4OverFrSh->GetXaxis()->FindBin(aeta) );

  }

  // functions for energy scale corrections

  double egammaEnergyCorrectionTool::getAlphaZee(long int runnumber, double eta, egEnergyCorr::Scale::Variation var, double varSF) const {

    if (!m_zeeNom) {
      ATH_MSG_FATAL("no data for Zee");
      return -999.0;
    }

    int ieta = m_zeeNom->GetXaxis()->FindBin(eta);
    double value = m_zeeNom->GetBinContent(ieta);

     // for es2017_R21_v0 different set of scales for 2017, 2016 and 2015 data
    if ( m_esmodel == egEnergyCorr::es2017_R21_v0 && runnumber<322817 && runnumber>=297000) {
       int ieta = m_zeeNom_data2016->GetXaxis()->FindBin(eta);
       value = m_zeeNom_data2016->GetBinContent(ieta);
    }
    
    if ( m_esmodel == egEnergyCorr::es2017_R21_v1 && runnumber<322817 && runnumber>=297000) {
      int ieta = m_zeeNom_data2016->GetXaxis()->FindBin(eta);
      value = m_zeeNom_data2016->GetBinContent(ieta);
    }

    if ((m_esmodel==egEnergyCorr::es2017 or m_esmodel == egEnergyCorr::es2017_summer or m_esmodel == egEnergyCorr::es2017_summer_improved or m_esmodel == egEnergyCorr::es2017_summer_final or m_esmodel == egEnergyCorr::es2015_5TeV or m_esmodel == egEnergyCorr::es2017_R21_v0 or m_esmodel == egEnergyCorr::es2017_R21_v1) && runnumber < 297000) {
      // 2 sets of scales for this configuration
      // change histogram if 2015 data
      int ieta = m_zeeNom_data2015->GetXaxis()->FindBin(eta);
      value = m_zeeNom_data2015->GetBinContent(ieta);
    }

    if (m_esmodel==egEnergyCorr::es2015PRE or m_esmodel==egEnergyCorr::es2015PRE_res_improved or
        m_esmodel == egEnergyCorr::es2015cPRE or m_esmodel == egEnergyCorr::es2015cPRE_res_improved) {
      // special case for es2015PRE
      // additional correction due to LAr temperature effect
      // for extrapolation 2012 -> 2015 temperature change
      // numbers from Guillaume 20150506
      /*
	2012 (K)   22/04/2015   DeltaResponse 2015/2012 (-2%/K)  deltaAlpha
	EndCap C       88.41        88.63                            -0.45%      -0.45%
	Barrel C       88.47        88.70                            -0.46%      -0.46%
	Barrel A       88.50        88.71                            -0.42%      -0.42%
	EndCap A       88.70        88.70                            +0.00%      +0.00%
      */
      if (eta >= 0) {  // side A
        if (eta < 1.45) value += -0.42E-2;
        else if (eta < 3.2) value += 0.;
      }
      else {  // side C
        if (eta > -1.45) value += -0.46E-2;
        else if (eta > -3.2) value += -0.45E-2;
      }

      // special case for es2015PRE
      // additional correction for uA->MeV first 2 weeks 2015 data
      if (runnumber >= 266904 and runnumber <= 267639 and m_use_uA2MeV_2015_first2weeks_correction)
      {
        const double uA2MeV_correction = m_uA2MeV_2015_first2weeks_correction->GetBinContent(
                                           m_uA2MeV_2015_first2weeks_correction->FindFixBin(std::abs(eta)));
        // this takes into account also O((uA2MeV_correction - 1) * alpha) terms
        // a simpler formula would be: value + uA2MeV_correction - 1
        value = uA2MeV_correction * (1 + value) - 1;
      }
    }  // end special case for es2015PRE*

    if ((m_esmodel == egEnergyCorr::es2015c_summer or m_esmodel == egEnergyCorr::es2016PRE) and m_use_temp_correction201215) {
      // keep the correction 2012->2015 for |eta| > 2.5
      // if (eta > 2.5 and eta < 3.2) value += 0.;
      if (eta < -2.5 and eta > -3.2) value += -0.45E-2;
    }

    if (m_esmodel == egEnergyCorr::es2015c_summer or m_esmodel == egEnergyCorr::es2016PRE) { // correction for the extrapolation from es2015_summer
      if (runnumber >= 297000) { // only for 2016 data
        if (eta >= 0) {  // side A
          if (eta < 1.45) value *= 1.00028;
          else if (eta < 3.2) value *= 1.00018;
        }
        else {  // side C
          if (eta > -1.45) value *= 1.00028;
          else if (eta > -3.2) value *= 0.99986;
        }
      }
    }


    if (var == egEnergyCorr::Scale::ZeeStatUp or var == egEnergyCorr::Scale::ZeeStatDown) {
      const double sign = (var == egEnergyCorr::Scale::ZeeStatUp) ? 1 : -1;
      TH1* h= ((TH1*)m_zeeNom.get());
      if ((m_esmodel == egEnergyCorr::es2017 or m_esmodel == egEnergyCorr::es2017_summer or m_esmodel == egEnergyCorr::es2017_summer_improved 
           or m_esmodel == egEnergyCorr::es2017_summer_final or m_esmodel == egEnergyCorr::es2015_5TeV 
           or m_esmodel == egEnergyCorr::es2017_R21_v0 or m_esmodel == egEnergyCorr::es2017_R21_v1) && runnumber < 297000){
        h=((TH1*) m_zeeNom_data2015.get()); // special for 2015 with es2017
      }
      if ( (m_esmodel== egEnergyCorr::es2017_R21_v0 || m_esmodel== egEnergyCorr::es2017_R21_v1) && runnumber>=297000 && runnumber<322817) {
        h=((TH1*)m_zeeNom_data2016.get()); // 2016 data
      }
      double stat_error = h->GetBinError(h->FindFixBin(eta));
	    if (m_use_stat_error_scaling) {
        stat_error = stat_error / sqrt(h->GetNbinsX());
	    }
	    value += sign * stat_error * varSF;

    }
    else if( var==egEnergyCorr::Scale::ZeeSystUp && m_zeeSyst ) {
      value += get_ZeeSyst(eta) * varSF;
    
    } else if( var==egEnergyCorr::Scale::ZeeSystDown && m_zeeSyst ) {
      value -= get_ZeeSyst(eta) * varSF;
    } else if( var==egEnergyCorr::Scale::ZeePhysUp && m_zeePhys ) {
      
      ieta = m_zeePhys->GetXaxis()->FindBin( eta );
      value += m_zeePhys->GetBinContent(ieta) * varSF;


    } else if( var==egEnergyCorr::Scale::ZeePhysDown && m_zeePhys ) {

      ieta = m_zeePhys->GetXaxis()->FindBin( eta );
      value -= m_zeePhys->GetBinContent(ieta) * varSF;


    }
    else if ((var == egEnergyCorr::Scale::LArTemperature2015PreUp or var == egEnergyCorr::Scale::LArTemperature2015PreDown) and
             (m_esmodel == egEnergyCorr::es2015PRE or m_esmodel == egEnergyCorr::es2015PRE_res_improved or
              m_esmodel == egEnergyCorr::es2015cPRE or m_esmodel == egEnergyCorr::es2015cPRE_res_improved) and
             m_use_temp_correction201215)
    {
      // special case only for es2015PRE
      // add LAr temperature effect for extrapolation 2012 -> 2015 temperature change
      // numbers from Guillaume 20150506

      const double aeta = std::abs(eta);
      const double sign = (var == egEnergyCorr::Scale::LArTemperature2015PreUp) ? 1 : -1;
      if (aeta < 1.45) { value += 0.15E-2 * sign; }
      else if (aeta > 1.45 and aeta < 3.2) { value += 0.25E-2 * sign; }
    }

    else if ((var == egEnergyCorr::Scale::LArTemperature2015PreUp or var == egEnergyCorr::Scale::LArTemperature2015PreDown) and
             (m_esmodel == egEnergyCorr::es2015c_summer or m_esmodel == egEnergyCorr::es2016PRE) and m_use_temp_correction201215) {
      // keep 2012->2015 extrapolation correction for eta > 2.5
      const double aeta = std::abs(eta);
      const double sign = (var == egEnergyCorr::Scale::LArTemperature2015PreUp) ? 1 : -1;
      if (aeta > 2.5 and aeta < 3.2) { value += 0.25E-2 * sign; }
    }


    else if ((var == egEnergyCorr::Scale::LArTemperature2016PreUp or var == egEnergyCorr::Scale::LArTemperature2016PreDown) and
             (m_esmodel == egEnergyCorr::es2016PRE) and
             m_use_temp_correction201516)
    {
      // special case for es2016PRE (extrapolation from 2015)
        const double sign = (var == egEnergyCorr::Scale::LArTemperature2016PreUp) ? 1 : -1;
        // temp + pileup
        value += qsum(0.05E-2, 0.02E-2) * sign; // Guillaume email 23/05/2016 + 26/5/2016
    }


    else if( var==egEnergyCorr::Scale::ZeeAllDown || var==egEnergyCorr::Scale::ZeeAllUp ) {

      ieta = m_zeeNom->GetXaxis()->FindBin( eta );
      double diff = pow(m_zeeNom->GetBinError(ieta) * varSF, 2);

      if( m_zeeSyst ) {
	diff += pow(get_ZeeSyst(eta) * varSF, 2);
      }

      if( m_zeePhys ) {
        ieta = m_zeePhys->GetXaxis()->FindBin(eta);
        diff += pow(m_zeePhys->GetBinContent(ieta) * varSF, 2);
      }

      if( var==egEnergyCorr::Scale::ZeeAllUp ) value += sqrt(diff);
      else if( var==egEnergyCorr::Scale::ZeeAllDown ) value -= sqrt(diff);
    }

    return value;

  }


  double egammaEnergyCorrectionTool::getE4Uncertainty(double eta) const {
    const double aeta = std::abs(eta);
    if ((aeta > 1.6) or (aeta < 1.4)) { return 0.; }

    // numbers from Archil 20/5/2016

    double data_mc_difference = 0.;
    if (aeta < 1.46) { data_mc_difference = 1E-2; }       // 1.4 - 1.46
    else if (aeta < 1.52) { data_mc_difference = 3E-2; }  // 1.46 - 1.52
    else { data_mc_difference = 4.3E-2; }                 // 1.52 - 1.6

    const double em_scale = 2.4E-2;
    const double mbias = 1E-2;  // Archil presentation 26/5/2016
    const double laser = 4E-2;

    return std::sqrt(data_mc_difference * data_mc_difference +
                     em_scale * em_scale + mbias * mbias +
                     laser * laser);
  }

  double egammaEnergyCorrectionTool::getWtots1Uncertainty(double cl_eta, double energy, PATCore::ParticleType::Type ptype) const {
    double value = 0;

    //Get slopes and wstot values
    //deltaE/E (Et, particle type ) =  2*A/mZ * ( wstot(Et,particle type)_data  - <wstot(40 GeV Et electrons)_data)> ) - 2*B/mZ* (wstot(Et,particle type)_MC - <w wstot(40 GeV Et electrons)_MC>)
    //factor 2 to go from slopes in dM/M to dE/E

    //|eta|>2.4 => use last eta bin
    if(cl_eta<-2.4) cl_eta = -2.35;
    if(cl_eta>2.4) cl_eta = 2.35;

    int bin = m_wstot_slope_A_data->FindFixBin(cl_eta);
    double A = m_wstot_slope_A_data->GetBinContent(bin);
    double B = m_wstot_slope_B_MC->GetBinContent(bin);
    double wstot_40_data = m_wstot_40GeV_data->GetBinContent(bin);
    double wstot_40_MC = m_wstot_40GeV_MC->GetBinContent(bin);

    //the wstot=f(pT) depends on the particle type
    double ETGeV = energy / cosh(cl_eta) / 1E3;
    double wstot_pT_data_p0 = 0.;
    double wstot_pT_data_p1 = 0.;
    double wstot_pT_MC_p0 = 0.;
    double wstot_pT_MC_p1 = 0.;

    if (ptype == PATCore::ParticleType::Electron ) {
      wstot_pT_data_p0 = m_wstot_pT_data_p0_electrons->GetBinContent(bin);
      wstot_pT_data_p1 = m_wstot_pT_data_p1_electrons->GetBinContent(bin);
      wstot_pT_MC_p0 = m_wstot_pT_MC_p0_electrons->GetBinContent(bin);
      wstot_pT_MC_p1 = m_wstot_pT_MC_p1_electrons->GetBinContent(bin);
    }
    else if (ptype == PATCore::ParticleType::UnconvertedPhoton) {
      wstot_pT_data_p0 = m_wstot_pT_data_p0_unconverted_photons->GetBinContent(bin);
      wstot_pT_data_p1 = m_wstot_pT_data_p1_unconverted_photons->GetBinContent(bin);
      wstot_pT_MC_p0 = m_wstot_pT_MC_p0_unconverted_photons->GetBinContent(bin);
      wstot_pT_MC_p1 = m_wstot_pT_MC_p1_unconverted_photons->GetBinContent(bin);
    }
    else if (ptype == PATCore::ParticleType::ConvertedPhoton) {
      wstot_pT_data_p0 = m_wstot_pT_data_p0_converted_photons->GetBinContent(bin);
      wstot_pT_data_p1 = m_wstot_pT_data_p1_converted_photons->GetBinContent(bin);
      wstot_pT_MC_p0 = m_wstot_pT_MC_p0_converted_photons->GetBinContent(bin);
      wstot_pT_MC_p1 = m_wstot_pT_MC_p1_converted_photons->GetBinContent(bin);
    }

    double wstot_pT_data = 0.;
    double wstot_pT_MC = 0.;

    //Initial parametrization: [0]+1*pT
    //wstot_pT_data = wstot_pT_data_p0+wstot_pT_data_p1*ETGeV;
    //prevent wstot_pT_data from being negative
    //if(wstot_pT_data<0) wstot_pT_data = 0.;

    //New parametrization: p0+p1/sqrt(pT)
    //flat uncertainty below 25 GeV
    if(ETGeV<25.) ETGeV = 25.;

    wstot_pT_data = wstot_pT_data_p0+wstot_pT_data_p1/sqrt(ETGeV);
    wstot_pT_MC = wstot_pT_MC_p0+wstot_pT_MC_p1/sqrt(ETGeV);

    value = 2*A/91.2*(wstot_pT_data-wstot_40_data) - 2*B/91.2*(wstot_pT_MC-wstot_40_MC);

    return value;
  }



  // Layer scale uncertainties and induced non-linearity
  //////////////////////////////////////////////////////

  double egammaEnergyCorrectionTool::getLayerUncertainty( int iLayer,
							  double cl_eta,
							  egEnergyCorr::Scale::Variation var,
                double varSF) const {

    double value = 0.;

    if (var == egEnergyCorr::Scale::Nominal) return value;

    // nearest eta outside of crack (for PS scale values and uncertainties)
    double nearestEta = nearestEtaBEC(cl_eta);

    if( iLayer==0 ) { // use nearestEta

      if( var==egEnergyCorr::Scale::PSUp && m_aPSNom )
	value =  m_aPSNom->GetBinError( m_aPSNom->FindBin(nearestEta) );

      else if( var==egEnergyCorr::Scale::PSDown && m_aPSNom )
	value = -m_aPSNom->GetBinError( m_aPSNom->FindBin(nearestEta) );

      else if( var==egEnergyCorr::Scale::PSb12Up && m_daPSb12 )
	value =  m_daPSb12->GetBinContent( m_daPSb12->FindBin(nearestEta) );
      
      else if( var==egEnergyCorr::Scale::PSb12Down && m_daPSb12 )
	value = -m_daPSb12->GetBinContent( m_daPSb12->FindBin(nearestEta) );
      
      else if( var==egEnergyCorr::Scale::LArElecUnconvUp && m_daPSCor )
	value =  m_daPSCor->GetBinContent( m_daPSCor->FindBin(nearestEta) );

      else if( var==egEnergyCorr::Scale::LArElecUnconvDown && m_daPSCor )
	value = -m_daPSCor->GetBinContent( m_daPSCor->FindBin(nearestEta) );
      
    }

    else if( iLayer==1 ) { // use cl_eta

      if (var == egEnergyCorr::Scale::S12Up && m_aS12Nom)	{ value = m_aS12Nom->GetBinError(m_aS12Nom->FindBin(cl_eta)); }
      else if (var == egEnergyCorr::Scale::S12Down && m_aS12Nom) { value = -m_aS12Nom->GetBinError(m_aS12Nom->FindBin(cl_eta)); }
      else if (var == egEnergyCorr::Scale::LArCalibUp && m_daS12Cor) { value = m_daS12Cor->GetBinContent(m_daS12Cor->FindBin(cl_eta)); }
      else if (var == egEnergyCorr::Scale::LArCalibDown && m_daS12Cor) { value = -m_daS12Cor->GetBinContent( m_daS12Cor->FindBin(cl_eta)); }
      else if (var == egEnergyCorr::Scale::LArCalibExtra2015PreUp and 
	       (m_esmodel == egEnergyCorr::es2015PRE or m_esmodel == egEnergyCorr::es2015PRE_res_improved or
		m_esmodel == egEnergyCorr::es2015cPRE or m_esmodel == egEnergyCorr::es2015cPRE_res_improved or
		m_esmodel == egEnergyCorr::es2015c_summer or m_esmodel == egEnergyCorr::es2016PRE or m_esmodel == egEnergyCorr::es2017
		or m_esmodel == egEnergyCorr::es2017_summer or m_esmodel == egEnergyCorr::es2017_summer_improved or m_esmodel == egEnergyCorr::es2017_R21_PRE
		or m_esmodel == egEnergyCorr::es2015_5TeV)) { 
	// special case for es2015PRE and also for es2015c_summer and also for es2017
	// numbers from Lydia and Christophe,
	// https://indico.cern.ch/event/395345/contribution/2/material/slides/0.pdf
	// assuming constant uncertainty
	// es2017_summer: increased to 5% in the endcap
	const double aeta = std::abs(cl_eta); 
	//endcap
	if ( aeta >= 1.37 and aeta < 2.5){
	  if ( m_esmodel == egEnergyCorr::es2017_summer or m_esmodel == egEnergyCorr::es2017_summer_improved or m_esmodel == egEnergyCorr::es2017_R21_PRE or m_esmodel == egEnergyCorr::es2015_5TeV) value = 5.0E-2;
	  else value = 1.5E-2;
	}
	else{//barrel
	  if (m_esmodel == egEnergyCorr::es2017_summer_improved) value = 2.5E-2;
	  else value = 1.5E-2;
	}
      }
      else if (var == egEnergyCorr::Scale::LArCalibExtra2015PreDown and
               (m_esmodel == egEnergyCorr::es2015PRE or m_esmodel == egEnergyCorr::es2015PRE_res_improved or
                m_esmodel == egEnergyCorr::es2015cPRE or m_esmodel == egEnergyCorr::es2015cPRE_res_improved or
                m_esmodel == egEnergyCorr::es2015c_summer or m_esmodel == egEnergyCorr::es2016PRE or m_esmodel == egEnergyCorr::es2017
		or m_esmodel == egEnergyCorr::es2017_summer or m_esmodel == egEnergyCorr::es2017_summer_improved or m_esmodel == egEnergyCorr::es2017_R21_PRE
		or m_esmodel == egEnergyCorr::es2015_5TeV)) {
	const double aeta = std::abs(cl_eta);  
        //endcap 
        if ( aeta >= 1.37 and aeta < 2.5){ 
          if ( m_esmodel == egEnergyCorr::es2017_summer or m_esmodel == egEnergyCorr::es2017_summer_improved or m_esmodel == egEnergyCorr::es2017_R21_PRE or m_esmodel == egEnergyCorr::es2015_5TeV) value = -5.0E-2; 
          else value = -1.5E-2; 
        } 
        else{//barrel 
          if (m_esmodel == egEnergyCorr::es2017_summer_improved or m_esmodel == egEnergyCorr::es2015_5TeV) value = -2.5E-2; 
          else value = -1.5E-2; 
        } 
      }
      
      else if (var == egEnergyCorr::Scale::S12ExtraLastEtaBinRun2Up or var == egEnergyCorr::Scale::S12ExtraLastEtaBinRun2Down) {
        // special large sys for run2 in the last eta-bin in es2017, see ATLASEG-42
        if (m_esmodel == egEnergyCorr::es2017 or m_esmodel == egEnergyCorr::es2017_summer or m_esmodel == egEnergyCorr::es2017_summer_improved or m_esmodel == egEnergyCorr::es2017_R21_PRE or m_esmodel == egEnergyCorr::es2015_5TeV) {
	  const double aeta = std::abs(cl_eta);
	  if (aeta >= 2.4 and aeta < 2.5) {
	    if (var == egEnergyCorr::Scale::S12ExtraLastEtaBinRun2Up) value = 25E-2;
	    else value = -25E-2;
	  }
	}
      }

    }


    return value * varSF;

  }


  double egammaEnergyCorrectionTool::getE4NonLinearity(double cl_eta, double energy, PATCore::ParticleType::Type ptype) const
  {
    double value = 0;
    const double aeta = std::abs(cl_eta);
    const double ETGeV = energy / cosh(cl_eta) / 1E3;

    //This will point to the return of get() of a unique_ptr
    TAxis* axis;
    TList* graphs;

    if (ptype == PATCore::ParticleType::Electron ) { 
      axis=(dynamic_cast<TAxis*>(m_E4ElectronEtaBins.get())); 
      graphs=(dynamic_cast<TList*>(m_E4ElectronGraphs.get())); 
      graphs->SetOwner();
    } else if (ptype == PATCore::ParticleType::UnconvertedPhoton) { 
      axis=dynamic_cast<TAxis*>(m_E4UnconvertedEtaBins.get()); 
      graphs=dynamic_cast<TList*>(m_E4UnconvertedGraphs.get()); 
      graphs->SetOwner();
    }
    else if (ptype == PATCore::ParticleType::ConvertedPhoton) { 
      axis=dynamic_cast<TAxis*>(m_E4ConvertedEtaBins.get()); 
      graphs=dynamic_cast<TList*>(m_E4ConvertedGraphs.get()); 
      graphs->SetOwner();
    }
    else { ATH_MSG_FATAL("invalid particle type"); return -1; }


    const int ieta = axis->FindFixBin(aeta) - 1;
    if (ieta >= 0 and ieta < graphs->GetSize()) {
      value = static_cast<TGraph*>(graphs->At(ieta))->Eval(ETGeV);
    }

    return value;
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

    if (value < 0) {
      ATH_MSG_DEBUG("Value is negative -> set to 0");
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


    return value * varSF;

  }

double egammaEnergyCorrectionTool::getMaterialEffect(egEnergyCorr::Geometry geo,PATCore::ParticleType::Type ptype,double cl_eta,double ET) const {
 
  //Again this does no need to be ptr just get the one owned
   TH2D* hmat;

   if (ptype==PATCore::ParticleType::Electron) {
      if (geo==egEnergyCorr::ConfigA) hmat=((TH2D*) m_electronBias_ConfigA.get());
      else if (geo==egEnergyCorr::ConfigEL) hmat=((TH2D*)m_electronBias_ConfigEpLp.get());
      else if (geo==egEnergyCorr::ConfigFMX) hmat=((TH2D*)m_electronBias_ConfigFpMX.get());
      else if (geo==egEnergyCorr::ConfigN) hmat=((TH2D*)m_electronBias_ConfigN.get());
      else if (geo==egEnergyCorr::ConfigIBL) hmat=((TH2D*)m_electronBias_ConfigIBL.get());
      else if (geo==egEnergyCorr::ConfigPP0) hmat=((TH2D*)m_electronBias_ConfigPP0.get());
      else return 0;
   } else if (ptype==PATCore::ParticleType::UnconvertedPhoton) {
      if (geo==egEnergyCorr::ConfigA) hmat=((TH2D*)m_unconvertedBias_ConfigA.get());
      else if (geo==egEnergyCorr::ConfigEL) hmat=((TH2D*)m_unconvertedBias_ConfigEpLp.get());
      else if (geo==egEnergyCorr::ConfigFMX) hmat=((TH2D*)m_unconvertedBias_ConfigFpMX.get());
      else if (geo==egEnergyCorr::ConfigN) hmat=((TH2D*)m_unconvertedBias_ConfigN.get());
      else if (geo==egEnergyCorr::ConfigIBL) hmat=((TH2D*)m_unconvertedBias_ConfigIBL.get());
      else if (geo==egEnergyCorr::ConfigPP0) hmat=((TH2D*)m_unconvertedBias_ConfigIBL.get());
      else return 0;
   } else if (ptype==PATCore::ParticleType::ConvertedPhoton) {
      if (geo==egEnergyCorr::ConfigA) hmat=((TH2D*)m_convertedBias_ConfigA.get());
      else if (geo==egEnergyCorr::ConfigEL) hmat=((TH2D*)m_convertedBias_ConfigEpLp.get());
      else if (geo==egEnergyCorr::ConfigFMX) hmat=((TH2D*)m_convertedBias_ConfigFpMX.get());
      else if (geo==egEnergyCorr::ConfigN) hmat=((TH2D*)m_convertedBias_ConfigN.get());
      else if (geo==egEnergyCorr::ConfigIBL) hmat=((TH2D*)m_convertedBias_ConfigIBL.get());
      else if (geo==egEnergyCorr::ConfigPP0) hmat=((TH2D*)m_convertedBias_ConfigPP0.get());
      else return 0;
   } else return 0;



   

   // use one bin in eta and linear interpolation in Et between 2 bins

  double aeta=fabs(cl_eta);
  int ieta = hmat->GetXaxis()->FindBin(aeta);

  int ipt = hmat->GetYaxis()->FindBin(ET);
  double ptBin = hmat->GetYaxis()->GetBinCenter(ipt);

  int i1,i2;
  double pt1,pt2;
   if (ET>ptBin) {
      i1=ipt;
      i2=ipt+1;
      pt1=ptBin;
      pt2= hmat->GetYaxis()->GetBinCenter(i2);
  }
  else {
     i1=ipt-1;
     i2=ipt;
     pt1=hmat->GetYaxis()->GetBinCenter(i1);
     pt2=ptBin;
  }

  int nbins=hmat->GetYaxis()->GetNbins();
  double value=0;
  if (i1>=1 && i1 < nbins) {
    double v1 = hmat->GetBinContent(ieta,i1);
    double v2 = hmat->GetBinContent(ieta,i2);
    value =  (v1*(pt2-ET) + v2*(ET-pt1)) / (pt2-pt1);
  }
  else{
    if (ipt<1) ipt=1;
    if (ipt>nbins) ipt=nbins;
    value=hmat->GetBinContent(ieta,ipt);
  }
  return value;

}


  // returns the energy dependence of the above (non-zero for electrons only).

  double egammaEnergyCorrectionTool::getMaterialNonLinearity( double cl_eta, double energy,
							      egEnergyCorr::MaterialCategory imat,
							      PATCore::ParticleType::Type ptype,
							      egEnergyCorr::Scale::Variation var, double varSF ) const {

    double value = 0;
    double ET = energy/cosh(cl_eta)/GeV;

    if( (ptype!=PATCore::ParticleType::Electron && m_esmodel != egEnergyCorr::es2017_R21_v1)  || var==egEnergyCorr::Scale::Nominal )
      return value;

    egEnergyCorr::Geometry geoID, geoCryo, geoCalo, geoGp;
    geoID = egEnergyCorr::ConfigA;
    if( fabs(cl_eta)<2. )
      geoCryo = egEnergyCorr::ConfigEL;
    else
      geoCryo = egEnergyCorr::ConfigFMX;

    //   G.Unal 21.08.2018
    // for Calo material use correctly ConfigN material for endcap (PS to Calo) in release 21 (not done for run 1, which used FMX in this case)
    if (fabs(cl_eta)>1.52 && m_esmodel == egEnergyCorr::es2017_R21_v1)
      geoCalo = egEnergyCorr::ConfigN;
    else   
      geoCalo = egEnergyCorr::ConfigFMX;
    geoGp = egEnergyCorr::ConfigGp;

    // look up material bias

    double DeltaX = getDeltaX(cl_eta, imat, var) - getDeltaX(cl_eta, imat, egEnergyCorr::Scale::Nominal);


    // calculate scale change per unit added material

    // G.Unal 21.08.2019 new code called for release 21 sensivitities

    double DAlphaDXGp,DAlphaDXID,DAlphaDXCryo,DAlphaDXCalo;

    if (m_esmodel == egEnergyCorr::es2017_R21_v1) {
       DAlphaDXGp = getMaterialEffect(egEnergyCorr::ConfigFMX,ptype,cl_eta,ET);   // no G' in release 21, use FMX for the crack
       DAlphaDXID = getMaterialEffect(geoID,ptype,cl_eta,ET);
       DAlphaDXCryo = getMaterialEffect(geoCryo,ptype,cl_eta,ET);
       DAlphaDXCalo = getMaterialEffect(geoCalo,ptype,cl_eta,ET);

    } else {
      int ialpha = m_matElectronEtaBins->FindBin( fabs(cl_eta) ) - 1;
      if (ialpha<0 || ialpha>=m_matElectronGraphs[geoGp]->GetSize())
        return 0.;

      DAlphaDXGp   = ((TGraphErrors*)m_matElectronGraphs[geoGp]->At(ialpha))->GetFunction("fNonLin")->Eval( ET );
      DAlphaDXID   = ((TGraphErrors*)m_matElectronGraphs[geoID]->At(ialpha))->GetFunction("fNonLin")->Eval( ET );
      DAlphaDXCryo = ((TGraphErrors*)m_matElectronGraphs[geoCryo]->At(ialpha))->GetFunction("fNonLin")->Eval( ET );
      DAlphaDXCalo = ((TGraphErrors*)m_matElectronGraphs[geoCalo]->At(ialpha))->GetFunction("fNonLin")->Eval( ET );
    }


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
    {
      if (m_matX0Additions[geoID]->GetBinContent(idx) >0.) DAlphaDXID /= m_matX0Additions[geoID]->GetBinContent(idx);
      else DAlphaDXID=0.;
    }

    idx = m_matX0Additions[geoCryo]->FindBin( fabs(cl_eta) );
    if(idx<1 || idx>m_matX0Additions[geoCryo]->GetNbinsX())
      DAlphaDXCryo = 0;
    else
    {
      if (m_matX0Additions[geoCryo]->GetBinContent(idx)>0.) DAlphaDXCryo /= m_matX0Additions[geoCryo]->GetBinContent(idx);
      else DAlphaDXCryo=0.;
    }

    idx = m_matX0Additions[geoCalo]->FindBin( fabs(cl_eta) );
    if(idx<1 || idx>m_matX0Additions[geoCalo]->GetNbinsX())
      DAlphaDXCalo = 0;
    else
    {
      if (m_matX0Additions[geoCalo]->GetBinContent(idx)>0.) DAlphaDXCalo /= m_matX0Additions[geoCalo]->GetBinContent(idx);
      else DAlphaDXCalo=0.;
    }

    // final value

    if( imat==egEnergyCorr::MatID )
      value = DeltaX * (DAlphaDXID - DAlphaDXCryo);
    else if( imat==egEnergyCorr::MatCryo )
      value = DeltaX * DAlphaDXCryo;
    else if( imat==egEnergyCorr::MatCalo )
      value = DeltaX * DAlphaDXCalo;


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

  double egammaEnergyCorrectionTool::getAlphaPedestal(double cl_eta, double energy, double eraw, PATCore::ParticleType::Type ptype, bool isRef,
						      egEnergyCorr::Scale::Variation var, double varSF) const {
    double alpha = 0.;
    if (var == egEnergyCorr::Scale::PedestalUp || var == egEnergyCorr::Scale::PedestalDown) {
      if (m_esmodel == egEnergyCorr::es2017) {
	const double delta = getValueHistoAt(*m_pedestals_es2017, std::abs(cl_eta));
	alpha = delta / (energy / cosh(cl_eta));
	if (var == egEnergyCorr::Scale::PedestalDown) alpha *= -1;
      }
      else if(m_esmodel == egEnergyCorr::es2017_summer or m_esmodel == egEnergyCorr::es2017_summer_improved or m_esmodel == egEnergyCorr::es2017_summer_final or m_esmodel == egEnergyCorr::es2017_R21_PRE or m_esmodel == egEnergyCorr::es2015_5TeV or m_esmodel == egEnergyCorr::es2017_R21_v0 or  m_esmodel == egEnergyCorr::es2017_R21_v1){
	//Et uncertainty band: 10 MeV for the corrected cluster
	alpha = 10. / (energy / cosh(cl_eta));
	if (var == egEnergyCorr::Scale::PedestalDown) alpha *= -1;
      }
      else {
	// observed pedestal corrected as a systematic on MC for now.
	// TODO : correct for it in the data



	double pedestal = getLayerPedestal(cl_eta, ptype, 0, var, varSF) +
	  getLayerPedestal(cl_eta, ptype, 1, var, varSF) +
	  getLayerPedestal(cl_eta, ptype, 2, var, varSF) +
	  getLayerPedestal(cl_eta, ptype, 3, var, varSF);


	if( isRef )
	  alpha = pedestal/energy*1.06; // approximate average ratio between calibrated and raw
	else
	  alpha = pedestal/eraw;

      }
    }

    return alpha * varSF;
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


  double egammaEnergyCorrectionTool::pileUpTerm(double energy, double eta, int particle_type) const {

    double pileupNoise;

    // release 21 for <mu> =32 (combined 2015-2016-2017 dataset), pileup noise = f(Et) for superclusters
    if (m_esmodel == egEnergyCorr::es2017_R21_v0 or m_esmodel == egEnergyCorr::es2017_R21_v1) {
       double et = energy/cosh(eta);
       if (et<5000.) et=5000.;
       if (et>50000.) et=50000.;
       pileupNoise=sqrt(32.)*(60.+40.*log(et/10000.)/log(5.));
    }
    else {
      // approximate pileup noise addition to the total noise in MeV   for <mu_data> (2012) = 20
      // converted photons and electrons
      pileupNoise=240.;
      // unconverted photons, different values in barrel and end-cap
      if (particle_type==1) {
      if (fabs(eta)<1.4) pileupNoise=200.;
      }
    }
    return pileupNoise;

  }


  void egammaEnergyCorrectionTool::getResolution_systematics(int particle_type,
							     double energy,
							     double eta,
							     double etaCalo,
							     int syst_mask,
							     double& resolution,
							     double& resolution_error,
                                                             double& resolution_error_up,
                                                             double& resolution_error_down,
							     int resol_type,
                                                             bool fast) const {

    double pileupNoise =  pileUpTerm(energy,eta, particle_type);
    double et = energy/cosh(eta);

    resolution = m_resolution_tool->getResolution(particle_type,energy,eta,resol_type);
    //std::cout << " resolution from tool " << resolution << std::endl;
    double smearingZ=dataConstantTerm(m_use_etaCalo_scales ? etaCalo : eta);
    double esmearingZ=dataConstantTermError(m_use_etaCalo_scales ? etaCalo : eta);
    double resolution2=resolution*resolution+smearingZ*smearingZ + (pileupNoise*pileupNoise)/(et*et);
    resolution=sqrt(resolution2);


    double_t sum_sigma_resolution2=0.;
    double sum_deltaDown=0.;
    double sum_deltaUp=0.;

    for (int isys=0;isys<10;isys++) {

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
    ATH_MSG_DEBUG(boost::format("sys resolution Zsmearing: %.7f %.7f %.7f") % sigma2 % sigma2up % sigma2down);
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
    ATH_MSG_DEBUG(boost::format("sys resolution intrinsic: %.7f %.7f %.7f") % sigma2 % sigma2up % sigma2down);
	}

	// systematics from configA ID material
	if (isys==2) {
    double sigmaA= m_getMaterialDelta->getDelta(particle_type,energy,eta,1,0);
	  sigma2 = sigmaA*sigmaA;
	  sigma2up = sigma2;
	  sigma2down = -1.*sigma2;
    ATH_MSG_DEBUG(boost::format("sys resolution configA ID material: %.7f %.7f %.7f") % sigma2 % sigma2up % sigma2down);
	}

	// systematics from material presampler-layer 1 in barrel (based on half config M )
	if (isys==3 ) {
	  if( fabs(eta)<1.45) {
	    double sigmaM = m_getMaterialDelta->getDelta(particle_type,energy,eta,1,3);
	    sigma2 = 0.5*sigmaM*sigmaM;
	  } else sigma2=0.;
	  sigma2up = sigma2;
	  sigma2down = -1.*sigma2;
    ATH_MSG_DEBUG(boost::format("sys resolution presampler-layer1: %.7f %.7f %.7f") % sigma2 % sigma2up % sigma2down);
	}

	// systematic from material in barrel-endcap gap (using full config X for now)
	if (isys==4) {
	  if (fabs(eta)>1.52 && fabs(eta)<1.82) {
	    double sigmaX =  m_getMaterialDelta->getDelta(particle_type,energy,eta,1,3);
	    sigma2 = sigmaX*sigmaX;
	  } else sigma2=0.;
	  sigma2up = sigma2;
	  sigma2down = -1.*sigma2;
    ATH_MSG_DEBUG(boost::format("sys resolution barrel-endcap gap: %.7f %.7f %.7f") % sigma2 % sigma2up % sigma2down);
	}

	// systematics from material in cryostat area (using half config EL, FIXME: could use clever eta dependent scaling)
	if (isys==5) {
	  double sigmaEL = m_getMaterialDelta->getDelta(particle_type,energy,eta,1,2);
	  sigma2 = 0.5*sigmaEL*sigmaEL;
	  sigma2up = sigma2;
	  sigma2down = -1.*sigma2;
    ATH_MSG_DEBUG(boost::format("sys resolution cryostat area: %.7f %.7f %.7f") % sigma2 % sigma2up % sigma2down);
	}

	// systematics from pileup noise  on total noise (200 MeV in quadrature, somewhat conservative)
	if (isys==6) {
   
          double et = energy/cosh(eta);
          double sigmaPileUp=0.;
          double sigmaZ=0.;
          // release 21 - 10% uncertainty on pileup noise
          if (m_esmodel == egEnergyCorr::es2017_R21_v0 or m_esmodel == egEnergyCorr::es2017_R21_v1) {
              double deltaNoise = sqrt(1.1*1.1-1.0)*pileupNoise;  // uncertainty in quadrature 1.1*noise - noise
              sigmaPileUp = deltaNoise/et;   // sigmaE/E impact
              sigmaZ = deltaNoise/40000.;    // sigmaE/E for Z->ee electrons (absorbed in smearing correction)
          }
          else {
           // older models
	    double deltaPileupNoise=100.; // MeV
	    if (std::abs(eta)>=1.4 && std::abs(eta)<1.8) deltaPileupNoise=200.; // larger systematic in this eta bin
	    double scaleNcells=1;
	    if (particle_type==1 && std::abs(eta)<1.4) scaleNcells = sqrt(3./5.);   // cluster=3X5 instead of 3x7, rms scales with cluster area
	    sigmaPileUp = deltaPileupNoise*scaleNcells/et;
	    sigmaZ = deltaPileupNoise/(40000.); // effect for Z->ee at Et=40 GeV
          }
	  sigma2=sigmaPileUp*sigmaPileUp-sigmaZ*sigmaZ;
	  sigma2up = sigma2;
	  sigma2down = -1.*sigma2;
    ATH_MSG_DEBUG(boost::format("sys resolution pileup noise: %.7f %.7f %.7f") % sigma2 % sigma2up % sigma2down);
	}

	// systematics from material in IBL+PP0 for barrel
	if (isys==7 && fabs(eta)<1.5  && (m_esmodel==egEnergyCorr::es2017 or m_esmodel == egEnergyCorr::es2017_summer or m_esmodel == egEnergyCorr::es2017_summer_improved or m_esmodel == egEnergyCorr::es2017_summer_final or m_esmodel == egEnergyCorr::es2015_5TeV or m_esmodel == egEnergyCorr::es2017_R21_v0 or m_esmodel == egEnergyCorr::es2017_R21_v1)) {
	  double  sigmaE = m_getMaterialDelta->getDelta(particle_type,energy,eta,1,5);
	  sigma2 = sigmaE*sigmaE;
	  sigma2up = sigma2;
	  sigma2down = -1.*sigma2;
	  ATH_MSG_DEBUG(boost::format("sys resolution ibl material: %.7f %.7f %.7f") % sigma2 % sigma2up % sigma2down);
	}

	// systematics from material in IBL+PP0 for end-cap
	if (isys==8 && fabs(eta)>1.5 && (m_esmodel==egEnergyCorr::es2017 or m_esmodel == egEnergyCorr::es2017_summer or m_esmodel == egEnergyCorr::es2017_summer_improved or m_esmodel == egEnergyCorr::es2017_summer_final or m_esmodel == egEnergyCorr::es2015_5TeV or m_esmodel == egEnergyCorr::es2017_R21_v0 or m_esmodel == egEnergyCorr::es2017_R21_v1)) {
	  double sigmaE =  m_getMaterialDelta->getDelta(particle_type,energy,eta,1,5);
	  // scale factor 2.3 in X0 => sqrt(2) in resolution or 2 in resolution**2
	  sigma2 = 2.3*sigmaE*sigmaE;
	  sigma2up = sigma2;
	  sigma2down = -1.*sigma2;
	  ATH_MSG_DEBUG(boost::format("sys resolution pp0 material: %.7f %.7f %.7f") % sigma2 % sigma2up % sigma2down);

	}

        // AF2 resolution systematics for es2017_R21_v1 model (neglected before that...)

        if (isys==9 && m_esmodel == egEnergyCorr::es2017_R21_v1 && fast) {
           const double ptGeV = et/1e3;
           if(particle_type == 0) sigma2 = getValueHistAt(*m_G4OverAFII_resolution_electron,eta,ptGeV,true,true,true,true);
           if(particle_type == 1) sigma2 = getValueHistAt(*m_G4OverAFII_resolution_unconverted,eta,ptGeV,true,true,true,true);
           if(particle_type == 2) sigma2 = getValueHistAt(*m_G4OverAFII_resolution_converted,eta,ptGeV,true,true,true,true);
           sigma2up = -1.*sigma2;  // AF2 resolution worse than full Sim, sigma2up gives back AF2 resolution
           sigma2down = sigma2;
        }


//  old method to use max of up and down for All
/*
	double rr1 = sqrt(resolution2+sigma2);   // nominal (data) + average error
	double rr2=0.;
	if((resolution2-sigma2) > 0.) rr2 = sqrt(resolution2-sigma2);  // max(0, nominal (data) - average error)
	double deltaSigma_sys;
	if ((rr1-resolution) > (resolution-rr2) ) deltaSigma_sys = rr1-resolution;
	else deltaSigma_sys = resolution-rr2;
	deltaSigma_sys = deltaSigma_sys / resolution;
*/

// use average of up and down for symmetric uncertainty for All

	double rr1=0.;
	if((resolution2+sigma2up)>0.) rr1 = sqrt(resolution2+sigma2up);   // nominal (data) + up error
	double rr2=0.;
        if((resolution2+sigma2down) > 0.) rr2 = sqrt(resolution2+sigma2down);  // max(0, nominal (data) + down error
        double deltaSigma_sys;
        deltaSigma_sys = 0.5*(rr1-rr2);        // average of up and down uncertainties
        deltaSigma_sys = deltaSigma_sys / resolution;   // relative resolution uncertainty

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
    //	      << " Z smearing " << smearingZ << " +- " << esmearingZ << std::endl;

  }

  string egammaEnergyCorrectionTool::variationName(egEnergyCorr::Scale::Variation& var) const {
    switch(var) {
    case egEnergyCorr::Scale::None: return "None";
    case egEnergyCorr::Scale::Nominal: return "Nominal";
    case egEnergyCorr::Scale::MomentumUp: return "MomentumUp";
    case egEnergyCorr::Scale::MomentumDown: return "MomentumDown";
    case egEnergyCorr::Scale::ZeeStatUp: return "ZeeStatUp";
    case egEnergyCorr::Scale::ZeeStatDown: return "ZeeStatDown";
    case egEnergyCorr::Scale::ZeeSystUp: return "ZeeSystUp";
    case egEnergyCorr::Scale::ZeeSystDown: return "ZeeSystDown";
    case egEnergyCorr::Scale::ZeePhysUp: return "ZeePhysUp";
    case egEnergyCorr::Scale::ZeePhysDown: return "ZeePhysDown";
    case egEnergyCorr::Scale::ZeeAllUp: return "ZeeAllUp";
    case egEnergyCorr::Scale::ZeeAllDown: return "ZeeAllDown";
    case egEnergyCorr::Scale::LArCalibUp: return "LArCalibUp";
    case egEnergyCorr::Scale::LArCalibDown: return "LArCalibDown";
    case egEnergyCorr::Scale::LArUnconvCalibUp: return "LArUnconvCalibUp";
    case egEnergyCorr::Scale::LArUnconvCalibDown: return "LArUnconvCalibDown";
    case egEnergyCorr::Scale::LArElecCalibUp: return "LArElecCalibUp";
    case egEnergyCorr::Scale::LArElecCalibDown: return "LArElecCalibDown";
    case egEnergyCorr::Scale::LArCalibExtra2015PreUp: return "LArCalibExtra2015PreUp";
    case egEnergyCorr::Scale::LArCalibExtra2015PreDown: return "LArCalibExtra2015PreDown";
    case egEnergyCorr::Scale::LArElecUnconvUp: return "LArElecUnconvUp";
    case egEnergyCorr::Scale::LArElecUnconvDown: return "LArElecUnconvDown";
    case egEnergyCorr::Scale::G4Up: return "G4Up";
    case egEnergyCorr::Scale::G4Down: return "G4Down";
    case egEnergyCorr::Scale::PSUp: return "PSUp";
    case egEnergyCorr::Scale::PSDown: return "PSDown";
    case egEnergyCorr::Scale::PSb12Up: return "PSb12Up";
    case egEnergyCorr::Scale::PSb12Down: return "PSb12Down";
    case egEnergyCorr::Scale::S12Up: return "S12Up";
    case egEnergyCorr::Scale::S12Down: return "S12Down";
    case egEnergyCorr::Scale::S12ExtraLastEtaBinRun2Up: return "S12ExtraLastEtaBinRun2Up";
    case egEnergyCorr::Scale::S12ExtraLastEtaBinRun2Down: return "S12ExtraLastEtaBinRun2Down";
    case egEnergyCorr::Scale::MatIDUp: return "MatIDUp";
    case egEnergyCorr::Scale::MatIDDown: return "MatIDDown";
    case egEnergyCorr::Scale::MatCryoUp: return "MatCryoUp";
    case egEnergyCorr::Scale::MatCryoDown: return "MatCryoDown";
    case egEnergyCorr::Scale::MatCaloUp: return "MatCaloUp";
    case egEnergyCorr::Scale::MatCaloDown: return "MatCaloDown";
    case egEnergyCorr::Scale::L1GainUp: return "L1GainUp";
    case egEnergyCorr::Scale::L1GainDown: return "L1GainDown";
    case egEnergyCorr::Scale::L2GainUp: return "L2GainUp";
    case egEnergyCorr::Scale::L2GainDown: return "L2GainDown";
    case egEnergyCorr::Scale::LeakageUnconvUp: return "LeakageUnconvUp";
    case egEnergyCorr::Scale::LeakageUnconvDown: return "LeakageUnconvDown";
    case egEnergyCorr::Scale::LeakageConvUp: return "LeakageConvUp";
    case egEnergyCorr::Scale::LeakageConvDown: return "LeakageConvDown";
    case egEnergyCorr::Scale::ConvEfficiencyUp: return "ConvEfficiencyUp";
    case egEnergyCorr::Scale::ConvEfficiencyDown: return "ConvEfficiencyDown";
    case egEnergyCorr::Scale::ConvFakeRateUp: return "ConvFakeRateUp";
    case egEnergyCorr::Scale::ConvFakeRateDown: return "ConvFakeRateDown";
    case egEnergyCorr::Scale::ConvRadiusUp: return "ConvRadiusUp";
    case egEnergyCorr::Scale::ConvRadiusDown: return "ConvRadiusDown";
    case egEnergyCorr::Scale::PedestalUp: return "PedestalUp";
    case egEnergyCorr::Scale::PedestalDown: return "PedestalDown";
    case egEnergyCorr::Scale::AllUp: return "AllUp";
    case egEnergyCorr::Scale::AllDown: return "AllDown";
    case egEnergyCorr::Scale::AllCorrelatedUp: return "AllCorrelatedUp";
    case egEnergyCorr::Scale::AllCorrelatedDown: return "AllCorrelatedDown";
    case egEnergyCorr::Scale::LArTemperature2015PreUp: return "LArTemperature2015PreUp";
    case egEnergyCorr::Scale::LArTemperature2015PreDown: return "LArTemperature2015PreDown";
    case egEnergyCorr::Scale::LArTemperature2016PreUp: return "LArTemperature2016PreUp";
    case egEnergyCorr::Scale::LArTemperature2016PreDown: return "LArTemperature2016PreDown";
    case egEnergyCorr::Scale::E4ScintillatorUp: return "E4ScintillatorUp";
    case egEnergyCorr::Scale::E4ScintillatorDown: return "E4ScintillatorDown";
    case egEnergyCorr::Scale::MatPP0Up: return "MatPP0Up";
    case egEnergyCorr::Scale::MatPP0Down: return "MatPP0Down";
    case egEnergyCorr::Scale::Wtots1Up: return "Wtots1Up";
    case egEnergyCorr::Scale::Wtots1Down: return "Wtots1Down";
    case egEnergyCorr::Scale::LastScaleVariation: return "LastScaleVariation";
    default: return "Unknown";
    }
  }

  string egammaEnergyCorrectionTool::variationName(egEnergyCorr::Resolution::Variation& var) const {
    switch(var) {
      case egEnergyCorr::Resolution::None: return "Resolution::None";
      case egEnergyCorr::Resolution::Nominal: return "Resolution::Nominal";
      case egEnergyCorr::Resolution::AllDown: return "Resolution::AllDown";
      case egEnergyCorr::Resolution::AllUp: return "Resolution::AllUp";
      case egEnergyCorr::Resolution::ZSmearingUp: return "Resolution::ZSmearingUp";
      case egEnergyCorr::Resolution::ZSmearingDown: return "Resolution::ZSmearingDown";
      case egEnergyCorr::Resolution::SamplingTermUp: return "Resolution::SamplingTermUp";
      case egEnergyCorr::Resolution::SamplingTermDown: return "Resolution::SamplingTermDown";
      case egEnergyCorr::Resolution::MaterialIDUp: return "Resolution::MaterialUp";
      case egEnergyCorr::Resolution::MaterialIDDown: return "Resolution::MaterialDown";
      case egEnergyCorr::Resolution::MaterialCaloUp: return "Resolution::MaterialUp";
      case egEnergyCorr::Resolution::MaterialCaloDown: return "Resolution::MaterialDown";
      case egEnergyCorr::Resolution::MaterialCryoUp: return "Resolution::MaterialUp";
      case egEnergyCorr::Resolution::MaterialCryoDown: return "Resolution::MaterialDown";
      case egEnergyCorr::Resolution::MaterialGapUp: return "Resolution::MaterialUp";
      case egEnergyCorr::Resolution::MaterialGapDown: return "Resolution::MaterialDown";
      case egEnergyCorr::Resolution::PileUpUp: return "Resolution::PileUpUp";
      case egEnergyCorr::Resolution::PileUpDown: return "Resolution::PileUpDown";
      case egEnergyCorr::Resolution::MaterialPP0Up: return "Resolution::MaterialPP0Up";
      case egEnergyCorr::Resolution::MaterialPP0Down: return "Resolution::MaterialPP0Down";
      case egEnergyCorr::Resolution::MaterialIBLUp: return "Resolution::MaterialIBLUp";
      case egEnergyCorr::Resolution::MaterialIBLDown: return "Resolution::MaterialIBLDown";
      case egEnergyCorr::Resolution::af2Up: return "Resolution::af2Up";
      case egEnergyCorr::Resolution::af2Down: return "Resolution::af2Down";
      case egEnergyCorr::Resolution::LastResolutionVariation: return "LastResolutionVariation";
      default: return "Resolution::Unknown";
    }
  }


  double egammaEnergyCorrectionTool::get_ZeeSyst(double eta) const
  {
    const auto ieta = m_zeeSyst->GetXaxis()->FindFixBin(eta);
    auto value_histo = m_zeeSyst->GetBinContent(ieta);
 
    return value_histo;
  }

  const TAxis& egammaEnergyCorrectionTool::get_ZeeStat_eta_axis() const
  { 
    return *m_zeeNom->GetXaxis(); 
  }



} // egRescaler
