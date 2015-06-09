/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FudgeMCTool_h
#define FudgeMCTool_h

/* @author Martin Tripiana  <Martin.Tripiana@cern.ch>  - IFLP(UNLP-CONICET)
 *
 *  FudgeMCTool provides a collection of fudge factors for different
 *  preselections and performs the fudge of shower variables for
 *  any of these sets.
 *
 *  For further details please check README file or go to 
 *  https://twiki.cern.ch/twiki/bin/view/AtlasProtected/PhotonFudgeFactors
 *
 */

#include <iostream>
#include <stdio.h>
#include <cmath>
using std::abs;
#include "PtEtaCollection.h"

#include "TH1F.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"

//static int m_dummy;
namespace IDVAR
{
	const int RHAD1  = 0;
	const int RHAD   = 1;
	const int E277   = 2;
	const int RETA   = 3;
	const int RPHI   = 4;
	const int WETA2  = 5;
	const int F1     = 6;
	const int FSIDE  = 7;
	const int WTOT   = 8;
	const int W1     = 9;
	const int DE     = 10;
	const int ERATIO = 11;
}

class FudgeMCTool 
{

 public:

  // initialize with pt and eta2 of candidate to be fudged/of FF to be required.
	FudgeMCTool( double pt     = -999.,
				double eta2   = -999.,
				int isConv        = -1 ,
				int preselection  = -1 );
	
  ~FudgeMCTool();

  //fudge shower shapes for preselection chosen.
  void FudgeShowers( double pt     ,
		     double eta2   ,
		     double& rhad1  ,
		     double& rhad   ,
		     double& e277   ,
		     double& reta   ,
		     double& rphi   ,
		     double& weta2  ,
		     double& f1     ,
		     double& fside  ,
		     double& wtot   ,
		     double& w1     ,
		     double& deltae ,
		     double& eratio ,
		     int    isConv  ,
		     int    preselection=-999);

  //fudge shower shapes for preselection chosen.
  void FudgeShowers( float pt     ,
		     float eta2   ,
		     float& rhad1  ,
		     float& rhad   ,
		     float& e277   ,
		     float& reta   ,
		     float& rphi   ,
		     float& weta2  ,
		     float& f1     ,
		     float& fside  ,
		     float& wtot   ,
		     float& w1     ,
		     float& deltae ,
		     float& eratio ,
		     int    isConv  ,
		     int    preselection=-999);
	
  // fudge showers using D3PD vectors (except for eratio)
  void FudgeShowers( std::vector<float> clE,
		     std::vector<float> eta2   ,
		     std::vector<float>& rhad1  ,
		     std::vector<float>& rhad   ,
		     std::vector<float>& e277   ,
		     std::vector<float>& reta   ,
		     std::vector<float>& rphi   ,
		     std::vector<float>& weta2  ,
		     std::vector<float>& f1     ,
		     std::vector<float>& fside  ,
		     std::vector<float>& wtot   ,
		     std::vector<float>& w1     ,
		     std::vector<float>& deltae ,
		     std::vector<float>& eratio ,
		     std::vector<int> isConv  ,
		     int    preselection=-999);

  /** Calculate Eratio (the only variable which is not stored in the egamma/PhotonD3PD)
    * given emaxs1 and Emax2. Return a pointer to a vector<float>
    **/
  static std::vector<float>* getEratio(std::vector<float> emaxs1, std::vector<float> Emax2)
  {
      std::vector<float> *eratio = new std::vector<float>();
      for (unsigned int i = 0; i < emaxs1.size(); ++i)
         eratio->push_back(emaxs1[i] + Emax2[i] == 0 ? 0 : 
            (emaxs1[i] - Emax2[i])/(emaxs1[i] + Emax2[i]));
      return eratio;
  }
  	     
  // get fudge factor for predefined preselection
  double GetFF_Rhad1 ( double pt, double eta2 ){ return m_rhad1_ff.Get(pt, fabs(eta2));  };
  double GetFF_Rhad  ( double pt, double eta2 ){ return m_rhad_ff.Get(pt, fabs(eta2));   };
  double GetFF_E277  ( double pt, double eta2 ){ return m_e277_ff.Get(pt, fabs(eta2));   };
  double GetFF_Reta  ( double pt, double eta2 ){ return m_reta_ff.Get(pt, fabs(eta2));   };
  double GetFF_Rphi  ( double pt, double eta2 ){ return m_rphi_ff.Get(pt, fabs(eta2));   };
  double GetFF_Weta2 ( double pt, double eta2 ){ return m_weta2_ff.Get(pt, fabs(eta2));  };
  double GetFF_F1    ( double pt, double eta2 ){ return m_f1_ff.Get(pt, fabs(eta2));     };
  double GetFF_DE    ( double pt, double eta2 ){ return m_deltae_ff.Get(pt, fabs(eta2)); };
  double GetFF_Eratio( double pt, double eta2 ){ return m_eratio_ff.Get(pt, fabs(eta2)); };
  double GetFF_Fside ( double pt, double eta2 ){ return m_fside_ff.Get(pt, fabs(eta2));  };
  double GetFF_Wtot  ( double pt, double eta2 ){ return m_wtot_ff.Get(pt, fabs(eta2));   };
  double GetFF_W1    ( double pt, double eta2 ){ return m_w1_ff.Get(pt, fabs(eta2));     };
	
  double GetFF_Rhad1 ( double pt, double eta2, int conv, int preselection );
  double GetFF_Rhad  ( double pt, double eta2, int conv, int preselection );
  double GetFF_E277  ( double pt, double eta2, int conv, int preselection );
  double GetFF_Reta  ( double pt, double eta2, int conv, int preselection );
  double GetFF_Rphi  ( double pt, double eta2, int conv, int preselection );
  double GetFF_Weta2 ( double pt, double eta2, int conv, int preselection );
  double GetFF_F1    ( double pt, double eta2, int conv, int preselection );
  double GetFF_DE    ( double pt, double eta2, int conv, int preselection );
  double GetFF_Eratio( double pt, double eta2, int conv, int preselection );
  double GetFF_Fside ( double pt, double eta2, int conv, int preselection );
  double GetFF_Wtot  ( double pt, double eta2, int conv, int preselection );
  double GetFF_W1    ( double pt, double eta2, int conv, int preselection );

  double GetFF_Rhad1 ();
  double GetFF_Rhad  ();
  double GetFF_E277  ();
  double GetFF_Reta  ();
  double GetFF_Rphi  ();
  double GetFF_Weta2 ();
  double GetFF_F1    ();
  double GetFF_DE    ();
  double GetFF_Eratio();
  double GetFF_Fside ();
  double GetFF_Wtot  ();
  double GetFF_W1    ();

  double GetFFerr_Rhad1 ( double pt, double eta2 ){ return m_rhad1_fferr.Get(pt, fabs(eta2));  };
  double GetFFerr_Rhad  ( double pt, double eta2 ){ return m_rhad_fferr.Get(pt, fabs(eta2));   };
  double GetFFerr_E277  ( double pt, double eta2 ){ return m_e277_fferr.Get(pt, fabs(eta2));   };
  double GetFFerr_Reta  ( double pt, double eta2 ){ return m_reta_fferr.Get(pt, fabs(eta2));   };
  double GetFFerr_Rphi  ( double pt, double eta2 ){ return m_rphi_fferr.Get(pt, fabs(eta2));   };
  double GetFFerr_Weta2 ( double pt, double eta2 ){ return m_weta2_fferr.Get(pt, fabs(eta2));  };
  double GetFFerr_F1    ( double pt, double eta2 ){ return m_f1_fferr.Get(pt, fabs(eta2));     };
  double GetFFerr_DE    ( double pt, double eta2 ){ return m_deltae_fferr.Get(pt, fabs(eta2)); };
  double GetFFerr_Eratio( double pt, double eta2 ){ return m_eratio_fferr.Get(pt, fabs(eta2)); };
  double GetFFerr_Fside ( double pt, double eta2 ){ return m_fside_fferr.Get(pt, fabs(eta2));  };
  double GetFFerr_Wtot  ( double pt, double eta2 ){ return m_wtot_fferr.Get(pt, fabs(eta2));   };
  double GetFFerr_W1    ( double pt, double eta2 ){ return m_w1_fferr.Get(pt, fabs(eta2));     };
	
  double GetFFerr_Rhad1 ( double pt, double eta2, int conv, int preselection );
  double GetFFerr_Rhad  ( double pt, double eta2, int conv, int preselection );
  double GetFFerr_E277  ( double pt, double eta2, int conv, int preselection );
  double GetFFerr_Reta  ( double pt, double eta2, int conv, int preselection );
  double GetFFerr_Rphi  ( double pt, double eta2, int conv, int preselection );
  double GetFFerr_Weta2 ( double pt, double eta2, int conv, int preselection );
  double GetFFerr_F1    ( double pt, double eta2, int conv, int preselection );
  double GetFFerr_DE    ( double pt, double eta2, int conv, int preselection );
  double GetFFerr_Eratio( double pt, double eta2, int conv, int preselection );
  double GetFFerr_Fside ( double pt, double eta2, int conv, int preselection );
  double GetFFerr_Wtot  ( double pt, double eta2, int conv, int preselection );
  double GetFFerr_W1    ( double pt, double eta2, int conv, int preselection );

  double GetFFerr_Rhad1 ();
  double GetFFerr_Rhad  ();
  double GetFFerr_E277  ();
  double GetFFerr_Reta  ();
  double GetFFerr_Rphi  ();
  double GetFFerr_Weta2 ();
  double GetFFerr_F1    ();
  double GetFFerr_DE    ();
  double GetFFerr_Eratio();
  double GetFFerr_Fside ();
  double GetFFerr_Wtot  ();
  double GetFFerr_W1    ();
							    							    
  // fudge a specific variable 				    				    
  double Fudge_Rhad1 ( double rhad1,  double pt, double eta2){  return ( rhad1  + m_rhad1_ff.Get(pt, fabs(eta2))  );};
  double Fudge_Rhad  ( double rhad,   double pt, double eta2){  return ( rhad   + m_rhad_ff.Get(pt, fabs(eta2))   );};
  double Fudge_E277  ( double e277,   double pt, double eta2){  return ( e277   + m_e277_ff.Get(pt, fabs(eta2))   );};
  double Fudge_Reta  ( double reta,   double pt, double eta2){  return ( reta   + m_reta_ff.Get(pt, fabs(eta2))   );};
  double Fudge_Rphi  ( double rphi,   double pt, double eta2){  return ( rphi   + m_rphi_ff.Get(pt, fabs(eta2))   );};
  double Fudge_Weta2 ( double weta2,  double pt, double eta2){  return ( weta2  + m_weta2_ff.Get(pt, fabs(eta2))  );};
  double Fudge_F1    ( double f1,     double pt, double eta2){  return ( f1     + m_f1_ff.Get(pt, fabs(eta2))     );};
  double Fudge_DE    ( double deltae, double pt, double eta2){  return ( deltae + m_deltae_ff.Get(pt, fabs(eta2)) );};
  double Fudge_Eratio( double eratio, double pt, double eta2){  return ( eratio + m_eratio_ff.Get(pt, fabs(eta2)) );};
  double Fudge_Fside ( double fside,  double pt, double eta2){  return ( fside  + m_fside_ff.Get(pt, fabs(eta2))  );};
  double Fudge_Wtot  ( double wtot,   double pt, double eta2){  return ( wtot   + m_wtot_ff.Get(pt, fabs(eta2))   );};
  double Fudge_W1    ( double w1,     double pt, double eta2){  return ( w1     + m_w1_ff.Get(pt, fabs(eta2))     );};

  double Fudge_Rhad1 ( double rhad1,  double pt, double eta2, int conv, int preselection );
  double Fudge_Rhad  ( double rhad,   double pt, double eta2, int conv, int preselection );
  double Fudge_E277  ( double e277,   double pt, double eta2, int conv, int preselection );
  double Fudge_Reta  ( double reta,   double pt, double eta2, int conv, int preselection );
  double Fudge_Rphi  ( double rphi,   double pt, double eta2, int conv, int preselection );
  double Fudge_Weta2 ( double weta2,  double pt, double eta2, int conv, int preselection );
  double Fudge_F1    ( double f1,     double pt, double eta2, int conv, int preselection );
  double Fudge_DE    ( double deltae, double pt, double eta2, int conv, int preselection );
  double Fudge_Eratio( double eratio, double pt, double eta2, int conv, int preselection );
  double Fudge_Fside ( double fside,  double pt, double eta2, int conv, int preselection );
  double Fudge_Wtot  ( double wtot,   double pt, double eta2, int conv, int preselection );
  double Fudge_W1    ( double w1,     double pt, double eta2, int conv, int preselection );

  // set shower preselection cuts
  // *** 2010 
  // 0 = tight  isolated
  // 1 = loose  isolated
  // 2 = tightx isolated
  // 3 = tighty isolated
  // 4 = tight  isolated . Distorted material 
  // *** 2011 
  // 5 = tight  isolated
  // 6 = tight  non-isolated
  // ***  
  // 10= tight  isolated. Old menu rel15 (tune 3 in PhotonIDTool).
	void SetPreselection ( int preselection = -1 ){ m_preselection = preselection; LoadFFs(m_conv);}; 

  void SetIsConverted ( int isConv = 0 ){ m_conv = isConv;}; 

  void SetPt   ( double pt  ){ m_pt   = pt; }; 
  void SetEta  ( double eta ){ m_eta2 = eta;}; 

  void SetVerbose( bool verbose=true ){ m_verbose=verbose; }

  int  GetPreselection(){ return m_preselection; };

  //drawing methods
  TGraphErrors* GetFFmap       (int var, double eta, int isConv, int preselection=-1);

  TGraphErrors* GetFFmap_Rhad1 (double eta, int isConv, int preselection=-1);
  TGraphErrors* GetFFmap_Rhad  (double eta, int isConv, int preselection=-1);
  TGraphErrors* GetFFmap_E277  (double eta, int isConv, int preselection=-1);
  TGraphErrors* GetFFmap_Reta  (double eta, int isConv, int preselection=-1);
  TGraphErrors* GetFFmap_Rphi  (double eta, int isConv, int preselection=-1);
  TGraphErrors* GetFFmap_Weta2 (double eta, int isConv, int preselection=-1);
  TGraphErrors* GetFFmap_F1    (double eta, int isConv, int preselection=-1);
  TGraphErrors* GetFFmap_Fside (double eta, int isConv, int preselection=-1);
  TGraphErrors* GetFFmap_Wtot  (double eta, int isConv, int preselection=-1);
  TGraphErrors* GetFFmap_W1    (double eta, int isConv, int preselection=-1);
  TGraphErrors* GetFFmap_DE    (double eta, int isConv, int preselection=-1);
  TGraphErrors* GetFFmap_Eratio(double eta, int isConv, int preselection=-1);
  
  TGraphErrors* GetFFmap_Rhad1 (); //Get current values
  TGraphErrors* GetFFmap_Rhad  ();
  TGraphErrors* GetFFmap_E277  ();
  TGraphErrors* GetFFmap_Reta  ();
  TGraphErrors* GetFFmap_Rphi  ();
  TGraphErrors* GetFFmap_Weta2 ();
  TGraphErrors* GetFFmap_F1    ();
  TGraphErrors* GetFFmap_Fside ();
  TGraphErrors* GetFFmap_Wtot  ();
  TGraphErrors* GetFFmap_W1    ();
  TGraphErrors* GetFFmap_DE    ();
  TGraphErrors* GetFFmap_Eratio();
	
  void LoadFFs(int isConv, int preselection=-1);

 private:
  void LoadFFsDefault(); // load preselection 0 for FudgeShowers()
  void LoadFFsDummy();

  bool useRhad1(double eta2){ return (fabs(eta2)<0.8 || fabs(eta2)>=1.37);};
  bool useRhad (double eta2){ return !useRhad1(eta2);};

  bool m_verbose;

  // discriminating variables
  double m_pt;
  double m_eta2;
  double m_rhad1; 
  double m_rhad; 
  double m_reta;
  double m_rphi;
  double m_weta2;
  double m_f1;
  double m_fside;
  double m_wtot;
  double m_w1;
  double m_deltae;
  double m_eratio;
  int    m_conv;

  //shower preselection to extract FFs
  int m_preselection;

  // collections of fudge factors 
  PtEtaCollection<double> m_rhad1_ff;
  PtEtaCollection<double> m_rhad_ff;
  PtEtaCollection<double> m_e277_ff;
  PtEtaCollection<double> m_reta_ff;
  PtEtaCollection<double> m_rphi_ff;
  PtEtaCollection<double> m_weta2_ff;
  PtEtaCollection<double> m_f1_ff;
  PtEtaCollection<double> m_deltae_ff;
  PtEtaCollection<double> m_wtot_ff;
  PtEtaCollection<double> m_fside_ff;
  PtEtaCollection<double> m_w1_ff;
  PtEtaCollection<double> m_eratio_ff;

  // collections of fudge factors errors
  PtEtaCollection<double> m_rhad1_fferr;
  PtEtaCollection<double> m_rhad_fferr;
  PtEtaCollection<double> m_e277_fferr;
  PtEtaCollection<double> m_reta_fferr;
  PtEtaCollection<double> m_rphi_fferr;
  PtEtaCollection<double> m_weta2_fferr;
  PtEtaCollection<double> m_f1_fferr;
  PtEtaCollection<double> m_deltae_fferr;
  PtEtaCollection<double> m_wtot_fferr;
  PtEtaCollection<double> m_fside_fferr;
  PtEtaCollection<double> m_w1_fferr;
  PtEtaCollection<double> m_eratio_fferr;

};

#endif // #ifdef FudgeMCTool_h

#ifdef FudgeMCTool_cxx

FudgeMCTool::FudgeMCTool(double pt,
	       double eta2,
	       int isConv,
	       int preselection)
  : 
  m_pt(pt),
  m_eta2(fabs(eta2)),
  //
  m_rhad1(0.), 
  m_rhad(0.),
  m_reta(0.),
  m_rphi(0.),
  m_weta2(0.),
  m_f1(0.),
  m_fside(0.),
  m_wtot(0.),
  m_w1(0.),
  m_deltae(0.),
  m_eratio(0.),
  //
  m_conv(isConv),
  m_preselection(preselection)
{
  this->LoadFFsDefault();
  m_verbose = false;
  //  std::cout << "Initiliazing the tool ---->>> preselection " << preselection << std::endl; 
  
}

FudgeMCTool::~FudgeMCTool()
{}

#endif// #ifdef FudgeMCTool_cxx

