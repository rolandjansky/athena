/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelDistortionData.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "TMath.h"

std::vector<float> PixelDistortionData::getDistortionMap(uint32_t hashID) const {
  int distosize;
  if (m_version < 2) distosize = 3;
  else distosize = 441;

  std::vector<float> map(distosize, 0.0);
  auto itr = m_distortionMap.find(hashID);
  if (itr!=m_distortionMap.end()) { 
    return itr->second; 
  }
  return map;
}

unsigned long long PixelDistortionData::getId(uint32_t hashID) const {
  auto search = m_ids.find(hashID);
  if (search != m_ids.end()) return search->second;

  return 0;
}

Amg::Vector2D PixelDistortionData::correction(uint32_t hashID, const Amg::Vector2D & locpos, const Amg::Vector3D & direction) const {
  double localphi = locpos[0];
  double localeta = locpos[1];
  const Amg::Vector2D nullCorrection(0.0,0.0);
  const unsigned long long ull_id = getId(hashID);

  // No corrections available for this module
  if (ull_id == 0 && m_version > 1) { return nullCorrection; }

  // This is needed to avoid rounding errors if the z component of the
  // direction vector is too small.
  if (std::fabs(direction.z())<1.*CLHEP::micrometer) { return nullCorrection; }

  // If a particle has a too shallow trajectory with respect to the 
  // module direction, it is likely to be a secondary particle and no 
  // shift should be applied.
  double invtanphi = direction.x()/direction.z(); 
  double invtaneta = direction.y()/direction.z();
  if (sqrt(invtanphi*invtanphi+invtaneta*invtaneta)>100.0) { return nullCorrection; }

  double localZ = 0;
  std::vector<float> map = getDistortionMap(hashID);

  // If old database versions, the corrections are from the pixel survey, otherwise corrections are from
  // insitu shape measurements.
  if (m_version < 2) {
     localZ = getSurveyZ(localeta, localphi, map.data());
  } else if (isOldParam(ull_id)) {
     localZ = getSurveyZ(localeta, localphi, map.data());
  } else {
     float modEtaSize, modPhiSize;

     // set module sizes in millimetre
     if (ull_id >= 0x200000000000000 && ull_id <= 0x20d980000000000) {
       if (isIBL3D(ull_id)) {
         modEtaSize = 19.75 * CLHEP::millimeter;
         modPhiSize = 16.75 * CLHEP::millimeter;
       } else {
         modEtaSize = 40.4 * CLHEP::millimeter;
         modPhiSize = 16.8 * CLHEP::millimeter;
       }
     } else {
       modEtaSize = 60.2 * CLHEP::millimeter;
       modPhiSize = 16.44 * CLHEP::millimeter;
     }

     localZ = getInSituZ(localeta, modEtaSize, localphi, modPhiSize, map.data());
  }

  double localetaCor = -localZ * invtaneta;
  double localphiCor = -localZ * invtanphi;

  // In earlies code version there was a bug in the sign of the correction. 
  // In MC this was not seen as reco just corrects back what was done in digitization. 
  // In order to maintain backward compatibilty in older MC we need to reproduce this wrong behaviour.
  if (getVersion()==0) { localphiCor = -localphiCor; }
  
  return Amg::Vector2D(localphiCor, localetaCor); 
}

Amg::Vector2D PixelDistortionData::correctReconstruction(uint32_t hashID, const Amg::Vector2D & locpos, const Amg::Vector3D & direction) const {
  Amg::Vector2D newlocpos(locpos);
  newlocpos += correction(hashID, locpos, direction);
  return newlocpos;
}

Amg::Vector2D PixelDistortionData::correctSimulation(uint32_t hashID, const Amg::Vector2D & locpos, const Amg::Vector3D & direction) const {
  Amg::Vector2D newlocpos(locpos);
  newlocpos -= correction(hashID, locpos, direction);
  return newlocpos;
}

double PixelDistortionData::getInSituZ(const double localeta, const double eta_size, const double localphi, const double phi_size, const float *disto) const
{
  double etaHalfRangeBB = eta_size * 10. / 21.;
  double phiHalfRangeBB = phi_size * 10. / 21.;
  double etaRangeBB = eta_size * 20. / 21.;
  double phiRangeBB = phi_size * 20. / 21.;
  double eta, phi;

  // map positions on interval [- edge, 1 + edge]
  // edge is the part outside of the Bezier-Bernstein range
  if (std::abs(localeta) - etaHalfRangeBB > 0) {
    if (localeta < 0)
      eta = (localeta + etaHalfRangeBB) / etaRangeBB;
    else
      eta = 1. + (localeta - etaHalfRangeBB) / etaRangeBB;
  } else {
    eta = localeta / etaRangeBB + 0.5;
  }
  if (std::abs(localphi) - phiHalfRangeBB > 0) {
    if (localphi < 0)
      phi = (localphi + phiHalfRangeBB) / phiRangeBB;
    else
      phi = 1. + (localphi - phiHalfRangeBB) / phiRangeBB;
  } else {
    phi = localphi / phiRangeBB + 0.5;
  }
  return bernstein_bezier(&eta, &phi, disto);
}

double PixelDistortionData::getSurveyZ(const double localeta, const double localphi, const float *disto) const
{
  const double xFE = 22.0 * CLHEP::millimeter; // Distance between the 2 Front-End line, where bows have been measured
  const double yFE = 60.8 * CLHEP::millimeter; // Length of the active surface of each module

  double data0 = disto[0] / CLHEP::meter;           // curvature is in m-1
  double data1 = disto[1] / CLHEP::meter;           // curvature is in m-1
  double data2 = tan(0.5 * disto[2] * CLHEP::degree);   // twist angle in degree

  double twist1 = -data2;
  double twist2 = data2;
  double b1 = sqrt((1. + twist1*twist1) * (1. + twist1*twist1) * (1. + twist1*twist1));
  double b2 = sqrt((1. + twist2*twist2) * (1. + twist2*twist2) * (1. + twist2*twist2));
  double z1 = localeta * twist1 - 0.5 * b1 * localeta*localeta * data1;
  double z2 = localeta * twist2 - 0.5 * b2 * localeta*localeta * data0;
  double zoff1 = (b1 * yFE*yFE * data1) / 24.;
  double zoff2 = (b2 * yFE*yFE * data0) / 24.;
  z1 = z1 + zoff1;
  z2 = z2 + zoff2;

  return z1 + ((z2 - z1) / xFE) * (localphi + xFE / 2.);
}

double PixelDistortionData::bernstein_grundpolynom(const double *t, const int n, const int i) const
{
  return TMath::Binomial(n, i) * TMath::Power(*t, i) * TMath::Power(1. - *t, n - i);
}

double PixelDistortionData::bernstein_bezier(const double *u, const double *v, const float *P) const
{
  int n = 20;
  int m = 20;
  double r = 0.;

  for (int i = 0; i <= n; ++i) {
    for (int j = 0; j <= m; ++j) {
      int k = (i * (m + 1)) + j;
      if (P[k] < -998.9) continue;
      r += bernstein_grundpolynom(u, n, i) * bernstein_grundpolynom(v, m, j) * P[k];
    }
  }

  return r;
}

bool PixelDistortionData::isOldParam(const unsigned long long ull_id) const
{
  // Only pixel modules can have the old parametrisation
  if (ull_id < 0x240000000000000) return false;

  // For now: no new parametrisation for Pixel
  return true;
}

bool PixelDistortionData::isIBL3D(const unsigned long long ull_id) const
{
  // Stave 1
  if (ull_id >= 0x200000000000000 && ull_id <= 0x200180000000000) return true;
  if (ull_id >= 0x200800000000000 && ull_id <= 0x200980000000000) return true;

  // Stave 2
  if (ull_id >= 0x201000000000000 && ull_id <= 0x201180000000000) return true;
  if (ull_id >= 0x201800000000000 && ull_id <= 0x201980000000000) return true;

  // Stave 3
  if (ull_id >= 0x202000000000000 && ull_id <= 0x202180000000000) return true;
  if (ull_id >= 0x202800000000000 && ull_id <= 0x202980000000000) return true;

  // Stave 4
  if (ull_id >= 0x203000000000000 && ull_id <= 0x203180000000000) return true;
  if (ull_id >= 0x203800000000000 && ull_id <= 0x203980000000000) return true;

  // Stave 5
  if (ull_id >= 0x204000000000000 && ull_id <= 0x204180000000000) return true;
  if (ull_id >= 0x204800000000000 && ull_id <= 0x204980000000000) return true;

  // Stave 6
  if (ull_id >= 0x205000000000000 && ull_id <= 0x205180000000000) return true;
  if (ull_id >= 0x205800000000000 && ull_id <= 0x205980000000000) return true;

  // Stave 7
  if (ull_id >= 0x206000000000000 && ull_id <= 0x206180000000000) return true;
  if (ull_id >= 0x206800000000000 && ull_id <= 0x206980000000000) return true;

  // Stave 8
  if (ull_id >= 0x207000000000000 && ull_id <= 0x207180000000000) return true;
  if (ull_id >= 0x207800000000000 && ull_id <= 0x207980000000000) return true;

  // Stave 9
  if (ull_id >= 0x208000000000000 && ull_id <= 0x208180000000000) return true;
  if (ull_id >= 0x208800000000000 && ull_id <= 0x208980000000000) return true;

  // Stave 10
  if (ull_id >= 0x209000000000000 && ull_id <= 0x209180000000000) return true;
  if (ull_id >= 0x209800000000000 && ull_id <= 0x209980000000000) return true;

  // Stave 11
  if (ull_id >= 0x20a000000000000 && ull_id <= 0x20a180000000000) return true;
  if (ull_id >= 0x20a800000000000 && ull_id <= 0x20a980000000000) return true;

  // Stave 12
  if (ull_id >= 0x20b000000000000 && ull_id <= 0x20b180000000000) return true;
  if (ull_id >= 0x20b800000000000 && ull_id <= 0x20b980000000000) return true;

  // Stave 13
  if (ull_id >= 0x20c000000000000 && ull_id <= 0x20c180000000000) return true;
  if (ull_id >= 0x20c800000000000 && ull_id <= 0x20c980000000000) return true;

  // Stave 14
  if (ull_id >= 0x20d000000000000 && ull_id <= 0x20d180000000000) return true;
  if (ull_id >= 0x20d800000000000 && ull_id <= 0x20d980000000000) return true;

  return false;
}

void PixelDistortionData::clear() {
  m_distortionMap.clear();
}

