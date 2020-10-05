/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/* Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de */

#ifndef ATLASHEPMC_FLOW_H
#define ATLASHEPMC_FLOW_H
#ifdef HEPMC3
#include "HepMC3/Attribute.h"
#include "HepMC3/GenParticle.h"
namespace HepMC
{
typedef std::shared_ptr<HepMC3::VectorIntAttribute> Flow;
inline int flow(HepMC3::GenParticlePtr p, int i){
std::shared_ptr<HepMC3::IntAttribute> f=p->attribute<HepMC3::IntAttribute>("flow"+std::to_string(i));  if (f) return f->value(); 
std::shared_ptr<HepMC3::VectorIntAttribute> vf=p->attribute<HepMC3::VectorIntAttribute>("flow");  if (vf) if (0<i&&i<(int)(vf->value().size())) return vf->value().at(i); 
return 0;
}
inline int flow(HepMC3::ConstGenParticlePtr p, int i){
std::shared_ptr<HepMC3::IntAttribute> f=p->attribute<HepMC3::IntAttribute>("flow"+std::to_string(i));  if (f) return f->value(); 
std::shared_ptr<HepMC3::VectorIntAttribute> vf=p->attribute<HepMC3::VectorIntAttribute>("flow");  if (vf) if (0<i&&i<(int)(vf->value().size())) return vf->value().at(i); 
return 0;
}
inline Flow flow(HepMC3::GenParticlePtr p){
std::shared_ptr<HepMC3::VectorIntAttribute> vf=p->attribute<HepMC3::VectorIntAttribute>("flow");  
if (vf)  return vf;

std::vector<int> fl;
for (int i=1;i<=10;i++)
{
std::shared_ptr<HepMC3::IntAttribute> f=p->attribute<HepMC3::IntAttribute>("flow"+std::to_string(i));  if (f) fl.push_back(i); else break;
}
return std::make_shared<HepMC3::VectorIntAttribute>(fl);
}
inline Flow flow(HepMC3::ConstGenParticlePtr p){
std::shared_ptr<HepMC3::VectorIntAttribute> vf=p->attribute<HepMC3::VectorIntAttribute>("flow");  
if (vf)  return vf;

std::vector<int> fl;
for (int i=1;i<=10;i++)
{
std::shared_ptr<HepMC3::IntAttribute> f=p->attribute<HepMC3::IntAttribute>("flow"+std::to_string(i));  if (f) fl.push_back(i); else break;
}
return std::make_shared<HepMC3::VectorIntAttribute>(fl);
}
}
#else
#include "HepMC/Flow.h"
namespace HepMC
{
template <class T> int flow(T a,int i){return a->flow(i);}
template <class T> Flow flow(T a){return a->flow();}
}
#endif
#endif
