/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *  Class definition of GSCTool - see Root/NPVBeamspotCorrection.cxx for more details
 *  Joe Taenzer (joseph.taenzer@cern.ch), Nov 2012
 */

#ifndef _KOJIORIGINCORRECTION_
#define _KOJIORIGINCORRECTION_

#include <iostream>
#include <TVector3.h>
#include <TString.h>
#include <TLorentzVector.h>

class KojiOriginCorrectionTool : public TNamed {

 public:

  KojiOriginCorrectionTool();
  virtual ~KojiOriginCorrectionTool() { };

#ifdef ROOTCORE
  ClassDef(KojiOriginCorrectionTool,1);
#endif

  void initKojiOriginCorrection();

  TLorentzVector ApplyApproximateOriginCorr(const TLorentzVector &jet_det, const TVector3 &PV, int samplingMax);
  TLorentzVector ApplyApproximateOriginCorr(const TLorentzVector &jet_det, const TVector3 &PV, int samplingMax, double eta_det);

 private:

  void error(TString msg) 
  { printf("\nERROR - KojiOriginCorrectionTool:\n\n  %s\n\n",msg.Data()); abort(); }

  void ApproximateJetCentroid(TVector3 &centroid, double eta_det, double phi_det, int samplingMax);
  TVector3 ApproximateJetCentroid(double eta_det, double phi_det, int samplingMax);
  void ApproximateEtaPhiOrigin(double &eta_origin, double &phi_origin, const TVector3 &PV, const TVector3 &centroid);
  double ApproximateEtaPhiOrigin(const TVector3 &PV, const TVector3 &jet_centroid, bool getEta);
  double ApproximateEtaPhiOrigin(const TVector3 &PV, double eta_det, double phi_det, int samplingMax, bool getEta);

};

#endif
