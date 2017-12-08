/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsKinematics/HepMCEventConverter.h"
#include "FadsKinematics/VertexManipulator.h"
#include "FadsKinematics/ParticleManipulator.h"
#include "FadsKinematics/GeneratorCenter.h"

#include "G4Event.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4LorentzVector.hh"
#include "G4Geantino.hh"

namespace FADS {

void HepMCEventConverter::HepMC2G4(const HepMC::GenEvent* evt,G4Event * anEvent, const bool) const
{
  //	std::cout<<" New HepMC event being passed to G4 "<<std::endl;
  int ntrack=0;

  GeneratorCenter *gc=GeneratorCenter::GetGeneratorCenter();

  vtxModifiers::const_iterator vit;
  for (vit=gc->BeginVertexManipulator();vit!=gc->EndVertexManipulator();vit++)
  	(*vit).second->EventInitialization();
	
  particleModifiers::const_iterator pit;
  for (pit=gc->BeginParticleManipulator();pit!=gc->EndParticleManipulator();pit++)
  	(*pit).second->EventInitialization();

  for ( HepMC::GenEvent::vertex_const_iterator v = evt->vertices_begin();
 	v != evt->vertices_end(); ++v ) {
    bool IsGood=false;
    for (HepMC::GenVertex::particle_iterator it=
	   (*v)->particles_begin(HepMC::children);
	 it!=(*v)->particles_end(HepMC::children);
	 ++it) {
      if (!(*it)->end_vertex() && (*it)->status()==1) {
	IsGood=true;
	break;
      }
    }
    if (IsGood) {
      //std::cout<<" new vertex stored"<<(*v)->position()<<std::endl;
      G4LorentzVector lv((*v)->position().x(),
                         (*v)->position().y(),
                         (*v)->position().z(),
                         (*v)->position().t());
      G4PrimaryVertex *vert= new G4PrimaryVertex (lv.x()*CLHEP::mm, 
						  lv.y()*CLHEP::mm, 
						  lv.z()*CLHEP::mm, 
						  lv.t()*CLHEP::mm/CLHEP::c_light);
      vtxModifiers::const_iterator it;
      bool stillGood=true;
      for (it=gc->BeginVertexManipulator();it!=gc->EndVertexManipulator();++it) {
	stillGood=stillGood&&(*it).second->EditVertex(vert);
	if (!stillGood) break;
      }
      if (!stillGood)
	delete vert;
      else {
	int nadded=0;
	for (HepMC::GenVertex::particle_iterator 
	       it= (*v)->particles_begin(HepMC::children);
	     it!=(*v)->particles_end(HepMC::children);
	     ++it) {
	  if((*it)->status()!=1) continue;
	  //if((*it)->status()!=1) {
	  //  std::cout<< "..." << (*it)->pdg_id()<< std::endl;
	  //}
	  ntrack++;
	  int pdgcode=(*it)->pdg_id();
	  CLHEP::HepLorentzVector p((*it)->momentum().px(),
                             (*it)->momentum().py(),
                             (*it)->momentum().pz(),
			     (*it)->momentum().e());
	  //std::cout<<"\t"<<ntrack<<
	  //" New particle "<<(*it)->pdg_id()<<
	  //" "<<(*it)->momentum()<<std::endl;
	  G4PrimaryParticle *part;
	  if (pdgcode!=999) 
	    part= new G4PrimaryParticle(pdgcode, 
					p.x()*CLHEP::GeV,
					p.y()*CLHEP::GeV,
					p.z()*CLHEP::GeV);
	  else {
	    G4ParticleDefinition *pdef=
	      G4Geantino::GeantinoDefinition();
	    part=new G4PrimaryParticle(pdef,p.x()*CLHEP::GeV,p.y()*CLHEP::GeV,p.z()*CLHEP::GeV);
	  }
	  bool pGood=true;
	  particleModifiers::const_iterator jt;
	  
	  for (jt=gc->BeginParticleManipulator();jt!=gc->EndParticleManipulator();
	       ++jt) {
	    pGood=pGood&&(*jt).second->EditParticle(part);
	    if (!pGood) break;
	  }
	  if (pGood) { 
	    nadded++;
	    vert->SetPrimary(part);
	    // push into primary map
// 	    g4primary2genMap.insert(std::make_pair(part, 
// 	      MCTGenParticle(const_cast<HepMC::GenEvent*>(evt),(*it))));
	  } else 
	    delete part;
	} // ... end of "parrticle" for-loop
	if (nadded) 
	  anEvent->AddPrimaryVertex(vert);
	else 
	  delete vert;
      }
    }
  }
}

}	// end namespace
