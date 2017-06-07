/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EtaPhiRectangle.h"

#include <cmath> // M_PI
// #include <algorithm>
// #include <iostream>
// #include <fstream>


using nsw::EtaPhiRectangle;

EtaPhiRectangle::EtaPhiRectangle(const double &etaMin, const double &etaMax,
                                 const double &phiMin, const double &phiMax){
  etaMin_ = etaMin;
  etaMax_ = etaMax;
  phiMin_ = phiMin;
  phiMax_ = phiMax;
  wrapsAroundPi_ = std::abs(phiMax_-phiMin_) > M_PI;
  computeCenterCoord();
}
//--------------------------------------------------------------
bool EtaPhiRectangle::contains(const EtaPhiPoint &point) const {
  return ((fabs(point.first - etaCen_) < etaHw_)
          && fabs(phi_mpi_pi(point.second - phiCen_)) < phiHw_);
}
//--------------------------------------------------------------
void EtaPhiRectangle::computeCenterCoord(){
  etaCen_ = 0.5*(etaMin_ + etaMax_);
  // this takes care of the 2pi wrap
  phiCen_ = atan2(sin(phiMin_) + sin(phiMax_),
                  cos(phiMin_) + cos(phiMax_));
  etaHw_ = 0.5*fabs(etaMax_ - etaMin_);
  phiHw_ = 0.5*fabs(phi_mpi_pi(phiMax_ - phiMin_));
}
//--------------------------------------------------------------
double EtaPhiRectangle::phi_mpi_pi(const double &val) {
  double value(val);
  while(value <= -M_PI) value += 2.*M_PI;
  while(value >  +M_PI) value -= 2.*M_PI;
  return value;
}
//--------------------------------------------------------------
double EtaPhiRectangle::phi_zero_2pi(const double &val) {
  double value(val);
  while(value <= 0.     ) value += 2.*M_PI;
  while(value >  2.*M_PI) value -= 2.*M_PI;
  return value;
}
//--------------------------------------------------------------
double EtaPhiRectangle::overlappingArea(const EtaPhiRectangle &lhs,
                                        const EtaPhiRectangle &rhs) {
  return overlappingRectangle(lhs, rhs).area();
}
//--------------------------------------------------------------
EtaPhiRectangle EtaPhiRectangle::overlappingRectangle(const EtaPhiRectangle &lhs,
                                                      const EtaPhiRectangle &rhs) {
  double etaOverlap(0.), phiOverlap(0.);
  double deltaEta12(fabs(lhs.eta() - rhs.eta()));
  double totHalfEtaWidth(lhs.etaHalfWidth() + rhs.etaHalfWidth());
  double deltaPhi12(fabs(phi_mpi_pi(lhs.phi() - rhs.phi())));
  double totHalfPhiWidth(lhs.phiHalfWidth() + rhs.phiHalfWidth());
  if (deltaEta12 < totHalfEtaWidth) etaOverlap = totHalfEtaWidth - deltaEta12;
  if (deltaPhi12 < totHalfPhiWidth) phiOverlap = totHalfPhiWidth - deltaPhi12;
  double etaAvg(0.5*(lhs.eta() + rhs.eta()));
  double phiAvg(atan2(sin(lhs.phi()) + sin(rhs.phi()),
                      cos(lhs.phi()) + cos(rhs.phi())));

  return EtaPhiRectangle(etaAvg-0.5*etaOverlap, etaAvg+0.5*etaOverlap,
                         phiAvg-0.5*phiOverlap, phiAvg+0.5*phiOverlap);
}

//--------------------------------------------------------------
void EtaPhiRectangle::print(std::ostream& stream) const {
  stream<<"eta ["<<etaMin_<<", "<<etaMax_<<"], phi ["<<phiMin_<<", "<<phiMax_<<"]";
}
//--------------------------------------------------------------
namespace nsw {
  std::ostream& operator<< (std::ostream& stream, const EtaPhiRectangle &epr) {
    epr.print(stream);
    return stream;
  }
}
