/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*  Return approximate jet origin correction based on the sampling max jet moment.
 *  Inspired by Koji's code from 2010.
 *
 *  Author: Koji Terashi <Koji.Terashi@cern.ch> 
 *  Adapted for ApplyJetCalibration in August 2013 by Dag Gillberg <dag.gillberg@cern.ch>, Joe Taenzer <joseph.taenzer@cern.ch>
 *
 *  Description: See e-mail at the bottom.
 */


#include "ApplyJetCalibration/KojiOriginCorrection.h"

KojiOriginCorrectionTool::KojiOriginCorrectionTool() {
  initKojiOriginCorrection();
}

  
void KojiOriginCorrectionTool::ApproximateJetCentroid(TVector3 &centroid, double eta_det, double phi_det, int samplingMax) {
  /*
    --------------------------------------------
    Sampling#  Layer	<r> [mm]   <z> [mm]
    --------------------------------------------
    0	   PreSamplerB	1456.66    -
    1      EMB1		1532.17    -
    2	   EMB2		1723.89    - 
    3	   EMB3		1923.02    -
    4	   PreSamplerE	-          3664
    5	   EME1		-          3780.03
    6	   EME2		-          3973.68
    7	   EME3		-          4185.84
    8	   HEC0		-          4456.25
    9	   HEC1		-          4864.5
    10	   HEC2		-          5419.5
    11	   HEC3		-          5900
    12	   TileBar0	2445       -
    13	   TileBar1	2995       -
    14	   TileBar2	3650       -
    15	   TileGap1	3213.85    3506.77
    16	   TileGap2	3649.43    3400
    17	   TileGap3	-          3539.65
    18	   TileExt0	2445       -
    19	   TileExt1	2870       -
    20	   TileExt2	3500       -
    21	   FCAL0	-          4930.6
    22	   FCAL1	-          5389.45
    23	   FCAL2	-          5863.95
    --------------------------------------------
  */
  static double r_layer[] = { 1456.66, 1532.17, 1723.89, 1923.02, // EMB
			      -999, -999, -999, -999,             // EMEC
			      -999, -999, -999, -999,             // HEC
			      2445, 2995, 3650,                   // TileBar
			      3213.85, 3649.43, -999,             // TileGap
			      2445, 2870, 3500,                   // TileExt
			      -999, -999, -999};                  // FCal

  static double z_layer[] = { -999, -999, -999, -999,             // EMB
			      3664, 3780.03, 3973.68, 4185.84,    // EMEC
			      4456.25, 4864.5, 5419.5, 5900,      // HEC
			      -999, -999, -999,                   // TileBar
			      3506.77, 3400, 3539.65,             // TileGap
			      -999, -999, -999,                   // TileExt
			      4930.6, 5389.45, 5863.95};          // FCal

  if (samplingMax<0||samplingMax>23) 
    error(Form("Don't know about sampling layer %d",samplingMax));

  double rT = r_layer[samplingMax], z = z_layer[samplingMax];
  if ( rT > 0 && z >0 ) { // Tile-gap 
    double r=sqrt(rT*rT+z*z); 
    centroid.SetPtEtaPhi(r/cosh(eta_det),eta_det,phi_det); 
  } else if ( rT > 0 ) { // Barrel+Tile
    centroid.SetPtEtaPhi(rT,eta_det,phi_det);
  } else { // Endcap (EMEC, HEC, FCal)
    centroid.SetPtEtaPhi(z/sinh(eta_det),eta_det,phi_det);
  }
}

TVector3 KojiOriginCorrectionTool::ApproximateJetCentroid(double eta_det, double phi_det, int samplingMax) {
  TVector3 centroid; ApproximateJetCentroid(centroid,eta_det,phi_det,samplingMax); return centroid;
}

void ApproximateEtaPhiOrigin(double &eta_origin, double &phi_origin, const TVector3 &PV, const TVector3 &centroid) {
  eta_origin=(centroid-PV).Eta(); phi_origin=(centroid-PV).Phi();
}

TLorentzVector KojiOriginCorrectionTool::ApplyApproximateOriginCorr(const TLorentzVector &jet_det, const TVector3 &PV, int samplingMax) {
  TVector3 centroid(ApproximateJetCentroid(jet_det.Eta(),jet_det.Phi(),samplingMax)); TLorentzVector jet;
  double eta=(centroid-PV).Eta(), phi=(centroid-PV).Phi(), m=jet_det.M(), E=jet_det.E(), p=m<E?sqrt(E*E-m*m):E;
  jet.SetPtEtaPhiM(p/cosh(eta),eta,phi,m); return jet;
}

TLorentzVector KojiOriginCorrectionTool::ApplyApproximateOriginCorr(const TLorentzVector &jet_det, const TVector3 &PV, int samplingMax, double eta_det) {
  TVector3 centroid(ApproximateJetCentroid(eta_det,jet_det.Phi(),samplingMax)); TLorentzVector jet;
  double eta=(centroid-PV).Eta(), phi=(centroid-PV).Phi(), m=jet_det.M(), E=jet_det.E(), p=m<E?sqrt(E*E-m*m):E;
  jet.SetPtEtaPhiM(p/cosh(eta),eta,phi,m); return jet;
}

double KojiOriginCorrectionTool::ApproximateEtaPhiOrigin(const TVector3 &PV, const TVector3 &jet_centroid, bool getEta) {
  if (getEta) return (jet_centroid-PV).Eta();
  return (jet_centroid-PV).Phi();
}

double KojiOriginCorrectionTool::ApproximateEtaPhiOrigin(const TVector3 &PV, double eta_det, double phi_det, int samplingMax, bool getEta) {
  static TVector3 centroid; ApproximateJetCentroid(centroid,eta_det,phi_det,samplingMax);
  return ApproximateEtaPhiOrigin(PV,centroid,getEta);
}

void KojiOriginCorrectionTool::initKojiOriginCorrection() {
  printf("\n\n");
  printf("===================================\n\n");
  printf("  Initializing the Koji origin correction tool\n");
}

/*
  Original code written by Koji Terashi on Jul 30, 2010.
  Rewritten by Dag on March 13, 2013

  from:	 Koji Terashi <Koji.Terashi@cern.ch>
  to:	 David Miller <David.W.Miller@cern.ch>
  cc:	 Dag Gillberg <dag.gillberg@cern.ch>,
  Pier-Olivier DeViveiros <viveiros@physics.utoronto.ca>,
  Peter Manning <pmmannin@ucsc.edu>,
  Michiru Kaneda <Michiru.Kaneda@cern.ch>
  date:	 Fri, Jul 30, 2010 at 9:26 PM

  subject:	 Re: Bug on Jet momentum constructed from the vertex

  Hello Dag, David and all,

  I tried the method using SamplingMax to get the approximate jet position in the calorimeter. Please find the attached files. I certainly appreciate your comments. As David said, this should be a temporary fix but it's important to have something reasonable soon as the inclusive jet paper is coming out...

  Naoko gave me the information of ideal calorimeter geometry, which is located at
  http://terashi.web.cern.ch/terashi/cell-155678.db.gz
  (numbers correspond to id, sampling#, eta, hi, x, y, z, noise, volume)

  From that, the average radial (r) and longitudinal (z) distances from the detector center can be calculated for each calorimeter layer (samplig_rz.txt). From these positions, I tried to estimate the jet centroid in the SampligMax layer, and calculate the physics eta using a piece of code (ugly_fix.txt). Attached ps file (physMomPlot.ps) shows the results from the data (run 159086, 1st page) and MC (2nd). The data has this problem but the MC doesn't as I am looking at MC D3PD produced from AOD (which is known to be okay for this problem). The left two columns show the plot suggested by Dag for different jet rapidity bins. The most-right columns show the difference between the new (etaFixed) and original (EtaPhys) eta w.r.t the primary vertex. You see that the physics eta can be reasonably well recovered by this method. However, you would notice that the slope looks slightly different between the two eta values for jets in the HEC (bottom plots in p.2). This could probably be due to the fact that I simply average over all cells in each layer, which have slightly different r/z positions...

  What do you think? The code is ugly and I would like someone to have it refined...
*/

