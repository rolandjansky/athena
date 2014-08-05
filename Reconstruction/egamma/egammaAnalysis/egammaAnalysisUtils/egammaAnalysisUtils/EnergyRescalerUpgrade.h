/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////
//
// REWRITE - May 2012, Maarten Boonekamp
//
///////////////////////////////////////////////////////////

#ifndef EnergyRescalerUpgrade_h
#define EnergyRescalerUpgrade_h

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <cmath>
#include <cctype>


#include <TRandom3.h>
#include <TFile.h>
#include <TGraphErrors.h>
#include <TH1D.h>
#include <TF1.h>


namespace egRescaler {

  // Taken from CLHEP/Units/SystemOfUnits.h
  static const double GeV = 1.e+3;
  
  
  class EnergyRescalerUpgrade {
    
  public:
    
    EnergyRescalerUpgrade();
    EnergyRescalerUpgrade(std::string filename, std::string year, std::string esmodel);
    virtual ~EnergyRescalerUpgrade();
    
    void Init(std::string filename, std::string year, std::string esmodel);

    // Resolution error variations    

    typedef enum { NOMINAL=0, ERR_DOWN=1, ERR_UP=2 } CorrType;
    
    // Scale error variations    

    enum Variation { 
      
      // central value
      Nominal, 
      
      // Zee scale uncertainty variations
      ZeeStatUp, ZeeStatDown, ZeeGenUp, ZeeGenDown, ZeeMethodUp, ZeeMethodDown, ZeeAllUp, ZeeAllDown,
      
      // PS and Material uncertainty variations
      PSStatUp, PSStatDown, R12StatUp, R12StatDown, B12Up, B12Down,
      
      // Low-pt
      LowPtUp, LowPtDown,

      // Leakage
      LeakageUp, LeakageDown, 

      // All
      AllUp, AllDown

    };
    

    // particle types
    
    enum ParticleType {
      
      Electron, 
      Unconverted, 
      Converted
      
    };

    // ES model

    enum ESModel {
      
      es2010,
      es2011,
      es2012

    };

    
    // take eta and uncorrected energy of electron, return  corrected energy, 
    // apply given variation, for given particle type
    // Note : energies in MeV
    
    double applyEnergyCorrection( double cl_eta, double energy, ParticleType ptype=Electron, Variation var=Nominal, double varSF=1., int runNumber=-999 );
    double applyEnergyCorrection( double cl_eta, ParticleType ptype=Electron, Variation var=Nominal, double varSF=1. );
    
    double getAlphaValue( double cl_eta, double energy, ParticleType ptype=Electron, Variation var=Nominal, double varSF=1., int runNumber=-999 );
    double getAlphaUncertainty( double cl_eta, double energy, ParticleType ptype=Electron, Variation var=Nominal, double varSF=1. );

    double getCorrectionUncertainty( double cl_eta, double energy, ParticleType ptype=Electron, Variation var=Nominal, double varSF=1. );

    // set properties

    void SetRandomSeed( unsigned seed=0 ) { m_random3.SetSeed(seed); };

    void SetDebugFlag( bool flag=false ) { m_debug=flag; };

    void includePresamplerContribution( bool flag=true ) { m_includePresamplerContribution = flag; };
    void includeMaterialContribution( bool flag=true ) { m_includeMaterialContribution = flag; };
    void includeLeakageContribution( bool flag=true ) { m_includeLeakageContribution = flag; };

    
    /// smearing corrections
    // Note : energies in MeV
    
    double getSmearingCorrection( double eta, double energy, int value=NOMINAL, bool mc_withCT=false );
    
    /// MC calibration corrections
    
    double applyMCCalibration(double eta, double ET, ParticleType ptype);
    double applyAFtoG4(double eta);
    double applyFStoG4(double eta);
    
    // functions for resolution uncertainty evaluation
    
    double mcSamplingTerm(double cl_eta);
    double mcSamplingTermRelError( double cl_eta );
    double mcNoiseTerm( double cl_eta );
    double mcConstantTerm( double cl_eta );

    double dataConstantTerm( double cl_eta );
    double dataConstantTermError( double cl_eta );

    double dataZPeakResolution( double cl_eta );
    double mcZPeakResolution( double cl_eta );
    double dataConstantTermCorError( double cl_eta ); 
    double fcn_sigma( double energy, double Cdata, double Cdata_er, double S, double S_er );
    void   resolutionError( double energy, double cl_eta, double& errUp, double& errDown );
    double resolution( double energy, double cl_eta, bool withCT );
    
    // functions for energy scale corrections

    double getZeeMeanET( double cl_eta );

    double getAlphaZee( double cl_eta, Variation var=Nominal, double varSF=1. );
    
    double getAlphaTimeDependence( double cl_eta, int runNumber );
    
    double getAlphaLowPt( double transverseEnergy, Variation var=Nominal, double varSF=1. );
    
    double getAlphaPS( double cl_eta, Variation var=Nominal, double varSF=1. );
    double getPSNonLinearity( double cl_eta, double energy, ParticleType ptype );
    
    double getR12( double cl_eta, Variation var=Nominal, double varSF=1. );
    double getAlphaMaterial( double cl_eta, ParticleType ptype );
    double getMaterialNonLinearity( double cl_eta, double energy, ParticleType ptype );
    
    double getLeakageNonLinearity( double cl_eta, double energy, ParticleType ptype, Variation var=Nominal, double varSF=1. );
    
    
    
  private:
    
    mutable TRandom3   m_random3;
    
    TFile* m_erFile;
    TH1D *m_zeeNom, *m_zeeModel, *m_zeeMethod;
    TH1D *m_resNom;
    TH1D *m_psNom, *m_psB12;
    TH1D *m_r12Nom, *m_r12B12;
    TH1D *m_peakResData, *m_peakResMC;

    TGraphErrors* m_eopEtaPosTimeDependence;
    TGraphErrors* m_eopEtaNegTimeDependence;
    
    TH1D*         m_psElectronEtaBins;
    TList*        m_psElectronGraphs;
    TH1D*         m_psUnconvertedEtaBins;
    TList*        m_psUnconvertedGraphs;
    TH1D*         m_psConvertedEtaBins;
    TList*        m_psConvertedGraphs;
    
    TGraphErrors* m_matElectronScale;
    TGraphErrors* m_matUnconvertedScale;
    TGraphErrors* m_matConvertedScale;
    
    TH1D*         m_matElectronEtaBins;
    TList*        m_matElectronGraphs;
    
    TH1D*         m_lkgElectronEtaBins;
    TList*        m_lkgElectronGraphs;
    
    TH1D*         m_G4OverAFII;
    TH1D*         m_G4OverFrSh;

    ESModel m_esmodel;

    bool m_includePresamplerContribution;
    bool m_includeMaterialContribution;
    bool m_includeLeakageContribution;

    bool m_initialized;
    bool m_debug;
    
#ifdef ROOTCORE
    ClassDef(EnergyRescalerUpgrade,1)
#endif
      
  };


} // end of namespace
#endif

