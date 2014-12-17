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

// ROOT includes
#include "TRandom3.h"
#include "TList.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TSystem.h"
#include "TF1.h"

// Forward declarations
class egammaMVACalib;
class egammaLayerRecalibTool;
struct StdCalibrationInputs;
class eg_resolution;
class get_MaterialResolutionEffect;
class e1hg_systematics;
namespace egGain { class GainTool; }

// Create a namespace for all needed enums
namespace egEnergyCorr {
  
  // Resolution error variations
  namespace Resolution {
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
  }
  
  
  // Scale error variations
  namespace Scale {
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
      ZeeStatUp, ZeeStatDown, ZeeSystUp, ZeeSystDown, ZeeAllUp, ZeeAllDown,
      
      // ... LAr systematics on scale and material determinations : correlated vs eta
      LArCalibUp, LArCalibDown, LArUnconvCalibUp, LArUnconvCalibDown, LArElecCalibUp, LArElecCalibDown, LArElecUnconvUp, LArElecUnconvDown, 

      // ... G4 systematics on E1/E2
      G4Up, G4Down,

      // ... Layer scale variations : data driven, uncorrelated vs eta
      PSUp, PSDown, S12Up, S12Down,

      // ... Material variations : data driven, uncorrelated vs eta
      MatIDUp, MatIDDown, MatCryoUp, MatCryoDown, MatCaloUp, MatCaloDown,

      // ... Gain correction
      L1GainUp, L1GainDown, L2GainUp, L2GainDown,
      
      // ... Pedestal
      PedestalUp, PedestalDown,


      // The following apply to photons only

      // ... Leakage
      LeakageUnconvUp, LeakageUnconvDown, LeakageConvUp, LeakageConvDown, 

      // ... Conversion efficiency (-> vary unconverted photon calib), fake rate (-> vary converted photon calib)
      ConvEfficiencyUp, ConvEfficiencyDown, ConvFakeRateUp, ConvFakeRateDown, ConvRadiusUp, ConvRadiusDown,


      // All (Only for error plotting - not correct when running over a sample!)
      AllUp, AllDown,

      // to help with loops
      LastScaleVariation

    };
  }
  
  
  // ES model

  enum ESModel {
    es2010,        // legacy
    es2011c,       // mc11c : faulty G4; old geometry
    es2011d,       // mc11d : corrected G4; new geometry == final Run1 scheme
    es2012a,       // mc12a : "crude" G4 fix; old geometry
    es2012c,       // mc12c : corrected G4; new geometry == final Run1 scheme
    es2015_day0_3percent,   // temporary for day0 run2
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
  
  class egammaEnergyCorrectionTool {
    
  public:
    // input struct
    struct ParticleInformation {
      float rawcl_Es0;
      float rawcl_Es1;
      float rawcl_Es2;
      float rawcl_Es3;
      float cl_eta;
      float cl_phi;
      float trk_eta;
      float cl_E;
      float cl_etaCalo;
      float cl_phiCalo;
      float ptconv;
      float pt1conv;
      float pt2conv;
      int convtrk1nPixHits;
      int convtrk1nSCTHits;
      int convtrk2nPixHits;
      int convtrk2nSCTHits;
      float Rconv;
      PATCore::ParticleType::Type ptype;
      
      ParticleInformation(float rawcl_Es0,
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
			  float Rconv)
	: rawcl_Es0(rawcl_Es0), rawcl_Es1(rawcl_Es1), rawcl_Es2(rawcl_Es2), rawcl_Es3(rawcl_Es3),
	  cl_eta(cl_eta), cl_phi(cl_phi), trk_eta(cl_eta), cl_E(cl_E), // cl_eta -> trk_eta
	  cl_etaCalo(cl_etaCalo), cl_phiCalo(cl_phiCalo),
	  ptconv(ptconv), pt1conv(pt1conv), pt2conv(pt2conv),
	  convtrk1nPixHits(convtrk1nPixHits), convtrk1nSCTHits(convtrk1nSCTHits),
	  convtrk2nPixHits(convtrk2nPixHits), convtrk2nSCTHits(convtrk2nSCTHits),
	  Rconv(Rconv), 
	  ptype((Rconv > 0 and Rconv < 800) ? PATCore::ParticleType::ConvertedPhoton : PATCore::ParticleType::UnconvertedPhoton) { } // I am a photon 

      ParticleInformation(float rawcl_Es0,
			  float rawcl_Es1,
			  float rawcl_Es2,
			  float rawcl_Es3,
			  float cl_eta,
			  float cl_phi,
			  float trk_eta,
			  float cl_E,
			  float cl_etaCalo,
			  float cl_phiCalo)
	: rawcl_Es0(rawcl_Es0), rawcl_Es1(rawcl_Es1), rawcl_Es2(rawcl_Es2), rawcl_Es3(rawcl_Es3),
	  cl_eta(cl_eta), cl_phi(cl_phi), trk_eta(trk_eta), cl_E(cl_E),
	  cl_etaCalo(cl_etaCalo), cl_phiCalo(cl_phiCalo),
	  ptconv(-999), pt1conv(-999), pt2conv(-999),
	  convtrk1nPixHits(-999), convtrk1nSCTHits(-999),
	  convtrk2nPixHits(-999), convtrk2nSCTHits(-999),
	  Rconv(-999), ptype(PATCore::ParticleType::Electron) { } // I am an electron
      
    };
    
  public:
    
    // Standard constructor and destructor
    //////////////////////////////////////

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
    
    // ... use/tweak Layer correction (set properly according to esmodel). !!! Switch at own risk !!!
    void useLayerCorrection( bool val ) { m_use_layer_correction = val; }
    void applyPSCorrection( bool val ) { m_applyPSCorrection = val; }
    void applyS12Correction( bool val ) { m_applyS12Correction = val; }

    // change layer 2 when calibrate E1/E2 (default = true)
    void useLayer2Recalibration( bool val ) { m_use_layer2_recalibration = val; }

    // ... use uniformity corrections (default = true )
    void useIntermoduleCorrection( bool val ) { m_use_intermodule_correction = val; }    
    void usePhiUniformCorrection( bool val ) { m_use_phi_unif_correction = val; }

    // ... use gain correction
    void useGainCorrection( bool val ) { m_use_gain_correction = val; }

    bool useIntermoduleCorrection() const { return m_use_intermodule_correction; }
    bool usePhiUniformCorrection() const { return m_use_phi_unif_correction; }
    bool useMVA ( ) const { return m_use_MVA_calibration; }
    bool useLayer2Recalibration ( ) const { return m_use_layer2_recalibration; }
    bool useGainCorrection ( ) const { return m_use_gain_correction; }

    // ... set a seed for the random number generator
    void setRandomSeed( unsigned seed=0 ) { m_random3.SetSeed(seed); }
    
    // ... Set output level to debug
    void setDebug( bool flag=false ) { m_debug=flag; }
    

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
			       double trk_eta,
			       double energy,
			       double energyS2,
			       egEnergyCorr::Scale::Variation scaleVar = egEnergyCorr::Scale::None,
			       egEnergyCorr::Resolution::Variation resVar = egEnergyCorr::Resolution::None,
                               egEnergyCorr::Resolution::resolutionType resType = egEnergyCorr::Resolution::SigmaEff90,
			       double varSF = 1.0 ) const;    

    double getCorrectedEnergy( unsigned int runnumber,
			       PATCore::ParticleDataType::DataType dataType,
			       const ParticleInformation & particle_info,
			       egEnergyCorr::Scale::Variation scaleVar = egEnergyCorr::Scale::None,
			       egEnergyCorr::Resolution::Variation resVar = egEnergyCorr::Resolution::None,
                               egEnergyCorr::Resolution::resolutionType resType = egEnergyCorr::Resolution::SigmaEff90,
			       double varSF = 1.0 ) const;
    
    double getCorrectedEnergy( unsigned int runnumber,
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
			       float ptconv = -999,
			       float pt1conv = -999 ,
			       float pt2conv = -999,
			       int convtrk1nPixHits = -999,
			       int convtrk1nSCTHits = -999,
			       int convtrk2nPixHits = -999,
			       int convtrk2nSCTHits = -999,
			       float Rconv = -999,
			       egEnergyCorr::Scale::Variation scaleVar = egEnergyCorr::Scale::None,
			       egEnergyCorr::Resolution::Variation resVar = egEnergyCorr::Resolution::None,
                               egEnergyCorr::Resolution::resolutionType resType = egEnergyCorr::Resolution::SigmaEff90,
			       double varSF = 1.0 ) const;
    
    double getCorrectedEnergyPhoton( unsigned int runnumber,
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
				     egEnergyCorr::Scale::Variation scaleVar = egEnergyCorr::Scale::None,
				     egEnergyCorr::Resolution::Variation resVar = egEnergyCorr::Resolution::None,
                                     egEnergyCorr::Resolution::resolutionType resType = egEnergyCorr::Resolution::SigmaEff90,
				     double varSF = 1.0 ) const;
    
    
    double getCorrectedEnergyElectron( unsigned int runnumber,
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
				       egEnergyCorr::Scale::Variation scaleVar = egEnergyCorr::Scale::None,
				       egEnergyCorr::Resolution::Variation resVar = egEnergyCorr::Resolution::None,
                                       egEnergyCorr::Resolution::resolutionType resType = egEnergyCorr::Resolution::SigmaEff90,
				       double varSF = 1.0 ) const;

    double resolution( double energy, double cl_eta, 
		       PATCore::ParticleType::Type ptype = PATCore::ParticleType::Electron, 
		       bool withCT=true,
                       egEnergyCorr::Resolution::resolutionType resType = egEnergyCorr::Resolution::SigmaEff90 ) const;
    
    // new for mc12c model. Return relative uncertainty on the resolution
    double getResolutionError(double energy, double eta,PATCore::ParticleType::Type ptype, egEnergyCorr::Resolution::Variation value,
                              egEnergyCorr::Resolution::resolutionType resType = egEnergyCorr::Resolution::Gaussian) const;

    
    StdCalibrationInputs* getCalibInputs() { return m_calibInputs; }
    float getCalibInputs(int i);

    std::string variationName(egEnergyCorr::Scale::Variation& var) const;
    std::string variationName(egEnergyCorr::Resolution::Variation& var) const;

    static egEnergyCorr::Scale::Variation ScaleVariationFromString(std::string& var);
    static egEnergyCorr::Resolution::Variation ResolutionVariationFromString(std::string& var);

  private:

    mutable egammaMVACalib* m_mva_electron_tool;                
    mutable egammaMVACalib* m_mva_photon_tool;                  
    mutable egammaLayerRecalibTool* m_layer_recalibration_tool;   
    mutable egGain::GainTool* m_gain_tool;                      
    mutable eg_resolution* m_resolution_tool;                   
    mutable get_MaterialResolutionEffect* m_getMaterialDelta;
    mutable e1hg_systematics* m_e1hg_tool;

    double getAlphaValue( double cl_eta, 
			  double energy,
			  double energyS2,
                          PATCore::ParticleType::Type ptype = PATCore::ParticleType::Electron,
                          egEnergyCorr::Scale::Variation var = egEnergyCorr::Scale::Nominal,     
                          double varSF = 1. ) const;

    double getAlphaUncertainty( double cl_eta, 
				double energy,
				double energyS2,
                                PATCore::ParticleType::Type ptype = PATCore::ParticleType::Electron,
                                egEnergyCorr::Scale::Variation var = egEnergyCorr::Scale::Nominal, 
                                double varSF = 1. ) const;
    
    
    /// smearing corrections
    // Note : energies in MeV
    
    double getSmearingCorrection( double eta, double energy,
				  PATCore::ParticleType::Type ptype = PATCore::ParticleType::Electron,
                                  egEnergyCorr::Resolution::Variation value = egEnergyCorr::Resolution::Nominal,
                                  egEnergyCorr::Resolution::resolutionType resType = egEnergyCorr::Resolution::SigmaEff90 ) const;
    
    /// MC calibration corrections
    
    double applyMCCalibration( double eta, double ET, PATCore::ParticleType::Type ptype ) const;
    double applyAFtoG4(double eta) const;
    double applyFStoG4(double eta) const;
    

    /// Phi uniformity corrections 

    static double IntermoduleCorrectionTool(double Ecl, double phi, double eta);
    static double CorrectionPhiUnif(double eta, double phi);
    
    // functions for resolution uncertainty evaluation
    
    // functions for old model
    double mcSamplingTerm(double cl_eta) const;
    double mcSamplingTermRelError( double cl_eta ) const;
    double mcNoiseTerm( double cl_eta ) const;
    double mcConstantTerm( double cl_eta ) const;

    // to access Z smearing and uncertainty
    double dataConstantTerm( double cl_eta ) const;
    double dataConstantTermError( double cl_eta ) const;

    // functions for old model
    double dataZPeakResolution( double cl_eta ) const;
    double mcZPeakResolution( double cl_eta ) const;
    double dataConstantTermCorError( double cl_eta ) const; 
    double fcn_sigma( double energy, double Cdata, double Cdata_er, double S, double S_er ) const;
    void   resolutionError( double energy, double cl_eta, double& errUp, double& errDown ) const;

    // functions for energy scale corrections

    double getZeeMeanET( double cl_eta ) const;

    double getAlphaZee( double cl_eta, egEnergyCorr::Scale::Variation var = egEnergyCorr::Scale::Nominal, double varSF = 1. ) const;
 
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

    double getAlphaPedestal(double cl_eta, double energy, PATCore::ParticleType::Type ptype, bool isRef, 
			    egEnergyCorr::Scale::Variation var = egEnergyCorr::Scale::Nominal, double varSF = 1. ) const;

    double getLayerPedestal(double cl_eta, PATCore::ParticleType::Type ptype, int iLayer,
			    egEnergyCorr::Scale::Variation var = egEnergyCorr::Scale::Nominal, double varSF = 1. ) const;

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
    void getResolution_systematics(int particle_type, double energy, double eta, int syst_mask, double& resolution, double& resolution_error,double& resolution_error_up, double & resolution_error_down, int resol_type=0) const;

    // approximate pileup noise contribution to the resolution
    double pileUpTerm(double eta, int particle_type) const;

  private:

    StdCalibrationInputs* m_calibInputs;
      
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
    TH1D*         m_zeeSyst;

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

    TH1D*         m_convRadius;
    TH1D*         m_convFakeRate;
    TH1D*         m_convRecoEfficiency;

    TH1D*         m_leakageConverted;
    TH1D*         m_leakageUnconverted;

    TH1D*         m_zeeES2Profile;

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
    
    TH1D*         m_G4OverAFII;
    TH1D*         m_G4OverFrSh;

    // Main ES model switch

    egEnergyCorr::ESModel m_esmodel;

    // general switches

    bool m_use_layer_correction;       // default = true for es2011d and es2012c; false otherwise
    bool m_use_MVA_calibration;        // default = true for es2011d and es2012c; false otherwise
    bool m_use_intermodule_correction; // default = true
    bool m_use_phi_unif_correction;    // default = true
    bool m_use_layer2_recalibration;   // default = true
    bool m_use_gain_correction;        // default = false; true only for es2012c

    // for tests
    bool m_applyPSCorrection;          // default = true
    bool m_applyS12Correction;          // default = true

    bool m_initialized;
    bool m_debug;
    
#ifdef ROOTCORE
    ClassDef(egammaEnergyCorrectionTool,1)
#endif
      
  };

} // end of namespace



#endif

