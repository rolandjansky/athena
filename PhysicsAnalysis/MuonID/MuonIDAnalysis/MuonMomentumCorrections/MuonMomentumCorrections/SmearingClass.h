/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////
//           SmearingClass.h -- ATLAS Experiment Software                 //
////////////////////////////////////////////////////////////////////////////
///
/// class providing corrections to the simulated muon pT to match pT in data.
/// Resolution smearing and scale shift numbers are preliminary
/// 
/// responsible: atlas-perf-muons-conveners (at) cern.ch
///
#ifndef MuonMomentumCorrections_SmearingClass_H
#define MuonMomentumCorrections_SmearingClass_H

#include <cstring>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <TROOT.h>
#include <math.h>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <TRandom3.h>
#include <TLorentzVector.h>

#include "Regions.h"

namespace MuonSmear{
  /** Smearing and scaling types (enums are used for faster access) */
  typedef enum {
    SMEAR_PT=1, SMEAR_QPT=2
  } SMEARTYPE;
  typedef enum {
    SCALE_DEFAULT=1, SCALE_KPKM=2, SCALE_KC=3, SCALE_K=4, SCALE_C=5
  } SCALETYPE;
  typedef enum{
    DET_MS=1, DET_ID=2, DET_CB=3
  } DETTYPE;
  typedef enum{
    ID_UP=1, ID_LOW=2, MS_UP=3, MS_LOW=4,SCALE_UP=5, SCALE_LOW=6
  } SYSTTYPE;
  typedef enum{
    DATA10=1, DATA11=2,DATA12=3
  } DATATYPE;
  typedef enum{
    MUID=1, STACO=2, MUONS=3
  } ALGOTYPE;
  typedef enum{
    REL16_6=1, REL17=2, REL17_2=3, REL17_2_REPRO=4, REL17_2_SUM13=5
  } RELTYPE;

  const double defaultInit = -999.;  

  /** Smearing Class */
  class SmearingClass{
  
  public:
    /*Constructors*/
    SmearingClass();
    SmearingClass(std::string Data, std::string Algo="muons", 
		  std::string SmearType="q_pT",
		  std::string Rel="Rel17.2Sum13", std::string Dir="");
    SmearingClass(const SmearingClass&);
    virtual ~SmearingClass();
  
    /*************/
    /*  Methods  */   
    /*************/
      
    /** configure smearing **/
    void SetSeed(int);
    void SetSeed(int, int, int offset=680049);
    void UseTan2(bool);
    void UseGeV();
    void UseScale(bool);
    //Use the analytic ID resolution function developed by the Brandeis group: 
    //  see BrandeisMomentumResolution.h for additional details      
    void UseIDResoFunc(const bool useIDResoFunc_ = true) { useIDResoFunc = useIDResoFunc_; };
    //Use the energy loss correction for the MS tracks derived by Aleksandra.Dimitrievska@cern.ch, 
    // Nenad.Vranjes@cern.ch and Laurent.Chavalier@cern.ch with studies of JPsi decays.
    //NB: I need to multiply the correction for cosh(eta) (=sin(theta)) because EnLoss is parametrized for p and not for pT
    void UseEnLossMSCorr(const bool useEnLossMSCorr_ = true) {useEnLossMSCorr = useEnLossMSCorr_; };
    void RestrictCurvatureCorrections(double nsigma=2.5);
    void UseImprovedCombine();
    void ApplyToData(bool U=true);
    void SetAlgoSmearRelDir(std::string, std::string, std::string SmearType="q_pT", 
			    std::string Rel="Rel16.6", std::string Dir="");
    void FillScales(std::string Scaletype="KC", bool ApplyToData=false);
    void PrintValues();
 
    void Event(const TLorentzVector & p, std::string DetType, double Charge=0);
    /* Pt      -> Momentum of the muon in the given detector part.
       DetType -> Detector type: ID = inner detector.
	                         MS = muon spectrometer.
       Charge  -> Charge of the muon.
       Eta     -> Eta of the muon
       Phi     -> Phi of the muon
    */
    void Event(double Pt, double Eta, std::string DetType, double Charge=0,
               double Phi=defaultInit);
    void Event(const TLorentzVector & p_MS,
               const TLorentzVector & p_ID,
               const TLorentzVector & p_CB,
               double Charge=0);
    ///< \param p_MS    Stand-alone muon momentum, i.e MS momentum corrected
    ///<                for the energy loss in the calorimeters.
    ///< \param p_ID    Inner detector momentum.
    ///< \param p_CB    Combined muon momentum.
    ///< \param Charge  Charge of the muon.
    void Event(double PtMS, double PtID, double PtCB, double Eta,
               double Charge=0, double Phi=defaultInit);
    
    void Event();

    double ExpectedResolution(const std::string& DetType, const bool mc=false) const; //!< Expected resolution in data (or unsmeard MC if second argument is true)
    double ExpectedResolution(const int DetType, const bool mc=false) const; //!< Expected resolution in data (or unsmeard MC if second argument is true)
    
    double ScaleApply(const double pt, const double S1, const double S2, double S=1.0, const double S_EnLoss = 0.) const;

    double pTMS() const;
    double pTID() const;
    double pTCB() const;
  
    double ChargeFlip(const double) const;
    double ChargeFlipMS() const;
    double ChargeFlipID() const;
    double ChargeFlipCB() const;
  
    double SMS() const { return smearDeltaMS;};
    double SID() const { return smearDeltaID;};
    double SCB() const { return smearDeltaCB;};
 	      
    double VMS() const {return vms;};
    double VID() const {return vid;};
    double Corr()const {return corr;};
 
    void MSUP(double&); 
    void MSUP(double&, double&, double&);
    void MSLOW(double&);
    void MSLOW(double&, double&, double&);
    void IDUP(double&);
    void IDUP(double&, double&, double&);
    void IDLOW(double&);
    void IDLOW(double&, double&, double&);
  
    SYSTTYPE SelectFun(std::string);
    DATATYPE SetData(std::string);
    ALGOTYPE SetAlgorithm(std::string);
    RELTYPE  SetRelease(std::string);

    void PTVar(double&, std::string);
    void PTVar(double&, double&, double&, std::string);
  
    double Sign(const double x) const{ return (x<0? -1 : 1);};
  
    /** simple methods for retrieving input values 
	(not needed since all members are public) **/
    double ptms_orig()const {return ptms*GeV;};
    double ptid_orig()const {return ptid*GeV;};
    double ptcb_orig()const {return ptcb*GeV;};
    double ETA()const {return eta;};
    int DetRegion()const {return detRegion;};
    int GetScaleRegion();
    void AlwaysUseCombineFactors(const int NsigmaForICombine = 0){
	useNsigmaForICombine = NsigmaForICombine;
    };
    std::vector<double> getScale_ID()const {return m_scale_ID;};
    std::vector<double> getScaleSyst_ID()const {return m_scaleSyst_ID;};
    std::vector<double> getScale_MS()const {return m_scale_MS;};
    std::vector<double> getScaleSyst_MS()const {return m_scaleSyst_MS;};
    std::vector<double> getEnLoss_MS()const {return m_enLoss_MS;};
    std::vector<double> getEnLossSyst_MS()const {return m_enLossSyst_MS;};
    std::vector<double> getScale_CB()const {return m_scale_CB;};
    std::vector<double> getScaleSyst_CB()const {return m_scaleSyst_CB;};
  
    std::vector<double> getp1_ID()const {return m_p1_ID;};
    std::vector<double> getp2_ID()const {return m_p2_ID;};
    std::vector<double> getp2_ID_TAN()const {return m_p2_ID_TAN;};
    std::vector<double> getp0_MS()const {return m_p0_MS;};
    std::vector<double> getp1_MS()const {return m_p1_MS;};
    std::vector<double> getp2_MS()const {return m_p2_MS;};
  
    std::vector<double> getE_p1_ID()const {return m_E_p1_ID;};
    std::vector<double> getE_p2_ID()const {return m_E_p2_ID;};   
    std::vector<double> getE_p2_ID_TAN()const {return m_E_p2_ID_TAN;};   
    std::vector<double> getE_p0_MS()const {return m_E_p0_MS;};
    std::vector<double> getE_p1_MS()const {return m_E_p1_MS;};
    std::vector<double> getE_p2_MS()const {return m_E_p2_MS;};
				   
    std::vector<double> getS_p1_ID()const {return m_S_p1_ID;};
    std::vector<double> getS_p2_ID()const {return m_S_p2_ID;};
    std::vector<double> getS_p2_ID_TAN()const {return m_S_p2_ID_TAN;};   
    std::vector<double> getS_p0_MS()const {return m_S_p0_MS;};   
    std::vector<double> getS_p1_MS()const {return m_S_p1_MS;};   
    std::vector<double> getS_p2_MS()const {return m_S_p2_MS;};   
  
    std::vector<double> getMC_p1_ID()const {return m_MC_p1_ID;};
    std::vector<double> getMC_p2_ID()const {return m_MC_p2_ID;};   
    std::vector<double> getMC_p2_ID_TAN()const {return m_MC_p2_ID_TAN;}; 
    std::vector<double> getMC_p0_MS()const {return m_MC_p0_MS;};
    std::vector<double> getMC_p1_MS()const {return m_MC_p1_MS;};
    std::vector<double> getMC_p2_MS()const {return m_MC_p2_MS;};

    std::vector<double> getCorrMatC0()const {return m_CorrMatC0;};
    std::vector<double> getCorrMatC1()const {return m_CorrMatC1;};
    std::vector<double> getCorrMatC2()const {return m_CorrMatC2;};
    std::vector<double> getCorrMatC3()const {return m_CorrMatC3;};
    std::vector<double> getCorrMatC4()const {return m_CorrMatC4;};
    std::vector<double> getCorrMatC5()const {return m_CorrMatC5;};
  		      
    std::vector<double> getCorrMatTanC0()const {return m_CorrMatTanC0;};
    std::vector<double> getCorrMatTanC1()const {return m_CorrMatTanC1;};
    std::vector<double> getCorrMatTanC2()const {return m_CorrMatTanC2;};
    std::vector<double> getCorrMatTanC3()const {return m_CorrMatTanC3;};
    std::vector<double> getCorrMatTanC4()const {return m_CorrMatTanC4;};
    std::vector<double> getCorrMatTanC5()const {return m_CorrMatTanC5;};

#ifdef ROOTCORE
    ClassDef(SmearingClass,1)
#endif
  
  protected:
    bool m_CallSetClass;
    double m_pTmax;
    std::string m_DataYear;
    std::string m_Fdir;
    std::string m_Release;
    std::string m_ScaleType;
    SMEARTYPE Tsmear;
    SCALETYPE Tscale;
    DETTYPE   Tdet;
    SYSTTYPE  Tfun;
    DATATYPE  Tdata;
    RELTYPE   Trel;
    ALGOTYPE  Talgo;
    
    std::string m_Algorithm;

    /* overall scale correction */
    std::vector<double> m_scale_ID, m_enLoss_MS, m_scale_MS, m_scale_CB;
    //sys variations (stat error added in quadrature), one if it's simmetrized, 2 if Up != Dw.
    std::vector<double> m_scaleSyst_ID, m_enLossSyst_MS, m_scaleSyst_MS, m_scaleSyst_CB;
    //  Available only from Rel17.2Sum13 and for MS, ID.
    std::vector<double> m_scaleSystUp_ID, m_enLossSystUp_MS, m_scaleSystUp_MS;
    std::vector<double> m_scaleSystDw_ID, m_enLossSystDw_MS, m_scaleSystDw_MS;

    /* charge-dependent momentum corrections */
    std::vector<double> m_scaleBins;
    std::vector<double> m_S1_ID;
    std::vector<double> m_S2_ID;
    std::vector<double> m_S1_MS;
    std::vector<double> m_S2_MS;
    std::vector<double> m_S1_CB;
    std::vector<double> m_S2_CB;
    /* correlated and anti-correlated errors on the above */
    std::vector<double> m_S1Corr_ID;
    std::vector<double> m_S2Corr_ID;
    std::vector<double> m_S1Corr_MS;
    std::vector<double> m_S2Corr_MS;
    std::vector<double> m_S1Corr_CB;
    std::vector<double> m_S2Corr_CB;
    std::vector<double> m_S1ACorr_ID;
    std::vector<double> m_S2ACorr_ID;
    std::vector<double> m_S1ACorr_MS;
    std::vector<double> m_S2ACorr_MS;
    std::vector<double> m_S1ACorr_CB;
    std::vector<double> m_S2ACorr_CB;
    
    /* corrections on resolution parameters */
    std::vector<double> m_p1_ID, m_p2_ID, m_p2_ID_TAN,
	m_p0_MS, m_p1_MS, m_p2_MS;
    /* stat. errors on resolution parameters corrections */
    std::vector<double> m_E_p1_ID, m_E_p2_ID, m_E_p2_ID_TAN,
	m_E_p0_MS, m_E_p1_MS, m_E_p2_MS;
    // syst. errors on resolution parameters corrections:
    //one if it's simmetrized, then Stat and Sys err are separate in cfg file.
    std::vector<double> m_S_p1_ID, m_S_p2_ID, m_S_p2_ID_TAN,
	m_S_p0_MS, m_S_p1_MS, m_S_p2_MS;   
    //Two if Up != Dw, Stat and Sys err added in quadrature in cfg file.
    std::vector<double> m_SUp_p1_ID, m_SUp_p2_ID, m_SUp_p2_ID_TAN,
	m_SUp_p0_MS, m_SUp_p1_MS, m_SUp_p2_MS;   
    std::vector<double> m_SDw_p1_ID, m_SDw_p2_ID, m_SDw_p2_ID_TAN,
	m_SDw_p0_MS, m_SDw_p1_MS, m_SDw_p2_MS;   


    /* MC resolution parameters */
    std::vector<double> m_MC_p1_ID, m_MC_p2_ID, m_MC_p2_ID_TAN,
	m_MC_p0_MS, m_MC_p1_MS, m_MC_p2_MS;   
    /* correlation matrix */
    std::vector<double> m_CorrMatC0;
    std::vector<double> m_CorrMatC1;
    std::vector<double> m_CorrMatC2;
    std::vector<double> m_CorrMatC3;
    std::vector<double> m_CorrMatC4;
    std::vector<double> m_CorrMatC5;
    std::vector<double> m_CorrMatTanC0;
    std::vector<double> m_CorrMatTanC1;
    std::vector<double> m_CorrMatTanC2;
    std::vector<double> m_CorrMatTanC3;
    std::vector<double> m_CorrMatTanC4;
    std::vector<double> m_CorrMatTanC5;
  
  private:
    /*members*/
    //Original values of the input muon
    double ptms, ptid, ptcb, //Within the class these are used in GeV
	eta, phi, charge;
    double vms, vid, corr;
    double smearDeltaMS, smearDeltaID, smearDeltaCB;
    bool   useTan2;
    int detRegion;
    int scaleRegion;
    double GeV;
    double g0, g1, g2, g3, g4;
    bool useScale;
    bool useIDResoFunc;
    bool useEnLossMSCorr;
    double restrictCurvCorrSigma;
    std::ifstream InValues; //! don't persistify
    bool useImprovedCombine;
    bool apply_to_data;

    int  useNsigmaForICombine;

    //THIS FUNCTION NEED TO BE CAREFULLY REVIEWED, DOESN'T WORK AT THE MOMENT! +++++++++++
    bool useErrMatrix;
    void ErrorMatrix();

    static const bool dbg = false;
    static TRandom3* m_rand;

    Regions* DetRegions;
    int m_nb_regions;
    /* methods */
    void Initialize(std::string data, std::string algo, std::string smearType,
		    std::string rel, std::string dir);
    void Finalize();
    void Clean();
    void CleanScales();

    void FillValues();
  
    double Smearing(int);
    double SmearingSYSVar(int, double var);

    //Calculate the weights of ID and MS used for CB. Two ways are implemented: 
    // - weight by ^2 of exp reso
    // - from equation pT_CB = R(pT_ID) + (1-R)(pT_MS)
    //(weights should be correctly normalized)
    void CalcCBWeights();
    double weightMS, weightID;

    double pTCalc(const int DetType, 
		  const double inSmearID = defaultInit, const double inSmearMS = defaultInit,
		  const double scaleVar =0.) const;
    int getRegionOld(const double eta) const;
  };
}
#endif
