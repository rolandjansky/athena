// Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_HEPMC_TOOLS_GENEVENTBEAMEFFECTBOOSTER_H
#define ISF_HEPMC_TOOLS_GENEVENTBEAMEFFECTBOOSTER_H 1

// Base class headers
#include "AthenaBaseComps/AthAlgTool.h"
#include "ISF_HepMC_Interfaces/IGenEventManipulator.h"
// Athena headers
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
// Gaudi headers
#include "GaudiKernel/ServiceHandle.h"
// CLHEP headers
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/LorentzRotation.h"

namespace CLHEP {
  class HepRandomEngine;
}

namespace HepMC {
  class GenParticle;
}

namespace ISF {

  /** @class GenEventBeamEffectBooster

      This tool takes a HepMC::GenEvent and applies boosts due to beam
      tilt, etc.

      based on:
      https://svnweb.cern.ch/trac/atlasoff/browser/Simulation/G4Atlas/G4AtlasUtilities/trunk/src/BeamEffectTransformation.cxx
  */
  class GenEventBeamEffectBooster : public AthAlgTool, virtual public IGenEventManipulator
  {

  public:
    /** Constructor with parameters */
    GenEventBeamEffectBooster( const std::string& t, const std::string& n, const IInterface* p );

    /** Athena algtool's Hooks */
    StatusCode initialize() override final;
    StatusCode finalize() override final;

    StatusCode initializeAthenaEvent();// override final;

    /** modifies the given GenEvent */
    StatusCode manipulate(HepMC::GenEvent& ge) override final;
  private:
    /** calculate the transformations that we want to apply to the particles in the current GenEvent */
    StatusCode initializeGenEvent();
    /** apply boost to individual GenParticles */
    void boostParticle(HepMC::GenParticle* p);

    ServiceHandle<IBeamCondSvc>     m_beamCondSvc;
    ServiceHandle<IAtRndmGenSvc>    m_rndGenSvc;
    CLHEP::HepRandomEngine*         m_randomEngine;
    std::string                     m_randomEngineName;         //!< Name of the random number stream
    bool                            m_applyBoost;
    bool                            m_applyDivergence;

    CLHEP::HepLorentzRotation       m_trf;
    //BACKUP properties needed until IBeamCondSvc is updated
    double m_sigma_px_b1; // angular divergence in x of beam 1 [rad]
    double m_sigma_px_b2; // angular divergence in x of beam 2 [rad]
    double m_sigma_py_b1; // angular divergence in y of beam 1 [rad]
    double m_sigma_py_b2; // angular divergence in y of beam 2 [rad]
    // Crossing angles, note the convention here is taken from online https://atlas-dcs.cern.ch/index.php?page=LHC_INS::INS_BPM
    double m_xing_x_b1;   // half-crossing in x for beam 1 at IP1, i.e. angle between beam 1 and z-axis
    double m_xing_x_b2;    // half-crossing in x for beam 2 at IP1, i.e. angle between beam 2 and z-axis
    double m_xing_y_b1;  // half-crossing in y for beam 1 at IP1, i.e. angle between beam 1 and z-axis
    double m_xing_y_b2; // half-crossing in y for beam 2 at IP1, i.e. angle between beam 2 and z-axis
    double m_dE; // Delta_E/E theoretical value
    double m_pbeam1; /// @todo Get from a service
    double m_pbeam2; /// @todo Get from a service
  };

}

#endif //> !ISF_HEPMC_TOOLS_GENEVENTBEAMEFFECTBOOSTER_H
