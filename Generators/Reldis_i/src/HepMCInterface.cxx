/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Reldis_i/HepMCInterface.h"

#include "GaudiKernel/IPartPropSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

namespace HepMC {

  void I_Reldis::fill_next_event(ReldisTree& tree, GenEvent* evt, int ievtnum) {
  
    if (!evt) { std::cerr << "I_Reldis::fill_next_event, ERROR - passed null event." << std::endl; return; }
  
    evt->set_event_number(ievtnum);
      
    evt->weights().push_back(1.); // Set event weight to 1 (events are physically distributed)
    
    GenVertex* prod_vtx = new GenVertex();
    
    prod_vtx->set_position(FourVector()); // Create a HepMC vertex at (0, 0, 0) with all particles
    
    // See suggestion of Andy on how to set the statusCode:
    // https://savannah.cern.ch/task/?func=detailitem&item_id=25015#comment29
    addParticles(prod_vtx, tree.Nleft,     tree.Ipdg_l_n,  tree.Pxl,    tree.Pyl,    tree.Pzl,    1); // Add neutrons left 
    addParticles(prod_vtx, tree.Nright,    tree.Ipdg_r_n,  tree.Pxr,    tree.Pyr,    tree.Pzr,    1); // Add neutrons right 
    addParticles(prod_vtx, tree.Nleft_p,   tree.Ipdg_l_p,  tree.Pxl_p,  tree.Pyl_p,  tree.Pzl_p,  1); // Add protons  left 
    addParticles(prod_vtx, tree.Nright_p,  tree.Ipdg_r_p,  tree.Pxr_p,  tree.Pyr_p,  tree.Pzr_p,  1); // Add protons  right 
    addParticles(prod_vtx, tree.Nleft_pp,  tree.Ipdg_l_pp, tree.Pxl_pp, tree.Pyl_pp, tree.Pzl_pp, 2); // Add pi plus  left 
    addParticles(prod_vtx, tree.Nright_pp, tree.Ipdg_r_pp, tree.Pxr_pp, tree.Pyr_pp, tree.Pzr_pp, 2); // Add pi plus  right 
    addParticles(prod_vtx, tree.Nleft_pm,  tree.Ipdg_l_pm, tree.Pxl_pm, tree.Pyl_pm, tree.Pzl_pm, 2); // Add pi minus left 
    addParticles(prod_vtx, tree.Nright_pm, tree.Ipdg_r_pm, tree.Pxr_pm, tree.Pyr_pm, tree.Pzr_pm, 2); // Add pi minus right 
    addParticles(prod_vtx, tree.Nleft_p0,  tree.Ipdg_l_p0, tree.Pxl_p0, tree.Pyl_p0, tree.Pzl_p0, 2); // Add pi zero  left 
    addParticles(prod_vtx, tree.Nright_p0, tree.Ipdg_r_p0, tree.Pxr_p0, tree.Pyr_p0, tree.Pzr_p0, 2); // Add pi zero  right 
    addParticles(prod_vtx, tree.Nleft_et,  tree.Ipdg_l_et, tree.Pxl_et, tree.Pyl_et, tree.Pzl_et, 2); // Add eta      left 
    addParticles(prod_vtx, tree.Nright_et, tree.Ipdg_r_et, tree.Pxr_et, tree.Pyr_et, tree.Pzr_et, 2); // Add eta      right 
    addParticles(prod_vtx, tree.Nleft_om,  tree.Ipdg_l_om, tree.Pxl_om, tree.Pyl_om, tree.Pzl_om, 2); // Add omega    left 
    addParticles(prod_vtx, tree.Nright_om, tree.Ipdg_r_om, tree.Pxr_om, tree.Pyr_om, tree.Pzr_om, 2); // Add omega    right 

    evt->add_vertex(prod_vtx);
  }

  void I_Reldis::addParticles(GenVertex* prod_vtx, 
			      Int_t nParticles, 
			      Int_t   pdg[ReldisTree::MAX_NUM_PART], 
			      Float_t px [ReldisTree::MAX_NUM_PART], 
			      Float_t py [ReldisTree::MAX_NUM_PART], 
			      Float_t pz [ReldisTree::MAX_NUM_PART],
			      Int_t   statusCode) {

    for (int i=0; i<nParticles; i++) {
      
      ThreeVector mom(px[i]*1000, py[i]*1000, pz[i]*1000);
      
      int    pid  = pdg[i];
      double mass = getParticleMass(pid);

      GenParticle* aParticle = new GenParticle(FourVector(mom.x(), mom.y(), mom.z(), sqrt(mom.r()*mom.r() + mass*mass)), 
					       pid, statusCode, Flow(), Polarization(0, 0));
      
      prod_vtx->add_particle_out(aParticle);
    }
  }

  double I_Reldis::getParticleMass(int pid) {
    
    if (m_particleTable == 0) {

      std::cout << " Inizializing m_particleTable " << std::endl;

      IPartPropSvc* partPropSvc = 0;
      StatusCode PartPropStatus = Gaudi::svcLocator()->service("PartPropSvc", partPropSvc);
      
      if (!PartPropStatus.isSuccess() || 0 == partPropSvc)
	throw GaudiException("PartPropSvc error", "I_Reldis", StatusCode::FAILURE);      
      
      m_particleTable = partPropSvc->PDT();
    }
    
    const HepPDT::ParticleData* particle = m_particleTable->particle(HepPDT::ParticleID(abs(pid)));
    
    return particle->mass().value();
  }  
}
