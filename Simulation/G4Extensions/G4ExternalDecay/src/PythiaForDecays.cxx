/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Abused from Pythia6.cc in Geant4 extended examples

#include "G4ExternalDecay/PythiaForDecays.h"
#include "Pythia_i/Pythia.h"
#include "Pythia_i/Pydat3.h"
#include "Pythia_i/Pydat2.h"
#include "Pythia_i/Pydat1.h"
#include "HepMC/IO_HEPEVT.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ThreeVector.hh"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>

struct PPYJETS{
  int   n;      
  int npad;
  int   k[5][4000];
  double p[5][4000];
  double v[5][4000];
};

extern "C" {
  int  pycomp_(int*);
  void py1ent_(int&, int&, double&, double&, double&);
  void pyhepc_(int*);
  PPYJETS pyjets_;
  void pylist_(int*);
  void pyexec_();
}
void pygldc_() {}//ACH for gluino decay
void pysqdc_(int*) {}//ACH for stop/sbottom decay


PythiaForDecays* PythiaForDecays::s_instance = 0;

PythiaForDecays* PythiaForDecays::Instance() 
{
   if ( ! s_instance ) s_instance = new PythiaForDecays();
   return s_instance;
}

PythiaForDecays::PythiaForDecays()  
{
   // Protect against multiple objects.   All access should be via the
   // Instance member function. 
   if ( s_instance ) {
      std::cerr << "There's already an instance of PythiaForDecays" << std::endl;
      exit (1);
   }   
}

PythiaForDecays::~PythiaForDecays()
{;} // It's ok little class, I never owned anything either...

int PythiaForDecays::Pycomp(int kf) 
{
/// Interface with fortran routine pycomp
   return pycomp_(&kf);
}

void PythiaForDecays::Py1ent(int ip, int kf, double pe, double theta, double phi)
{
/// Add one entry to the event record, i.e. either a parton or a
/// particle. 
///
///  IP:   normally line number for the parton/particle. There are two
///        exceptions:
/// 
///        If IP = 0: line number 1 is used and PYEXEC is called. 
///        If IP < 0: line -IP is used, with status code K(-IP,2)=2
///                   rather than 1; thus a parton system may be built
///                   up by filling all but the last parton of the
///                   system with IP < 0.   
///  KF:   parton/particle flavour code (PDG code)
///  PE:   parton/particle energy. If PE is smaller than the mass,
///        the parton/particle is taken to be at rest.  
///  THETA:
///  PHI:  polar and azimuthal angle for the momentum vector of the
///        parton/particle. 
  
  //std::cout<<"ACH start Py1ent 1"<<std::endl;
  py1ent_(ip, kf, pe, theta, phi);
  //int pyl=3; pylist_(&pyl);
}

void PythiaForDecays::DecayRHadron(int pdgid){
  //std::cout<<"ACH start DecayRHadron for pdgid "<<pdgid<<std::endl;
  int pyl=3;//level to list to output text

  //only decay daughter particles from the decay that are "near" the hadron
  Pydat1 pydat1;
  pydat1.mstj(21)=2;
  pydat1.mstj(22)=3;
  pydat1.parj(72)=0.001;

  //Deal with standard hadrons first...
  if (abs(pdgid)<1000000){
    //std::cout<<"ACH decay standard hadron for pdgid "<<pdgid<<std::endl;
    pyjets_.k[0][0]=5;//make it decay right away!
    //pylist_(&pyl);
    pyexec_();
    //pylist_(&pyl);
    return;
  }

  //Real RHadron stuff...
  pdgid=abs(pdgid);
  pyjets_.k[0][0]=6;
  Pydat3 pydat3;
  Pydat2 pydat2;
  //pylist_(&pyl);
  if ( pdgid/10000==109 || pdgid/1000==1009 || pdgid/100==10009 ){
    //std::cout<<"ACH decay gluino rhadron .."<<std::endl;
    int kfgl = 1000021;//gluino
    pydat3.mdcy(pycomp_(&kfgl),1)=1;// MDCY(PYCOMP(KFGL),1)=1 //unstable gluino
    float oldl=pydat2.pmas(pycomp_(&kfgl),4);//remember lifetime
    pydat2.pmas(pycomp_(&kfgl),4)=0;//decay it promptly!
    pygldc_();//gluino DECAY
    pydat2.pmas(pycomp_(&kfgl),4)=oldl;//set lifetime back to what it was
    //pylist_(&pyl);
  }
  else if ( pdgid/1000==1006 || pdgid/100==10006 ){
    //std::cout<<"ACH decay stop rhadron .."<<std::endl;
    Pydat3 pydat3;
    int kfgl = 1000006;//stop
    pydat3.mdcy(pycomp_(&kfgl),1)=1;// MDCY(PYCOMP(KFGL),1)=1 //unstable stop
    float oldl=pydat2.pmas(pycomp_(&kfgl),4);//remember lifetime
    pydat2.pmas(pycomp_(&kfgl),4)=0;//decay it promptly!
    pysqdc_(&kfgl);//stop DECAY!
    pydat2.pmas(pycomp_(&kfgl),4)=oldl;//set lifetime back to what it was
    //pylist_(&pyl);
  }
  else if ( pdgid/1000==1005 || pdgid/100==10005 ){
    //std::cout<<"ACH decay sbottom rhadron .."<<std::endl;
    Pydat3 pydat3;
    int kfgl = 1000005;//sbottom
    pydat3.mdcy(pycomp_(&kfgl),1)=1;// MDCY(PYCOMP(KFGL),1)=1 //unstable sbottom
    float oldl=pydat2.pmas(pycomp_(&kfgl),4);//remember lifetime
    pydat2.pmas(pycomp_(&kfgl),4)=0;//decay it promptly!    
    pysqdc_(&kfgl);//sbottom DECAY!
    pydat2.pmas(pycomp_(&kfgl),4)=oldl;//set lifetime back to what it was
    //pylist_(&pyl);
  }
  else{
    std::cout<<"ACH ERROR !!! Can't decay unknown particle with abspdgid="<<pdgid<<std::endl;
    pylist_(&pyl);
    //exit(-19);
  }

}

//______________________________________________________________________________
void PythiaForDecays::ImportParticles( std::vector<G4DynamicParticle*> & particles)
{
  particles.clear();

  int mconv=1;
  pyhepc_(&mconv);

  // Fill up the HEP Event
  HepMC::GenEvent* evt = new HepMC::GenEvent(1,1);
  HepMC::IO_HEPEVT hepio;
  hepio.set_print_inconsistency_errors(0);
  hepio.fill_next_event(evt);

  for (HepMC::GenEvent::particle_const_iterator p=evt->particles_begin(); p!=evt->particles_end(); ++p){
    if (! (*p) ) continue;

    /*
    double r=0;
    if ((*p)->production_vertex()) {
      double x=(*p)->production_vertex()->position().x();
      double y=(*p)->production_vertex()->position().y();
      r=sqrt(x*x+y*y);
    }
    std::cout<<"ImportParticles: "<<(*p)->pdg_id()<<", vertex r "<<r<<", status "<<(*p)->status()<<std::endl;
    */

    if ( (*p)->status()!=1 ) continue; // stable only
    //if ( std::abs((*p)->pdg_id())==12 || std::abs((*p)->pdg_id())==14 || std::abs((*p)->pdg_id())==16 ) continue; // skip neutrinos
    G4ThreeVector momentum( (*p)->momentum().x() , (*p)->momentum().y() , (*p)->momentum().z() );
    momentum*=1000.0;//GeV to MeV
    const G4ParticleDefinition * particleDefinition = GetParticleDefinition( (*p)->pdg_id() );
    if (!particleDefinition){
      std::cout<<"WARNING: I don't know a definition for pdgid "<<(*p)->pdg_id()<<"! Skipping it..."<<std::endl;
      continue;
    }
    G4DynamicParticle* dynamicParticle = new G4DynamicParticle(particleDefinition, momentum);
    particles.push_back( dynamicParticle );
  }
}

G4ParticleDefinition* PythiaForDecays::GetParticleDefinition(const int pdgEncoding) const
{
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particleDefinition = 0;
  if (pdgEncoding != 0) particleDefinition = particleTable->FindParticle(pdgEncoding);
  return particleDefinition;
}

