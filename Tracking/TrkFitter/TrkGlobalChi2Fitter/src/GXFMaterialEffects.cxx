/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkGlobalChi2Fitter/GXFMaterialEffects.h"
#include "TrkSurfaces/Surface.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"

namespace Trk {
  GXFMaterialEffects::GXFMaterialEffects() {
    m_eloss = 0;
    m_scatphi = m_scattheta = m_sigmascatphi = m_sigmascattheta = m_x0 = m_measscatphi = 0;
    m_surf = 0;
    m_matprop = 0;
    m_deltap = 0;
    m_sigmadeltae = 0;
    m_sigmadeltaepos = 0;
    m_sigmadeltaeneg = 0;
    m_deltae = 0;
    m_iskink = false;
    m_ismeasuredeloss = true;
    m_owneloss = false;
    m_x0 = 0;
    m_sintheta = 1;
  }

  GXFMaterialEffects::GXFMaterialEffects(const MaterialEffectsOnTrack *meot) {
    m_owneloss = false;
    m_sigmadeltae = 0;
    if (meot->energyLoss()) {
      m_deltae = meot->energyLoss()->deltaE();
      m_sigmadeltaepos = meot->energyLoss()->sigmaPlusDeltaE();
      m_sigmadeltaeneg = meot->energyLoss()->sigmaMinusDeltaE();
      if (!meot->scatteringAngles()) {
        m_eloss = meot->energyLoss()->clone();
        m_sigmadeltae = meot->energyLoss()->sigmaDeltaE();
        m_owneloss = true;
      }else {
        m_eloss = 0;
      }
    }else {
      m_eloss = 0;
      m_deltae = 0;
      m_sigmadeltaepos = 0;
      m_sigmadeltaeneg = 0;
    }
    // std::cout << "m_eloss: " << m_eloss << std::endl;
    m_x0 = 0;
    m_sintheta = 1;

    double x0 = meot->thicknessInX0();

    const ScatteringAngles *scatangles = meot->scatteringAngles();
    if (scatangles && x0 > 0) {
      m_x0 = x0;
      m_sintheta = scatangles->sigmaDeltaTheta() / scatangles->sigmaDeltaPhi();
      m_scatphi = scatangles->deltaPhi();
      m_scattheta = scatangles->deltaTheta();
      m_sigmascatphi = scatangles->sigmaDeltaPhi();
      // m_sigmascatphi=0.001;
      m_sigmascattheta = scatangles->sigmaDeltaTheta();
      // if (m_sigmascatphi!=0 && std::abs(m_scatphi/m_sigmascatphi)>10)
      // m_scatphi*=(10*m_sigmascatphi/std::abs(m_scatphi));
      // if (m_sigmascattheta!=0 && std::abs(m_scattheta/m_sigmascattheta)>10)
      // m_scattheta*=(10*m_sigmascattheta/std::abs(m_scattheta));
    }else {
      m_scatphi = m_scattheta = m_sigmascatphi = m_sigmascattheta = 0;
    }
    m_surf = &meot->associatedSurface();
    m_matprop = 0;

    m_iskink = false;// (m_scattheta>0.09) ? true : false;
    m_ismeasuredeloss = true;
    m_deltap = 0;
    m_measscatphi = 0;
  }

  GXFMaterialEffects::GXFMaterialEffects(GXFMaterialEffects &rhs) {
    m_eloss = rhs.m_eloss ? (rhs.m_owneloss ? rhs.m_eloss->clone() : rhs.m_eloss) : 0;
    m_scatphi = rhs.m_scatphi;
    m_scattheta = rhs.m_scattheta;
    m_sigmascatphi = rhs.m_sigmascatphi;
    m_sigmascattheta = rhs.m_sigmascattheta;
    m_x0 = rhs.m_x0;
    m_deltap = rhs.m_deltap;
    m_deltae = rhs.m_deltae;
    m_sigmadeltae = rhs.m_sigmadeltae;
    m_sigmadeltaepos = rhs.m_sigmadeltaepos;
    m_sigmadeltaeneg = rhs.m_sigmadeltaeneg;
    m_iskink = rhs.m_iskink;
    m_ismeasuredeloss = rhs.m_ismeasuredeloss;
    m_surf = rhs.m_surf;
    m_matprop = rhs.m_matprop;
    m_owneloss = rhs.m_owneloss;
    m_measscatphi = rhs.m_measscatphi;
    m_sintheta = rhs.m_sintheta;
    m_owneloss = rhs.m_owneloss;
  }

  GXFMaterialEffects &
  GXFMaterialEffects::operator = (GXFMaterialEffects &rhs) {
    if (this != &rhs) {
      m_eloss = rhs.m_eloss ? (rhs.m_owneloss ? rhs.m_eloss->clone() : rhs.m_eloss) : 0;

      m_scatphi = rhs.m_scatphi;
      m_scattheta = rhs.m_scattheta;
      m_sigmascatphi = rhs.m_sigmascatphi;
      m_sigmascattheta = rhs.m_sigmascattheta;
      m_x0 = rhs.m_x0;
      m_deltap = rhs.m_deltap;
      m_deltae = rhs.m_deltae;
      m_sigmadeltae = rhs.m_sigmadeltae;
      m_sigmadeltaepos = rhs.m_sigmadeltaepos;
      m_sigmadeltaeneg = rhs.m_sigmadeltaeneg;
      m_iskink = rhs.m_iskink;
      m_ismeasuredeloss = rhs.m_ismeasuredeloss;
      m_surf = rhs.m_surf;
      m_matprop = rhs.m_matprop;
      m_measscatphi = rhs.m_measscatphi;
      m_sintheta = rhs.m_sintheta;
      m_owneloss = rhs.m_owneloss;
    }
    return *this;
  }

  GXFMaterialEffects::~GXFMaterialEffects() {
    // std::cout << "destructing GXFMat" << std::endl;
    if (m_owneloss) {
      delete m_eloss;
    }
  }

  void
  GXFMaterialEffects::setScatteringAngles(double scatphi, double scattheta) {
    m_scatphi = scatphi;
    m_scattheta = scattheta;
  }

  void
  GXFMaterialEffects::setScatteringSigmas(double scatsigmaphi, double scatsigmatheta) {
    m_sigmascatphi = scatsigmaphi;
    m_sigmascattheta = scatsigmatheta;
  }

  double
  GXFMaterialEffects::x0() {
    return m_x0;
  }

  void
  GXFMaterialEffects::setX0(double x0) {
    m_x0 = x0;
  }

  double
  GXFMaterialEffects::deltaPhi() {
    return m_scatphi;
  }

  double
  GXFMaterialEffects::measuredDeltaPhi() {
    return m_measscatphi;
  }

  double
  GXFMaterialEffects::deltaTheta() {
    return m_scattheta;
  }

  double
  GXFMaterialEffects::sigmaDeltaPhi() {
    // return 0.0001;
    // if (m_iskink) return 0.05;
    // else
    return m_sigmascatphi;
  }

  double
  GXFMaterialEffects::sigmaDeltaTheta() {
    // if (m_iskink) return 0.05;
    // else
    return m_sigmascattheta;
  }

  double
  GXFMaterialEffects::deltaE() {
    return m_deltae;
    // if (m_eloss) return m_eloss->deltaE();
    // return 0;
  }

  void
  GXFMaterialEffects::setEloss(Trk::EnergyLoss *eloss) {
    if (m_owneloss) {
      delete m_eloss;
    }
    m_eloss = eloss;
    m_owneloss = true;
  }

  double
  GXFMaterialEffects::sigmaDeltaE() {
    return m_sigmadeltae;
  }

  double
  GXFMaterialEffects::sigmaDeltaEPos() {
//  if (m_sigmadeltae!=0)
    return m_sigmadeltaepos;
    // return 0;
  }

  double
  GXFMaterialEffects::sigmaDeltaENeg() {
//  if (m_sigmadeltae!=0)
    return m_sigmadeltaeneg;
    // return 0;
  }

  double
  GXFMaterialEffects::sigmaDeltaEAve() {
    if (m_eloss) {
      return m_eloss->sigmaDeltaE();
    }
    return 0;
  }

  void
  GXFMaterialEffects::setSigmaDeltaE(double sigmadeltae) {
    m_sigmadeltae = sigmadeltae;
  }

  void
  GXFMaterialEffects::setSigmaDeltaEPos(double sigmadeltaepos) {
    m_sigmadeltaepos = sigmadeltaepos;
  }

  void
  GXFMaterialEffects::setSigmaDeltaENeg(double sigmadeltaeneg) {
    m_sigmadeltaeneg = sigmadeltaeneg;
  }

  void
  GXFMaterialEffects::setDeltaE(double deltae) {
    m_deltae = deltae;
  }

  void
  GXFMaterialEffects::setMeasuredDeltaPhi(double measdf) {
    m_measscatphi = measdf;
  }

  double
  GXFMaterialEffects::delta_p() {
    return m_deltap;
  }

  void
  GXFMaterialEffects::setdelta_p(double delta_p) {
    m_deltap = delta_p;
  }

  void
  GXFMaterialEffects::setKink(bool iskink) {
    m_iskink = iskink;
  }

  bool
  GXFMaterialEffects::isKink() {
    return m_iskink;
  }

  void
  GXFMaterialEffects::setMeasuredEloss(bool ismeasuredeloss) {
    m_ismeasuredeloss = ismeasuredeloss;
  }

  bool
  GXFMaterialEffects::isMeasuredEloss() {
    return m_ismeasuredeloss;
  }

  const Surface *
  GXFMaterialEffects::surface() {
    return m_surf;
  }

  void
  GXFMaterialEffects::setSurface(const Surface *surf) {
    m_surf = surf;
  }

  MaterialEffectsBase *
  GXFMaterialEffects::makeMEOT() {
    ScatteringAngles *scatangles = 0;

    if (m_sigmascattheta != 0) {
      scatangles = new ScatteringAngles(m_scatphi, m_scattheta, m_sigmascatphi, m_sigmascattheta);
    }
    std::bitset<MaterialEffectsBase::NumberOfMaterialEffectsTypes> typePattern;
    typePattern.set(MaterialEffectsBase::FittedMaterialEffects);
    const Trk::EnergyLoss *neweloss = 0;
    // std::cout << "eloss: " << m_eloss << " iskink: " << m_iskink << std::endl;
    if (m_deltae != 0) {
      // if (m_owneloss) neweloss=m_eloss;
      // else
      // if (!m_iskink)     neweloss=m_eloss->clone();
      if (m_eloss) {
        neweloss = m_eloss;
        if (!m_owneloss) {
          neweloss = neweloss->clone();
        }
      }else {
        neweloss = new Trk::EnergyLoss(m_deltae, m_sigmadeltae, m_sigmadeltaeneg, m_sigmadeltaepos);
      }
      // std::cout << "deltae: " << neweloss->deltaE() << " sigde: " << neweloss->sigmaDeltaE() << " sigplus: " <<
      // neweloss->sigmaPlusDeltaE() << " sigminus: " << neweloss->sigmaMinusDeltaE() << std::endl;
    }
    MaterialEffectsOnTrack *meot = new MaterialEffectsOnTrack(m_x0, scatangles, neweloss, *m_surf, typePattern);
    m_owneloss = false;
    // m_eloss=0;
    return meot;
  }

  const Trk::MaterialProperties *
  GXFMaterialEffects::materialProperties() {
    return m_matprop;
  }

  void
  GXFMaterialEffects::setMaterialProperties(const Trk::MaterialProperties *matprop) {
    m_matprop = matprop;
  }

/* std::vector<double> &GXFMaterialEffects::momentumJacobians(){
   return m_pjac;
   } */
}
