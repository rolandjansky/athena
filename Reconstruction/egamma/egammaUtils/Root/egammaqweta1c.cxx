/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaUtils/egammaqweta1c.h"
#include "xAODCaloEvent/CaloCluster.h"
#include <cmath>

float
egammaqweta1c::Correct(const float eta, const float etacell, const float width)
{

  //Make correction to the width of the cluster shower in sampling 1
  const float aeta = fabs(eta);
  const float etarel = RelPosition(eta, etacell);
  if (aeta < 1.0) {
    return (width - 0.76 * pow(etarel, 2));
  }
  if (aeta < 1.45) {
    return (width - 0.85 * pow(etarel, 2) + 1.9 * pow(etarel, 4));
  }
  if (aeta < 1.5) {
    return width;
  }
  if (aeta < 1.8) {
    return (width - 0.85 * pow(etarel, 2) + 1.9 * pow(etarel, 4));
  }
  if (aeta < 2.0) {
    return (width - 0.84 * pow(etarel, 2));
  }
  if (aeta < 2.5) {
    return (width - 0.40 * pow(etarel, 2) - 2.1 * pow(etarel, 4));
  }
  return width;
}

double
egammaqweta1c::RelPosition(const float eta, const float etacell)
{
  /*
   * get relative position within cell in eta of the cluster
   * shower in sampling 1
   */
  const float aeta = fabs(eta);
  if (aeta < 1.8) {
    const float ngra = 8.;
    const double dgra = 0.025 / ngra;
    const double etapos = fabs(eta - etacell - dgra / 2.);
    return (fmod(etapos, dgra) / dgra - 0.5);
  }
  if (aeta < 2.0) {
    const float ngra = 6.;
    const float dgra = 0.025 / ngra;
    const double etapos = fabs(eta - etacell - dgra / 2.);
    return (fmod(etapos, dgra) / dgra - 0.5);
  }
  if (aeta < 2.4) {
    const float ngra = 4.;
    const float dgra = 0.025 / ngra;
    const double etapos = fabs(eta - etacell - dgra / 2.);
    return (fmod(etapos, dgra) / dgra - 0.5);
  }
  if (aeta < 2.5) {
    const float ngra = 1.;
    const float dgra = 0.025 / ngra;
    const double etapos = fabs(eta - etacell - dgra / 2.);
    return (fmod(etapos, dgra) / dgra - 0.5);
  }

  return -999.;
}

