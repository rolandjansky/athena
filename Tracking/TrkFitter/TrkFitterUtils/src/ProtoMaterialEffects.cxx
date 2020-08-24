/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkFitterUtils/ProtoMaterialEffects.h"
#include "TrkSurfaces/Surface.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"

namespace Trk{
ProtoMaterialEffects::ProtoMaterialEffects(){
  m_eloss=nullptr;
  m_scatphi=m_scattheta=m_sigmascatphi=m_sigmascattheta=m_x0=m_measscatphi=0;
  m_surf=nullptr;
  m_matprop=nullptr;
  m_deltap=0;
  m_sigmadeltae=0;
  m_sigmadeltaepos=0;
  m_sigmadeltaeneg=0;
  m_deltae=0;
  m_iskink=false;
  m_ismeasuredeloss=true;
  m_owneloss=false;
  m_x0=0;
  m_sintheta=1;
}

ProtoMaterialEffects::ProtoMaterialEffects(const MaterialEffectsOnTrack *meot){
  m_owneloss=false;
  m_sigmadeltae=0;
  if (meot->energyLoss()) {
    m_deltae=meot->energyLoss()->deltaE();
    m_sigmadeltaepos=meot->energyLoss()->sigmaPlusDeltaE();
    m_sigmadeltaeneg=meot->energyLoss()->sigmaMinusDeltaE();
    m_sigmadeltae=meot->energyLoss()->sigmaDeltaE();
    if (!meot->scatteringAngles()) {
      m_eloss=meot->energyLoss()->clone();
      m_owneloss=true;
    }
    else {
      m_eloss=nullptr;
    }
  }
  else {
    m_eloss=nullptr;
    m_deltae=0;
    m_sigmadeltaepos=0;
    m_sigmadeltaeneg=0;
  }
  //std::cout << "m_eloss: " << m_eloss << std::endl;
  m_x0=0;
  m_sintheta=1;

  double x0=meot->thicknessInX0();
  
  const ScatteringAngles *scatangles=meot->scatteringAngles();
  if (scatangles && x0>0){
    m_x0=x0;
    m_sintheta=scatangles->sigmaDeltaTheta()/scatangles->sigmaDeltaPhi();
    m_scatphi=scatangles->deltaPhi();
    m_scattheta=scatangles->deltaTheta();
    m_sigmascatphi=scatangles->sigmaDeltaPhi();
    //m_sigmascatphi=0.001;
    m_sigmascattheta=scatangles->sigmaDeltaTheta();
    //if (m_sigmascatphi!=0 && std::abs(m_scatphi/m_sigmascatphi)>10) m_scatphi*=(10*m_sigmascatphi/std::abs(m_scatphi));
    //if (m_sigmascattheta!=0 && std::abs(m_scattheta/m_sigmascattheta)>10) m_scattheta*=(10*m_sigmascattheta/std::abs(m_scattheta));
  }
  else m_scatphi=m_scattheta=m_sigmascatphi=m_sigmascattheta=0;
  m_surf=meot->associatedSurface().associatedDetectorElement() ?
         &meot->associatedSurface():
         meot->associatedSurface().clone();
  m_matprop=nullptr;

  m_iskink= false;//(m_scattheta>0.09) ? true : false;
  m_ismeasuredeloss=true;
  m_deltap=0;
  m_measscatphi=0;
}

ProtoMaterialEffects::ProtoMaterialEffects(const ProtoMaterialEffects& rhs) {
  m_eloss=rhs.m_eloss ? (rhs.m_owneloss ? rhs.m_eloss->clone() : rhs.m_eloss) : nullptr;
  m_scatphi=rhs.m_scatphi;
  m_scattheta=rhs.m_scattheta;
  m_sigmascatphi=rhs.m_sigmascatphi;
  m_sigmascattheta=rhs.m_sigmascattheta;
  m_x0=rhs.m_x0;
  m_deltap=rhs.m_deltap;
  m_deltae=rhs.m_deltae;
  m_sigmadeltae=rhs.m_sigmadeltae;
  m_sigmadeltaepos=rhs.m_sigmadeltaepos;
  m_sigmadeltaeneg=rhs.m_sigmadeltaeneg;
  m_iskink=rhs.m_iskink;
  m_ismeasuredeloss=rhs.m_ismeasuredeloss;
  if (rhs.m_surf) {
    m_surf = (!rhs.m_surf->associatedDetectorElement()) ?
      rhs.m_surf->clone() : rhs.m_surf;
  } else { m_surf = nullptr; }
  m_matprop=rhs.m_matprop;
  m_owneloss=rhs.m_owneloss;
  m_measscatphi=rhs.m_measscatphi;
  m_sintheta=rhs.m_sintheta;
  //m_pjac=rhs.m_pjac;
}

ProtoMaterialEffects& ProtoMaterialEffects::operator=(const ProtoMaterialEffects& rhs) {
  if (this!=&rhs){
    m_eloss=rhs.m_eloss ? (rhs.m_owneloss ? rhs.m_eloss->clone() : rhs.m_eloss) : nullptr;

    m_scatphi=rhs.m_scatphi;   
    m_scattheta=rhs.m_scattheta;   
    m_sigmascatphi=rhs.m_sigmascatphi;   
    m_sigmascattheta=rhs.m_sigmascattheta;   
    m_x0=rhs.m_x0;
    m_deltap=rhs.m_deltap;
    m_deltae=rhs.m_deltae;
    m_sigmadeltae=rhs.m_sigmadeltae;
    m_sigmadeltaepos=rhs.m_sigmadeltaepos;
    m_sigmadeltaeneg=rhs.m_sigmadeltaeneg;
    m_iskink=rhs.m_iskink;
    m_ismeasuredeloss=rhs.m_ismeasuredeloss;
    if (rhs.m_surf) {
      m_surf = (!rhs.m_surf->associatedDetectorElement()) ?
        rhs.m_surf->clone() : rhs.m_surf;
    } else { m_surf = nullptr; }
    m_matprop=rhs.m_matprop;
    m_measscatphi=rhs.m_measscatphi;
    m_sintheta=rhs.m_sintheta;
    //m_pjac=rhs.m_pjac;
  }
  return *this;
}

ProtoMaterialEffects::~ProtoMaterialEffects(){
  //std::cout << "destructing GXFMat" << std::endl;
  if (m_owneloss) delete m_eloss;
  if (m_surf && !m_surf->associatedDetectorElement()) delete m_surf;
}

void ProtoMaterialEffects::setScatteringAngles(double scatphi,double scattheta){
  m_scatphi=scatphi;
  m_scattheta=scattheta;
}

void ProtoMaterialEffects::setScatteringSigmas(double scatsigmaphi,double scatsigmatheta){
  m_sigmascatphi=scatsigmaphi;
  m_sigmascattheta=scatsigmatheta;
}

double ProtoMaterialEffects::x0() const{
  return m_x0;
}

void ProtoMaterialEffects::setX0(double x0){
  m_x0=x0;
}

double ProtoMaterialEffects::deltaPhi() const{
  return m_scatphi;
}

double ProtoMaterialEffects::measuredDeltaPhi() const{
  return m_measscatphi;
}

double ProtoMaterialEffects::deltaTheta() const {
  return m_scattheta;
}

double ProtoMaterialEffects::sigmaDeltaPhi() const {
  //return 0.0001;
  //if (m_iskink) return 0.05;
 // else 
return m_sigmascatphi;
}

double ProtoMaterialEffects::sigmaDeltaTheta() const {
  //if (m_iskink) return 0.05;
  //else 
  return m_sigmascattheta;
}


double ProtoMaterialEffects::deltaE() const {
  return m_deltae;
  //if (m_eloss) return m_eloss->deltaE();
  //return 0;
}

void ProtoMaterialEffects::setEloss(Trk::EnergyLoss *eloss){
  if (m_owneloss) delete m_eloss;
  m_eloss=eloss;
  m_owneloss=true;
}

double ProtoMaterialEffects::sigmaDeltaE() const {
  return m_sigmadeltae;
}

double ProtoMaterialEffects::sigmaDeltaEPos() const {
//  if (m_sigmadeltae!=0) 
return m_sigmadeltaepos;
  //return 0;
}

double ProtoMaterialEffects::sigmaDeltaENeg() const {
//  if (m_sigmadeltae!=0) 
return m_sigmadeltaeneg;
  //return 0;
}

double ProtoMaterialEffects::sigmaDeltaEAve() const {
  if (m_eloss) return m_eloss->sigmaDeltaE();
  return 0;
}

void ProtoMaterialEffects::setSigmaDeltaE(double sigmadeltae){
  m_sigmadeltae=sigmadeltae;
}

void ProtoMaterialEffects::setSigmaDeltaEPos(double sigmadeltaepos){
  m_sigmadeltaepos=sigmadeltaepos;
}

void ProtoMaterialEffects::setSigmaDeltaENeg(double sigmadeltaeneg){
  m_sigmadeltaeneg=sigmadeltaeneg;
}

void ProtoMaterialEffects::setDeltaE(double deltae){
  m_deltae=deltae;
}

void ProtoMaterialEffects::setMeasuredDeltaPhi(double measdf){
  m_measscatphi=measdf;
}

double ProtoMaterialEffects::delta_p() const{
  return m_deltap;
}

void ProtoMaterialEffects::setdelta_p(double delta_p){
  m_deltap=delta_p;
}


void ProtoMaterialEffects::setKink(bool iskink){
  m_iskink=iskink;
}

bool ProtoMaterialEffects::isKink() const {
  return m_iskink;
}

void ProtoMaterialEffects::setMeasuredEloss(bool ismeasuredeloss){
  m_ismeasuredeloss=ismeasuredeloss;
}

bool ProtoMaterialEffects::isMeasuredEloss() const {
  return m_ismeasuredeloss;
}

const Surface *ProtoMaterialEffects::surface() const{
  return m_surf;
}

void ProtoMaterialEffects::setSurface(const Surface *surf){
  if (m_surf && !m_surf->associatedDetectorElement()) delete m_surf;
  m_surf=surf;
}

MaterialEffectsBase *ProtoMaterialEffects::makeMEOT() const {
  ScatteringAngles *scatangles=nullptr;
  if (m_sigmascattheta!=0) scatangles=new ScatteringAngles(m_scatphi,m_scattheta,m_sigmascatphi,m_sigmascattheta);
  std::bitset<MaterialEffectsBase::NumberOfMaterialEffectsTypes> typePattern;
  typePattern.set(MaterialEffectsBase::FittedMaterialEffects);
  const Trk::EnergyLoss *neweloss=nullptr;
  //std::cout << "eloss: " << m_eloss << " iskink: " << m_iskink << std::endl;
  if (m_deltae!=0){
    //if (m_owneloss) neweloss=m_eloss;
    //else 
    //if (!m_iskink)     neweloss=m_eloss->clone();
    if (m_eloss) {
      neweloss=m_eloss;
      if (!m_owneloss) neweloss=neweloss->clone();
    }
    else neweloss=new Trk::EnergyLoss(m_deltae,m_sigmadeltae,m_sigmadeltaeneg,m_sigmadeltaepos);
    //std::cout << "deltae: " << neweloss->deltaE() << " sigde: " << neweloss->sigmaDeltaE() << " sigplus: " << neweloss->sigmaPlusDeltaE() << " sigminus: " << neweloss->sigmaMinusDeltaE() << std::endl;
  }
  MaterialEffectsOnTrack *meot=new MaterialEffectsOnTrack(m_x0,scatangles,neweloss,*m_surf,typePattern);
  const_cast<bool&>(m_owneloss)=false; //Non MT safe function
  //m_eloss=0;
  return meot;  
}

const Trk::MaterialProperties *ProtoMaterialEffects::materialProperties() const{
  return m_matprop;
}

void ProtoMaterialEffects::setMaterialProperties(const Trk::MaterialProperties *matprop){
  m_matprop=matprop;
}

/* std::vector<double> &ProtoMaterialEffects::momentumJacobians(){
  return m_pjac;
} */

}
