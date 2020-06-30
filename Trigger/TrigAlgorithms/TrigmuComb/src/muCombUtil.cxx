/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *********************************************************************
// 
// NAME:     muCombUtil.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigmuComb
// 
// AUTHOR:   S. Giagu <stefano.giagu@cern.ch>
// 
// PURPOSE:  Utility namespace for LVL2 Combined Muon Reco FEX Algorithm
// *********************************************************************
#include <iostream>
#include <math.h>

#include "muCombUtil.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigInDetEvent/TrigInDetTrack.h"

namespace muCombUtil {

  void setMuFastRes(std::vector<double>& vec, double p1,double p2,
		    double p3,double p4,double p5,double p6) {
    vec.clear();
    vec.push_back(p1);
    vec.push_back(p2);
    vec.push_back(p3);
    vec.push_back(p4);
    vec.push_back(p5);
    vec.push_back(p6);
  }

  void setIDSCANRes(std::vector<double>& vec, double p1,double p2) {
    vec.clear();
    vec.push_back(p1);
    vec.push_back(p2);
  }


  double getMuFastRes(std::vector<double> vec, const MuonFeature* feature) {

   double  ptGev = feature->pt();
   double  pt = ptGev*1000.;     //muFast Pt(MeV)

   if (pt == 0) return 1.0e33;
   
   double AbsPtInv = fabs(1./pt);  //muFast 1/Pt  (1/MeV)
   int    add      = feature->saddress();
   double AbsEta   = fabs(feature->eta());
   double phi      = feature->phi();

   if ( add != -1) {
      if (AbsPtInv < 0.000186) {
	 return vec[0]*AbsPtInv + vec[1]/1000.;
      } 
      else {
	 double AbsPtInv3 = AbsPtInv*AbsPtInv*AbsPtInv;
	 double AbsPtInv2 = AbsPtInv*AbsPtInv;
	 return vec[2]*AbsPtInv3/(1000.*1000.) +
	    vec[3]*AbsPtInv2/(1000.)       +
	    vec[4]*AbsPtInv                +
	    vec[5]/1000.;
      }
   } 
   else {//Takuya/Kunihiro updated numbers 

      const int N_PARAMS = 5;
      const double vparEC1[N_PARAMS] = {0.291483, -6.11348,  65.1099, -285.664,  440.041};
      const double vparEC2[N_PARAMS] = {0.286307, -4.6759,   43.2815, -163.185,  210.786};
      const double vparEC3[N_PARAMS] = {0.330699, -6.70755,  70.4725, -291.85,   408.739};
      const double vparEC4[N_PARAMS] = {0.261738, -4.69971,  47.4762, -183.98,   236.813};
      const double vparEC5[N_PARAMS] = {0.196301, -3.57276,  38.3744, -159.808,  228.256};
      const double vparEC6[N_PARAMS] = {0.172939, -3.10788,  33.3823, -142.996,  212.957};
      const double vparEC7[N_PARAMS] = {0.233017, -4.377,    42.5691, -171.752,  245.702};
      const double vparEC8[N_PARAMS] = {0.22389,  -4.16259,  40.1369, -162.824,  236.39};
      const double vparEC9[N_PARAMS] = {0.197992, -3.52117,  33.5997, -136.014,  197.474};
      const double vparECA[N_PARAMS] = {0.417289, -0.852254,-31.9257,  308.873, -719.591};
      const double vparECB[N_PARAMS] = {0.526612, -8.04087,  82.1906, -336.87,   462.973};

      double AbsPtInvGeV = AbsPtInv * 1000;

      const double AbsPtInvGeVMin = 5e-3;   // 200 GeV
      const double AbsPtInvGeVMax = 0.25;   //   4 GeV
      if( AbsPtInvGeV < AbsPtInvGeVMin ) AbsPtInvGeV = AbsPtInvGeVMin;
      if( AbsPtInvGeV > AbsPtInvGeVMax ) AbsPtInvGeV = AbsPtInvGeVMax;
      
      //cout << "...AbsPtInvGeV=" << AbsPtInv << endl;

      const double* vpar;
      int spReg = whichECRegion(AbsEta,phi);
      //cout << "...spReg=" << spReg << endl;
      if     ( spReg==1 ) { vpar = vparECA; }
      else if( spReg==2 ) { vpar = vparECB; }
      else {
	 if     ( AbsEta < 1.20) { vpar = vparEC1; }
	 else if( AbsEta < 1.35) { vpar = vparEC2; }
	 else if( AbsEta < 1.50) { vpar = vparEC3; }
	 else if( AbsEta < 1.65) { vpar = vparEC4; }
	 else if( AbsEta < 1.80) { vpar = vparEC5; }
	 else if( AbsEta < 1.95) { vpar = vparEC6; }
	 else if( AbsEta < 2.10) { vpar = vparEC7; }
	 else if( AbsEta < 2.35) { vpar = vparEC8; }
	 else                    { vpar = vparEC9; }
      }

      double fracRes = vpar[0] + vpar[1]*AbsPtInvGeV
	 + vpar[2]*AbsPtInvGeV*AbsPtInvGeV 
	 + vpar[3]*AbsPtInvGeV*AbsPtInvGeV*AbsPtInvGeV
	 + vpar[4]*AbsPtInvGeV*AbsPtInvGeV*AbsPtInvGeV*AbsPtInvGeV;

      return fabs(fracRes * AbsPtInv);
   }
}

int whichECRegion( const float eta, const float phi ) {
   // 0: bulk
   // 1: WeakBfield A
   // 2: WeakBfield B

   float absEta = fabs(eta);

   if(      ( 1.3 <= absEta && absEta < 1.45) &&
            ( (0            <= fabs(phi) && fabs(phi) < M_PI/48. )     ||
	      (M_PI*11./48. <= fabs(phi) && fabs(phi) < M_PI*13./48. ) ||
	      (M_PI*23./48. <= fabs(phi) && fabs(phi) < M_PI*25./48. ) ||
	      (M_PI*35./48. <= fabs(phi) && fabs(phi) < M_PI*37./48. ) ||
	      (M_PI*47./48. <= fabs(phi) && fabs(phi) < M_PI )
	  )
      ) return 1;
   
   else if( ( 1.5 <= absEta && absEta < 1.65 ) &&
	    ( (M_PI*3./32.  <= fabs(phi) && fabs(phi) < M_PI*5./32. ) ||
	      (M_PI*11./32. <= fabs(phi) && fabs(phi) < M_PI*13./32.) ||
	      (M_PI*19./32. <= fabs(phi) && fabs(phi) < M_PI*21./32.) ||
	      (M_PI*27./32. <= fabs(phi) && fabs(phi) < M_PI*29./32.)
	       )
      ) return 2;
   
   else return 0;
}

/*
  double getMuFastRes(std::vector<double> barrelvec, std::vector<double> ec1vec, 
		      std::vector<double> ec2vec, std::vector<double> ec3vec, std::vector<double> ec4vec,
		      const MuonFeature* feature) {

    double pt = feature->pt()*1000.; //muFast 1/Pt  (1/MeV)
    if (pt == 0) return 1.0e33;
  
    double AbsPtInv = fabs(1./pt);  //muFast 1/Pt  (1/MeV)
    int    add      = feature->saddress();
    double AbsEta   = fabs(feature->eta());

    std::vector<double> vec;
    if ( add != -1) {
      vec = barrelvec;
      if (AbsPtInv < 0.000186) {
	return vec[0]*AbsPtInv + vec[1]/1000.;
      } else {
	double AbsPtInv3 = AbsPtInv*AbsPtInv*AbsPtInv;
	double AbsPtInv2 = AbsPtInv*AbsPtInv;
	return vec[2]*AbsPtInv3/(1000.*1000.) +
	  vec[3]*AbsPtInv2/(1000.)       +
	  vec[4]*AbsPtInv                +
	  vec[5]/1000.;
      }
    } else {
      if (AbsEta < 1.35) vec = ec1vec;
      else if (AbsEta>=1.35 && AbsEta<1.65) vec = ec2vec;
      else if (AbsEta>=1.65 && AbsEta<2.0)  vec = ec3vec;
      else vec = ec4vec;

      if (AbsPtInv < 0.000191) {
	return vec[0]*AbsPtInv + vec[1]/1000.;
      } else {
	double AbsPtInv3 = AbsPtInv*AbsPtInv*AbsPtInv;
	double AbsPtInv2 = AbsPtInv*AbsPtInv;
	return vec[2]*AbsPtInv3/(1000.*1000.) +
	  vec[3]*AbsPtInv2/(1000.)       +
	  vec[4]*AbsPtInv                +
	  vec[5]/1000.;
      }
    }
    return 9999.;
  }
*/

  double getIDSCANRes(std::vector<double> barrelvec, std::vector<double> ec1vec, 
		      std::vector<double> ec2vec, std::vector<double> ec3vec, std::vector<double> ec4vec,
		      double pt_id, double eta_id) {

    double pt = pt_id;
    if (pt == 0) return 1.0e33;

    double AbsPtInv = fabs(1./pt);
    double AbsEta   = fabs(eta_id);

    std::vector<double> vec;
    if (AbsEta < 1.05) vec = barrelvec;
    else if (AbsEta>=1.05 && AbsEta<1.35) vec = ec1vec;
    else if (AbsEta>=1.35 && AbsEta<1.65) vec = ec2vec;
    else if (AbsEta>=1.65 && AbsEta<2.0)  vec = ec3vec;
    else vec = ec4vec;

    return vec[0]*AbsPtInv+vec[1]/1000.;
  }


  double getMuFastEtaRes(const MuonFeature* feature) {

    double pt  = feature->pt();
    double eta = feature->eta();
    if (pt < 4. ) pt = 4.;
    if (pt > 40.) pt = 40.;
    bool ts = false;
    if (feature->radius() <= 10.) ts = true;

    if (fabs(eta) < 1.) {//barrel
      return 7.75e-2/pt + 8.1e-3;
    }
    else {
      if (ts) {//trigger station
	if (fabs(eta) >= 1.  && fabs(eta) < 1.2) {//ec0
	  return 2.0e-1/pt + 1.5e-3;
	}
	else if (fabs(eta) >= 1.2 && fabs(eta) < 1.5) {//ec1
	  return 2.5e-1/pt + 0.1e-3;
	}
	else if (fabs(eta) >= 1.5 && fabs(eta) < 2.0) {//ec2
	  return 1.3e-1/pt + 6.0e-3;
	}
	else {//ec3
	  return 1.3e-1/pt + 8.0e-3;
	}
      }
      else {
	if (fabs(eta) >= 1.  && fabs(eta) < 1.6) {//ec0/1
	  return 0.071/pt + 0.0055;
	}
	else {//ec2/3
	  return 0.055/pt + 0.0063;
	}
      }
    }
  }


  double getMuFastPhiRes(const MuonFeature* feature) {

    double pt  = feature->pt();
    double eta = feature->eta();
    if (pt < 4. ) pt = 4.;
    if (pt > 40.) pt = 40.;
    bool ts = false;
    if (feature->radius() <= 10.) ts = true;

    if (fabs(eta) < 1.) {//barrel
      return 1.9e-1/pt - 3.4e-4;
    }
    else {
      if (ts) {//trigger station
	if (fabs(eta) >= 1.  && fabs(eta) < 1.2) {//ec0
	  return 2.0e-1/pt + 2.0e-3;
	}
	else if (fabs(eta) >= 1.2 && fabs(eta) < 1.5) {//ec1
	  return 1.8e-1/pt + 3.0e-3;
	}
	else if (fabs(eta) >= 1.5 && fabs(eta) < 2.0) {//ec2
	  return 1.5e-1/pt + 8.0e-3;
	}
	else {//ec3
	  return 1.0e-1/pt + 1.8e-2;
	}
      }
      else {
	//if (fabs(eta) >= 1.  && fabs(eta) < 1.6) {//ec0/1
	  return 0.08/pt + 0.0025;
	//}
	//else {//ec2/3
	//  return 0.08/pt + 0.0025;
	//}
      }
    }
  }

  double getG4ExtEtaRes(double pt, double eta) {

    //double pt  = feature->pt();
    //double eta = feature->eta();
    if (pt < 4. ) pt = 4.;
    if (pt > 40.) pt = 40.;
    // bool ts = false;
    // if (feature->radius() <= 10.) ts = true;

    //G4 extrapolator eta resolution parametrized as A/pt^2 + B/pt + C + D*pt
    // Resolution evaluated in regions of eta: 0.0,0.5,1.0,1.5,2.0,above...
    if (fabs(eta) < 0.5) {
      return 6.0e-3/(pt*pt) + 4.4e-2/pt + 1.2e-2 + (-3.2e-5*pt);
    }
    else if (fabs(eta) >= 0.5 && fabs(eta) < 1.0) {
      return 5.6e-3/(pt*pt) + 3.9e-2/pt + 1.3e-2 + (-3.7e-5*pt);
    }
    else if (fabs(eta) >= 1.0 && fabs(eta) < 1.5) {
      return 2.1e-1/(pt*pt) + 8.8e-2/pt + 7.1e-4 + (1.7e-4*pt);
    }
    else if (fabs(eta) >= 1.5 && fabs(eta) < 2.0) {
      return 4.5e-3/(pt*pt) + 1.4e-2/pt + 1.1e-2 + (-5.7e-5*pt);
    }
    else { // if (fabs(eta) >= 2.0) {
      return 3.1e-2/(pt*pt) + 4.9e-1/pt + (-2.6e-3) + (3.2e-5*pt);
    }
  }


  double getG4ExtPhiRes(double pt, double eta) {

    //double pt  = feature->pt();
    //double eta = feature->eta();
    if (pt < 4. ) pt = 4.;
    if (pt > 40.) pt = 40.;
    // bool ts = false;
    // if (feature->radius() <= 10.) ts = true;

    //G4 extrapolator phi resolution parametrized as A/pt^2 + B/pt + C + D*pt
    // Resolution evaluated in regions of eta: 0.0,0.5,1.0,1.5,2.0,above...
    if (fabs(eta) < 0.5) {
      return 4.1e-2/(pt*pt) + 1.5e-1/pt + 1.4e-3 + (5.8e-5*pt);
    }
    else if (fabs(eta) >= 0.5 && fabs(eta) < 1.0) {
      return 9.4e-3/(pt*pt) + 8.8e-2/pt + 1.1e-2 + (-1.e-4*pt);
    }
    else if (fabs(eta) >= 1.0 && fabs(eta) < 1.5) {
      return 3.5e-1/(pt*pt) + 9.4e-2/pt + 8.2e-3 + (-3.3e-7*pt);
    }
    else if (fabs(eta) >= 1.5 && fabs(eta) < 2.0) {
      return 2.6e-2/(pt*pt) + 1.2e-1/pt + 3.8e-3 + (2.2e-5*pt);
    }
    else { // if (fabs(eta) >= 2.0) {
      return 2.7e-1/(pt*pt) + 3.2e-2/pt + (9.0e-3) + (-2.6e-5*pt);
    }
  }

  double getDeltaPhi(double phi1, double phi2) {
    double dphi = phi1 - phi2;
    if (dphi >  M_PI) dphi -= 2*M_PI;
    if (dphi < -M_PI) dphi += 2*M_PI;
    return fabs(dphi);
  }

  double getDeltaEta(double eta1, double eta2) {
    return fabs(eta1-eta2);
  }

  double getDeltaR(double eta1, double phi1, double eta2, double phi2) {
    double deta = getDeltaEta(eta1,eta2);
    double dphi = getDeltaPhi(phi1,phi2);
    return sqrt(deta*deta + dphi*dphi);
  }

  double getCombinedAverage(double p1, double sp1, double p2, double sp2) {
    if      (sp1 != 0 && sp2 != 0)  return (sp2*sp2*fabs(p1) + sp1*sp1*fabs(p2))/(sp1*sp1 + sp2*sp2);
    else if (sp1 != 0 && sp2 == 0)  return fabs(p2);
    else if (sp1 == 0 && sp2 != 0)  return fabs(p1);
    else                            return (fabs(p1)+fabs(p2))*0.5;
  }

  double getCombinedAverageSigma(double sp1, double sp2) {
    if (sp1 != 0 && sp2 != 0)       return sqrt((sp1*sp1*sp2*sp2)/(sp1*sp1 + sp2*sp2));
    else                            return 0.0;
  }

  double getChi2(int& ndof,   double ipt, 
		 double eta1, double seta1, double phi1, double sphi1, double ipt1, double sipt1,
		 double eta2, double seta2, double phi2, double sphi2, double ipt2, double sipt2, bool useAbsPt) {
    
	double deta    = getDeltaEta(eta1,eta2);
	double sdeta   = seta1*seta1+seta2*seta2;
	double dphi    = getDeltaPhi(phi1,phi2);
	double sdphi   = sphi1*sphi1+sphi2*sphi2;
	double dipt_1  = ipt - ipt1;
        if (useAbsPt) dipt_1  = fabs(ipt) - fabs(ipt1);
	double sdipt_1 = sipt1*sipt1;
	double dipt_2  = ipt - ipt2;
        if (useAbsPt) dipt_2  = fabs(ipt) - fabs(ipt2);
	double sdipt_2 = sipt2*sipt2;
	
	double chi2  = 0.0;
	ndof         = 0;
	if (sdeta != 0)   { chi2 += deta*deta/sdeta; ndof++; }
	if (sdphi != 0)   { chi2 += dphi*dphi/sdphi; ndof++; }
	if (sdipt_1 != 0) { chi2 += dipt_1*dipt_1/sdipt_1; ndof++; }
	if (sdipt_2 != 0) { chi2 += dipt_2*dipt_2/sdipt_2; ndof++; }
	
	if (ndof == 0) return 1.0e30;
	else           return chi2;
  }


  double getStdChi2(int& ndof,
                    double eta1, double seta1, double phi1, double sphi1, double qOvpt1, double sqOvpt1,
                    double eta2, double seta2, double phi2, double sphi2, double qOvpt2, double sqOvpt2, bool useAbsPt) {

        double deta    = getDeltaEta(eta1,eta2);
        double sdeta   = seta1*seta1+seta2*seta2;
        double dphi    = getDeltaPhi(phi1,phi2);
        double sdphi   = sphi1*sphi1+sphi2*sphi2;
        double dipt    = qOvpt1 - qOvpt2;
        if (useAbsPt)  dipt = fabs(qOvpt1) - fabs(qOvpt2);
        double sdipt   = sqOvpt1*sqOvpt1+sqOvpt2*sqOvpt2;

        double chi2  = 0.0;
        ndof         = 0;
        if (sdeta != 0) { chi2 += deta*deta/sdeta; ndof++; }
        if (sdphi != 0) { chi2 += dphi*dphi/sdphi; ndof++; }
        if (sdipt != 0) { chi2 += dipt*dipt/sdipt; ndof++; }

        if (ndof == 0) return 1.0e30;
        else           return chi2;
  }



  
}//muCombUtil
