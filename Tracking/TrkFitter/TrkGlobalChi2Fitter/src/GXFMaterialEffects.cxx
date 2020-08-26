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
    m_eloss = nullptr;
    m_scatphi = m_scattheta = m_sigmascatphi = m_sigmascattheta = m_x0 = m_measscatphi = 0;
    m_surf = nullptr;
    m_matprop = nullptr;
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
  
  GXFMaterialEffects::GXFMaterialEffects(const MaterialEffectsOnTrack * meot) {
    m_owneloss = false;
    m_sigmadeltae = 0;
    
    if (meot->energyLoss() != nullptr) {
      m_deltae = meot->energyLoss()->deltaE();
      m_sigmadeltaepos = meot->energyLoss()->sigmaPlusDeltaE();
      m_sigmadeltaeneg = meot->energyLoss()->sigmaMinusDeltaE();
      
      if (meot->scatteringAngles() == nullptr) {
        m_eloss = meot->energyLoss()->clone();
        m_sigmadeltae = meot->energyLoss()->sigmaDeltaE();
        m_owneloss = true;
      } else {
        m_eloss = nullptr;
      }
    } else {
      m_eloss = nullptr;
      m_deltae = 0;
      m_sigmadeltaepos = 0;
      m_sigmadeltaeneg = 0;
    }

    m_x0 = 0;
    m_sintheta = 1;

    double x0 = meot->thicknessInX0();

    const ScatteringAngles *scatangles = meot->scatteringAngles();
    
    if ((scatangles != nullptr) && x0 > 0) {
      m_x0 = x0;
      m_sintheta = scatangles->sigmaDeltaTheta() / scatangles->sigmaDeltaPhi();
      m_scatphi = scatangles->deltaPhi();
      m_scattheta = scatangles->deltaTheta();
      m_sigmascatphi = scatangles->sigmaDeltaPhi();
      m_sigmascattheta = scatangles->sigmaDeltaTheta();
    } else {
      m_scatphi = m_scattheta = m_sigmascatphi = m_sigmascattheta = 0;
    }
    
    m_surf = &meot->associatedSurface();
    m_matprop = nullptr;
    m_iskink = false;
    m_ismeasuredeloss = true;
    m_deltap = 0;
    m_measscatphi = 0;
  }

  GXFMaterialEffects::GXFMaterialEffects(GXFMaterialEffects & rhs) {
    m_eloss = rhs.m_eloss != nullptr ? (rhs.m_owneloss ? rhs.m_eloss->clone() : rhs.m_eloss) : nullptr;
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

  GXFMaterialEffects & GXFMaterialEffects::operator =(GXFMaterialEffects & rhs) {
    if (this != &rhs) {
      m_eloss = rhs.m_eloss != nullptr ? (rhs.m_owneloss ? rhs.m_eloss->clone() : rhs.m_eloss) : nullptr;
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
    if (m_owneloss) {
      delete m_eloss;
    }
  }

  void GXFMaterialEffects::setScatteringAngles(double scatphi, double scattheta) {
    m_scatphi = scatphi;
    m_scattheta = scattheta;
  }

  void GXFMaterialEffects::setScatteringSigmas(double scatsigmaphi, double scatsigmatheta) {
    m_sigmascatphi = scatsigmaphi;
    m_sigmascattheta = scatsigmatheta;
  }

  double GXFMaterialEffects::x0() const {
    return m_x0;
  }

  void GXFMaterialEffects::setX0(double x0) {
    m_x0 = x0;
  }

  double GXFMaterialEffects::deltaPhi() const {
    return m_scatphi;
  }

  double GXFMaterialEffects::measuredDeltaPhi() const {
    return m_measscatphi;
  }

  double GXFMaterialEffects::deltaTheta() const {
    return m_scattheta;
  }

  double GXFMaterialEffects::sigmaDeltaPhi() const {
    return m_sigmascatphi;
  }

  double GXFMaterialEffects::sigmaDeltaTheta() const {
    return m_sigmascattheta;
  }

  double GXFMaterialEffects::deltaE() const {
    return m_deltae;
  }

  void GXFMaterialEffects::setEloss(Trk::EnergyLoss * eloss) {
    if (m_owneloss) {
      delete m_eloss;
    }
    m_eloss = eloss;
    m_owneloss = true;
  }

  double GXFMaterialEffects::sigmaDeltaE() const {
    return m_sigmadeltae;
  }

  double GXFMaterialEffects::sigmaDeltaEPos() const {
    return m_sigmadeltaepos;
  }

  double GXFMaterialEffects::sigmaDeltaENeg() const {
    return m_sigmadeltaeneg;
  }

  double GXFMaterialEffects::sigmaDeltaEAve() const {
    if (m_eloss != nullptr) {
      return m_eloss->sigmaDeltaE();
    }
    return 0;
  }

  void GXFMaterialEffects::setSigmaDeltaE(double sigmadeltae) {
    m_sigmadeltae = sigmadeltae;
  }

  void GXFMaterialEffects::setSigmaDeltaEPos(double sigmadeltaepos) {
    m_sigmadeltaepos = sigmadeltaepos;
  }

  void GXFMaterialEffects::setSigmaDeltaENeg(double sigmadeltaeneg) {
    m_sigmadeltaeneg = sigmadeltaeneg;
  }

  void GXFMaterialEffects::setDeltaE(double deltae) {
    m_deltae = deltae;
  }

  void GXFMaterialEffects::setMeasuredDeltaPhi(double measdf) {
    m_measscatphi = measdf;
  }

  double GXFMaterialEffects::delta_p() const {
    return m_deltap;
  }

  void GXFMaterialEffects::setdelta_p(double delta_p) {
    m_deltap = delta_p;
  }

  void GXFMaterialEffects::setKink(bool iskink) {
    m_iskink = iskink;
  }

  bool GXFMaterialEffects::isKink() const {
    return m_iskink;
  }

  void GXFMaterialEffects::setMeasuredEloss(bool ismeasuredeloss) {
    m_ismeasuredeloss = ismeasuredeloss;
  }

  bool GXFMaterialEffects::isMeasuredEloss() const {
    return m_ismeasuredeloss;
  }

  const Surface *GXFMaterialEffects::surface() const {
    return m_surf;
  }

  void GXFMaterialEffects::setSurface(const Surface * surf) {
    m_surf = surf;
  }

  MaterialEffectsBase *GXFMaterialEffects::makeMEOT() {
    ScatteringAngles *scatangles = nullptr;

    if (m_sigmascattheta != 0) {
      scatangles = new ScatteringAngles(m_scatphi, m_scattheta, m_sigmascatphi, m_sigmascattheta);
    }
    
    std::bitset<MaterialEffectsBase::NumberOfMaterialEffectsTypes> typePattern;
    typePattern.set(MaterialEffectsBase::FittedMaterialEffects);
    const Trk::EnergyLoss * neweloss = nullptr;
    
    if (m_deltae != 0) {
      if (m_eloss != nullptr) {
        neweloss = m_eloss;
        if (!m_owneloss) {
          neweloss = neweloss->clone();
        }
      } else {
        neweloss = new Trk::EnergyLoss(m_deltae, m_sigmadeltae, m_sigmadeltaeneg, m_sigmadeltaepos);
      }
    }
    
    MaterialEffectsOnTrack *meot = new MaterialEffectsOnTrack(m_x0, scatangles, neweloss, *m_surf, typePattern);
                                 
    m_owneloss = false;
    
    return meot;
  }

  const Trk::MaterialProperties * GXFMaterialEffects::materialProperties() const {
    return m_matprop;
  }

  void GXFMaterialEffects::setMaterialProperties(const Trk::MaterialProperties * matprop) {
    m_matprop = matprop;
  }
}
