// Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_HEPMC_GENEVENTBEAMEFFECTBOOSTER_H
#define ISF_HEPMC_GENEVENTBEAMEFFECTBOOSTER_H 1

// Base class headers
#include "AthenaBaseComps/AthAlgTool.h"
#include "HepMC_Interfaces/IGenEventManipulator.h"
// Athena headers
#include "AthenaKernel/IAthRNGSvc.h"
// Gaudi headers
#include "GaudiKernel/ServiceHandle.h"
// CLHEP headers
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/LorentzRotation.h"

namespace ATHRNG {
  class RNGWrapper;
}

#include "AtlasHepMC/GenParticle_fwd.h"

namespace Simulation {

  /** @class GenEventBeamEffectBooster

      This tool takes a HepMC::GenEvent and applies boosts due to beam
      tilt, etc.

      based on:
      https://svnweb.cern.ch/trac/atlasoff/browser/Simulation/G4Atlas/G4AtlasUtilities/trunk/src/BeamEffectTransformation.cxx
  */
  class GenEventBeamEffectBooster : public extends<AthAlgTool, IGenEventManipulator>
  {

  public:
    /** Constructor with parameters */
    GenEventBeamEffectBooster( const std::string& t, const std::string& n, const IInterface* p );

    /** Athena algtool's Hooks */
    StatusCode initialize() override final;
    StatusCode finalize() override final;

    StatusCode initializeAthenaEvent();// override final;

    /** modifies the given GenEvent */
    StatusCode manipulate(HepMC::GenEvent& ge) const override final;
  private:
    /** calculate the transformations that we want to apply to the particles in the current GenEvent */
    StatusCode initializeGenEvent(CLHEP::HepLorentzRotation& transform) const;
    /** apply boost to individual GenParticles */
    void boostParticle(HepMC::GenParticlePtr p, const CLHEP::HepLorentzRotation& transform) const;

    ServiceHandle<IAthRNGSvc>       m_rndGenSvc{this, "RandomSvc", "AthRNGSvc"};
    ATHRNG::RNGWrapper*             m_randomEngine{};             //!< Slot-local RNG
    Gaudi::Property<std::string>     m_randomEngineName{this, "RandomStream", "BEAM"};         //!< Name of the random number stream
    Gaudi::Property<bool>              m_applyBoost{this, "ApplyBoost", true};
    Gaudi::Property<bool>              m_applyDivergence{this, "ApplyDivergence", true};

    //BACKUP properties needed until IBeamCondSvc is updated
    Gaudi::Property<double> m_sigma_px_b1{this, "Sigma_px_b1", 20.e-6, "angular divergence in x of beam 1 [rad]"};
    Gaudi::Property<double> m_sigma_px_b2{this, "Sigma_px_b2", 21.e-6, "angular divergence in x of beam 2 [rad]"};
    Gaudi::Property<double> m_sigma_py_b1{this, "Sigma_py_b1", 22.e-6, "angular divergence in y of beam 1 [rad]"};
    Gaudi::Property<double> m_sigma_py_b2{this, "Sigma_py_b2", 23.e-6, "angular divergence in y of beam 2 [rad]"};
    // Crossing angles, note the convention here is taken from online https://atlas-dcs.cern.ch/index.php?page=LHC_INS::INS_BPM
    Gaudi::Property<double> m_xing_x_b1{this, "HalfXing_x_b1", 19.e-6, "half-crossing in x for beam 1 at IP1, i.e. angle between beam 1 and z-axis"};
    Gaudi::Property<double> m_xing_x_b2{this, "HalfXing_x_b2", 1.e-6, "half-crossing in x for beam 2 at IP1, i.e. angle between beam 2 and z-axis"};
    Gaudi::Property<double> m_xing_y_b1{this, "HalfXing_y_b1", 150.e-6, "half-crossing in y for beam 1 at IP1, i.e. angle between beam 1 and z-axis"};
    Gaudi::Property<double> m_xing_y_b2{this, "HalfXing_y_b2", -152.e-6, "half-crossing in y for beam 2 at IP1, i.e. angle between beam 2 and z-axis"};
    Gaudi::Property<double> m_dE{this,  "deltaEOverE", 1.1E-4, "Delta_E/E theoretical value"}; // Delta_E/E theoretical value
    Gaudi::Property<double> m_pbeam1{this, "pbeam1",        3500.0E3,    "Beam 1 Momentum / MeV"}; /// @todo Get from a service
    Gaudi::Property<double> m_pbeam2{this, "pbeam2",        3500.0E3,    "Beam 2 Momentum / MeV"}; /// @todo Get from a service
    Gaudi::Property<double> m_beam1ParticleMass{this, "Beam1ParticleMass", CLHEP::proton_mass_c2, "Mass of particle used in beam 1"};
    Gaudi::Property<double> m_beam2ParticleMass{this, "Beam2ParticleMass", CLHEP::proton_mass_c2, "Mass of particle used in beam 2"};
  };

}

#endif //> !ISF_HEPMC_GENEVENTBEAMEFFECTBOOSTER_H
