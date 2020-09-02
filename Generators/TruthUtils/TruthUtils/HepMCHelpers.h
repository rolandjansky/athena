/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

/// @file
///
/// Provides the HepMC tools from the external MCUtils header package,
/// ATLAS-specific HepMC functions not suitable for MCUtils.

#include "TruthUtils/PIDHelpers.h"
#include "TruthUtils/TruthParticleHelpers.h"
#include "MCUtils/PIDUtils.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/Relatives.h"
namespace MC
{
using namespace MCUtils::PID;
#ifdef HEPMC3
inline bool hasNoChildren(HepMC::ConstGenVertexPtr v) {return v->particles_out().size()==0; }
inline bool hasNoParents(HepMC::ConstGenVertexPtr v) {return v->particles_in().size()==0; }
inline bool isDisconnected(HepMC::ConstGenVertexPtr v) {return (v->particles_in().size()==0)&&(v->particles_out().size()==0); }
#else
inline bool hasNoChildren(HepMC::ConstGenVertexPtr v) {return v->particles_out_size()==0; }
inline bool hasNoParents(HepMC::ConstGenVertexPtr v) {return v->particles_in_size()==0; }
inline bool isDisconnected(HepMC::ConstGenVertexPtr v) {return (v->particles_in_size()==0)&&(v->particles_out_size()==0); }
#endif

inline std::vector<HepMC::ConstGenVertexPtr>   const_vertices_match(const HepMC::GenEvent* e, std::function<bool(HepMC::ConstGenVertexPtr)>  cl)
{
      std::vector<HepMC::ConstGenVertexPtr> ret;
#ifdef HEPMC3
      for (auto v: e->vertices()) if (cl(v)) ret.push_back(v);

#else
      for (auto v=e->vertices_begin();v!=e->vertices_end();++v) if (cl(*v)) ret.push_back(*v);
#endif
      return ret;
}




#ifdef HEPMC3

inline void reduce(HepMC::GenEvent* ge, HepMC::GenParticlePtr gp) { ge->remove_particle(gp);}

#else
//---->//This is copied from MCUtils
  /// @name Event reduction functions
  //@{

  /// Remove an unwanted particle from the event, collapsing the graph structure consistently
  inline void reduce(HepMC::GenEvent* ge, HepMC::GenParticle* gp) {
    // Do nothing if for some reason this particle is not actually in this event
    if (gp->parent_event() != ge) return;

    // Get start and end vertices
    HepMC::GenVertex* vstart = gp->production_vertex();
    HepMC::GenVertex* vend = gp->end_vertex();

    // Disconnect the unwanted particle from its vertices and delete it
    if (vstart != NULL) vstart->remove_particle(gp);
    if (vend != NULL) vend->remove_particle(gp);
    delete gp;

    // If start/end vertices are valid and distinct, and this was the only particle that
    // connected them, then reassign the end vertex decay products to the start vertex
    // and rewrite the vertex position as most appropriate.
    /// @note The disconnected end vertex will be picked up by the final "sweeper" loop if necessary.
    /// @note We do the reassigning this way since GV::add_particle_*() modifies the end vertex
    if (vstart != NULL && vend != NULL && vend != vstart) {
      bool is_only_link = true;
      for (auto pchild=vstart->particles_out_const_begin();pchild!=vstart->particles_out_const_end();++pchild) {
        if ((*pchild)->end_vertex() == vend) is_only_link = false;
      }
      if (is_only_link) {
        if (vend->position() != HepMC::FourVector())
          vstart->set_position(vend->position()); //< @todo Always use end position if defined... ok?
        while (vend->particles_out_size() > 0) {
          vstart->add_particle_out(*vend->particles_out_const_begin());
        }
        while (vend->particles_in_size() > 0) {
          vstart->add_particle_in(*vend->particles_in_const_begin());
        }
      }
    }

    // Sweep up any vertices orphaned by the particle removal
    /// @todo Can we be a bit more efficient rather than having to run over all vertices every time?
    ///       Or allow disabling of this clean-up, with a single clean being run at the end of filtering.
    /// @todo Use neater looping via vertices_match (or iterated vertex_match)
    /// @todo Also look for and report changes in number of no-parent and no-child vertices
    std::vector<HepMC::GenVertex*> orphaned_vtxs;
    for (HepMC::GenEvent::vertex_const_iterator vi = ge->vertices_begin(); vi != ge->vertices_end(); ++vi) {
      if ((*vi)->particles_in_size() == 0 && (*vi)->particles_out_size() == 0) orphaned_vtxs.push_back(*vi);
    }
    for (HepMC::GenVertex* gv : orphaned_vtxs) delete gv;
  }
#endif

  /// Remove unwanted particles from the event, collapsing the graph structure consistently
  inline void reduce(HepMC::GenEvent* ge, std::vector<HepMC::GenParticlePtr> toremove) {
    while (toremove.size()) {
      auto gp = toremove.back();
      toremove.pop_back();
      reduce(ge, gp);
    }
  }

//<----//This is copied from MCUtils

template <class T> inline bool isDecayed(T p)  { return p->status() == 2;}
template <class T> inline bool isStable(T p)   { return p->status() == 1;}
template <class T> inline bool isPhysical(T p) { return isStable<T>(p) || isDecayed<T>(p); }
template <class T> inline bool isPhysicalHadron(T p) { return PID::isHadron(p->pdg_id()) && isPhysical<T>(p);}
template <class T> inline bool fromDecay(T p)  { 
      if (!p) return false;  
      auto v=p->production_vertex();
      if (!v) return false;                                           
#ifdef HEPMC3
      for ( auto anc: v->particles_in()) 
      if (isDecayed(anc) && (PID::isTau(anc->pdg_id()) || PID::isHadron(anc->pdg_id()))) return true;
      for ( auto anc: v->particles_in()) 
      if (fromDecay<T>(anc)) return true;
#else
      for (auto  anc=v->particles_in_const_begin(); anc != v->particles_in_const_end(); ++anc) 
      if (isDecayed((*anc)) && (PID::isTau((*anc)->pdg_id()) || PID::isHadron((*anc)->pdg_id()))) return true;
      for (auto  anc=v->particles_in_const_begin(); anc != v->particles_in_const_end(); ++anc)
      if (fromDecay<T>(*anc)) return true; 
#endif     
      return false;
      }
template <class T>  std::vector<T> findChildren(T p)
      {
      std::vector<T> ret;
      if (!p) return ret;
#ifdef HEPMC3
      ret=HepMC::children_particles(p);
#else
      auto v=p->end_vertex();
      if (p) for (auto pp=v->particles_out_const_begin();pp!=v->particles_out_const_end();++pp) ret.push_back(*pp);
#endif
      if (ret.size()==1) if (ret.at(0)->pdg_id()==p->pdg_id()) ret=findChildren(ret.at(0));
      return ret;
      }
}


namespace MC {

  // Use the MCUtils and HEPUtils functions as if they were defined in the ATLAS MC and MC::PID namespaces
  using namespace MCUtils;
  using namespace HEPUtils;


  /// @name Extra ATLAS-specific particle classifier functions
  //@{

  /// @brief Determine if the particle is stable at the generator (not det-sim) level,
  ///
  /// The receipe for this is barcode < 200k and status = 1. Gen-stable particles decayed by
  /// G4 are not set to have status = 2 in ATLAS, but simply have more status = 1 children,
  /// with barcodes > 200k.
  inline bool isGenStable(HepMC::ConstGenParticlePtr p) {
    return isGenStable(p->status(), HepMC::barcode(p));
  }


  /// @todo There are many kinds of stable: stable from generator, stable at intermediate stage of det sim transport, or stable after all det sim. Need fns for each?


  /// @brief Identify if the particle is considered stable at the post-detector-sim stage
  inline bool isSimStable(HepMC::ConstGenParticlePtr p) {
    if (p->status() != 1) return false;
    if (isGenStable(p)) return p->end_vertex() == NULL;
    return true;
  }

  /// @brief Identify if the particle is considered stable at the post-detector-sim stage
  /// @todo I'm sure this shouldn't be exactly the same as isGenStable, but it is...
  /// @deprecated Use isSimulStable: this function _will_ be removed!
  inline bool isGenSimulStable(HepMC::ConstGenParticlePtr p) {
    return isSimStable(p);
  }


  /// @brief Identify if the particle would not interact with the detector, i.e. not a neutrino or WIMP
  inline bool isNonInteracting(HepMC::ConstGenParticlePtr p) {
    return MC::isNonInteracting(p->pdg_id()); //< From TruthUtils/PIDHelpers.h
  }

  /// @brief Identify if the particle could interact with the detector during the simulation, e.g. not a neutrino or WIMP
  inline bool isSimInteracting(HepMC::ConstGenParticlePtr p) {
    if (! MC::isGenStable(p)) return false; //skip particles which the simulation would not see
    return !MC::isNonInteracting(p);
  }

  /// @brief Oddly-named alias for isSimInteracting
  /// @deprecated Use isSimInteracting: this function _will_ be removed!
  inline bool isGenInteracting(HepMC::ConstGenParticlePtr p) {
    return isSimInteracting(p);
  }

  //@}


}
