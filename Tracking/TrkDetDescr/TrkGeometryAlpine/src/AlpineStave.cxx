/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkGeometryAlpine/AlpineStave.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/RectangleBounds.h"
#include <TMath.h>

Trk::AlpineStave::AlpineStave(double plainStart, double plainStep, double shift, std::vector<double> mountainPositions)
{
  m_plainStart    = plainStart+shift;
  m_plainStep     = plainStep;
  m_plainEnd      = m_plainStart+m_plainModules.size()*m_plainStep;
  m_mountainShift = shift; 
  if(m_plainStep==0) {
    std::cerr << "WARNING: plainStep=0 set to 1e-9" << std::endl;
    m_plainStep = 1e-9;
  }
  m_mountainPosPositions = new std::vector<double>;
  m_mountainNegPositions = new std::vector<double>;
  m_ownVector = true;
  for(unsigned int i=0;i<mountainPositions.size();i++) {
    m_mountainPosPositions->push_back(mountainPositions[i]);
    m_mountainNegPositions->push_back(-mountainPositions[i]);
  }
}

Trk::AlpineStave::AlpineStave(double plainStart, double plainStep, double shift, std::vector<double> mountainPosPositions, std::vector<double> mountainNegPositions)
{
  m_plainStart    = plainStart+shift;
  m_plainStep     = plainStep;
  m_plainEnd      = m_plainStart+m_plainModules.size()*m_plainStep;
  m_mountainShift = shift; 
  if(m_plainStep==0) {
    std::cerr << "WARNING: plainStep=0 set to 1e-9" << std::endl;
    m_plainStep = 1e-9;
  }
  m_mountainPosPositions = new std::vector<double>;
  m_mountainNegPositions = new std::vector<double>;
  m_ownVector = true;
  for(unsigned int i=0;i<mountainPosPositions.size();i++) {
    m_mountainPosPositions->push_back(mountainPosPositions[i]);
  }
  for(unsigned int i=0;i<mountainNegPositions.size();i++) {
    m_mountainNegPositions->push_back(mountainNegPositions[i]);
  }
}

Trk::AlpineStave::AlpineStave(double plainStart, double plainStep, double shift, std::vector<double> *mountainPosPositions, std::vector<double> *mountainNegPositions)
{
  m_plainStart    = plainStart+shift;
  m_plainStep     = plainStep;
  m_plainEnd      = m_plainStart+m_plainModules.size()*m_plainStep;
  m_mountainShift = shift; 
  if(m_plainStep==0) {
    std::cerr << "WARNING: plainStep=0 set to 1e-9" << std::endl;
    m_plainStep = 1e-9;
  }
  m_ownVector = false;
  m_mountainPosPositions = mountainPosPositions;
  m_mountainNegPositions = mountainNegPositions;
}

Trk::AlpineStave::~AlpineStave()
{
  // Delete the vector of mountains if we own it
  if(m_ownVector) {
    delete m_mountainPosPositions;
    delete m_mountainNegPositions;
  }
}

// RETURN SURFACES
std::vector<const Trk::Surface*>& Trk::AlpineStave::getSurfaces(){

  int nmodules;

  // Plain modules
  nmodules = m_plainModules.size();
  for(int imodule=0;imodule<nmodules;imodule++) {
    Trk::TrkDetElementBase* module = (Trk::TrkDetElementBase*) m_plainModules.at(imodule);
    const Trk::Surface* moduleSurface = &module->surface();
    if(!moduleSurface) continue;
    m_surfaces.push_back(moduleSurface);
  }

  // Plain transition modules
  nmodules = m_transitionModules.size();
  for(int imodule=0;imodule<nmodules;imodule++) {
    Trk::TrkDetElementBase* module = (Trk::TrkDetElementBase*) m_transitionModules.at(imodule);
    const Trk::Surface* moduleSurface = &module->surface();
    if(!moduleSurface) continue;
    m_surfaces.push_back(moduleSurface);
  }

  // Mountain modules >0
  nmodules = m_mountainPosModules.size();
  for(int imodule=0;imodule<nmodules;imodule++) {
    Trk::TrkDetElementBase* module = (Trk::TrkDetElementBase*) m_mountainPosModules.at(imodule);
    const Trk::Surface* moduleSurface = &module->surface();
    if(!moduleSurface) continue;
    m_surfaces.push_back(moduleSurface);
  }

  // Mountain modules <0
  nmodules = m_mountainNegModules.size();
  for(int imodule=0;imodule<nmodules;imodule++) {
    Trk::TrkDetElementBase* module = (Trk::TrkDetElementBase*) m_mountainNegModules.at(imodule);
    const Trk::Surface* moduleSurface = &module->surface();
    if(!moduleSurface) continue;
    m_surfaces.push_back(moduleSurface);
  }
  
  return m_surfaces;
}

// RETURN PLAIN MODULES
void Trk::AlpineStave::pushPlainModules(std::vector<const Trk::Surface*> &v, double z_inner, double z_outer) const 
{

  if(z_outer<m_plainStart-m_plainStep) return; 
  if(z_outer>m_plainEnd+m_plainStep)   return; 

  //int n = int(m_plainModules.size());
  unsigned int imodule_outer = 0;
  unsigned int imodule_inner = 0;
  if(z_outer>m_plainStart) imodule_outer = (z_outer - m_plainStart) / m_plainStep;
  if(z_inner>m_plainStart) imodule_inner = (z_inner - m_plainStart) / m_plainStep;

  unsigned int istart=imodule_inner; unsigned int iend=imodule_outer;
  if(z_inner>z_outer) {
    istart=imodule_outer; iend=imodule_inner;
  }

  // dirty fix for missing chunks when particles enter modules through the sides 
  iend++;
  if(istart>0) istart--;
  if(iend>m_plainModules.size()) iend = m_plainModules.size();

  // push plain modules
  for(unsigned int i=istart;i<iend;i++) {
    const Trk::Surface* s = &(m_plainModules[i]->surface());
    if(find(v.begin(),v.end(),s)==v.end()) v.push_back(s);
  }
  
  return;

}

// RETURN MOUNTAIN MODULES
void Trk::AlpineStave::pushMountainModules(std::vector<const Trk::Surface*> &v, double z_inner, double z_outer) const 
{
  if(z_outer<m_plainStart+m_plainStep || z_outer>m_plainEnd-m_plainStep){ 
    if(z_outer>0) pushMountainPosModules(v,z_inner-m_mountainShift,z_outer-m_mountainShift);
    else pushMountainNegModules(v,-z_inner+m_mountainShift,-z_outer+m_mountainShift); 
  }
  return;
}

// RETURN POSITIVE MOUNTAIN MODULES
void Trk::AlpineStave::pushMountainPosModules(std::vector<const Trk::Surface*> &v, double z_inner, double z_outer) const 
{
  int n = int(m_mountainPosPositions->size());
  if(n<=0) return;  

  if(m_mountainPosPositions->size()>m_mountainPosModules.size()) {
    std::cout << "WARNING: Mountain position vector size is bigger than mountain positive module vector!" << std::endl;
    n = int(m_mountainPosModules.size());
  }

  // here we start thinking a bit
  int iend=n-1; 
  while( z_outer < m_mountainPosPositions->at(iend) && iend>0 ) {
    iend--;
  }
  int istart=n-1;
  while( z_inner < m_mountainPosPositions->at(istart) && istart>0){
    istart--;
  }

  if(istart>iend) {int itmp = istart;istart=iend;iend=itmp;}

  // and returns the mountains
  for(int i=istart;i<=iend;i++) {
    const Trk::Surface* s = &(m_mountainPosModules[i]->surface());
    if(find(v.begin(),v.end(),s)==v.end()) v.push_back(s);
  }
  
  return;
}

// RETURN NEGATIVE MOUNTAIN MODULES
void Trk::AlpineStave::pushMountainNegModules(std::vector<const Trk::Surface*> &v, double z_inner, double z_outer) const 
{
  // WARNING WARNING WARNING:
  // Mountain positions are absolute numbers
  // So z_inner and z_outer should have their sign changed as done in pushMountainModules() method
  int n = int(m_mountainNegPositions->size());
  if(n<=0) return;

  if(m_mountainNegPositions->size()>m_mountainNegModules.size()) {
    std::cout << "WARNING: Mountain position vector size is bigger than mountain negative module vector!" << std::endl;
    n = int(m_mountainNegModules.size());
  }

  // here we start thinking a bit
  int iend=n-1; 
  while( z_outer < m_mountainNegPositions->at(iend) && iend>0 ) {
    //std::cout << " ---check  " << iend << " z_outer=" << z_outer << " mountain pos=" << m_mountainNegPositions->at(iend) << std::endl;
    iend--;
  }
  int istart=n-1;
  while( z_inner < m_mountainNegPositions->at(istart) && istart>0) {
    istart--;
  }

  if(istart>iend) {int itmp = istart;istart=iend;iend=itmp;}

  // and returns the mountains
  for(int i=istart;i<=iend;i++) {
    const Trk::Surface* s = &(m_mountainNegModules[i]->surface());
    if(find(v.begin(),v.end(),s)==v.end()) v.push_back(s);
  }
  return;
}

// UTILITIES
void Trk::AlpineStave::dump2D(std::ofstream &fout)
{
  unsigned int nmodules;
  fout << "    // ---------------------- Plain module surfaces -------------------------------// " << std::endl;
  nmodules = m_plainModules.size();
  for(unsigned int imodule=0;imodule<nmodules;imodule++) {
    Trk::TrkDetElementBase* module = (Trk::TrkDetElementBase*) m_plainModules.at(imodule);
    const Trk::Surface& sf = module->surface();
    // this version handles only planar bounds
    const Trk::RectangleBounds* recBo = dynamic_cast<const Trk::RectangleBounds*>(&(sf.bounds()));
    if (!recBo) continue;

    // surface center
    double x = sf.center().x();
    double y = sf.center().y();
    double z = sf.center().z();
    double r = sqrt(x*x+y*y);
    // it is a rectangle surface
    double halfL = recBo->halflengthY(); // Y is the module length, X its width
    fout << "    l = new TLine(" << z-halfL << "," << r << "," << z+halfL << "," << r << "); l->SetLineColor(kMagenta+2); l->SetLineWidth(4); l->Draw(\"same\"); // Plain module " << imodule << std::endl;
  }
  fout << "    // ---------------------- Transition module surfaces -------------------------------// " << std::endl;
  nmodules = m_transitionModules.size();
  for(unsigned int imodule=0;imodule<nmodules;imodule++) {
    Trk::TrkDetElementBase* module = (Trk::TrkDetElementBase*) m_transitionModules.at(imodule);
    const Trk::Surface& sf = module->surface();
    // this version handles only planar bounds
    const Trk::RectangleBounds* recBo = dynamic_cast<const Trk::RectangleBounds*>(&(sf.bounds()));
    if (!recBo) continue;

    // surface center
    double x = sf.center().x();
    double y = sf.center().y();
    double z = sf.center().z();
    double r = sqrt(x*x+y*y);
    // it is a rectangle surface
    double halfL = recBo->halflengthY(); // Y is the module length, X its width
    fout << "    l = new TLine(" << z-halfL << "," << r << "," << z+halfL << "," << r << "); l->SetLineColor(kMagenta+2); l->SetLineWidth(4); l->Draw(\"same\"); // Plain module " << imodule << std::endl;
  }
  fout << "    // ---------------------- Negative mountains module surfaces -------------------------------// " << std::endl;
  nmodules = m_mountainNegModules.size();
  for(unsigned int imodule=0;imodule<nmodules;imodule++) {
    Trk::TrkDetElementBase* module = (Trk::TrkDetElementBase*) m_mountainNegModules.at(imodule);
    const Trk::Surface& sf = module->surface();
    // this version handles only planar bounds
    const Trk::RectangleBounds* recBo = dynamic_cast<const Trk::RectangleBounds*>(&(sf.bounds()));
    if (!recBo) continue;

    // surface center
    double x = sf.center().x();
    double y = sf.center().y();
    double z = sf.center().z();
    double r = sqrt(x*x+y*y);
    double angle = 0.28;
    // it is a rectangle surface
    double halfL = recBo->halflengthY(); // Y is the module length, X its width
    fout << "    l = new TLine(" << z+halfL*sin(angle) << "," << r+halfL*cos(angle) << "," << z-halfL*sin(angle) << "," << r-halfL*cos(angle) << "); l->SetLineColor(kMagenta+2); l->SetLineWidth(4); l->Draw(\"same\"); // Plain module " << imodule << std::endl;
  }
  fout << "    // ---------------------- Positive mountains module surfaces -------------------------------// " << std::endl;
  nmodules = m_mountainPosModules.size();
  for(unsigned int imodule=0;imodule<nmodules;imodule++) {
    Trk::TrkDetElementBase* module = (Trk::TrkDetElementBase*) m_mountainPosModules.at(imodule);
    const Trk::Surface& sf = module->surface();
    // this version handles only planar bounds
    const Trk::RectangleBounds* recBo = dynamic_cast<const Trk::RectangleBounds*>(&(sf.bounds()));
    if (!recBo) continue;

    // surface center
    double x = sf.center().x();
    double y = sf.center().y();
    double z = sf.center().z();
    double r = sqrt(x*x+y*y);
    double angle = 0.28;
    // it is a rectangle surface
    double halfL = recBo->halflengthY(); // Y is the module length, X its width
    fout << "    l = new TLine(" << z-halfL*sin(angle) << "," << r+halfL*cos(angle) << "," << z+halfL*sin(angle) << "," << r-halfL*cos(angle) << "); l->SetLineColor(kMagenta+2); l->SetLineWidth(4); l->Draw(\"same\"); // Plain module " << imodule << std::endl;
  }
}
