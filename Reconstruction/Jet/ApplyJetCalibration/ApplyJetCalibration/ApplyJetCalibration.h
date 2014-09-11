/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 /*
 *  Dag Gillberg, Nov 2011
 */

#ifndef _APPLYJETCALIBRATION_
#define _APPLYJETCALIBRATION_

#include <iostream>
#include <TROOT.h>
#include <TString.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TEnv.h>
#include <TLorentzVector.h>
#include <TAxis.h>
#include <cmath>
#include <vector>
#include <TH2D.h>
#include <TFile.h>
#include <TRandom3.h>

#include "GSC.h"
#include "NPVBeamspotCorrection.h"
#include "KojiOriginCorrection.h"

namespace JetAnalysisCalib {
  class JetCalibrationTool : public TNamed {
    
  public:
    // Constructor
    JetCalibrationTool(TString jetAlgo, //="AntiKt4TopoEM", 
		       TString JESsettingFile="CalibrationConfigs/InsituJES_2011_Preliminary.config",
		       bool isData=true, TString dir="");
    
    JetCalibrationTool(); //for persistification, can be used also with subsequent call to init()
    
    virtual ~JetCalibrationTool();
    
#ifdef APPLYJETCALIB_STANDALONE
    ClassDef(JetCalibrationTool,1);
#endif
    
  private:
    typedef std::vector<double> VecD;
    typedef std::vector<TString> StrV;
    typedef unsigned int uint;
    enum interpol { cut_corners, gaus, none };
    interpol _interpol;
    
    void error(TString msg) 
    { 
      std::cerr << Form("\nERROR - JetCalibrationTool:\n\n  %s\n\n",msg.Data()) << std::endl;
      std::cout << Form("\nERROR - JetCalibrationTool:\n\n  %s\n\n",msg.Data()) << std::endl;
      abort();
    }
    
    TString FindFile(TString fn);
    bool FileExist(TString fn) { return gSystem->AccessPathName(gSystem->ExpandPathName(fn.Data()))==false; }
    
  public:
    void init(TString jetAlgo, TString JESfile, bool isData);
    
    // Returns the offset ET correction (in GeV)
    double GetOffset(double eta_det, double mu, double NPV,
		     double mu_ref=-99, double NPV_ref=-99);
    double GetResidualOffset(double eta_det, double mu, double NPV);
    // Returns the offset scale factor
    double GetOffsetSF(double pT_det, double eta_det, double mu, double NPV,
		       double mu_ref=-99, double NPV_ref=-99);
    double GetResidualOffsetSF(double pT_det, double eta_det, double mu, double NPV);
    double GetOffsetScaleFactor(double pT_det, double offsetET);
    
    double GetJES(double E_uncorr, double eta_det);
    double GetLowPtJES(double E_uncorr, double eta_det);
    double GetEtaCorr(double Ecorr, double eta_det);
    double GetMassCorr(double Ecorr, double eta_det);
    double GetNPVBeamspotCorrection(double NPV);
    int GetNPVcorrApprox(int NPV, int eventNumber = -99);
    int GetNPVcorrApprox2(int NPV, int eventNumber = -99);
    bool JetAreaJES() { return _resOffsetBins != NULL; }
    double MuScaleFactor() { return _muSF; }
    double MuForPlotting(double mu) { return _isData ? mu/_muSF : mu; }
    
    double GetGSC( TLorentzVector jet, double eta_det,
		   double trackWIDTH, double nTrk, double Tile0, double EM3, int Nsegments);
    
    TLorentzVector ApplyJES( double E_uncorr, double eta_det, double eta, 
			     double phi, double mass_uncorr );
    
    TLorentzVector ApplyEtaJES( double E_uncorr, double eta_det, 
				double eta, double phi, double mass_uncorr );
    
    TLorentzVector ApplyEtaMassJES( double E_uncorr, double eta_det,
				    double eta, double phi, double mass_uncorr);
    
    TLorentzVector ApplyOffsetEtaJES( double E_uncorr, double eta_det, 
				      double eta, double phi, double mass_uncorr,
				      double mu, double NPV, double MUref=-99, double NPVref=-99);
    
    TLorentzVector ApplyOffsetEtaJESGSC( double E_uncorr, double eta_det,
					 double eta, double phi, double mass_uncorr,
					 double trackWIDTH, double nTrk, double Tile0, double EM3, double Nsegments,
					 double mu, double NPV);

    
    TLorentzVector ApplyJetAreaOffsetEtaJESGSC( double E_uncorr, double eta_det, double phi, double mass_uncorr,
						double Ax, double Ay, double Az, double Ae, double rho,
						double trackWIDTH, double nTrk, double Tile0, double EM3, double Nsegments,
						double mu, double NPV);

    TLorentzVector ApplyJetAreaOffsetOriginEtaJESGSC( double E_uncorr, double eta_det, double phi, double mass_uncorr,
						      double Ax, double Ay, double Az, double Ae, double rho,
						      TVector3 PV, int samplingMax,
						      double trackWIDTH, double nTrk, double Tile0, double EM3, double Nsegments,
						      double mu, double NPV);

    TLorentzVector ApplyJetAreaOffsetOriginEtaJESGSC( double E_uncorr, double eta_det, double phi,
						      double eta_origin, double phi_origin, double m_origin,
						      double Ax, double Ay, double Az, double Ae, double rho,
						      double trackWIDTH, double nTrk, double Tile0, double EM3, double Nsegments,
						      double mu, double NPV);
    
    TLorentzVector ApplyJetAreaOffset( double E_det, double eta_det, double phi_det, double m_det,
				       double Ax, double Ay, double Az, double Ae, double rho,
				       double mu, double NPV);
    
    TLorentzVector ApplyJetAreaOffsetOrigin( double E_det, double eta_det, double phi_det, 
					     double eta_origin, double phi_origin, double m_origin,
					     double Ax, double Ay, double Az, double Ae, double rho,
					     double mu, double NPV);
    
    TLorentzVector ApplyJetAreaOffsetEtaJES( double E_det, double eta_det, double phi_det, double m_det,
					     double Ax, double Ay, double Az, double Ae, double rho,
					     double mu, double NPV);
    
    TLorentzVector ApplyJetAreaOffsetOriginEtaJES( double E_det, double eta_det, double phi_det,
						   double eta_origin, double phi_origin, double m_origin,
						   double Ax, double Ay, double Az, double Ae, double rho,
						   double mu, double NPV);
    
    TLorentzVector ApplyOffset( double E_uncorr, double eta_det, 
				double eta, double phi, double mass_uncorr,
				double mu, double NPV, double MUref=-99, double NPVref=-99);
    
    TLorentzVector ApplyBadChnOffsetEtaJES( double E_uncorr, double eta_det, 
					    double eta, double phi, double mass_uncorr,
					    double bch_corr_cell, double bch_corr_jet,
					    double mu, double NPV, double MUref=-99, double NPVref=-99);

    TLorentzVector ApplyKojiOriginCorrection( const TLorentzVector injet, const TVector3 PV, int samplingMax );
    TLorentzVector ApplyKojiOriginCorrection( const TLorentzVector injet, const TVector3 PV, int samplingMax, double eta_det );

    inline TString AbsoluteJES_Description() { return _jesDesc; };
    inline TString OffsetCorr_Description()  { return _offsetDesc; };
    inline TString ResidualOffsetCorr_Description()  { return _resOffsetDesc; };
    
    void SetInterpolation(TString interpolStr);
    
    void SetMuScaleFactor(double muScaleFactor) { _muSF = muScaleFactor; }

    double GetInsituCorr(double pt, double eta);
    

    void UseGeV(bool useGeV=true) { if (useGeV) _GeV=1; else _GeV=1000; }

  private:
    double GetLogPolN(const double *factors, double x);
    double GetLogPolNSlope(const double *factors, double x);
    double GetLogPolN2ndDeriv(const double *factors, double x);
  
    double GetOffsetPt(double aeta, double mu, double NPV,
		       VecD OffsetMu, VecD OffsetNPV, TAxis *OffsetBins);

    int GetEtaBin(double eta_det);

    TH1 *GetHisto(TFile *file, TString hname) {
      TH1 *h = (TH1*)file->Get(hname);
      if (h==NULL)
	printf("WARNING: Cannot access histogram \"%s\" in file %s",
	       hname.Data(),file->GetName());
      return h;
    }
    
    TH2D *CombineCalibration(TH2D *h2d, TH1D *h);
    
    int GetOffsetEtaBin(double eta_det);
    
    StrV Vectorize(TString txt, TString sep=" ");
    VecD VectorizeD(TString txt, TString sep=" ");
    
    // 90 eta bins, and up to 7 parameter for the pol-fit
    const static uint NetaBins=90;
    const static uint Npar=7;
    double JES_factors[NetaBins][Npar];
    double JES_minPt_slopes[NetaBins];
    double JES_minPt_E[NetaBins];
    double JES_minPt_R[NetaBins];
    double JES_minPt_Rmin[NetaBins];
    double JES_minPt_Param1[NetaBins];
    double JES_minPt_Param2[NetaBins];
    double etaCorr_factors[NetaBins][Npar];
    double JMS_factors[NetaBins][Npar];
    
    TAxis *_etaBinAxis;
    
    // Calibration function constraints
    double _minPt_JES, _minPt_EtaCorr, _maxE_EtaCorr;
    TString _jesDesc;
    int _lowPtextrap;
    double _lowPtMinR;
    
    double _mu_ref, _NPV_ref;
    TAxis *_offsetBins, *_resOffsetBins;
    VecD _offsetSlopes, _offsetIntercepts;
    VecD _resOffsetMu, _resOffsetNPV;
    TString _offsetDesc, _resOffsetDesc;
    bool _offsetNewParameterization;
    
    bool _isData;
    bool _applyOffsetCorrection; // whether or not to apply the pile-up correction
    bool _applyMassCorrection;
    TString _basePath;
    double _GeV;
    
    TH2D *_insituCorr;
    double _insituEtaMax, _insituPtMin, _insituPtMax;
    bool _useFullJetArea4vectorCorr;
    
    GSCTool *_gsc;
    bool _applyGSCCorrection;
    TString _GSCDepthString;
    int _GSCDepth;
    
    NPVBeamspotCorrectionTool *_npv;
    bool _applyNPVBeamspotCorrection;
    double _muSF;

    KojiOriginCorrectionTool *_kojiorigin;
    bool _applyKojiOriginCorrection;
    
    TRandom3 *_rand;
  };
}
#endif
