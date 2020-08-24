/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     muIsoUtil.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigmuIso
// 
// AUTHOR:   S. Giagu <stefano.giagu@cern.ch>
// 
// PURPOSE:  Utility namespace for LVL2 Muon Isolation FEX Algorithm
// ********************************************************************
#include <iostream>
#include <math.h>

#include "muIsoUtil.h"

namespace muIsoUtil {

//SGExtrapolator
//Return deltaEta between eta@MS entrance and eta@Calo entrance
//Input: phi, pt, charge of ID track at IP
double corrDEtaCM(double phi, double pt, int charge) {

 pt = fabs(pt);

 float pA_0 =  1.13931e+05;
 float pA_1 =  3.46418e+01;
 float pA_2 =  1.96513e-05;
 float pB   =  8.0065;
 float pC_0 =  8.15165e+05;
 float pC_1 =  4.12867e+03;
 float pC_2 = -1.56694e+00;

 if (pt < 4000.)   pt = 4000.;
 if (pt > 100000.) pt = 100000.;

 float A = pA_0/pt/pt + pA_1/pt + pA_2;
 float B = pB;
 float C = pC_0/pt/pt + pC_1/pt + pC_2;
 if (charge > 0) C *= -1;

 return A*sin(B*phi + C);
}

//Return deltaPhi between phi@MS entrance and phi@Calo entrance
//Input: eta, pt, charge of ID track at IP
double corrDPhiCM(double eta, double pt, int charge) {

 pt = fabs(pt);
 eta = fabs(eta);
 if (eta > 3.4) eta = 3.4;

 float pA_0 = -4.95931e+05;
 float pA_1 = -7.53506e+01;
 float pA_2 =  7.18963e-05;
 float pB_0 =  3.17051e+05;
 float pB_1 = -6.52186e+01;
 float pB_2 = -5.60320e-05;
 if (eta > 1.4 && eta < 1.8) {
   pA_0 = 0.0;
   pA_1 = -6.67742e+02;
   pA_2 = -2.29636e-04;
   pB_0 = 0.0;
   pB_1 = 3.47403e+02;
   pB_2 = 1.38649e-04;
 }
 if (eta > 1.8) {
   pA_0 = 0.0;
   pA_1 = -8.04743e+01;
   pA_2 =  6.23445e-05;
   pB_0 = 0.0;
   pB_1 =  2.27455e+01;
   pB_2 = -3.65776e-05;
 }

 if (pt < 4000.)   pt = 4000.;
 if (pt > 100000.) pt = 100000.;

 float A = pA_0/pt/pt + pA_1/pt + pA_2;
 float B = pB_0/pt/pt + pB_1/pt + pB_2;

 float ris = A + B*eta;

 if (charge < 0) return  ris;
 else            return -ris;
}

//Return deltaEta between eta@Calo entrance and eta@IP (dummy)
//Input: phi, pt, charge of ID track at IP
//double corrDEtaC(double phi, double pt, int charge) { return 0.0; }
double corrDEtaC() { return 0.0; }

//Return deltaPhi between phi@Calo entrance and phi@IP
//Input: eta, pt, charge of ID track at IP
double corrDPhiC(double eta, double pt, int charge) {

 pt = fabs(pt);

 if (pt == 0) return 0.0;
 if (pt <   4000.) pt =   4000.;
 if (pt > 100000.) pt = 100000.;

 float p0 = 1258.61;
 float p1 = -3119.93;
 float p2 = 6286.04;
 float p3 = -5844.6;
 float p4 = 2617.29;
 float p5 = -562.278;
 float p6 = 46.7315;

 float et1 = fabs(eta);

 if (et1 > 3.3) et1 = 3.3;
 if (et1 < 1.)  et1 = 1.0;

 double A = p0 +
            p1*et1 +
            p2*et1*et1 +
            p3*et1*et1*et1 +
            p4*et1*et1*et1*et1 +
            p5*et1*et1*et1*et1*et1 +
            p6*et1*et1*et1*et1*et1*et1;

 if (charge > 0) A *= -1.0;

 float s0 = -3.77939e-06;
 float s1 = -1.23528e-05;
 float s2 =  9.92281e-06;
 float s3 = -1.81095e-06;

 if (charge > 0) {
   s0 =  6.56653e-06;
   s1 =  9.83486e-06;
   s2 = -9.09965e-06;
   s3 =  1.70549e-06;
 }

 float et2 = fabs(eta);

 if (et2 > 3.0)  et2 = 3.0;
 if (et2 < 0.75) et2 = 0.75;

 double B = s0 +
            s1*et2 +
            s2*et2*et2 +
            s3*et2*et2*et2;

 return (A/pt + B);
}

//Return phi@Calo entrance
//Input: phi, eta, pt, charge of ID track at IP
double phiATCal(double phi, double eta, double pt, int charge) {
   float xphi = phi + corrDPhiC(eta, pt, charge);
   if (xphi >  M_PI) xphi -= 2*M_PI;
   if (xphi < -M_PI) xphi += 2*M_PI;
   return xphi;
}

//Return eta@Calo entrance
//Input: phi, eta, pt, charge of ID track at IP
//double etaATCal(double phi, double eta, double pt, int charge) {
double etaATCal(double eta) {
   return eta;
   //return eta + corrDEtaC(phi, pt, charge);
}

//Return phi@MS entrance
//Input: phi, eta, pt, charge of ID track at IP
double phiATMS(double phi, double eta, double pt, int charge) {
   float xphi =  phiATCal(phi, eta, pt, charge) + corrDPhiCM(eta, pt, charge);
   if (xphi >  M_PI) xphi -= 2*M_PI;
   if (xphi < -M_PI) xphi += 2*M_PI;
   return xphi;
}

//Return eta@MS entrance
//Input: phi, eta, pt, charge of ID track at IP
double etaATMS(double phi, double eta, double pt, int charge) {
   return eta + corrDEtaCM(phi, pt, charge);
}

//Return approximate radii of various calo samplings (in cm).
double rsam(int sampling) {

  //sampling:  0: PS   1/2/3 E1/2/3    4/5/6 T1/2/3   7MS

  //              PS/E1 E2    E3    T1    T2    T3    MS
  //Inner radius: 125., 145., 205., 225., 275., 375., 425.

  double r[6] = {135.,175., 215., 250., 325., 400.}; //Middle radius EM S1,2,3  Tile S1,2,3

  if (sampling <=0) return 125.;
  if (sampling >=7) return 425.;
  return r[sampling-1];
}

//Return linear interp. of track param. at various samplings
double parATSampling(int sampling, float parc, float parm) {

  float rc = rsam(0);
  float rm = rsam(7);
  float  r = rsam(sampling);

  float A = (parm-parc) / (rm - rc);
  float B = (rm*parc-rc*parm) / (rm - rc);

  return A*r+B;
}


}//muIsoUtil
