// Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_HEPMC_GENEVENTROTATOR_H
#define ISF_HEPMC_GENEVENTROTATOR_H 1

// Base class headers
#include "AthenaBaseComps/AthAlgTool.h"
#include "HepMC_Interfaces/IGenEventManipulator.h"
// Gaudi headers
#include "GaudiKernel/ServiceHandle.h"
// CLHEP headers
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/LorentzRotation.h"

#include "AtlasHepMC/GenParticle.h"

namespace Simulation {

  /** @class GenEventRotator

      This tool takes a HepMC::GenEvent and applies rotations
      See https://twiki.cern.ch/twiki/bin/viewauth/Atlas/AtlasG4EventFilters for more details.
  */
  class GenEventRotator : public extends<AthAlgTool, IGenEventManipulator>
  {

  public:
    /** Constructor with parameters */
    GenEventRotator( const std::string& t, const std::string& n, const IInterface* p );

    /** Athena algtool's Hooks */
    StatusCode initialize() override final;
    StatusCode finalize() override final;

    StatusCode initializeAthenaEvent();// override final;

    /** modifies the given GenEvent */
    StatusCode manipulate(HepMC::GenEvent& ge) const override final;
  private:
    /** calculate the transformations that we want to apply to the particles in the current GenEvent */
    StatusCode initializeGenEvent(CLHEP::HepLorentzRotation& transform) const;
    /** apply rotations to individual GenParticles */
    void rotateParticle(HepMC::GenParticlePtr p, const CLHEP::HepLorentzRotation& transform) const;

    Gaudi::Property<double> m_xangle{this, "xAngle", 0.0, "rotation about x-axis"};
    Gaudi::Property<double> m_yangle{this, "yAngle", 0.0, "rotation about y-axis"};
    Gaudi::Property<double> m_zangle{this, "zAngle", 0.0, "rotation about z-axis"};
  };

}

#endif //> !ISF_HEPMC_GENEVENTROTATOR_H
