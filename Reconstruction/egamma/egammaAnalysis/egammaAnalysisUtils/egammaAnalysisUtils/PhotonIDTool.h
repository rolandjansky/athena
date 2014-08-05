/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhotonIDTool_h
#define PhotonIDTool_h

/* 
 *
 *  PhotonIDTool emulates the isEM() photons selections from
 *  HiggsAnalysisUtils::PhoonIdentificationTool(), and serve ad base
 *  class for study of other possible photon cuts.
 *
 */

#include <stdio.h>
#include <cmath>
using std::abs;
#include "PtEtaCollection.h"

static int m_dummy;

class PhotonIDTool 
{

 public:

  // initialize with basic EGamma variable (need to compute derived quantities)
  PhotonIDTool( double pt     = -999.,
		double eta2   = -999.,
		double ethad1 = -999.,
		double ethad  = -999.,
		double e277   = -999.,
		double e237   = -999.,
		double e233   = -999.,
		double weta2  = -999.,
		double f1     = -999.,
		double emax   = -999.,
		double emax2  = -999.,
		double emin2  = -999.,
		double fracm  = -999.,
		double wtot   = -999.,
		double w1     = -999.,
		int conv      = 0 );

  // directly initialize with derived variables
  PhotonIDTool( double pt     ,
		double eta2   ,
		double rhad1  ,
		double rhad   ,
		double e277   ,
		double reta   ,
		double rphi   ,
		double weta2  ,
		double f1     ,
		double fside  ,
		double wtot   ,
		double w1     ,
		double deltae ,
		double eratio ,
		int conv      );

  ~PhotonIDTool();

  // set discriminating variables
  void SetPt    ( double pt )    { m_pt     = pt;         ComputeDerivedQuantities(); }
  void SetEta2  ( double eta2 )  { m_eta2   = abs(eta2); }
  void SetEthad ( double ethad1, double ethad)  { m_ethad1 = ethad1; m_ethad = ethad; ComputeDerivedQuantities(); }
  void SetE277  ( double e277 )  { m_e277   = e277;       }
  void SetE237  ( double e237 )  { m_e237   = e237;       }
  void SetE233  ( double e233)   { m_e233   = e233;       }
  void SetWeta2 ( double weta2 ) { m_weta2  = weta2;      }
  void SetF1    ( double f1 )    { m_f1     = f1;         }
  void SetEmax1 ( double emax1 ) { m_emax1  = emax1;      ComputeDerivedQuantities(); }
  void SetEmax2 ( double emax2 ) { m_emax2  = emax2;      ComputeDerivedQuantities(); }
  void SetEmin2 ( double emin2 ) { m_emin2  = emin2;      ComputeDerivedQuantities(); }
  void SetFracm ( double fracm ) { m_fracm  = fracm;      }
  void SetWtot  ( double wtot )  { m_wtot   = wtot;       }
  void SetW1    ( double w1 )    { m_w1     = w1;         }
  void SetConv  ( int    conv)   { m_conv   = conv;       }

  // set derived quantities
  void SetReta  ( double reta   ){ m_Reta37 = reta; }
  void SetRphi  ( double rphi   ){ m_Rphi33 = rphi; }
  void SetDeltaE( double deltae ){ m_deltae = deltae; }
  void SetEratio( double eratio ){ m_eratio = eratio; }

  // isEM mask (loose + tight)
  unsigned int isEM(int tuneloose=1,int tunetight=1);

  // Photon cuts
  int  PhotonCutsBits(int itune=2);
  bool PhotonCutsBool(int itune=2);
  bool PhotonCutsParam(bool UseCutEta2      = true,
		       bool UseCutHad       = true,
		       bool UseCutS2ratio1  = true,
		       bool UseCutS2ratio2  = true,
		       bool UseCutS2weta2   = true,
		       bool UseCutS1f1      = true,
		       bool UseCutS1emax2r  = true,
		       bool UseCutS1deltae  = true,
		       bool UseCutS1wtot    = true,
		       bool UseCutS1fracm   = true,
		       bool UseCutS1w1      = true,
		       bool UseCutS1eratio  = false,
		       float FixPt  = -999.,
		       float FixEta = -999.,
		       int& CutEnabButFail=m_dummy);

  bool PhotonCutsFixPtEta( float FixPt  = -999., float FixEta = -999 );

  bool PhotonCutsLooseElectron(int& CutEnabButFail=m_dummy); // obsolete, replace with PhotonCutsLoose(1)

  bool PhotonCutsLoose(int tune=1, int& CutEnabButFail=m_dummy); 
  bool PhotonCutsMedium(int tune=1, int& CutEnabButFail=m_dummy); 
  bool PhotonCutsTight(int tune=1, int& CutEnabButFail=m_dummy);

  void SetRhadCuts   ( PtEtaCollection<double> rhad_cut )   { m_rhad_cut   = rhad_cut;   }  
  void SetRatio1Cuts ( PtEtaCollection<double> ratio1_cut ) { m_ratio1_cut = ratio1_cut; }
  void SetRatio2Cuts ( PtEtaCollection<double> ratio2_cut ) { m_ratio2_cut = ratio2_cut; }
  void SetWeta2Cuts  ( PtEtaCollection<double> weta2_cut )  { m_weta2_cut  = weta2_cut;  }
  void SetF1Cuts     ( PtEtaCollection<double> f1_cut_min,
		       PtEtaCollection<double> f1_cut_max ) 
  { 
    m_f1_cut_min = f1_cut_min;
    m_f1_cut_max = f1_cut_max;
  }
  void SetEmax2rCuts ( PtEtaCollection<double> emax2r_cut ) { m_emax2r_cut = emax2r_cut; }
  void SetDeltaeCuts ( PtEtaCollection<double> deltae_cut ) { m_deltae_cut = deltae_cut; }
  void SetWtotCuts   ( PtEtaCollection<double> wtot_cut )   { m_wtot_cut   = wtot_cut;   }
  void SetFracmCuts  ( PtEtaCollection<double> fracm_cut )  { m_fracm_cut  = fracm_cut;  }
  void Setw1Cuts     ( PtEtaCollection<double> w1_cut )     { m_w1_cut     = w1_cut;     }
  void SetEratioCuts ( PtEtaCollection<double> eratio_cut ) { m_eratio_cut = eratio_cut; }

  double GetRhadCut(float pt, float etas2) { return m_rhad_cut.Get(pt,fabs(etas2)); }
  double GetRetaCut(float pt, float etas2) { return m_ratio1_cut.Get(pt,fabs(etas2)); }
  double GetRphiCut(float pt, float etas2) { return m_ratio2_cut.Get(pt,fabs(etas2)); }
  double GetWeta2Cut(float pt, float etas2) { return m_weta2_cut.Get(pt,fabs(etas2)); }
  double GetWtotCut(float pt, float etas2) { return m_wtot_cut.Get(pt,fabs(etas2)); }
  double GetW1Cut(float pt, float etas2) { return m_w1_cut.Get(pt,fabs(etas2)); }
  double GetFsideCut(float pt, float etas2) { return m_fracm_cut.Get(pt,fabs(etas2)); }
  double GetDeltaeCut(float pt, float etas2) { return m_deltae_cut.Get(pt,fabs(etas2)); }
  double GetEratioCut(float pt, float etas2) { return m_eratio_cut.Get(pt,fabs(etas2)); }
  double GetF1Cut( /*float pt, float etas2*/ ) { return 0.005; }

  void LoadLooseCuts(int tune=1);
  void LoadMediumCuts(int tune=1);
  void LoadTightCuts(bool isConv=false, int tune=1);

  void SetVerbose(bool verbose=true){m_verbose=verbose;}

 private:

  void ComputeDerivedQuantities();

  // cuts as implemented in HiggsAnalysisUtils by Guillaume
  int cuts(int itune=0);
  bool CutHad_tune1();
  bool CutHad_tune2();
  bool CutHad_tune3();
  bool CutS2_tune1();
  bool CutS2_tune2();
  bool CutS2_tune3();
  bool CutS1_tune1();
  bool CutS1_tune2();
  bool CutS1_tune3();

  // parametric cuts (return 'true' if particle pass the cut)
  bool CutEta2     () { return ( m_eta2<1.37 || (m_eta2>1.52 && m_eta2<2.37) ) ; }

  bool CutHad ( double rhad_cut ) { 
    bool result = m_rhad <= rhad_cut ; 
    if (m_verbose)
      printf("Rhad:\t (%8.5f < %8.5f) => [%1d]\n",m_rhad,rhad_cut,result);
    return result;
  }

  bool CutS2ratio1 ( double ratio1_cut ) { 
    bool result = m_Reta37>=ratio1_cut ; 
    if (m_verbose)
      printf("Reta:\t (%8.5f > %8.5f) => [%1d]\n",m_Reta37,ratio1_cut,result);
    if (m_e277<0.1) 
      return false;
    else 
      return result;
  }

  bool CutS2ratio2 ( double ratio2_cut ) { 
    bool result = m_Rphi33>=ratio2_cut ; 
    if (m_verbose)
      printf("Rphi:\t (%8.5f > %8.5f) => [%1d]\n",m_Rphi33,ratio2_cut,result);
    //if (m_e237<0.1) 
    //  return false; 
    //else 
    return result;
  }

  bool CutS2weta2 ( double weta2_cut  ) { 
    bool result = m_weta2 <= weta2_cut; 
    if (m_verbose)
      printf("weta2:\t (%8.5f < %8.5f) => [%1d]\n",m_weta2,weta2_cut,result);
    return result;
  }

  bool CutS1f1 ( double f1_cut_min /*, double f1_cut_max */ ) { 
    bool result = ( m_f1 >= f1_cut_min ); // && m_f1 < f1_cut_max ) ; 
    return result;
  }

  bool CutS1emax2r ( double emax2r_cut ) { 
    bool result =  m_emax2r <= emax2r_cut;
    if (m_verbose)
      printf("emax2r:\t (%8.5f < %8.5f) => [%1d]\n",m_emax2r,emax2r_cut,result);
    return result;
  }

  bool CutS1deltae ( double deltae_cut ) {
    bool result = m_deltae <= deltae_cut;
    if (m_verbose)
      printf("deltae:\t (%8.5f < %8.5f) => [%1d]\n",m_deltae,deltae_cut,result);
    return result;
  }

  bool CutS1wtot   ( double wtot_cut   ) { 
    bool result = m_wtot <= wtot_cut;
    if (m_verbose)
      printf("wtot:\t (%8.5f < %8.5f) => [%1d]\n",m_wtot,wtot_cut,result);
    return result;
  }

  bool CutS1fracm  ( double fracm_cut  ) { 
    bool result = m_fracm <= fracm_cut;
    if (m_verbose)
      printf("fracm:\t (%8.5f < %8.5f) => [%1d]\n",m_fracm,fracm_cut,result);
    return result;
  }

  bool CutS1w1     ( double w1_cut     ) { 
    bool result = m_w1 <= w1_cut;
    if (m_verbose)
      printf("w1:\t (%8.5f < %8.5f) => [%1d]\n",m_w1,w1_cut,result);
    return result;
  }

  bool CutS1eratio ( double eratio_cut ) { 
    bool result =  m_eratio > eratio_cut; 
    if (m_verbose)
      printf("eratio:\t (%8.5f >= %8.5f) => [%1d]\n",m_eratio,eratio_cut,result);
    return result;
  }

  void LoadDefaultCuts(); // load tune2 thresholds for PhotonCutsParam()

  bool m_verbose;

  // discriminating variables
  double m_pt;
  double m_eta2;
  double m_ethad1;
  double m_ethad;
  double m_rhad;  // derived
  double m_e277;
  double m_e237;
  double m_e233;
  double m_Reta37; // derived
  double m_Rphi33; // derived
  double m_weta2;
  double m_f1;
  double m_emax1;
  double m_emax2;
  double m_emin2;
  double m_fracm;
  double m_wtot;
  double m_w1;
  double m_emax2r; // derived
  double m_deltae; // derived
  double m_eratio; // derived
  int m_conv;

  // collections of threshold for cuts
  PtEtaCollection<double> m_rhad_cut;
  PtEtaCollection<double> m_ratio1_cut;
  PtEtaCollection<double> m_ratio2_cut;
  PtEtaCollection<double> m_weta2_cut;
  PtEtaCollection<double> m_f1_cut_min;
  PtEtaCollection<double> m_f1_cut_max;
  PtEtaCollection<double> m_emax2r_cut;
  PtEtaCollection<double> m_deltae_cut;
  PtEtaCollection<double> m_wtot_cut;
  PtEtaCollection<double> m_fracm_cut;
  PtEtaCollection<double> m_w1_cut;
  PtEtaCollection<double> m_eratio_cut;

};

#endif // #ifdef PhotonIDTool_h

#ifdef PhotonIDTool_cxx

PhotonIDTool::PhotonIDTool(double pt,
			   double eta2,
			   double ethad1,
			   double ethad,
			   double e277,
			   double e237,
			   double e233,
			   double weta2,
			   double f1,
			   double emax1,
			   double emax2,
			   double emin2,
			   double fracm,
			   double wtot,
			   double w1,
			   int conv)
  : m_pt(pt),
    m_eta2(fabs(eta2)),
    m_ethad1(ethad1),
    m_ethad(ethad),
    m_e277(e277),
    m_e237(e237),
    m_e233(e233),
    m_weta2(weta2),
    m_f1(f1),
    m_emax1(emax1),
    m_emax2(emax2),
    m_emin2(emin2),
    m_fracm(fracm),
    m_wtot(wtot),
    m_w1(w1),
    m_conv(conv)
{
  this->ComputeDerivedQuantities();
  this->LoadDefaultCuts();
  m_verbose = false;
}

PhotonIDTool::PhotonIDTool( double pt     ,
			    double eta2   ,
			    double rhad1  ,
			    double rhad   ,
			    double e277   ,
			    double reta   ,
			    double rphi   ,
			    double weta2  ,
			    double f1     ,
			    double fside  ,
			    double wtot   ,
			    double w1     ,
			    double deltae ,
			    double eratio ,
			    int conv      )
: m_pt(pt),
  m_eta2(fabs(eta2)),
  m_ethad1(rhad1*pt),
  m_ethad(rhad*pt),
  m_e277(e277),
  m_Reta37(reta),
  m_Rphi33(rphi),
  m_weta2(weta2),
  m_f1(f1),
  //
  m_emax1(0.),
  m_emax2(0.),
  m_emin2(0.),
  //
  m_fracm(fside),
  m_wtot(wtot),
  m_w1(w1),
  //
  m_emax2r(0.),
  //
  m_deltae(deltae),
  m_eratio(eratio),
  m_conv(conv)
{
  m_e237 = m_Reta37 * m_e277;
  m_e233 = m_Rphi33 * m_e237;
  if (0.8<=m_eta2 && m_eta2<1.37) {
    m_rhad = rhad;
  } else {
    m_rhad = rhad1;
  }
  m_verbose = false;
}

PhotonIDTool::~PhotonIDTool()
{}

void PhotonIDTool::ComputeDerivedQuantities()
{
  if (0.8<=m_eta2 && m_eta2<1.37) {
    //m_rhad = m_ethad/m_pt;
    m_rhad = m_pt != 0. ? m_ethad/m_pt :  0.;
  } else {
    //m_rhad = m_ethad1/m_pt;
    m_rhad = m_pt != 0. ? m_ethad1/m_pt :  0.;
  }

  m_Reta37 = m_e277 != 0. ? m_e237/m_e277 :  0.;
  m_Rphi33 = m_e237 != 0. ? m_e233/m_e237 : 0.;

  //m_emax2r = m_emax2/(1.+0.009*(m_pt/1000)); // as implemented in old PAU photonID tool, looks wrong wrt egamma code.
  m_emax2r = m_emax2/(1000.+0.009*m_pt);

  m_deltae = m_emax2-m_emin2;  
  m_eratio = (m_emax1+m_emax2)==0. ? 0 : (m_emax1-m_emax2)/(m_emax1+m_emax2);
}

#endif // #ifdef PhotonIDTool_cxx
