/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//////////////////////////////////////////////////////////////
//
// REWRITE - February 2013, Karsten Koeneke
//
///////////////////////////////////////////////////////////

#ifndef ELECTRONPHOTONFOURMOMENTUMCORRECTION_EGAMMAENERGYCORRECTIONTOOL_H
#define ELECTRONPHOTONFOURMOMENTUMCORRECTION_EGAMMAENERGYCORRECTIONTOOL_H

// STL includes
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <cmath>
#include <cctype>
#include <cstddef>

// PAT includes
#include "PATCore/PATCoreEnums.h"
#include <AsgTools/AsgMessaging.h>


// ROOT includes
#include "TRandom3.h"
#include "TList.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TSystem.h"
#include "TF1.h"

// Forward declarations
class eg_resolution;
class get_MaterialResolutionEffect;
class e1hg_systematics;
namespace egGain { class GainTool;            // run1 tool
                   class GainUncertainty;     // run2 tool
                 }

// Create a namespace for all needed enums
namespace egEnergyCorr {
  struct ROOT6_OpenNamespaceWorkaround { };  // workaround for reflex dict generation

  // Resolution error variations
  namespace Resolution {
    struct ROOT6_OpenNamespaceWorkaround { };  // workaround for reflex dict generation

    enum Variation {

      // ZSmearing,SamplingTerm,Material,PileUp only implemented for mc12c...

      // Nothing to be done
      None,

      // Nominal
      Nominal,

      // All (Only for error plotting - not correct when running over a sample!)
      AllDown, AllUp,

      // Z smearing uncertainty (symmetrized)
      ZSmearingDown, ZSmearingUp,

      // Sampling term uncertainty
      SamplingTermDown, SamplingTermUp,

      // Material uncertainty
      MaterialIDDown, MaterialIDUp, MaterialCaloDown, MaterialCaloUp, MaterialGapDown, MaterialGapUp, MaterialCryoDown, MaterialCryoUp,

      // Pileup uncertainty
      PileUpDown, PileUpUp,

      // IBL+PP0 for run 2
      MaterialIBLUp, MaterialIBLDown, MaterialPP0Up, MaterialPP0Down,


      // to help with loops
      LastResolutionVariation

    };

    // type of resolution parameterization
    enum resolutionType {
       // gaussian "core"
       Gaussian,
       // sigma_eff 80%
       SigmaEff80,
       // sigma_eff 90%
       SigmaEff90
    };

  } // End: namespace Resolution


  // Scale error variations
  namespace Scale {
    struct ROOT6_OpenNamespaceWorkaround { };  // workaround for reflex dict generation

    enum Variation {

      // Nothing to be done
      None,

      // central value
      Nominal,


      // This applies to electrons only

      // ... Momentum scale systematics
      MomentumUp, MomentumDown,


      // The following apply to electrons and photons

      // ... Zee scale uncertainty variations : Stat uncorrelated; Syst correlated vs eta
      ZeeStatUp, ZeeStatDown, ZeeSystUp, ZeeSystDown, ZeePhysUp, ZeePhysDown, ZeeAllUp, ZeeAllDown,

      // ... LAr systematics on scale and material determinations : correlated vs eta
      LArCalibUp, LArCalibDown, LArUnconvCalibUp, LArUnconvCalibDown, LArElecCalibUp, LArElecCalibDown, LArElecUnconvUp, LArElecUnconvDown,

      // extra systematics for 2015PRE*
      LArCalibExtra2015PreUp, LArCalibExtra2015PreDown,
      LArTemperature2015PreUp, LArTemperature2015PreDown,

      // extra systematics for 2015->2016 extrapolation
      LArTemperature2016PreUp, LArTemperature2016PreDown,

      // ... G4 systematics on E1/E2
      G4Up, G4Down,

      // scale for E4 TileGap3
      E4ScintillatorUp, E4ScintillatorDown,

      // ... Layer scale variations : data driven, uncorrelated vs eta
      PSUp, PSDown, S12Up, S12Down,

      // extra E12 for es2017 run2
      S12ExtraLastEtaBinRun2Up, S12ExtraLastEtaBinRun2Down,

      // ... Material variations : data driven, uncorrelated vs eta
      MatIDUp, MatIDDown, MatCryoUp, MatCryoDown, MatCaloUp, MatCaloDown,

      // ... Gain correction
      L1GainUp, L1GainDown, L2GainUp, L2GainDown,

      // ... Pedestal
      PedestalUp, PedestalDown,

      // ... wtots1
      Wtots1Up, Wtots1Down,

      // PP0
      MatPP0Up, MatPP0Down,

      // The following apply to photons only

      // ... Leakage
      LeakageUnconvUp, LeakageUnconvDown, LeakageConvUp, LeakageConvDown,

      // ... Conversion efficiency (-> vary unconverted photon calib), fake rate (-> vary converted photon calib)
      ConvEfficiencyUp, ConvEfficiencyDown, ConvFakeRateUp, ConvFakeRateDown, ConvRadiusUp, ConvRadiusDown,


      AllUp, AllDown,
      AllCorrelatedUp, AllCorrelatedDown,

      // to help with loops
      LastScaleVariation

    };

  } // End: namespace Scale


  // ES model

  enum ESModel {

    es2010,                 // legacy

    es2011c,                // mc11c : faulty G4; old geometry

    es2011d,                // mc11d : corrected G4; new geometry == final Run1 scheme
    es2011dMedium,          // mc11d : ditto, medium electrons, |eta|<2.47
    es2011dTight,           // mc11d : ditto, tight electrons, |eta|<2.47

    es2012a,                // mc12a : "crude" G4 fix; old geometry

    es2012c,                // mc12c : corrected G4; new geometry == final Run1 scheme
    es2012cMedium,          // mc12c : ditto, medium electrons, |eta|<2.47
    es2012cTight,           // mc12c : ditto, tight electrons, |eta|<2.47

    es2015_day0_3percent,   // temporary for day0 run2
    es2012XX,               // as es2012 + mc15 MVA calibration + new scales
    es2015PRE,              // as es2012 + mc15 MVA calibration + new scales + additional unc
    es2015PRE_res_improved,
    es2015cPRE,             // as 2015PRE but with new MVA calibration for crack for rel 20.7
    es2015cPRE_res_improved,
    es2015c_summer,         // data-driven for mc15c (to be used in summer 2016)
    es2016PRE,              // as es2015c_summer + temperature extrapolation
    es2017,                 // Moriond 2017
    es2017_summer,          // Summer 2017
    es2017_summer_improved, // Recommendations for Higgs mass paper
    es2017_R21_PRE,         // Pre-recommendations for release 21
    
    UNDEFINED

  };

  // Geometry dostortions

  enum Geometry {
    ConfigA=0,     // 5% ID material scaling
    ConfigCD,      // 10% services scaling
    ConfigEL,      // +7.5%X0 in SCT/TRT endcap; 10%X0, radial, in cryostat
    ConfigFMX,     // +7.5%X0 on ID endplate; 5%X0, radial, between PS and Strips
    ConfigGp       // all together
  };

  // Measured material categories

  enum MaterialCategory {
    MatID,         // ID material
    MatCryo,       // from ID to Presampler (|eta|<1.82), or Accordion (|eta|>1.82)
    MatCalo        // in calorimeter (between PS and Strips)
  };

} // End: namespace egEnergyCorr


namespace AtlasRoot {

  // Taken from CLHEP/Units/SystemOfUnits.h
  static const double GeV = 1.e+3;

  class egammaEnergyCorrectionTool : public asg::AsgMessaging {

  public:
    egammaEnergyCorrectionTool();
    virtual ~egammaEnergyCorrectionTool();

    // Mandatory setup functions
    ////////////////////////////

    // ... energy correction model to be used. To be called before initialize()
    void setESModel ( egEnergyCorr::ESModel val ){ m_esmodel = val; }

    // ... Initialize this tool with all internal parameters, depending on the previous user setup
    int initialize();

    // Optional
    ///////////

    // ... set input file
    inline void setFileName ( const std::string& val ){ m_rootFileName = val; }

    // ... set a seed for the random number generator
    void setRandomSeed( unsigned seed=0 ) { m_random3.SetSeed(seed); }

    void useStatErrorScaling(bool flag) { m_use_stat_error_scaling = flag; }

    void use_temp_correction201215(bool flag) { m_use_temp_correction201215 = flag; }
    void use_temp_correction201516(bool flag) { m_use_temp_correction201516 = flag; }
    void use_uA2MeV_2015_first2weeks_correction(bool flag) { m_use_uA2MeV_2015_first2weeks_correction = flag; }

    double applyMCCalibration( double eta, double ET, PATCore::ParticleType::Type ptype ) const;

    /** take eta and uncorrected energy of electron, return  corrected energy,
	apply given variation, for given particle type
	Note : energies in MeV
	This is the main method for users. It internally calls all other needed methods automatically */

    double getCorrectedMomentum( PATCore::ParticleDataType::DataType dataType,
				 PATCore::ParticleType::Type ptype,
				 double momentum,
				 double trk_eta,
				 egEnergyCorr::Scale::Variation scaleVar = egEnergyCorr::Scale::None,
				 double varSF = 1.0 ) const;

    double getCorrectedEnergy( unsigned int runnumber,
			       PATCore::ParticleDataType::DataType dataType,
			       PATCore::ParticleType::Type ptype,
			       double cl_eta,
			       double cl_etaCalo,
			       double energy,
			       double energyS2,
			       double eraw,
			       egEnergyCorr::Scale::Variation scaleVar = egEnergyCorr::Scale::None,
			       egEnergyCorr::Resolution::Variation resVar = egEnergyCorr::Resolution::None,
                               egEnergyCorr::Resolution::resolutionType resType = egEnergyCorr::Resolution::SigmaEff90,
			       double varSF = 1.0 ) const;


    double resolution(double energy, double cl_eta, double cl_etaCalo,
                      PATCore::ParticleType::Type ptype,
		      bool withCT,
		      bool fast,
                      egEnergyCorr::Resolution::resolutionType resType = egEnergyCorr::Resolution::SigmaEff90 ) const;

    // new for mc12c model. Return relative uncertainty on the resolution
    double getResolutionError(double energy, double eta, double etaCalo, PATCore::ParticleType::Type ptype, egEnergyCorr::Resolution::Variation value,
                              egEnergyCorr::Resolution::resolutionType resType = egEnergyCorr::Resolution::Gaussian) const;


    std::string variationName(egEnergyCorr::Scale::Variation& var) const;
    std::string variationName(egEnergyCorr::Resolution::Variation& var) const;

    static egEnergyCorr::Scale::Variation ScaleVariationFromString(std::string& var);
    static egEnergyCorr::Resolution::Variation ResolutionVariationFromString(std::string& var);

    // convenient method for decorrelation of statistical error
    const TAxis& get_ZeeStat_eta_axis() const { return *m_zeeNom->GetXaxis(); }

  private:
    // TODO: remove mutable
    mutable egGain::GainTool* m_gain_tool;                    // run 1
    egGain::GainUncertainty* m_gain_tool_run2;        // based on special run for run2
    mutable eg_resolution* m_resolution_tool;
    mutable get_MaterialResolutionEffect* m_getMaterialDelta;
    mutable e1hg_systematics* m_e1hg_tool;

    double getAlphaValue(long int runnumber, double cl_eta, double cl_etaCalo,
			 double energy, double energyS2, double eraw,
			 PATCore::ParticleType::Type ptype = PATCore::ParticleType::Electron,
			 egEnergyCorr::Scale::Variation var = egEnergyCorr::Scale::Nominal,
			 double varSF = 1. ) const;

    double getAlphaUncertainty(long int runnumber, double cl_eta, double cl_etaCalo,
			       double energy,
			       double energyS2,
			       double eraw,
			       PATCore::ParticleType::Type ptype = PATCore::ParticleType::Electron,
			       egEnergyCorr::Scale::Variation var = egEnergyCorr::Scale::Nominal,
			       double varSF = 1. ) const;


    /// smearing corrections
    // Note : energies in MeV

    double getSmearingCorrection( double eta, double etaCalo, double energy,
				  PATCore::ParticleType::Type ptype = PATCore::ParticleType::Electron,
				  PATCore::ParticleDataType::DataType dataType = PATCore::ParticleDataType::Full,
                                  egEnergyCorr::Resolution::Variation value = egEnergyCorr::Resolution::Nominal,
                                  egEnergyCorr::Resolution::resolutionType resType = egEnergyCorr::Resolution::SigmaEff90 ) const;

    /// MC calibration corrections


    double applyAFtoG4(double eta, PATCore::ParticleType::Type ptype) const;
    double applyFStoG4(double eta) const;

    // functions for resolution uncertainty evaluation

    // functions for old model
    double mcSamplingTerm(double cl_eta) const;
    double mcSamplingTermRelError( double cl_eta ) const;
    double mcNoiseTerm( double cl_eta ) const;
    double mcConstantTerm( double cl_eta ) const;

    // to access Z smearing and uncertainty
    double dataConstantTerm(double eta) const;
    double dataConstantTermError(double eta) const;

    // functions for old model
    double dataZPeakResolution( double cl_eta ) const;
    double mcZPeakResolution( double cl_eta ) const;
    double dataConstantTermCorError( double cl_eta ) const;
    double fcn_sigma( double energy, double Cdata, double Cdata_er, double S, double S_er ) const;
    void   resolutionError( double energy, double cl_eta, double& errUp, double& errDown ) const;

    // functions for energy scale corrections

    double getZeeMeanET( double cl_eta ) const;

    double getAlphaZee(long int runnumber, double eta, egEnergyCorr::Scale::Variation var = egEnergyCorr::Scale::Nominal, double varSF = 1. ) const;

    double getE4Uncertainty(double eta) const;
    double getE4NonLinearity(double cl_eta, double meanE, PATCore::ParticleType::Type) const;

    double getWtots1Uncertainty(double cl_eta, double energy, PATCore::ParticleType::Type ptype) const;

    double getLayerUncertainty( int iLayer, double cl_eta,
				egEnergyCorr::Scale::Variation var = egEnergyCorr::Scale::Nominal, double varSF=1. ) const;

    double getLayerNonLinearity( int iLayer, double cl_eta, double energy, PATCore::ParticleType::Type ptype ) const;

    double getDeltaX( double cl_eta, egEnergyCorr::MaterialCategory imat,
		      egEnergyCorr::Scale::Variation var = egEnergyCorr::Scale::Nominal ) const;

    double getAlphaMaterial( double cl_eta, egEnergyCorr::MaterialCategory imat, PATCore::ParticleType::Type ptype,
			     egEnergyCorr::Scale::Variation var = egEnergyCorr::Scale::Nominal, double varSF = 1. ) const;

    double getMaterialNonLinearity( double cl_eta, double energy, egEnergyCorr::MaterialCategory imat, PATCore::ParticleType::Type ptype,
				    egEnergyCorr::Scale::Variation var = egEnergyCorr::Scale::Nominal, double varSF = 1. ) const;

    double getAlphaLeakage(double cl_eta, PATCore::ParticleType::Type ptype,
			   egEnergyCorr::Scale::Variation var = egEnergyCorr::Scale::Nominal, double varSF = 1. ) const;

    double getAlphaConvSyst(double cl_eta, double energy, PATCore::ParticleType::Type ptype,
			    egEnergyCorr::Scale::Variation var = egEnergyCorr::Scale::Nominal, double varSF = 1. ) const;

    double getAlphaPedestal(double cl_eta, double energy, double eraw, PATCore::ParticleType::Type ptype, bool isRef,
			    egEnergyCorr::Scale::Variation var = egEnergyCorr::Scale::Nominal, double varSF = 1. ) const;

    double getLayerPedestal(double cl_eta, PATCore::ParticleType::Type ptype, int iLayer,
			    egEnergyCorr::Scale::Variation var = egEnergyCorr::Scale::Nominal, double varSF = 1. ) const;
    double get_ZeeSyst(double eta) const;
    bool isInCrack( double cl_eta ) const;
    double nearestEtaBEC( double cl_eta ) const;

 /** @brief get resolution and its uncertainty)
     @brief particle type : 0=electron, 1=reco unconverted photon, 2=reco converted photon
     @brief energy = Energy in MeV
     @brief eta
     @brief syst_mask bit mask of systematics to consider (0x1 = smearing uncertainty, 0x2= intrisinc resolution uncertainty, 0x4 = ID material,  0x8 = PS-layer1 material, 0x10 = Material in barrel-endcap gap, 0x20 = Material in "cryo area", 0x40 = Pileup noise uncertainty)
     @brief  Output : resolution = energy resolution in MeV
     @brief  Output : resolution_error = uncertainty on energy resolution in MeV from the systematics included according to bit mask
     @brief resolution_type 0=gaussian core, 1= sigma eff 80%, 2 = sigma eff 90%
  */
    void getResolution_systematics(int particle_type, double energy, double eta, double etaCalo, int syst_mask, double& resolution, double& resolution_error,double& resolution_error_up, double & resolution_error_down, int resol_type=0) const;

    // approximate pileup noise contribution to the resolution
    double pileUpTerm(double eta, int particle_type) const;

  private:


    TFile* m_rootFile;
    std::string m_rootFileName;

    mutable TRandom3   m_random3;

    unsigned int  m_begRunNumber;
    unsigned int  m_endRunNumber;

    TH1D*         m_trkSyst;

    TH1D*         m_aPSNom;
    TH1D*         m_daPSCor;
    TH1D*         m_aS12Nom;
    TH1D*         m_daS12Cor;

    TH1D*         m_zeeNom;
    TH1D*         m_zeeNom_data2015;

    TH1D*         m_zeeSyst;
    TH1D*         m_zeePhys;
    TH1*          m_uA2MeV_2015_first2weeks_correction;

    TH1D*         m_resNom;
    TH1D*         m_resSyst;
    TH1D*         m_peakResData;
    TH1D*         m_peakResMC;

    TH1D*         m_dX_ID_Nom;

    TH1D*         m_dX_IPPS_Nom;
    TH1D*         m_dX_IPPS_LAr;

    TH1D*         m_dX_IPAcc_Nom;
    TH1D*         m_dX_IPAcc_G4;
    TH1D*         m_dX_IPAcc_LAr;
    TH1D*         m_dX_IPAcc_GL1;

    TH1D*         m_dX_PSAcc_Nom;
    TH1D*         m_dX_PSAcc_G4;
    TH1D*         m_dX_PSAcc_LAr;

    TAxis*        m_psElectronEtaBins;
    TList*        m_psElectronGraphs;
    TAxis*        m_psUnconvertedEtaBins;
    TList*        m_psUnconvertedGraphs;
    TAxis*        m_psConvertedEtaBins;
    TList*        m_psConvertedGraphs;

    TAxis*        m_E4ElectronEtaBins = nullptr;
    TList*        m_E4ElectronGraphs = nullptr;
    TAxis*        m_E4UnconvertedEtaBins = nullptr;
    TList*        m_E4UnconvertedGraphs = nullptr;
    TAxis*        m_E4ConvertedEtaBins = nullptr;
    TList*        m_E4ConvertedGraphs = nullptr;

    TAxis*        m_s12ElectronEtaBins;
    TList*        m_s12ElectronGraphs;
    TAxis*        m_s12UnconvertedEtaBins;
    TList*        m_s12UnconvertedGraphs;
    TAxis*        m_s12ConvertedEtaBins;
    TList*        m_s12ConvertedGraphs;

    TH1D*         m_pedestalL0;
    TH1D*         m_pedestalL1;
    TH1D*         m_pedestalL2;
    TH1D*         m_pedestalL3;

    TH1F*         m_pedestals_es2017;

    TH1D*         m_convRadius;
    TH1D*         m_convFakeRate;
    TH1D*         m_convRecoEfficiency;

    TH1D*         m_leakageConverted;
    TH1D*         m_leakageUnconverted;

    TH1D*         m_zeeES2Profile;

    TH2D*         m_pp0_elec;
    TH2D*         m_pp0_unconv;
    TH2D*         m_pp0_conv;

    TH1D*         m_wstot_slope_A_data;
    TH1D*         m_wstot_slope_B_MC;
    TH1D*         m_wstot_40GeV_data;
    TH1D*         m_wstot_40GeV_MC;
    TH1D*         m_wstot_pT_data_p0_electrons;
    TH1D*         m_wstot_pT_data_p1_electrons;
    TH1D*         m_wstot_pT_data_p0_unconverted_photons;
    TH1D*         m_wstot_pT_data_p1_unconverted_photons;
    TH1D*         m_wstot_pT_data_p0_converted_photons;
    TH1D*         m_wstot_pT_data_p1_converted_photons;
    TH1D*         m_wstot_pT_MC_p0_electrons;
    TH1D*         m_wstot_pT_MC_p1_electrons;
    TH1D*         m_wstot_pT_MC_p0_unconverted_photons;
    TH1D*         m_wstot_pT_MC_p1_unconverted_photons;
    TH1D*         m_wstot_pT_MC_p0_converted_photons;
    TH1D*         m_wstot_pT_MC_p1_converted_photons;
    
    // Geometry distortion vectors (to be ordered as in the the Geometry enum!)

    std::vector<TH1D*> m_matElectronScale;
    std::vector<TH1D*> m_matUnconvertedScale;
    std::vector<TH1D*> m_matConvertedScale;
    std::vector<TH1D*> m_matElectronCstTerm;
    std::vector<TH1D*> m_matX0Additions;

    // Non-linearity graphs

    TAxis*              m_matElectronEtaBins;
    std::vector<TList*> m_matElectronGraphs;

    // Fastsim -> Fullsim corrections

    TH1D*         m_G4OverAFII_electron;
    TH1D*         m_G4OverAFII_converted;
    TH1D*         m_G4OverAFII_unconverted;
    TH1D*         m_G4OverFrSh;

    TH2F* m_G4OverAFII_resolution_electron;
    TH2F* m_G4OverAFII_resolution_unconverted;
    TH2F* m_G4OverAFII_resolution_converted;

    // Main ES model switch

    egEnergyCorr::ESModel m_esmodel;

    // general switches

    bool m_use_etaCalo_scales;         // true for >= es2012XX

    // for tests
    bool m_applyPSCorrection;          // default = true
    bool m_applyS12Correction;          // default = true

    bool m_initialized;
    bool m_use_new_resolution_model;
    bool m_use_stat_error_scaling;  // default = false

    bool m_use_temp_correction201215;  // default = true (used only for es2015PRE)
    bool m_use_temp_correction201516;
    bool m_use_uA2MeV_2015_first2weeks_correction; // default = true (used only for es2105PRE)
  };

} // end of namespace



#endif
