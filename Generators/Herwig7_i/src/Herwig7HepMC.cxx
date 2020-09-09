// -*- C++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/*! \file Herwig7HepMC.cxx
 *  \brief Implementation of the Herwig7 HepMC interface.
 *  \author Andrii.verbytskyi@mpp.mpg.de
 */


#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "ThePEG/EventRecord/Event.h"
#include "ThePEG/EventRecord/SubProcess.h"
#include "ThePEG/Handlers/XComb.h"
#include "ThePEG/Handlers/EventHandler.h"
#include "ThePEG/PDF/PDF.h"

#include "Herwig/API/HerwigAPI.h"



#include "ThePEG/Vectors/HepMCConverter.h"
#ifdef HWVER_IS_72
#ifdef HEPMC3
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
namespace ThePEG {
  template<>
  struct HepMCTraits<HepMC3::GenEvent>
    : public HepMCTraitsBase<HepMC3::GenEvent,
                             HepMC3::GenParticle,
                             HepMC3::GenParticlePtr,
                             HepMC3::GenVertex,
                             HepMC3::GenVertexPtr,
                             std::pair<double,double>,
                             HepMC3::GenPdfInfo>
  {
    static bool hasUnits() {
      return true;
    }


  /** Set the \a scale, \f$\alpha_S\f$ (\a aS) and \f$\alpha_{EM}\f$
      (\a aEM) for the event \a e. The scale will be scaled with \a
      unit before given to the GenEvent. */
  static void setScaleAndAlphas(HepMC3::GenEvent & e, Energy2 scale,
				double aS,  double aEM, Energy unit) {
    e.add_attribute("event_scale",std::make_shared<HepMC3::DoubleAttribute>(sqrt(scale)/unit));
    e.add_attribute("alphaQCD",std::make_shared<HepMC3::DoubleAttribute>(aS));
    e.add_attribute("alphaQED",std::make_shared<HepMC3::DoubleAttribute>(aEM));
  }

  /** Set the primary vertex, \a v, for the event \a e. */
  static void setSignalProcessVertex(HepMC3::GenEvent & e, HepMC3::GenVertexPtr v) {
  e.add_attribute("signal_process_vertex",std::make_shared<HepMC3::IntAttribute>(v->id()));
  }

  /** Set the polarization directions, \a the and \a phi, for particle
      \a p. */
  static void setPolarization(HepMC3::GenParticle & genp, double the, double phi) {
    genp.add_attribute("theta",std::make_shared<HepMC3::DoubleAttribute>(the));
    genp.add_attribute("phi",std::make_shared<HepMC3::DoubleAttribute>(phi));
  }

  /** Set the colour line (with index \a indx) to \a coline for
      particle \a p. */
  static void setColourLine(HepMC3::GenParticle & p, int indx, int coline) {
	  p.add_attribute("flow"+std::to_string(indx),std::make_shared<HepMC3::IntAttribute>(coline));
  }
    /** Set the beam particles for the event.*/
  static void setBeamParticles(HepMC3::GenEvent & e, HepMC3::GenParticlePtr p1, HepMC3::GenParticlePtr p2) {
    p1->set_status(4);
    p2->set_status(4);
    e.set_beam_particles(p1,p2);
  }
  
    static HepMC3::GenParticlePtr newParticle(const Lorentz5Momentum & p,
				 long id, int status, Energy unit) {
    // Note that according to the documentation the momentum is stored in a
    // HepLorentzVector in GeV (event though the CLHEP standard is MeV).
    LorentzVector<double> p_scalar = p/unit;
   HepMC3::GenParticlePtr genp = std::make_shared<HepMC3::GenParticle>(HepMC::FourVector(p_scalar.x(),p_scalar.y(),p_scalar.z(),p_scalar.e()), id, status);
   genp->set_generated_mass(p.mass()/unit);
    return genp;
  }
    /** Set the position \a p for the vertex, \a v. The length will be
      scaled with \a unit which normally should be millimeters. */
  static void setPosition( HepMC3::GenVertex & v, const LorentzPoint & p, Length unit) {
    LorentzVector<double> p_scaled = p/unit;
    v.set_position(HepMC::FourVector(p_scaled.x(),p_scaled.y(),p_scaled.z(),p_scaled.t()));
  }
  
    /** Create a new vertex. */
  static HepMC3::GenVertexPtr newVertex() {
    return  std::make_shared<HepMC3::GenVertex>();
  }
    };
}
#else
namespace ThePEG {
  template<>
  struct HepMCTraits<HepMC::GenEvent>
    : public HepMCTraitsBase<HepMC::GenEvent,
                             HepMC::GenParticle,
                             HepMC::GenParticle *,
                             HepMC::GenVertex,
                             HepMC::GenVertex *,
                             HepMC::Polarization,
                             HepMC::PdfInfo>
  {
    static bool hasUnits() {
      return true;
    }
  };
}
#endif
#else
namespace ThePEG {
  template<>
  struct HepMCTraits<HepMC::GenEvent>
    : public HepMCTraitsBase<HepMC::GenEvent,
                             HepMC::GenParticle,
                             HepMC::GenVertex,
                             HepMC::Polarization,
                             HepMC::PdfInfo>
  {
    static bool hasUnits() {
      return true;
    }
  };
}
#endif
  
void   convert_to_HepMC(const ThePEG::Event & event, HepMC::GenEvent & evt, bool nocopies,ThePEG::Energy eunit, ThePEG::Length lunit){
  ThePEG::HepMCConverter<HepMC::GenEvent>::convert(event, evt, nocopies, eunit, lunit);
}
