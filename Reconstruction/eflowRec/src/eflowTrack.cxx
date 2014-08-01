/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/********************************************************************

NAME:     eflowTrack.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  D.R. Tovey
CREATED:  8th November, 2001

*******************************************************************/

// INCLUDE HEADER FILES:

#include "eflowRec/eflowTrack.h"
#include "Particle/TrackParticle.h"
#include "eflowRec/phicorr.h"
#include "eflowRec/cycle.h"

#include "TrkTrack/Track.h"

#include "eflowEvent/eflowCaloCluster.h"

#include "GaudiKernel/SystemOfUnits.h"

#include <math.h>
#include <iostream>

  static const int nEBins = 6;
  static const double eBins[nEBins] = { 6.0*Gaudi::Units::GeV, 10.0*Gaudi::Units::GeV, 20.0*Gaudi::Units::GeV, 50.0*Gaudi::Units::GeV, 100.0*Gaudi::Units::GeV, 200.0*Gaudi::Units::GeV };
  static const double piEOverP[nEBins][25] = {
    //  E006GeV
    { 0.538079, 0.56996, 0.569752, 0.561625, 0.575196, 0.562927, 0.559404, 0.538602, 0.498178, 0.495936,
      0.48578, 0.503655, 0.502391, 0.444408, 0.363311, 0.434135, 0.449856, 0.507721, 0.529574, 0.551478,
      0.567558, 0.562321, 0.557608, 0.574375, 0.548285 },
    //  E010GeV
    { 0.576383, 0.58673, 0.583321, 0.584648, 0.589828, 0.592145, 0.587606, 0.570677, 0.527654, 0.520921,
      0.532431, 0.521303, 0.536726, 0.478723, 0.399964, 0.469302, 0.493389, 0.544032, 0.531953, 0.583176,
      0.587059, 0.57095, 0.583344, 0.554815, 0.55208 },
    //  E020GeV
    { 0.653178, 0.675495, 0.675254, 0.669511, 0.681222, 0.657703, 0.668688, 0.6446, 0.619075, 0.594195,
      0.614555, 0.639146, 0.641823, 0.571881, 0.484811, 0.589383, 0.600439, 0.638183, 0.633598, 0.667057,
      0.65799, 0.674147, 0.664412, 0.659556, 0.656381 },
    //  E050GeV
    { 0.715763, 0.743176, 0.743916, 0.733508, 0.745213, 0.740859, 0.735187, 0.725765, 0.70822, 0.672347,
      0.71743, 0.745799, 0.72989, 0.667255, 0.602429, 0.682776, 0.697206, 0.720544, 0.722885, 0.74179,
      0.729653, 0.726003, 0.72584, 0.734034, 0.72425 },
    //  E100GeV
    { 0.354137, 0, 0.317618, 0.333668, 0.423185, 0.343473, 0, 0, 0, 0,
      0, 0.325611, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0.348751 },
    //  E200GeV
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0 }
  };
  static const double piEOverPStdDev[nEBins][25] = {
    //  E006GeV
    { 0.20173, 0.200671, 0.202916, 0.198869, 0.198242, 0.201189, 0.21344, 0.219492, 0.212276, 0.216894,
      0.220243, 0.228913, 0.227379, 0.218423, 0.202786, 0.23124, 0.227297, 0.242357, 0.233061, 0.219485,
      0.202146, 0.199805, 0.210095, 0.203764, 0.208529 },
    //  E010GeV
    { 0.174485, 0.17452, 0.1802, 0.17608, 0.179159, 0.175103, 0.17571, 0.188017, 0.193406, 0.188347,
      0.202858, 0.213894, 0.206327, 0.199071, 0.205845, 0.21956, 0.224996, 0.210466, 0.212453, 0.185832,
      0.175618, 0.19288, 0.189573, 0.179058, 0.177864 },
    //  E020GeV
    { 0.15479, 0.158334, 0.147516, 0.147761, 0.148847, 0.146109, 0.148134, 0.159867, 0.162006, 0.167485,
      0.178004, 0.179553, 0.168008, 0.174502, 0.193686, 0.193689, 0.199744, 0.182445, 0.190512, 0.155009,
      0.160511, 0.163805, 0.161526, 0.161651, 0.164439 },
    //  E050GeV
    { 0.119441, 0.121115, 0.119694, 0.130549, 0.119669, 0.130714, 0.125975, 0.128714, 0.132054, 0.132996,
      0.13584, 0.141842, 0.127883, 0.134931, 0.15677, 0.153287, 0.142425, 0.149938, 0.141647, 0.128307,
      0.132443, 0.137769, 0.136997, 0.132339, 0.126419 },
    //  E100GeV
    { 2.39996e-09, 0, 1.87533e-09, 1.22927e-09, 3.58816e-09, 2.23323e-09, 0, 0, 0, 0,
      0, 9.84381e-10, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 1.19857e-09 },
    //  E200GeV
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0 }
  };


eflowTrack::eflowTrack(const xAOD::TrackParticle* track, int eflowType, double trackExtrapolatedEta):
  m_track(track), m_e(0), m_vare(0) {
  sim(eflowType, trackExtrapolatedEta);
}

eflowTrack::~eflowTrack() { }
  
void eflowTrack::sim(int eflowType, double trackExtrapolatedEta) {
  double trackE   = m_track->e();
  double trackPt  = m_track->pt();

  double eOverP = 0;
  double eOverPStdDev = 0;

  if ((eflowType == 1) || (eflowType == 4) || (eflowType == 6)) { //electron
    eOverP = getElectronEOverP(trackPt, trackExtrapolatedEta);
    eOverPStdDev = getElectronEOverPStdDev(trackPt, trackExtrapolatedEta);
  } else if (eflowType == 5 ){ //hadron
    eOverP = getPiEOverP(trackPt, trackExtrapolatedEta);
    eOverPStdDev = getPiEOverPStdDev(trackExtrapolatedEta, trackPt);
  }

  m_e = eOverP * trackE;
  m_vare = eOverPStdDev* trackE;
  m_vare *= m_vare;
}

double eflowTrack::getElectronEOverP(double trackPt, double trackEta) const {
  double absEta = fabs(trackEta);
  double y1 = 0.0, y2 = 0.0;
  double eOverPAtEta03;
  double x = trackPt * cosh(trackEta) / (cosh(0.3) * Gaudi::Units::GeV);
  if (x <= 15.0) y1 = 0.7552 + x * (0.05511 + x * (-4.533e-3 + x * 1.233e-4));

  if (x >= 10.0) y2 = 0.9788;

  if (x < 10.0)
    eOverPAtEta03 = y1;
  else if (x < 15.0)
    eOverPAtEta03 = ((15.0 - x) * y1 + (x - 10.0) * y2) / 5.0;
  else
    eOverPAtEta03 = y2;

  y1 = 1.0;
  double temp = (absEta - 1.475) / 0.06;
  y2 = 0.78 * exp(-temp * temp / 2.0);
  double etaDependence = y1 - y2;

  return eOverPAtEta03 * etaDependence;
}

double eflowTrack::getPiEOverP(double trackPt, double trackEta) const {
  double absEta = fabs(trackEta);
  double e = trackPt * cosh(absEta);
  double y[2] = { };
  int iE[2] = { nEBins - 1 };
  for (int i = 0; i < nEBins; i++) {
    if (e <= eBins[i]) {
      iE[0] = i - 1;
      iE[1] = i;
      break;
    }
  }
  int j = (absEta < 2.5) ? (int) ((absEta / 0.1)) : 24;
  for (int i = 0; i < 2; i++) {
    if (iE[i] > -1) {
      y[i] = piEOverP[iE[i]][j];
    }
  }
  if (-1 == iE[1]) {
    return y[0];
  } else if (-1 == iE[0]) {
    return y[1];
  } else {
    return (log(eBins[iE[1]] / e) * y[0] + log(e / eBins[iE[0]]) * y[1])
          / log(eBins[iE[1]] / eBins[iE[0]]);
  }
}

double eflowTrack::getElectronEOverPStdDev(double trackPt, double trackEta) const {
  double absEta = fabs(trackEta);
  double y1 = 0.0, y2 = 0.0;
  double fudgeAtEta03, etaDependence;
  double x = trackPt * cosh(trackEta) / (cosh(0.3) * Gaudi::Units::GeV);
  if (x <= 92.012578) {
    y1 = 0.2137 + x * (-0.002926 + x * 1.59e-5);
  }
  if (x >= 92.012578) {
    y2 = 0.079085597;
  }
  if (x < 92.012578) {
    fudgeAtEta03 = y1;
  } else {
    fudgeAtEta03 = y2;
  }
  x = absEta;
  y1 = 1.0;
  y2 = 0.0;
  if (x > 1.3) {
    double temp = (log(x - 1.3) + 1.9) / 1.0;
    y2 = 0.1 * exp(-temp * temp / 2.0) / (x - 1.3);
  }
  etaDependence = y1 + y2;
  return fudgeAtEta03 * etaDependence;
}

double eflowTrack::getPiEOverPStdDev(double trackEta, double trackPt) const {
  double absEta = fabs(trackEta);
  double e = trackPt * cosh(absEta);
  double y[2] = { };
  int iE[2] = { nEBins - 1 };
  for (int i = 0; i < nEBins; i++) {
    if (e <= eBins[i]) {
      iE[0] = i - 1;
      iE[1] = i;
      break;
    }
  }
  int j = (absEta < 2.5) ? (int) ((absEta / 0.1)) : 24;
  for (int i = 0; i < 2; i++) {
    if (iE[i] > -1) {
      y[i] = piEOverPStdDev[iE[i]][j];
    }
  }
  if (-1 == iE[1]) {
    return y[0];
  } else if (-1 == iE[0]) {
    return y[1];
  } else {
    return (log(eBins[iE[1]] / e) * y[0] + log(e / eBins[iE[0]]) * y[1])
             / log(eBins[iE[1]] / eBins[iE[0]]);
  }
}

